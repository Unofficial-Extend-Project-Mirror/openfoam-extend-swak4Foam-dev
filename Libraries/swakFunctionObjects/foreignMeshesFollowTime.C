/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2012-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
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

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
bool
#else
void
#endif
Foam::foreignMeshesFollowTime::write()
{
    followCurrentTime();

#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
    return true;
#endif
}

void Foam::foreignMeshesFollowTime::clearData()
{
}

// ************************************************************************* //
