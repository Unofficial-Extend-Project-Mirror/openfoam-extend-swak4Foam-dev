/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2011, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "ExpressionDriverWriter.H"

#include "CommonValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

defineTypeNameAndDebug(ExpressionDriverWriter, 0);

ExpressionDriverWriter::ExpressionDriverWriter(
        const word &name,
        CommonValueExpressionDriver &driver
):
    regIOobject(
        IOobject(
            name,
            driver.mesh().time().timeName(),
            "swak4Foam",
            driver.mesh().time(),
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        )
    ),
    driver_(driver)
{
    if(debug) {
        Pout << "ExpressionDriverWriter at " << objectPath() << " created" << endl;
    }

    if(headerOk()) {
        if(debug) {
            Pout << "Found a file " <<  objectPath() << endl;
        }

        readData(
            readStream(
                "ExpressionDriverWriter"
#ifdef FOAM_READSTREAM_METHOD_NEEDS_BOOL_PARAMETER
                ,true
#endif
            )
        );
    }
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

ExpressionDriverWriter::~ExpressionDriverWriter()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool ExpressionDriverWriter::writeData(Ostream &os) const
{
    if(debug) {
        Pout << "ExpressionDriverWriter at " << objectPath()
            << " writing" << endl;
    }

    dictionary dict;

    driver_.prepareData(dict);

    dict.write(os,false);

    if(debug) {
        Pout << "written " << dict << endl;
    }

    return os.good();
}

bool ExpressionDriverWriter::readData(Istream &is)
{
    if(debug) {
        Pout << "ExpressionDriverWriter at " << objectPath()
            << " reading" << endl;
    }

    const dictionary dict(is);

    if(debug) {
        Pout << "reading " << dict << endl;
    }

    driver_.getData(dict);

    return !is.bad();
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
