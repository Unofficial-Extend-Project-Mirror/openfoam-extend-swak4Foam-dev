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

Contributors/Copyright:
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SampledSetValueExpressionDriver.H"
#include "SampledSetValuePluginFunction.H"

#include "SetsRepository.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(SampledSetValueExpressionDriver, 0);

word SampledSetValueExpressionDriver::driverName_="set";

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, SampledSetValueExpressionDriver, dictionary, set);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, SampledSetValueExpressionDriver, idName, set);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void SampledSetValueExpressionDriver::setDebug()
{
    if(debug>1) {
        if(sampledSet::debug<1) {
            sampledSet::debug=1;
        }
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


SampledSetValueExpressionDriver::SampledSetValueExpressionDriver(
    const sampledSet &surf,
    const SampledSetValueExpressionDriver& orig
)
:
        SubsetValueExpressionDriver(orig),
        //        theSet_(surf.clone()),
        theSet_(surf),
        interpolate_(orig.interpolate_),
        interpolationType_(orig.interpolationType_)
{
    setDebug();
}

SampledSetValueExpressionDriver::SampledSetValueExpressionDriver(
    sampledSet &surf,
    bool autoInterpolate,
    bool warnAutoInterpolate
)
:
    SubsetValueExpressionDriver(autoInterpolate,warnAutoInterpolate),
    theSet_(surf),
    interpolate_(false),
    interpolationType_("nix")
{
    setDebug();
}

SampledSetValueExpressionDriver::SampledSetValueExpressionDriver(
    const word &id,
    const fvMesh &mesh
)
:
    SubsetValueExpressionDriver(true,false),
    theSet_(
        SetsRepository::getRepository(mesh).getSet(
            id,
            mesh
        )
    ),
    interpolate_(false),
    interpolationType_("nix")
{
    setDebug();
}

SampledSetValueExpressionDriver::SampledSetValueExpressionDriver(
    const dictionary& dict,const fvMesh&mesh
)
 :
    SubsetValueExpressionDriver(dict),
    theSet_(
        SetsRepository::getRepository(mesh).getSet(
            dict,
            mesh
        )
    ),
    interpolate_(dict.lookupOrDefault<bool>("interpolate",false)),
    interpolationType_(
        interpolate_
        ?
        word(dict.lookup("interpolationType"))
        :
        word("nix")
    )
{
    setDebug();
}
// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SampledSetValueExpressionDriver::~SampledSetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

label SampledSetValueExpressionDriver::size() const
{
    return theSet_.faces().size();
}

bool SampledSetValueExpressionDriver::update()
{
    bool updated=false; // nu update in sampledSet
    if(debug) {
        Pout << "Updated: " << updated << " " << this->size() << endl;
    }

    return updated;
}

tmp<Field<scalar> > SampledSetValueExpressionDriver::getScalarField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<scalar,volScalarField,surfaceScalarField>
        (
            name,
            oldTime
        );
}

tmp<Field<vector> > SampledSetValueExpressionDriver::getVectorField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<vector,volVectorField,surfaceVectorField>
        (
            name,
            oldTime
        );
}

tmp<Field<tensor> > SampledSetValueExpressionDriver::getTensorField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<tensor,volTensorField,surfaceTensorField>
        (
            name,
            oldTime
        );
}

tmp<Field<symmTensor> > SampledSetValueExpressionDriver::getSymmTensorField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<symmTensor,volSymmTensorField,
                                       surfaceSymmTensorField>
        (
            name,
            oldTime
        );
}

tmp<Field<sphericalTensor> >
SampledSetValueExpressionDriver::getSphericalTensorField(
        const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<sphericalTensor,
                                       volSphericalTensorField,
                                       surfaceSphericalTensorField>
        (
            name,
            oldTime
        );
}

tmp<vectorField> SampledSetValueExpressionDriver::makePositionField() const
{
    return tmp<vectorField>(
        new vectorField(theSet_)
    );
}

tmp<scalarField> SampledSetValueExpressionDriver::makeCellVolumeField() const
{
    FatalErrorIn("SampledSetValueExpressionDriver::makeCellVolumeField()")
        << "faceZone knows nothing about cells"
            << endl
            << exit(FatalError);
    return tmp<scalarField>(
        new scalarField(0)
    );
}


// tmp<vectorField> SampledSetValueExpressionDriver::makePointField()
// {
//     notImplemented("SampledSetValueExpressionDriver::makePointField");
// }

tmp<scalarField> SampledSetValueExpressionDriver::makeFaceAreaMagField() const
{
    FatalErrorIn("SampledSetValueExpressionDriver::makeFaceAreaMagField()")
        << "sampledSets knows nothing about faces"
            << endl
            << exit(FatalError);

    return tmp<scalarField>(
        new scalarField(0)
    );
}

tmp<scalarField> SampledSetValueExpressionDriver::makeFaceFlipField() const
{
    tmp<scalarField> result(new scalarField(this->size(),false));

    return result;
}

tmp<vectorField> SampledSetValueExpressionDriver::makeFaceNormalField() const
{
    return this->makeFaceAreaField()/this->makeFaceAreaMagField();
}

tmp<vectorField> SampledSetValueExpressionDriver::makeFaceAreaField() const
{
    FatalErrorIn("SampledSetValueExpressionDriver::makeFaceAreaField()")
        << "sampledSets knows nothing about faces"
            << endl
            << exit(FatalError);

    return tmp<vectorField>(
        new vectorField(0)
    );
}

autoPtr<CommonPluginFunction>
SampledSetValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        SampledSetValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool SampledSetValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return SampledSetValuePluginFunction::exists(
        *this,
        name
    );
}

tmp<scalarField> SampledSetValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!faceSize) {
        Pout << "Expected size: " << size
            << " The size: " << faceSize << endl;

        FatalErrorIn("SampledSetValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(
        new scalarField(
            this->size(),
            1.
        )
    );
}

// ************************************************************************* //

} // namespace
