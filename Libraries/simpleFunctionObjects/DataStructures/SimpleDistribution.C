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

Contributors/Copyright:
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SimpleDistribution.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
SimpleDistribution<Type>::SimpleDistribution()
:
    Distribution<Type>()
{
}

template<class Type>
SimpleDistribution<Type>::SimpleDistribution(const scalar binSize)
:
    Distribution<Type>(
        pTraits<Type>::one*binSize
    )
{
    if(binSize<SMALL) {
        FatalErrorIn("SimpleDistribution<Type>::SimpleDistribution(const scalar binSize)")
            << "Specified bin size " << binSize << " is smaller than " << SMALL
                << endl
                << exit(FatalError);
    }
}

template<class Type>
Type SimpleDistribution<Type>::smaller10Power(const Type value)
{
    Type result=value;
    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        const scalar v=component(value,i);
        if(v>SMALL) {
            setComponent(result,i)=
                pow(
                    10,
                    floor(
                        log10(v)
                    )
                );
        } else {
            setComponent(result,i)=SMALL;
        }
    }

    return result;
}

template<class Type>
SimpleDistribution<Type>::SimpleDistribution(
    const Type mini,
    const Type maxi,
    const label binNr
)
:
    Distribution<Type>(
        Foam::max(
            smaller10Power(
                (maxi-mini)/binNr
            ),
            // avoid overflow if span=0 but mini/maxi is big
            1e3*pTraits<Type>::one*(
                Foam::max(
                    mag(maxi),mag(mini)
                )/scalar(pTraits<label>::max)
            )
        )
    )
{
}

template<class Type>
SimpleDistribution<Type>::SimpleDistribution(const Distribution<Type> &o)
:
    Distribution<Type>(o)
{
    recalcLimits();
}

template<class Type>
SimpleDistribution<Type>::SimpleDistribution(const SimpleDistribution<Type> &o)
:
    Distribution<Type>(o)
{
    recalcLimits();
}

template <typename Type>
label SimpleDistribution<Type>::maxNrBins() const
{
    label maxBin=0;
    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        Pair<label> lim=this->validLimits(i);
        label spread=lim.second()-lim.first();
        if(spread>maxBin) {
            maxBin=spread;
        }
    }

    return maxBin;
}

template <typename Type>
Type SimpleDistribution<Type>::min() const
{
    Type val(pTraits<Type>::zero);
    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        label index=this->validLimits(i).first();
        label key=this->keys(i)[index];
        setComponent(val,i)=component(this->binWidth(),i)*key;
    }

    return val;
}

template <typename Type>
Type SimpleDistribution<Type>::max() const
{
    Type val(pTraits<Type>::zero);
    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        label index=this->validLimits(i).second();
        label key=this->keys(i)[index];
        setComponent(val,i)=component(this->binWidth(),i)*(key+1);
    }

    return val;
}

template<class Type>
void SimpleDistribution<Type>::calcScalarWeight(
    const Field<Type> &values,
    const Field<scalar> &weights
)
{
    if(values.size()!=weights.size()) {
        FatalErrorIn("SimpleDistribution::calcScalarWeight")
            << "Size " << values.size() << " of the values and size "
                << weights.size() << " of the weights differ"
                << endl
                << exit(FatalError);
    }

    this->clear();
    forAll(values,i) {
        this->add(
            values[i],
            pTraits<Type>::one*weights[i]
        );
    }

    reduce(*this,plusOp<SimpleDistribution<Type> >());

    recalcLimits();
}

template <typename Type>
void SimpleDistribution<Type>::recalcLimits()
{
    validLimits_=List<Pair<label> >(
        pTraits<Type>::nComponents,
        Pair<label>(-1,-1)
    );

    for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
    {
        const List<scalar> &vals=(*this)[cmpt];
        Pair<label> &limits=validLimits_[cmpt];
        forAll(vals,i) {
            if(mag(vals[i])>VSMALL) {
                if (limits.first() == -1)
                {
                    limits.first() = i;
                    limits.second() = i;
                }
                else
                {
                    limits.second() = i;
                }
            }
        }
    }
}

