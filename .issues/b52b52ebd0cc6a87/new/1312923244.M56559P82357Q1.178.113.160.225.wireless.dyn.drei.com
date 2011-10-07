From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Tue, 09 Aug 2011 22:52:06 +0200
State: fixed
Subject: Python integration
Message-Id: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>

Add a function object that would execute a python-script. Possible
application would be to use PyFoam to manipulate dictionaries on the fly