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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "GeneralSurfaceEvaluationPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "SampledSurfaceValueExpressionDriver.H"

namespace Foam {


defineTemplateTypeNameAndDebug(GeneralSurfaceEvaluationPluginFunction<scalar>,0);
defineTemplateTypeNameAndDebug(GeneralSurfaceEvaluationPluginFunction<vector>,0);
defineTemplateTypeNameAndDebug(GeneralSurfaceEvaluationPluginFunction<tensor>,0);
defineTemplateTypeNameAndDebug(GeneralSurfaceEvaluationPluginFunction<symmTensor>,0);

defineTemplateTypeNameAndDebug(GeneralSurfaceEvaluationPluginFunction<sphericalTensor>,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template< class Type>
GeneralSurfaceEvaluationPluginFunction<Type>::GeneralSurfaceEvaluationPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    GeneralSurfacesPluginFunction(
        parentDriver,
        name,
        pTraits<resultType>::typeName,
        string("surfaceName primitive word,")+
        "surfaceExpression surface "+pTraits<Type>::typeName
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template< class Type>
word GeneralSurfaceEvaluationPluginFunction<Type>::getID(
    label index
) {
    assert(index==1);

    return name();
}

template< class Type>
void GeneralSurfaceEvaluationPluginFunction<Type>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==1);
    this->values_.set(
        new Field<Type>(
            const_cast<SampledSurfaceValueExpressionDriver &>(
                //                dynamicCast<const SampledSurfaceValueExpressionDriver &>(
                dynamic_cast<const SampledSurfaceValueExpressionDriver &>(
                    driver
                )
            ).getResult<Type>()
        )
    );
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

template
class GeneralSurfaceEvaluationPluginFunction<scalar>;
template
class GeneralSurfaceEvaluationPluginFunction<vector>;
template
class GeneralSurfaceEvaluationPluginFunction<tensor>;
template
class GeneralSurfaceEvaluationPluginFunction<symmTensor>;
template
class GeneralSurfaceEvaluationPluginFunction<sphericalTensor>;

} // namespace

// ************************************************************************* //
