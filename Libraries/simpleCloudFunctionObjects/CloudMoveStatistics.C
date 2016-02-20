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

#include "CloudMoveStatistics.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //

template<class CloudType>
void Foam::CloudMoveStatistics<CloudType>::write()
{
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CloudMoveStatistics<CloudType>::CloudMoveStatistics
(
    const dictionary& dict,
    CloudType& owner,
    const word& modelName
)
:
    CloudFunctionObject<CloudType>(dict, owner, modelName, typeName),
    faceHitCounter_()
{}


template<class CloudType>
Foam::CloudMoveStatistics<CloudType>::CloudMoveStatistics
(
    const CloudMoveStatistics<CloudType>& ppm
)
:
    CloudFunctionObject<CloudType>(ppm),
    faceHitCounter_(ppm.faceHitCounter_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CloudMoveStatistics<CloudType>::~CloudMoveStatistics()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::CloudMoveStatistics<CloudType>::preEvolve()
{
    Info << this->modelName() << ":" << this->modelType()
        << ": Clearing data" << endl;
    faceHitCounter_.clear();

    // Initialize with zero to make sure that particles that don't hit faces are counted as well
    forAllConstIter(typename CloudType, this->owner(), iter) {
        const typename CloudType::parcelType& p = iter();
        faceHitCounter_.insert(labelPair(p.origProc(), p.origId()), 0);
    }
}

template<class CloudType>
void Foam::CloudMoveStatistics<CloudType>::postEvolve()
{
    label faceHitNr=faceHitCounter_.size();
    label faceHitMin=labelMax;
    label faceHitMax=labelMin;
    label faceHitSum=0;
    forAllConstIter(hitTableType,faceHitCounter_,iter) {
        faceHitSum+=iter();
        faceHitMin=min(faceHitMin,iter());
        faceHitMax=max(faceHitMax,iter());
    }
    if(faceHitNr>0) {
        Pout << this->modelName() << ":" << this->modelType()
            << ": Face hit Nr: " << faceHitNr
            << " Min: " << faceHitMin
            << " Mean: " << float(faceHitSum)/faceHitNr
            << " Max: " << faceHitMax << endl;
    } else {
        Pout << this->modelName() << ":" << this->modelType()
            << ": No face hits" << endl;
    }
    if(Pstream::parRun()) {
        reduce(faceHitNr,plusOp<label>());
        reduce(faceHitSum,plusOp<label>());
        reduce(faceHitMin,minOp<label>());
        reduce(faceHitMax,maxOp<label>());
        if(faceHitNr>0) {
            Info << this->modelName() << ":" << this->modelType()
                << ": Face hit Nr: " << faceHitNr
                << " Min: " << faceHitMin
                << " Mean: " << float(faceHitSum)/faceHitNr
                << " Max: " << faceHitMax << endl;
        } else {
            Info << this->modelName() << ":" << this->modelType()
                << ": No face hits" << endl;
        }
    }
}

template<class CloudType>
void Foam::CloudMoveStatistics<CloudType>::postMove
(
    typename CloudType::parcelType& p,
    const label cellI,
    const scalar dt,
    const point& position0,
    bool& keepParticle
)
{
}

template<class CloudType>
void Foam::CloudMoveStatistics<CloudType>::postPatch
(
    const typename CloudType::parcelType& p,
    const polyPatch& pp,
    const scalar trackFraction,
    const tetIndices& testIs,
    bool& keepParticle
)
{
}

template<class CloudType>
void Foam::CloudMoveStatistics<CloudType>::postFace
(
    const parcelType& p,
    const label,
    bool&
)
{
    hitTableType::iterator iter =
        faceHitCounter_.find(labelPair(p.origProc(), p.origId()));

    if (iter != faceHitCounter_.end())
    {
        iter()++;
    }
    else
    {
        // particles that come from another processor will be counted on both
        faceHitCounter_.insert(labelPair(p.origProc(), p.origId()), 1);
    }
}


// ************************************************************************* //
