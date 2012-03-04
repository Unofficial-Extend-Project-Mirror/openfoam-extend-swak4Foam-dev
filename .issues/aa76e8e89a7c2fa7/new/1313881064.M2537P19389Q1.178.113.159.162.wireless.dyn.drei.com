From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Sun, 21 Aug 2011 00:56:27 +0200
State: fixed
Subject: Add default values for non-existing variables
Message-Id: <aa76e8e89a7c2fa7-0-artemis@178.113.159.162.wireless.dyn.drei.com>

Mainly of use for variables that don't exist during initialization
(because they will be set later)