/*---------------------------------------------------------------------------*\
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
    2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"
#include "shiftFieldGeneralPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "meshToMesh.H"
#include "MeshInterpolationOrder.H"

namespace Foam {

    // defineTemplateTypeNameAndDebug(shiftFieldGeneralPluginFunction,1);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
shiftFieldGeneralPluginFunction<Type,Order>::shiftFieldGeneralPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const string &shiftDescription
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word(ResultType::typeName),
        "field internalField "+ResultType::typeName+","+shiftDescription
    ),
    defaultValue_(
        pTraits<Type>::zero
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type,meshToMeshOrder Order>
void shiftFieldGeneralPluginFunction<Type,Order>::doEvaluation()
{
    const fvMesh &origMesh=this->mesh();
    fvMesh shiftMesh(
        IOobject(
            origMesh.name()+"Shifted",
            origMesh.polyMesh::instance(),
            origMesh.polyMesh::db()
        ),
        Xfer<pointField>(origMesh.points()),
        Xfer<faceList>(origMesh.faces()),
        Xfer<labelList>(origMesh.faceOwner()),
        Xfer<labelList>(origMesh.faceNeighbour())
    );
    {
        const polyBoundaryMesh &origBound=origMesh.boundaryMesh();
        List<polyPatch*> newBound(origBound.size(),NULL);
        forAll(origBound,patchI) {
            newBound[patchI]=origBound[patchI].clone(shiftMesh.boundaryMesh()).ptr();
        }
        shiftMesh.removeFvBoundary();

#ifdef FOAM_MESHTOMESH_OLD_STYLE
        shiftMesh.addFvPatches(newBound);
#else
        shiftMesh.addPatches(newBound);
#endif

#ifdef FOAM_MESHTOMESH_HAS_CORRECTEDCELLVOLUMEWEIGHT
        shiftMesh.fvSchemes::merge(
            origMesh.schemesDict()
        );
        shiftMesh.fvSchemes::readOpt()=IOobject::READ_IF_PRESENT;
        shiftMesh.fvSchemes::read();
        //        const_cast<dictionary&>(shiftMesh.schemesDict())=origMesh.schemesDict();
        // Info << origMesh.schemesDict() << endl;
        // Info << shiftMesh.schemesDict().name() << endl;
        // Info << shiftMesh.schemesDict() << endl;
        // Info << shiftMesh.gradScheme("valsShift") << endl;
#endif
    }

    pointField newPoints(origMesh.points()-this->displacement());
    shiftMesh.movePoints(newPoints);

    ResultType newField(
        IOobject(
            field_->name()+"Shift",
            shiftMesh
        ),
        shiftMesh,
        field_->dimensions(),
        field_->internalField(),
        field_->boundaryField()
    );

    autoPtr<ResultType> mappedField(
        new ResultType(
            field_()
        )
    );
    ResultType &initField=mappedField();
#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
    const_cast<Field<Type>&>(initField.internalField().field())=defaultValue_;
#else
    initField.internalField()=defaultValue_;
#endif
    initField.correctBoundaryConditions();

    meshToMesh interpolation(
        shiftMesh,
        origMesh
#ifdef FOAM_NEW_MESH2MESH
        //        meshToMesh::imCellVolumeWeight,   // strange patterns
        //        meshToMesh::imDirect,  // almost no  fitting
        //        meshToMesh::imMapNearest, // stable. No default
        ,Order
#endif
#ifndef FOAM_MESHTOMESH_OLD_STYLE
        ,false
#endif
    );

#ifdef FOAM_NEW_MESH2MESH
#if defined(FOAM_MESH2MESH_NO_2ND_ORDER_TENSOR) && !defined(FOAM_MESHTOMESH_MAPSRCTOTGT_REDUCE)
    interpolation.mapSrcToTgt(
        newField,
        initField
    );
#else
    interpolation.mapSrcToTgt(
        newField,
        eqOp<Type>(),
        initField
    );
#endif
#else
    interpolation.interpolate(
#ifdef FOAM_MESHTOMESH_OLD_STYLE
        initField,
#endif
        newField,
        //        meshToMesh::imCellVolumeWeight,
        Order
#ifdef FOAM_MESHTOMESH_INTERPOLATE_REDUCE
        ,eqOp<Type>(),
#endif
#ifndef FOAM_MESHTOMESH_OLD_STYLE
        ,initField
#endif
    );
#endif

    result().setObjectResult(
        mappedField
    );
}

template<class Type,meshToMeshOrder Order>
void shiftFieldGeneralPluginFunction<Type,Order>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0);

    field_.set(
        new ResultType(
            dynamic_cast<const FieldValueExpressionDriver &>(
                driver
            ).getResult<ResultType>()
        )
    );
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template
class shiftFieldGeneralPluginFunction<scalar,SHIFT_METHOD>;
template
class shiftFieldGeneralPluginFunction<vector,SHIFT_METHOD>;
#ifndef FOAM_MESHTOMESH_MAPSRCTOTGT_NO_TENSOR
template
class shiftFieldGeneralPluginFunction<tensor,SHIFT_METHOD>;
template
class shiftFieldGeneralPluginFunction<symmTensor,SHIFT_METHOD>;
template
class shiftFieldGeneralPluginFunction<sphericalTensor,SHIFT_METHOD>;
#endif

template
class shiftFieldGeneralPluginFunction<scalar,MAP_METHOD>;
template
class shiftFieldGeneralPluginFunction<vector,MAP_METHOD>;
#ifndef FOAM_MESHTOMESH_MAPSRCTOTGT_NO_TENSOR
template
class shiftFieldGeneralPluginFunction<tensor,MAP_METHOD>;
template
class shiftFieldGeneralPluginFunction<symmTensor,MAP_METHOD>;
template
class shiftFieldGeneralPluginFunction<sphericalTensor,MAP_METHOD>;
#endif

} // namespace

// ************************************************************************* //
