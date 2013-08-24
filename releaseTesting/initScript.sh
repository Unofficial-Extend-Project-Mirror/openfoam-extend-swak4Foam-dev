#! /bin/bash

echo "Init script"

apt-get -y install mercurial
apt-get -y install bison
apt-get -y install flex
apt-get -y install g++

VERS=$(lsb_release -cs)
echo deb http://www.openfoam.org/download/ubuntu $VERS main > /etc/apt/sources.list.d/openfoam.list

apt-get update -y
apt-get install -y --force-yes openfoam221

echo "Ended"
