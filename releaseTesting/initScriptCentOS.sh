#! /bin/bash

boxName=$1

echo
echo "Init script for $boxName"
echo

rpm -Uhv http://fedora.aau.at/epel/6/x86_64/epel-release-6-8.noarch.rpm

# yum install mercurial
rpm -Uhv http://pkgs.repoforge.org/mercurial/mercurial-2.2.2-1.el6.rfx.x86_64.rpm
# Don't want to add too many repositories

# minimal is OK. But man should be there
yum install -y man man-pages

# this should add the iberty-library
yum install -y binutils-devel

yum install -y bison
yum install -y flex
yum install -y ccache

# needed by CentFOAM
yum install -y libXt-devel mesa-libOSMesa tcl tix  tk

# Not needed. Just to keep Bernhard happy
yum install -y emacs

# stuff for testing python
yum install -y ipython scipy python-matplotlib

cd /home/vagrant/

of21Dir=/home/vagrant/centFOAM/OpenFOAM/OpenFOAM-2.1.1

# comment out to really install this OF2.1
mkdir -p $of21Dir
chown -R vagrant /home/vagrant/centFOAM

if [ ! -e $of21Dir ]
then
    su vagrant - -c "/vagrant/ThirdParty/centFOAM.py --OF21"
fi

# half a Gig. Currently 2.1 is enough
# su vagrant - -c "/vagrant/ThirdParty/centFOAM.py --OF21"

of16Dir=/home/vagrant/centFOAM/OpenFOAM/OpenFOAM-1.6-ext
if [ ! -e $of16Dir ]
then
    su vagrant - -c "/vagrant/ThirdParty/centFOAM.py --OF16"
fi

ln -s ./centFOAM/OpenFOAM/ .

/vagrant/initScriptGeneral.sh

echo
echo "Ended"
