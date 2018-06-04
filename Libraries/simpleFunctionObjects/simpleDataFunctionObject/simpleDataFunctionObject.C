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
    2008-2011, 2013, 2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "simpleDataFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(simpleDataFunctionObject, 0);

fileName simpleDataFunctionObject::defaultPostProcDir_("postProcessing");

void simpleDataFunctionObject::setPostProcDir(const fileName &f)
{
    Info << "Setting output directory name for simpleFunctionObjects to "
        << f << endl;
    defaultPostProcDir_=f;
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

simpleDataFunctionObject::simpleDataFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    postProcDir_(defaultPostProcDir_)
{
    Dbug << name << " - Constructor" << endl;

    if(dict.found("postProcDir")) {
        postProcDir_=fileName(
            dict.lookup("postProcDir")
        );
        Info << name << " writes to " << postProcDir_
            << " instead of " << defaultPostProcDir_ << endl;
    }
}

fileName simpleDataFunctionObject::dataDir()
{
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    // make sure that when starting we take the start time
    if(
        obr_.time().timeIndex()
        <=
        obr_.time().startTimeIndex()+1
    ) {
        return baseDir()/obr_.time().timeName(
            obr_.time().startTime().value()
        );
    } else {
        return baseDir()/obr_.time().timeName();
    }
#else
    return baseDir()/obr_.time().timeName();
#endif
}

fileName simpleDataFunctionObject::baseDir()
{
    fileName theDir;
    fileName dir=dirName()+"_"+name();
    if(obr().name()!=polyMesh::defaultRegion) {
        dir=obr().name() / dir;
    }
    if (Pstream::parRun())
    {
        // Put in undecomposed case (Note: gives problems for
        // distributed data running)
        theDir =
            obr_.time().path()
            /".."
            /postProcDir_
            /dir;
    }
    else
    {
        theDir =
            obr_.time().path()
            /postProcDir_
            /dir;
    }

    return theDir;
}

bool simpleDataFunctionObject::start()
{
    Dbug << name() << "::start()" << endl;

    simpleFunctionObject::start();

    mkDir(dataDir());

    return true;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
