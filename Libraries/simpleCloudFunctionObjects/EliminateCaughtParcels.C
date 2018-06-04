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

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017-2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "EliminateCaughtParcels.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::write()
{
    if (eliminatedPtr_.valid())
    {
        eliminatedPtr_->write();

        eliminatedPtr_->clear();
    }
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
    ),
    toEliminate_(),
    eliminatedPtr_(nullptr),
    out_(
        this->outputDir(),
        this->owner().time()
    )
{
    out_.addSpec(
        "eliminated.*",
        "sum"
    );
}


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
    minDistanceMove_(ppm.minDistanceMove_),
    toEliminate_(ppm.toEliminate_),
    eliminatedPtr_(ppm.eliminatedPtr_),
    out_(
        this->outputDir(),
        this->owner().time()
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::EliminateCaughtParcels<CloudType>::~EliminateCaughtParcels()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::preEvolve()
{
    Info << this->modelName() << ":" << this->owner().name()
        << ":" << this->modelType()
        << ": Clearing data" << endl;

    faceHitCounter_.clear();
    lastFace_.clear();
    lastPosition_.clear();
    toEliminate_.clear();

    if (!eliminatedPtr_.valid())
    {
        eliminatedPtr_.reset
            (
                this->owner().cloneBare(
                    this->owner().name()
                    +
                    this->modelName()
                    +
                    "Eliminated"
                ).ptr()
            );
    }
}

template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::postEvolve()
{
    label nrEliminated=toEliminate_.size();
    label totalEliminated=
        nrEliminated
        +
        this->template getModelProperty<scalar>("nrEliminated");

    this->template setModelProperty<scalar>(
        "nrEliminated",
        totalEliminated
    );
    if(Pstream::parRun()) {
        out_["eliminatedCpu"+Foam::name(Pstream::myProcNo())]
            << nrEliminated << endl;
    }
    reduce(nrEliminated,plusOp<label>());
    if(Pstream::parRun()) {
        totalEliminated=
            nrEliminated
            +
            this->template getModelProperty<scalar>("nrEliminatedAllProc");

        this->template setModelProperty<scalar>(
            "nrEliminatedAllProc",
            totalEliminated
        );
    }

    if(Pstream::master()) {
        out_["eliminatedTotal"]
            << nrEliminated << endl;
    }

    if(totalEliminated>0) {
        Info << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
            << " : " << nrEliminated << " parcels eliminated this timestep. "
            << totalEliminated << " in total" << endl;
    }

    CloudFunctionObject<CloudType>::postEvolve();
}


template<class CloudType>
void Foam::EliminateCaughtParcels<CloudType>::postMove
(
    typename CloudType::parcelType& p,
    const label cellI,
    const scalar dt,
    const point& position0,
    bool& keepParticle
)
{
    const labelPair theId(p.origProc(), p.origId());
    if(toEliminate_.found(theId)) {
        if(keepParticle) {
            Pout << this->modelName() << ":" << this->owner().name()
                << ":" << this->modelType()
                << ": removing " << theId << " ... postMove" << endl;
        }
        keepParticle=false;
    }
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
            lastPosition_[theId]=vector::one*pTraits<scalar>::max;
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
                    if(keepParticle) {
                        // only print this once
                        Pout << this->modelName() << ":" << this->owner().name()
                            << ":" << this->modelType()
                            << "Eliminating particle because it only moved " << moved << nl
                            << p << endl;

                        keepParticle=false;
#ifdef FOAM_KINEMATIC_PARCEL_ACTIVE_SET_THROUGH_METHOD
                        const_cast<parcelType&>(p).active(false);
#else
                        const_cast<parcelType&>(p).active()=false;
#endif
                        if(!toEliminate_.found(theId)) {
                            toEliminate_.insert(theId);
                            eliminatedPtr_->append(
                                p.clone(this->owner().mesh()).ptr()
                            );
                        }
                    }
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
                        // only print this once
                        if(keepParticle) {
                            Pout << this->modelName() << ":" << this->owner().name()
                                << ":" << this->modelType()
                                << "Eliminating particle because it hit face " << faceI
                                << " for " << iter() << " times in a row" << nl
                                << p << endl;

                            keepParticle=false;
#ifdef FOAM_KINEMATIC_PARCEL_ACTIVE_SET_THROUGH_METHOD
                            const_cast<parcelType&>(p).active(false);
#else
                            const_cast<parcelType&>(p).active()=false;
#endif
                            if(!toEliminate_.found(theId)) {
                                toEliminate_.insert(theId);
                                eliminatedPtr_->append(
                                    p.clone(this->owner().mesh()).ptr()
                                );
                            }
                        };
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
