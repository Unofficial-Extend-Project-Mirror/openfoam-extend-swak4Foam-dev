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

    SWAKParent="/swakSources"

    branchName=`hg branch -R $SWAKParent`

    echo "Parent is on branch $branchName"

    if [ "$boxName" == "precise" ]
    then
	echo "Using 2.x-branch anyway"
	branchName="port_2.0.x"
    fi
    su vagrant - -c "hg clone -u $branchName $SWAKParent $SWAKDIR"
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
