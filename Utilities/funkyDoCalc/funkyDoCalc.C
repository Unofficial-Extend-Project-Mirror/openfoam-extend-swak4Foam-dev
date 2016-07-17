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
    2011-2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "CommonValueExpressionDriver.H"

#include "timeSelector.H"

#include "IFstream.H"

#include "printSwakVersion.H"

#include "AccumulationCalculation.H"

#include "RepositoryBase.H"

#include "dlLibraryTable.H"

template <typename Type>
Ostream& writeValue(
    Ostream &o,
    Type value
) {
    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        o << ","
            << component(value,i);
    }
    return o;
}

// Yeah. Global. Bad. But convenient
fileName dataDir="";

typedef HashPtrTable<OFstream> CsvFiles;
CsvFiles csvFiles;

template <class T>
void writeData(
    CommonValueExpressionDriver &driver,
    const dictionary &dict,
    const Time &time,
    const word &name,
    const bool globalWriteCsv,
    const bool globalWriteDistributions
)
{
    bool writeCsv=
        globalWriteCsv
        ||
        dict.lookupOrDefault<bool>("writeCsv",false);
    bool writeDistributions=
        globalWriteDistributions
        ||
        dict.lookupOrDefault<bool>("writeDistributions",false);

    wordList accumulationNames(dict.lookup("accumulations"));
    List<NumericAccumulationNamedEnum::accuSpecification> accumulations(
        NumericAccumulationNamedEnum::readAccumulations(
            accumulationNames,
            dict.name()
        )
    );

    if(
        ( writeCsv || writeDistributions)
        &&
        !exists(dataDir)
    ) {
        //        Info << "Creating data directory " << dataDir << endl;
        mkDir(dataDir);
    }

    bool firstTime=false;
    OStringStream header;
    OStringStream data;

    if(writeCsv) {
        if(!csvFiles.found(name)) {
            firstTime=true;
            csvFiles.insert(
                name,
                new OFstream(dataDir / name+".csv")
            );
        }
    }

    header << "Time";
    data << time.timeName();

    bool isPoint=driver.result().isPoint();

    Field<T> result(driver.getResult<T>(isPoint));

    autoPtr<Field<bool> > mask;
    autoPtr<Field<scalar> > weight;

    if(dict.found("mask")) {
        Info << " with mask " << flush;

        driver.parse(
            exprString(
                dict.lookup("mask"),
                dict
            )
        );
        mask.set(
            new Field<bool>(
                driver.getResult<bool>(isPoint)
            )
        );
    }
    if(dict.found("weight")) {
        Info << " with weight " << flush;

        driver.parse(
            exprString(
                dict.lookup("weight"),
                dict
            )
        );
        weight.set(
            new Field<scalar>(
                driver.getResult<scalar>(isPoint)
            )
        );
    }

    autoPtr<AccumulationCalculation<T> > pCalculator;

    if(mask.valid()) {
        if(weight.valid()) {
            pCalculator.set(
                new AccumulationCalculation<T>(
                    result,
                    isPoint,
                    driver,
                    mask,
                    weight
                )
            );
        } else {
            pCalculator.set(
                new AccumulationCalculation<T>(
                    result,
                    isPoint,
                    driver,
                    mask
                )
            );
        }
    } else {
        if(weight.valid()) {
            pCalculator.set(
                new AccumulationCalculation<T>(
                    result,
                    isPoint,
                    driver,
                    weight
                )
            );
        } else {
            pCalculator.set(
                new AccumulationCalculation<T>(
                    result,
                    isPoint,
                    driver
                )
            );
        }
    }

    AccumulationCalculation<T> calculator=pCalculator();

    if(
        dict.found("distributionMaxBinNr")
        ||
        dict.found("distributionBinWidth")
    ) {
        calculator.resetNumberOfBins(
            dict.lookupOrDefault<label>("distributionMaxBinNr",-1),
            dict.lookupOrDefault<scalar>("distributionBinWidth",-1)
        );
    }

    forAll(accumulations,i) {
        const NumericAccumulationNamedEnum::accuSpecification accu=
            accumulations[i];

        T val=calculator(accu);

        if(pTraits<T>::nComponents==1) {
            header << "," << accumulations[i];
        } else {
            for(label j=0;j<pTraits<T>::nComponents;j++) {
                header << "," << accumulations[i] << "_"
                    << pTraits<T>::componentNames[j];
            }
        }
        writeValue(data,val);

        Info << " " << accu << "=" << val;
    }

    if(writeDistributions) {
        Info << " - write distribution";

        fileName toDir=dataDir
            /
            time.timeName();

        mkDir(toDir);

        calculator.distribution().write(
            toDir
            /
            name
        );
        calculator.weightedDistribution().write(
            toDir
            /
            name+"_weighted"
        );
    }
    if(writeCsv) {
        if(firstTime) {
            *csvFiles[name] << header.str().c_str() << endl;
        }
        *csvFiles[name] << data.str().c_str() << endl;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);
    Foam::argList::validArgs.append("expressionDict");

#   include "addRegionOption.H"

#   include "addLoadFunctionPlugins.H"

    argList::validOptions.insert("noDimensionChecking","");
    argList::validOptions.insert("foreignMeshesThatFollowTime",
                                  "<list of mesh names>");
    argList::validOptions.insert("writeCsv","");
    argList::validOptions.insert("writeDistributions","");
    argList::validOptions.insert("noDimensionChecking","");

#   include "setRootCase.H"

    printSwakVersion();

    IFstream theFile(args.args()[1]);
    dictionary theExpressions(theFile);
    wordList foreignMeshesThatFollowTime(0);

    bool globalWriteCsv=args.options().found("writeCsv");
    bool globalWriteDistributions=args.options().found("writeDistributions");

    dataDir=args.path()/fileName(args.args()[1]).name()+"_data";

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

#   include "loadFunctionPlugins.H"

    forAllConstIter(IDLList<entry>, theExpressions, iter)
    {
        const dictionary &dict=iter().dict();

        CommonValueExpressionDriver::readForeignMeshInfo(dict);
    }

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info << "\nTime = " << runTime.timeName() << Foam::endl;

        RepositoryBase::updateRepos();

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
            const word name=iter().keyword();

            Info << name << " : " << flush;

            const dictionary &dict=iter().dict();

            autoPtr<CommonValueExpressionDriver> driver=
                CommonValueExpressionDriver::New(dict,mesh);

            driver->setSearchBehaviour(
                true,
                true,
                true         // search on disc
            );

            driver->clearVariables();
            driver->parse(
                exprString(
                    dict.lookup("expression"),
                    dict)
            );
            word rType=driver->CommonValueExpressionDriver::getResultType();

            if(rType==pTraits<scalar>::typeName) {
                writeData<scalar>(driver(),dict,runTime,
                                  name,globalWriteCsv,globalWriteDistributions);
            } else if(rType==pTraits<vector>::typeName) {
                writeData<vector>(driver(),dict,runTime,
                                  name,globalWriteCsv,globalWriteDistributions);
            } else if(rType==pTraits<tensor>::typeName) {
                writeData<tensor>(driver(),dict,runTime,
                                  name,globalWriteCsv,globalWriteDistributions);
            } else if(rType==pTraits<symmTensor>::typeName) {
                writeData<symmTensor>(driver(),dict,runTime,
                                  name,globalWriteCsv,globalWriteDistributions);
            } else if(rType==pTraits<sphericalTensor>::typeName) {
                writeData<sphericalTensor>(driver(),dict,runTime,
                                  name,globalWriteCsv,globalWriteDistributions);
            } else {
                WarningIn(args.executable())
                    << "Don't know how to handle type " << rType
                        << endl;
            }

            Info << endl;
        }
        Info << endl;
    }

    if(csvFiles.size()>0) {
        Info << csvFiles.size() << " CSV files written" << endl;
        csvFiles.clear();
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
