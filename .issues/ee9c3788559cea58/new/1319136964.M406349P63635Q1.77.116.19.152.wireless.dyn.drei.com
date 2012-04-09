From: Bernhard F.W. Gschaider <bgschaid@ice-sf.at>
Date: Thu, 20 Oct 2011 20:54:49 +0200
Subject: Re: functionObject calculateGlobalVariable
Message-Id: <ee9c3788559cea58-e121464095cd5511-artemis@77.116.19.152.wireless.dyn.drei.com>
References: <ee9c3788559cea58-0-artemis@bgs-Greybook.local>
In-Reply-To: <ee9c3788559cea58-0-artemis@bgs-Greybook.local>

Writing an reading now works. But it looks like one data set is
missing from stored variables because the functionObject is called
AFTER the data is written