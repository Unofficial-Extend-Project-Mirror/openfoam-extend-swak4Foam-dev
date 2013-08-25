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
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
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
        SurfacesRepository::getRepository(mesh).getSurface(
            id,
            mesh
        )
    ),
    interpolate_(false),
    interpolationType_("nix")
{
    setDebug();
}

SampledSurfaceValueExpressionDriver::SampledSurfaceValueExpressionDriver(
    const dictionary& dict,
    const fvMesh&mesh
)
 :
    SubsetValueExpressionDriver(dict),
    theSurface_(
        SurfacesRepository::getRepository(mesh).getSurface(
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
    if(debug) {
        Info << "SampledSurfaceValueExpressionDriver::update() "
            << "needsUpdate: " << theSurface_.needsUpdate() << endl;
    }
    bool updated=const_cast<sampledSurface &>(theSurface_).update(); // valgrind reports huge memory losses here

    if(debug) {
        Pout << "Updated: " << updated << " " << this->size() << endl;
    }

    return updated;
}

void SampledSurfaceValueExpressionDriver::updateIfNeeded()
{
    if(debug) {
        Info << "SampledSurfaceValueExpressionDriver::updateIfNeeded()" << endl;
    }
    if(theSurface_.needsUpdate()) {
        if(debug) {
            Info << "Forcing an update" << endl;
        }
        update();
    }
}

tmp<Field<scalar> > SampledSurfaceValueExpressionDriver::getScalarField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<scalar,volScalarField,surfaceScalarField>
        (
            name,
            oldTime
        );
}

tmp<Field<vector> > SampledSurfaceValueExpressionDriver::getVectorField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<vector,volVectorField,surfaceVectorField>
        (
            name,
            oldTime
        );
}

tmp<Field<tensor> > SampledSurfaceValueExpressionDriver::getTensorField(
    const word &name,bool oldTime
)
{
    return sampleOrInterpolateInternal<tensor,volTensorField,surfaceTensorField>
        (
            name,
            oldTime
        );
}

tmp<Field<symmTensor> > SampledSurfaceValueExpressionDriver::getSymmTensorField(
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
SampledSurfaceValueExpressionDriver::getSphericalTensorField(
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

tmp<vectorField> SampledSurfaceValueExpressionDriver::makePositionField() const
{
    return theSurface_.Cf();
    // valgrind reports huge memory losses here
}

tmp<scalarField>
SampledSurfaceValueExpressionDriver::makeCellVolumeField() const
{
    FatalErrorIn("SampledSurfaceValueExpressionDriver::makeCellVolumeField()")
        << "faceZone knows nothing about cells"
            << endl
            << exit(FatalError);

    return tmp<scalarField>(
        new scalarField(0)
    );
}


// tmp<vectorField> SampledSurfaceValueExpressionDriver::makePointField()
// {
//     notImplemented("SampledSurfaceValueExpressionDriver::makePointField");
// }

tmp<scalarField>
SampledSurfaceValueExpressionDriver::makeFaceAreaMagField() const
{
    if(debug) {
        Pout << "SampledSurfaceValueExpressionDriver::makeFaceAreaMagField()"
            << " size: " << this->size() << " magSf: "
            << theSurface_.magSf().size()
            << endl;
    }
    return tmp<scalarField>(
        new scalarField(theSurface_.magSf())
    );
}

tmp<scalarField> SampledSurfaceValueExpressionDriver::makeFaceFlipField() const
{
    tmp<scalarField> result(new scalarField(this->size(),false));

    return result;
}

tmp<vectorField>
SampledSurfaceValueExpressionDriver::makeFaceNormalField() const
{
    return this->makeFaceAreaField()/this->makeFaceAreaMagField();
}

tmp<vectorField> SampledSurfaceValueExpressionDriver::makeFaceAreaField() const
{
    return tmp<vectorField>(
        new vectorField(theSurface_.Sf())
    );
}

autoPtr<CommonPluginFunction>
SampledSurfaceValueExpressionDriver::newPluginFunction(
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

tmp<scalarField> SampledSurfaceValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!isFace) {
        Pout << "Expected size: " << size
            << " Face size: " << faceSize << endl;

        FatalErrorIn("SampledSurfaceValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(makeFaceAreaMagField());
}

label SampledSurfaceValueExpressionDriver::size() const
{
    if(debug) {
        Info << "SampledSurfaceValueExpressionDriver::size()" << endl;
        Info << "Needs update: " << theSurface_.needsUpdate() << endl;
    }

    const_cast<SampledSurfaceValueExpressionDriver&>(*this).updateIfNeeded();

    return theSurface_.faces().size();
}

label SampledSurfaceValueExpressionDriver::pointSize() const
{
    if(debug) {
        Info << "SampledSurfaceValueExpressionDriver::pointSize()" << endl;
    }

    const_cast<SampledSurfaceValueExpressionDriver&>(*this).updateIfNeeded();

    return theSurface_.points().size();
}

const fvMesh &SampledSurfaceValueExpressionDriver::mesh() const
{
    if(debug) {
        Info << "SampledSurfaceValueExpressionDriver::mesh()" << endl;
    }
    //        return dynamicCast<const fvMesh&>(faceZone_.zoneMesh().mesh()); // doesn't work with gcc 4.2
    return dynamic_cast<const fvMesh&>(theSurface_.mesh());
}

// ************************************************************************* //

} // namespace
