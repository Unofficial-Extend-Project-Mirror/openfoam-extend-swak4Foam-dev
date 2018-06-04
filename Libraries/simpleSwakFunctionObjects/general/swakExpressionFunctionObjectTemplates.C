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
    2010-2011, 2013, 2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakExpressionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"

#include "AccumulationCalculation.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class T>
void swakExpressionFunctionObject::writeTheData(
    CommonValueExpressionDriver &driver
)
{
    Field<T> result(
        driver.getResult<T>(
            driver.result().isPoint()
        )
    );

    autoPtr<AccumulationCalculation<T> > pCalculator;

    if(maskExpression_=="") {
        pCalculator.set(
            new AccumulationCalculation<T>(
                result,
                driver.result().isPoint(),
                driver
            )
        );
    } else {
        bool isPoint=driver.result().isPoint();

        driver.parse(maskExpression_);

        autoPtr<Field<bool> > maskValues;
        if(
            driver.CommonValueExpressionDriver::getResultType()
            ==
            pTraits<bool>::typeName
        ) {
            maskValues.reset(
                driver.getResult<bool>().ptr()
            );
        } else if(
            driver.CommonValueExpressionDriver::getResultType()
            ==
            pTraits<scalar>::typeName
        ) {
            scalarField rawMask(driver.getResult<scalar>());
            maskValues.reset(
                new Field<bool>(rawMask.size(),false)
            );
            forAll(rawMask,i) {
                if(rawMask[i]>SMALL) {
                    maskValues()[i]=true;
                }
            }
        } else {
            FatalErrorIn("swakExpressionFunctionObject::writeTheData")
                << "Don't know how to handle logical expressions of type "
                    << driver.CommonValueExpressionDriver::getResultType()
                    << " from " << maskExpression_
                    << endl
                    << exit(FatalError);
        }
        if(maskValues().size()!=result.size()) {
            FatalErrorIn("swakExpressionFunctionObject::writeTheData")
                << "Size of mask " << maskExpression_ << " not equal to "
                    << expression_ << " (" << maskValues().size() << " vs "
                    << result.size() << ")"
                    << endl
                    << exit(FatalError);
        }

        pCalculator.set(
            new AccumulationCalculation<T>(
                result,
                isPoint,
                driver,
                maskValues
            )
        );
    }

    AccumulationCalculation<T> &calculator=pCalculator();

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
        writeTime(name(),time().value());
        writeData(name(),results);
        endData(name());
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
