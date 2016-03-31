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

#include "EliminateOutsideParticles.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateOutsideParticles<CloudType>::write()
{
    if (eliminatedPrePtr_.valid())
    {
        eliminatedPrePtr_->write();

        eliminatedPrePtr_->clear();
    }
    if (eliminatedPostPtr_.valid())
    {
        eliminatedPostPtr_->write();

        eliminatedPostPtr_->clear();
    }
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::EliminateOutsideParticles<CloudType>::EliminateOutsideParticles
(
    const dictionary& dict,
    CloudType& owner,
    const word& modelName
)
:
    CloudFunctionObject<CloudType>(dict, owner, modelName, typeName),
    eliminatedPrePtr_(NULL),
    eliminatedPostPtr_(NULL),
    eliminatePre_(
        readBool(dict.lookup("eliminatePre"))
    ),
    eliminatePost_(
        readBool(dict.lookup("eliminatePost"))
    ),
    out_(
        this->outputDir(),
        this->owner().time()
    ),
    search_(
        this->owner().mesh()
    ),
    nrPre_(0)
{
    out_.addSpec(
        "eliminated.*",
        "pre post"
    );
}


template<class CloudType>
Foam::EliminateOutsideParticles<CloudType>::EliminateOutsideParticles
(
    const EliminateOutsideParticles<CloudType>& ppm
)
:
    CloudFunctionObject<CloudType>(ppm),
    eliminatedPrePtr_(ppm.eliminatedPrePtr_),
    eliminatedPostPtr_(ppm.eliminatedPostPtr_),
    eliminatePre_(ppm.eliminatePre_),
    eliminatePost_(ppm.eliminatePost_),
    out_(
        this->outputDir(),
        this->owner().time()
    ),
    search_(
        this->owner().mesh()
    ),
    nrPre_(ppm.nrPre_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::EliminateOutsideParticles<CloudType>::~EliminateOutsideParticles()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateOutsideParticles<CloudType>::preEvolve()
{
    if(
        this->owner().mesh().changing()
    ) {
        Info << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
            << ": Mesh moving" << endl;

        search_.correct();
    }

    Info << this->modelName() << ":" << this->owner().name()
        << ":" << this->modelType()
        << ": Checking pre" << endl;

    if(eliminatePre_) {
        if (!eliminatedPrePtr_.valid())
        {
            eliminatedPrePtr_.reset
                (
                    this->owner().cloneBare(
                        this->owner().name()
                        +
                        this->modelName()
                        +
                        "EliminatedPre"
                    ).ptr()
                );
        }

        nrPre_=checkInside(eliminatedPrePtr_());
    }
}

template<class CloudType>
void Foam::EliminateOutsideParticles<CloudType>::postEvolve()
{
    Info << this->modelName() << ":" << this->owner().name()
        << ":" << this->modelType()
        << ": Checking post" << endl;

    label nrPost=0;
    if(eliminatePost_) {
        if(!eliminatedPostPtr_.valid())
        {
            eliminatedPostPtr_.reset
                (
                    this->owner().cloneBare(
                        this->owner().name()
                        +
                        this->modelName()
                        +
                        "EliminatedPost"
                    ).ptr()
                );
        }

        nrPost=checkInside(eliminatedPostPtr_());
    }
    label nrEliminatedPre=nrPre_;
    label totalEliminatedPre=
        nrEliminatedPre
        +
        this->template getModelProperty<scalar>("nrEliminatedPre");

    this->template setModelProperty<scalar>(
        "nrEliminatedPre",
        totalEliminatedPre
    );
    label nrEliminatedPost=nrPost;
    label totalEliminatedPost=
        nrEliminatedPost
        +
        this->template getModelProperty<scalar>("nrEliminatedPost");

    this->template setModelProperty<scalar>(
        "nrEliminatedPost",
        totalEliminatedPost
    );

    if(Pstream::parRun()) {
        out_["eliminatedCpu"+name(Pstream::myProcNo())]
            << nrEliminatedPre << tab << nrEliminatedPost << endl;
    }
    reduce(nrEliminatedPre,plusOp<label>());
    reduce(nrEliminatedPost,plusOp<label>());
    if(Pstream::parRun()) {
        totalEliminatedPre=
            nrEliminatedPre
            +
            this->template getModelProperty<scalar>("nrEliminatedAllProcPre");

        this->template setModelProperty<scalar>(
            "nrEliminatedAllProcPre",
            totalEliminatedPre
        );
        totalEliminatedPost=
            nrEliminatedPost
            +
            this->template getModelProperty<scalar>("nrEliminatedAllProcPost");

        this->template setModelProperty<scalar>(
            "nrEliminatedAllProcPost",
            totalEliminatedPost
        );
    }

    if(Pstream::master()) {
        out_["eliminatedTotal"]
            << nrEliminatedPre << tab << nrEliminatedPost << endl;
    }

    if(
        totalEliminatedPre>0
        ||
        totalEliminatedPost>0
    ) {
        Info << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
            << " : " << nrEliminatedPre << " parcels eliminated before timestep. "
            << totalEliminatedPre << " in total" << tab
            << "After: " << nrEliminatedPost << " (" << totalEliminatedPost
            << ")" << endl;
    }

    CloudFunctionObject<CloudType>::postEvolve();
}

template<class CloudType>
Foam::label Foam::EliminateOutsideParticles<CloudType>::checkInside(Cloud<parcelType> &eliminate)
{
    label cnt=0;
    label outCnt=0;

    forAllIter(typename CloudType,this->owner(),iter) {
        parcelType &p=iter();
        label oldCellI=p.cell();
        label cellI=search_.findCell(
            p.position(),
            oldCellI
        );
        if(
            cellI<0
            // ||
            // (cellI % 4)==0
        ) {
            cnt++;
            eliminate.append(
                p.clone(this->owner().mesh()).ptr()
            );
            this->owner().deleteParticle(p);
        } else if(cellI!=oldCellI) {
            // Pout << "Cell at position " << p.position() << " is " << cellI
            //     << ". Not " << oldCellI << endl;
            outCnt++;
        }
    }
    if(outCnt>0) {
        Pout << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
            << " : " << outCnt << " particles not in the right cell" << endl;
    }
    return cnt;
}

// ************************************************************************* //
