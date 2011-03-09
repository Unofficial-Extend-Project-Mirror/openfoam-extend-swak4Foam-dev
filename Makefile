default:
	echo "Use ./Allwmake"

getSimple:
	./downloadSimpleFunctionObjects.sh

dpkg-only:
	cd debian; ./prepareForPackaging.py
#	dpkg-buildpackage -us -uc
	debuild -us -uc

dpkg: getSimple dpkg-only
