#! /bin/bash

boxName=$1

echo
echo "Init script for $boxName"
echo

rpm -Uhv http://fedora.aau.at/epel/6/x86_64/epel-release-6-8.noarch.rpm

# yum install mercurial
rpm -Uhv http://pkgs.repoforge.org/mercurial/mercurial-2.2.2-1.el6.rfx.x86_64.rpm
# Don't want to add too many repositories

yum install -y bison
yum install -y flex
yum install -y ccache

# Not needed. Just to keep Bernhard happy
yum install -y emacs

cd /home/vagrant/

# wget http://downloads.sourceforge.net/project/centfoam/centFOAM.py?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fcentfoam%2Ffiles%2F&ts=1378420317&use_mirror=switch
# chmod a+x centFOAM.py

/vagrant/initScriptGeneral.sh

echo
echo "Ended"
