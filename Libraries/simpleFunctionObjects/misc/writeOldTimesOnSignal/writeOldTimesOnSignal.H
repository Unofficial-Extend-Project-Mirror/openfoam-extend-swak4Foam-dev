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

Class
    writeOldTimesOnSignalFunctionObject

Description
    This function object tries to record old fields and writes them in
    case of a Signal

SourceFiles
    writeOldTimesOnSignalFunctionObject.C

Contributors/Copyright:
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef writeOldTimesOnSignalFunctionObject_H
#define writeOldTimesOnSignalFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "TimeCloneList.H"
#include "DynamicList.H"

#include <signal.h>

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class writeOldTimesOnSignalFunctionObject Declaration
\*---------------------------------------------------------------------------*/

    class objectRegistry;

class writeOldTimesOnSignalFunctionObject
:
    public functionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    writeOldTimesOnSignalFunctionObject(const writeOldTimesOnSignalFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const writeOldTimesOnSignalFunctionObject&);

    TimeCloneList times_;

    const Time &theTime_;

    class SignalHandlerInfo {
    protected:
        struct sigaction oldAction_;
        word name_;
        int sig_;
        bool set_;
    public:
        SignalHandlerInfo(
            word name,
            int sig
        );

        SignalHandlerInfo()
            :
            name_("nix"),
            sig_(-1),
            set_(false)
            {}

        void resetHandler();

        ~SignalHandlerInfo() {};
    };

    static void sigHandler(int);

    static writeOldTimesOnSignalFunctionObject *singleton_;

    DynamicList<SignalHandlerInfo> handlers_;

    bool writeCurrent_;

    bool sigFPE_;
    bool sigSEGV_;
    bool sigINT_;
    bool sigQUIT_;

public:

    //- Runtime type information
    TypeName("writeOldTimesOnSignal");


    // Constructors

    //- Construct from components
    writeOldTimesOnSignalFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
#ifdef FOAM_FUNCTIONOBJECT_EXECUTE_HAS_NO_FORCE
    virtual bool execute();
#else
    virtual bool execute(const bool forceWrite);
#endif

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

#ifdef FOAM_FUNCTIONOBJECT_NEEDS_UPDATEMESH

    virtual void updateMesh(const mapPolyMesh&) {
    }

    virtual void movePoints(const polyMesh&) {
    }
#endif

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
