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
    Foam::SampledSetValueExpressionDriver

Description

SourceFiles
    SampledSetValueExpressionDriverI.H
    SampledSetValueExpressionDriver.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SampledSetValueExpressionDriver_H
#define SampledSetValueExpressionDriver_H

#include "volFields.H"

#include "sampledSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "SubsetValueExpressionDriver.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SampledSetValueExpressionDriver Declaration
\*---------------------------------------------------------------------------*/

class SampledSetValueExpressionDriver
:
    public SubsetValueExpressionDriver
{
    static word driverName_;

    // Private data
    const sampledSet &theSet_;

    // Private Member Functions

    template<class Type,class volField,class surfField>
    Field<Type> *sampleOrInterpolateInternal(const word &name,bool oldTime=false);

    static void setDebug();

protected:
    //- update stuff the set
    virtual bool update();

    //- should we interpolate?
    bool interpolate_;

    //- what kind of interpolation
    word interpolationType_;
public:

     //- Runtime type information
    TypeName("SampledSetValueExpressionDriver");

    static const word &driverName() { return driverName_; }

    // Constructors

    //- Construct from components
    SampledSetValueExpressionDriver(
        sampledSet &,
        bool autoInterpolate=false,
        bool warnAutoInterpolate=true
    );

    //- Construct from an id
    SampledSetValueExpressionDriver(const word& id,const fvMesh&mesh);

    //- Construct from a dictionary
    SampledSetValueExpressionDriver(const dictionary& dict,const fvMesh&mesh);

//     //- Copy construct
    SampledSetValueExpressionDriver(const sampledSet &,const SampledSetValueExpressionDriver&);

    autoPtr<CommonValueExpressionDriver> clone() const
        {
            return autoPtr<CommonValueExpressionDriver>(
                new SampledSetValueExpressionDriver(theSet_,*this)
            );
        }

    // Destructor

    ~SampledSetValueExpressionDriver();


    // Member Functions

    virtual vectorField *makePositionField();

    virtual scalarField *makeFaceAreaMagField();

    virtual scalarField *makeCellVolumeField();

    //     virtual vectorField *makePointField()=0;

    virtual scalarField *makeFaceFlipField();

    virtual vectorField *makeFaceNormalField();

    virtual vectorField *makeFaceAreaField();

//     template<class Type>
//     Field<Type> *getField(const string &name);

    virtual bool isCell() { return false; }

    virtual Field<scalar> *getScalarField(const string &name,bool oldTime=false);

    virtual Field<vector> *getVectorField(const string &name,bool oldTime=false);

    virtual Field<tensor> *getTensorField(const string &name,bool oldTime=false);

    virtual Field<symmTensor> *getSymmTensorField(const string &name,bool oldTime=false);

    virtual Field<sphericalTensor> *getSphericalTensorField(const string &name,bool oldTime=false);

    // Handling the parser
    //    void parse(const std::string& f);

    virtual label size() const;

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

#include "SampledSetValueExpressionDriverI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
