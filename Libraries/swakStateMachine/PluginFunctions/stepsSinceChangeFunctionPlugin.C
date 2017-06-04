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

#include "stepsSinceChangeFunctionPlugin.H"

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
typedef stepsSinceChangePluginFunction<FieldValuePluginFunction> stepsSinceChange4Field;
defineTemplateTypeNameAndDebug(stepsSinceChange4Field,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, stepsSinceChange4Field , name, stateMachine_stepsSinceChange);

    // for patches
typedef stepsSinceChangePluginFunction<PatchValuePluginFunction> stepsSinceChange4Patch;
defineTemplateTypeNameAndDebug(stepsSinceChange4Patch,0);
addNamedToRunTimeSelectionTable(PatchValuePluginFunction, stepsSinceChange4Patch , name, stateMachine_stepsSinceChange);

    // for cellSets
typedef stepsSinceChangePluginFunction<CellSetValuePluginFunction> stepsSinceChange4CellSet;
defineTemplateTypeNameAndDebug(stepsSinceChange4CellSet,0);
addNamedToRunTimeSelectionTable(CellSetValuePluginFunction, stepsSinceChange4CellSet , name, stateMachine_stepsSinceChange);

    // for faceSets
typedef stepsSinceChangePluginFunction<FaceSetValuePluginFunction> stepsSinceChange4FaceSet;
defineTemplateTypeNameAndDebug(stepsSinceChange4FaceSet,0);
addNamedToRunTimeSelectionTable(FaceSetValuePluginFunction, stepsSinceChange4FaceSet , name, stateMachine_stepsSinceChange);

    // for cellZones
typedef stepsSinceChangePluginFunction<CellZoneValuePluginFunction> stepsSinceChange4CellZone;
defineTemplateTypeNameAndDebug(stepsSinceChange4CellZone,0);
addNamedToRunTimeSelectionTable(CellZoneValuePluginFunction, stepsSinceChange4CellZone , name, stateMachine_stepsSinceChange);

    // for sampledSets
typedef stepsSinceChangePluginFunction<SampledSetValuePluginFunction> stepsSinceChange4SampledSet;
defineTemplateTypeNameAndDebug(stepsSinceChange4SampledSet,0);
addNamedToRunTimeSelectionTable(SampledSetValuePluginFunction, stepsSinceChange4SampledSet , name, stateMachine_stepsSinceChange);

    // for sampledSurfaces
typedef stepsSinceChangePluginFunction<SampledSurfaceValuePluginFunction> stepsSinceChange4SampledSurface;
defineTemplateTypeNameAndDebug(stepsSinceChange4SampledSurface,0);
addNamedToRunTimeSelectionTable(SampledSurfaceValuePluginFunction, stepsSinceChange4SampledSurface , name, stateMachine_stepsSinceChange);

#ifdef FOAM_DEV

    // for faField
typedef stepsSinceChangePluginFunction<FaFieldValuePluginFunction> stepsSinceChange4FaField;
defineTemplateTypeNameAndDebug(stepsSinceChange4FaField,0);
addNamedToRunTimeSelectionTable(FaFieldValuePluginFunction, stepsSinceChange4FaField , name, stateMachine_stepsSinceChange);


    // for faPatch
typedef stepsSinceChangePluginFunction<FaPatchValuePluginFunction> stepsSinceChange4FaPatch;
defineTemplateTypeNameAndDebug(stepsSinceChange4FaPatch,0);
addNamedToRunTimeSelectionTable(FaPatchValuePluginFunction, stepsSinceChange4FaPatch , name, stateMachine_stepsSinceChange);

#endif

    // for clouds
typedef stepsSinceChangePluginFunction<CloudValuePluginFunction> stepsSinceChange4Cloud;
defineTemplateTypeNameAndDebug(stepsSinceChange4Cloud,0);
addNamedToRunTimeSelectionTable(CloudValuePluginFunction, stepsSinceChange4Cloud , name, stateMachine_stepsSinceChange);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class PluginType>
stepsSinceChangePluginFunction<PluginType>::stepsSinceChangePluginFunction(
    const typename PluginType::driverType &parentDriver,
    const word &name
):
    PluginType(
        parentDriver,
        name,
        "volScalarField",
        string(
            "machineName primitive word"
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
void stepsSinceChangePluginFunction<PluginType>::setArgument(
    label index,
    const word &value
) {
    assert(index==0);

    machineName_=value;
}


template <class PluginType>
void stepsSinceChangePluginFunction<PluginType>::doEvaluation()
{
    scalar elapsed=StateMachine::machine(machineName_).stepsSinceChange();

    this->result().setResult(
        elapsed,
        this->parentDriver().size()
    );
}

template <>
void stepsSinceChangePluginFunction<FieldValuePluginFunction>::doEvaluation()
{
    scalar elapsed=StateMachine::machine(machineName_).stepsSinceChange();

    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "stepsSinceChange_",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensionedScalar(
                "stepsSinceChange",
                dimless,
                elapsed
            )
        )
    );

    result().setObjectResult(pResult);
}

#ifdef FOAM_DEV
template <>
void stepsSinceChangePluginFunction<FaFieldValuePluginFunction>::doEvaluation() {
    scalar elapsed=StateMachine::machine(machineName_).stepsSinceChange();

    autoPtr<areaScalarField> pResult(
        new areaScalarField(
            IOobject(
                "stepsSinceChange_",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            FaCommonValueExpressionDriver::faRegionMesh(this->mesh()),
            dimensionedScalar(
                "stepsSinceChange",
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
