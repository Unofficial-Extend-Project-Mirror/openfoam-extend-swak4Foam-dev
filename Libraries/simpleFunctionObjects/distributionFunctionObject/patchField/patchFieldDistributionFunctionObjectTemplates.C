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

    OpenFOAM is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2008-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fieldDistributionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <typename T>
void patchFieldDistributionFunctionObject::getDistributionInternal(
    autoPtr<SimpleDistribution<T> > &dist
) {
    const fvMesh &mesh=refCast<const fvMesh>(obr_);

    typedef GeometricField<T,fvPatchField,volMesh> volTField;
    typedef GeometricField<T,fvsPatchField,surfaceMesh> surfaceTField;
    typedef GeometricField<T,pointPatchField,pointMesh> pointTField;

    bool firstTime=true;

    forAll(patchIDs_,i) {
        label patchID=patchIDs_[i];
        autoPtr<SimpleDistribution<T> > partial;

        if(mesh.foundObject<volTField>(fieldName())) {
            partial=setDataScalar(
                mesh.lookupObject<volTField>(
                    fieldName()
                ).boundaryField()[patchID],
                mesh.boundary()[patchID].magSf()
            );
        }
        if(mesh.foundObject<surfaceTField>(fieldName())) {
            partial=setDataScalar(
                mesh.lookupObject<surfaceTField>(
                    fieldName()
                ).boundaryField()[patchID],
                mesh.boundary()[patchID].magSf()
            );
        }
        if(mesh.foundObject<pointTField>(fieldName())) {
            partial=setDataScalar(
                mesh.lookupObject<pointTField>(
                    fieldName()
                ).boundaryField()[patchID].patchInternalField()(),
                scalarField(
                    mesh.lookupObject<pointTField>(
                        fieldName()
                    ).boundaryField()[patchID].size(),
                    1
                )
            );
        }

        if(partial.valid()) {
            if(firstTime) {
                firstTime=false;
                dist=partial;
            } else {
                SimpleDistribution<T> &d=dist();
                d=d+partial();
            }
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
