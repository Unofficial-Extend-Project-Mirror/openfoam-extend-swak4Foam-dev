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
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

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
    Foam::ReaderParticle

Description

SourceFiles

Contributors/Copyright:
    2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef ReaderParticle_H
#define ReaderParticle_H

#include "Particle.H"
#include "IOstream.H"
#include "autoPtr.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                      Class ReaderParticle Declaration
\*---------------------------------------------------------------------------*/

    class ReaderParticleCloud;

class ReaderParticle
:
    public Particle<ReaderParticle>
{

public:

    // Constructors

        //- Construct from components
        ReaderParticle
        (
            const Cloud<ReaderParticle>& c,
            const vector& position,
            const label celli
        )
        :
            Particle<ReaderParticle>(c, position, celli)
        {}

        //- Construct from Istream
        ReaderParticle
        (
            const Cloud<ReaderParticle>& c,
            Istream& is,
            bool readFields = true
        )
        :
            Particle<ReaderParticle>(c, is, readFields)
        {}

        //- Construct as copy
        ReaderParticle(const ReaderParticle& p)
        :
            Particle<ReaderParticle>(p)
        {}

        //- Construct and return a clone
        autoPtr<ReaderParticle> clone() const
        {
            return autoPtr<ReaderParticle>(new ReaderParticle(*this));
        }

        static void readFields (ReaderParticleCloud &c);

        static void writeFields (const Cloud<ReaderParticle> &c);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
