#! /bin/bash

boxName=$1

echo
echo "Init script for $boxName"
echo

echo "Install the EPEL-repository for additional software"

if [ "$boxName" == "centos70" ]
then
    echo "Centos 7"
    rpm -Uhv http://mirror.digitalnova.at/epel/7/x86_64/e/epel-release-7-2.noarch.rpm
else
    echo "Centos 6"
    rpm -Uvh http://download.fedoraproject.org/pub/epel/6/i386/epel-release-6-8.noarch.rpm
fi

# Don't want to add too many repositories

# minimal is OK. But man should be there
yum install -y man man-pages

neededPackages=(gcc-c++ gcc-gfortran mercurial git flex bison make ccache rpm-build wget zlib-devel binutils-devel libXt-devel cmake)
bonusPackages=(emacs csh tcsh zsh)

for p in ${neededPackages[@]}; do
    yum install -y $p
done

for p in ${bonusPackages[@]}; do
    yum install -y $p
done

# needed by CentFOAM
yum install -y libXt-devel mesa-libOSMesa tcl tix  tk

# Not needed. Just to keep Bernhard happy
yum install -y emacs

# stuff for testing python
yum install -y ipython scipy python-matplotlib

if [ "$boxName" == "centos6" ]
then
    echo "Update mercurial to a more recent version"
    rpm -Uhv http://pkgs.repoforge.org/mercurial/mercurial-2.2.2-1.el6.rfx.x86_64.rpm
fi

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
