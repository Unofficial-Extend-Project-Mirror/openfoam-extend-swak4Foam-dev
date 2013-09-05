#! /bin/bash

echo
echo "Copying stuff from skeleton"
echo
for f in $(ls -A /vagrant/skel)
do
    target="/home/vagrant/$f"
    if [ -e $target ]
    then
	echo "$target already there"
    else
	echo "Copying $target from skeleton"
	cp -r "/vagrant/skel/$f" $target
    fi
done

SWAKDIR=/home/vagrant/swak4Foam

if [ ! -e $SWAKDIR ]
then
    echo
    echo "Getting swak-sources"
    echo
    if [ "$boxName" == "precise" ]
    then
	branchName="port_2.0.x"
    else
	branchName="develop"
    fi
    su vagrant - -c "hg clone -r $branchName /swakSources $SWAKDIR"
    (cd $SWAKDIR; ln -s swakConfiguration.debian swakConfiguration )
    chown -R vagrant:vagrant $SWAKDIR
else
    echo "Repository already there. No cloning"
fi

echo
echo "Current ccache:"
export CCACHE_DIR=/vagrant/ccache4vm; ccache --show-stats

echo
echo "Ended"
