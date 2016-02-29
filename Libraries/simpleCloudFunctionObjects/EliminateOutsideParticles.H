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
    Foam::EliminateOutsideParticles

Description
    Eliminate parcels that are outside of the known cells

SourceFiles
    EliminateOutsideParticles.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

    Based on the CloudTracks-function object

\*---------------------------------------------------------------------------*/

#ifndef EliminateOutsideParticles_H
#define EliminateOutsideParticles_H

#include "CloudFunctionObject.H"

#include "swakCloudTypes.H"

#include "TimelineCollection.H"

#include "meshSearch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class EliminateOutsideParticles Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class EliminateOutsideParticles
:
    public CloudFunctionObject<CloudType>
{
    // Private data

    // Typedefs

    //- Convenience typedef for parcel type
    typedef typename CloudType::parcelType parcelType;

    //- Pointer to the cloud storage
    autoPtr<Cloud<parcelType> > eliminatedPrePtr_;

    //- Pointer to the cloud storage
    autoPtr<Cloud<parcelType> > eliminatedPostPtr_;

    bool eliminatePre_;

    bool eliminatePost_;

    //- here we get our files from
    TimelineCollection out_;

    //- data structure to search cells
    meshSearch search_;

    //- check whether particles are outside and mark particles to be outside
    label checkInside(Cloud<parcelType> &eliminate);

    label nrPre_;

protected:

    // Protected member functions

    //- Write post-processing info
    void write();


public:

    //- Runtime type information
    TypeName("eliminateOutsideParticles");


    // Constructors

    //- Construct from dictionary
    EliminateOutsideParticles
    (
        const dictionary& dict,
        CloudType& owner,
        const word& modelName
    );

    //- Construct copy
    EliminateOutsideParticles(const EliminateOutsideParticles<CloudType>& ppm);

    //- Construct and return a clone
    virtual autoPtr<CloudFunctionObject<CloudType> > clone() const
        {
            return autoPtr<CloudFunctionObject<CloudType> >
                (
                    new EliminateOutsideParticles<CloudType>(*this)
                );
        }


    //- Destructor
    virtual ~EliminateOutsideParticles();


    // Member Functions

    // Evaluation

    //- Pre-evolve hook
    virtual void preEvolve();

    //- Post-evolve hook
    virtual void postEvolve();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "EliminateOutsideParticles.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
