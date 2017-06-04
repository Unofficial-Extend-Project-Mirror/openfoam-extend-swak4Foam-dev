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

#include "timeSinceChangeFunctionPlugin.H"

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
typedef timeSinceChangePluginFunction<FieldValuePluginFunction> timeSinceChange4Field;
defineTemplateTypeNameAndDebug(timeSinceChange4Field,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, timeSinceChange4Field , name, stateMachine_timeSinceChange);

    // for patches
typedef timeSinceChangePluginFunction<PatchValuePluginFunction> timeSinceChange4Patch;
defineTemplateTypeNameAndDebug(timeSinceChange4Patch,0);
addNamedToRunTimeSelectionTable(PatchValuePluginFunction, timeSinceChange4Patch , name, stateMachine_timeSinceChange);

    // for cellSets
typedef timeSinceChangePluginFunction<CellSetValuePluginFunction> timeSinceChange4CellSet;
defineTemplateTypeNameAndDebug(timeSinceChange4CellSet,0);
addNamedToRunTimeSelectionTable(CellSetValuePluginFunction, timeSinceChange4CellSet , name, stateMachine_timeSinceChange);

    // for faceSets
typedef timeSinceChangePluginFunction<FaceSetValuePluginFunction> timeSinceChange4FaceSet;
defineTemplateTypeNameAndDebug(timeSinceChange4FaceSet,0);
addNamedToRunTimeSelectionTable(FaceSetValuePluginFunction, timeSinceChange4FaceSet , name, stateMachine_timeSinceChange);

    // for cellZones
typedef timeSinceChangePluginFunction<CellZoneValuePluginFunction> timeSinceChange4CellZone;
defineTemplateTypeNameAndDebug(timeSinceChange4CellZone,0);
addNamedToRunTimeSelectionTable(CellZoneValuePluginFunction, timeSinceChange4CellZone , name, stateMachine_timeSinceChange);

    // for sampledSets
typedef timeSinceChangePluginFunction<SampledSetValuePluginFunction> timeSinceChange4SampledSet;
defineTemplateTypeNameAndDebug(timeSinceChange4SampledSet,0);
addNamedToRunTimeSelectionTable(SampledSetValuePluginFunction, timeSinceChange4SampledSet , name, stateMachine_timeSinceChange);

    // for sampledSurfaces
typedef timeSinceChangePluginFunction<SampledSurfaceValuePluginFunction> timeSinceChange4SampledSurface;
defineTemplateTypeNameAndDebug(timeSinceChange4SampledSurface,0);
addNamedToRunTimeSelectionTable(SampledSurfaceValuePluginFunction, timeSinceChange4SampledSurface , name, stateMachine_timeSinceChange);

#ifdef FOAM_DEV

    // for faField
typedef timeSinceChangePluginFunction<FaFieldValuePluginFunction> timeSinceChange4FaField;
defineTemplateTypeNameAndDebug(timeSinceChange4FaField,0);
addNamedToRunTimeSelectionTable(FaFieldValuePluginFunction, timeSinceChange4FaField , name, stateMachine_timeSinceChange);


    // for faPatch
typedef timeSinceChangePluginFunction<FaPatchValuePluginFunction> timeSinceChange4FaPatch;
defineTemplateTypeNameAndDebug(timeSinceChange4FaPatch,0);
addNamedToRunTimeSelectionTable(FaPatchValuePluginFunction, timeSinceChange4FaPatch , name, stateMachine_timeSinceChange);

#endif

    // for clouds
typedef timeSinceChangePluginFunction<CloudValuePluginFunction> timeSinceChange4Cloud;
defineTemplateTypeNameAndDebug(timeSinceChange4Cloud,0);
addNamedToRunTimeSelectionTable(CloudValuePluginFunction, timeSinceChange4Cloud , name, stateMachine_timeSinceChange);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class PluginType>
timeSinceChangePluginFunction<PluginType>::timeSinceChangePluginFunction(
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
void timeSinceChangePluginFunction<PluginType>::setArgument(
    label index,
    const word &value
) {
    assert(index==0);

    machineName_=value;
}


template <class PluginType>
void timeSinceChangePluginFunction<PluginType>::doEvaluation()
{
    scalar elapsed=StateMachine::machine(machineName_).timeSinceChange();

    this->result().setResult(
        elapsed,
        this->parentDriver().size()
    );
}

template <>
void timeSinceChangePluginFunction<FieldValuePluginFunction>::doEvaluation()
{
    scalar elapsed=StateMachine::machine(machineName_).timeSinceChange();

    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "timeSinceChange_",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensionedScalar(
                "timeSinceChange",
                dimless,
                elapsed
            )
        )
    );

    result().setObjectResult(pResult);
}

#ifdef FOAM_DEV
template <>
void timeSinceChangePluginFunction<FaFieldValuePluginFunction>::doEvaluation() {
    scalar elapsed=StateMachine::machine(machineName_).timeSinceChange();

    autoPtr<areaScalarField> pResult(
        new areaScalarField(
            IOobject(
                "timeSinceChange_",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            FaCommonValueExpressionDriver::faRegionMesh(this->mesh()),
            dimensionedScalar(
                "timeSinceChange",
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
