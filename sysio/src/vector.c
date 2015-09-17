/**
 * @file
 * @brief
 *
 * Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>
 * All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysio/vector.h>

/* macros =================================================================== */
/* constants ================================================================ */
/* structures =============================================================== */
/* types ==================================================================== */
/* private variables ======================================================== */
/* private functions ======================================================== */
/* public variables ========================================================= */
/* internal public functions ================================================ */

// -----------------------------------------------------------------------------
/*
 * @brief Initialise un tableau dynamique
 *
 * @param vector pointeur sur le vecteur
 * @param fdestroy fonction chargée de libébrer la mémoire allouée aux données,
 *        NULL si les données sont statiques
 * @return 0, -1 si erreur
 */
int
iVectorInit (xVector * v, vVectorElmtNew fnew, vVectorElmtDestroy fdestroy) {

  if (v)  {

    memset (v, 0, sizeof (xVector));
    v->fdestroy = fdestroy;
    v->fnew = fnew;
    return 0;
  }
  return -1;
}

/*
 * @brief Initialise les fonctions de recherche
 *
 * @param vector pointeur sur le vecteur
 * @param fkey fonction de clé
 * @param fmatch fonction de comparaison
 */
void
vVectorInitSearch (xVector * v, pvVectorElmtKey fkey,
                   iVectorElmtMatch fmatch) {

  v->fkey = fkey;
  v->fmatch = fmatch;
}

/*
 * @brief Modifie la taille
 *
 * Si la nouvelle taille est inférieure à l'ancienne les éléments en excés sont
 * retirés avec iVectorRemove()
 *
 * @param vector pointeur sur le vecteur
 * @param new_size nouvelle taille
 * @return 0, -1 si erreur
 */
int
iVectorResize (xVector * v, int new_size) {

  if (new_size == v->size) {

    return 0;
  }

  if (new_size >= 0)  {
    void * p;

    // Retrait des éléments en excès
    if (v->fdestroy)  {
      for (int i = v->size; i >= new_size; i--) {

        if (v->data[i]) {

          v->fdestroy (v->data[i]);
        }
      }
    }

    p = realloc (v->data, sizeof (void*) * new_size);
    if (p) {

      v->data = p;
      if (new_size > v->size) {
        // Init des nouveaux éléments
        for (int i = v->size; i < new_size; i++) {
          if (v->fnew) {
            v->data[i] = v->fnew();
          }
          else {
            v->data[i] = NULL;
          }
        }
      }
      v->size = new_size;
      return 0;
    }
  }
  return -1;
}

// -----------------------------------------------------------------------------
/*
 * @brief Retire un élément
 *
 * La mémoire allouée à l'élément est libérée si une fonction destroy() a été
 * fournie à l'initialisation.
 *
 * @param vector pointeur sur le vecteur
 * @param index index de l'élément concerné
 * @return 0, -1 si erreur
 */
int
iVectorRemove (xVector *v, int index) {

  if ( (index < v->size) && (index >= 0)) {

    if ( (v->fdestroy) && (v->data[index])) {

      v->fdestroy (v->data[index]);
    }
    memcpy (&v->data[index], &v->data[index + 1], sizeof (void *) * (v->size - index - 1));
    return iVectorResize (v, v->size - 1);
  }
  return -1;
}


// -----------------------------------------------------------------------------
/*
 * @brief Destruction d'un tableau dynamique
 *
 * La mémoire allouée aux données de chaque élément est libérée si une fonction
 * destroy() a été fournie à l'initialisation.
 * @param vector pointeur sur le vecteur
 */
void
vVectorDestroy (xVector * v) {

  if (v->fdestroy) {

    for (int i = 0; i < v->size; i--) {

      if (v->data[i]) {

        v->fdestroy (v->data[i]);
      }
    }
  }
  free (v->data);
}

// -----------------------------------------------------------------------------
/*
 * @brief Affectation d'un élément à une case
 *
 * Si il y a déjà un élément dans le tableau, la mémoire allouée à celui-ci est
 * libérée si une fonction destroy() a été fournie à l'initialisation. \n
 * Si index est plus grand que la taille - 1, iVectorResize est appelé avant
 * afin de pouvoir stocker l'élément.
 *
 * @param vector pointeur sur le vecteur
 * @param index index de l'élément concerné
 * @param data à affecter au nouvel élément
 * @return 0, -1 si index est supérieur à iVectorCount()
 */
int
iVectorReplace (xVector * v, int index, void * e) {

  if ( (index < 0) || (index > v->size)) {

    return -1;
  }
  if (index == v->size) {

    if (iVectorResize (v, index + 1) != 0) {

      return -1;
    }
  }

  if ( (v->fdestroy) && (v->data[index])) {

    v->fdestroy (v->data[index]);
  }
  v->data[index] = e;
  return 0;
}

// -----------------------------------------------------------------------------
/*
 * @brief Pointeur sur l'élément
 * @param index index de l'élément concerné
 * @return le pointeur sur l'élément ou NULL si index hors plage
 */
void *
pvVectorGet (const xVector *v, int index) {

  if ( (index >= v->size) || (index < 0)) {
    return NULL;
  }

  return v->data[index];
}

// -----------------------------------------------------------------------------
/*
 * @brief Ajout en fin de tableau
 *
 * Si la taille du tableau est importante, cette fonction peut-être lente
 * car chaque élément est recopié dans une nouvelle zone mémoire. On préférera
 * dans ce cas utiliser une ve.
 *
 * @param vector pointeur sur le vecteur
 * @param data élément à affecter à la case
 * @return 0, -1 si erreur
 */
int
iVectorAppend (xVector *v, void *e) {

  return iVectorReplace (v, v->size, e);;
}

// -----------------------------------------------------------------------------
void *
pvVectorFindFirst (const xVector *v, const void * key) {

  if ( (v->fkey) && (v->fmatch)) {

    for (int i = 0; i < iVectorSize (v); i++) {
      
      if (v->fmatch (key, v->fkey (v->data[i])) == 0) {
        
        return v->data[i];
      }
    }
  }
  return NULL;
}

/* ========================================================================== */
