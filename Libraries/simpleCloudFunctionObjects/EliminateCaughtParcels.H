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
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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
    Foam::EliminateCaughtParcels

Description
    Eliminate parcels that were caught in unfortunate conditions (usually
    corners) and keep colliding with the walls for a large number of times

    Repeated offenders are caught if they hit the same patch face for a
    specified number of times without hitting any other faces between that
    and not moving significant amount between the hits

    The offending particle is marked to not be kept and reported to the terminal

SourceFiles
    EliminateCaughtParcels.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

    Based on the CloudTracks-function object

\*---------------------------------------------------------------------------*/

#ifndef EliminateCaughtParcels_H
#define EliminateCaughtParcels_H

#include "CloudFunctionObject.H"

#include "swakCloudTypes.H"

#include "TimelineCollection.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class EliminateCaughtParcels Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class EliminateCaughtParcels
:
    public CloudFunctionObject<CloudType>
{
    // Private data

    // Typedefs

    //- Convenience typedef for parcel type
    typedef typename CloudType::parcelType parcelType;

    //- Convenience typedef for hash hit-table
    typedef HashTable<label, labelPair, typename labelPair::Hash<> > hitTableType;
    typedef HashTable<vector, labelPair, typename labelPair::Hash<> > hitPositionTableType;


    //- Table of number of times a particle has hit a face
    hitTableType faceHitCounter_;

    //- Table of the last face a particle has hit
    hitTableType lastFace_;

    //- Table of the last position a particle has hit
    hitPositionTableType lastPosition_;

    //- number of times a particle may hit the same patch face
    label maxNumberOfHits_;

    //- distance below which it is assumed that the particle has not moved
    scalar minDistanceMove_;

    //- remember particles to eliminate because setting keepParticle to false in postPatch is not sufficient
    HashSet<labelPair, typename labelPair::Hash<> > toEliminate_;

    //- Pointer to the cloud storage
    autoPtr<Cloud<parcelType> > eliminatedPtr_;

    //- here we get our files from
    TimelineCollection out_;

protected:

    // Protected member functions

    //- Write post-processing info
    void write();


public:

    //- Runtime type information
    TypeName("eliminateCaughtParcels");


    // Constructors

    //- Construct from dictionary
    EliminateCaughtParcels
    (
        const dictionary& dict,
        CloudType& owner,
        const word& modelName
    );

    //- Construct copy
    EliminateCaughtParcels(const EliminateCaughtParcels<CloudType>& ppm);

    //- Construct and return a clone
    virtual autoPtr<CloudFunctionObject<CloudType> > clone() const
        {
            return autoPtr<CloudFunctionObject<CloudType> >
                (
                    new EliminateCaughtParcels<CloudType>(*this)
                );
        }


    //- Destructor
    virtual ~EliminateCaughtParcels();


    // Member Functions

    // Evaluation

    //- Pre-evolve hook
    virtual void preEvolve();

    //- Post-evolve hook
    virtual void postEvolve();

    //- Post-move hook
    virtual void postMove
    (
        typename CloudType::parcelType& p,
        const label cellI,
        const scalar dt,
        const point& position0,
        bool& keepParticle
    );

    //- Post-face hook
    virtual void postFace
    (
        const parcelType& p,
        const label faceI,
        bool& keepParticle
    );


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "EliminateCaughtParcels.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