template<class Type>
void SimpleDistribution<Type>::calcScalarWeight(
    const Field<Type> &values,
    const Field<scalar> &weights,
    const Field<bool> &mask
)
{
    if(
        values.size()!=weights.size()
        ||
        values.size()!=mask.size()
    ) {
        FatalErrorIn("SimpleDistribution::calcScalarWeight - with mask")
            << "Size " << values.size() << " of the values and size "
                << weights.size() << " of the weights or the size of the mask "
                << mask.size() << " differ"
                << endl
                << exit(FatalError);
    }

    this->clear();
    forAll(values,i) {
        if(mask[i]) {
            this->add(
                values[i],
                pTraits<Type>::one*weights[i]
            );
        }
    }

    reduce(*this,plusOp<SimpleDistribution<Type> >());

    recalcLimits();
}

template<class Type>
void SimpleDistribution<Type>::calc(
    const Field<Type> &values,
    const Field<Type> &weights
)
{
    if(values.size()!=weights.size()) {
        FatalErrorIn("SimpleDistribution::calc")
            << "Size " << values.size() << " of the values and size "
                << weights.size() << " of the weights differ"
                << endl
                << exit(FatalError);
    }

    this->clear();
    forAll(values,i) {
        this->add(
            values[i],
            weights[i]
        );
    }

    reduce(*this,plusOp<SimpleDistribution<Type> >());

    recalcLimits();
}

template<class Type>
void SimpleDistribution<Type>::calc(
    const Field<Type> &values,
    const Field<Type> &weights,
    const Field<bool> &mask
)
{
    if(
        values.size()!=weights.size()
        ||
        values.size()!=mask.size()
    ) {
        FatalErrorIn("SimpleDistribution::calc - with mask")
            << "Size " << values.size() << " of the values and size "
                << weights.size() << " of the weights or the size of the mask "
                << mask.size() << " differ"
                << endl
                << exit(FatalError);
    }

    this->clear();
    forAll(values,i) {
        if(mask[i]) {
            this->add(
                values[i],
                weights[i]
            );
        }
    }

    reduce(*this,plusOp<SimpleDistribution<Type> >());

    recalcLimits();
}

template<class Type>
void SimpleDistribution<Type>::divideByDistribution(
    const SimpleDistribution<Type> &weightSum,
    const Type &valueIfZero
)
{
    if(this->size()!=weightSum.size()) {
        FatalErrorIn("SimpleDistribution<Type>::divideByDistribution")
            << "Number of components " << this->size() << " differs from "
                << "number of components in divisor " << weightSum.size()
                << endl
                << exit(FatalError);
    }

    validLimits_=List<Pair<label> >(
        pTraits<Type>::nComponents,
        Pair<label>(-1,-1)
    );

    for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
    {
        List<scalar> &vals=(*this)[cmpt];
        const List<scalar> &weights=weightSum[cmpt];
        const scalar zero=component(valueIfZero,cmpt);
        Pair<label> &limits=validLimits_[cmpt];

        if(vals.size()!=weights.size()) {
            FatalErrorIn("SimpleDistribution<Type>::divideByDistribution")
                << "For component " << cmpt << " the size " << vals.size()
                    << " of the data and " << weights.size() << " of the weights "
                    << "differ"
                    << endl
                    << exit(FatalError);
        }

        forAll(vals,i) {
            if(mag(weights[i])<VSMALL) {
                vals[i]=zero;
            } else {
                vals[i]/=weights[i];
                if (limits.first() == -1)
                {
                    limits.first() = i;
                    limits.second() = i;
                }
                else
                {
                    limits.second() = i;
                }
            }
        }
    }
}

template<class Type>
Pair<label> SimpleDistribution<Type>::validLimits
(
    direction cmpt
) const
{
    if(validLimits_.size()<=label(cmpt)) {
        return Distribution<Type>::validLimits(cmpt);
    } else {
        return validLimits_[cmpt];
    }
}

template<class Type>
void SimpleDistribution<Type>::operator=(const SimpleDistribution<Type>&other)
{
    Distribution<Type>::operator=(other);

    recalcLimits();
}

    // generalization of the median-method from Distribution
