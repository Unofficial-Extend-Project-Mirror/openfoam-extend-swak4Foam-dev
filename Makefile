default:
	echo "Use ./Allwmake"

cleanStuff:
	./Allwclean
	wcleanLnIncludeAll

prepareDebian:
	cp swakConfiguration.debian swakConfiguration
	cd debian; ./prepareForPackaging.py

dpkg-only: cleanStuff prepareDebian
#	export DH_ALWAYS_EXCLUDE=.svn:.dep:.o; dpkg-buildpackage -us -uc
#	dpkg-buildpackage -k<PACKAGER_ID>
#	debuild -us -uc
	rm swakConfiguration
	export DH_ALWAYS_EXCLUDE=.svn:.dep:.o; dpkg-buildpackage

source-dpkg: cleanStuff prepareDebian
	export DH_ALWAYS_EXCLUDE=.svn:.dep:.o; dpkg-buildpackage -S -sa

dpkg: dpkg-only

globalinstall:
	./Allwmake
	./copySwakFilesToSite.sh
	./removeSwakFilesFromLocal.sh

scanbuild:
	c_compiler=
	cxx_compiler=
	scan-build -stats  --use-cc=$(shell wmake -show-c) --use-c++=$(shell wmake -show-cxx) -o scanbuild ./Allwmake
