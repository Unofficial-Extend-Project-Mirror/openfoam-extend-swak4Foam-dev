
dpkg:
	cd debian; ./prepareForPackaging.py
	dpkg-buildpackage -us -uc
