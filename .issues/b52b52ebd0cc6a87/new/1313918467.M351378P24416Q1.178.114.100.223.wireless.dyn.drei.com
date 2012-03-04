From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Sun, 21 Aug 2011 11:20:02 +0200
Subject: Re: Python integration
Message-Id: <b52b52ebd0cc6a87-d9d2b14f4e6943cb-artemis@178.114.100.223.wireless.dyn.drei.com>
References: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>
In-Reply-To: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>

Now is able to write and read swak global variables. Looses
independence of swak by that.

Also possible to only run on the master for parallel runs. Untested