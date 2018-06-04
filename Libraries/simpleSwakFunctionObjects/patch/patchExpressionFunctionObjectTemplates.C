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
    2010-2011, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "patchExpressionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"

#include "AccumulationCalculation.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class T>
void patchExpressionFunctionObject::writeTheData(const word &pName,PatchValueExpressionDriver &driver)
{
    Field<T> result(driver.getResult<T>());

    AccumulationCalculation<T> calculator(
        result,
        driver.result().isPoint(),
        driver
    );

    Field<T> results(accumulations_.size());

    forAll(accumulations_,i) {
        const NumericAccumulationNamedEnum::accuSpecification accu=
            accumulations_[i];
        T val=calculator(accu);

        results[i]=val;
        if(verbose()) {
            Info << " " << accu
                << "=" << val;
        }
    }

    if (Pstream::master()) {
        writeTime(pName,time().value());
        writeData(pName,results);
        endData(pName);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
