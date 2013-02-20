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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "NumericAccumulationNamedEnum.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<>
const char* NamedEnum
<
    NumericAccumulationNamedEnum::value,
    6
>::names[] =
{
    "min",
    "max",
    "average",
    "weightedAverage",
    "sum",
    "sumMag"
};

const NamedEnum<NumericAccumulationNamedEnum::value, 6>
    NumericAccumulationNamedEnum::names;


List<NumericAccumulationNamedEnum::value> NumericAccumulationNamedEnum::readAccumulations(const dictionary &dict, const word &name) {
    const wordList aNames(dict.lookup(name));

    List<value> accus(aNames.size());

    forAll(aNames,i) {
        if(!names.found(aNames[i])) {
            // setting it up to fail
            Info << endl << "Problem in " << dict.name()
                << " with the entry " << name << ":" << endl;
        }

        accus[i]=names[aNames[i]];
    }

    return accus;
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
