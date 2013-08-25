#! /bin/bash

echo "Init script"

apt-get -y install python-software-properties

VERS=$(lsb_release -cs)
echo deb http://www.openfoam.org/download/ubuntu $VERS main > /etc/apt/sources.list.d/openfoam.list

add-apt-repository ppa:mercurial-ppa/releases

apt-get -y install mercurial
apt-get -y install bison
apt-get -y install flex
apt-get -y install g++
apt-get -y install make
apt-get -y install python-dev

# Needed for packaging
apt-get -y install default-mta
apt-get -y install dpkg-dev
apt-get -y install debhelper devscripts cdbs

# Not needed. Just to keep Bernhard happy
apt-get -y install emacs

apt-get update -y
apt-get install -y --force-yes openfoam221
apt-get install -y --force-yes openfoam171

SWAKDIR=/home/vagrant/swak4Foam

if [ ! -e $SWAKDIR ]
then
    hg clone /swakSources $SWAKDIR
    (cd $SWAKDIR; ln -s swakConfiguration.debian swakConfiguration )
    (cd $SWAKDIR;  hg update port_2.0.x )
    chown -R vagrant:vagrant $SWAKDIR
else
    echo "Repository already there. No cloning"
fi

echo "Ended"
