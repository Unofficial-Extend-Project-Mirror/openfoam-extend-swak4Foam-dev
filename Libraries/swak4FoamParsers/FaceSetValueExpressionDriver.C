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

#include "FaceSetValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "cellSet.H"
#include "SortableList.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(FaceSetValueExpressionDriver, 0);

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceSetValueExpressionDriver, dictionary, faceSet);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, FaceSetValueExpressionDriver, idName, faceSet);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


    FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(const faceSet &set,const FaceSetValueExpressionDriver& orig)
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

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    SetSubsetValueExpressionDriver(dict,dict.lookup("setName"),INVALID),
    faceSet_(
        getSet<faceSet>(
            regionMesh(dict,mesh),
            dict.lookup("setName"),
            origin_
        )
    )
{
}

FaceSetValueExpressionDriver::FaceSetValueExpressionDriver(const word& id,const fvMesh&mesh)
 :
    SetSubsetValueExpressionDriver(
        id,
        INVALID,
        true,
        false
    ),
    faceSet_(
        getSet<faceSet>(
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
inline label SubsetValueExpressionDriver::getIndexFromIterator(const faceSet::const_iterator &it) 
{
    return it.key();
}

Field<scalar> *FaceSetValueExpressionDriver::getScalarField(const string &name)
{
    return getFieldInternalAndInterpolate<surfaceScalarField,volScalarField,faceSet,scalar>(name,faceSet_);
}

Field<vector> *FaceSetValueExpressionDriver::getVectorField(const string &name)
{
    return getFieldInternalAndInterpolate<surfaceVectorField,volVectorField,faceSet,vector>(name,faceSet_);
}

Field<tensor> *FaceSetValueExpressionDriver::getTensorField(const string &name)
{
    return getFieldInternalAndInterpolate<surfaceTensorField,volTensorField,faceSet,tensor>(name,faceSet_);
}

Field<symmTensor> *FaceSetValueExpressionDriver::getSymmTensorField(const string &name)
{
    return getFieldInternalAndInterpolate<surfaceSymmTensorField,volSymmTensorField,faceSet,symmTensor>(name,faceSet_);
}

Field<sphericalTensor> *FaceSetValueExpressionDriver::getSphericalTensorField(const string &name)
{
    return getFieldInternalAndInterpolate<surfaceSphericalTensorField,volSphericalTensorField,faceSet,sphericalTensor>(name,faceSet_);
}

vectorField *FaceSetValueExpressionDriver::makePositionField()
{
    return getFromFieldInternal(this->mesh().Cf(),faceSet_());
}

scalarField *FaceSetValueExpressionDriver::makeCellVolumeField()
{
    FatalErrorIn("FaceSetValueExpressionDriver::makeCellVolumeField()")
        << "faceSet knows nothing about cells"
            << endl
            << abort(FatalError);
    return new scalarField(0);
}


// vectorField *FaceSetValueExpressionDriver::makePointField()
// {
//     notImplemented("FaceSetValueExpressionDriver::makePointField");
// }

scalarField *FaceSetValueExpressionDriver::makeFaceFlipField()
{
    // inspired by the setsToZones-utility

    scalarField *result=new scalarField(faceSet_->size());
    word setName(faceSet_->name() + "SlaveCells");
    const fvMesh &mesh=this->mesh();

    SetOrigin origin=INVALID;

    cellSet cells(
        mesh,
        setName,
        getSet<cellSet>(
            mesh,
            setName,
            origin
        )
    );
    assert(origin!=INVALID);

    SortableList<label> faceLabels(faceSet_->toc());

    forAll(faceLabels, i)
    {
        label faceI = faceLabels[i];

        bool flip = false;

        if (mesh.isInternalFace(faceI))
        {
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
                FatalErrorIn("scalarField *FaceSetValueExpressionDriver::makeFaceFlipField()")
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
                        << abort(FatalError);
            }
        }
        else
        {
            if (cells.found(mesh.faceOwner()[faceI]))
            {
                flip = false;
            }
            else
            {
                flip = true;
            }
        }
    
        (*result)[i]= (flip ? -1 : 1 );
    }
 
    return result;
}

scalarField *FaceSetValueExpressionDriver::makeFaceAreaMagField()
{
    return getFromFieldInternal(this->mesh().magSf(),faceSet_());
}

vectorField *FaceSetValueExpressionDriver::makeFaceNormalField()
{
    autoPtr<vectorField> sf(this->makeFaceAreaField());
    autoPtr<scalarField> magSf(this->makeFaceAreaMagField());

    return new vectorField(sf()/magSf());
}

vectorField *FaceSetValueExpressionDriver::makeFaceAreaField()
{
    return getFromFieldInternal(this->mesh().Sf(),faceSet_());
}

bool FaceSetValueExpressionDriver::update()
{
    if(debug) {
        Pout << "FaceSet: update " << faceSet_->name() << endl;
    }

    return true;
}

// ************************************************************************* //

} // namespace
