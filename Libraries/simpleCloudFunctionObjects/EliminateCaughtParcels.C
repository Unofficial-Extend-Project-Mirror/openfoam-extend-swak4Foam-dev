/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
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

\*---------------------------------------------------------------------------*/

#include "EliminateCaughtParcels.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::write()
{
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::EliminateCaughtParcels<CloudType>::EliminateCaughtParcels
(
    const dictionary& dict,
    CloudType& owner,
    const word& modelName
)
:
    CloudFunctionObject<CloudType>(dict, owner, modelName, typeName),
    faceHitCounter_(),
    lastFace_(),
    lastPosition_(),
    maxNumberOfHits_(
        readLabel(dict.lookup("maximumNumberOfHits"))
    ),
    minDistanceMove_(
        readScalar(dict.lookup("minDistanceMove"))
    )
{}


template<class CloudType>
Foam::EliminateCaughtParcels<CloudType>::EliminateCaughtParcels
(
    const EliminateCaughtParcels<CloudType>& ppm
)
:
    CloudFunctionObject<CloudType>(ppm),
    faceHitCounter_(ppm.faceHitCounter_),
    lastFace_(ppm.lastFace_),
    lastPosition_(ppm.lastPosition_),
    maxNumberOfHits_(ppm.maxNumberOfHits_),
    minDistanceMove_(ppm.minDistanceMove_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::EliminateCaughtParcels<CloudType>::~EliminateCaughtParcels()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::preEvolve()
{
    Info << this->modelName() << ":" << this->modelType()
        << ": Clearing data" << endl;

    faceHitCounter_.clear();
    lastFace_.clear();
    lastPosition_.clear();
}

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::postEvolve()
{
}

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::postFace
(
    const parcelType& p,
    const label faceI,
    bool& keepParticle
)
{
    const labelPair theId(p.origProc(), p.origId());

    hitTableType::iterator iter =
        faceHitCounter_.find(theId);

    if(faceI<this->owner().mesh().nInternalFaces()) {
        if(iter != faceHitCounter_.end()) {
            // Clear existing data because this is an internal face
            iter()=0;
            lastFace_[theId]=-1;
            lastPosition_[theId]=vector::one*HUGE;
        }
    } else {
        if (iter != faceHitCounter_.end())
        {
            if(minDistanceMove_>0) {
                const scalar moved=mag(lastPosition_[theId]-p.position());

                if(
                    moved
                    <
                    minDistanceMove_
                ) {
                    Pout << this->modelName() << ":" << this->modelType()
                        << "Eliminating particle because it only moved " << moved << nl
                        << p << endl;

                    keepParticle=false;
                }
            }
            if(maxNumberOfHits_>1) {
                if(lastFace_[theId]==faceI) {
                    iter()++;
                    if(
                        iter()
                        >
                        maxNumberOfHits_
                    ) {
                        Pout << this->modelName() << ":" << this->modelType()
                            << "Eliminating particle because it hit face " << faceI
                            << " for " << iter() << " times in a row" << nl
                            << p << endl;

                        keepParticle=false;
                    }
                } else {
                    // different face. Reset
                    iter()=1;
                    lastFace_[theId]=faceI;
                }
            }
            lastPosition_[theId]=p.position();
        }
        else
        {
            // First time this particle is recorded
            faceHitCounter_.insert(theId, 1);
            lastFace_.insert(theId,faceI);
            lastPosition_.insert(theId,p.position());
        }
    }
}


// ************************************************************************* //
