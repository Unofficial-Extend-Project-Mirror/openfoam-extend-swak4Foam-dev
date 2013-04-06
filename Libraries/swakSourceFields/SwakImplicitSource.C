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
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "SwakImplicitSource.H"
#include "polyMesh.H"
#include "cellSet.H"
#include "fvMatrix.H"
#include "fvm.H"

#include "FieldValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
SwakImplicitSource<T>::SwakImplicitSource
(
    const word& name,
    const word& modelType,
    const dictionary& dict,
    const fvMesh& mesh
)
:
    SwakBasicSourceCommon<T>(name, modelType, dict, mesh),
    switchExplicitImplicit_(
        readBool(
            this->coeffs().lookup("switchExplicitImplicit")
        )
    )
{
    this->read(dict);

    this->driver().createWriterAndRead(
        dict.name().name()+"_"+this->type()+"<"+
        pTraits<T>::typeName+">"
    );

    if(this->verbose_) {
        WarningIn(
            string("SwakImplicitSource<") + pTraits<T>::typeName +
            ">::SwakImplicitSource"
        )    << "Adding source term to the fields " << this->fieldNames_
            << " to the values " << this->expressions_
            << " will be verbose. To switch this off set the "
            << "parameter 'verbose' to false" << endl;
    }

}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
SwakImplicitSource<T>::~SwakImplicitSource()
{}


//- Add implicit contribution to equation
template<class T>
void SwakImplicitSource<T>::addSup(fvMatrix<T>& eqn, const label fieldI)
{
    if (debug)
    {
        Info<< "SwakImplicitSource<"<< pTraits<T>::typeName
            << ">::addSup for source " << this->name_ << endl;
    }

    this->driver().clearVariables();
    this->driver().parse(this->expressions_[fieldI]);

    if(
        !this->driver().
        FieldValueExpressionDriver::resultIsTyp<volScalarField>()
    ) {
        FatalErrorIn("SwakImplicitSource<"+word(pTraits<T>::typeName)+">::addSup()")
            << "Result of " << this->expressions_[fieldI] << " is not a "
                << "volScalarField"
                << endl
                << exit(FatalError);
    }

    volScalarField result(
        this->driver().
        FieldValueExpressionDriver::getResult<volScalarField>()
    );

    if(switchExplicitImplicit_) {
        eqn+=fvm::SuSp(result,eqn.psi());
    } else {
        eqn+=fvm::Sp(result,eqn.psi());
    }
}

} // end namespace

// ************************************************************************* //