template<class Type>
Type SimpleDistribution<Type>::quantile(scalar frac) const
{
    Type quantileValue(pTraits<Type>::zero);

    List< List < Pair<scalar> > > normDistribution = this->normalised();

    for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
    {
        List< Pair<scalar> >& normDist = normDistribution[cmpt];

        if (normDist.size())
        {
            if (normDist.size() == 1)
            {
                setComponent(quantileValue, cmpt) = normDist[0].first();
            }
            else if
            (
                normDist.size() > 1
                && normDist[0].second()*component(this->binWidth(), cmpt) > frac
            )
            {
                scalar xk =
                    normDist[0].first()
                  + 0.5*component(this->binWidth(), cmpt);

                scalar xkm1 =
                    normDist[0].first()
                  - 0.5*component(this->binWidth(), cmpt);

                scalar Sk = (normDist[0].second())*component(this->binWidth(), cmpt);

                setComponent(quantileValue, cmpt) = frac*(xk - xkm1)/(Sk) + xkm1;
            }
            else
            {
                label previousNonZeroIndex = 0;

                scalar cumulative = 0.0;
                bool done=false;

                forAll(normDist, nD)
                {
                    if
                    (
                        cumulative
                      + (normDist[nD].second()*component(this->binWidth(), cmpt))
                      > frac
                    )
                    {
                        scalar xk =
                            normDist[nD].first()
                          + 0.5*component(this->binWidth(), cmpt);

                        scalar xkm1 =
                            normDist[previousNonZeroIndex].first()
                          + 0.5*component(this->binWidth(), cmpt);

                        scalar Sk =
                            cumulative
                          + (normDist[nD].second()*component(this->binWidth(), cmpt));

                        scalar Skm1 = cumulative;

                        setComponent(quantileValue, cmpt) =
                            (frac - Skm1)*(xk - xkm1)/(Sk - Skm1) + xkm1;

                        done=true;
                        break;
                    }
                    else if (mag(normDist[nD].second()) > VSMALL)
                    {
                        cumulative +=
                            normDist[nD].second()*component(this->binWidth(), cmpt);

                        previousNonZeroIndex = nD;
                    }
                }
                if(!done) {
                    // the quantile is in the last bin or on its edge
                    scalar xk =
                        normDist[normDist.size()-1].first()
                        + 0.5*component(this->binWidth(), cmpt);

                    scalar xkm1 =
                        normDist[previousNonZeroIndex].first()
                        - 0.5*component(this->binWidth(), cmpt);

                    scalar Sk =
                        cumulative
                        + (
                            normDist[normDist.size()-1].second()
                            *
                            component(this->binWidth(), cmpt)
                        );

                    scalar Skm1 = cumulative;

                    setComponent(quantileValue, cmpt) =
                        (frac - Skm1)*(xk - xkm1)/(Sk - Skm1) + xkm1;
                }
            }
        }

    }

    return quantileValue;
}

template<class Type>
Type SimpleDistribution<Type>::smaller(scalar value) const
{
    Type smallerValue(-1*pTraits<Type>::one);

    List< List < Pair<scalar> > > normDistribution = this->normalised();

    for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
    {
        List< Pair<scalar> >& normDist = normDistribution[cmpt];

        if (normDist.size())
        {
            scalar firstValue=
                normDist[0].first()
                -0.5*component(this->binWidth(), cmpt);
            scalar lastValue=
                normDist[normDist.size()-1].first()
                +0.5*component(this->binWidth(), cmpt);

            if( value < firstValue ) {
                setComponent(smallerValue, cmpt) = 0;
            } else if( value > lastValue ) {
                setComponent(smallerValue, cmpt) = 1;
            }
            else if (normDist.size() == 1)
            {
                // fraction of the only bin
                setComponent(smallerValue, cmpt) =
                    (value-firstValue)
                    /
                    (component(this->binWidth(), cmpt)+SMALL);
            }
            else if
            (
                normDist.size() > 1
                && (firstValue+component(this->binWidth(), cmpt))>value
            )
            {
                // fraction of the bin
                setComponent(smallerValue, cmpt) =
                    normDist[0].second()
                    *
                    (value-firstValue);
            }
            else
            {
                scalar cumulative = 0.0;
                bool done=false;

                forAll(normDist, nD)
                {
                    if
                    (
                        value
                        <
                        (
                            normDist[nD].first()
                            +
                            0.5*component(this->binWidth(), cmpt)
                        )
                    )
                    {
                        setComponent(smallerValue, cmpt) =
                            cumulative
                            +
                            normDist[nD].second()
                            *
                            (
                                value
                                -
                                (
                                    normDist[nD].first()
                                    -
                                    0.5*component(this->binWidth(), cmpt)
                                )
                            );
                        done=true;
                        break;
                    }
                    else if (mag(normDist[nD].second()) > VSMALL)
                    {
                        cumulative +=
                            normDist[nD].second()*component(this->binWidth(), cmpt);
                    }
                }
                // shouldn't be needed. Just in case
                if(!done) {
                    setComponent(smallerValue, cmpt) = 1;
                }
            }
        }

    }

    return smallerValue;
}


