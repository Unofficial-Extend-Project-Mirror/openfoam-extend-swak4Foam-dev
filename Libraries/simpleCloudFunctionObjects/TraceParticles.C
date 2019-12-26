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

 SWAK Revision: $Id$
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
    forAllConstIter(typename IDLList<typename CloudType::particleType>, this->owner(), iter) {
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
    forAllConstIter(typename IDLList<typename CloudType::particleType>, this->owner(), iter) {
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
