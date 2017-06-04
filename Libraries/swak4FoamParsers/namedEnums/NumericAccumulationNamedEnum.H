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
    Foam::NumericAccumulationNamedEnum

Description

SourceFiles
    NumericAccumulationNamedEnum.C

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef NumericAccumulationNamedEnum_H
#define NumericAccumulationNamedEnum_H

#include "NamedEnum.H"
#include "dictionary.H"

#include "Tuple2.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class NumericAccumulationNamedEnum Declaration
\*---------------------------------------------------------------------------*/

class NumericAccumulationNamedEnum
{
    // Private data

public:

    enum value {
        numMin,
        numMax,
        numAverage,
        numWeightedAverage,
        numSum,
        numSumMag,
        numWeightedSum,
        numIntegrate,
        numMedian,
        numWeightedMedian,
        numQuantile,
        numWeightedQuantile,
        numRange,
        numWeightedRange,
        numSmaller,
        numWeightedSmaller,
        numBigger,
        numWeightedBigger,
        numWeightSum,
        numSize
    };

    typedef Tuple2<value,scalar> accuSpecification;

    //- String representation of order enums
    static const NamedEnum<value, 20> names;

    static List<accuSpecification> readAccumulations(
        const dictionary &dict,
        const word &name
    );

    static List<accuSpecification> readAccumulations(
        const wordList &names,
        const fileName &name
    );

    static word toString(const accuSpecification &);
};

Ostream &operator<<(
    Ostream &,
    const NumericAccumulationNamedEnum::accuSpecification &
);
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
