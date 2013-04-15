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
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "dumpSwakGlobalVariableFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"
#include "GlobalVariablesRepository.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
void dumpSwakGlobalVariableFunctionObject::writeValue(
    Ostream &o,
    const scalar &val,
    unsigned int &w
)
{
    o << setw(w) << val;
}

template<class Type>
void dumpSwakGlobalVariableFunctionObject::writeValue(
    Ostream &o,
    const Type &val,
    unsigned int &w
)
{
    for(label j=0;j<Type::nComponents;j++) {
        o << setw(w) << val[j];
    }
}

template <class T>
void dumpSwakGlobalVariableFunctionObject::writeTheData(
    ExpressionResult &value
)
{
    Field<T> result(value.getResult<T>());

    if (Pstream::master()) {
        writeTime(name(),time().value());
        writeData(name(),result);
        endData(name());
    } else {
        Pout << "My data is lost because for dumpSwakGlobalVariableFunctionObject"
            << " only the masters data gets written" << endl;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
