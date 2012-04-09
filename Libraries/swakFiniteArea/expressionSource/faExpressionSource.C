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
{
    createWriterAndRead(dict.name().name()+"_"+this->type()+"<"+pTraits<T>::typeName+">");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
faExpressionSource<T>::~faExpressionSource()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
tmp<typename faExpressionSource<T>::resultField> faExpressionSource<T>::operator()()
{
    clearVariables();
    parse(expression_);
    if(!resultIsTyp<resultField>()) {
        FatalErrorIn("faExpressionSource<"+word(pTraits<T>::typeName)+">::operator()()")
            << "Result of " << expression_ << " is not a " << pTraits<T>::typeName
                << endl
                << exit(FatalError);
    }

    tmp<resultField> result(new resultField(getResult<resultField>()));
    
    return result;
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
