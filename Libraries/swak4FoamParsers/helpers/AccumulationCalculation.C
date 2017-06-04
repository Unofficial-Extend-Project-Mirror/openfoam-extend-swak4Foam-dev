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
    2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
    mask_(data_.size(),true),
    isPoint_(isPoint),
    driver_(driver),
    hasWeightSum_(false),
    hasSize_(false),
    hasMaximum_(false),
    hasMinimum_(false),
    hasAverage_(false),
    hasWeightedAverage_(false),
    hasSum_(false),
    hasWeightedSum_(false),
    hasSumMag_(false),
    numberOfBins_(defaultNumberOfBins_)
{
}

template <typename Type>
AccumulationCalculation<Type>::AccumulationCalculation(
    const Field<Type> &data,
    bool isPoint,
    CommonValueExpressionDriver &driver,
    const Field<bool> &mask
):
    data_(data),
    mask_(mask),
    isPoint_(isPoint),
    driver_(driver),
    hasWeightSum_(false),
    hasSize_(false),
    hasMaximum_(false),
    hasMinimum_(false),
    hasAverage_(false),
    hasWeightedAverage_(false),
    hasSum_(false),
    hasWeightedSum_(false),
    hasSumMag_(false),
    numberOfBins_(defaultNumberOfBins_)
{
    if(data_.size()!=mask_.size()) {
        FatalErrorIn("AccumulationCalculation<Type>::AccumulationCalculation")
            << "Sizes of data " << data_.size()
                << " and specified maks " << mask_.size()
                << " differ"
                << endl
                << exit(FatalError);
    }
}

template <typename Type>
AccumulationCalculation<Type>::AccumulationCalculation(
    const Field<Type> &data,
    bool isPoint,
    CommonValueExpressionDriver &driver,
    const Field<scalar> &weight
):
    data_(data),
    mask_(data_.size(),true),
    isPoint_(isPoint),
    driver_(driver),
    weights_(new Field<scalar>(weight)),
    hasWeightSum_(false),
    hasSize_(false),
    hasMaximum_(false),
    hasMinimum_(false),
    hasAverage_(false),
    hasWeightedAverage_(false),
    hasSum_(false),
    hasWeightedSum_(false),
    hasSumMag_(false),
    numberOfBins_(defaultNumberOfBins_)
{
    if(data_.size()!=weights_().size()) {
        FatalErrorIn("AccumulationCalculation<Type>::AccumulationCalculation")
            << "Sizes of data " << data_.size()
                << " and specified weights " << weights_().size()
                << " differ"
                << endl
                << exit(FatalError);
    }
}

