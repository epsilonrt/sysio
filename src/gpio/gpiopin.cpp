/**
 * @file
 * @brief Broche et connecteurs GPIO
 *
 * Copyright © 2018 epsilonRT, All rights reserved.
 * This software is governed by the CeCILL license <http://www.cecill.info>
 */
#include <sysio/gpio.h>
#include <sysio/gpiodevice.h>
#include <exception>
#include <fstream>
#include <sstream>
//
#include <csignal>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

namespace Gpio {

// -----------------------------------------------------------------------------
//
//                              Pin Class
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
  std::string Pin::_syspath = "/sys/class/gpio";

// -----------------------------------------------------------------------------
  const std::map<Pin::Type, std::string> Pin::_types = {
    {TypeGpio, "Gpio"},
    {TypePower, "Power"},
    {TypeUsb, "Usb"},
    {TypeSound, "Sound"},
    {TypeVideo, "Video"},
    {TypeNotConnected, "Not Connected"},
    {TypeUnknown, "Unknown"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Numbering, std::string> Pin::_numberings = {
    {NumberingLogical, "Logical"},
    {NumberingMcu, "Mcu"},
    {NumberingSystem, "System"},
    {NumberingUnknown, "Unknown"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Pull, std::string> Pin::_pulls = {
    {PullOff, "off"},
    {PullDown, "down"},
    {PullUp, "up"},
    {PullUnknown, "unk"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Edge, std::string> Pin::_edges = {
    {EdgeNone, "none"},
    {EdgeRising, "rising"},
    {EdgeFalling, "falling"},
    {EdgeBoth, "both"},
    {EdgeUnknown, "unk"}
  };

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> Pin::_sysfsmodes = {
    { ModeInput, "in" },
    { ModeOutput, "out" },
    { ModeUnknown, "unk" }
  };

// -----------------------------------------------------------------------------
  const std::map<std::string, Pin::Edge> Pin::_str2edge = {
    { "none", EdgeNone },
    { "rising", EdgeRising },
    { "falling", EdgeFalling },
    { "both", EdgeBoth }
  };

// -----------------------------------------------------------------------------
  const std::map<std::string, Pin::Mode> Pin::_str2mode = {
    { "in", ModeInput },
    { "out", ModeOutput }
  };

// -----------------------------------------------------------------------------
  Pin::Pin (Connector * parent, const Descriptor * desc) :
    _isopen (false), _parent (parent), _descriptor (desc), _holdMode (ModeUnknown),
    _holdPull (PullUnknown), _holdState (false), _useSysFs (false),
    _holdExported (-1), _valueFd (-1) {

    AccessLayer layer = parent->board()->accessLayer();

    if (layer == AccessLayerSysFs) {

      // Seul accès possible SysFs: on exporte la broche
      _useSysFs = true;
    }
    else if (sysFsIsExport()) {

      // Export existant, on laisse si autorisé sinon on dévalide l'export
      _useSysFs = ( (layer & AccessLayerSysFs) != 0);
    }
  }

// -----------------------------------------------------------------------------
  Pin::~Pin() {

    close();
  }

// -----------------------------------------------------------------------------
  bool Pin::isOpen() const {

    return _isopen;
  }

// -----------------------------------------------------------------------------
  bool Pin::open() {

    if (!isOpen()) {

      if ( (type() == TypeGpio) && _useSysFs) {

        _isopen = sysFsEnable (true);
      }
      else {

        _isopen = true;
      }
    }
    return isOpen();
  }

// -----------------------------------------------------------------------------
  void
  Pin::close() {

    if (isOpen()) {

      if (type() == TypeGpio) {

        sysFsClose();

        if (board()->releaseOnClose()) {

          release();
        }

      }
      _isopen = false;
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::useSysFs() const {

    return _useSysFs;
  }

// -----------------------------------------------------------------------------
  AccessLayer
  Pin::accessLayer() const {

    board()->accessLayer();
  }

// -----------------------------------------------------------------------------
  Pin::Type
  Pin::type() const {

    return _descriptor->type;
  }

// -----------------------------------------------------------------------------
  int
  Pin::logicalNumber () const {

    return _descriptor->num.logical;
  }

// -----------------------------------------------------------------------------
  int
  Pin::mcuNumber () const {

    return _descriptor->num.mcu;
  }

// -----------------------------------------------------------------------------
  int
  Pin::systemNumber () const {

    return _descriptor->num.system;
  }

// -----------------------------------------------------------------------------
  int
  Pin::number (Pin::Numbering n) const {

    switch (n) {
      case NumberingLogical:
        return logicalNumber();
      case NumberingMcu:
        return mcuNumber();
      case NumberingSystem:
        return systemNumber();
      default:
        break;
    }

    throw std::invalid_argument ("Bad pin numbering requested");
  }

// -----------------------------------------------------------------------------
  int
  Pin::row() const {

    return _descriptor->num.row;
  }

// -----------------------------------------------------------------------------
  int
  Pin::column() const {

    return _descriptor->num.column;
  }

// -----------------------------------------------------------------------------
  int
  Pin::physicalNumber () const {

    return connector()->pinNumber (row(), column());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::name (Pin::Mode m) const {

    return _descriptor->name.at (m);
  }

// -----------------------------------------------------------------------------
  Connector *
  Pin::connector() const {

    return _parent;
  }

// -----------------------------------------------------------------------------
  Board *
  Pin::board() const {

    return connector()->board();
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::typeName() const {

    return typeName (type());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::numberingName (Pin::Numbering n)  {

    return _numberings.at (n);
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::typeName (Pin::Type t) {

    return _types.at (t);
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::edgeName (Pin::Edge e) {

    return _edges.at (e);
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::pullName (Pin::Pull p) {

    return _pulls.at (p);
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Type, std::string> &
  Pin::types () {

    return _types;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Numbering, std::string> &
  Pin::numberings () {

    return _numberings;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Pull, std::string> &
  Pin::pulls () {

    return _pulls;
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Edge, std::string> &
  Pin::edges () {

    return _edges;
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::modeName (Pin::Mode m) const {

    return modes().at (m);
  }

// -----------------------------------------------------------------------------
  Device *
  Pin::device() const {

    return connector()->device();
  }

// -----------------------------------------------------------------------------
  const std::map<Pin::Mode, std::string> &
  Pin::modes () const {

    return device() ? device()->modes() : _sysfsmodes;
  }

// -----------------------------------------------------------------------------
  bool
  Pin::isDebug() const {

    return device() ? device()->isDebug() : false;
  }

// -----------------------------------------------------------------------------
  void
  Pin::setDebug (bool enable) {

    if (device()) {

      device()->setDebug (enable);
    }
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::modeName() const {

    return modeName (mode ());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::pullName () const {

    return pullName (pull());
  }

// -----------------------------------------------------------------------------
  const std::string &
  Pin::name() const {

    if (isOpen() && (type() == TypeGpio)) {

      try {

        return name (mode());
      }
      catch (...) {
        // Pas de nom pour le mode concerné
      }
    }
    return name (ModeInput);
  }

// -----------------------------------------------------------------------------
  Pin::Pull
  Pin::pull () const {

    if (isOpen() && (type() == TypeGpio) && device()) {

      return device()->pull (this);
    }
    return PullUnknown;
  }

// -----------------------------------------------------------------------------
  void
  Pin::setPull (Pin::Pull p) {

    if (isOpen() && (type() == TypeGpio) && device()) {

      holdPull();
      device()->setPull (this, p);
    }
  }

// -----------------------------------------------------------------------------
  Pin::Mode
  Pin::mode () const {

    if (isOpen() && (type() == TypeGpio)) {

      if (_useSysFs) {

        if (sysFsFileExist ("direction")) {

          std::string value = sysFsReadFile ("direction");
          return _str2mode.at (value);
        }
      }
      else {
        return device()->mode (this);
      }
    }
    return ModeUnknown;
  }

// -----------------------------------------------------------------------------
  void
  Pin::setMode (Pin::Mode m) {

    if (isOpen() && (type() == TypeGpio)) {

      holdMode();
      if (_useSysFs) {

        if (sysFsFileExist ("direction") && ( (m == ModeInput) || (m == ModeOutput))) {

          sysFsWriteFile ("direction", modeName (m));
        }
        else {
          std::ostringstream msg;

          msg << "Pin " << name (ModeInput) << " does not support " << modeName (m) << " mode";
          throw std::system_error (ENOTSUP, std::system_category(), msg.str());
        }
      }
      else {

        device()->setMode (this, m);
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::setEdge (Pin::Edge e) {

    if (isOpen() && (type() == TypeGpio) && _useSysFs) {

      if (sysFsFileExist ("edge")) {

        sysFsWriteFile ("edge", edgeName (e));
        read(); // clear irq
      }
      else {
        std::ostringstream msg;

        msg << "Pin " << name (ModeInput) << " does not have interrupt";
        throw std::system_error (ENOTSUP, std::system_category(), msg.str());
      }
    }
  }

// -----------------------------------------------------------------------------
  Pin::Edge
  Pin::edge() const {

    if (isOpen() && (type() == TypeGpio) && _useSysFs && sysFsFileExist ("edge")) {
      std::string value;

      value = sysFsReadFile ("edge");
      return _str2edge.at (value);
    }
    return EdgeUnknown;
  }

// -----------------------------------------------------------------------------
  void
  Pin::write (bool value) {

    if (isOpen() && (type() == TypeGpio)) {

      if (_useSysFs) {
        int ret;

        if (value) {

          ret = ::write (_valueFd, "1\n", 2);
        }
        else {

          ret = ::write (_valueFd, "0\n", 2);
        }

        if (ret < 0) {

          throw std::system_error (errno, std::system_category(), "write");
        }
      }
      else {

        device()->write (this, value);
      }
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::toggle () {

    if (isOpen() && (type() == TypeGpio)) {

      if (_useSysFs) {

        write (!read());
      }
      else {

        device()->toggle (this);
      }
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::read () const {

    if (isOpen() && (type() == TypeGpio)) {

      if (_useSysFs) {
        int ret;
        char buffer[2];

        lseek (_valueFd, 0, SEEK_SET);
        // Lire la valeur actuelle du GPIO.
        ret = ::read (_valueFd, buffer, sizeof (buffer));
        if (ret < 0) {

          throw std::system_error (errno, std::system_category(), "read");
        }

        return buffer[0] != '0';
      }

      return device()->read (this);
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::release () {

    if (isOpen() && (type() == TypeGpio)) {

      if (_holdExported >= 0) {

        forceUseSysFs (_holdExported);
        _holdExported = -1;
      }

      if (_holdMode != ModeUnknown) {

        setMode (_holdMode);
        if (_holdMode == ModeOutput) {

          write (_holdState);
        }
        _holdMode = ModeUnknown;
      }

      if (_holdPull != PullUnknown) {

        setPull (_holdPull);
        _holdPull = PullUnknown;
      }
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::forceUseSysFs (bool enable) {

    if (isOpen() && (_useSysFs != enable)) {

      _useSysFs = sysFsEnable (enable);
    }
    else {

      _useSysFs = enable;
    }
    return useSysFs();
  }

// -----------------------------------------------------------------------------
  bool
  Pin::waitForInt (Pin::Edge e, int timeout_ms) {
    int ret;
    char buffer[2];
    struct pollfd  fds;

    forceUseSysFs (true);
    setEdge (e); // passes en mode SysFs si nécessaire

    fds.fd = _valueFd;
    fds.events = POLLPRI | POLLERR;
    ret = poll (& fds, 1, timeout_ms);
    if (ret < 0) {

      throw std::system_error (errno, std::system_category(), "poll");
    }
    else if (ret == 0) {

      throw std::system_error (ETIME, std::generic_category());
    }

    lseek (_valueFd, 0, SEEK_SET);
    // Lire la valeur actuelle du GPIO.
    ret = ::read (_valueFd, buffer, sizeof (buffer));
    if (ret < 0) {

      throw std::system_error (errno, std::system_category(), "read");
    }

    return buffer[0] != '0';
  }

// -----------------------------------------------------------------------------
//                                   Private
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
  void Pin::holdPull() {

    if (_holdPull == PullUnknown) {

      _holdPull = pull();
    }
  }

// -----------------------------------------------------------------------------
  void
  Pin::holdMode() {

    if (_holdMode == ModeUnknown) {

      _holdMode = mode();
      if (_holdMode == ModeOutput) {

        _holdState = read();
      }
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsEnable (bool enable) {

    if (enable) {

      holdMode();
      holdPull();
      sysFsExport (true);
      return sysFsOpen();
    }

    sysFsClose();
    sysFsExport (false);
    return false;
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsExport (bool enable) {

    if ( (type() == TypeGpio) && (enable != sysFsIsExport())) {
      std::ostringstream fn;
      std::ofstream f;

      if (systemNumber() < 0) {
        std::ostringstream msg;

        msg << "Unknown pin number in SysFs for " << name (ModeInput) << " !";
        throw std::invalid_argument (msg.str());
      }

      if (_holdExported < 0) {

        _holdExported = sysFsIsExport();
      }

      if (enable) {

        // Export
        fn << _syspath << "/export";

      }
      else {

        // Unexport
        fn << _syspath << "/unexport";
      }

      f.open (fn.str());
      f.exceptions (f.failbit | f.badbit);
      f << systemNumber() << std::endl;
      f.exceptions (f.failbit | f.badbit);
      f.close();
    }
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsOpen() {

    if (_valueFd < 0) {
      std::ostringstream fn;
      char buffer[2];

      // Construction du nom du fichier
      fn << _syspath << "/gpio" << systemNumber() << "/value";

      // Ouvrir le fichier
      if ( (_valueFd = ::open (fn.str().c_str(), O_RDWR)) < 0) { //

        throw std::system_error (errno, std::system_category(), "open");
      }
    }
    return _valueFd >= 0;
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsClose() {

    if (_valueFd >= 0) {

      // fermeture fichier value sysFs
      if (::close (_valueFd) < 0) {

        throw std::system_error (errno, std::system_category(), "close");
      }
      _valueFd = -1;
    }
  }

// -----------------------------------------------------------------------------
  std::string
  Pin::sysFsReadFile (const char * n) const {
    std::ostringstream fn;
    std::ifstream f;
    std::string value;
    std::string::size_type pos;

    fn << _syspath << "/gpio" << systemNumber() << "/" << n;
    f.open (fn.str());
    f.exceptions (f.failbit | f.badbit);

    std::getline (f, value);
    f.exceptions (f.failbit | f.badbit);

    pos = value.rfind ('\n');
    if (pos != std::string::npos) {
      value.resize (pos + 1);
    }

    f.close();
    return value;
  }

// -----------------------------------------------------------------------------
  void
  Pin::sysFsWriteFile (const char * n, const std::string & v) {
    std::ostringstream fn;
    std::ofstream f;

    fn << _syspath << "/gpio" << systemNumber() << "/" << n;
    f.open (fn.str());
    f.exceptions (f.failbit | f.badbit);

    f << v << std::endl;
    f.exceptions (f.failbit | f.badbit);
    f.close();
  }

// -----------------------------------------------------------------------------
  bool
  Pin::directoryExist (const std::string & dname) {
    struct stat sb;

    return (stat (dname.c_str(), &sb) == 0 && S_ISDIR (sb.st_mode));
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsIsExport () const {
    std::ostringstream fn;

    fn << _syspath << "/gpio" << systemNumber();
    return directoryExist (fn.str());
  }

// -----------------------------------------------------------------------------
  bool
  Pin::sysFsFileExist (const char * n) const {
    struct stat sb;
    std::ostringstream fn;

    fn << _syspath << "/gpio" << systemNumber() << "/" << n;
    return (stat (fn.str().c_str(), &sb) == 0 && S_ISREG (sb.st_mode));
  }
}
/* ========================================================================== */
