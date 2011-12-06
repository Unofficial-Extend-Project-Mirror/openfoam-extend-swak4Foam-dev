From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Sun, 06 Nov 2011 10:00:14 +0100
State: fixed
Subject: Make Allwmake check for bison
Message-Id: <a7061c7160c682d4-0-artemis@178.114.153.215.wireless.dyn.drei.com>

Add a check to Allwmake whether bison/flex is installed and whether
the version is sufficient