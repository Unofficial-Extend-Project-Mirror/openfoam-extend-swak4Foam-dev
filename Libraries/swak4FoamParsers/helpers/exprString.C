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
    2012-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "exprString.H"

#include "CommonValueExpressionDriver.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

exprString::exprString()
:
    string()
{
    isValid();
}

exprString::exprString(const exprString &o)
:
    string(o)
{
    isValid();
}

// exprString::exprString(const string &o)
// :
//     string(o)
// {
//     isValid();
// }

// exprString::exprString(const std::string &o)
// :
//     string(o)
// {
//     isValid();
// }

exprString::exprString(const char *o)
:
    string(o)
{
    isValid();
}

exprString::exprString(
    Istream & in,
    const dictionary &dict
)
    :
    string(in)
{
    (*this)=CommonValueExpressionDriver::expandDictVariables(
        (*this),
        dict
    );
    isValid();
}

exprString::exprString(
    const string &in,
    const dictionary &dict
)
    :
    string(in)
{
    (*this)=CommonValueExpressionDriver::expandDictVariables(
        (*this),
        dict
    );
    isValid();
}

exprString::~exprString() {
}

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

exprString exprString::toExpr(const string &o)
{
    exprString e;

    e.string::operator=(o);

    return e;
}

exprString &exprString::operator=(const string &o)
{
    string::operator=(o);
    isValid();
    return *this;
}

bool exprString::isValid() {
    if(find('$')!=std::string::npos) {
        FatalErrorIn("exprString::isValid()")
            << "There is a '$' in " << *this
                << endl
                << exit(FatalError);

        return false;
    }
    return true;
}

// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
