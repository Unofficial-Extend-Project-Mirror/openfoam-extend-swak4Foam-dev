/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           |
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
    2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "StateMachine.H"

#include "isStateFunctionPlugin.H"

#include "FieldValueExpressionDriver.H"
#include "PatchValueExpressionDriver.H"
#include "CellSetValueExpressionDriver.H"
#include "FaceSetValueExpressionDriver.H"
#include "CellZoneValueExpressionDriver.H"
#include "FaceZoneValueExpressionDriver.H"
#include "SampledSetValueExpressionDriver.H"
#include "SampledSurfaceValueExpressionDriver.H"
#include "CloudValueExpressionDriver.H"

#ifdef FOAM_DEV
#include "FaFieldValueExpressionDriver.H"
#include "FaPatchValueExpressionDriver.H"
#endif

#include "Pstream.H"
#include "addToRunTimeSelectionTable.H"

#include "FieldValuePluginFunction.H"
#include "PatchValuePluginFunction.H"
#include "CellSetValuePluginFunction.H"
#include "FaceSetValuePluginFunction.H"
#include "CellZoneValuePluginFunction.H"
#include "FaceZoneValuePluginFunction.H"
#include "SampledSetValuePluginFunction.H"
#include "SampledSurfaceValuePluginFunction.H"

#ifdef FOAM_DEV
#include "FaFieldValuePluginFunction.H"
#include "FaPatchValuePluginFunction.H"
#endif

#include "CloudValuePluginFunction.H"

namespace Foam {

    // for fields
typedef isStatePluginFunction<FieldValuePluginFunction> isState4Field;
defineTemplateTypeNameAndDebug(isState4Field,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, isState4Field , name, stateMachine_isState);

    // for patches
typedef isStatePluginFunction<PatchValuePluginFunction> isState4Patch;
defineTemplateTypeNameAndDebug(isState4Patch,0);
addNamedToRunTimeSelectionTable(PatchValuePluginFunction, isState4Patch , name, stateMachine_isState);

    // for cellSets
typedef isStatePluginFunction<CellSetValuePluginFunction> isState4CellSet;
defineTemplateTypeNameAndDebug(isState4CellSet,0);
addNamedToRunTimeSelectionTable(CellSetValuePluginFunction, isState4CellSet , name, stateMachine_isState);

    // for faceSets
typedef isStatePluginFunction<FaceSetValuePluginFunction> isState4FaceSet;
defineTemplateTypeNameAndDebug(isState4FaceSet,0);
addNamedToRunTimeSelectionTable(FaceSetValuePluginFunction, isState4FaceSet , name, stateMachine_isState);

    // for cellZones
typedef isStatePluginFunction<CellZoneValuePluginFunction> isState4CellZone;
defineTemplateTypeNameAndDebug(isState4CellZone,0);
addNamedToRunTimeSelectionTable(CellZoneValuePluginFunction, isState4CellZone , name, stateMachine_isState);

    // for sampledSets
typedef isStatePluginFunction<SampledSetValuePluginFunction> isState4SampledSet;
defineTemplateTypeNameAndDebug(isState4SampledSet,0);
addNamedToRunTimeSelectionTable(SampledSetValuePluginFunction, isState4SampledSet , name, stateMachine_isState);

    // for sampledSurfaces
typedef isStatePluginFunction<SampledSurfaceValuePluginFunction> isState4SampledSurface;
defineTemplateTypeNameAndDebug(isState4SampledSurface,0);
addNamedToRunTimeSelectionTable(SampledSurfaceValuePluginFunction, isState4SampledSurface , name, stateMachine_isState);

#ifdef FOAM_DEV

    // for faField
typedef isStatePluginFunction<FaFieldValuePluginFunction> isState4FaField;
defineTemplateTypeNameAndDebug(isState4FaField,0);
addNamedToRunTimeSelectionTable(FaFieldValuePluginFunction, isState4FaField , name, stateMachine_isState);


    // for faPatch
typedef isStatePluginFunction<FaPatchValuePluginFunction> isState4FaPatch;
defineTemplateTypeNameAndDebug(isState4FaPatch,0);
addNamedToRunTimeSelectionTable(FaPatchValuePluginFunction, isState4FaPatch , name, stateMachine_isState);

#endif

    // for clouds
typedef isStatePluginFunction<CloudValuePluginFunction> isState4Cloud;
defineTemplateTypeNameAndDebug(isState4Cloud,0);
addNamedToRunTimeSelectionTable(CloudValuePluginFunction, isState4Cloud , name, stateMachine_isState);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class PluginType>
isStatePluginFunction<PluginType>::isStatePluginFunction(
    const typename PluginType::driverType &parentDriver,
    const word &name
):
    PluginType(
        parentDriver,
        name,
        "volLogicalField",
        string(
            "machineName primitive word,stateName primitive word"
        )
    )
{
    typedef typename PluginType::driverType PluginTypeDriverType;

    if(PluginTypeDriverType::driverName()!="internalField") {
        this->returnType()="bool";
    }
#ifdef FOAM_DEV
    if(PluginTypeDriverType::driverName()=="internalFaField") {
        this->returnType()="areaLogicalField";
    }
#endif
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class PluginType>
void isStatePluginFunction<PluginType>::setArgument(
    label index,
    const word &value
) {
    assert(index==0 || index==1);

    if(index==0) {
        machineName_=value;
    } else {
        state_=value;
    }
}


template <class PluginType>
void isStatePluginFunction<PluginType>::doEvaluation()
{
    bool yep=
        StateMachine::machine(machineName_).stateCode(state_)
        ==
        StateMachine::machine(machineName_).currentState();

    this->result().setResult(
        yep,
        this->parentDriver().size()
    );
}

template <>
void isStatePluginFunction<FieldValuePluginFunction>::doEvaluation()
{
    bool yep=
        StateMachine::machine(machineName_).stateCode(state_)
        ==
        StateMachine::machine(machineName_).currentState();

    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "isState_"+state_,
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensionedScalar(
                "isState",
                dimless,
                yep ? 1. : 0
            )
        )
    );

    result().setObjectResult(pResult);
}

#ifdef FOAM_DEV
template <>
void isStatePluginFunction<FaFieldValuePluginFunction>::doEvaluation()
{
    bool yep=
    StateMachine::machine(machineName_).stateCode(state_)
    ==
    StateMachine::machine(machineName_).currentState();

    autoPtr<areaScalarField> pResult(
        new areaScalarField(
            IOobject(
                "isState_"+state_,
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            FaCommonValueExpressionDriver::faRegionMesh(this->mesh()),
            dimensionedScalar(
                "isState",
                dimless,
                yep ? 1 : 0
            )
        )
    );

    pResult->correctBoundaryConditions();

    result().setObjectResult(pResult);
}
#endif

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