template<class Type>
List< List < Pair<scalar> > > SimpleDistribution<Type>::rawNegative() const
{
    const_cast<SimpleDistribution<Type>&>(*this).recalcLimits();

    // Copy paste from Distribution::raw to use our validLimits
    List< List < Pair<scalar> > > rawDistribution(pTraits<Type>::nComponents);

    for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
    {
        const List<scalar>& cmptDistribution = (*this)[cmpt];

        if (cmptDistribution.empty())
        {
            continue;
        }

        List<label> cmptKeys = this->keys(cmpt);

        List< Pair<scalar> >& rawDist = rawDistribution[cmpt];

        Pair<label> limits = validLimits(cmpt);

        rawDist.setSize(limits.second() - limits.first() + 1);

        for
        (
            label k = limits.first(), i = 0;
            k <= limits.second();
            k++, i++
        )
        {
            label key = cmptKeys[k];

            rawDist[i].first() = (0.5 + scalar(key))*component(
                this->binWidth(), cmpt
            );

            rawDist[i].second() = cmptDistribution[k];
        }
    }

    return rawDistribution;
}

template<class Type>
void SimpleDistribution<Type>::writeRaw(const fileName& filePrefix) const
{
    List< List < Pair<scalar> > > rawDistribution = rawNegative();

    for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
    {
        const List< Pair<scalar> >& rawPairs = rawDistribution[cmpt];

        OFstream os(filePrefix + '_' + pTraits<Type>::componentNames[cmpt]);

        os  << "# key raw" << endl;

        forAll(rawPairs, i)
        {
            os  << rawPairs[i].first()
                << ' ' << rawPairs[i].second()
                << nl;
        }
    }
}

template<class Type>
Istream& operator>>
(
    Istream& is,
    SimpleDistribution<Type>& d
) {
    is  >>  static_cast<Distribution<Type> &>(d);
    d.recalcLimits();
    return is;
}

template<class Type>
Ostream& operator<<
(
    Ostream& os,
    const SimpleDistribution<Type>& d
)
{
    os  <<  static_cast<const Distribution<Type> &>(d);
    return os;
}

    // Copy/paste from Distribution.C with two changed lines
template<class Type>
SimpleDistribution<Type> operator+
(
    const SimpleDistribution<Type>& d1,
    const SimpleDistribution<Type>& d2
)
{
    // The coarsest binWidth is the sensible choice
    SimpleDistribution<Type> d(max(d1.binWidth(), d2.binWidth()));

    List< List< List < Pair<scalar> > > > rawDists(2);

    // the changed lines
    rawDists[0] = d1.rawNegative();
    rawDists[1] = d2.rawNegative();

    forAll(rawDists, rDI)
    {
        for (direction cmpt = 0; cmpt < pTraits<Type>::nComponents; cmpt++)
        {
            List<scalar>& cmptSimpleDistribution = d[cmpt];

            const List < Pair<scalar> >& cmptRaw = rawDists[rDI][cmpt];

            forAll(cmptRaw, rI)
            {
                scalar valueToAdd = cmptRaw[rI].first();
                scalar cmptWeight = cmptRaw[rI].second();

                label n =
                label
                (
                    component(valueToAdd, cmpt)
                   /component(d.binWidth(), cmpt)
                )
                - label
                (
                    neg(component(valueToAdd, cmpt)
                   /component(d.binWidth(), cmpt))
                );

                label listIndex = d.index(cmpt, n);

                cmptSimpleDistribution[listIndex] += cmptWeight;
            }
        }
    }

    d.recalcLimits();

    return d;
}


}

// ************************************************************************* //
