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

#include "changedToFunctionPlugin.H"

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
typedef changedToPluginFunction<FieldValuePluginFunction> changedTo4Field;
defineTemplateTypeNameAndDebug(changedTo4Field,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, changedTo4Field , name, stateMachine_changedTo);

    // for patches
typedef changedToPluginFunction<PatchValuePluginFunction> changedTo4Patch;
defineTemplateTypeNameAndDebug(changedTo4Patch,0);
addNamedToRunTimeSelectionTable(PatchValuePluginFunction, changedTo4Patch , name, stateMachine_changedTo);

    // for cellSets
typedef changedToPluginFunction<CellSetValuePluginFunction> changedTo4CellSet;
defineTemplateTypeNameAndDebug(changedTo4CellSet,0);
addNamedToRunTimeSelectionTable(CellSetValuePluginFunction, changedTo4CellSet , name, stateMachine_changedTo);

    // for faceSets
typedef changedToPluginFunction<FaceSetValuePluginFunction> changedTo4FaceSet;
defineTemplateTypeNameAndDebug(changedTo4FaceSet,0);
addNamedToRunTimeSelectionTable(FaceSetValuePluginFunction, changedTo4FaceSet , name, stateMachine_changedTo);

    // for cellZones
typedef changedToPluginFunction<CellZoneValuePluginFunction> changedTo4CellZone;
defineTemplateTypeNameAndDebug(changedTo4CellZone,0);
addNamedToRunTimeSelectionTable(CellZoneValuePluginFunction, changedTo4CellZone , name, stateMachine_changedTo);

    // for sampledSets
typedef changedToPluginFunction<SampledSetValuePluginFunction> changedTo4SampledSet;
defineTemplateTypeNameAndDebug(changedTo4SampledSet,0);
addNamedToRunTimeSelectionTable(SampledSetValuePluginFunction, changedTo4SampledSet , name, stateMachine_changedTo);

    // for sampledSurfaces
typedef changedToPluginFunction<SampledSurfaceValuePluginFunction> changedTo4SampledSurface;
defineTemplateTypeNameAndDebug(changedTo4SampledSurface,0);
addNamedToRunTimeSelectionTable(SampledSurfaceValuePluginFunction, changedTo4SampledSurface , name, stateMachine_changedTo);

#ifdef FOAM_DEV

    // for faField
typedef changedToPluginFunction<FaFieldValuePluginFunction> changedTo4FaField;
defineTemplateTypeNameAndDebug(changedTo4FaField,0);
addNamedToRunTimeSelectionTable(FaFieldValuePluginFunction, changedTo4FaField , name, stateMachine_changedTo);


    // for faPatch
typedef changedToPluginFunction<FaPatchValuePluginFunction> changedTo4FaPatch;
defineTemplateTypeNameAndDebug(changedTo4FaPatch,0);
addNamedToRunTimeSelectionTable(FaPatchValuePluginFunction, changedTo4FaPatch , name, stateMachine_changedTo);

#endif

    // for clouds
typedef changedToPluginFunction<CloudValuePluginFunction> changedTo4Cloud;
defineTemplateTypeNameAndDebug(changedTo4Cloud,0);
addNamedToRunTimeSelectionTable(CloudValuePluginFunction, changedTo4Cloud , name, stateMachine_changedTo);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class PluginType>
changedToPluginFunction<PluginType>::changedToPluginFunction(
    const typename PluginType::driverType &parentDriver,
    const word &name
):
    PluginType(
        parentDriver,
        name,
        "volScalarField",
        string(
            "machineName primitive word,state primitive word"
        )
    )
{
    typedef typename PluginType::driverType PluginTypeDriverType;

    if(PluginTypeDriverType::driverName()!="internalField") {
        this->returnType()="scalar";
    }
#ifdef FOAM_DEV
    if(PluginTypeDriverType::driverName()=="internalFaField") {
        this->returnType()="areaScalarField";
    }
#endif
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class PluginType>
void changedToPluginFunction<PluginType>::setArgument(
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
void changedToPluginFunction<PluginType>::doEvaluation()
{
    const StateMachine &m=StateMachine::machine(machineName_);
    scalar elapsed=m.changedTo(m.stateCode(state_));

    this->result().setResult(
        elapsed,
        this->parentDriver().size()
    );
}

template <>
void changedToPluginFunction<FieldValuePluginFunction>::doEvaluation()
{
    const StateMachine &m=StateMachine::machine(machineName_);
    scalar elapsed=m.changedTo(m.stateCode(state_));

    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "changedTo_",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensionedScalar(
                "changedTo",
                dimless,
                elapsed
            )
        )
    );

    result().setObjectResult(pResult);
}

#ifdef FOAM_DEV
template <>
void changedToPluginFunction<FaFieldValuePluginFunction>::doEvaluation() {
    const StateMachine &m=StateMachine::machine(machineName_);
    scalar elapsed=m.changedTo(m.stateCode(state_));

    autoPtr<areaScalarField> pResult(
        new areaScalarField(
            IOobject(
                "changedTo_",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            FaCommonValueExpressionDriver::faRegionMesh(this->mesh()),
            dimensionedScalar(
                "changedTo",
                dimless,
                elapsed
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
