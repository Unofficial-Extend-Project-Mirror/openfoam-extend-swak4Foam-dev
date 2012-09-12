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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "LagrangianCloudSourcePluginFunction.H"
#include "FieldValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class CloudType>
LagrangianCloudSourcePluginFunction<CloudType>::LagrangianCloudSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnValueType,
    const string &additionalArgs
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        returnValueType,
        string("cloudName primitive word")+ (
            additionalArgs==""
            ?
            additionalArgs
            :
            string(string(",")+additionalArgs)
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class CloudType>
void LagrangianCloudSourcePluginFunction<CloudType>::setArgument(
    label index,
    const word &name
)
{
    assert(index==0);
    cloudName_=name;
}


template <class CloudType>
template<class LookupCloudType>
bool LagrangianCloudSourcePluginFunction<CloudType>::hasCloudAs()
{
    return mesh().template foundObject<LookupCloudType>
        (
            cloudName()
        );
}

template <class CloudType>
template<class OtherCloudType,class LookupCloudType>
const OtherCloudType &LagrangianCloudSourcePluginFunction<CloudType>::getCloudAs()
{
    return
        dynamicCast<const OtherCloudType &>(
            mesh().template lookupObject<LookupCloudType>
            (
                cloudName()
            )
        );
}



// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
