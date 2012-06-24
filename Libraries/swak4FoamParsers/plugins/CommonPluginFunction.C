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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "CommonPluginFunction.H"

namespace Foam {

defineTypeNameAndDebug(CommonPluginFunction,1);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CommonPluginFunction::CommonPluginFunction(
    CommonValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnType,
    const stringList &argumentSpecification
):
    parentDriver_(parentDriver),
    name_(name),
    returnType_(returnType),
    argumentNames_(argumentSpecification.size()),
    argumentParsers_(argumentSpecification.size()),
    argumentTypes_(argumentSpecification.size())
{
    forAll(argumentSpecification,i)
    {
	const string &spec=argumentSpecification[i];
        label space1=spec.find(' ');
        label space2=spec.find(' ',space1+1);
        label space3=spec.find(' ',space2+1);

        if(
            space1 < 0 || space2 < 0 || space3 > 0
        ) {
            FatalErrorIn("CommonValueExpressionDriver::CommonPluginFunction::CommonPluginFunction")
                << "The argument specification " << spec
                    << " does not fit template '<name> <parser> <type>'"
                    << endl
                    << exit(FatalError);
        }
        argumentNames_[i]=spec(space1);
        argumentParsers_[i]=spec(space1+1,space2-space1-1);
        argumentTypes_[i]=spec(space2+1,spec.size()-space2-1);
    }

}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

string CommonPluginFunction::helpText() const
{
    string result=returnType_+" "+name_+"(";
    forAll(argumentNames_,i)
    {
        if(i>0) {
            result+=",";
        }
        result+=argumentParsers_[i] + "/" + argumentNames_[i]
            + " " + argumentNames_[i];
    }
    result+=")";

    return result;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
