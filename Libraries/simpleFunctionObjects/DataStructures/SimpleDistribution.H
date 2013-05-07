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
    SimpleDistribution

Description
    Encapsulates the regular Distribution-class to allow fast usage with fields
    and parallelize

SourceFiles
    SimpleDistribution.C

Contributors/Copyright:
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SimpleDistribution_H
#define SimpleDistribution_H

#include "Distribution.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

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

public:

    //- from a dictionary
    SimpleDistribution(const scalar binSize);

    //- from a base class
    SimpleDistribution(const Distribution<Type> &);

    //- direct copy
    SimpleDistribution(const SimpleDistribution<Type> &);

    void calc(
        const Field<Type> &values,
        const scalarField &weights
    );

   void calc(
        const Field<Type> &values,
        const scalarField &weights,
        const Field<bool> &mask
    );

    Type mean() const { return Distribution<Type>::mean(); }

    Type median() const { return Distribution<Type>::median(); }

    scalar totalWeight(direction cmpt) const
        { return Distribution<Type>::totalWeight(cmpt); }

    Type min() const;

    Type max() const;

    void write(const fileName& filePrefix) const {
        Distribution<Type>::write(filePrefix);
    }

    label maxNrBins() const;

    // Member Operators

    void operator=(const SimpleDistribution<Type>&);

};

template<class Type>
SimpleDistribution<Type> operator+
(
    const SimpleDistribution<Type>&,
    const SimpleDistribution<Type>&
);


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#ifdef NoRepository
#   include "SimpleDistribution.C"
#endif


#endif

// ************************************************************************* //
