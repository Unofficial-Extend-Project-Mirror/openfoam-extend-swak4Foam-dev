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
    Foam::NumericAccumulationNamedEnum

Description

SourceFiles
    NumericAccumulationNamedEnum.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
