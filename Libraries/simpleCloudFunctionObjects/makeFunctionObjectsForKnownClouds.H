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

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description
    Make instances of a function object for all known intermediate cloud types

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef makeFunctionObjectsForKnownClouds_H
#define makeFunctionObjectsForKnownClouds_H

#include "swakCloudTypes.H"

#include "CloudFunctionObject.H"

#include "basicKinematicCloud.H"
#include "basicThermoCloud.H"
#include "basicReactingCloud.H"
#include "basicReactingMultiphaseCloud.H"

#ifdef FOAM_HAS_MPPIC_CLOUD
#include "basicKinematicMPPICCloud.H"
#define makeCloudFunctionObjectTypeMultiMPPIC(FunctionObjectType)   \
      makeCloudFunctionObjectTypeMulti(FunctionObjectType,basicKinematicMPPICCloud);
#else
#define makeCloudFunctionObjectTypeMultiMPPIC(FunctionObjectType)
#endif

#ifdef FOAM_HAS_COLLIDING_CLOUD
#include "basicKinematicCollidingCloud.H"
#define makeCloudFunctionObjectTypeMultiColliding(FunctionObjectType)   \
    makeCloudFunctionObjectTypeMulti(FunctionObjectType,basicKinematicCollidingCloud);
#else
#define makeCloudFunctionObjectTypeMultiColliding(FunctionObjectType)
#endif

// redefinition of the original because the typedef is not unique
#define makeCloudFunctionObjectTypeMulti(SS, CloudType)             \
                                                                        \
    typedef Foam::CloudType::kinematicCloudType KCT##CloudType;         \
    defineNamedTemplateTypeNameAndDebug(Foam::SS<KCT##CloudType>, 0);   \
                                                                        \
    Foam::CloudFunctionObject<KCT##CloudType>::                         \
    adddictionaryConstructorToTable<Foam::SS<KCT##CloudType> >          \
    add##SS##CloudType##kinematicCloudType##ConstructorToTable_;

#define makeFunctionObjectsForKnownClouds(FunctionObjectType)       \
    makeCloudFunctionObjectTypeMulti(FunctionObjectType,basicKinematicCloud); \
    makeCloudFunctionObjectTypeMultiMPPIC(FunctionObjectType);          \
    makeCloudFunctionObjectTypeMultiColliding(FunctionObjectType);      \
    makeCloudFunctionObjectTypeMulti(FunctionObjectType,basicThermoCloud); \
    makeCloudFunctionObjectTypeMulti(FunctionObjectType,basicReactingCloud); \
    makeCloudFunctionObjectTypeMulti(FunctionObjectType,basicReactingMultiphaseCloud);

#endif
