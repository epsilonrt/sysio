###############################################################################
# Copyright © 2017 epsilonRT, All rights reserved.                            #
# This software is governed by the CeCILL license <http://www.cecill.info>    #
###############################################################################
# GitVersion.cmake
# Dépendances: git, git-version
# Variables en sortie :
# ${_prefix}_FOUND
# Si ${_prefix}_FOUND = TRUE:
#   ${_prefix}_STRING
#   ${_prefix}_SHORT
#   ${_prefix}_TINY
#   ${_prefix}_MAJOR
#   ${_prefix}_MINOR
#   ${_prefix}_PATCH
#   ${_prefix}_SHA1

find_package(Git REQUIRED)

#-------------------------------------------------------------------------------
function(GetGitVersion _prefix)
  
  set (${_prefix}_FOUND FALSE PARENT_SCOPE)
  unset(GITVERSION)
  
  #message("GetGitVersion:PROJECT_TOOLS_PATHS=${PROJECT_TOOLS_PATHS}")
  find_program(GITVERSION NAMES git-version PATHS ${PROJECT_TOOLS_PATHS} NO_DEFAULT_PATH)
  find_program(GITVERSION NAMES git-version)
  #message("GetGitVersion:GITVERSION=${GITVERSION}")
  
  if (GITVERSION)
    
    execute_process(COMMAND ${GITVERSION} -q -f RESULT_VARIABLE ret OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
    #message("GetGitVersion:str=${str}")
    #message("GetGitVersion:ret=${ret}")
    if (ret EQUAL 0 AND str)
      #message("GetGitVersion:version=${str}")
      set (${_prefix}_FOUND TRUE PARENT_SCOPE)
      set (${_prefix}_STRING ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}='${str}'")
      execute_process(COMMAND ${GITVERSION} -q -s OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
      set (${_prefix}_SHORT ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}_SHORT='${str}'")
      execute_process(COMMAND ${GITVERSION} -q -t OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
      set (${_prefix}_TINY ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}_TINY='${str}'")
      execute_process(COMMAND ${GITVERSION} -q -M OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
      set (${_prefix}_MAJOR ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}_MAJOR='${str}'")
      execute_process(COMMAND ${GITVERSION} -q -m OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
      set (${_prefix}_MINOR ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}_MINOR='${str}'")
      execute_process(COMMAND ${GITVERSION} -q -p OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
      set (${_prefix}_PATCH ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}_PATCH='${str}'")
      execute_process(COMMAND ${GITVERSION} -q -c OUTPUT_VARIABLE str OUTPUT_STRIP_TRAILING_WHITESPACE)
      set (${_prefix}_SHA1 ${str} PARENT_SCOPE)
      #message (STATUS "${_prefix}_SHA1='${str}'")
    else (ret EQUAL 0 AND str)
      message(FATAL_ERROR "git-version unable to find current git version !")
    endif (ret EQUAL 0 AND str)
  else (GITVERSION)
    message(FATAL_ERROR "git-version not found")
  endif (GITVERSION)
endfunction()

#-------------------------------------------------------------------------------
function(WriteGitVersionFile _filename)

  find_program(GITVERSION NAMES git-version PATHS ${PROJECT_TOOLS_PATHS})
  #message("WriteGitVersionFile:GITVERSION=${GITVERSION}")
  if (GITVERSION)
    execute_process(COMMAND ${GITVERSION} ${_filename})
  endif (GITVERSION)
endfunction()

#-------------------------------------------------------------------------------
