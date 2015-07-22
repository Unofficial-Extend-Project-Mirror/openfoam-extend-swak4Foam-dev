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
    2010-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SwakExplicitSource.H"
#include "polyMesh.H"
#include "cellSet.H"
#include "fvMatrix.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {

#ifdef FOAM_HAS_FVOPTIONS
    namespace fv {
#endif

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
SwakExplicitSource<T>::SwakExplicitSource
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    SwakBasicSourceCommon<T>(name, modelType, dict, mesh)
{
    this->read(dict);

    this->driver().createWriterAndRead(
        dict.name().name()+"_"+this->type()+"<"+
        pTraits<T>::typeName+">"
    );

    if(this->verbose_) {
        WarningIn(
            string("SwakExplicitSource<") + pTraits<T>::typeName +
            ">::SwakExplicitSource"
        )    << "Adding source term to the fields " << this->fieldNames_
            << " to the values " << this->expressions_
            << " will be verbose. To switch this off set the "
            << "parameter 'verbose' to false" << endl;
    }

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
SwakExplicitSource<T>::~SwakExplicitSource()
{}


//- Add explicit contribution to equation
template<class T>
void SwakExplicitSource<T>::addSup(
    fvMatrix<T>& eqn,
    const label fieldI
)
{
    if (debug)
    {
        Info<< "SwakExplicitSource<"<< pTraits<T>::typeName
            << ">::addSup for source " << this->name_ << endl;
    }

    this->driver().clearVariables();
    this->driver().parse(this->expressions_[fieldI]);

    if(
        !this->driver().
        FieldValueExpressionDriver::template resultIsTyp<typename SwakExplicitSource<T>::resultField>()
    ) {
        FatalErrorIn("SwakExplicitSource<"+word(pTraits<T>::typeName)+">::addSup()")
            << "Result of " << this->expressions_[fieldI] << " is not a "
                << pTraits<T>::typeName
                << endl
                << exit(FatalError);
    }

    typename SwakExplicitSource<T>::resultField result(
        this->driver().
        FieldValueExpressionDriver::template getResult<typename SwakExplicitSource<T>::resultField>()
    );
    result.dimensions().reset(this->dimensions_[fieldI]);
    typename SwakExplicitSource<T>::resultField usedResult(result*0);
    forAll(this->cells_,i) {
        label cellI=this->cells_[i];
        usedResult[cellI]=result[cellI];
    }
    eqn+=usedResult;
}

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
template<class T>
void SwakExplicitSource<T>::addSup(
    const volScalarField& rho,
    fvMatrix<T>& eqn,
    const label fieldI
)
{
    this->addSup(eqn,fieldI);
}

template<class T>
void SwakExplicitSource<T>::addSup(
    const volScalarField& alpha,
    const volScalarField& rho,
    fvMatrix<T>& eqn,
    const label fieldI
)
{
    this->addSup(eqn,fieldI);
}
#endif

#ifdef FOAM_HAS_FVOPTIONS
    }
#endif

} // end namespace

// ************************************************************************* //
