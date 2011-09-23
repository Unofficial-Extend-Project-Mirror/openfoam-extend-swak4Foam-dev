#! /bin/sh

. theFiles.sh

for lib in ${SWAKLIBS[@]}
do
  echo "Removing lib$lib from $FOAM_USER_LIBBIN"
  rm $FOAM_USER_LIBBIN/lib$lib.* 
done

for util in ${SWAKUTILS[@]}
do
  echo "Removing $util from $FOAM_USER_APPBIN"
  rm $FOAM_USER_APPBIN/$util
done