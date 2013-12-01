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
    \\  /    A nd           | Copyright  held by original author
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
    Foam::ReaderParticleCloud

Description
    A Cloud of passive particles

SourceFiles
    ReaderParticleCloud.C

Contributors/Copyright:
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef ReaderParticleCloud_H
#define ReaderParticleCloud_H

#include "Cloud.H"
#include "ReaderParticle.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class CloudRepository;

/*---------------------------------------------------------------------------*\
                   Class ReaderParticleCloud Declaration
\*---------------------------------------------------------------------------*/

class ReaderParticleCloud
:
    public Cloud<ReaderParticle>
{
    // Private Member Functions

        //- Disallow default bitwise copy construct
        ReaderParticleCloud(const ReaderParticleCloud&);

        //- Disallow default bitwise assignment
        void operator=(const ReaderParticleCloud&);

    HashTable<Field<label>,word> labelData_;
    HashTable<Field<scalar>,word> scalarData_;
    HashTable<Field<vector>,word> vectorData_;
    HashTable<Field<tensor>,word> tensorData_;
    HashTable<Field<symmTensor>,word> symmTensorData_;
    HashTable<Field<sphericalTensor>,word> sphericalTensorData_;
    //    HashTable<Field<bool>,word> boolData_;

    template <typename T>
    void addDataInternal(
        const word &name,
        const Field<T> &inData,
        HashTable<Field<T>,word> &data,
        bool overwrite=false
    );

protected:

    void addScalarField(const word &name);
    void addLabelField(const word &name);
    void addVectorField(const word &name);
    void addTensorField(const word &name);
    void addSymmTensorField(const word &name);
    void addSphericalTensorField(const word &name);
    //    void addBoolField(const word &name);

    template <typename T>
    void writeData(
        const HashTable<Field<T>,word> &data
    ) const;

    void clearData();

public:

    //- Type of parcel the cloud was instantiated for
    typedef ReaderParticle particleType;

    // Constructors

        //- Construct given mesh
        ReaderParticleCloud
        (
            const polyMesh&,
            const word& cloudName,
            bool readFields = true
        );

    static autoPtr<ReaderParticleCloud> makeCloudFromVariables(
            const polyMesh &mesh,
            const word &cloudName,
            const wordList & globalNameSpacesToUse,
            const word &positionVar
    );

    friend class ReaderParticle;

    template <typename T>
    void setValues(
        const word &name,
        const Field<T> &values
    );

    template <typename T>
    const Field<T> &getValues(
        const word &name
    );

    wordList getScalarFieldNames() const { return scalarData_.toc(); }

    const Field<scalar> &getScalarField(const word &name) const
        { return scalarData_[name]; }

    bool hasScalar(const word &name) const
        { return scalarData_.found(name); }

    wordList getLabelFieldNames() const { return labelData_.toc(); }

    const Field<label> &getLabelField(const word &name) const
        { return labelData_[name]; }

    bool hasLabel(const word &name) const
        { return labelData_.found(name); }

    wordList getVectorFieldNames() const { return vectorData_.toc(); }

    const Field<vector> &getVectorField(const word &name) const
        { return vectorData_[name]; }

    bool hasVector(const word &name) const
        { return vectorData_.found(name); }

    wordList getTensorFieldNames() const { return tensorData_.toc(); }

    const Field<tensor> &getTensorField(const word &name) const
        { return tensorData_[name]; }

    bool hasTensor(const word &name) const
        { return tensorData_.found(name); }

    wordList getSymmTensorFieldNames() const { return symmTensorData_.toc(); }

    const Field<symmTensor> &getSymmTensorField(const word &name) const
        { return symmTensorData_[name]; }

    bool hasSymmTensor(const word &name) const
        { return symmTensorData_.found(name); }

    wordList getSphericalTensorFieldNames() const { return sphericalTensorData_.toc(); }

    const Field<sphericalTensor> &getSphericalTensorField(const word &name) const
        { return sphericalTensorData_[name]; }

    bool hasSphericalTensor(const word &name) const
        { return sphericalTensorData_.found(name); }

    // wordList getBoolFieldNames() const { return boolData_.toc(); }

    // const Field<bool> &getBoolField(const word &name) const
    //     { return boolData_[name]; }

    // bool hasBool(const word &name) const
    //     { return boolData_.found(name); }

    friend class CloudRepository;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "ReaderParticleCloudI.H"
#endif

#endif

// ************************************************************************* //
