/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           |
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
    stateMachineFvSolutionFvSchemesFunctionObject

Description
    Set the fvSolution and fvSchemes depending on the state of a state machine

    The entries to be set are in sub-dictionaries of the original files

    States that are not in the mapping mean that the default values are used

SourceFiles
    stateMachineFvSolutionFvSchemesFunctionObject.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef stateMachineFvSolutionFvSchemesFunctionObject_H
#define stateMachineFvSolutionFvSchemesFunctionObject_H

#include "manipulateFvSolutionFvSchemesFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class stateMachineFvSolutionFvSchemesFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class stateMachineFvSolutionFvSchemesFunctionObject
:
    public manipulateFvSolutionFvSchemesFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    stateMachineFvSolutionFvSchemesFunctionObject(const stateMachineFvSolutionFvSchemesFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const stateMachineFvSolutionFvSchemesFunctionObject&);

    typedef HashTable<word,word> StateToMap;

    const word solutionStateMachineName_;
    const StateToMap stateToSolutionMapping_;
    word lastSolutionState_;

    const word schemesStateMachineName_;
    const StateToMap stateToSchemesMapping_;
    word lastSchemesState_;

    bool resetBeforeTrigger_;

    void checkTriggerMapping(
        const word &machine,
        const StateToMap &mapping,
        const dictionary &dict
    );

    bool triggerMachine(
        const Time &t,
        const word &machine,
        const StateToMap &mapping,
        dictionary &dict,
        word &current
    );
public:

    //- Runtime type information
    TypeName("stateMachineFvSolutionFvSchemes");


    // Constructors

    //- Construct from components
    stateMachineFvSolutionFvSchemesFunctionObject
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