template <typename Type>
AccumulationCalculation<Type>::AccumulationCalculation(
    const Field<Type> &data,
    bool isPoint,
    CommonValueExpressionDriver &driver,
    const Field<bool> &mask,
    const Field<scalar> &weight
):
    data_(data),
    mask_(mask),
    isPoint_(isPoint),
    driver_(driver),
    hasWeightSum_(false),
    hasSize_(false),
    hasMaximum_(false),
    hasMinimum_(false),
    hasAverage_(false),
    hasWeightedAverage_(false),
    hasSum_(false),
    hasWeightedSum_(false),
    hasSumMag_(false),
    numberOfBins_(defaultNumberOfBins_)
{
    if(data_.size()!=mask_.size()) {
        FatalErrorIn("AccumulationCalculation<Type>::AccumulationCalculation")
            << "Sizes of data " << data_.size()
                << " and specified maks " << mask_.size()
                << " differ"
                << endl
                << exit(FatalError);
    }
    if(data_.size()!=weight.size()) {
        FatalErrorIn("AccumulationCalculation<Type>::AccumulationCalculation")
            << "Sizes of data " << data_.size()
                << " and specified weights " << weight.size()
                << " differ"
                << endl
                << exit(FatalError);
    }

    weights_.set(
        new scalarField(
            maskSize(),
            0
        )
    );

    label cnt=0;
    forAll(mask_,i) {
        if(mask_[i]) {
            weights_()[cnt]=weight[i];
            cnt++;
        }
    }

    assert(cnt==maskSize());
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template <typename Type>
AccumulationCalculation<Type>::~AccumulationCalculation()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


template <typename Type>
label AccumulationCalculation<Type>::maskSize() const
{
    label size=0;
    forAll(mask_,i) {
        if(mask_[i]) {
            size++;
        }
    }
    return size;
}

template <typename Type>
const scalarField &AccumulationCalculation<Type>::weights()
{
    if(!weights_.valid()) {
        scalarField rawWeight(
            driver_.weights(
                data_.size(),
                isPoint_
            )
        );

        weights_.set(
            new scalarField(
                maskSize(),
                0
            )
        );

        label cnt=0;
        forAll(mask_,i) {
            if(mask_[i]) {
                weights_()[cnt]=rawWeight[i];
                cnt++;
            }
        }

        assert(cnt==maskSize());
    }
    return weights_();
}

template <typename Type>
const Field<Type> &AccumulationCalculation<Type>::data()
{
    if(!usedData_.valid()) {
        usedData_.set(
            new Field<Type>(
                maskSize(),
                pTraits<Type>::zero
            )
        );

        label cnt=0;
        forAll(mask_,i) {
            if(mask_[i]) {
                usedData_()[cnt]=data_[i];
                cnt++;
            }
        }

        assert(cnt==maskSize());
     }

    return usedData_();
}

template <typename Type>
void AccumulationCalculation<Type>::resetNumberOfBins(
    const label newNumberOfBins,
    const scalar binWidth
)
{
    if(binWidth<0) {
        if(newNumberOfBins>0) {
            numberOfBins_=newNumberOfBins;
        }
    } else {
        Type newBins=(this->maximum()-this->minimum())/binWidth;
        numberOfBins_=1;
        for(direction i=0;i<pTraits<Type>::nComponents;i++) {
            label v=component(newBins,i);
            if(v>numberOfBins_) {
                numberOfBins_=v;
            }
        }
        if(
            newNumberOfBins>0
            &&
            numberOfBins_>newNumberOfBins
        ) {
            numberOfBins_=newNumberOfBins;
        }
    }

    distribution_.reset();
}

template <typename Type>
const SimpleDistribution<Type> &AccumulationCalculation<Type>::distribution()
{
    if(!distribution_.valid()) {
        distribution_.set(
            new SimpleDistribution<Type>(
                this->minimum(),
                this->maximum(),
                numberOfBins_
            )
        );
        Field<scalar> oneWeight(data().size(),1);
        distribution_().calcScalarWeight(
            data(),
            oneWeight
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
                this->minimum(),
                this->maximum(),
                this->numberOfBins_
            )
        );
        weightedDistribution_().calcScalarWeight(
            data(),
            this->weights()
        );
    }

    return weightedDistribution_();
}

template <typename Type>
scalar AccumulationCalculation<Type>::weightSum()
{
    if(!hasWeightSum_) {
        weightSum_=gSum(weights());
        hasWeightSum_=true;
    }
    return weightSum_;
}

template <typename Type>
label AccumulationCalculation<Type>::size()
{
    if(!hasSize_) {
        size_=data().size();
        reduce(size_,plusOp<label>());
        hasSize_=true;
    }
    return size_;
}

