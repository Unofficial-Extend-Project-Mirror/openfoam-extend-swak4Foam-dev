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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "setValueMinimumPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

typedef setValueMinimumPluginFunction<scalar> setMinimumScalar;
defineTemplateTypeNameAndDebug(setMinimumScalar,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, setMinimumScalar , name, setValueMinimumScalar);

typedef setValueMinimumPluginFunction<vector> setMinimumVector;
defineTemplateTypeNameAndDebug(setMinimumVector,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, setMinimumVector , name, setValueMinimumVector);

typedef setValueMinimumPluginFunction<tensor> setMinimumTensor;
defineTemplateTypeNameAndDebug(setMinimumTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, setMinimumTensor , name, setValueMinimumTensor);

typedef setValueMinimumPluginFunction<symmTensor> setMinimumSymmTensor;
defineTemplateTypeNameAndDebug(setMinimumSymmTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, setMinimumSymmTensor , name, setValueMinimumSymmTensor);

typedef setValueMinimumPluginFunction<sphericalTensor> setMinimumSphericalTensor;
defineTemplateTypeNameAndDebug(setMinimumSphericalTensor,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction, setMinimumSphericalTensor , name, setValueMinimumSphericalTensor);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
setValueMinimumPluginFunction<Type>::setValueMinimumPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    GeneralSetEvaluationPluginFunction<Type>(
        parentDriver,
        name
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void setValueMinimumPluginFunction<Type>::doEvaluation()
{
    typedef typename GeneralSetEvaluationPluginFunction<Type>::resultType rType;
    autoPtr<rType> pValueMinimum(
        new rType(
            IOobject(
                "setValueMinimumInCell",
                this->mesh().time().timeName(),
                this->mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            this->mesh(),
            dimensioned<Type>("no",dimless,pTraits<Type>::zero)
        )
    );

    const labelList &cells=this->theSet().cells();
    List<bool> here(pValueMinimum->size(),false);
    const Field<Type> vals=this->values();

    forAll(cells,i) {
        const label cellI=cells[i];

        if(here[cellI]) {
            pValueMinimum()[cellI]=min(
                vals[i],
                pValueMinimum()[cellI]
            );
        } else {
            here[cellI]=true;
            pValueMinimum()[cellI]=vals[i];
        }
    }

    pValueMinimum->correctBoundaryConditions();

    this->result().setObjectResult(pValueMinimum);
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
