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
    swakExpressionDistributionFunctionObject

Description
    Distribution of a swak-expression

SourceFiles
    swakExpressionDistributionFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakExpressionDistributionFunctionObject_H
#define swakExpressionDistributionFunctionObject_H

#include "distributionFunctionObject.H"

#include "CommonValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class swakExpressionDistributionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class swakExpressionDistributionFunctionObject
:
    public distributionFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    swakExpressionDistributionFunctionObject(const swakExpressionDistributionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const swakExpressionDistributionFunctionObject&);


    //- the expression that is to be evaluated
    exprString expression_;

    //- the expression for the weight
    exprString weightExpression_;

    //- the expression for the mask
    exprString maskExpression_;

    //- the expression driver
    autoPtr<CommonValueExpressionDriver> driver_;

    template <typename T>
    void getDistributionInternal(
        autoPtr<SimpleDistribution<T> > &dist,
        autoPtr<Field<T> > &sameWeight
    );

    autoPtr<Field<scalar> > weightValuesScalar_;
    autoPtr<Field<vector> > weightValuesVector_;
    autoPtr<Field<tensor> > weightValuesTensor_;
    autoPtr<Field<symmTensor> > weightValuesSymmTensor_;
    autoPtr<Field<sphericalTensor> > weightValuesSphericalTensor_;

    autoPtr<Field<bool> > maskValues_;

protected:

    word dirName();

    virtual word baseName();

    virtual void getDistribution();

public:

    //- Runtime type information
    TypeName("swakExpressionDistribution");


    // Constructors

    //- Construct from components
    swakExpressionDistributionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "swakExpressionDistributionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
