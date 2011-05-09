#! /bin/sh

. theFiles.sh

for lib in ${SWAKLIBS[@]}
do
  echo "Removing lib$lib from $FOAM_SITE_LIBBIN"
  rm $FOAM_SITE_LIBBIN/lib$lib.* 
done

for util in ${SWAKUTILS[@]}
do
  echo "Removing $util from $FOAM_SITE_APPBIN"
  rm $FOAM_SITE_APPBIN/$util
done