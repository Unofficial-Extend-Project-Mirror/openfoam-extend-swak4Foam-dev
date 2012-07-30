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

#include "patchContributionScalarFunctionPlugin.H"
#include "PatchValueExpressionDriver.H"
#include "Pstream.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(patchContributionScalarPluginFunction,1);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, patchContributionScalarPluginFunction , name, patchContributionScalar);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

patchContributionScalarPluginFunction::patchContributionScalarPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        "volScalarField",
        string("patchName primitive word,patchValues patch scalar")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void patchContributionScalarPluginFunction::setArgument(
    label index,
    const word &value
) {
    assert(index==0);

    patchName_=value;
}

word patchContributionScalarPluginFunction::getID(
    label index
) {
    if(index==1) {
        return patchName_;
    } else {
        return CommonPluginFunction::getID(index);
    }
}

void patchContributionScalarPluginFunction::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==1);

    PatchValueExpressionDriver &patchDriver=
        const_cast<PatchValueExpressionDriver &>(
            dynamicCast<const PatchValueExpressionDriver &>(
                driver
            )
        );

    values_.set(
        new scalarField(
            patchDriver.getResult<scalar>()
        )
    );

    positions_.set(
        patchDriver.makePositionField()
    );
}

void patchContributionScalarPluginFunction::doEvaluation()
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
                "contributionFromPatch",
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
                sum+=localValues[i]/dist;
                distSum+=1/dist;
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
