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

Class
    provokeSignalFunctionObject

Description
    Raises a specified signal at a specified time. For parallel runs also
    the processors on which to raise the signal can be specified.

    The only purpose of this function object is the testing of the error
    handling

SourceFiles
    provokeSignalFunctionObject.C

Contributors/Copyright:
    2014, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: provokeSignalFunctionObject.H,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef provokeSignalFunctionObject_H
#define provokeSignalFunctionObject_H

#include "simpleFunctionObject.H"

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
#include "Enum.H"
#else
#include "NamedEnum.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class provokeSignalFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class provokeSignalFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    provokeSignalFunctionObject(const provokeSignalFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const provokeSignalFunctionObject&);

public:

    enum possibleSignals {
        sigFPE,
        sigSEGV,
        sigINT,
        sigTERM,
        sigQUIT,
        sigUSR1,
        sigUSR2,
        FoamFatal
    };

private:

    possibleSignals signalToRaise_;

    scalar timeToRaise_;

    bool raiseOnThisProc_;

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    static const Enum<possibleSignals> possibleSignalsNames_;
#else
    static const NamedEnum<possibleSignals,8> possibleSignalsNames_;
#endif

public:

    //- Runtime type information
    TypeName("provokeSignal");


    // Constructors

    //- Construct from components
    provokeSignalFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
