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

#include "PatchValueExpressionDriver.H"
#include "CellSetValueExpressionDriver.H"
#include "FaceSetValueExpressionDriver.H"
#include "CellZoneValueExpressionDriver.H"
#include "FaceZoneValueExpressionDriver.H"
#include "SampledSetValueExpressionDriver.H"
#include "SampledSurfaceValueExpressionDriver.H"

#include "Pstream.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<PatchValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,PatchValueExpressionDriver , name, patchContributionScalar);

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<CellSetValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,CellSetValueExpressionDriver , name, cellSetContributionScalar);

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<CellZoneValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,CellZoneValueExpressionDriver , name, cellZoneContributionScalar);

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<FaceSetValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,FaceSetValueExpressionDriver , name, faceSetContributionScalar);

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<FaceZoneValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,FaceZoneValueExpressionDriver , name, faceZoneContributionScalar);

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<SampledSetValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,SampledSetValueExpressionDriver , name, sampledSetContributionScalar);

defineTemplateTypeNameAndDebug(ContributionScalarPluginFunction<SampledSurfaceValueExpressionDriver>,0);
addNamedTemplateToRunTimeSelectionTable(FieldValuePluginFunction, ContributionScalarPluginFunction,SampledSurfaceValueExpressionDriver , name, sampledSurfaceContributionScalar);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class Driver>
ContributionScalarPluginFunction<Driver>::ContributionScalarPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
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
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class Driver>
void ContributionScalarPluginFunction<Driver>::setArgument(
    label index,
    const word &value
) {
    assert(index==0);

    unitName_=value;
}

template <class Driver>
word ContributionScalarPluginFunction<Driver>::getID(
    label index
) {
    if(index==1) {
        return unitName_;
    } else {
        return CommonPluginFunction::getID(index);
    }
}

template <class Driver>
void ContributionScalarPluginFunction<Driver>::setArgument(
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

template <class Driver>
void ContributionScalarPluginFunction<Driver>::doEvaluation()
{
    List<scalarField> values(Pstream::nProcs());
    values[Pstream::myProcNo()]=values_();
    List<vectorField> positions(Pstream::nProcs());
    positions[Pstream::myProcNo()]=positions_();

    Pstream::scatterList(values);
    Pstream::scatterList(positions);

    autoPtr<volScalarField> pResult(
        new volScalarField(
            IOobject(
                "contributionFrom_"+Driver::driverName(),
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            dimensionedScalar("contribution",dimless,0)
        )
    );

    volScalarField &result=pResult();

    forAll(result,cellI)
    {
        scalar sum=0;
        scalar distSum=0;
        const vector &here=mesh().C()[cellI];
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

    this->result().setObjectResult(pResult);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
