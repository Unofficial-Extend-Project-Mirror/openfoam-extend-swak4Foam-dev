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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::AccumulationCalculation

Description
    Calculates the accumulations for a field. The purpose of this utilitiy
    class is to cache important intermediate results and thus make sure that
    - they are not calculated twice
    - parallel communication is minimized
    Also should it collect the implementation of all possible accumulations
    in one place

SourceFiles
    AccumulationCalculation.C

Contributors/Copyright:
    2013-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef AccumulationCalculation_H
#define AccumulationCalculation_H

#include "NumericAccumulationNamedEnum.H"
#include "CommonValueExpressionDriver.H"
#include "SimpleDistribution.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class AccumulationCalculation Declaration
\*---------------------------------------------------------------------------*/

template <typename Type>
class AccumulationCalculation
{
    // Private data

    // Constructors

    //- Disallow default bitwise assignment
    void operator=(const AccumulationCalculation&);

    //- Construct null
    AccumulationCalculation(const IOobject &);

    //- the data
    const Field<Type> &data_;

    //- mask that selects what part of the data to use
    const Field<bool> mask_;

    //- the data that is actually used
    autoPtr<Field<Type> > usedData_;

    //- is this point data?
    const bool isPoint_;

    //- the driver
    const CommonValueExpressionDriver &driver_;

    //- weights on demand
    autoPtr<scalarField> weights_;

    //- on demand distribution
    autoPtr<SimpleDistribution<Type> > distribution_;

    //- on demand weighted distribution
    autoPtr<SimpleDistribution<Type> > weightedDistribution_;

    bool hasWeightSum_;
    scalar weightSum_;

    bool hasSize_;
    label size_;

    bool hasMaximum_;
    Type maximum_;

    bool hasMinimum_;
    Type minimum_;

    bool hasAverage_;
    Type average_;

    bool hasWeightedAverage_;
    Type weightedAverage_;

    bool hasSum_;
    Type sum_;

    bool hasWeightedSum_;
    Type weightedSum_;

    bool hasSumMag_;
    Type sumMag_;

    static const label defaultNumberOfBins_=100;

    label numberOfBins_;

    label maskSize() const;

protected:

    const scalarField &weights();

    const Field<Type> &data();

public:
     //- Runtime type information
    //    TypeName("AccumulationCalculation");

    //- Constructor
    AccumulationCalculation(
        const Field<Type> &data,
        bool isPoint,
        CommonValueExpressionDriver &driver
    );

     //- Constructor with a mask
    AccumulationCalculation(
        const Field<Type> &data,
        bool isPoint,
        CommonValueExpressionDriver &driver,
        const Field<bool> &mask
    );

    //- Constructor with weights
    AccumulationCalculation(
        const Field<Type> &data,
        bool isPoint,
        CommonValueExpressionDriver &driver,
        const Field<scalar> &weight
    );

    //- Constructor with a mask and weights
    AccumulationCalculation(
        const Field<Type> &data,
        bool isPoint,
        CommonValueExpressionDriver &driver,
        const Field<bool> &mask,
        const Field<scalar> &weight
    );

    //- Destructor

    ~AccumulationCalculation();

    const SimpleDistribution<Type> &distribution();

    const SimpleDistribution<Type> &weightedDistribution();

    label size();
    scalar weightSum();

    Type maximum();
    Type minimum();
    Type average();
    Type weightedAverage();
    Type sum();
    Type sumMag();
    Type weightedSum();

    //- do an accumulation according to a specification
    Type operator()(const NumericAccumulationNamedEnum::accuSpecification &);

    void resetNumberOfBins(
        const label newNumberOfBins,
        const scalar binWidth
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "AccumulationCalculation.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