template <typename Type>
Type AccumulationCalculation<Type>::maximum()
{
    if(!hasMaximum_) {
        if(this->size()>0) {
            maximum_=gMax(data());
        } else {
#ifdef FOAM_PTRAITS_HAS_ROOTMAX
            maximum_=pTraits<Type>::max/pTraits<scalar>::rootMax;
#else
            maximum_=pTraits<Type>::max/sqrt(pTraits<scalar>::max);
#endif
        }
        hasMaximum_=true;
    }
    return maximum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::minimum()
{
    if(!hasMinimum_) {
        if(this->size()>0) {
            minimum_=gMin(data());
        } else {
#ifdef FOAM_PTRAITS_HAS_ROOTMAX
            minimum_=pTraits<Type>::min/pTraits<scalar>::rootMax;
#else
            minimum_=pTraits<Type>::min/sqrt(pTraits<scalar>::max);
#endif
        }
        hasMinimum_=true;
    }
    return minimum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::average()
{
    if(!hasAverage_) {
        average_=gAverage(data());
        hasAverage_=true;
    }
    return average_;
}

template <typename Type>
Type AccumulationCalculation<Type>::weightedAverage()
{
    if(!hasWeightedAverage_) {
        const scalar wSum=gSum(weights());
        const Type tSum=gSum(weights()*data());

        if(mag(wSum)>SMALL) {
            weightedAverage_=tSum/wSum;
        } else {
            weightedAverage_=pTraits<Type>::one*pTraits<scalar>::max;
        }
        hasWeightedAverage_=true;
    }
    return weightedAverage_;
}

template <typename Type>
Type AccumulationCalculation<Type>::sum()
{
    if(!hasSum_) {
        sum_=gSum(data());
        hasSum_=true;
    }
    return sum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::weightedSum()
{
    if(!hasWeightedSum_) {
        weightedSum_=gSum(data()*weights());
        hasWeightedSum_=true;
    }
    return weightedSum_;
}

template <typename Type>
Type AccumulationCalculation<Type>::sumMag()
{
    if(!hasSumMag_) {
        for(direction i=0;i<pTraits<Type>::nComponents;i++) {
            setComponent(sumMag_,i)=gSumMag(data().component(i));
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
        case NumericAccumulationNamedEnum::numQuantile:
        case NumericAccumulationNamedEnum::numWeightedQuantile:
        case NumericAccumulationNamedEnum::numRange:
        case NumericAccumulationNamedEnum::numWeightedRange:
            if(
                accu.second()<0
                ||
                accu.second()>1
            ) {
                WarningIn("AccumulationCalculation<Type>operator()")
                    << "Accumulation " << accu << " does not have parameter "
                        << "in the range [0,1]"
                        << endl;
            }
            break;
        default:
            // others need no range checking
            ;
    }

    switch(accu.first()) {
        case NumericAccumulationNamedEnum::numSize:
            return size()*pTraits<Type>::one;
            break;
        case NumericAccumulationNamedEnum::numWeightSum:
            return weightSum()*pTraits<Type>::one;
            break;
        case NumericAccumulationNamedEnum::numMin:
            return minimum();
            break;
        case NumericAccumulationNamedEnum::numMax:
            return maximum();
            break;
        case NumericAccumulationNamedEnum::numSum:
            return sum();
            break;
        case NumericAccumulationNamedEnum::numWeightedSum:
        case NumericAccumulationNamedEnum::numIntegrate:
            return weightedSum();
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
        case NumericAccumulationNamedEnum::numMedian:
            return distribution().median();
            break;
        case NumericAccumulationNamedEnum::numWeightedMedian:
            return weightedDistribution().median();
            break;
        case NumericAccumulationNamedEnum::numQuantile:
            return distribution().quantile(accu.second());
            break;
        case NumericAccumulationNamedEnum::numWeightedQuantile:
            return weightedDistribution().quantile(accu.second());
            break;
        case NumericAccumulationNamedEnum::numRange:
            return distribution().quantile(0.5*(1+accu.second()))
                -
                distribution().quantile(0.5*(1-accu.second()));
            break;
        case NumericAccumulationNamedEnum::numWeightedRange:
            return weightedDistribution().quantile(0.5*(1+accu.second()))
                -
                weightedDistribution().quantile(0.5*(1-accu.second()));
            break;
        case NumericAccumulationNamedEnum::numSmaller:
            return distribution().smaller(accu.second());
            break;
        case NumericAccumulationNamedEnum::numBigger:
            return pTraits<Type>::one-distribution().smaller(accu.second());
            break;
        case NumericAccumulationNamedEnum::numWeightedSmaller:
            return weightedDistribution().smaller(accu.second());
            break;
        case NumericAccumulationNamedEnum::numWeightedBigger:
            return pTraits<Type>::one-weightedDistribution().smaller(accu.second());
            break;
        default:
            WarningIn("AccumulationCalculation<Type>operator()")
                << "Unimplemented accumultation type "
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
