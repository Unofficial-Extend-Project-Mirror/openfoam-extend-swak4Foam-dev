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

Class
    Foam::CellSetValueExpressionDriver

Description

SourceFiles
    CellSetValueExpressionDriverI.H
    CellSetValueExpressionDriver.C

Contributors/Copyright:
    2010-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CellSetValueExpressionDriver_H
#define CellSetValueExpressionDriver_H

#include "volFields.H"

#include "cellSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SetSubsetValueExpressionDriver.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class CellSetValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class CellSetValueExpressionDriver
:
    public SetSubsetValueExpressionDriver
{

    static word driverName_;

    // Private data

    autoPtr<cellSet> cellSet_;

    // Private Member Functions

protected:

    //- update stuff that needs updating
    virtual bool update();

    //- weights of the entries if it is not a point field

    //- parametert size is the expected size of the weight field and
    //- will be used to determine whether this is correct.
    virtual tmp<scalarField> weightsNonPoint(
        label size
    ) const;

public:

     //- Runtime type information
    TypeName("CellSetValueExpressionDriver");

    static const word &driverName() { return driverName_; }

    //- entity this driver operates on
    typedef cellSet EntityType;

   // Constructors

    //- Construct from components
    CellSetValueExpressionDriver(const cellSet &);

    //- Construct from a dictionary
    CellSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh);

    //- Construct from an id
    CellSetValueExpressionDriver(const word& id,const fvMesh&mesh);

    //- Copy construct
    CellSetValueExpressionDriver(
        const cellSet &,
        const CellSetValueExpressionDriver&
    );

    autoPtr<CommonValueExpressionDriver> clone() const
        {
            return autoPtr<CommonValueExpressionDriver>(
                new CellSetValueExpressionDriver(cellSet_(),*this)
            );
        }

    // Destructor

    ~CellSetValueExpressionDriver();


    // Member Functions

    virtual tmp<vectorField> makePositionField() const;

    virtual tmp<scalarField> makeFaceAreaMagField() const;

    virtual tmp<scalarField> makeCellVolumeField() const;

    //     virtual tmp<vectorField> makePointField()=0;

    virtual tmp<scalarField> makeFaceFlipField() const;

    virtual tmp<vectorField> makeFaceNormalField() const;

    virtual tmp<vectorField> makeFaceAreaField() const;

//     template<class Type>
//     Field<Type> *getField(const string &name);

    virtual bool isCell() { return true; }

    virtual tmp<Field<scalar> > getScalarField(
        const word &name,
        bool oldTime=false
    );

    virtual tmp<Field<vector> > getVectorField(
        const word &name,
        bool oldTime=false
    );

    virtual tmp<Field<tensor> > getTensorField(
        const word &name,
        bool oldTime=false
    );

    virtual tmp<Field<symmTensor> > getSymmTensorField(
        const word &name,
        bool oldTime=false
    );

    virtual tmp<Field<sphericalTensor> > getSphericalTensorField(
        const word &name,
        bool oldTime=false
    );

    // Handling the parser
    //    void parse(const std::string& f);

    inline virtual label size() const;

    inline virtual label pointSize() const;

    inline virtual const fvMesh &mesh() const;

//     template<class Type>
//     virtual Field<Type> *toFace(const Field<Type> &)=0;

//     template<class Type>
//     virtual Field<Type> *toPoint(const Field<Type> &)=0;

    //- return a new plugin-function
    virtual autoPtr<CommonPluginFunction> newPluginFunction(
        const word &name
    );

    //- tests for a plugin-function
    virtual bool existsPluginFunction(
        const word &name
    );

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "CellSetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
