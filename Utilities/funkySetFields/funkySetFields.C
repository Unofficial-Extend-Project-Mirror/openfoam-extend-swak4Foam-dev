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
    funkySetFields

Description

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "FieldValueExpressionDriver.H"

#include "timeSelector.H"

template<class T>
void setField
(
    const string &name,
    const fvMesh &mesh,
    const string &time,
    const T &result,
    const scalarField &cond,
    bool create,
    const dimensionSet &dim,
    bool keepPatches,
    const wordList &valuePatches
) {
    dimensioned<typename T::value_type> init("nix",dim,typename T::value_type());

    T *tmp;
    if(create) {
      tmp=new T
        (
            IOobject  
            (
                name,
                time,
                mesh,
		IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            init
        );
    } else {
      tmp=new T
        (
            IOobject  
            (
                name,
                time,
                mesh,
		IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            mesh
        );
    }

    FieldValueExpressionDriver::makePatches(*tmp,keepPatches,valuePatches);
    FieldValueExpressionDriver::copyCalculatedPatches(*tmp,result);
    
    label setCells=0;

    forAll(*tmp,cellI) {
        if(cond[cellI]!=0) {
	  (*tmp)[cellI]=result[cellI];
            setCells++;
        }
    }

    label totalCells=tmp->size();
    reduce(totalCells,plusOp<label>());
    reduce(setCells,plusOp<label>());

    FieldValueExpressionDriver::setValuePatches(*tmp,keepPatches,valuePatches);

    Info << " Setting " << setCells << " of " << totalCells << " cells" << endl;

    Info << " Writing to " << name << endl;

    tmp->write();

    delete tmp;
}

void doAnExpression
(
    const fvMesh &mesh,
    const word &field,
    const string &expression,
    const string &condition,
    const Time& runTime,
    bool doDebug,
    bool create,
    bool cacheVariables,
    const dictionary &dict,
    const dimensionSet &dim,
    bool keepPatches,
    const wordList &valuePatches
) {
    const string &time = runTime.timeName();
    word oldFieldType="none";

    if(!create) {
        IOobject f 
            (
                field,
                time,
                mesh,
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            );
        f.headerOk();
        
        oldFieldType=f.headerClassName();

        Info << " Modifying field " << field 
            << " of type " << oldFieldType << "\n" << endl;
    } else {
        Info << " Creating field " << field << "\n" << endl;
    }

    Info << " Putting " << expression << " into field " << field 
        << " at t = " << time << " if condition " << condition 
        << " is true" << endl;
    if(keepPatches) {
        Info << " Keeping patches unaltered" << endl;
    } else {
        if(valuePatches.size()>0) {
            Info << " Setting the patches " << valuePatches 
                << " to fixed value" << endl;
        }
    }

    Info << endl;

    FieldValueExpressionDriver driver(
        time,
        runTime,
        mesh,
        cacheVariables
    );
    driver.readVariablesAndTables(dict);
//     FieldValueExpressionDriver ldriver(
//         time,
//         runTime,
//         mesh,
//         cacheVariables);

    if (doDebug) {
        Info << "Parsing expression: " << expression << "\nand condition " 
            << condition << "\n" << endl; 
        driver.setTrace(true,true);
    }

    driver.clearVariables();

    scalarField conditionField;
    bool evaluatedCondition=false;
    bool conditionIsSurface=false;
    if(condition!="true") {
        evaluatedCondition=true;

        driver.parse(condition);
        if(
            !driver.resultIsTyp<volScalarField>(true)
            &&
            !driver.resultIsTyp<surfaceScalarField>(true)
        ) {
            FatalErrorIn("doAnExpression()")
                << " condition: " << condition 
                    << " does not evaluate to a logical expression" 
                    << exit(FatalError);
        }

        if(driver.resultIsTyp<volScalarField>(true)) {
            conditionField=driver.getResult<volScalarField>().internalField();
            conditionIsSurface=false;
        } else {
            conditionField=driver.getResult<surfaceScalarField>().internalField();
            conditionIsSurface=true;
        }
    }

    driver.parse(expression);

    if(!evaluatedCondition) {
        conditionIsSurface=driver.isSurfaceField();

        if(conditionIsSurface) {
            conditionField=scalarField(mesh.cells().size(),1);
        } else {
            conditionField=scalarField(mesh.nInternalFaces(),1);
        }
    }

    if(create) {
        oldFieldType=driver.typ();
    }

    if(conditionIsSurface!=driver.isSurfaceField()) {
        FatalErrorIn("doAnExpression()")
            << "Inconsistent expression and condition. "
                << "Expression " << expression << " is defined on the "
                << (driver.isSurfaceField() ? "faces" : "cells")
                << " while condition " << condition << " is defined on "
                << (conditionIsSurface ? "faces" : "cells")
                << endl
                << exit(FatalError);
    }

    if(driver.typ()!=oldFieldType) {
        FatalErrorIn("doAnExpression()")
            //            << args.executable()
                << " inconsistent types: " << field << " is  " 
                    << oldFieldType
                    << " while the expression evaluates to a " 
                    << driver.typ()
            << exit(FatalError);
    } else {
        if(driver.typ()==pTraits<volScalarField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volScalarField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volVectorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volVectorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volSymmTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volSymmTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volSphericalTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volSphericalTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceScalarField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceScalarField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceVectorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceVectorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceSymmTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceSymmTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceSphericalTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceSphericalTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches
            );
        } else {
            FatalErrorIn("doAnExpression")
                << "Expression " << expression
                    << " evaluates to an unsupported type "
                    << driver.typ() << endl
                    << exit(FatalError);
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"

    argList::validOptions.insert("field","<field to overwrite>");
    argList::validOptions.insert("expression","<expression to write>");
    argList::validOptions.insert("condition","<logical condition>");
    argList::validOptions.insert("dimension","<dimension of created field>");
    argList::validOptions.insert("debugParser","");
    argList::validOptions.insert("noCacheVariables","");
    argList::validOptions.insert("create","");
    argList::validOptions.insert("keepPatches","");
    argList::validOptions.insert("valuePatches","<list of patches that get a fixed value>");
    argList::validOptions.insert("dictExt","<extension to the default funkySetFieldsDict-dictionary>");
    argList::validOptions.insert("allowFunctionObjects","");
    argList::validOptions.insert("addDummyPhi","");

#   include "setRootCase.H"

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

    autoPtr<surfaceScalarField> dummyPhi;

#   include "createNamedMesh.H"

    if(!args.options().found("allowFunctionObjects")) {
        runTime.functionObjects().off();
    }

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "Time = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();

        if(args.options().found("addDummyPhi")) {
            Info << "Adding a dummy phi to make inletOutlet happy" << endl;
            dummyPhi.set(
                new surfaceScalarField(
                    IOobject
                    (
                        "phi",
                        mesh.time().system(),
                        mesh,
                        IOobject::NO_READ,
                        IOobject::NO_WRITE
                    ),
                    mesh,
                    dimensionedScalar("phi",dimless,0)
                )
            );
        }

        if(args.options().found("allowFunctionObjects")) {
            runTime.functionObjects().start();
        }

        if(args.options().found("field")) {
            Info << " Using command-line options\n" << endl;

            word field=args.options()["field"];
        
            string expression=args.options()["expression"];

            string condition="true";
            if (args.options().found("condition")) {
                condition=args.options()["condition"];
            }

            string dimString="[0 0 0 0 0]";
	
            if (args.options().found("dimension")) {
                dimString=args.options()["dimension"];
            }
            IStringStream dimStream(dimString);
            dimensionSet dim(dimStream);

            bool create=false;
            if (args.options().found("create")) {
                create=true;
            }

            bool keepPatches=false;
            if (args.options().found("keepPatches")) {
                keepPatches=true;
                if(keepPatches && create) {
                    FatalErrorIn("main()")
                        << args.executable()
                            << ": the options <keepPatches> and <create> contradict"
                            << exit(FatalError);
                }
            }

            string valuePatchesString="";
            if (args.options().found("valuePatches")) {
                valuePatchesString=args.options()["valuePatches"];
            }
            IStringStream valuePatchesStream("("+valuePatchesString+")"); 
            wordList valuePatches(valuePatchesStream);

            dictionary dummyDict;

            doAnExpression(
                mesh,
                field,
                expression,
                condition,
                runTime,
                args.options().found("debugParser"),
                create,
                !args.options().found("noCacheVariables"),
                dummyDict,
                dim,
                keepPatches,
                valuePatches
            );
        } else {
            Info << " Using funkySetFieldsDict \n" << endl;
        
            if(
                args.options().found("keepPatches") 
                ||
                args.options().found("valuePatches")
                ||
                args.options().found("create")
                || 
                args.options().found("dimension")
                ||
                args.options().found("condition") 
                ||
                args.options().found("expression")
            ) {
                FatalErrorIn("main()")
                    << args.executable()
                        << ": No other options than -time valid when using funkySetFieldsDict"
                        << exit(FatalError);
            }

            word dictName="funkySetFieldsDict";

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

            PtrList<entry> parts=funkyDict.lookup("expressions");

            forAll(parts,partI) {
                const dictionary &part=parts[partI].dict();

                Info << "\n\nPart: " << parts[partI].keyword() << endl;

                word field=part["field"];

                string expression=part["expression"];

                string condition="true";

                if (part.found("condition")) {
                    condition=part["condition"];
                }

                dimensionSet dim(0,0,0,0,0);

                if (part.found("dimension")) {
                    part.lookup("dimension") >> dim;
                }

                bool create=false;
                if (part.found("create")) {
                    create=readBool(part["create"]);
                }

                bool keepPatches=false;
                if (part.found("keepPatches")) {
                    keepPatches=readBool(part["keepPatches"]);
                    if(keepPatches && create) {
                        FatalErrorIn("main()")
                            << args.executable()
                                << ": the options <keepPatches> and <create> contradict"
                                << exit(FatalError);
                    }
                }

                wordList valuePatches;
                if (part.found("valuePatches")) {
                    valuePatches=wordList(part.lookup("valuePatches"));
                }

                doAnExpression(
                    mesh,
                    field,
                    expression,
                    condition,runTime,
                    args.options().found("debugParser"),
                    create,
                    !args.options().found("noCacheVariables"),
                    part,
                    dim,
                    keepPatches,
                    valuePatches
                );
            }
        }
    }
    
    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
