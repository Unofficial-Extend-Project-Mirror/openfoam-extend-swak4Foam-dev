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
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakRegistryProxySurface.H"
#include "dictionary.H"
#include "volFields.H"
#include "volPointInterpolation.H"
#include "addToRunTimeSelectionTable.H"
#include "fvMesh.H"
//#include "isoSurface.H"
// #include "isoSurfaceCell.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(swakRegistryProxySurface, 0);
    addToRunTimeSelectionTable(sampledSurface, swakRegistryProxySurface, word);
}

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

// void Foam::swakRegistryProxySurface::createGeometry()
// {
//     if (debug)
//     {
//         Pout<< "swakRegistryProxySurface::createGeometry() - doing nothing"
//             << endl;
//     }
// }

Foam::sampledSurface &Foam::swakRegistryProxySurface::realSurface()
{
    return SurfacesRepository::getRepository(
        mesh()
    ).getSurface(
        surfaceName_,
        static_cast<const fvMesh&>(mesh())
    );
}

const Foam::sampledSurface &Foam::swakRegistryProxySurface::realSurface() const
{
    return SurfacesRepository::getRepository(
        mesh()
    ).getSurface(
        surfaceName_,
        static_cast<const fvMesh&>(mesh())
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::swakRegistryProxySurface::swakRegistryProxySurface
(
    const word& name,
    const polyMesh& mesh,
    const dictionary& dict
)
:
    sampledSurface(name, mesh, dict),
    surfaceName_(dict.lookup("surfaceName"))
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::swakRegistryProxySurface::~swakRegistryProxySurface()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::swakRegistryProxySurface::needsUpdate() const
{
    bool originalUpdate=realSurface().needsUpdate();

    if(debug) {
        Pout << "Foam::swakRegistryProxySurface::needsUpdate(): " << originalUpdate << endl;
    }

    return originalUpdate;
}


bool Foam::swakRegistryProxySurface::expire()
{
    bool originalExpire=realSurface().expire();

    if(debug) {
        Pout << "Foam::swakRegistryProxySurface::expire(): " << originalExpire << endl;
    }

    return originalExpire;
}


bool Foam::swakRegistryProxySurface::update()
{
    bool originalUpdate=realSurface().update();

    if(
        this->Sf().size() != realSurface().Sf().size()
        ||
        this->Cf().size() != realSurface().Cf().size()
        ||
        this->magSf().size() != realSurface().magSf().size()
    ) {
        if(debug) {
            Pout << "Foam::swakRegistryProxySurface::update(): Clearin Geometry" << endl;
        }
        clearGeom();
    }

    if(debug) {
        Pout << "Foam::swakRegistryProxySurface::update(): " << originalUpdate << endl;
    }
    return originalUpdate;
}

#ifdef FOAM_SAMPLEDSURFACE_SAMPLE_WANTS_INTERPOLATION

Foam::tmp<Foam::scalarField>
Foam::swakRegistryProxySurface::sample
(
    const interpolation<scalar>& inter
) const
{
    return realSurface().sample(inter);
}

Foam::tmp<Foam::vectorField>
Foam::swakRegistryProxySurface::sample
(
    const interpolation<vector>& inter
) const
{
    return realSurface().sample(inter);
}

Foam::tmp<Foam::tensorField>
Foam::swakRegistryProxySurface::sample
(
    const interpolation<tensor>& inter
) const
{
    return realSurface().sample(inter);
}

Foam::tmp<Foam::symmTensorField>
Foam::swakRegistryProxySurface::sample
(
    const interpolation<symmTensor>& inter
) const
{
    return realSurface().sample(inter);
}

Foam::tmp<Foam::sphericalTensorField>
Foam::swakRegistryProxySurface::sample
(
    const interpolation<sphericalTensor>& inter
) const
{
    return realSurface().sample(inter);
}


Foam::tmp<Foam::scalarField>
Foam::swakRegistryProxySurface::sample
(
    const surfaceScalarField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::vectorField>
Foam::swakRegistryProxySurface::sample
(
    const surfaceVectorField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::sphericalTensorField>
Foam::swakRegistryProxySurface::sample
(
    const surfaceSphericalTensorField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::symmTensorField>
Foam::swakRegistryProxySurface::sample
(
    const surfaceSymmTensorField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::tensorField>
Foam::swakRegistryProxySurface::sample
(
    const surfaceTensorField& vField
) const
{
    return realSurface().sample(vField);
}
#else
Foam::tmp<Foam::scalarField>
Foam::swakRegistryProxySurface::sample
(
    const volScalarField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::vectorField>
Foam::swakRegistryProxySurface::sample
(
    const volVectorField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::sphericalTensorField>
Foam::swakRegistryProxySurface::sample
(
    const volSphericalTensorField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::symmTensorField>
Foam::swakRegistryProxySurface::sample
(
    const volSymmTensorField& vField
) const
{
    return realSurface().sample(vField);
}


Foam::tmp<Foam::tensorField>
Foam::swakRegistryProxySurface::sample
(
    const volTensorField& vField
) const
{
    return realSurface().sample(vField);
}
#endif

Foam::tmp<Foam::scalarField>
Foam::swakRegistryProxySurface::interpolate
(
    const interpolation<scalar>& interpolator
) const
{
    return realSurface().interpolate(interpolator);
}


Foam::tmp<Foam::vectorField>
Foam::swakRegistryProxySurface::interpolate
(
    const interpolation<vector>& interpolator
) const
{
    return realSurface().interpolate(interpolator);
}

Foam::tmp<Foam::sphericalTensorField>
Foam::swakRegistryProxySurface::interpolate
(
    const interpolation<sphericalTensor>& interpolator
) const
{
    return realSurface().interpolate(interpolator);
}


Foam::tmp<Foam::symmTensorField>
Foam::swakRegistryProxySurface::interpolate
(
    const interpolation<symmTensor>& interpolator
) const
{
    return realSurface().interpolate(interpolator);
}


Foam::tmp<Foam::tensorField>
Foam::swakRegistryProxySurface::interpolate
(
    const interpolation<tensor>& interpolator
) const
{
    return realSurface().interpolate(interpolator);
}


void Foam::swakRegistryProxySurface::print(Ostream& os) const
{
    os  << "swakRegistryProxySurface: " << name() << " :"
        << "  realSurface:" << surfaceName_;
}

// ************************************************************************* //
