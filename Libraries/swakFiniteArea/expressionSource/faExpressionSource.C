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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "faExpressionSource.H"
#include "polyMesh.H"

#include "FaFieldValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
faExpressionSource<T>::faExpressionSource
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    FaFieldValueExpressionDriver(dict,mesh),
    expression_(dict.lookup("expression"))
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
faExpressionSource<T>::~faExpressionSource()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<>
tmp<faExpressionSource<scalar>::resultField> faExpressionSource<scalar>::operator()()
{
    clearVariables();
    parse(expression_);
    if(!resultIsScalar()) {
        FatalErrorIn("faExpressionSource<scalar>::operator()()")
            << "Result of " << expression_ << " is not a scalar"
                << endl
                << abort(FatalError);
    }

    tmp<resultField> result(new resultField(getScalar()));
    
    return result;
}

template<>
tmp<faExpressionSource<vector>::resultField> faExpressionSource<vector>::operator()()
{
    clearVariables();
    parse(expression_);
    if(!resultIsVector()) {
        FatalErrorIn("faExpressionSource<vector>::operator()()")
            << "Result of " << expression_ << " is not a vector"
                << endl
                << abort(FatalError);
    }

    tmp<resultField> result(new resultField(getVector()));

    return result;
}

// Catch all for those not implemented
template<class T>
tmp<typename faExpressionSource<T>::resultField> faExpressionSource<T>::operator()()
{
    FatalErrorIn(
        "faExpressionSource<T>operator()()"
    )
        <<  "not implemented for for T="
            << pTraits<T>::typeName
            << endl
            << abort(FatalError);
}

template
class faExpressionSource<scalar>;

template
class faExpressionSource<vector>;

template
class faExpressionSource<tensor>;

template
class faExpressionSource<symmTensor>;

template
class faExpressionSource<sphericalTensor>;

} // end namespace

// ************************************************************************* //
