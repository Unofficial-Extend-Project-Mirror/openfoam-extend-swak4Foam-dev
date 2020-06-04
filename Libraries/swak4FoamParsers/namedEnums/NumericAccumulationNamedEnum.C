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

Contributors/Copyright:
    2012-2013, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017-2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id: NumericAccumulationNamedEnum.C,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
\*---------------------------------------------------------------------------*/

#include "NumericAccumulationNamedEnum.H"
#include "HashSet.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
const Enum<NumericAccumulationNamedEnum::value>
NumericAccumulationNamedEnum::names
({
    {value::numMin,"min"},
    {value::numMax,"max"},
    {value::numAverage,"average"},
    {value::numWeightedAverage,"weightedAverage"},
    {value::numSum,"sum"},
    {value::numSumMag,"sumMag"},
    {value::numWeightedSum,"weightedSum"},
    {value::numIntegrate,"integrate"},
    {value::numMedian,"median"},
    {value::numWeightedMedian,"weightedMedian"},
    {value::numQuantile,"quantile"},
    {value::numWeightedQuantile,"weightedQuantile"},
    {value::numRange,"range"},
    {value::numWeightedRange,"weightedRange"},
    {value::numSmaller,"smaller"},
    {value::numWeightedSmaller,"weightedSmaller"},
    {value::numBigger,"bigger"},
    {value::numWeightedBigger,"weightedBigger"},
    {value::numWeightSum,"weightSum"},
    {value::numSize,"size"},
});
#else
template<>
const char* NamedEnum
<
    NumericAccumulationNamedEnum::value,
    20
>::names[] =
{
    "min",
    "max",
    "average",
    "weightedAverage",
    "sum",         // 5
    "sumMag",
    "weightedSum",
    "integrate",
    "median",
    "weightedMedian",   // 10
    "quantile",
    "weightedQuantile",
    "range",
    "weightedRange",
    "smaller",          // 15
    "weightedSmaller",
    "bigger",
    "weightedBigger",
    "weightSum",
    "size",             // 20
};
const NamedEnum<NumericAccumulationNamedEnum::value, 20>
NumericAccumulationNamedEnum::names;
#endif


List<NumericAccumulationNamedEnum::accuSpecification>
NumericAccumulationNamedEnum::readAccumulations(
    const dictionary &dict,
    const word &name
) {
    const wordList aNames(dict.lookup(name));

    return readAccumulations(
        aNames,
        dict.name()
    );
}

List<NumericAccumulationNamedEnum::accuSpecification>
NumericAccumulationNamedEnum::readAccumulations(
    const wordList &aNames,
    const fileName &name
) {
    HashSet<NumericAccumulationNamedEnum::value, Hash<label> > needsArgument;
    needsArgument.insert(numQuantile);
    needsArgument.insert(numRange);
    needsArgument.insert(numSmaller);
    needsArgument.insert(numBigger);
    needsArgument.insert(numWeightedQuantile);
    needsArgument.insert(numWeightedRange);
    needsArgument.insert(numWeightedSmaller);
    needsArgument.insert(numWeightedBigger);

    List<accuSpecification> accus(aNames.size());

    forAll(aNames,i) {
        scalar value=pTraits<scalar>::min;
        bool hasValue=false;

        string aName;
        // valid starts for a floating point number
        size_t numPos=aNames[i].find_first_of("-.0123456789");
        if(numPos==std::string::npos) {
            aName=aNames[i];
        } else {
            aName=aNames[i].substr(0,numPos);
            value=readScalar(
                IStringStream(aNames[i].substr(numPos))()
            );
            hasValue=true;
        }
        if(!names.found(aName)) {
            // setting it up to fail
            Info << endl << "Problem in " << name
                << " with the entry " << name << ":" << endl;
        }

        accus[i].first()=names[aName];

        if(hasValue) {
            if(!needsArgument.found(accus[i].first())) {
                FatalErrorIn("NumericAccumulationNamedEnum::readAccumulations")
                    << "Problem in " << name << " with " << aNames[i]
                        << endl
                        << "Accumulator " << aName
                        << " does not need an argument" << endl
                        << exit(FatalError);
            }
        } else {
            if(needsArgument.found(accus[i].first())) {
                FatalErrorIn("NumericAccumulationNamedEnum::readAccumulations")
                    << "Problem in " << name << " with " << aNames[i]
                        << endl
                        << "Accumulator " << aName
                        << " needs an argument" << endl
                        << exit(FatalError);
            }
        }

        accus[i].second()=value;
    }
    return accus;
}

word NumericAccumulationNamedEnum::toString(const accuSpecification &accu)
{
    OStringStream o;
    o << NumericAccumulationNamedEnum::names[accu.first()];
    if(accu.second()>pTraits<scalar>::min) {
        o << accu.second();
    }
    return word(o.str());
}

Ostream &operator<<(
    Ostream &o,
    const NumericAccumulationNamedEnum::accuSpecification &accu
)
{
    o << NumericAccumulationNamedEnum::toString(accu);
    return o;
}


// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
