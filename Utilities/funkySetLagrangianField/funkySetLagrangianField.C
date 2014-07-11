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
    funkySetLagrangianField

Description

Contributors/Copyright:
    2006-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "CloudValueExpressionDriver.H"

#include "timeSelector.H"

#include "printSwakVersion.H"

#include "RepositoryBase.H"

#include "ReaderParticleCloud.H"

#include "loadFieldFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

template <typename T>
void writeResult(
    ReaderParticleCloud &theCloud,
    const word &field,
    const Field<T> &value,
    const Field<bool> &condition,
    bool create
) {
    Field<T> setValue(value);

    if(!create) {
        const Field<T> &old=theCloud.getValues<T>(field);
        forAll(condition,i)
        {
            if(!condition[i]) {
                setValue[i]=old[i];
            }
        }
    }

    theCloud.setValues(field,setValue);
}

void doAnExpression(
    CloudValueExpressionDriver &driver,
    ReaderParticleCloud &theCloud,
    const word &field,
    const exprString &expression,
    const exprString &condition,
    const dictionary &dict,
    bool create,
    bool writeValueAsLabel,
    bool doDebug
) {
    double time=driver.mesh().time().value();

    if(create) {
        if(driver.proxy().hasField(field)) {
            FatalErrorIn("doAnExpression")
                << "Cloud " << theCloud.name()
                    << " already has a field " << field
                    << " of type " << driver.proxy().fieldType(field)
                    << endl
                    << exit(FatalError);
        } else {
            Info << "  Creating field " << field << " in cloud "
                << theCloud.name() << endl;
        }
    } else {
        if(driver.proxy().hasField(field)) {
            Info << "  Modifying field " << field << " of type "
                << driver.proxy().fieldType(field) << " in cloud "
                << theCloud.name() << endl;
        } else {
            FatalErrorIn("doAnExpression")
                << "No field " << field << " in cloud " << theCloud.name()
                    << endl
                    << exit(FatalError);
        }
    }
    Info << " Putting " << expression << " into field " << field
        << " at t = " << time << " if condition " << condition
        << " is true" << endl;

    driver.readVariablesAndTables(dict);

    if (doDebug) {
        Info << "Parsing expression: " << expression << "\nand condition "
            << condition << "\n" << endl;
        driver.setTrace(true,true);
    }

    driver.clearVariables();

    Field<bool> conditionField;
    bool evaluatedCondition=false;

    if(condition!="true") {
        if(create) {
            FatalErrorIn("doAnExpression")
                << "If a field is created no condition can be applied"
                    << endl
                    << exit(FatalError);
        }

        evaluatedCondition=true;
        if (doDebug) {
            Info << "funkySetLagrangianField : Parsing condition:"
                << condition << endl;
        }
        driver.parse(condition);
        if(driver.getResultType()!=pTraits<bool>::typeName) {
            FatalErrorIn("doAnExpression")
                << "Expected 'bool' as result of " << condition << nl
                    << "Got " << driver.getResultType()
                    << endl
                    << exit(FatalError);

        }
        conditionField=driver.getResult<bool>();
    }

    if (doDebug) {
        Info << "funkySetLagrangianField : Parsing expression:"
            << expression << endl;
    }
    driver.parse(expression);
    if(!evaluatedCondition) {
        conditionField=Field<bool>(driver.result().size(),true);
    }
    if(driver.result().size()!=conditionField.size()) {
        FatalErrorIn("doAnExpression")
            << "Size " << driver.result().size() << " of expression "
                << expression << " and condition " << condition << "("
                << conditionField.size() << ") differ!"
                << endl
                << exit(FatalError);
    }

    if(
        writeValueAsLabel
        &&
        driver.getResultType()!="scalar"
    ) {
        FatalErrorIn("doAnExpression")
            << "Only results of type 'scalar' and 'bool' can be written as 'label'"
                << endl
                << exit(FatalError);
    }

    if(driver.getResultType()=="scalar") {
        if(writeValueAsLabel) {
            scalarField orig(driver.getResult<scalar>());
            labelField data(orig.size());
            forAll(orig,i)
            {
                data[i]=label(round(orig[i]));
            }
            writeResult(
                theCloud,
                field,
                data,
                conditionField,
                create
            );
        } else {
            writeResult(
                theCloud,
                field,
                driver.getResult<scalar>()(),
                conditionField,
                create
            );
        }
    } else if(driver.getResultType()=="vector") {
        writeResult(
            theCloud,
            field,
            driver.getResult<vector>()(),
            conditionField,
            create
        );
    } else if(driver.getResultType()=="tensor") {
        writeResult(
            theCloud,
            field,
            driver.getResult<tensor>()(),
            conditionField,
            create
        );
    } else if(driver.getResultType()=="symmTensor") {
        writeResult(
            theCloud,
            field,
            driver.getResult<symmTensor>()(),
            conditionField,
            create
        );
    } else if(driver.getResultType()=="sphericalTensor") {
        writeResult(
            theCloud,
            field,
            driver.getResult<sphericalTensor>()(),
            conditionField,
            create
        );
    } else if(driver.getResultType()=="bool") {
        Field<bool> orig(driver.getResult<bool>());
        labelField data(orig.size());
        forAll(orig,i)
        {
            data[i]=orig[i];
        }
        writeResult(
            theCloud,
            field,
            data,
            conditionField,
            create
        );
    } else {
        FatalErrorIn("doAnExpression")
            << "Results of type " << driver.getResultType()
                << " not supported"
                << endl
                << exit(FatalError);
    }
}

