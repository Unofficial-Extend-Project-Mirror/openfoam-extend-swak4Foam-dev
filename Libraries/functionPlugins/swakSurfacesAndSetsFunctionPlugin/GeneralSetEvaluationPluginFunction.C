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

#include "GeneralSetEvaluationPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "SampledSetValueExpressionDriver.H"

namespace Foam {


defineTemplateTypeNameAndDebug(GeneralSetEvaluationPluginFunction<scalar>,0);
defineTemplateTypeNameAndDebug(GeneralSetEvaluationPluginFunction<vector>,0);
defineTemplateTypeNameAndDebug(GeneralSetEvaluationPluginFunction<tensor>,0);
defineTemplateTypeNameAndDebug(GeneralSetEvaluationPluginFunction<symmTensor>,0);

defineTemplateTypeNameAndDebug(GeneralSetEvaluationPluginFunction<sphericalTensor>,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template< class Type>
GeneralSetEvaluationPluginFunction<Type>::GeneralSetEvaluationPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    GeneralSetsPluginFunction(
        parentDriver,
        name,
        pTraits<resultType>::typeName,
        string("setName primitive word,")+
        "setExpression set "+pTraits<Type>::typeName
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template< class Type>
word GeneralSetEvaluationPluginFunction<Type>::getID(
    label index
) {
    assert(index==1);

    return name();
}

template< class Type>
void GeneralSetEvaluationPluginFunction<Type>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==1);
    this->values_.set(
        new Field<Type>(
            const_cast<SampledSetValueExpressionDriver &>(
                dynamicCast<const SampledSetValueExpressionDriver &>(
                    driver
                )
            ).getResult<Type>()
        )
    );
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template
class GeneralSetEvaluationPluginFunction<scalar>;
template
class GeneralSetEvaluationPluginFunction<vector>;
template
class GeneralSetEvaluationPluginFunction<tensor>;
template
class GeneralSetEvaluationPluginFunction<symmTensor>;
template
class GeneralSetEvaluationPluginFunction<sphericalTensor>;

} // namespace

// ************************************************************************* //
