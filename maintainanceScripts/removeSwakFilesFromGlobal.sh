#! /bin/sh

. theFiles.sh

for lib in ${SWAKLIBS[@]}
do
  echo "Removing lib$lib from $FOAM_LIBBIN"
  rm $FOAM_LIBBIN/lib$lib.* 
done

for util in ${SWAKUTILS[@]}
do
  echo "Removing $util from $FOAM_APPBIN"
  rm $FOAM_APPBIN/$util
done