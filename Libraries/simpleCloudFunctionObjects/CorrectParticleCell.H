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
    Foam::CorrectParticleCell

Description
    Correct the cell of the particle at the begin of the evolution

    This might only be necessary for moving meshes were the particles
    end up in the wrong cell

SourceFiles
    CorrectParticleCell.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

    Based on the CloudTracks-function object

\*---------------------------------------------------------------------------*/

#ifndef CorrectParticleCell_H
#define CorrectParticleCell_H

#include "CloudFunctionObject.H"

#include "swakCloudTypes.H"

#include "TimelineCollection.H"

#include "meshSearch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class CorrectParticleCell Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class CorrectParticleCell
:
    public CloudFunctionObject<CloudType>
{
    // Private data

    // Typedefs

    //- Convenience typedef for parcel type
    typedef typename CloudType::parcelType parcelType;

    //- here we get our files from
    TimelineCollection out_;

    //- data structure to search cells
    meshSearch search_;

    //- should all corrected particles be written
    bool logCorrected_;

protected:

    // Protected member functions

    //- Write post-processing info
    void write();

public:

    //- Runtime type information
    TypeName("correctParticleCell");


    // Constructors

    //- Construct from dictionary
    CorrectParticleCell
    (
        const dictionary& dict,
        CloudType& owner,
        const word& modelName
    );

    //- Construct copy
    CorrectParticleCell(const CorrectParticleCell<CloudType>& ppm);

    //- Construct and return a clone
    virtual autoPtr<CloudFunctionObject<CloudType> > clone() const
        {
            return autoPtr<CloudFunctionObject<CloudType> >
                (
                    new CorrectParticleCell<CloudType>(*this)
                );
        }


    //- Destructor
    virtual ~CorrectParticleCell();


    // Member Functions

    // Evaluation

    //- Pre-evolve hook
    virtual void preEvolve();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "CorrectParticleCell.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
