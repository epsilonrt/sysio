#!/bin/bash
if [ $# -lt 1 ]; then
  echo "You must provide the name of the file to be created"
  exit -1
fi

OUTPUT=${1}
EXT=${OUTPUT##*.}
BACKUP=".version.$EXT"

GIT=$(which git)
DIFF=$(which diff)

if [ -n "$GIT" ]; then
  # echo git found
  VERSION="$(${GIT} describe)"
  VERSION=${VERSION#v}
  case "$EXT" in

    h)  echo "#define VERSION \"$VERSION\"" > ${BACKUP}
        ;;
    mk) VERSION=${VERSION%%-*}
        echo "VERSION=$VERSION" > ${BACKUP}
        ;;
    *)  echo "$0: unknown file extension !"
        exit -1
        ;;
  esac

  if [ -n "$DIFF" ]; then
    # echo diff found
    diff -q -w ${BACKUP} ${OUTPUT} > /dev/null  2>&1 && exit 0
  fi
  # ${BACKUP} used for next call if git not found
else
  echo "$0: ${OUTPUT} will not be updated because GIT has not been found on this system"
  exit -1
fi

cp ${BACKUP} ${OUTPUT}
cat ${OUTPUT}
