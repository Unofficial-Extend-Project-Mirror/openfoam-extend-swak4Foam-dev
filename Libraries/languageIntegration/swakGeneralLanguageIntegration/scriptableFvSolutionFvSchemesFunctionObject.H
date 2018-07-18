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
    scriptableFvSolutionFvSchemesFunctionObject

Description
    Set the fvSolution and fvSchemes depending on the simulation time

    The entries to be set are in sub-dictionaries of the original files

    The special name 'reset' resets to the original state
SourceFiles
    scriptableFvSolutionFvSchemesFunctionObject.C

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef scriptableFvSolutionFvSchemesFunctionObject_H
#define scriptableFvSolutionFvSchemesFunctionObject_H

#include "manipulateFvSolutionFvSchemesFunctionObject.H"

#include "generalInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class scriptableFvSolutionFvSchemesFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class scriptableFvSolutionFvSchemesFunctionObject
:
    public manipulateFvSolutionFvSchemesFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    scriptableFvSolutionFvSchemesFunctionObject(const scriptableFvSolutionFvSchemesFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const scriptableFvSolutionFvSchemesFunctionObject&);

    autoPtr<generalInterpreterWrapper> interpreter_;

    string initCode_;

    word schemesFromDict_;

    word dictFromSchemes_;

    string schemesCode_;

    word solutionFromDict_;

    word dictFromSolution_;

    string solutionCode_;

    bool executeCodeWriteToDict(
        const string code,
        const word &toDict,
        const word &fromDict,
        dictionary &dict
    );
protected:

    generalInterpreterWrapper &interpreter();

public:

    //- Runtime type information
    TypeName("scriptableFvSolutionFvSchemes");


    // Constructors

    //- Construct from components
    scriptableFvSolutionFvSchemesFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool manipulateFvSolution(const Time &t);

    bool manipulateFvSchemes(const Time &t);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
