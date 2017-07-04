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
    patchExpressionDistributionFunctionObject

Description
    Distribution of an expression on a patch

SourceFiles
    patchExpressionDistributionFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef patchExpressionDistributionFunctionObject_H
#define patchExpressionDistributionFunctionObject_H

#include "distributionFunctionObject.H"
#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class patchExpressionDistributionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class patchExpressionDistributionFunctionObject
:
    public distributionFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    patchExpressionDistributionFunctionObject(const patchExpressionDistributionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const patchExpressionDistributionFunctionObject&);

    wordList patchNames_;

    labelList patchIDs_;

    template <typename T>
    void getDistributionInternal(autoPtr<SimpleDistribution<T> > &dist);
protected:

    word dirName();

    virtual word baseName();

    virtual void getDistribution();

    //- the expression that is to be evaluated
    exprString expression_;

    //- the specification
    dictionary data_;

    PtrList<PatchValueExpressionDriver> drivers_;

public:

    //- Runtime type information
    TypeName("patchExpressionDistribution");


    // Constructors

    //- Construct from components
    patchExpressionDistributionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "patchExpressionDistributionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
