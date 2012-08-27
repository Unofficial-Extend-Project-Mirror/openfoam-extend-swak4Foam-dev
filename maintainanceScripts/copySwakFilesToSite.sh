#! /bin/sh

. ./theFiles.sh

if [ -d $FOAM_SITE_LIBBIN ]
then
    echo "$FOAM_SITE_LIBBIN already existing"
else
    mkdir -pv $FOAM_SITE_LIBBIN
fi

if [ -d $FOAM_SITE_APPBIN ]
then
    echo "$FOAM_SITE_APPBIN already existing"
else
    mkdir -pv $FOAM_SITE_APPBIN
fi

for lib in ${SWAKLIBS[@]}
do
  echo "Copying lib$lib to $FOAM_SITE_LIBBIN"
  cp $FOAM_USER_LIBBIN/lib$lib.* $FOAM_SITE_LIBBIN
done

for util in ${SWAKUTILS[@]}
do
  echo "Copying $util to $FOAM_SITE_APPBIN"
  cp $FOAM_USER_APPBIN/$util $FOAM_SITE_APPBIN
done
