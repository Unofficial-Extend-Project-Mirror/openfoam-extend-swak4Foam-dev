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

Contributors/Copyright:
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "CommonValueExpressionDriver.H"

#include "timeSelector.H"

#include "IFstream.H"

#include "printSwakVersion.H"

#include "NumericAccumulationNamedEnum.H"

template <class T>
void writeData(
    CommonValueExpressionDriver &driver,
    const wordList &accumulations
)
{
    bool isPoint=driver.result().isPoint();

    Field<T> result(driver.getResult<T>(isPoint));

    forAll(accumulations,i) {
        const word &aName=accumulations[i];
        const NumericAccumulationNamedEnum::value accu=
            NumericAccumulationNamedEnum::names[aName];

        T val=pTraits<T>::zero;

        switch(accu) {
            case NumericAccumulationNamedEnum::numMin:
                val=gMin(result);
                break;
            case NumericAccumulationNamedEnum::numMax:
                val=gMax(result);
                break;
            case NumericAccumulationNamedEnum::numSum:
                val=gSum(result);
                break;
            case NumericAccumulationNamedEnum::numAverage:
                val=gAverage(result);
                break;
            // case NumericAccumulationNamedEnum::numSumMag:
            //     val=gSumMag(result);
            //     break;
            case NumericAccumulationNamedEnum::numWeightedAverage:
                val=driver.calcWeightedAverage(result);
                break;
            default:
                WarningIn("funkyDoCalc")
                    << "Unimplemented accumultation type "
                        << NumericAccumulationNamedEnum::names[accu]
                        << ". Currently only 'min', 'max', 'sum', 'weightedAverage' and 'average' are supported"
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
    argList::validOptions.insert("noDimensionChecking","");
    argList::validOptions.insert("foreignMeshesThatFollowTime",
                                  "<list of mesh names>");

#   include "setRootCase.H"

    printSwakVersion();

    IFstream theFile(args.args()[1]);
    dictionary theExpressions(theFile);
    wordList foreignMeshesThatFollowTime(0);

    if (!args.options().found("time") && !args.options().found("latestTime")) {
        FatalErrorIn("main()")
            << args.executable()
                << ": time/latestTime option is required" << endl
            << exit(FatalError);
    }

    if(args.options().found("noDimensionChecking")) {
        dimensionSet::debug=0;
    }
    if(args.options().found("foreignMeshesThatFollowTime")) {
        string followMeshes(
            args.options()["foreignMeshesThatFollowTime"]
        );
        IStringStream followStream("("+followMeshes+")");
        foreignMeshesThatFollowTime=wordList(followStream);
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    forAllConstIter(IDLList<entry>, theExpressions, iter)
    {
        const dictionary &dict=iter().dict();

        CommonValueExpressionDriver::readForeignMeshInfo(dict);
    }

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info << "\nTime = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();

        forAll(foreignMeshesThatFollowTime,i) {
            const word &name=foreignMeshesThatFollowTime[i];
            if(MeshesRepository::getRepository().hasMesh(name)) {
                Info << "Setting mesh " << name << " to current Time"
                    << endl;

                MeshesRepository::getRepository().setTime(
                    name,
                    runTime.value()
                );
            } else {
                FatalErrorIn(args.executable())
                    << "No mesh with name " << name << " declared. " << nl
                        << "Can't follow current time"
                        << endl
                        << exit(FatalError);

            }
        }

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
            word rType=driver->CommonValueExpressionDriver::getResultType();

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
