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

Contributors/Copyright:
    2010-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SwakSetValue.H"
#include "polyMesh.H"
#include "cellSet.H"
#include "fvMatrix.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
SwakSetValue<T>::SwakSetValue
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    SwakBasicSourceCommon<T>(name, modelType, dict, mesh),
    useMaskExpression_(readBool(this->coeffs().lookup("useMaskExpression"))),
    maskExpression_(
        useMaskExpression_
        ?
        exprString(
	    this->coeffs().lookup("maskExpression"),
	    this->coeffs()
	)
        :
        exprString("")
    )
{
    this->read(dict);

    this->driver().createWriterAndRead(
        dict.name().name()+"_"+this->type()+"<"+
        pTraits<T>::typeName+">"
    );

    if(this->verbose_) {
        WarningIn(
            string("SwakSetValue<") + pTraits<T>::typeName +
            ">::SwakSetValue"
        )    << "Fixing to the fields " << this->fieldNames_
            << " to the values " << this->expressions_
            << " with the mask " << maskExpression_
            << " will be verbose. To switch this off set the "
            << "parameter 'verbose' to false" << endl;
    }

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
SwakSetValue<T>::~SwakSetValue()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //
template<class T>
bool SwakSetValue<T>::getMask(DynamicList<label> &cellIDs,const word &psi)
{
    this->driver().parse(maskExpression_);
    if(
        !this->driver().
        FieldValueExpressionDriver::resultIsTyp<volScalarField>(true)
    ) {
        FatalErrorIn("SwakSetValue<scalar>::getMask")
            << "Result of " << maskExpression_ << " is not a logical expression"
                << endl
                << exit(FatalError);
    }

    const volScalarField &cond=this->driver().
        FieldValueExpressionDriver::getResult<volScalarField>();
    volScalarField usedCond(0*cond);
    forAll(this->cells_,i) {
        label cellI=this->cells_[i];
        usedCond[cellI]=cond[cellI];
    }

    forAll(usedCond,cellI) {
        if(usedCond[cellI]!=0) {
            cellIDs.append(cellI);
        }
    }

    cellIDs.shrink();
    label size=cellIDs.size();
    reduce(size,plusOp<label>());

    if(size==0) {
        if(this->verbose_) {
            Info << "No cells fixed for field " << psi << endl;
        }
        return false;
    }
    if(this->verbose_) {
        Info << size << " cells fixed for field " << psi << endl;
    }

    return true;
}


template<class T>
void SwakSetValue<T>::setValue
(
    fvMatrix<T>& eqn,
    const label fieldI
)
{
    this->driver().clearVariables();
    this->driver().parse(this->expressions_[fieldI]);
    if(
        !this->driver().
        FieldValueExpressionDriver::resultIsTyp<typename SwakSetValue<T>::resultField>()
    ) {
        FatalErrorIn("SwakSetValue<"+word(pTraits<T>::typeName)+">::setValue()")
            << "Result of " << this->expressions_[fieldI] << " is not a "
                << pTraits<T>::typeName
                << endl
                << exit(FatalError);
    }

    typename SwakSetValue<T>::resultField result(
        this->driver().
        FieldValueExpressionDriver::getResult<typename SwakSetValue<T>::resultField>()
    );

    if(this->dimensions_[fieldI]!=eqn.psi().dimensions()) {
        FatalErrorIn("SwakSetValue<T>::setValue")
            << "Dimension " << this->dimensions_[fieldI] << " for field "
                << eqn.psi().name() << " in " << this->name()
                << " is not the required " << eqn.psi().dimensions()
                << endl
                << exit(FatalError);
    };

    DynamicList<label> cellIDs;

    if(useMaskExpression_) {
        if(!getMask(cellIDs,eqn.psi().name())) {
            return;
        }
    } else {
        cellIDs=this->cells_;
    }

    List<T> values(cellIDs.size());

    //    UIndirectList<Type>(values, cells_) = injectionRate_[fieldI];
    forAll(cellIDs,i)
    {
	label cellI=cellIDs[i];

        values[i]=result[cellI];
    }

    eqn.setValues(cellIDs, values);
}

} // end namespace

// ************************************************************************* //
