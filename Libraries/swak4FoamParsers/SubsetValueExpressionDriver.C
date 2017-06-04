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
    2010-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SubsetValueExpressionDriver.H"

#include "Random.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(SubsetValueExpressionDriver, 0);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


SubsetValueExpressionDriver::SubsetValueExpressionDriver(const SubsetValueExpressionDriver& orig)
:
    CommonValueExpressionDriver(orig),
    autoInterpolate_(orig.autoInterpolate_),
    warnAutoInterpolate_(orig.warnAutoInterpolate_)
{}

SubsetValueExpressionDriver::SubsetValueExpressionDriver(const dictionary& dict)
:
    CommonValueExpressionDriver(dict),
    autoInterpolate_(dict.lookupOrDefault("autoInterpolate",false)),
    warnAutoInterpolate_(dict.lookupOrDefault("warnAutoInterpolate",true))
{}

SubsetValueExpressionDriver::SubsetValueExpressionDriver(
        bool autoInterpolate,
        bool warnAutoInterpolate
)
:
    CommonValueExpressionDriver(),
    autoInterpolate_(autoInterpolate),
    warnAutoInterpolate_(warnAutoInterpolate)
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

SubsetValueExpressionDriver::~SubsetValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void SubsetValueExpressionDriver::setAutoInterpolate(
    bool autoInterpolate,
    bool warnAutoInterpolate
)
{
    autoInterpolate_=autoInterpolate;
    warnAutoInterpolate_=warnAutoInterpolate;
}

void SubsetValueExpressionDriver::parseInternal (int startToken)
{
    parserSubset::SubsetValueExpressionParser parser (
        scanner_,
        *this,
        startToken,
        0
    );
    parser.set_debug_level (traceParsing());
    parser.parse ();
}

tmp<scalarField> SubsetValueExpressionDriver::makeIdField()
{
    tmp<scalarField> ids(
        new scalarField(this->size())
    );
    forAll(ids(),i) {
        const_cast<scalar&>(ids()[i])=i;
    }
    return ids;
}

tmp<vectorField> SubsetValueExpressionDriver::makePositionField() const
{
    notImplemented("SubsetValueExpressionDriver::makePositionField");

    return tmp<vectorField>(new vectorField(0));
}

// tmp<vectorField> SubsetValueExpressionDriver::makePointField() const
// {
//     notImplemented("SubsetValueExpressionDriver::makePointField");
// }

tmp<vectorField> SubsetValueExpressionDriver::makeFaceNormalField() const
{
    notImplemented("SubsetValueExpressionDriver::makeFaceNormalField");

    return tmp<vectorField>(new vectorField(0));
}

tmp<vectorField> SubsetValueExpressionDriver::makeFaceAreaField() const
{
    notImplemented("SubsetValueExpressionDriver::makeFaceAreaField");

    return tmp<vectorField>(new vectorField(0));
}

template<>
SubsetValueExpressionDriver::SymbolTable<SubsetValueExpressionDriver>::SymbolTable()
:
StartupSymbols()
{
    // default value
    insert("",parserSubset::SubsetValueExpressionParser::token::START_DEFAULT);

    insert(
        "scalar_SC",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_SCALAR_COMMA
    );
    insert(
        "scalar_CL",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_SCALAR_CLOSE
    );
    insert(
        "point_scalar_SC",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_SCALAR_COMMA
    );
    insert(
        "point_scalar_CL",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_SCALAR_CLOSE
    );
    insert(
        "vector_SC",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_VECTOR_COMMA
    );
    insert(
        "vector_CL",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_VECTOR_CLOSE
    );
    insert(
        "point_vector_SC",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_VECTOR_COMMA
    );
    insert(
        "point_vector_CL",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_VECTOR_CLOSE
    );
    insert(
        "tensor_SC",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_TENSOR_COMMA
    );
    insert(
        "tensor_CL",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_TENSOR_CLOSE
    );
    insert(
        "point_tensor_SC",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_TENSOR_COMMA
    );
    insert(
        "point_tensor_CL",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_TENSOR_CLOSE
    );
    insert(
        "symmTensor_SC",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_YTENSOR_COMMA
    );
    insert(
        "symmTensor_CL",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_YTENSOR_CLOSE
    );
    insert(
        "point_symmTensor_SC",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_YTENSOR_COMMA
    );
    insert(
        "point_symmTensor_CL",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_YTENSOR_CLOSE
    );
    insert(
        "sphericalTensor_SC",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_HTENSOR_COMMA
    );
    insert(
        "sphericalTensor_CL",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_HTENSOR_CLOSE
    );
    insert(
        "point_sphericalTensor_SC",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_HTENSOR_COMMA
    );
    insert(
        "point_sphericalTensor_CL",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_HTENSOR_CLOSE
    );
    insert(
        "logical_SC",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_LOGICAL_COMMA
    );
    insert(
        "logical_CL",
        parserSubset::SubsetValueExpressionParser::token::START_FACE_LOGICAL_CLOSE
    );
    insert(
        "point_logical_SC",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_LOGICAL_COMMA
    );
    insert(
        "point_logical_CL",
        parserSubset::SubsetValueExpressionParser::token::START_POINT_LOGICAL_CLOSE
    );

    insert(
        "CL",
        parserSubset::SubsetValueExpressionParser::token::START_CLOSE_ONLY
    );
    insert(
        "SC",
        parserSubset::SubsetValueExpressionParser::token::START_COMMA_ONLY
    );
}

const SubsetValueExpressionDriver::SymbolTable<SubsetValueExpressionDriver> &SubsetValueExpressionDriver::symbolTable()
{
    static SymbolTable<SubsetValueExpressionDriver> actualTable;

    return actualTable;
}

int SubsetValueExpressionDriver::startupSymbol(const word &name) {
    return symbolTable()[name];
}

// ************************************************************************* //

} // namespace
