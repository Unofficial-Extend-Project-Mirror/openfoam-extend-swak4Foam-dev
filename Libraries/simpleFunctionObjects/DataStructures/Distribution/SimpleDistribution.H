/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    SimpleDistribution

Description
    Encapsulates the regular Distribution-class to allow fast usage with fields
    and parallelize

SourceFiles
    SimpleDistribution.C

Contributors/Copyright:
    2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SimpleDistribution_H
#define SimpleDistribution_H

#include "Distribution.H"

#include "Tuple2.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<class Type>
class SimpleDistribution;

template<class Type>
Istream& operator>>(Istream&, SimpleDistribution<Type>&);

template<class Type>
Ostream& operator<<(Ostream&, const SimpleDistribution<Type>&);

template<class Type>
SimpleDistribution<Type> operator+
(
    const SimpleDistribution<Type>&,
    const SimpleDistribution<Type>&
);

/*---------------------------------------------------------------------------*\
                           Class SimpleDistribution Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class SimpleDistribution
:
    protected Distribution<Type>
{
    // Private data

   // Private Member Functions

   //- Disallow default bitwise copy construct
    //    SimpleDistribution(const SimpleDistribution&);

    //- Disallow default bitwise assignment
    //    void operator=(const SimpleDistribution&);

    //- store the valid limits
    List<Pair<label> > validLimits_;

    void recalcLimits();

    bool hasInvalidValue_;

    Type invalidValue_;

    void setInvalidValue(const Type &val);

protected:
    //- Returns the indices of the first and last non-zero entries
    Pair<label> validLimits(direction cmpt) const;

    //- round value down to the next nearst power of 10
    Type smaller10Power(const Type value);

    //- minimum for distributions that can handle it
    List<Field<scalar> > minimum_;
    //- maximum for distributions that can handle it
    List<Field<scalar> > maximum_;
    //- number of samples for distributions that can handle it
    List<Field<label> > nSamples_;

public:

    //- Construct null
    SimpleDistribution();

     //- setting a size
    SimpleDistribution(const scalar binSize);

    //- setting using a span plus the bin number
    SimpleDistribution(const Type min,const Type max,const label binNr);

    //- from a base class
    SimpleDistribution(const Distribution<Type> &);

    //- direct copy
    SimpleDistribution(const SimpleDistribution<Type> &);

    void calc(
        const Field<Type> &values,
        const Field<Type> &weights
    );

    void calc(
        const Field<Type> &values,
        const Field<Type> &weights,
        const Field<bool> &mask
    );

   void calcScalarWeight(
        const Field<Type> &values,
        const Field<scalar> &weights,
        bool doReduce=true
    );

   void calcScalarWeight(
        const Field<Type> &values,
        const Field<scalar> &weights,
        const Field<bool> &mask,
        bool doReduce=true
    );

   void calcMinimumMaximum(
        const Field<Type> &values,
        const Field<scalar> &weights,
        const Field<bool> &mask
    );

   void calcMinimumMaximum(
        const Field<Type> &values,
        const Field<scalar> &weights
    );

    void divideByDistribution(
        const SimpleDistribution<Type> &weightSum,
        const Type &valueIfZero
    );

    //- Return the distribution of the total bin weights - adapted to allow negative weights
    List< List < Pair<scalar> > > rawNegative() const;

    template<class FType>
    List< List < Tuple2<scalar,FType> > > rawField(const List<Field<FType> > &f) const;

    Type mean() const { return Distribution<Type>::mean(); }

    Type median() const { return Distribution<Type>::median(); }

    scalar totalWeight(direction cmpt) const
        { return Distribution<Type>::totalWeight(cmpt); }

    Type min() const;

    Type max() const;

    //- return the value where a fraction of the distribution is smaller
    Type quantile(scalar frac) const;

    //- return the fraction of the distribution that is smaller than the value
    Type smaller(scalar value) const;

    void write(const fileName& filePrefix) const {
        Distribution<Type>::write(filePrefix);
    }

    label maxNrBins() const;

    //- Return the normalised distribution (probability density)
    //  and bins
    List< List<Pair<scalar> > > normalised() const
        { return Distribution<Type>::normalised(); }

    //- Return the distribution of the total bin weights
    List< List < Pair<scalar> > > raw() const
        { return Distribution<Type>::raw(); }

    //- Return the cumulative normalised distribution and
    //  integration locations (at end of bins)
    List< List<Pair<scalar> > > cumulativeNormalised() const
        { return Distribution<Type>::cumulativeNormalised(); }

    //- Return the cumulative total bin weights and integration
    //  locations (at end of bins)
    List< List<Pair<scalar> > > cumulativeRaw() const
        { return Distribution<Type>::cumulativeRaw(); }

    //- Write the distribution to file: only key and raw.
    //  Produces a separate file for each component.
    void writeRaw(const fileName& filePrefix) const;

    // Member Operators

    void operator=(const SimpleDistribution<Type>&);

    friend Ostream& operator<< <Type>
    (
        Ostream&,
        const SimpleDistribution<Type>&
    );

    friend Istream& operator>> <Type>
    (
        Istream& is,
        SimpleDistribution<Type>& d
    );

   friend SimpleDistribution<Type> operator+ <Type>
    (
        const SimpleDistribution<Type>& d1,
        const SimpleDistribution<Type>& d2
    );
};

    //template <typename T>
    //inline bool contiguous<SimpleDistribution<T> >() { return false; }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#ifdef NoRepository
#   include "SimpleDistribution.C"
#endif


#endif

// ************************************************************************* //
