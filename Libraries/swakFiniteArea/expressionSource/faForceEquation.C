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

#include "faForceEquation.H"
#include "polyMesh.H"

#include "FaFieldValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
faForceEquation<T>::faForceEquation
(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    FaFieldValueExpressionDriver(dict,mesh),
    valueExpression_(dict.lookup("valueExpression")),
    maskExpression_(dict.lookup("maskExpression")),
    verbose_(dict.lookupOrDefault<bool>("verbose",true))
{
    if(verbose_) {
        WarningIn(string("faForceEquation<") + pTraits<T>::typeName + ">::faForceEquation") 
            << "Fixing to the values " << valueExpression_ 
                << " with the mask " << maskExpression_ 
                << " will be verbose. To switch this off set the parameter 'verbose'"
                << "to false" << endl;
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
faForceEquation<T>::~faForceEquation()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
bool faForceEquation<T>::getMask(DynamicList<label> &cellIDs,const word &psi)
{
    parse(maskExpression_);
    if(!resultIsLogical()) {
        FatalErrorIn("faForceEquation<scalar>::operator()(faMatrix<T> &)")
            << "Result of " << maskExpression_ << " is not a logical expression"
                << endl
                << abort(FatalError);
    }

    const areaScalarField &cond=getScalar();

    forAll(cond,cellI) {
        if(cond[cellI]!=0) {
            cellIDs.append(cellI);
        }
    }

    cellIDs.shrink();
    label size=cellIDs.size();
    reduce(size,plusOp<label>());

    if(size==0) {
        if(verbose_) {
            Info << "No cells fixed for field " << psi << endl;
        }
        return false;
    }
    if(verbose_) {
        Info << size << " cells fixed for field " << psi << endl;
    }

    return true;
}

template<class T>
tmp<areaScalarField> faForceEquation<T>::getMask()
{
    clearVariables();
    parse(maskExpression_);

    return tmp<areaScalarField>
        (
            new areaScalarField
            (
                getScalar()
            )
        );
}

template<>
void faForceEquation<scalar>::operator()(faMatrix<scalar> &eq)
{
    clearVariables();

    DynamicList<label> cellIDs;

    if(!getMask(cellIDs,eq.psi().name())) {
        return;
    }

    Field<scalar> values(cellIDs.size());

    parse(valueExpression_);
    const areaScalarField &calculated=getScalar();

    forAll(cellIDs,i) {
        values[i]=calculated[cellIDs[i]];
    }

    eq.setValues(cellIDs,values);
}

template<>
void faForceEquation<vector>::operator()(faMatrix<vector> &eq)
{
    clearVariables();

    DynamicList<label> cellIDs;

    if(!getMask(cellIDs,eq.psi().name())) {
        return;
    }

    Field<vector> values(cellIDs.size());

    parse(valueExpression_);
    const areaVectorField &calculated=getVector();

    forAll(cellIDs,i) {
        values[i]=calculated[cellIDs[i]];
    }

    eq.setValues(cellIDs,values);
}

// Catch all for those not implemented
template<class T>
void faForceEquation<T>::operator()(faMatrix<T> &)
{
    FatalErrorIn(
        "faForceEquation<T>operator()(faMatrix<T> &)"
    )
        <<  "not implemented for for T="
            << pTraits<T>::typeName
            << endl
            << abort(FatalError);
}

template
class faForceEquation<scalar>;

template
class faForceEquation<vector>;

template
class faForceEquation<tensor>;

template
class faForceEquation<symmTensor>;

template
class faForceEquation<sphericalTensor>;

} // end namespace

// ************************************************************************* //
