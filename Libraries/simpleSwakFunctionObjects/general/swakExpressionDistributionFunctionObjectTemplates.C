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
    2008-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakExpressionDistributionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <typename T>
void swakExpressionDistributionFunctionObject::getDistributionInternal(
    autoPtr<SimpleDistribution<T> > &dist,
    autoPtr<Field<T> > &sameWeight
) {
    if(sameWeight.valid()) {
        dist=setData(
            driver_->getResult<T>()(),
            sameWeight(),
            maskValues_()
        );
    } else if(weightValuesScalar_.valid()) {
        dist=setDataScalar(
            driver_->getResult<T>()(),
            weightValuesScalar_(),
            maskValues_()
        );
    } else {
        FatalErrorIn("swakExpressionDistributionFunctionObject::getDistributionInternal")
            << "Weight neither of type " << pTraits<scalar>::typeName
                << " nor " << pTraits<T>::typeName
                << endl
                << "Set weights are: "
                << pTraits<scalar>::typeName << ":"
                << weightValuesScalar_.valid() << " "
                << pTraits<vector>::typeName << ":"
                << weightValuesVector_.valid() << " "
                << pTraits<tensor>::typeName << ":"
                << weightValuesTensor_.valid() << " "
                << pTraits<symmTensor>::typeName << ":"
                << weightValuesSymmTensor_.valid() << " "
                << pTraits<sphericalTensor>::typeName << ":"
                << weightValuesSphericalTensor_.valid() << " "
                << exit(FatalError);

    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
