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

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2008-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fieldDistributionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <typename T>
void fieldDistributionFunctionObject::getDistributionInternal(
    autoPtr<SimpleDistribution<T> > &dist
) {
    const fvMesh &mesh=refCast<const fvMesh>(obr_);

    typedef GeometricField<T,fvPatchField,volMesh> volTField;
    typedef GeometricField<T,fvsPatchField,surfaceMesh> surfaceTField;
    typedef GeometricField<T,pointPatchField,pointMesh> pointTField;

    if(mesh.foundObject<volTField>(fieldName_)) {
        dist=setDataScalar(
            mesh.lookupObject<volTField>(
                fieldName_
            ).internalField(),
            mesh.V()
        );
        return;
    }
    if(mesh.foundObject<surfaceTField>(fieldName_)) {
        dist=setDataScalar(
            mesh.lookupObject<surfaceTField>(
                fieldName_
            ).internalField(),
            mesh.magSf()
        );
        return;
    }
    if(mesh.foundObject<pointTField>(fieldName_)) {
        dist=setDataScalar(
            mesh.lookupObject<pointTField>(
                fieldName_
            ).internalField(),
            scalarField(mesh.nPoints(),1)
        );
        return;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
