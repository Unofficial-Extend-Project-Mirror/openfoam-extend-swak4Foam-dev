/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudRepository.H"
#include "writer.H"

#include "polyMesh.H"
#include "meshSearch.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(CloudRepository, 0);

CloudRepository *CloudRepository::repositoryInstance(NULL);

CloudRepository::CloudRepository(const IOobject &o)
    :
    regIOobject(o)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CloudRepository::~CloudRepository()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

CloudRepository &CloudRepository::getRepository(const objectRegistry &obr)
{
    CloudRepository*  ptr=repositoryInstance;

    if(debug) {
        Pout << "CloudRepository: asking for Singleton" << endl;
    }

    if(ptr==NULL) {
        Pout << "swak4Foam: Allocating new repository for sampledSets\n";

        ptr=new CloudRepository(
            IOobject(
                "swakClouds",
                obr.time().timeName(),
                "cloudRepository",
                obr.time(),
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            )
        );
    }

    repositoryInstance=ptr;

    return *repositoryInstance;
}

bool CloudRepository::writeData(Ostream &f) const
{
    if(debug) {
        Info << "CloudRepository::write()" << endl;
    }

    f << clouds_.toc();

    return true;
}

void CloudRepository::addCloud(
    autoPtr<cloud> c
) {
    const word &name=c->name();

    if(clouds_.found(name)) {
        WarningIn("CloudRepository::addCloud")
            << "Repository of clouds already has an entry "
                << name <<". Overwriting. Expect strange behaviour"
                << endl;
        clouds_.set(
            name,
            c.ptr()
        );
    } else {
        clouds_.insert(
            name,
            c.ptr()
        );
    }
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
