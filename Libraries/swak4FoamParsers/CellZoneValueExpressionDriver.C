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
    2010-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CellZoneValueExpressionDriver.H"
#include "CellZoneValuePluginFunction.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CellZoneValueExpressionDriver, 0);

word CellZoneValueExpressionDriver::driverName_="cellZone";

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellZoneValueExpressionDriver, dictionary, cellZone);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CellZoneValueExpressionDriver, idName, cellZone);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

label getCellZoneID(const fvMesh &mesh,const word &name)
{
    label result=mesh.cellZones().findZoneID(name);
    if(result<0) {
        FatalErrorIn("getCellZoneID(const fvMesh &mesh,const word &name)")
            << "The cellZone " << name << " was not found in "
                << mesh.cellZones().names()
                << endl
                << exit(FatalError);

    }
    return result;
}


CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(
    const cellZone &zone,
    const CellZoneValueExpressionDriver& orig
)
:
        SubsetValueExpressionDriver(orig),
        cellZone_(zone)
{}

CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(
    const cellZone &zone
)
:
    SubsetValueExpressionDriver(),
    cellZone_(zone)
{}

CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(
    const dictionary& dict,
    const fvMesh&mesh
)
 :
    SubsetValueExpressionDriver(dict),
    cellZone_(
        regionMesh(
            dict,
            mesh,
            searchOnDisc()
        ).cellZones()[
            getCellZoneID(
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

CellZoneValueExpressionDriver::CellZoneValueExpressionDriver(
    const word& id,
    const fvMesh&mesh
)
 :
    SubsetValueExpressionDriver(),
    cellZone_(
        mesh.cellZones()[
            getCellZoneID(
                mesh,
                id
            )
        ]
    )
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CellZoneValueExpressionDriver::~CellZoneValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<>
inline label SubsetValueExpressionDriver::getIndexFromIterator(
    const cellZone::const_iterator &it
) const
{
    return *it;
}

tmp<Field<scalar> > CellZoneValueExpressionDriver::getScalarField(
    const word &name,bool oldTime
)
{
    return getFieldInternal<volScalarField,cellZone,scalar>(
        name,cellZone_,oldTime
    );
}

tmp<Field<vector> > CellZoneValueExpressionDriver::getVectorField(
    const word &name,bool oldTime
)
{
    return getFieldInternal<volVectorField,cellZone,vector>(
        name,cellZone_,oldTime
    );
}

tmp<Field<tensor> > CellZoneValueExpressionDriver::getTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternal<volTensorField,cellZone,tensor>(
        name,cellZone_,oldTime
    );
}

tmp<Field<symmTensor> > CellZoneValueExpressionDriver::getSymmTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternal<volSymmTensorField,cellZone,symmTensor>(
        name,cellZone_,oldTime
    );
}

tmp<Field<sphericalTensor> > CellZoneValueExpressionDriver::getSphericalTensorField(
    const word &name,bool oldTime
)
{
    return getFieldInternal<volSphericalTensorField,cellZone,sphericalTensor>(
        name,cellZone_,oldTime
    );
}

tmp<vectorField> CellZoneValueExpressionDriver::makePositionField() const
{
    return getFromFieldInternal(this->mesh().C(),cellZone_);
}

tmp<scalarField> CellZoneValueExpressionDriver::makeCellVolumeField() const
{
    return getFromFieldInternal(this->mesh().V(),cellZone_);
}


// tmp<vectorField> CellZoneValueExpressionDriver::makePointField()
// {
//     notImplemented("CellZoneValueExpressionDriver::makePointField");
// }

tmp<scalarField> CellZoneValueExpressionDriver::makeFaceAreaMagField() const
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceAreaField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<scalarField>(
        new scalarField(0)
    );
}

tmp<vectorField> CellZoneValueExpressionDriver::makeFaceNormalField() const
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceNormalField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<vectorField>(
        new vectorField(0)
    );
}

tmp<scalarField> CellZoneValueExpressionDriver::makeFaceFlipField() const
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceFlipField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<scalarField>(
        new scalarField(0)
    );
}

tmp<vectorField> CellZoneValueExpressionDriver::makeFaceAreaField() const
{
    FatalErrorIn("CellZoneValueExpressionDriver::makeFaceAreaField()")
        << "cellZone knows nothing about faces"
            << endl
            << exit(FatalError);
    return tmp<vectorField>(
        new vectorField(0)
    );
}

autoPtr<CommonPluginFunction> CellZoneValueExpressionDriver::newPluginFunction(
    const word &name
) {
    return autoPtr<CommonPluginFunction>(
        CellZoneValuePluginFunction::New(
            *this,
            name
        ).ptr()
    );
}

bool CellZoneValueExpressionDriver::existsPluginFunction(
    const word &name
) {
    return CellZoneValuePluginFunction::exists(
        *this,
        name
    );
}


tmp<scalarField> CellZoneValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    const label cellSize=this->size();
    bool isCell=(size==cellSize);
    reduce(isCell,andOp<bool>());

    if(!isCell) {
        Pout << "Expected size: " << size
            << " Cell size: " << cellSize << endl;

        FatalErrorIn("CellZoneValueExpressionDriver::weightsNonPoint")
            << "Can not construct weight field of the expected size. "
                << " For sizes on the processors see above"
                << endl
                << exit(FatalError);
    }

    return tmp<scalarField>(makeCellVolumeField());
}

// ************************************************************************* //

} // namespace
