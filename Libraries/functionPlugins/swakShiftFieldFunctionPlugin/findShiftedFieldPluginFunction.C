/*---------------------------------------------------------------------------*\
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
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"
#include "findShiftedFieldPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "meshSearch.H"

namespace Foam {

    typedef findShiftedFieldPluginFunction<scalar> findShiftedFieldScalar;
    defineTemplateTypeNameAndDebug(findShiftedFieldScalar,0);
    addNamedToRunTimeSelectionTable(FieldValuePluginFunction, findShiftedFieldScalar , name, findShiftedScalarField);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
findShiftedFieldPluginFunction<Type>::findShiftedFieldPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        word(ResultType::typeName),
        "field internalField "+ResultType::typeName
        +",shift internalField volVectorField"
        +",default internalField "+ResultType::typeName
    ),
    defaultValue_(
        pTraits<Type>::zero
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
void findShiftedFieldPluginFunction<Type>::doEvaluation()
{
    autoPtr<ResultType> mappedField(
        new ResultType(
            default_()
        )
    );

    meshSearch search(mesh());

    forAll(shift_(),cellI) {
        const vector &shift=shift_()[cellI];
        const vector targetPlace=mesh().C()[cellI]+shift;
        label fromCell=search.findCell(targetPlace);
        if(fromCell>=0) {
            mappedField()[cellI]=field_()[fromCell];
        }
    }

    mappedField->correctBoundaryConditions();

    result().setObjectResult(
        mappedField
    );
}

template<class Type>
void findShiftedFieldPluginFunction<Type>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0 || index==1 || index==2);

    if(index==0) {
        Dbug << "Setting field" << endl;
        field_.set(
            new ResultType(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<ResultType>()
            )
        );
    } else if(index==1) {
        Dbug << "Setting shift" << endl;
        shift_.set(
            new volVectorField(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<volVectorField>()
            )
        );
    } else if(index==2) {
        Dbug << "Setting default" << endl;
        default_.set(
            new ResultType(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<ResultType>()
            )
        );
    }
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //


} // namespace

// ************************************************************************* //
