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

#include "patchFieldFlowFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "surfaceFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <class T>
Field<T> patchFieldFlowFunctionObject::fieldFlow(const word& fieldName,T unsetVal) const
{
    const GeometricField<T, fvPatchField, volMesh>& fld =
        obr_.lookupObject<GeometricField<T, fvPatchField, volMesh> >
        (
            fieldName
        );

    const surfaceScalarField &phi=obr_.lookupObject<surfaceScalarField>(solver_.phi());

    Field<T> vals(patchNames_.size(), unsetVal);

    forAll(patchNames_, patchI)
    {
        if (patchIndizes_[patchI] >= 0)
        {
            label index=patchIndizes_[patchI];
            vals[patchI] = sum
                        (
                            phi.boundaryField()[index]
                            *fld.boundaryField()[index]
                        );
        }
    }

    forAll(vals,i) {
        reduce(vals[i],sumOp<T>());
        vals[i]*=factor();
    }

    if(verbose()) {
        Info<< regionString()
            << " Flows of " << fieldName << " :";

        forAll(patchNames_, patchI)
        {
            Info << "  " << patchNames_[patchI] << " = "
                << vals[patchI];
        }

        Info << endl;
    }

    return vals;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
