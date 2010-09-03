//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
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

\*---------------------------------------------------------------------------*/

#include "swakExpressionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(swakExpressionFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        swakExpressionFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakExpressionFunctionObject::swakExpressionFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    timelineFunctionObject(name,t,dict),
    expression_(dict.lookup("expression")),
    accumulations_(dict.lookup("accumulations")),
    driver_(
        CommonValueExpressionDriver::New(
            dict,
            refCast<const fvMesh>(obr_)
        )
    ) 
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word swakExpressionFunctionObject::dirName()
{
    return typeName;
}

wordList swakExpressionFunctionObject::fileNames()
{
    return wordList(1,name());
}

string swakExpressionFunctionObject::firstLine()
{
    string result="";

    forAll(accumulations_,i) {
        result+=" "+accumulations_[i];
    }

    return result;
}

void swakExpressionFunctionObject::write()
{

    if(verbose()) {
        Info << "Expression " << name() << " : ";
    }
    
    driver_->clearVariables();
    driver_->parse(expression_);
    word rType=driver_->getResultType();

    if(rType==pTraits<scalar>::typeName) {
        writeData<scalar>(driver_());
    } else if(rType==pTraits<vector>::typeName) {
        writeData<vector>(driver_());
    } else if(rType==pTraits<tensor>::typeName) {
        writeData<tensor>(driver_());
    } else if(rType==pTraits<symmTensor>::typeName) {
        writeData<symmTensor>(driver_());
    } else if(rType==pTraits<sphericalTensor>::typeName) {
        writeData<sphericalTensor>(driver_());
    } else {
        WarningIn("swakExpressionFunctionObject::write()")
            << "Don't know how to handle type " << rType 
                << endl;
    }
    
    if(verbose()) {
        Info << endl;
    }
}

} // namespace Foam

// ************************************************************************* //
