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
    swakExpressionAverageDistributionFunctionObject

Description
    Averages of a swak-expression in various directions

SourceFiles
    swakExpressionAverageDistributionFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakExpressionAverageDistributionFunctionObject_H
#define swakExpressionAverageDistributionFunctionObject_H

#include "timelineFunctionObject.H"

#include "CommonValueExpressionDriver.H"

#include "SimpleDistribution.H"

#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class swakExpressionAverageDistributionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class swakExpressionAverageDistributionFunctionObject
:
    public timelineFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    swakExpressionAverageDistributionFunctionObject(const swakExpressionAverageDistributionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const swakExpressionAverageDistributionFunctionObject&);


    //- the expression that is to be evaluated
    exprString expression_;

    //- the expression for the weight
    exprString weightExpression_;

    //- the weight values
    autoPtr<Field<scalar> > weightValues_;

    //- the expression for the mask
    exprString maskExpression_;

    //- the mask values
    autoPtr<Field<bool> > maskValues_;

    //- the expression for the abscissa
    exprString abscissaExpression_;

    //- the expression driver
    autoPtr<CommonValueExpressionDriver> driver_;

    template <typename T>
    void getDistributionInternal(PtrList<SimpleDistribution<T> > &dist);

    template <typename T>
    void writeADistribution(const PtrList<SimpleDistribution<T> > &dist);

    template <typename T>
    void reportADistribution(const PtrList<SimpleDistribution<T> > &dist);

    //- calculate extremes in the distribution from the current abscissa
    bool dynamicExtremesAbscissa_;

protected:

    word dirName();

    virtual word baseName();

    PtrList<SimpleDistribution<scalar> > distScalar_;
    PtrList<SimpleDistribution<vector> > distVector_;
    PtrList<SimpleDistribution<tensor> > distTensor_;
    PtrList<SimpleDistribution<symmTensor> > distSymmTensor_;
    PtrList<SimpleDistribution<sphericalTensor> > distSphericalTensor_;

    wordList componentNames_;

    template<class Type,class AType>
    autoPtr<PtrList<SimpleDistribution<AType> > > setData(
        const Field<AType> &xValues,
        const Field<Type> &values,
        const Field<scalar> &weights
    );

    template<class Type,class AType>
    autoPtr<PtrList<SimpleDistribution<AType> > > setData(
        const Field<AType> &xValues,
        const Field<Type> &values,
        const Field<scalar> &weights,
        const Field<bool> &mask
    );

    void clearDistributions();

    // virtual void getDistribution();

public:

    //- Runtime type information
    TypeName("swakExpressionAverageDistribution");


    // Constructors

    //- Construct from components
    swakExpressionAverageDistributionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //     virtual void flush();

    virtual void writeSimple();

    virtual bool start();

protected:
    virtual stringList columnNames();

    virtual wordList fileNames();

    // virtual stringList componentNames();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "swakExpressionAverageDistributionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
