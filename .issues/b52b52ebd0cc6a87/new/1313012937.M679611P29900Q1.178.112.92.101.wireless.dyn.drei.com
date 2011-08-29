From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Wed, 10 Aug 2011 23:47:04 +0200
Subject: Re: Python integration
Message-Id: <b52b52ebd0cc6a87-e5471700aeca2ebc-artemis@178.112.92.101.wireless.dyn.drei.com>
References: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>
In-Reply-To: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>

Works now and can read from files

Refactor the Interpreter-stuff into a separate base-class. THat way
other PYthon-libraries can use the counting how many interpreters are
needed 