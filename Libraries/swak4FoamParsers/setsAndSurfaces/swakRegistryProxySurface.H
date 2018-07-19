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

Class
    Foam::swakRegistryProxySurface

Description
    A sampledSurface that is only a Proxy for one found in the
    Surface-registry of swak4Foam

SourceFiles
    swakRegistryProxySurface.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2016 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakRegistryProxySurface_H
#define swakRegistryProxySurface_H

#include "sampledSurface.H"

#include "SurfacesRepository.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class swakRegistryProxySurface Declaration
\*---------------------------------------------------------------------------*/

class swakRegistryProxySurface
:
    public sampledSurface
{
    // Private data

    word surfaceName_;

    const sampledSurface &realSurface() const;

    sampledSurface &realSurface();

public:

    //- Runtime type information
    TypeName("swakRegistryProxy");


    // Constructors

        //- Construct from dictionary
        swakRegistryProxySurface
        (
            const word& name,
            const polyMesh& mesh,
            const dictionary& dict
        );


    // Destructor

        virtual ~swakRegistryProxySurface();


    // Member Functions

        //- Does the surface need an update?
        virtual bool needsUpdate() const;

        //- Mark the surface as needing an update.
        //  May also free up unneeded data.
        //  Return false if surface was already marked as expired.
        virtual bool expire();

        //- Update the surface as required.
        //  Do nothing (and return false) if no update was needed
        virtual bool update();

        //- Points of surface
        virtual const pointField& points() const
        {
            return realSurface().points();
        }

        //- Faces of surface
        virtual const faceList& faces() const
        {
            return realSurface().faces();
        }

#ifdef FOAM_SAMPLED_SURFACE_ADDITIONAL_METHODS
        // Could also re-direct to realSurface(), but need to verify

        //- Const access to per-face zone/region information
        virtual const labelList& zoneIds() const
        {
            return realSurface().zoneIds();
        }

        //- Face area vectors
        virtual const vectorField& Sf() const
        {
            return realSurface().Sf();
        }

        //- Face area magnitudes
        virtual const scalarField& magSf() const
        {
            return realSurface().magSf();
        }

        //- Face centres
        virtual const vectorField& Cf() const
        {
            return realSurface().Cf();
        }
#endif

        //- interpolate field on surface
        virtual tmp<scalarField> interpolate
        (
            const interpolation<scalar>&
        ) const;

        //- interpolate field on surface
        virtual tmp<vectorField> interpolate
        (
            const interpolation<vector>&
        ) const;

        //- interpolate field on surface
        virtual tmp<sphericalTensorField> interpolate
        (
            const interpolation<sphericalTensor>&
        ) const;

        //- interpolate field on surface
        virtual tmp<symmTensorField> interpolate
        (
            const interpolation<symmTensor>&
        ) const;

        //- interpolate field on surface
        virtual tmp<tensorField> interpolate
        (
            const interpolation<tensor>&
        ) const;


#ifdef FOAM_SAMPLEDSURFACE_SAMPLE_WANTS_INTERPOLATION
        //- sample field on surface
        virtual tmp<scalarField> sample
        (
            const interpolation<scalar> &
        ) const;

        //- sample field on surface
        virtual tmp<vectorField> sample
        (
            const interpolation<vector> &
        ) const;

        //- sample field on surface
        virtual tmp<sphericalTensorField> sample
        (
            const interpolation<sphericalTensor> &
        ) const;

        //- sample field on surface
        virtual tmp<symmTensorField> sample
        (
            const interpolation<symmTensor> &
        ) const;

        //- sample field on surface
        virtual tmp<tensorField> sample
        (
            const interpolation<tensor> &
        ) const;

        //- sample field on surface
        virtual tmp<scalarField> sample
        (
            const surfaceScalarField&
        ) const;

        //- sample field on surface
        virtual tmp<vectorField> sample
        (
            const surfaceVectorField&
        ) const;

        //- sample field on surface
        virtual tmp<sphericalTensorField> sample
        (
            const surfaceSphericalTensorField&
        ) const;

        //- sample field on surface
        virtual tmp<symmTensorField> sample
        (
            const surfaceSymmTensorField&
        ) const;

        //- sample field on surface
        virtual tmp<tensorField> sample
        (
            const surfaceTensorField&
        ) const;
#else
        //- sample field on surface
        virtual tmp<scalarField> sample
        (
            const volScalarField&
        ) const;

        //- sample field on surface
        virtual tmp<vectorField> sample
        (
            const volVectorField&
        ) const;

        //- sample field on surface
        virtual tmp<sphericalTensorField> sample
        (
            const volSphericalTensorField&
        ) const;

        //- sample field on surface
        virtual tmp<symmTensorField> sample
        (
            const volSymmTensorField&
        ) const;

        //- sample field on surface
        virtual tmp<tensorField> sample
        (
            const volTensorField&
        ) const;
#endif

        //- Write
        virtual void print(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
