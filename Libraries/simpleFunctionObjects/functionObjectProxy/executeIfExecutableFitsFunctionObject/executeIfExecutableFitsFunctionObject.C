//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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

\*---------------------------------------------------------------------------*/

#include "executeIfExecutableFitsFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"
#include "argList.H"

#ifdef darwin
#include "mach-o/dyld.h"
#endif
#ifdef linux
#include <unistd.h>
#endif
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(executeIfExecutableFitsFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        executeIfExecutableFitsFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

executeIfExecutableFitsFunctionObject::executeIfExecutableFitsFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    conditionalFunctionObjectListProxy(
        name,
        t,
        dict
    )
{
    // do it here to avoid the superclass-read being read twice
    readRegexp(dict);

    fileName exePath;
    
#ifdef darwin
    {
        char path[1024];
        uint32_t size = sizeof(path);
        if (_NSGetExecutablePath(path, &size) == 0) {
            exePath=string(path);
        }
    }
#elif defined(linux)
    {
        const int bufSize=1024;
        char path[bufSize];
        label length=readlink("/proc/self/exe",path,bufSize-1);
        path[length]='\0';
        exePath=string(path);
    }
#else
    Not yet implemented;
#endif

    executable_=exePath.name();

    if(debug) {
        Info << "Executable: " << executable_ << " "<< exePath << endl;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool executeIfExecutableFitsFunctionObject::condition()
{
    return executableNameRegexp_.match(executable_);
}

void executeIfExecutableFitsFunctionObject::readRegexp(const dictionary& dict)
{
    executableNameRegexp_.set(
        string(dict.lookup("executableNameRegexp")),
        dict.lookupOrDefault<bool>("ignoreCase",false)
    );
}

bool executeIfExecutableFitsFunctionObject::read(const dictionary& dict)
{
    readRegexp(dict);
    return conditionalFunctionObjectListProxy::read(dict);
}

} // namespace Foam

// ************************************************************************* //
