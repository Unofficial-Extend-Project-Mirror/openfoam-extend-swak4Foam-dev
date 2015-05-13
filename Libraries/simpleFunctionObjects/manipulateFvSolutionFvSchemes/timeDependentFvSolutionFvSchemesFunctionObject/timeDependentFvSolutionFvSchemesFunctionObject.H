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
    timeDependentFvSolutionFvSchemesFunctionObject

Description
    Set the fvSolution and fvSchemes depending on the simulation time

    The entries to be set are in sub-dictionaries of the original files

    The special name 'reset' resets to the original state
SourceFiles
    timeDependentFvSolutionFvSchemesFunctionObject.C

Contributors/Copyright:
    2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef timeDependentFvSolutionFvSchemesFunctionObject_H
#define timeDependentFvSolutionFvSchemesFunctionObject_H

#include "manipulateFvSolutionFvSchemesFunctionObject.H"

#include "Tuple2.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class timeDependentFvSolutionFvSchemesFunctionObject Declaration
\*---------------------------------------------------------------------------*/


class timeDependentFvSolutionFvSchemesFunctionObject
:
    public manipulateFvSolutionFvSchemesFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    timeDependentFvSolutionFvSchemesFunctionObject(const timeDependentFvSolutionFvSchemesFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const timeDependentFvSolutionFvSchemesFunctionObject&);

    typedef Tuple2<scalar,word> TriggerTimeName;

    List<TriggerTimeName> solutionTriggers_;
    label currentSolutionTrigger_;

    List<TriggerTimeName> schemesTriggers_;
    label currentSchemesTrigger_;

    bool resetBeforeTrigger_;

    void checkTriggerList(
        const List<TriggerTimeName> &triggers,
        const dictionary &dict
    );

    bool triggerTrigger(
        const Time &t,
        const List<TriggerTimeName> &triggers,
        dictionary &dict,
        label &current
    );
public:

    //- Runtime type information
    TypeName("timeDependentFvSolutionFvSchemes");


    // Constructors

    //- Construct from components
    timeDependentFvSolutionFvSchemesFunctionObject
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
