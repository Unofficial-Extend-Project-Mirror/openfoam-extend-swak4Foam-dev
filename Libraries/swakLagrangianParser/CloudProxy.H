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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::CloudProxy

Description
    Class that interfaces to the actual clouds

SourceFiles
    CloudProxy.C

Contributors/Copyright:
    2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudProxy_H
#define CloudProxy_H

#include "cloud.H"
#include "Field.H"

#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class CloudProxy;

Ostream &operator<<(Ostream &,const CloudProxy &);

/*---------------------------------------------------------------------------*\
                       Class CloudProxy Declaration
\*---------------------------------------------------------------------------*/

class CloudProxy
{
    // Private data

    const cloud &cloud_;

    //- types of the available functions
    HashTable<word,word> fieldTypes_;

    //- descriptions of the functions
    HashTable<string,word> fieldDescriptions_;

    //- Disallow default bitwise assignment
    void operator=(const CloudProxy&);

    //- Disallow copy construct
    CloudProxy(const CloudProxy &);

protected:

    //- add description of a "field"
    template<class Type>
    void addField(
        const word &name,
        const string &description
    );

    const cloud &theCloud() const
        { return cloud_; }

public:

    //- Runtime type information
    TypeName("CloudProxy");


    // Constructors

        //- Construct from components
        CloudProxy
        (
            const cloud &
        );

    // Declare run-time constructor selection table

    declareRunTimeSelectionTable
    (
        autoPtr,
        CloudProxy,
        cloud,
        (
            const cloud& aCloud
        ),
        (aCloud)
    );

    static autoPtr<CloudProxy> New(
        const cloud &c,
        const word& alternateType=""
    );

    static autoPtr<CloudProxy> New(
        const cloud &c,
        const dictionary& dict
    );

    // Destructor

        virtual ~CloudProxy();

    bool hasField(const word &name) const {
        return fieldTypes_.found(name);
    }

    word fieldType(const word &name) const {
        return fieldTypes_[name];
    }

    bool isScalarField(const word &name) const;
    virtual tmp<Field<scalar> > getScalarField(
        const word &name
    ) const = 0;

    bool isVectorField(const word &name) const;
    virtual tmp<Field<vector> > getVectorField(
        const word &name
    ) const = 0;

    bool isTensorField(const word &name) const;
    virtual tmp<Field<tensor> > getTensorField(
        const word &name
    ) const = 0;

    bool isSymmTensorField(const word &name) const;
    virtual tmp<Field<symmTensor> > getSymmTensorField(
        const word &name
    ) const = 0;

    bool isSphericalTensorField(const word &name) const;
    virtual tmp<Field<sphericalTensor> > getSphericalTensorField(
        const word &name
    ) const = 0;

    bool isBoolField(const word &name) const;
    virtual tmp<Field<bool> > getBoolField(
        const word &name
    ) const = 0;

    virtual tmp<Field<vector> > getPositions() const = 0;

    virtual tmp<Field<label> > getCells() const = 0;

    virtual tmp<Field<scalar> > weights() const = 0;

    virtual label size() const = 0;

    friend Ostream &operator<<(Ostream &,const CloudProxy &);

};

#define addCloudProxyToTable(ProxyType,CloudType)\
    CloudProxy::addcloudConstructorToTable                              \
    <ProxyType<CloudType> >                                             \
    add##ProxyType##CloudType##cloudConstructorToCloudProxyTable(       \
        Cloud<CloudType::particleType>::typeName)

#define addCloudProxyToTable2(ProxyType,CloudType,ParcelType)              \
    CloudProxy::addcloudConstructorToTable                              \
    <ProxyType<CloudType> >                                             \
    add##ProxyType##CloudType##cloudConstructorToCloudProxyTable(       \
        Cloud<ParcelType>::typeName)

    // This variation is used to add proxies that work in CloudFunctionObjects
#define addKinematicCloudProxyToTable(ProxyType,CloudType)      \
    CloudProxy::addcloudConstructorToTable                              \
    <ProxyType<CloudType::kinematicCloudType> >                         \
    addKinematic##ProxyType##CloudType##cloudConstructorToCloudProxyTable( \
        word("kinematic")+Cloud<CloudType::particleType>::typeName)

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "CloudProxyI.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
