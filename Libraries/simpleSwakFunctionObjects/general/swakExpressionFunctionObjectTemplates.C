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
    2010-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakExpressionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

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
    Field<T> result(driver.getResult<T>());

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
