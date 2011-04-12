default:
	echo "Use ./Allwmake"

getSimple:
	./downloadSimpleFunctionObjects.sh
	wcleanLnIncludeAll

dpkg-only:
	cd debian; ./prepareForPackaging.py
#	dpkg-buildpackage -us -uc
	dpkg-buildpackage -kBernhard.Gschaider@ice-sf.at
#	debuild -us -uc

dpkg: getSimple dpkg-only

install:
	./downloadSimpleFunctionObjects.sh
	./Allwmake
	./copySwakFilesToGlobal.sh
	./removeSwakFilesFromLocal.sh
