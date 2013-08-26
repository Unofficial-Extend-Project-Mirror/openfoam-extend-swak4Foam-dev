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
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "Random.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CloudValueExpressionDriver, 0);

addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CloudValueExpressionDriver, dictionary, cloud);
addNamedToRunTimeSelectionTable(CommonValueExpressionDriver, CloudValueExpressionDriver, idName, cloud);


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


CloudValueExpressionDriver::CloudValueExpressionDriver(const CloudValueExpressionDriver& orig)
:
    CommonValueExpressionDriver(orig),
    cloud_(orig.cloud_)

{}

CloudValueExpressionDriver::CloudValueExpressionDriver(
    const dictionary& dict,
    const fvMesh& mesh
)
:
    CommonValueExpressionDriver(dict),
    cloud_(
        mesh.lookupObject<cloud>(
            word(dict.lookup("cloudName"))
        )
    )
{}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CloudValueExpressionDriver::~CloudValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void CloudValueExpressionDriver::parseInternal (int startToken)
{
    parserCloud::CloudValueExpressionParser parser (
        scanner_,
        *this,
        startToken,
        0
    );
    parser.set_debug_level (traceParsing());
    parser.parse ();
}

tmp<scalarField> CloudValueExpressionDriver::makeIdField()
{
    tmp<scalarField> ids(
        new scalarField(this->size())
    );
    forAll(ids(),i) {
        ids()[i]=i;
    }
    return ids;
}

tmp<vectorField> CloudValueExpressionDriver::makePositionField() const
{
    notImplemented("CloudValueExpressionDriver::makePositionField");

    return tmp<vectorField>(new vectorField(0));
}

template<>
CloudValueExpressionDriver::SymbolTable<CloudValueExpressionDriver>::SymbolTable()
:
StartupSymbols()
{
    // default value
    insert("",parserCloud::CloudValueExpressionParser::token::START_DEFAULT);

    insert(
        "scalar_SC",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_SCALAR_COMMA
    );
    insert(
        "scalar_CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_SCALAR_CLOSE
    );
    insert(
        "vector_SC",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_VECTOR_COMMA
    );
    insert(
        "vector_CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_VECTOR_CLOSE
    );
    insert(
        "tensor_SC",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_TENSOR_COMMA
    );
    insert(
        "tensor_CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_TENSOR_CLOSE
    );
    insert(
        "symmTensor_SC",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_YTENSOR_COMMA
    );
    insert(
        "symmTensor_CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_YTENSOR_CLOSE
    );
    insert(
        "sphericalTensor_SC",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_HTENSOR_COMMA
    );
    insert(
        "sphericalTensor_CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_HTENSOR_CLOSE
    );
    insert(
        "logical_SC",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_LOGICAL_COMMA
    );
    insert(
        "logical_CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOUD_LOGICAL_CLOSE
    );

    insert(
        "CL",
        parserCloud::CloudValueExpressionParser::token::START_CLOSE_ONLY
    );
    insert(
        "SC",
        parserCloud::CloudValueExpressionParser::token::START_COMMA_ONLY
    );
}

const CloudValueExpressionDriver::SymbolTable<CloudValueExpressionDriver> &CloudValueExpressionDriver::symbolTable()
{
    static SymbolTable<CloudValueExpressionDriver> actualTable;

    return actualTable;
}

int CloudValueExpressionDriver::startupSymbol(const word &name) {
    return symbolTable()[name];
}

tmp<scalarField> CloudValueExpressionDriver::weightsNonPoint(
    label size
) const
{
    notImplemented("CloudValueExpressionDriver::weightsNonPoint");

    return tmp<scalarField>(new scalarField(0));
}

label CloudValueExpressionDriver::size() const
{
    return cloud_.size();
}

label CloudValueExpressionDriver::pointSize() const
{
    notImplemented("CloudValueExpressionDriver::pointSize()");
    return 0;
}

const fvMesh &CloudValueExpressionDriver::mesh() const
{
    return dynamic_cast<const fvMesh&>(cloud_.db());
}

// ************************************************************************* //

} // namespace
