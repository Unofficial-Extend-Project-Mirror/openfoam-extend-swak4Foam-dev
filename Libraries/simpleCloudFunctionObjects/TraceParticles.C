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

#include "TraceParticles.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //

template<class CloudType>
void Foam::TraceParticles<CloudType>::write()
{
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::TraceParticles<CloudType>::TraceParticles
(
    const dictionary& dict,
    CloudType& owner,
    const word& modelName
)
:
    CloudFunctionObject<CloudType>(dict, owner, modelName, typeName),
    ids_(),
    out_(
        this->outputDir(),
        this->owner().time()
    )
{
    List<dictionary> idList(dict.lookup("particleIds"));
    forAll(idList,i) {
        const dictionary &d=idList[i];
        ids_.insert(
            labelPair(
                readLabel(d.lookup("origProc")),
                readLabel(d.lookup("origId"))
            ),
            i
        );
    }
    Info << this->modelName()
        << ": Tracing " << ids_.size() << " particles for cloud "
        << this->owner().name() << endl;
    out_.addSpec(
        "trace.*",
        "descr "+CloudType::parcelType::propertyList()
    );
}


template<class CloudType>
Foam::TraceParticles<CloudType>::TraceParticles
(
    const TraceParticles<CloudType>& ppm
)
:
    CloudFunctionObject<CloudType>(ppm),
    ids_(ppm.ids_),
    out_(
        this->outputDir(),
        this->owner().time()
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::TraceParticles<CloudType>::~TraceParticles()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::TraceParticles<CloudType>::preEvolve()
{
    label cnt=0;
    forAllConstIter(typename CloudType, this->owner(), iter) {
        const typename CloudType::parcelType& p = iter();
        if(ids_.found(labelPair(p.origProc(), p.origId()))) {
            writeParticle(
                p,
                "preEvolve"
            );
            cnt++;
        }
    }
    reduce(cnt,plusOp<label>());
    Info << this->modelName() << ":" << this->owner().name()
        << ":" << this->modelType()
        << ": tracing " << cnt << " particles" << endl;
}

template<class CloudType>
void Foam::TraceParticles<CloudType>::writeParticle(
    const typename CloudType::parcelType& p,
    const word &descr
) {
    out_(
        "trace"
        +
        name(
            ids_[
                labelPair(
                    p.origProc(),
                    p.origId()
                )
            ]
        )
    ) << descr << tab << p << endl;
}

template<class CloudType>
void Foam::TraceParticles<CloudType>::postEvolve()
{
    label cnt=0;
    forAllConstIter(typename CloudType, this->owner(), iter) {
        const typename CloudType::parcelType& p = iter();
        if(ids_.found(labelPair(p.origProc(), p.origId()))) {
            writeParticle(
                p,
                "postEvolve"
            );
            cnt++;
        }
    }
    reduce(cnt,plusOp<label>());
    Info << this->modelName() << ":" << this->owner().name()
        << ":" << this->modelType()
        << ": traced " << cnt << " particles" << endl;
}

template<class CloudType>
void Foam::TraceParticles<CloudType>::postMove
(
    typename CloudType::parcelType& p,
    const label cellI,
    const scalar dt,
    const point& position0,
    bool& keepParticle
)
{
    if(ids_.found(labelPair(p.origProc(), p.origId()))) {
        writeParticle(
            p,
            "postMove_cell"+name(cellI)
        );
    }
}

template<class CloudType>
void Foam::TraceParticles<CloudType>::postPatch
(
    const typename CloudType::parcelType& p,
    const polyPatch& pp,
    const scalar trackFraction,
    const tetIndices& testIs,
    bool& keepParticle
)
{
    if(ids_.found(labelPair(p.origProc(), p.origId()))) {
        writeParticle(
            p,
            "postPatch_"+pp.name()
        );
    }
}

template<class CloudType>
void Foam::TraceParticles<CloudType>::postFace
(
    const parcelType& p,
    const label faceI,
    bool&
)
{
    if(ids_.found(labelPair(p.origProc(), p.origId()))) {
        writeParticle(
            p,
            "postFace_face"+name(faceI)
        );
    }
}


// ************************************************************************* //
