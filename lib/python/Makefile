###############################################################################
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# LD_LIBRARY_PATH=. python test/unit/test_unt.py

MODULE=sysio

CPPFLAGS=$(shell python-config --cflags | sed 's/-Wstrict-prototypes//g')
TARGET=_$(MODULE).so

all:$(TARGET)
clean:
	rm -f $(TARGET) $(MODULE)_wrap.* $(MODULE).py*
	rm -fr build

$(MODULE)_wrap.cpp: $(MODULE).i
	swig -python -c++ -o $@ $<

$(MODULE)_wrap.o: $(MODULE)_wrap.cpp
	gcc -fPIC -c $< $(CPPFLAGS)

_$(MODULE).so: $(MODULE)_wrap.o
	gcc -shared $< -lradio++ -lstdc++ -o $@

