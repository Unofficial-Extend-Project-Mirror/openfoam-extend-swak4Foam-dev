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

Contributors/Copyright:
    2011 Petr Vita <petr.vita@unileoben.ac.at>
    2011-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "faCFD.H"

#include "FaFieldValueExpressionDriver.H"

#include "timeSelector.H"

#include "areaFields.H"

#include "printSwakVersion.H"

#include "RepositoryBase.H"

template<class T,template<class> class PField,class Mesh>
void writeVolumeField(
    const string &name,
    const faMesh &mesh,
    const string &time,
    const dimensioned<T> &init,
    const GeometricField<T,PField,Mesh> &theField
);

template<class T,class Mesh>
void writeVolumeField(
    const string &name,
    const faMesh &mesh,
    const string &time,
    const dimensioned<T> &init,
    const GeometricField<T,faPatchField,Mesh> &theField
) {
    word vName(name+"Volume");
    Info << " Writing volume field to " << vName << endl;

    typedef GeometricField<T,fvPatchField,volMesh> vField;

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

    mapper.mapToVolume(theField, volField.boundaryField());
    volField.write();
}

template<class T,template<class> class PField,class Mesh>
void writeVolumeField(
    const string &name,
    const faMesh &mesh,
    const string &time,
    const dimensioned<T> &init,
    const GeometricField<T,PField,Mesh> &theField
) {
    typedef GeometricField<T,PField,Mesh> aFieldType;

    WarningIn("template<class T,template<class> class PField,class Mesh> void writeVolumeField")
        << "No way to interpolate a " << pTraits<aFieldType>::typeName
            << " to a volume-field. Skipping" << endl;
}

template<class T>
void setField
(
    const string &name,
    const faMesh &mesh,
    const string &time,
    const T &result,
    const scalarField &cond,
    bool create,
    const dimensionSet &dim,
    bool keepPatches,
    const wordList &valuePatches,
    bool createVolumeField,
    bool noWrite
) {
    dimensioned<typename T::value_type> init("nix",dim,typename T::value_type());

    T *pTemp;
    if(create) {
      pTemp=new T
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
      pTemp=new T
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

    FaFieldValueExpressionDriver::makePatches(*pTemp,keepPatches,valuePatches);

    FaFieldValueExpressionDriver::copyCalculatedPatches(*pTemp,result);

    label setCells=0;

    forAll(*pTemp,cellI) {
        if(cond[cellI]!=0) {
	  (*pTemp)[cellI]=result[cellI];
            setCells++;
        }
    }

    label totalCells=pTemp->size();
    reduce(totalCells,plusOp<label>());
    reduce(setCells,plusOp<label>());

    FaFieldValueExpressionDriver::setValuePatches(
        *pTemp,
        keepPatches,
        valuePatches
    );

    forAll(result.boundaryField(),patchI) {
        typename T::PatchFieldType &pf=pTemp->boundaryField()[patchI];
        const typename T::PatchFieldType &pfOrig=result.boundaryField()[patchI];

        if(pf.patch().coupled()) {
            pf==pfOrig;
        }
    }

    Info << " Setting " << setCells << " of " << totalCells << " cells" << endl;

    if(!noWrite) {
        Info << " Writing to " << name << endl;

        pTemp->write();
    }

    if(createVolumeField) {
        writeVolumeField(
            name,
            mesh,
            time,
            init,
            *pTemp
        );
    }
    delete pTemp;
}

void doAnExpression
(
    const fvMesh &mesh,
    const word &field,
    const exprString &expression,
    const exprString &condition,
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

    scalarField conditionField;
    bool evaluatedCondition=false;
    bool conditionIsSurface=false;
    if(condition!="true") {
        evaluatedCondition=true;

        driver.parse(condition);
        if(
            !driver.resultIsTyp<areaScalarField>(true)
            &&
            !driver.resultIsTyp<edgeScalarField>(true)
        ) {
            FatalErrorIn("doAnExpression()")
                << " condition: " << condition
                    << " does not evaluate to a logical expression"
                    << exit(FatalError);
        }

        if(driver.resultIsTyp<areaScalarField>(true)) {
            conditionField=driver.getResult<areaScalarField>().internalField();
            conditionIsSurface=false;
        } else {
            conditionField=driver.getResult<edgeScalarField>().internalField();
            conditionIsSurface=true;
        }
    }

    if (doDebug) {
        Info << "funkySetAreaFields : Parsing expression:"
            << expression << endl;
    }
    driver.parse(expression);
    if (doDebug) {
        Info << "funkySetAreaFields : Parsed expression" << endl;
    }

    if(!evaluatedCondition) {
        conditionIsSurface=driver.isSurfaceField();

        if(conditionIsSurface) {
            conditionField=scalarField(driver.aMesh().edges().size(),1);
        } else {
            conditionField=scalarField(driver.aMesh().faces().size(),1);
        }
    }

    if(create) {
        oldFieldType=driver.typ();
    }

    if(conditionIsSurface!=driver.isSurfaceField()) {
        FatalErrorIn("doAnExpression()")
            << "Inconsistent expression and condition. "
                << "Expression " << expression << " is defined on the "
                << (driver.isSurfaceField() ? "edges" : "faces")
                << " while condition " << condition << " is defined on "
                << (conditionIsSurface ? "edges" : "faces")
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
        if(driver.typ()==pTraits<areaScalarField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<areaScalarField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<areaVectorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<areaVectorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<areaTensorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<areaTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<areaSymmTensorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<areaSymmTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<areaSphericalTensorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<areaSphericalTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<edgeScalarField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<edgeScalarField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<edgeVectorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<edgeVectorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<edgeTensorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<edgeTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<edgeSymmTensorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<edgeSymmTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
            );
        } else if(driver.typ()==pTraits<edgeSphericalTensorField>::typeName) {
            setField(
                field,
                driver.aMesh(),
                time,
                driver.getResult<edgeSphericalTensorField>(),
                conditionField,
                create,
                dim,
                keepPatches,
                valuePatches,
                createVolumeField,
                noWrite
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
    argList::validOptions.insert("createVolumeField","");
    argList::validOptions.insert("onlyVolumeField","");
    argList::validOptions.insert("keepPatches","");
    argList::validOptions.insert("valuePatches","<list of patches that get a fixed value>");
    argList::validOptions.insert("dictExt","<extension to the default funkySetAreaFieldsDict-dictionary>");

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

    faMesh aMesh(mesh);
    aMesh.edgeCentres(); // to force the creation of a field that enables the areaMesh to be found

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "Time = " << runTime.timeName() << Foam::endl;

        RepositoryBase::updateRepos();

        mesh.readUpdate();

        if(args.options().found("field")) {
            Info << " Using command-line options\n" << endl;

            word field=args.options()["field"];

            exprString expression(
                args.options()["expression"],
                dictionary::null
            );

            exprString condition="true";
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

                exprString expression(
                    part["expression"],
                    part
                );

                exprString condition="true";

                if (part.found("condition")) {
                    condition=exprString(
                        part["condition"],
                        part
                    );
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
