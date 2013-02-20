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

#include "CellSetValueExpressionDriver.H"
#include "CellSetValuePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CellSetValueExpressionDriver, 0);

word CellSetValueExpressionDriver::driverName_="cellSet";

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellSetValueExpressionDriver, dictionary, cellSet);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellSetValueExpressionDriver, idName, cellSet);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


    CellSetValueExpressionDriver::CellSetValueExpressionDriver(const cellSet &set,const CellSetValueExpressionDriver& orig)
:
        SetSubsetValueExpressionDriver(orig),
        cellSet_(
            new cellSet(
                dynamic_cast<const fvMesh&>(set.db()),
                //            dynamicCast<const fvMesh&>(set.db()), // doesn't work with f++ 4.2
                //            set.name()+"_copy",
                set.name(),
                set
            )
        )
{}

CellSetValueExpressionDriver::CellSetValueExpressionDriver(const cellSet &set)
:
    SetSubsetValueExpressionDriver(set.name(),NEW),
    cellSet_(
        new cellSet
        (
            dynamic_cast<const fvMesh&>(set.db()),
            //            dynamicCast<const fvMesh&>(set.db()), // doesn't work with gcc 4.2
            //            set.name()+"_copy",
            set.name(),
            set
        )
    )
{}

CellSetValueExpressionDriver::CellSetValueExpressionDriver(const word& id,const fvMesh&mesh)
 :
    SetSubsetValueExpressionDriver(id,INVALID),
    cellSet_(
        getSet<cellSet>(
            mesh,
            id,
            origin_
        )
    )
{
}

CellSetValueExpressionDriver::CellSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh)
 :
    SetSubsetValueExpressionDriver(dict,dict.lookup("setName"),NEW),
    cellSet_(
        getSet<cellSet>(
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
// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CellSetValueExpressionDriver::~CellSetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
inline label SubsetValueExpressionDriver::getIndexFromIterator(
    const cellSet::const_iterator &it
) const
{
    return it.key();
}

tmp<Field<scalar> > CellSetValueExpressionDriver::getScalarField(
        const string &name,bool oldTime
)
{
    return getFieldInternal<volScalarField,cellSet,scalar>(
        name,cellSet_,oldTime
    );
}

tmp<Field<vector> > CellSetValueExpressionDriver::getVectorField(
    const string &name,bool oldTime
)
{
    return getFieldInternal<volVectorField,cellSet,vector>(
        name,cellSet_,oldTime
    );
}

tmp<Field<tensor> > CellSetValueExpressionDriver::getTensorField(
    const string &name,bool oldTime
)
{
    return getFieldInternal<volTensorField,cellSet,tensor>(
        name,cellSet_,oldTime
    );
}

tmp<Field<symmTensor> > CellSetValueExpressionDriver::getSymmTensorField(
    const string &name,bool oldTime
)
{
    return getFieldInternal<volSymmTensorField,cellSet,symmTensor>(
        name,cellSet_,oldTime
    );
}

tmp<Field<sphericalTensor> > CellSetValueExpressionDriver::getSphericalTensorField(
    const string &name,bool oldTime
)
{
    return getFieldInternal<volSphericalTensorField,cellSet,sphericalTensor>(
        name,cellSet_,oldTime
);
}

tmp<vectorField> CellSetValueExpressionDriver::makePositionField() const
{
    return getFromFieldInternal(this->mesh().C(),cellSet_());
}

tmp<scalarField> CellSetValueExpressionDriver::makeCellVolumeField() const
{
    return getFromFieldInternal(this->mesh().V(),cellSet_());
}


// tmp<vectorField> CellSetValueExpressionDriver::makePointField() const
// {
//     notImplemented("CellSetValueExpressionDriver::makePointField");
// }

tmp<scalarField> CellSetValueExpressionDriver::makeFaceAreaMagField() const
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceAreaField()")
        << "cellSet knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<scalarField>(new scalarField(0));
}

tmp<scalarField> CellSetValueExpressionDriver::makeFaceFlipField() const
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceFlipField()")
        << "cellSet knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<scalarField>(new scalarField(0));
}

tmp<vectorField> CellSetValueExpressionDriver::makeFaceNormalField() const
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceNormalField()")
        << "cellSet knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<vectorField>(new vectorField(0));
}

tmp<vectorField> CellSetValueExpressionDriver::makeFaceAreaField() const
{
    FatalErrorIn("CellSetValueExpressionDriver::makeFaceAreaField()")
        << "cellSet knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<vectorField>(new vectorField(0));
}

bool CellSetValueExpressionDriver::update()
{
    if(debug) {
        Pout << "CellSet: update " << cellSet_->name()
            << endl;
    }

    return updateSet(cellSet_,id_,origin_);
}

autoPtr<CommonPluginFunction> CellSetValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        CellSetValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool CellSetValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return CellSetValuePluginFunction::exists(
        *this,
        name
    );
}

tmp<scalarField> CellSetValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label cellSize=this->size();
    bool isCell=(size==cellSize);
    reduce(isCell,andOp<bool>());

    if(!cellSize) {
        Pout << "Expected size: " << size
            << " Cell size: " << cellSize << endl;

        FatalErrorIn("CellSetValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(makeCellVolumeField());
}

// ************************************************************************* //

} // namespace
