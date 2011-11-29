From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Wed, 19 Oct 2011 20:10:55 +0200
State: reopened
Subject: Conditional function object
Message-Id: <b197ca3b6e9f1b97-0-artemis@178.113.228.200.wireless.dyn.drei.com>

FunctionObject that encapsulates another function object and only
executes it under certain conditions

Would help to avoid the comment/uncomment-Problem