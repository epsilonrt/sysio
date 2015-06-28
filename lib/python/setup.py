#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  setup.py
#
#-----------------------------------------------------------------------------#
# Copyright © 2015 Pascal JEAN aka epsilonRT <pascal.jean--AT--btssn.net>     #
# All rights reserved.                                                        #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
#-----------------------------------------------------------------------------#

from distutils.core import setup, Extension

module_swig = Extension('_sysio',
                        ['sysio.i'],
                        include_dirs = ['/usr/local/include'],
                        libraries = ['sysio++','stdc++'],
                        library_dirs = ['/usr/local/lib'],
                        swig_opts=['-c++'],
                        language='c++')
setup (name = 'sysio',
       version = '1.1',
       description = 'Extended I/O system library',
       author = 'Pascal JEAN aka epsilonrt',
       author_email = 'pjean@btssn.net',
       url = 'http://www.btssn.net',
       license = 'CeCILL license Version 2.1 <http://www.cecill.info>',
       ext_modules = [module_swig],
       py_modules = ['sysio'],)

