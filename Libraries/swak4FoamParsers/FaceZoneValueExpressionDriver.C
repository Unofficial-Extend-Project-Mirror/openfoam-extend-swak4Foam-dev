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
    2010-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FaceZoneValueExpressionDriver.H"
#include "FaceZoneValuePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(FaceZoneValueExpressionDriver, 0);

word FaceZoneValueExpressionDriver::driverName_="faceZone";

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceZoneValueExpressionDriver, dictionary, faceZone);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceZoneValueExpressionDriver, idName, faceZone);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

label getFaceZoneID(const fvMesh &mesh,const word &name)
{
    label result=mesh.faceZones().findZoneID(name);
    if(result<0) {
        FatalErrorIn("getFaceZoneID(const fvMesh &mesh,const word &name)")
            << "The faceZone " << name << " was not found in "
                << mesh.faceZones().names()
                << endl
                << exit(FatalError);

    }
    return result;
}


FaceZoneValueExpressionDriver::FaceZoneValueExpressionDriver(
        const faceZone &zone,
        const FaceZoneValueExpressionDriver& orig
)
:
        SubsetValueExpressionDriver(orig),
        faceZone_(zone)
{}

FaceZoneValueExpressionDriver::FaceZoneValueExpressionDriver(
    const faceZone &zone,
    bool autoInterpolate,
    bool warnAutoInterpolate
)
:
    SubsetValueExpressionDriver(autoInterpolate,warnAutoInterpolate),
    faceZone_(zone)
{}

FaceZoneValueExpressionDriver::FaceZoneValueExpressionDriver(
    const dictionary& dict,
    const fvMesh&mesh
)
 :
    SubsetValueExpressionDriver(dict),
    faceZone_(
        regionMesh(
            dict,
            mesh,
            searchOnDisc()
        ).faceZones()[
            getFaceZoneID(
                regionMesh(
                    dict,
                    mesh,
                    searchOnDisc()
                ),
                dict.lookup(
                    "zoneName"
                )
            )
        ]
    )
{
}

FaceZoneValueExpressionDriver::FaceZoneValueExpressionDriver(
    const word& id,
    const fvMesh&mesh
)
 :
    SubsetValueExpressionDriver(true,false),
    faceZone_(
        mesh.faceZones()[
            getFaceZoneID(
                mesh,
                id
            )
        ]
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

FaceZoneValueExpressionDriver::~FaceZoneValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<>
inline label SubsetValueExpressionDriver::getIndexFromIterator(
    const faceZone::const_iterator &it
) const
{
    return *it;
}

tmp<Field<scalar> > FaceZoneValueExpressionDriver::getScalarField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceScalarField,volScalarField,faceZone,scalar>(
            name,
            faceZone_,
            oldTime
        );
}

tmp<Field<vector> > FaceZoneValueExpressionDriver::getVectorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceVectorField,volVectorField,faceZone,vector>(
            name,
            faceZone_,
            oldTime
        );
}

tmp<Field<tensor> > FaceZoneValueExpressionDriver::getTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceTensorField,volTensorField,faceZone,tensor>(
            name,
            faceZone_,
            oldTime
        );
}

tmp<Field<symmTensor> > FaceZoneValueExpressionDriver::getSymmTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceSymmTensorField,volSymmTensorField,faceZone,symmTensor>(
            name,
            faceZone_,
            oldTime
        );
}

tmp<Field<sphericalTensor> >
FaceZoneValueExpressionDriver::getSphericalTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceSphericalTensorField,volSphericalTensorField,
         faceZone,sphericalTensor>(
            name,
            faceZone_,
            oldTime
        );
}

tmp<vectorField> FaceZoneValueExpressionDriver::makePositionField() const
{
    return getFromSurfaceFieldInternal(this->mesh().Cf(),faceZone_);
}

tmp<scalarField> FaceZoneValueExpressionDriver::makeCellVolumeField() const
{
    FatalErrorIn("FaceZoneValueExpressionDriver::makeCellVolumeField()")
        << "faceZone knows nothing about cells"
            << endl
            << exit(FatalError);
    return tmp<scalarField>(
        new scalarField(0)
    );
}


// tmp<vectorField> FaceZoneValueExpressionDriver::makePointField()
// {
//     notImplemented("FaceZoneValueExpressionDriver::makePointField");
// }

tmp<scalarField> FaceZoneValueExpressionDriver::makeFaceAreaMagField() const
{
    return getFromSurfaceFieldInternal(this->mesh().magSf(),faceZone_);
}

tmp<scalarField> FaceZoneValueExpressionDriver::makeFaceFlipField() const
{
    tmp<scalarField> result(new scalarField(this->size()));
    const boolList &flip=faceZone_.flipMap();

    label cnt=0;

    forAll(faceZone_,i) {
        const label faceI=faceZone_[i];
        if(useFaceValue(faceI)) {
            result()[cnt]= (flip[i] ? -1 : 1);
            cnt++;
        }
    }

    assert(cnt==result->size());

    return result;
}

tmp<vectorField> FaceZoneValueExpressionDriver::makeFaceNormalField() const
{
    return this->makeFaceAreaField()/this->makeFaceAreaMagField();
}

tmp<vectorField> FaceZoneValueExpressionDriver::makeFaceAreaField() const
{
    return getFromSurfaceFieldInternal(this->mesh().Sf(),faceZone_);
}

autoPtr<CommonPluginFunction> FaceZoneValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        FaceZoneValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool FaceZoneValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return FaceZoneValuePluginFunction::exists(
        *this,
        name
    );
}

tmp<scalarField> FaceZoneValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!isFace) {
        Pout << "Expected size: " << size
            << " Face size: " << faceSize << endl;

        FatalErrorIn("FaceZoneValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(makeFaceAreaMagField());
}

// ************************************************************************* //

} // namespace
