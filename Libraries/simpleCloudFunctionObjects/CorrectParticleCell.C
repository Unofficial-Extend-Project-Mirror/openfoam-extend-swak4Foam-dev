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

#include "CorrectParticleCell.H"
#include "Pstream.H"
#include "ListListOps.H"
#include "IOPtrList.H"

// * * * * * * * * * * * * * protected Member Functions  * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CorrectParticleCell<CloudType>::CorrectParticleCell
(
    const dictionary& dict,
    CloudType& owner,
    const word& modelName
)
:
    CloudFunctionObject<CloudType>(dict, owner, modelName, typeName),
    out_(
        this->outputDir(),
        this->owner().time()
    ),
    search_(
        this->owner().mesh()
    )
{
    out_.addSpec(
        "correctedCell.*",
        "corrected outside"
    );
}


template<class CloudType>
Foam::CorrectParticleCell<CloudType>::CorrectParticleCell
(
    const CorrectParticleCell<CloudType>& ppm
)
:
    CloudFunctionObject<CloudType>(ppm),
    out_(
        this->outputDir(),
        this->owner().time()
    ),
    search_(
        this->owner().mesh()
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CorrectParticleCell<CloudType>::~CorrectParticleCell()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::CorrectParticleCell<CloudType>::preEvolve()
{
    if(
        this->owner().mesh().changing()
    ) {
        Info << this->modelName() << ":" << this->modelType()
            << ": Mesh moving" << endl;

        search_.correct();
    }

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
        } else if(cellI!=oldCellI) {
            outCnt++;
            p.cell()=cellI;
            //            p.initCellFacePt();
        }
    }
    if(outCnt>0) {
        Pout << outCnt << " particles not in the right cell" << endl;
    }

    if(Pstream::parRun()) {
        out_["correctedCellProc"+name(Pstream::myProcNo())]
            << outCnt << tab << cnt;
    }
    reduce(cnt,plusOp<label>());
    reduce(outCnt,plusOp<label>());
    if(Pstream::master()) {
        out_["correctedCellTotal"]
            << outCnt << tab << cnt;
    }
}


// ************************************************************************* //
