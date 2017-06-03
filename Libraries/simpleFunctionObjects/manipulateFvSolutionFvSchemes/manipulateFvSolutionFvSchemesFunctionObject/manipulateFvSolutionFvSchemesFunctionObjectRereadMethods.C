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

Descriptions:
    Encapsulates dirty tricks to access the private read-methods of
    Foam::solution and Foam::fvSchemes

Contributors/Copyright:
    2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/


// not working

#include "swak.H"

#ifdef __clang__

#ifndef FOAM_SOLUTION_HAS_NO_READ_WITH_DICT

#include "IOdictionary.H"
#define class struct
#include "fvSchemes.H"
#undef class

#define cache readPrivate(const dictionary &d); bool cache
#include "solution.H"
#undef debug

bool Foam::solution::readPrivate(const dictionary &d)
{
    read(d);
    return true;
}

#endif

#endif

#include "manipulateFvSolutionFvSchemesFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "fvMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

    // recipe for accessing private methods found at
    // http://bloglitb.blogspot.co.at/2010/07/access-to-private-members-thats-easy.html
    // don't try this at home

    template<typename Tag>
    struct resultPrivate {
        /* export it ... */
        typedef typename Tag::type type;
        static type ptr;
    };

    template<typename Tag>
    typename resultPrivate<Tag>::type resultPrivate<Tag>::ptr;

    template<typename Tag, typename Tag::type p>
    struct robPrivate {
        /* fill it ... */
        robPrivate() { resultPrivate<Tag>::ptr = p; }
        static robPrivate robPrivate_obj;
    };

    template<typename Tag, typename Tag::type p>
    robPrivate<Tag, p> robPrivate<Tag, p>::robPrivate_obj;

#ifndef FOAM_SOLUTION_HAS_NO_READ_WITH_DICT

#ifndef __clang__
    struct ReadFvSolution { typedef void(solution::*type)(const dictionary &); };
    template struct robPrivate<ReadFvSolution, &solution::read>;
#endif

#endif

void manipulateFvSolutionFvSchemesFunctionObject::rereadFvSolution()
{
#ifndef FOAM_SOLUTION_HAS_NO_READ_WITH_DICT
    if(debug) {
        Info << "Rereading " << fvSolution_.name() << endl;
    }
#ifdef __clang__
    fvSolution_.readPrivate(fvSolutionDict());
#else
    (fvSolution_.*resultPrivate<ReadFvSolution>::ptr)(fvSolutionDict());
#endif

#endif
}

#ifndef FOAM_SCHEMES_HAS_NO_READ_WITH_DICT

struct ReadFvSchemes { typedef void(fvSchemes::*type)(const dictionary &); };
template struct robPrivate<ReadFvSchemes, &fvSchemes::read>;

#endif

void manipulateFvSolutionFvSchemesFunctionObject::rereadFvSchemes()
{
#ifndef FOAM_SCHEMES_HAS_NO_READ_WITH_DICT
    if(debug) {
        Info << "Rereading " << fvSchemes_.name() << endl;
    }
    (fvSchemes_.*resultPrivate<ReadFvSchemes>::ptr)(fvSchemesDict());
#endif
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
