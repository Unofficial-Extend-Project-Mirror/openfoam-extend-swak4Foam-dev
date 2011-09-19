From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Wed, 10 Aug 2011 01:21:42 +0200
Subject: Re: Python integration
Message-Id: <b52b52ebd0cc6a87-b7b16e50ab551331-artemis@178.113.160.225.wireless.dyn.drei.com>
References: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>
In-Reply-To: <b52b52ebd0cc6a87-0-artemis@178.113.160.225.wireless.dyn.drei.com>

Now strings are read from a dictionary and executed.

Todo:
- Compile the strings and execute the code-objects (if possible)
- Check whether we need write
- Make time and case available to the code
- Read code from files