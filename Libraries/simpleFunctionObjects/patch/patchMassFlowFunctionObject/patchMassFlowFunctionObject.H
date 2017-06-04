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
    patchMassFlowFunctionObject

Description
    Calculates the mass flow on the patches

SourceFiles
    patchMassFlowFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef patchMassFlowFunctionObject_H
#define patchMassFlowFunctionObject_H

#include "patchFunctionObject.H"
#include "SolverInfo.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class patchMassFlowFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class patchMassFlowFunctionObject
:
    public patchFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    patchMassFlowFunctionObject(const patchMassFlowFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const patchMassFlowFunctionObject&);

    SolverInfo solver_;

protected:

    //- Names of the files
    virtual wordList fileNames();

    word dirName();

public:

    //- Runtime type information
    TypeName("patchMassFlow");


    // Constructors

    //- Construct from components
    patchMassFlowFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- Sample and write
    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
