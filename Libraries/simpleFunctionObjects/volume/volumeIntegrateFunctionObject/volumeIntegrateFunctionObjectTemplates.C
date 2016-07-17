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

#include "volumeIntegrateFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class T>
Field<T> volumeIntegrateFunctionObject::integrate(const word& fieldName,T unsetVal) const
{
    const GeometricField<T, fvPatchField, volMesh>& fld =
        obr_.lookupObject<GeometricField<T, fvPatchField, volMesh> >
        (
            fieldName
        );

    Field<T> vals(1, unsetVal);

    const fvMesh &mesh=refCast<const fvMesh>(obr_);
    
    vals[0] = (
        sum
        (
            mesh.V()*fld
        )
    ).value();

    forAll(vals,i) {
        vals[i]*=factor();
    }

    if(verbose()) {
        Info<< regionString() 
            << " Integral of " << fieldName << " = "
            << vals[0] << "  " 
            << fld.dimensions()*dimensionSet(0,3,0,0,0,0,0)
            << endl;
    }

    //    Pstream::listCombineGather(vals, isNotEqOp<T>());
    //    Pstream::listCombineScatter(vals);

    return vals;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
