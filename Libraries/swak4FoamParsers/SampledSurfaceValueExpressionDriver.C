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

#include "SampledSurfaceValueExpressionDriver.H"
#include "SampledSurfaceValuePluginFunction.H"

#include "SurfacesRepository.H"

#include "addToRunTimeSelectionTable.H"

#include "sampledIsoSurface.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(SampledSurfaceValueExpressionDriver, 0);

word SampledSurfaceValueExpressionDriver::driverName_="surface";

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, SampledSurfaceValueExpressionDriver, dictionary, surface);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, SampledSurfaceValueExpressionDriver, idName, surface);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void SampledSurfaceValueExpressionDriver::setDebug()
{
    if(debug>1) {
        if(sampledSurface::debug<1) {
            sampledSurface::debug=1;
        }
        if(sampledIsoSurface::debug<1) {
            sampledIsoSurface::debug=1;
        }
//         if(isoSurface::debug<1) {
//             isoSurface::debug=1;
//         }
    }
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


SampledSurfaceValueExpressionDriver::SampledSurfaceValueExpressionDriver(
    const sampledSurface &surf,
    const SampledSurfaceValueExpressionDriver& orig
)
:
        SubsetValueExpressionDriver(orig),
        //        theSurface_(surf.clone()),
        theSurface_(surf),
        interpolate_(orig.interpolate_),
        interpolationType_(orig.interpolationType_)
{
    setDebug();
}

SampledSurfaceValueExpressionDriver::SampledSurfaceValueExpressionDriver(
    sampledSurface &surf,
    bool autoInterpolate,
    bool warnAutoInterpolate
)
:
    SubsetValueExpressionDriver(autoInterpolate,warnAutoInterpolate),
    theSurface_(surf),
    interpolate_(false),
    interpolationType_("nix")
{
    setDebug();
}

SampledSurfaceValueExpressionDriver::SampledSurfaceValueExpressionDriver(
    const word &id,
    const fvMesh &mesh
)
:
    SubsetValueExpressionDriver(true,false),
    theSurface_(
        SurfacesRepository::getRepository().getSurface(
            id,
            mesh
        )
    ),
    interpolate_(false),
    interpolationType_("nix")
{
    setDebug();
}

SampledSurfaceValueExpressionDriver::SampledSurfaceValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    SubsetValueExpressionDriver(dict),
    theSurface_(
        SurfacesRepository::getRepository().getSurface(
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

SampledSurfaceValueExpressionDriver::~SampledSurfaceValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool SampledSurfaceValueExpressionDriver::update()
{
    bool updated=const_cast<sampledSurface &>(theSurface_).update(); // valgrind reports huge memory losses here
    if(debug) {
        Pout << "Updated: " << updated << " " << this->size() << endl;
    }

    return updated;
}

Field<scalar> *SampledSurfaceValueExpressionDriver::getScalarField(const string &name,bool oldTime)
{
    return sampleOrInterpolateInternal<scalar,volScalarField,surfaceScalarField>
        (
            name,
            oldTime
        );
}

Field<vector> *SampledSurfaceValueExpressionDriver::getVectorField(const string &name,bool oldTime)
{
    return sampleOrInterpolateInternal<vector,volVectorField,surfaceVectorField>
        (
            name,
            oldTime
        );
}

Field<tensor> *SampledSurfaceValueExpressionDriver::getTensorField(const string &name,bool oldTime)
{
    return sampleOrInterpolateInternal<tensor,volTensorField,surfaceTensorField>
        (
            name,
            oldTime
        );
}

Field<symmTensor> *SampledSurfaceValueExpressionDriver::getSymmTensorField(const string &name,bool oldTime)
{
    return sampleOrInterpolateInternal<symmTensor,volSymmTensorField,surfaceSymmTensorField>
        (
            name,
            oldTime
        );
}

Field<sphericalTensor> *SampledSurfaceValueExpressionDriver::getSphericalTensorField(const string &name,bool oldTime)
{
    return sampleOrInterpolateInternal<sphericalTensor,volSphericalTensorField,surfaceSphericalTensorField>
        (
            name,
            oldTime
        );
}

vectorField *SampledSurfaceValueExpressionDriver::makePositionField()
{
    return new vectorField(theSurface_.Cf());  // valgrind reports huge memory losses here
}

scalarField *SampledSurfaceValueExpressionDriver::makeCellVolumeField()
{
    FatalErrorIn("SampledSurfaceValueExpressionDriver::makeCellVolumeField()")
        << "faceZone knows nothing about cells"
            << endl
            << exit(FatalError);
    return new scalarField(0);
}


// vectorField *SampledSurfaceValueExpressionDriver::makePointField()
// {
//     notImplemented("SampledSurfaceValueExpressionDriver::makePointField");
// }

scalarField *SampledSurfaceValueExpressionDriver::makeFaceAreaMagField()
{
    if(debug) {
        Pout << "SampledSurfaceValueExpressionDriver::makeFaceAreaMagField()"
            << " size: " << this->size() << " magSf: " << theSurface_.magSf().size()
            << endl;
    }
    return new scalarField(theSurface_.magSf());
}

scalarField *SampledSurfaceValueExpressionDriver::makeFaceFlipField()
{
    scalarField *result=new scalarField(this->size(),false);

    return result;
}

vectorField *SampledSurfaceValueExpressionDriver::makeFaceNormalField()
{
    autoPtr<vectorField> sf(this->makeFaceAreaField());
    autoPtr<scalarField> magSf(this->makeFaceAreaMagField());

    return new vectorField(sf()/magSf());
}

vectorField *SampledSurfaceValueExpressionDriver::makeFaceAreaField()
{
    return new vectorField(theSurface_.Sf());
}

autoPtr<CommonPluginFunction> SampledSurfaceValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        SampledSurfaceValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool SampledSurfaceValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return SampledSurfaceValuePluginFunction::exists(
        *this,
        name
    );
}

// ************************************************************************* //

} // namespace
