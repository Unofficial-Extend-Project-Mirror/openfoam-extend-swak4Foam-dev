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
    2010-2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "expressionSource.H"
#include "polyMesh.H"
#include "cellSet.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
expressionSource<T>::expressionSource
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    FieldValueExpressionDriver(dict,mesh),
    expression_(
        dict.lookup("expression"),
        dict
    )
{
    createWriterAndRead(dict.name().name()+"_"+this->type()+"<"+pTraits<T>::typeName+">");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
expressionSource<T>::~expressionSource()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
tmp<typename expressionSource<T>::resultField> expressionSource<T>::operator()()
{
    clearVariables();
    parse(expression_);
    if(!resultIsTyp<resultField>()) {
        FatalErrorIn("expressionSource<"+word(pTraits<T>::typeName)+">::operator()()")
            << "Result of " << expression_ << " is not a " << pTraits<T>::typeName
                << endl
                << exit(FatalError);
    }

    tmp<resultField> result(new resultField(getResult<resultField>()));

    return result;
}

template
class expressionSource<scalar>;

template
class expressionSource<vector>;

template
class expressionSource<tensor>;

template
class expressionSource<symmTensor>;

template
class expressionSource<sphericalTensor>;

} // end namespace

// ************************************************************************* //
