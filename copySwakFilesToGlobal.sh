#! /bin/sh

. theFiles.sh

for lib in ${SWAKLIBS[@]}
do
  echo "Copying lib$lib to $FOAM_LIBBIN"
  cp $FOAM_USER_LIBBIN/lib$lib.* $FOAM_LIBBIN
done

for util in ${SWAKUTILS[@]}
do
  echo "Copying $util to $FOAM_APPBIN"
  cp $FOAM_USER_APPBIN/$util $FOAM_APPBIN
done