int main(int argc, char *argv[])
{
    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"

    argList::validOptions.insert("field","field to overwrite");
    argList::validOptions.insert("cloud","name of the cloud to work on");
    argList::validOptions.insert("expression","expression to write");
    argList::validOptions.insert("condition","logical condition");
    argList::validOptions.insert("debugParser","");
    argList::validOptions.insert("noCacheVariables","");
    argList::validOptions.insert("create","");
    argList::validOptions.insert("writeValueAsLabel","");
    argList::validOptions.insert("dictExt","extension to the default funkySetLagrangianFieldDict-dictionary");
    argList::validOptions.insert("allowFunctionObjects","");
    argList::validOptions.insert("defaultInterpolation","interpolation scheme used for continous phase");

#   include "setRootCase.H"

    printSwakVersion();

    // make sure the program never fails due to dimension "problems"
    dimensionSet::debug=false;

    if (!args.options().found("time") && !args.options().found("latestTime")) {
        FatalErrorIn("main()")
            << args.executable()
                << ": time/latestTime option is required" << endl
            << exit(FatalError);
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    if(!args.options().found("allowFunctionObjects")) {
        runTime.functionObjects().off();
    }

    dlLibraryTable table;

    bool debugParser=args.options().found("debugParser");

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "\nTime = " << runTime.timeName() << Foam::endl;

        RepositoryBase::updateRepos();

        mesh.readUpdate();

        if(args.options().found("allowFunctionObjects")) {
            runTime.functionObjects().start();
        }

        if(args.options().found("field")) {
            Info << " Using command-line options\n" << endl;

            word field=args.options()["field"];

            word cloudName=args.options()["cloud"];

            exprString expression(
                args.options()["expression"],
                dictionary::null
            );

            exprString condition("true");
            if (args.options().found("condition")) {
                condition=args.options()["condition"];
            }

            bool create=false;
            if (args.options().found("create")) {
                create=true;
            }

            bool writeValueAsLabel=false;
            if (args.options().found("writeValueAsLabel")) {
                writeValueAsLabel=true;
            }

            word defaultInterpolation("none");
            if (args.options().found("defaultInterpolation")) {
                defaultInterpolation=word(
                    args.options()["defaultInterpolation"]
                );
            }

            dictionary dummyDict;

            ReaderParticleCloud theCloud(
                mesh,
                cloudName
            );

            CloudValueExpressionDriver driver(
                theCloud,
                defaultInterpolation
            );
            driver.setSearchBehaviour(
                true,
                true,
                true         // search on disc
            );

            doAnExpression(
                driver,
                theCloud,
                field,
                expression,
                condition,
                dummyDict,
                create,
                writeValueAsLabel,
                debugParser
            );

            Info << endl << "Writing " << cloudName << nl << endl;
            theCloud.write();
        } else {
            Info << " Using funkySetLagrangianFieldDict \n" << endl;

            if(
                args.options().found("create")
                ||
                args.options().found("cloud")
                ||
                args.options().found("condition")
                ||
                args.options().found("expression")
                ||
                args.options().found("writeValueAsLabel")
            ) {
                FatalErrorIn("main()")
                    << args.executable()
                        << ": No other options than -time valid when using funkySetLagrangianFieldDict"
                        << exit(FatalError);
            }

            word dictName="funkySetLagrangianFieldDict";

            if(args.options().found("region")) {
                dictName+="."+args.options()["region"];
            }

            if(args.options().found("dictExt")) {
                dictName+="."+args.options()["dictExt"];
            }

            IOdictionary funkyDict
                (
                    IOobject
                    (
                        dictName,
                        runTime.system(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::NO_WRITE
                    )
                );

            forAllConstIter(dictionary,funkyDict,cloudIter) {
                const dictionary &cloudDict=(*cloudIter).dict();

                word cloudName((*cloudIter).keyword());
                Info << "\n\nCloud: " << cloudName << endl;

                if(readBool(cloudDict.lookup("createCloud"))) {
                    Info << "Creating from scratch" << endl;
                    const dictionary &spec=cloudDict.subDict("creationSpec");

                    if(!spec.found("libs")) {
                        FatalErrorIn(args.executable())
                            << "No entry 'libs' in " << spec.name()
                                << endl
                                << exit(FatalError);
                    }
                    table.open(spec,"libs");

                    wordList preloadFieldNames(spec.lookup("preloadFields"));

                    if(!spec.found("functions")) {
                        FatalErrorIn(args.executable())
                            << "No entry 'functions' in " << spec.name()
                                << endl
                                << exit(FatalError);
                    }

                    Info << "Preloading fields" << endl;
                    SLPtrList<volScalarField> scalarFieldsPre;
                    SLPtrList<volVectorField> vectorFieldsPre;
                    SLPtrList<volTensorField> tensorFieldsPre;
                    SLPtrList<volSymmTensorField> symmTensorFieldsPre;
                    SLPtrList<volSphericalTensorField> sphericalTensorFieldsPre;

                    forAll(preloadFieldNames,fieldI) {
                        word fName=preloadFieldNames[fieldI];

                        if(
                            !loadFieldFunction(mesh,fName,scalarFieldsPre)
                            &&
                            !loadFieldFunction(mesh,fName,vectorFieldsPre)
                            &&
                            !loadFieldFunction(mesh,fName,tensorFieldsPre)
                            &&
                            !loadFieldFunction(mesh,fName,symmTensorFieldsPre)
                            &&
                            !loadFieldFunction(mesh,fName,sphericalTensorFieldsPre)
                        ) {
                            Info << "Field " << fName << " not found " << endl;
                        }
                    }

                    functionObjectList functions(
                        mesh.time(),
                        spec
                    );

                    wordList globalNameSpacesToUse(
                        spec.lookup("globalNamespacesForData")
                    );
                    word positionVar(
                        spec.lookup("positionVariable")
                    );

                    Info << "Function objects: start" << endl << endl;
                    functions.start();
                    Info << endl;
                    Info << "Function objects: execute" << endl << endl;
                    functions.execute();
                    Info << endl;
                    Info << "Creating Cloud" << endl;
                    autoPtr<ReaderParticleCloud> cloud=
                        ReaderParticleCloud::makeCloudFromVariables(
                            mesh,
                            cloudName,
                            globalNameSpacesToUse,
                            positionVar
                        );
                    Info << "Writing cloud" << endl;
                    cloud->write();
                }

                ReaderParticleCloud theCloud(
                    mesh,
                    cloudName
                );

                const dictionary &parts=cloudDict.subDict("expressions");

                forAllConstIter(dictionary,parts,iter) {
                    const dictionary &part=(*iter).dict();

                    Info << "\n\nPart: " << (*iter).keyword() << endl;

                    word field=part["field"];

                    exprString expression(
                        part["expression"],
                        part
                    );

                    exprString condition("true");

                    if (part.found("condition")) {
                        condition=exprString(
                            part["condition"],
                            part
                        );
                    }

                    bool create=false;
                    if (part.found("create")) {
                        create=readBool(part["create"]);
                    }

                    bool writeValueAsLabel=part.lookupOrDefault<bool>(
                        "writeValueAsLabel",false
                    );

                    CloudValueExpressionDriver driver(
                        part,
                        theCloud
                    );

                    driver.setSearchBehaviour(
                        true,
                        true,
                        true         // search on disc
                    );

                    doAnExpression(
                        driver,
                        theCloud,
                        field,
                        expression,
                        condition,
                        part,
                        create,
                        writeValueAsLabel,
                        debugParser
                    );
                }

                Info << endl << "Writing " << cloudName << nl << endl;
                theCloud.write();
             }
        }
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
