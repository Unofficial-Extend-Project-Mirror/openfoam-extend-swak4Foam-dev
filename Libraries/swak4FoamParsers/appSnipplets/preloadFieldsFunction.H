/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Description
    Load fields into memory if it exists

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "DynamicList.H"
#include "PtrList.H"

template<class FieldType>
void preLoadFieldsFunction(
    const DynamicList<fvMesh*> &meshes,
    const wordList &fieldNames,
    PtrList<FieldType> &fieldList
)
{
    forAll(meshes,m) {
        const fvMesh &mesh=*(meshes[m]);

        forAll(fieldNames,i) {
            const word &name=fieldNames[i];

            IOobject fieldHeader
                (
                    name,
                    mesh.time().timeName(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                );

            if
            (
#ifdef FOAM_HAS_TYPE_HEADER_OK
                fieldHeader.typeHeaderOk<FieldType>(true
#ifdef FOAM_TYPE_HEADER_OK_HAS_EXTRA_PARAMTERS
                                                    ,true,false
#endif
                                                    )
#else
                fieldHeader.headerOk()
                && fieldHeader.headerClassName() == pTraits<FieldType>::typeName
#endif
            )
            {
                Info << " Preloading " << name << " of type "
                    << pTraits<FieldType>::typeName
                    << " for mesh " << mesh.name() << endl;

                label sz=fieldList.size();
                fieldList.setSize(sz+1);
                fieldList.set(sz, new FieldType(fieldHeader, mesh));
            }
        }
    }
}

#define PRELOAD_FIELDS_LISTS                \
    PtrList<volScalarField> vsf;                \
    PtrList<volVectorField> vvf;                \
    PtrList<volTensorField> vtf;                \
    PtrList<volSymmTensorField> vyf;            \
    PtrList<volSphericalTensorField> vhf;       \
                                                \
    PtrList<surfaceScalarField> ssf;            \
    PtrList<surfaceVectorField> svf;            \
    PtrList<surfaceTensorField> stf;            \
    PtrList<surfaceSymmTensorField> syf;        \
    PtrList<surfaceSphericalTensorField> shf;   \

#define DO_PRELOAD_FIELDS(meshList,nameList)        \
    preLoadFieldsFunction(meshList,nameList,vsf);   \
    preLoadFieldsFunction(meshList,nameList,vvf);   \
    preLoadFieldsFunction(meshList,nameList,vtf);   \
    preLoadFieldsFunction(meshList,nameList,vyf);   \
    preLoadFieldsFunction(meshList,nameList,vhf);   \
                                                    \
    preLoadFieldsFunction(meshList,nameList,ssf);   \
    preLoadFieldsFunction(meshList,nameList,svf);   \
    preLoadFieldsFunction(meshList,nameList,stf);   \
    preLoadFieldsFunction(meshList,nameList,syf);   \
    preLoadFieldsFunction(meshList,nameList,shf);
