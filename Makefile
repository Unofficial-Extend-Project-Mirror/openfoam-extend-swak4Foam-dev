default:
	echo "Use ./Allwmake"

getSimple:
	./downloadSimpleFunctionObjects.sh
	wcleanLnIncludeAll

dpkg-only:
	cd debian; ./prepareForPackaging.py
	dpkg-buildpackage -us -uc
#	dpkg-buildpackage -k<PACKAGER_ID>
#	debuild -us -uc

dpkg: getSimple dpkg-only

install:
	./downloadSimpleFunctionObjects.sh
	./Allwmake
	./copySwakFilesToGlobal.sh
	./removeSwakFilesFromLocal.sh
