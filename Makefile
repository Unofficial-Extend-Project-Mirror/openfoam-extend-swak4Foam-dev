default:
	echo "Use ./Allwmake"

getSimple:
	./downloadSimpleFunctionObjects.sh
	wcleanLnIncludeAll

cleanStuff:
	./Allwclean
	wcleanLnIncludeAll

dpkg-only: cleanStuff
	cd debian; ./prepareForPackaging.py
	export DH_ALWAYS_EXCLUDE=.svn:.dep:.o; dpkg-buildpackage -us -uc
#	dpkg-buildpackage -k<PACKAGER_ID>
#	debuild -us -uc

dpkg: getSimple dpkg-only

install:
	./downloadSimpleFunctionObjects.sh
	./Allwmake
	./copySwakFilesToGlobal.sh
	./removeSwakFilesFromLocal.sh
