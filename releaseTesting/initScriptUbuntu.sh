#! /bin/bash

boxName=$1

echo
echo "Init script for $boxName"
echo

if [ "$boxName" == "lucid" ]
then
    echo
    echo "Additional PYthon-Repository"
    echo

    # needed for add-appt-repository
    apt-get -y install python-software-properties

    add-apt-repository ppa:mercurial-ppa/releases
fi

echo
echo "Adding OpenFOAM-Repository"
echo

VERS=$(lsb_release -cs)
echo deb http://www.openfoam.org/download/ubuntu $VERS main > /etc/apt/sources.list.d/openfoam.list

apt-get update -y

echo
echo "Installing additional packages"
echo

apt-get -y install mercurial
apt-get -y install bison
apt-get -y install flex
apt-get -y install g++
apt-get -y install make
apt-get -y install python-dev
apt-get -y install ccache

echo
echo "Setting for postfix"
echo

# Make sure that default-mta installs
debconf-set-selections <<< "postfix postfix/mailname string vagrant.test.machine.com"
debconf-set-selections <<< "postfix postfix/myhostname string vagrant.test.machine.com"
debconf-set-selections <<< "postfix postfix/main_mailer_type string 'Internet Site'"
debconf-set-selections <<< "postfix postfix/destinations string localhost"

export DEBIAN_FRONTEND=noninteractive

echo
echo "Tools for packaging"
echo

# Needed for packaging
apt-get -y install default-mta
apt-get -y install dpkg-dev
apt-get -y install debhelper devscripts cdbs

# Not needed. Just to keep Bernhard happy
apt-get -y install emacs

echo
echo "Installing OpenFOAM-versions"
echo

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
    echo
    echo "Installing OpenFOAM $v"
    echo
    apt-get install -y --force-yes "openfoam$v"
done

FOAM3PKG=https://downloads.sourceforge.net/project/openfoam-extend/foam-extend-3.0/foam-extend_3.0-1_amd64.precise.deb

PKGDIR=/opt/vagrantDownloads

mkdir $PKGDIR

case "$boxName" in
    precise)
	pkgUrls=( "https://downloads.sourceforge.net/project/openfoam-extend/foam-extend-3.0/foam-extend_3.0-1_amd64.precise.deb" )
	;;
esac

for v in "${pkgUrls[@]}"
do
    echo
    echo "Getting Package $v"
    echo
    wget -c -P $PKGDIR $v
done

dpkg -i $PKGDIR/*.deb
if [ $? -gt 0 ]; then
    # install missing stuff
    apt-get -f --force-yes --yes install
    dpkg -i $PKGDIR/*.deb
fi

/vagrant/initScriptGeneral.sh

echo
echo "Ended"
