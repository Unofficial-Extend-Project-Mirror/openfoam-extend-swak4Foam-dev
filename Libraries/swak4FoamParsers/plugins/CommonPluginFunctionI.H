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

#include "CommonValueExpressionDriver.H"

namespace Foam {
 

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    template <typename T>
tmp<T> CommonPluginFunction::evaluate(
    const string &content,
    label &consumed
)
{
    if(debug) {
        Info << "Evaluating " << helpText() << " with " << content << endl;
    }
    consumed=0;

    forAll(argumentTypes_,i)
    {
        string currentContent=content.substr(
            consumed,
            content.size()-consumed
        );

	if(debug) {
            Info << "Argument" << i << " with " << currentContent << endl;
        }

        label used=0;

        if(argumentParsers_[i]=="primitive") {
            used=readArgument(i,currentContent,argumentTypes_[i]);
        } else {
            autoPtr<CommonValueExpressionDriver> driver=
                CommonValueExpressionDriver::New(
                    argumentParsers_[i],
                    argumentNames_[i],
                    parentDriver_.mesh()
                );
            used=readArgument(i,currentContent,driver);
        }

        if(used<=0) {
            FatalErrorIn("CommonPluginFunction::evaluate")
                << "Evaluation of argument " << i
                    << " consumed no characters"
                    << endl
                    << exit(FatalError);

        }
        consumed+=used;
    }
    
    if(debug) {
        Info << "Starting the actual evaluation" << endl;
    }
    
    doEvaluation();

    if(debug) {
        Info << "Getting result and returning it" << endl;
    }
    
    return tmp<T>(result_.getObjectResult<T>(false));
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}

// ************************************************************************* //
