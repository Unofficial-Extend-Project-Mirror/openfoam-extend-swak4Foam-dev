#! /bin/bash

boxName=$1

echo "Init script for $boxName"

if [ "$boxName" == "lucid" ]
then
    # needed for add-appt-repository
    apt-get -y install python-software-properties

    add-apt-repository ppa:mercurial-ppa/releases
fi


VERS=$(lsb_release -cs)
echo deb http://www.openfoam.org/download/ubuntu $VERS main > /etc/apt/sources.list.d/openfoam.list

apt-get update -y

apt-get -y install mercurial
apt-get -y install bison
apt-get -y install flex
apt-get -y install g++
apt-get -y install make
apt-get -y install python-dev

# Needed for packaging
export DEBIAN_FRONTEND=noninteractive
apt-get -y install default-mta
apt-get -y install dpkg-dev
apt-get -y install debhelper devscripts cdbs

# Not needed. Just to keep Bernhard happy
apt-get -y install emacs

case "$boxName" in
    lucid)
	ofPackages=( "171" "201" "211" )
	;;
    precise)
	ofPackages=( "221" "211" "220" )
	;;
esac

for v in "${ofPackages[@]}"
do
    echo "Installing OpenFOAM $v"
    apt-get install -y --force-yes "openfoam$v"
done

SWAKDIR=/home/vagrant/swak4Foam

if [ ! -e $SWAKDIR ]
then
    hg clone /swakSources $SWAKDIR
    (cd $SWAKDIR; ln -s swakConfiguration.debian swakConfiguration )
    if [ "$boxName" == "precise" ]
    then
	(cd $SWAKDIR;  hg update port_2.0.x )
    else
	(cd $SWAKDIR;  hg update develop )
    fi
    chown -R vagrant:vagrant $SWAKDIR
else
    echo "Repository already there. No cloning"
fi

for f in $(ls -A /vagrant/skel)
do
    target="/home/vagrant/$f"
    if [ -e $target ]
    then
	echo "$target already there"
    else
	cp -r "/vagrant/skel/$f" $target
    fi
done

echo "Ended"
