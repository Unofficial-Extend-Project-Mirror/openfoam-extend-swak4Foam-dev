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
#include "faCFD.H"

#include "FaFieldValueExpressionDriver.H"

#include "timeSelector.H"

template<class T>
void setField
(
    const string &name,
    const faMesh &mesh,
    const string &time,
    const GeometricField<T,faPatchField,areaMesh> &result,
    const areaScalarField &cond,
    bool create,
    const dimensionSet &dim,
    bool keepPatches,
    const wordList &valuePatches,
    bool createVolumeField,
    bool noWrite
) {
    dimensioned<T> init("nix",dim,pTraits<T>::zero);
    typedef GeometricField<T,faPatchField,areaMesh> aField;
    typedef GeometricField<T,fvPatchField,volMesh> vField;

    aField *tmp;
    if(create) {
      tmp=new aField
        (
            IOobject  
            (
                name,
                time,
                dynamic_cast<const fvMesh&>(mesh.thisDb()),
		IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            mesh,
            init
        );
    } else {
      tmp=new aField
        (
            IOobject  
            (
                name,
                time,
                dynamic_cast<const fvMesh&>(mesh.thisDb()),
		IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            mesh
        );
    }

    FaFieldValueExpressionDriver::makePatches(*tmp,keepPatches,valuePatches);

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

    FaFieldValueExpressionDriver::setValuePatches(*tmp,keepPatches,valuePatches);

    Info << " Setting " << setCells << " of " << totalCells << " cells" << endl;

    if(!noWrite) {
        Info << " Writing to " << name << endl;
        
        tmp->write();
    }

    if(createVolumeField) {
        word vName(name+"Volume");
        Info << " Writing volume field to " << vName << endl;
        
        vField volField(
            IOobject  
            (
                vName,
                time,
                dynamic_cast<const fvMesh&>(mesh.thisDb()),
		IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            dynamic_cast<const fvMesh&>(mesh.thisDb()),
            init,
            "fixedValue"
        );

        volSurfaceMapping mapper(mesh);

        mapper.mapToVolume(*tmp, volField.boundaryField());
        volField.write();       
    }
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
    const wordList &valuePatches,
    bool createVolumeField,
    bool noWrite
) {
    const string &time = runTime.timeName();
    bool isScalar=false;

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
        
        word classN=f.headerClassName();
        if(classN=="areaScalarField") {
            isScalar=true;
        } else if (classN!="areaVectorField") {
            FatalErrorIn("doAnExpression()")
                //            << args.executable()
                << " unsupported type " << classN << " of field " 
                    << field << " or not existing at time " << time
                    << exit(FatalError);
        }
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

    FaFieldValueExpressionDriver driver(
        mesh,
        cacheVariables);
//     FaFieldValueExpressionDriver ldriver(
//         mesh,
//         cacheVariables);

    driver.readVariablesAndTables(dict);

    if (doDebug) {
        Info << "Parsing expression: " << expression << "\nand condition " 
            << condition << "\n" << endl; 
        driver.setTrace(true,true);
    }

    driver.clearVariables();

    driver.parse(condition);
    if(!driver.resultIsLogical()) {
        FatalErrorIn("doAnExpression()")
                << " condition: " << condition 
                    << " does not evaluate to a logical expression" 
                    << exit(FatalError);
    }

    areaScalarField conditionField(driver.getScalar());

    driver.parse(expression);

    if(create) {
        if(driver.resultIsVector()) {
            isScalar=false;
        } else if(driver.resultIsScalar()) {
            isScalar=true;
        } else {
            FatalErrorIn("doAnExpression()")
                << " result is neither scalar nor vector" 
            << exit(FatalError);
        }
    }

    if(driver.resultIsVector()==isScalar) {
        FatalErrorIn("doAnExpression()")
            //            << args.executable()
                << " inconsistent types: " << field << " is  " 
                    << (isScalar ? "scalar" : "vector" ) 
                    << " while the expression evaluates to a " 
                    << (!driver.resultIsVector() ? "scalar" : "vector" )
            << exit(FatalError);
    } else {
        if(isScalar) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getScalar(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else {
	  setField(
              field,
              driver.aMesh(),
              time,
              driver.getVector(),
              conditionField,
              create,
              dim,
              keepPatches,
              valuePatches,
              createVolumeField,
              noWrite
          );
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
    argList::validOptions.insert("createVolumeField","");
    argList::validOptions.insert("onlyVolumeField","");
    argList::validOptions.insert("keepPatches","");
    argList::validOptions.insert("valuePatches","<list of patches that get a fixed value>");
    argList::validOptions.insert("dictExt","<extension to the default funkySetAreaFieldsDict-dictionary>");

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

#   include "createNamedMesh.H"

    faMesh aMesh(mesh);
    aMesh.edgeCentres(); // to force the creation of a field that enables the areaMesh to be found

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "Time = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();

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

            bool createVolumeField=(
                args.options().found("createVolumeField")
                ||
                args.options().found("onlyVolumeField")
            );

            bool noWrite=args.options().found("onlyVolumeField");

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
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else {
            Info << " Using funkySetAreaFieldsDict \n" << endl;
        
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
                        << ": No other options than -time valid when using funkySetAreaFieldsDict"
                        << exit(FatalError);
            }

            word dictName="funkySetAreaFieldsDict";

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

                bool createVolumeField=(
                    args.options().found("createVolumeField")
                    ||
                    args.options().found("onlyVolumeField")
                );

                bool noWrite=args.options().found("onlyVolumeField");

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
                    valuePatches,
                    createVolumeField,
                    noWrite
                );
            }
        }
    }
    
    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
