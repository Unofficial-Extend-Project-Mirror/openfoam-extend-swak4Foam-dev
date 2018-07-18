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
    Foam::fv::matrixChangeBaseFvOption

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef matrixChangeBaseFvOptionI_H
#define matrixChangeBaseFvOptionI_H

#include "swak.H"

namespace Foam
{
namespace fv
{

template<class T>
tmp<GeometricField<T,fvPatchField,volMesh> >
matrixChangeBaseFvOption::calcResiduum(const fvMatrix<T> &matrix) const
{
    tmp<GeometricField<T,fvPatchField,volMesh> > tres(
        new GeometricField<T,fvPatchField,volMesh>(
            IOobject(
                "tmp"+this->fieldName(),
                matrix.psi().mesh().time().timeName(),
                matrix.psi().mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            matrix.psi().mesh(),
            dimensioned<T>(
                "no",
                matrix.dimensions()/dimVolume,
                pTraits<T>::zero
            )
        )
    );
    GeometricField<T,fvPatchField,volMesh> &res=
        const_cast<GeometricField<T,fvPatchField,volMesh>&>(tres());

#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
    const_cast<DimensionedField<T,volMesh>&>(res.internalField())=
#else
    res.internalField() =
#endif
        (matrix & matrix.psi());
#ifdef FOAM_NO_DIMENSIONEDINTERNAL_IN_GEOMETRIC
    const_cast<Field<T>&>(res.internalField().field())-=
#else
    res.internalField() -=
#endif
        matrix.source();

    return tres;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
