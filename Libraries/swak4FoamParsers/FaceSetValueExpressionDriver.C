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
    2010-2014, 2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FaceSetValueExpressionDriver.H"
#include "FaceSetValuePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

#include "cellSet.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(FaceSetValueExpressionDriver, 0);

word FaceSetValueExpressionDriver::driverName_="faceSet";

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceSetValueExpressionDriver, dictionary, faceSet);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceSetValueExpressionDriver, idName, faceSet);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(
    const faceSet &set,
    const FaceSetValueExpressionDriver& orig
)
:
        SetSubsetValueExpressionDriver(orig),
        faceSet_(
            //            dynamicCast<const fvMesh&>(set.db()), // doesn't work with gcc 4.2
            new faceSet(
                dynamic_cast<const fvMesh&>(set.db()),
                //            set.name()+"_copy",
                set.name(),
                set
            )
        )
{}

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(
    const faceSet &set,
    bool autoInterpolate,
    bool warnAutoInterpolate
)
:
    SetSubsetValueExpressionDriver(
        set.name(),
        INVALID,
        autoInterpolate,
        warnAutoInterpolate
    ),
    faceSet_(
        //            dynamicCast<const fvMesh&>(set.db()), // doesn't work with gcc 4.2
        new faceSet(
            dynamic_cast<const fvMesh&>(set.db()),
            //            set.name()+"_copy",
            set.name(),
            set
        )
    )
{}

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(
    const dictionary& dict,
    const fvMesh&mesh
)
 :
    SetSubsetValueExpressionDriver(dict,dict.lookup("setName"),INVALID),
    faceSet_(
        getTopoSet<faceSet>(
            regionMesh(
                dict,
                mesh,
                searchOnDisc()
            ),
            dict.lookup("setName"),
            origin_
        )
    )
{
}

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(
    const word& id,
    const fvMesh&mesh
)
 :
    SetSubsetValueExpressionDriver(
        id,
        INVALID,
        true,
        false
    ),
    faceSet_(
        getTopoSet<faceSet>(
            mesh,
            id,
            origin_
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

FaceSetValueExpressionDriver::~FaceSetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
inline label SubsetValueExpressionDriver::getIndexFromIterator(
    const faceSet::const_iterator &it
) const
{
    return it.key();
}

tmp<Field<scalar> > FaceSetValueExpressionDriver::getScalarField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceScalarField,volScalarField,faceSet,scalar>(
            name,
            faceSet_,
            oldTime
        );
}

tmp<Field<vector> > FaceSetValueExpressionDriver::getVectorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceVectorField,volVectorField,faceSet,vector>(
            name,
            faceSet_,
            oldTime
        );
}

tmp<Field<tensor> > FaceSetValueExpressionDriver::getTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceTensorField,volTensorField,faceSet,tensor>(
            name,
            faceSet_,
            oldTime
        );
}

tmp<Field<symmTensor> > FaceSetValueExpressionDriver::getSymmTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceSymmTensorField,volSymmTensorField,faceSet,symmTensor>(
            name,
            faceSet_,
            oldTime
        );
}

tmp<Field<sphericalTensor> >
FaceSetValueExpressionDriver::getSphericalTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternalAndInterpolate
        <surfaceSphericalTensorField,volSphericalTensorField,
         faceSet,sphericalTensor>(
             name,
             faceSet_,
             oldTime
         );
}

tmp<vectorField> FaceSetValueExpressionDriver::makePositionField() const
{
    return getFromSurfaceFieldInternal(this->mesh().Cf(),faceSet_());
}

tmp<scalarField> FaceSetValueExpressionDriver::makeCellVolumeField() const
{
    FatalErrorIn("FaceSetValueExpressionDriver::makeCellVolumeField()")
        << "faceSet knows nothing about cells"
            << endl
            << exit(FatalError);

    return tmp<scalarField>(
        new scalarField(0)
    );
}


// tmp<vectorField> FaceSetValueExpressionDriver::makePointField()
// {
//     notImplemented("FaceSetValueExpressionDriver::makePointField");
// }

tmp<scalarField> FaceSetValueExpressionDriver::makeFaceFlipField() const
{
    // inspired by the setsToZones-utility

    tmp<scalarField> result(
        new scalarField(this->size())
    );

    word setName(faceSet_->name() + "SlaveCells");
    const fvMesh &mesh=this->mesh();

    SetOrigin origin=INVALID;

    cellSet cells(
        mesh,
        setName,
        getTopoSet<cellSet>(
            mesh,
            setName,
            origin
        )
    );
    assert(origin!=INVALID);

    List<label> faceLabels(faceSet_->toc());
    label cnt=0;

    forAll(faceLabels, i)
    {
        label faceI = faceLabels[i];
        bool use=false;

        bool flip = false;

        if (mesh.isInternalFace(faceI))
        {
            use=true;
            if
                (
                    cells.found(mesh.faceOwner()[faceI])
                    && !cells.found(mesh.faceNeighbour()[faceI])
                )
            {
                flip = false;
            }
            else if
                (
                    !cells.found(mesh.faceOwner()[faceI])
                    && cells.found(mesh.faceNeighbour()[faceI])
                )
            {
                flip = true;
            }
            else
            {
                FatalErrorIn("tmp<scalarField> FaceSetValueExpressionDriver::makeFaceFlipField()")
                    << "One of owner or neighbour of internal face "
                        << faceI << " should be in cellSet " << cells.name()
                        << " to be able to determine orientation." << endl
                        << "Face:" << faceI
                        << " own:" << mesh.faceOwner()[faceI]
                        << " OwnInCellSet:"
                        << cells.found(mesh.faceOwner()[faceI])
                        << " nei:" << mesh.faceNeighbour()[faceI]
                        << " NeiInCellSet:"
                        << cells.found(mesh.faceNeighbour()[faceI])
                        << exit(FatalError);
            }
        }
        else
        {
            use=useFaceValue(faceI);
            if(use) {
                if (cells.found(mesh.faceOwner()[faceI]))
                {
                    flip = false;
                }
                else
                {
                    flip = true;
                }
            }
        }

        if(use) {
            result()[cnt]= (flip ? -1 : 1 );
            cnt++;
        }
    }

    assert(cnt==result->size());

    return result;
}

tmp<scalarField> FaceSetValueExpressionDriver::makeFaceAreaMagField() const
{
    return getFromSurfaceFieldInternal(this->mesh().magSf(),faceSet_());
}

tmp<vectorField> FaceSetValueExpressionDriver::makeFaceNormalField() const
{
    return this->makeFaceAreaField()/this->makeFaceAreaMagField();
}

tmp<vectorField> FaceSetValueExpressionDriver::makeFaceAreaField() const
{
    return getFromSurfaceFieldInternal(this->mesh().Sf(),faceSet_());
}

bool FaceSetValueExpressionDriver::update()
{
    if(debug) {
        Pout << "FaceSet: update " << faceSet_->name() << endl;
    }

    return true;
}

autoPtr<CommonPluginFunction> FaceSetValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        FaceSetValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool FaceSetValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return FaceSetValuePluginFunction::exists(
        *this,
        name
    );
}


tmp<scalarField> FaceSetValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label faceSize=this->size();
    bool isFace=(size==faceSize);
    reduce(isFace,andOp<bool>());

    if(!isFace) {
        Pout << "Expected size: " << size
            << " Face size: " << faceSize << endl;

        FatalErrorIn("FaceSetValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(makeFaceAreaMagField());
}

// ************************************************************************* //

} // namespace
