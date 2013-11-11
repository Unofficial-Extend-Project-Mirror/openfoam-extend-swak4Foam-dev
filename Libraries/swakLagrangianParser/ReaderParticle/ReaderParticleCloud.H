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

    HashTable<Field<scalar>,word> scalarData_;

    template <typename T>
    void addDataInternal(
        const word &name,
        const Field<T> &inData,
        HashTable<Field<T>,word> &data
    );

protected:

    void addScalarField(const word &name);

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

    friend class ReaderParticle;

    wordList getScalarFieldNames() const { return scalarData_.toc(); }

    const Field<scalar> &getScalarField(const word &name) const
        { return scalarData_[name]; }

    bool hasScalar(const word &name) const
        { return scalarData_.found(name); }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "ReaderParticleCloudI.H"
#endif

#endif

// ************************************************************************* //
