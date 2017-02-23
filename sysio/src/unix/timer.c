/**
 * @file
 * @brief Timer de temporisation
 * Copyright © 2017 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sysio/timer.h>
#include <sysio/dlist.h>

#define MAX_EVENTS 20

/* structures =============================================================== */
struct xTimer {
  int fd;
  long interval;
  iTimerCallback callback;
  void * udata;
  union {
    uint8_t flags;
    struct {
      uint8_t is_singleshot: 1;
      uint8_t is_active: 1;
    };
  };
};

struct xTimerContext {
  int epfd;
  pthread_t thread;
//  pthread_mutex_t read_mutex;
  bool run;
};

struct xTimerContext ctx;

/* private functions ======================================================== */

// -----------------------------------------------------------------------------
// Thread de surveillance des entrées du port
static void *
pvThread (void * unused) {
  int ret;
  struct epoll_event ev[MAX_EVENTS];
  xTimer * tm;

  while (ctx.run) {

    ret = epoll_wait (ctx.epfd, ev, MAX_EVENTS, -1);

    for (int i = 0; i < ret; i++) {

      tm = (xTimer *) ev[i].data.ptr;

      if (tm) {
        uint64_t res;

        ret = read (tm->fd, &res, sizeof (res));

        if ( (ret == sizeof (res)) && (res > 0)) {

          for (int j = 0; j < res; j++) {
            if (tm->callback) {

              tm->callback (tm);
            }
          }
        }
      }
    }
  }
  return NULL;
}

// -----------------------------------------------------------------------------
static int
iCreateThread (void) {
  int ret = 0;

  if (ctx.epfd == 0) {

    ret = epoll_create (1);
    if (ret >= 0) {

      ctx.epfd = ret;
      ctx.run = true;
      ret = pthread_create (&ctx.thread, NULL, pvThread, NULL);
    }
  }
  return ret;
}

/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
int
iTimerSetCallback (xTimer * tm, iTimerCallback callback, void * udata) {
  int ret;

  ret = iCreateThread();
  if (ret == 0) {

    ret =  epoll_ctl (ctx.epfd, EPOLL_CTL_DEL, tm->fd, NULL);
    if ( (ret == -1) && (errno == ENOENT)) {
      struct epoll_event ev;

      tm->callback = callback;
      tm->udata = udata;
      ev.events = EPOLLIN;
      ev.data.ptr = (void *) tm;
      ret =  epoll_ctl (ctx.epfd, EPOLL_CTL_ADD, tm->fd, &ev);
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
int
iTimerUnsetCallback (xTimer * tm) {
  int ret;

  if (ctx.epfd) {
    ret = epoll_ctl (ctx.epfd, EPOLL_CTL_DEL, tm->fd, NULL);
    if (ret == 0) {
      tm->callback = NULL;
      tm->udata = NULL;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
void *
pvTimerUserData (xTimer * tm) {

  return tm->udata;
}

// -----------------------------------------------------------------------------
xTimer *
xTimerNew (void) {
  int fd = timerfd_create (CLOCK_MONOTONIC, 0);

  if (fd != -1) {

    xTimer * tm = calloc (1, sizeof (xTimer)); // remplit de zeros
    tm->fd = fd;
    tm->is_singleshot = 1;
    return tm;
  }
  return NULL;
}

// -----------------------------------------------------------------------------
int
iTimerDelete (xTimer * tm) {
  int ret;

  (void) iTimerStop (tm);
  ret = close (tm->fd);
  if (ret == 0) {

    free (tm);
  }
  return ret;
}

// -----------------------------------------------------------------------------
bool
bTimerIsActive (const xTimer * tm) {

  return tm->is_active;
}

// -----------------------------------------------------------------------------
void
vTimerSetInterval (xTimer * tm, long ms) {

  tm->interval = ms;
}

// -----------------------------------------------------------------------------
long
lTimerInterval (const xTimer * tm) {

  return tm->interval;
}

// -----------------------------------------------------------------------------
bool
bTimerIsSingleShot (const xTimer * tm) {

  return tm->is_singleshot;
}

// -----------------------------------------------------------------------------
void
vTimerSetSingleShot (xTimer * tm, bool singleShot) {

  tm->is_singleshot = singleShot;
}

// -----------------------------------------------------------------------------
int
iTimerStop (xTimer * tm) {
  int ret = 0;

  if (tm->is_active) {
    struct itimerspec ts;

    memset (&ts, 0, sizeof (ts));
    ret = timerfd_settime (tm->fd, 0, &ts, NULL);
    tm->is_active = (ret == 0);
  }

  return ret;
}

// -----------------------------------------------------------------------------
int
iTimerStart (xTimer * tm) {
  int ret;

  ret = iTimerStop (tm);
  if (ret == 0) {
    struct itimerspec ts;
    time_t sec = tm->interval / 1000;
    long nsec = (tm->interval % 1000) * 1000000;

    if (tm->is_singleshot) {

      ts.it_interval.tv_sec = 0;
      ts.it_interval.tv_nsec = 0;
    }
    else {

      ts.it_interval.tv_sec = sec;
      ts.it_interval.tv_nsec = nsec;
    }

    ts.it_value.tv_sec = sec;
    ts.it_value.tv_nsec = nsec;

    ret =  timerfd_settime (tm->fd, 0, &ts, NULL);
    tm->is_active = (ret == 0);
  }
  return ret;
}


// -----------------------------------------------------------------------------
/*
  timerfd_gettime() renvoie dans curr_value une structure itimerspec qui contient
  la configuration actuelle de la temporisation référencée par le descripteur de
  fichier fd.

  Le champ it_value renvoie le temps restant jusqu'à la prochaine expiration de la
  temporisation. Si les deux champs de cette structure sont nuls, la temporisation
  est actuellement désarmée. Ce champ contient toujours une valeur relative,
  que l'attribut TFD_TIMER_ABSTIME ait été spécifié ou non lors de la
  configuration de la temporisation.

  Le champ it_interval renvoie la période de la temporisation. Si les deux champs
  de cette structure sont nuls, la temporisation a été configurée pour n'expirer
  qu'une seule fois, à la date spécifiée par curr_value.it_value.
*/
long
lTimerRemainingTime (const xTimer * tm) {
  long ret = -1;

  if (tm->is_active) {
    struct itimerspec ts;

    ret = timerfd_gettime (tm->fd, &ts);
    if (ret == 0) {

      ret = ts.it_value.tv_sec * 1000 + ts.it_value.tv_nsec / 1000000;
    }
  }
  return ret;
}

// -----------------------------------------------------------------------------
bool
bTimerIsTimeout (xTimer * tm) {

  return lTimerRemainingTime (tm) == 0;
}

// -----------------------------------------------------------------------------
int
iTimerWait (xTimer * tm) {
  int ret;
  uint64_t res;

  ret = read (tm->fd, &res, sizeof (res));
  if (ret == sizeof (res)) {
    ret = (int) res;
  }
  return ret;
}


/* ========================================================================== */
