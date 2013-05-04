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
SimpleDistribution<Type>::SimpleDistribution(const scalar binSize)
:
    Distribution<Type>(
        pTraits<Type>::one*binSize
    )
{
}

template<class Type>
SimpleDistribution<Type>::SimpleDistribution(const Distribution<Type> &o)
:
    Distribution<Type>(o)
{
}

template<class Type>
SimpleDistribution<Type>::SimpleDistribution(const SimpleDistribution<Type> &o)
:
    Distribution<Type>(o)
{
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
void SimpleDistribution<Type>::calc(
    const Field<Type> &values,
    const scalarField &weights
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
            pTraits<Type>::one*weights[i]
        );
    }

    Distribution<Type> &myself=*this;
    reduce(myself,plusOp<Distribution<Type> >());
}

template<class Type>
void SimpleDistribution<Type>::calc(
    const Field<Type> &values,
    const scalarField &weights,
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
                pTraits<Type>::one*weights[i]
            );
        }
    }

    Distribution<Type> &myself=*this;
    reduce(myself,plusOp<Distribution<Type> >());
}

template<class Type>
void SimpleDistribution<Type>::operator=(const SimpleDistribution<Type>&other)
{
    Distribution<Type>::operator=(other);
}

template<class Type>
SimpleDistribution<Type> operator+
(
    const SimpleDistribution<Type>&sa,
    const SimpleDistribution<Type>&sb
)
{
    return
        reinterpret_cast<const Distribution<Type>&>(sa)
        +
        reinterpret_cast<const Distribution<Type>&>(sb);
}

}

// ************************************************************************* //
