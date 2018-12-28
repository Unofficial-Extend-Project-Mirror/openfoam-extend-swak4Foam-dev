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
    2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakCloudTypes.H"

#ifndef FOAM_BARYCENTRIC_PARTICLES

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
    ),
    logCorrected_(
        dict.lookupOrDefault<bool>("logCorrected",false)
    )
{
    out_.addSpec(
        "correctedCell.*",
        "corrected outside"
    );
    out_.addSpec(
        "logCorrectedParticles.*",
        CloudType::parcelType::propertyList()
    );
    out_.addSpec(
        "logOutsideParticles.*",
        CloudType::parcelType::propertyList()
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
    ),
    logCorrected_(
        ppm.logCorrected_
    )
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::CorrectParticleCell<CloudType>::~CorrectParticleCell()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void Foam::CorrectParticleCell<CloudType>::write()
{
}

template<class CloudType>
void Foam::CorrectParticleCell<CloudType>::preEvolve()
{
    if(
        this->owner().mesh().changing()
    ) {
        Info << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
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
            if(logCorrected_) {
                out_("logOutsideParticles")
                    << p << endl;
            }
            //            Info << "Not in Mesh" << endl;
            // label tetC=-1,tetP=-1,newCell=-1;
            // this->owner().mesh().findCellFacePt(
            //     p.position(),
            //     newCell,
            //     tetC,
            //     tetP
            // );
            // Info << p.position() << " " << newCell << " " << tetC << " " << tetP << endl;
            // Info << "Old: " << oldCellI << " " << p.tetFace() << " " << p.tetPt() << endl;
        } else if(cellI!=oldCellI) {
            if(logCorrected_) {
                out_("logCorrectedParticles")
                    << p << endl;
            }
            // Info << "Cell: " << cellI << " old: " << oldCellI << endl;
            label tetC=-1,tetP=-1,newCell=-1;
            this->owner().mesh().findCellFacePt(
                p.position(),
                newCell,
                tetC,
                tetP
            );
            // Info << "Corrected: " << p.position() << " " << newCell << " " << tetC << " " << tetP << endl;
            // Info << "Old: " << p.cell() << " "<< p.tetFace() << " " << p.tetPt() << endl;

            outCnt++;
            p.cell()=newCell;
            p.tetFace()=tetC;
            p.tetPt()=tetP;
            p.cell()=cellI;
            p.initCellFacePt();
        }
    }
    if(outCnt>0) {
        Pout << outCnt << " particles not in the right cell" << endl;
    }

    if(Pstream::parRun()) {
        out_["correctedCellProc"+Foam::name(Pstream::myProcNo())]
            << outCnt << tab << cnt << endl;
    }
    reduce(cnt,plusOp<label>());
    reduce(outCnt,plusOp<label>());
    if(Pstream::master()) {
        out_["correctedCellTotal"]
            << outCnt << tab << cnt << endl;
    }
    if(outCnt>0) {
        Info << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
            << "Corrected " << outCnt << " particles" << endl;
    }
    if(cnt>0) {
        Info << this->modelName() << ":" << this->owner().name()
            << ":" << this->modelType()
            << "Not in mesh " << cnt << " particles" << endl;
    }
}

#endif

// ************************************************************************* //
