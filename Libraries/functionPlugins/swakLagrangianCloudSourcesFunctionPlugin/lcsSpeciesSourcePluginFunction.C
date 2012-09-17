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

#include "lcsSpeciesSourcePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "basicReactingCloud.H"
#include "basicReactingMultiphaseCloud.H"

namespace Foam {

defineTypeNameAndDebug(lcsSpeciesSourcePluginFunction,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,lcsSpeciesSourcePluginFunction , name, lcsSpeciesSource);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

lcsSpeciesSourcePluginFunction::lcsSpeciesSourcePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    LagrangianCloudSourcePluginFunction(
        parentDriver,
        name,
        "volScalarField",
        "speciesName primitive word"
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

#define getSpeciesIndex(neededType,usedType)                                \
    if(speciesList.size()==0 && hasCloudAs<neededType>()) {                 \
        const speciesTable &spec=                                           \
            getCloudAs<neededType,usedType>().thermo().carrier().species(); \
        speciesList=spec;                                                   \
        if(spec.contains(speciesName_)) {                                   \
            speciesIndex=spec[speciesName_];                                \
        }                                                                   \
    }

void lcsSpeciesSourcePluginFunction::doEvaluation()
{
    typedef DimensionedField<scalar,volMesh> dimScalarField;
    autoPtr<dimScalarField> pSrho;

    label speciesIndex=-1;
    wordList speciesList;

    getSpeciesIndex(basicReactingCloud,reactingCloud);
    getSpeciesIndex(basicReactingMultiphaseCloud,reactingMultiphaseCloud);

    if(speciesList.size()==0) {
        FatalErrorIn("lcsSpeciesSourcePluginFunction::doEvaluation()")
            << "No species list found (probably wrong cloud type)"
                << endl
                << exit(FatalError);
    }

    if(speciesIndex<0) {
        FatalErrorIn("lcsSpeciesSourcePluginFunction::doEvaluation()")
            << "Species " << speciesName_ << " not found in gas composition "
                << speciesList
                << endl
                << exit(FatalError);

    }

    // pick up the first fitting class
    castAndCall(pSrho,dimScalarField,basicReactingCloud,reactingCloud,Srho(speciesIndex));
    castAndCall(pSrho,dimScalarField,basicReactingMultiphaseCloud,reactingMultiphaseCloud,Srho(speciesIndex));

    noCloudFound(pSrho);

    const dimScalarField &Srho=pSrho();

    autoPtr<volScalarField> pSource(
        new volScalarField(
            IOobject(
                cloudName()+"SpeciesSource",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh(),
            Srho.dimensions(),
            "zeroGradient"
        )
    );

    pSource->internalField()=Srho.field();

    result().setObjectResult(pSource);
}

void lcsSpeciesSourcePluginFunction::setArgument(
    label index,
    const word &name
)
{
    if(index==0) {
        LagrangianCloudSourcePluginFunction::setArgument(
            index,
            name
        );
        return;
    }
    assert(index==1);
    speciesName_=name;
}



// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //


} // namespace

// ************************************************************************* //
