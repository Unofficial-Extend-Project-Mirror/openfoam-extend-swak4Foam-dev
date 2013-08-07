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

Contributors/Copyright:
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "AccumulationCalculation.H"

namespace Foam {

    // defineTemplateTypeNameAndDebug(AccumulationCalculation, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <typename Type>
AccumulationCalculation<Type>::AccumulationCalculation(
    const Field<Type> &data,
    bool isPoint,
    CommonValueExpressionDriver &driver
):
    data_(data),
    isPoint_(isPoint),
    driver_(driver),
    hasMaximum_(false),
    hasMinimum_(false),
    hasAverage_(false),
    hasSum_(false),
    hasSumMag_(false)
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template <typename Type>
AccumulationCalculation<Type>::~AccumulationCalculation()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


template <typename Type>
const scalarField &AccumulationCalculation<Type>::weights()
{
    if(!weights_.valid()) {
        weights_.set(
            new scalarField(
                driver_.weights(
                    data_.size(),
                    isPoint_
                )
            )
        );
    }
    return weights_();
}

template <typename Type>
const SimpleDistribution<Type> &AccumulationCalculation<Type>::distribution()
{
    if(!distribution_.valid()) {
        distribution_.set(
            new SimpleDistribution<Type>(
                this->maximum()-this->minimum(),
                numberOfBins
            )
        );
        distribution_().calcScalarWeight(
            data_,
            oneField()
        );
    }

    return distribution_();
}

template <typename Type>
const SimpleDistribution<Type> &AccumulationCalculation<Type>::weightedDistribution()
{
    if(!weightedDistribution_.valid()) {
        weightedDistribution_.set(
            new SimpleDistribution<Type>(
                this->maximum()-this->minimum(),
                numberOfBins
            )
        );
        weightedDistribution_().calcScalarWeight(
            data_,
            this->weights()
        );
    }

    return weightedDistribution_();
}

template <typename Type>
Type AccumulationCalculation<Type>::maximum()
{
    if(!hasMaximum_) {
        maximum_=gMax(data_);
        hasMaximum_=true;
    }
    return maximum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::minimum()
{
    if(!hasMinimum_) {
        minimum_=gMin(data_);
        hasMinimum_=true;
    }
    return minimum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::average()
{
    if(!hasAverage_) {
        average_=gAverage(data_);
        hasAverage_=true;
    }
    return average_;
}

template <typename Type>
Type AccumulationCalculation<Type>::weightedAverage()
{
    if(!hasWeightedAverage_) {
        const scalar wSum=gSum(weights());
        const Type tSum=gSum(weights()*data_);

        weightedAverage_=tSum/wSum;
        hasWeightedAverage_=true;
    }
    return weightedAverage_;
}

template <typename Type>
Type AccumulationCalculation<Type>::sum()
{
    if(!hasSum_) {
        sum_=gSum(data_);
        hasSum_=true;
    }
    return sum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::sumMag()
{
    if(!hasSumMag_) {
        for(direction i=0;i<pTraits<Type>::nComponents;i++) {
            setComponent(sumMag_,i)=gSumMag(data_.component(i));
        }
        hasSumMag_=true;
    }
    return sumMag_;
}

template <typename Type>
Type AccumulationCalculation<Type>::operator()(
    const NumericAccumulationNamedEnum::accuSpecification &accu
) {
    switch(accu.first()) {
        case NumericAccumulationNamedEnum::numMin:
            return minimum();
            break;
        case NumericAccumulationNamedEnum::numMax:
            return maximum();
            break;
        case NumericAccumulationNamedEnum::numSum:
            return sum();
            break;
        case NumericAccumulationNamedEnum::numAverage:
            return average();
            break;
        case NumericAccumulationNamedEnum::numSumMag:
            return sumMag();
            break;
        case NumericAccumulationNamedEnum::numWeightedAverage:
            return weightedAverage();
            break;
        default:
            WarningIn("AccumulationCalculation<Type>operator()")
                << "Unknown accumultation type "
                    << NumericAccumulationNamedEnum::names[accu.first()]
                // << ". Currently only 'min', 'max', 'sum', 'weightedAverage' and 'average' are supported"
                    << endl;
            return pTraits<Type>::zero;
    }
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
