###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# LD_LIBRARY_PATH=. python test/python/unit/test_unit.py

MODULE=sysio

CPPFLAGS=$(shell python-config --cflags | sed 's/-Wstrict-prototypes//g')
TARGET=_$(MODULE).so

MSG_BUILDING = [BUILD]\t\t
MSG_CLEANING = [CLEAN]\t\t
MSG_INSTALL = [INSTALL]
MSG_UNINSTALL = [UNINSTALL]
MSG_COMPILING_CPP = [CPP]\t\t
MSG_LINKING = [LINK]\t\t
MSG_SWIGING = [SWIG]\t\t

all: build
lib: $(TARGET)

clean:
	@echo "$(MSG_CLEANING)"
	@rm -f $(TARGET) $(MODULE)_wrap.* $(MODULE).py*

distclean: clean
	@rm -fr build

build:
	@echo "$(MSG_BUILDING)"
	@python setup.py build_ext

install: build
	@echo "$(MSG_INSTALL)"
	python setup.py install
	@-ldconfig

uninstall: 
	@echo "$(MSG_UNINSTALL)"
	@python setup.py install --record .installfiles
	@(cat .installfiles | xargs rm -fr)
	@rm -f .installfiles

$(MODULE)_wrap.cpp: $(MODULE).i
	@echo "$(MSG_SWIGING)"
	@swig -python -c++ -o $@ $<

$(MODULE)_wrap.o: $(MODULE)_wrap.cpp
	@echo "$(MSG_COMPILING_CPP)"
	@gcc -fPIC -c $< $(CPPFLAGS)

_$(MODULE).so: $(MODULE)_wrap.o
	@echo "$(MSG_LINKING)"
	@gcc -shared $< -lsysio++ -lstdc++ -o $@

.PHONY : all build lib clean distclean build install uninstall
