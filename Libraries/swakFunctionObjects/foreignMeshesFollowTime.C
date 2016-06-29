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
    \\  /    A nd           | Copyright  held by original author
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
    2012-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $ 
\*---------------------------------------------------------------------------*/

#include "foreignMeshesFollowTime.H"

#include "CommonValueExpressionDriver.H"

namespace Foam {
    defineTypeNameAndDebug(foreignMeshesFollowTime,0);
}

Foam::foreignMeshesFollowTime::foreignMeshesFollowTime
(
    const word& name,
    const objectRegistry& obr,
    const dictionary& dict,
    const bool loadFromFiles
):
    obr_(obr)
{
    read(dict);
    execute();
}

Foam::foreignMeshesFollowTime::~foreignMeshesFollowTime()
{}

void Foam::foreignMeshesFollowTime::followCurrentTime()
{
    if(followingMeshes_.size()<=0) {
        return;
    }
    scalar t=obr_.time().value();
    label index=obr_.time().timeIndex();
    Info << "Setting meshes to t=" << t << endl;
    forAll(followingMeshes_,i) {
        const word &name=followingMeshes_[i];
        Info << "  " << name << " : " << flush;
        scalar usedTime=MeshesRepository::getRepository().setTime(
            name,
            t,
            index
        );
        Info << " actual time: t=" << usedTime << endl;
    }
}

void Foam::foreignMeshesFollowTime::timeSet()
{
    // Do nothing
}

void Foam::foreignMeshesFollowTime::read(const dictionary& dict)
{
    followingMeshes_=wordList(dict.lookup("followingMeshes"));
    followCurrentTime();
}

void Foam::foreignMeshesFollowTime::execute()
{
    //    followCurrentTime();
}


void Foam::foreignMeshesFollowTime::end()
{
    followCurrentTime();
}

void Foam::foreignMeshesFollowTime::write()
{
    followCurrentTime();
}

void Foam::foreignMeshesFollowTime::clearData()
{
}

// ************************************************************************* //
