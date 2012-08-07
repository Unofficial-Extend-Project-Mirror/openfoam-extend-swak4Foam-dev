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

#include "ContributionScalarFunctionPlugin.H"

#include "FieldValueExpressionDriver.H"
#include "PatchValueExpressionDriver.H"
#include "CellSetValueExpressionDriver.H"
#include "FaceSetValueExpressionDriver.H"
#include "CellZoneValueExpressionDriver.H"
#include "FaceZoneValueExpressionDriver.H"
#include "SampledSetValueExpressionDriver.H"
#include "SampledSurfaceValueExpressionDriver.H"

#include "Pstream.H"
#include "addToRunTimeSelectionTable.H"

#include "FieldValuePluginFunction.H"
#include "PatchValuePluginFunction.H"
#include "CellSetValuePluginFunction.H"

namespace Foam {

    // for fields
typedef ContributionScalarPluginFunction<PatchValueExpressionDriver,FieldValuePluginFunction> patchToFieldContribute;
defineTemplateTypeNameAndDebug(patchToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, patchToFieldContribute , name, patchContributionScalar);

typedef ContributionScalarPluginFunction<CellSetValueExpressionDriver,FieldValuePluginFunction> cellSetToFieldContribute;
defineTemplateTypeNameAndDebug(cellSetToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, cellSetToFieldContribute , name, cellSetContributionScalar);

typedef ContributionScalarPluginFunction<CellZoneValueExpressionDriver,FieldValuePluginFunction> cellZoneToFieldContribute;
defineTemplateTypeNameAndDebug(cellZoneToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, cellZoneToFieldContribute , name, cellZoneContributionScalar);

typedef ContributionScalarPluginFunction<FaceSetValueExpressionDriver,FieldValuePluginFunction> faceSetToFieldContribute;
defineTemplateTypeNameAndDebug(faceSetToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, faceSetToFieldContribute , name, faceSetContributionScalar);

typedef ContributionScalarPluginFunction<FaceZoneValueExpressionDriver,FieldValuePluginFunction> faceZoneToFieldContribute;
defineTemplateTypeNameAndDebug(faceZoneToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, faceZoneToFieldContribute , name, faceZoneContributionScalar);

typedef ContributionScalarPluginFunction<SampledSetValueExpressionDriver,FieldValuePluginFunction> sampledSetToFieldContribute;
defineTemplateTypeNameAndDebug(sampledSetToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, sampledSetToFieldContribute , name, sampledSetContributionScalar);

typedef ContributionScalarPluginFunction<SampledSurfaceValueExpressionDriver,FieldValuePluginFunction> sampledSurfaceToFieldContribute;
defineTemplateTypeNameAndDebug(sampledSurfaceToFieldContribute,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, sampledSurfaceToFieldContribute , name, sampledSurfaceContributionScalar);

    // for patches
typedef ContributionScalarPluginFunction<PatchValueExpressionDriver,PatchValuePluginFunction> patchToPatchContribute;
defineTemplateTypeNameAndDebug(patchToPatchContribute,0);
addNamedToRunTimeSelectionTable(PatchValuePluginFunction, patchToPatchContribute , name, patchContributionScalar);

    // for cellSets
typedef ContributionScalarPluginFunction<PatchValueExpressionDriver,CellSetValuePluginFunction> patchToCellSetContribute;
defineTemplateTypeNameAndDebug(patchToCellSetContribute,0);
addNamedToRunTimeSelectionTable(CellSetValuePluginFunction, patchToCellSetContribute , name, patchContributionScalar);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class Driver,class PluginType>
ContributionScalarPluginFunction<Driver,PluginType>::ContributionScalarPluginFunction(
    const typename PluginType::driverType &parentDriver,
    const word &name
):
    PluginType(
        parentDriver,
        name,
        "volScalarField",
        string(
            Driver::driverName()+"Name primitive word,"+
            Driver::driverName()+"Values "+
            Driver::driverName()+
            " scalar"
        )
    )
{
    typedef typename PluginType::driverType PluginTypeDriverType;

    if(PluginTypeDriverType::driverName()!="internalField") {
        this->returnType()="scalar";
    }
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class Driver,class PluginType>
void ContributionScalarPluginFunction<Driver,PluginType>::setArgument(
    label index,
    const word &value
) {
    assert(index==0);

    unitName_=value;
}

template <class Driver,class PluginType>
word ContributionScalarPluginFunction<Driver,PluginType>::getID(
    label index
) {
    if(index==1) {
        return unitName_;
    } else {
        return CommonPluginFunction::getID(index);
    }
}

template <class Driver,class PluginType>
void ContributionScalarPluginFunction<Driver,PluginType>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==1);

    Driver &theDriver=
        const_cast<Driver &>(
            dynamicCast<const Driver &>(
                driver
            )
        );

    values_.set(
        new scalarField(
            theDriver.CommonValueExpressionDriver::getResult<scalar>()
        )
    );

    positions_.set(
        theDriver.makePositionField()
    );
}

template <class PluginType >
void setResultForContribution(
    const fvMesh &mesh,
    ExpressionResult &result,
    const scalarField &values
) {
    result.setResult(values);
}

template <>
void setResultForContribution<FieldValuePluginFunction>(
    const fvMesh &mesh,
    ExpressionResult &result,
    const scalarField &values
) {
    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "contributionFrom_", // +Driver::driverName(),
                mesh.time().timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            dimensionedScalar("contribution",dimless,0)
        )
    );

    pResult->internalField()=values;
    pResult->correctBoundaryConditions();

    result.setObjectResult(pResult);
}

template <class Driver,class PluginType>
void ContributionScalarPluginFunction<Driver,PluginType>::doEvaluation()
{
    List<scalarField> values(Pstream::nProcs());
    values[Pstream::myProcNo()]=values_();
    List<vectorField> positions(Pstream::nProcs());
    positions[Pstream::myProcNo()]=positions_();

    Pstream::scatterList(values);
    Pstream::scatterList(positions);

    scalarField result(
        this->parentDriver().size()
    );

    forAll(result,cellI)
    {
        scalar sum=0;
        scalar distSum=0;
        const vector &here=this->mesh().C()[cellI];
        bool found=false;

        forAll(values,valI)
        {
            const scalarField &localValues=values[valI];
            const vectorField &localPos=positions[valI];

            forAll(localValues,i)
            {
                const scalar dist=mag(localPos[i]-here);
                if(dist<SMALL) {
                    // Avoid divison by zero
                    result[cellI]=localValues[i];
                    found=true;
                    break;
                }
                //                const scalar weight=dist*dist;
                const scalar weight=dist;
                sum+=localValues[i]/weight;
                distSum+=1/weight;
            }

            if(found) {
                break;
            }
        }

        if(!found) {
            result[cellI]=sum/distSum;
        }
    }

    setResultForContribution<PluginType>(this->mesh(),this->result(),result);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
