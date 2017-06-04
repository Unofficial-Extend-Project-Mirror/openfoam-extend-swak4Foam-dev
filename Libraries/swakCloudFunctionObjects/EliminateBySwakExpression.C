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

#include "EliminateBySwakExpression.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateBySwakExpression<CloudType>::write()
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
Foam::EliminateBySwakExpression<CloudType>::EliminateBySwakExpression
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
    driver_(
        this->owner(),
        dict,
        "kinematic"+dynamic_cast<cloud&>(owner).type()
    ),
    expression_(
        dict.lookup("eliminationExpression"),
        dict
    ),
    nrPre_(0)
{
    driver_.readVariablesAndTables(dict);

    out_.addSpec(
        "eliminated.*",
        "pre post"
    );
}


template<class CloudType>
Foam::EliminateBySwakExpression<CloudType>::EliminateBySwakExpression
(
    const EliminateBySwakExpression<CloudType>& ppm
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
    driver_(
        ppm.driver_
    ),
    expression_(
        ppm.expression_
    ),
    nrPre_(ppm.nrPre_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::EliminateBySwakExpression<CloudType>::~EliminateBySwakExpression()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::EliminateBySwakExpression<CloudType>::preEvolve()
{
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

        nrPre_=checkExpression(eliminatedPrePtr_());
    }
}

template<class CloudType>
void Foam::EliminateBySwakExpression<CloudType>::postEvolve()
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

        nrPost=checkExpression(eliminatedPostPtr_());
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
Foam::label Foam::EliminateBySwakExpression<CloudType>::checkExpression(Cloud<parcelType> &eliminate)
{
    driver_.clearVariables();
    driver_.parse(expression_);

    if(driver_.getResultType()!=pTraits<bool>::typeName) {
        FatalErrorIn("Foam::EliminateBySwakExpression<CloudType>::checkExpression")
            << "Expected 'bool' as result of " << expression_ << nl
                << "Got " << driver_.getResultType()
                << endl
                << exit(FatalError);
    }

    Field<bool> conditionField(driver_.getResult<bool>());
    assert(conditionField.size()==this->owner().size());

    label cnt=0;
    label nr=0;

    forAllIter(typename CloudType,this->owner(),iter) {
        parcelType &p=iter();
        if(
            conditionField[nr]
        ) {
            cnt++;
            eliminate.append(
                p.clone(this->owner().mesh()).ptr()
            );
            this->owner().deleteParticle(p);
        }
        nr++;
    }

    return cnt;
}

// ************************************************************************* //
