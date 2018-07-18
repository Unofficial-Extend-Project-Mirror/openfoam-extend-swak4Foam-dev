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
    2008-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakExpressionDistributionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"

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
