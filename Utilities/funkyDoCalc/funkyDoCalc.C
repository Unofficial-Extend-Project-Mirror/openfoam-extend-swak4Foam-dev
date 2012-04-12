/*---------------------------------------------------------------------------*\
 ##   ####  ######     | 
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright (C) 1991-2005 OpenCFD Ltd.
    \\/      M anipulation   |
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

Application
    funkyDoCalc

Description

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "CommonValueExpressionDriver.H"

#include "timeSelector.H"

#include "IFstream.H"

#include "printSwakVersion.H"

template <class T>
void writeData(
    CommonValueExpressionDriver &driver,
    const wordList &accumulations
)
{
    Field<T> result=driver.getResult<T>();

    forAll(accumulations,i) {
        const word &aName=accumulations[i];
        T val=pTraits<T>::zero;

        if(aName=="min") {
            val=gMin(result);
        } else if(aName=="max") {
            val=gMax(result);
        } else if(aName=="sum") {
            val=gSum(result);
        } else if(aName=="average") {
            val=gAverage(result);
        } else {
            WarningIn("swakExpressionFunctionObject::writeData")
                << "Unknown accumultation type " << aName
                    << ". Currently only 'min', 'max', 'sum' and 'average' are supported"
                    << endl;
        }

        Info << " " << aName << "=" << val;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);
    Foam::argList::validArgs.append("expressionDict");
#   include "addRegionOption.H"

#   include "setRootCase.H"

    printSwakVersion();

   IFstream theFile(args.args()[1]);
   dictionary theExpressions(theFile);
   
    if (!args.options().found("time") && !args.options().found("latestTime")) {
        FatalErrorIn("main()")
            << args.executable()
                << ": time/latestTime option is required" << endl
            << exit(FatalError);
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info << "\nTime = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();
       
        forAllConstIter(IDLList<entry>, theExpressions, iter) 
        {
            Info << iter().keyword() << " : " << flush;

            const dictionary &dict=iter().dict();

            autoPtr<CommonValueExpressionDriver> driver=
                CommonValueExpressionDriver::New(dict,mesh); 
            wordList accumulations(dict.lookup("accumulations"));

            driver->setSearchBehaviour(
                true,
                true,
                true         // search on disc
            );

            driver->clearVariables();
            driver->parse(string(dict.lookup("expression")));
            word rType=driver->getResultType();
            
            if(rType==pTraits<scalar>::typeName) {
                writeData<scalar>(driver(),accumulations);
            } else if(rType==pTraits<vector>::typeName) {
                writeData<vector>(driver(),accumulations);
            } else if(rType==pTraits<tensor>::typeName) {
                writeData<tensor>(driver(),accumulations);
            } else if(rType==pTraits<symmTensor>::typeName) {
                writeData<symmTensor>(driver(),accumulations);
            } else if(rType==pTraits<sphericalTensor>::typeName) {
                writeData<sphericalTensor>(driver(),accumulations);
            } else {
                WarningIn(args.executable())
                    << "Don't know how to handle type " << rType 
                        << endl;
            }

            Info << endl;
        }
    }
    
    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
