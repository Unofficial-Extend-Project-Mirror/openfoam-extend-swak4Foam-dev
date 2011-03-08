
dpkg:
	./downloadSimpleFunctionObjects.sh
	cd debian; ./prepareForPackaging.py
	dpkg-buildpackage -us -uc
