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

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "patchExpressionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class T>
void patchExpressionFunctionObject::writeTheData(const word &pName,PatchValueExpressionDriver &driver)
{
    Field<T> result=driver.getResult<T>();

    Field<T> results(accumulations_.size());

    forAll(accumulations_,i) {
        const NumericAccumulationNamedEnum::value accu=accumulations_[i];
        T val=pTraits<T>::zero;

        switch(accu) {
            case NumericAccumulationNamedEnum::numMin:
                val=gMin(result);
                break;
            case NumericAccumulationNamedEnum::numMax:
                val=gMax(result);
                break;
            case NumericAccumulationNamedEnum::numSum:
                val=gSum(result);
                break;
            case NumericAccumulationNamedEnum::numAverage:
                val=gAverage(result);
                break;
            // case NumericAccumulationNamedEnum::numSumMag:
            //     val=gSumMag(result);
            //     break;
            case NumericAccumulationNamedEnum::numWeightedAverage:
                val=driver.calcWeightedAverage(result);
                break;
            default:
                WarningIn("patchExpressionFunctionObject::writeData")
                    << "Unknown accumultation type "
                        << NumericAccumulationNamedEnum::names[accu]
                        << ". Currently only 'min', 'max', 'sum', 'weightedAverage' and 'average' are supported"
                        << endl;
        }
        results[i]=val;
        if(verbose()) {
            Info << " " << NumericAccumulationNamedEnum::names[accu]
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
