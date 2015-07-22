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
    2006-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "FieldValueExpressionDriver.H"

#include "timeSelector.H"

#include "printSwakVersion.H"

#include "RepositoryBase.H"

#include "dlLibraryTable.H"

// template<typename T,typename PF,typename M>
template<typename F>
void doCorrectBoundaryConditions(
    bool correctBoundaryField,
    //    typename GeometricField<T,PF,M> &field
    F &field
)
{
    if(correctBoundaryField) {
        Info << "Correcting boundary conditions" << endl;
        field.correctBoundaryConditions();
    }
}

// template<class T,class PF>
// template<>
// void doCorrectBoundaryConditions<typename GeometricField<T,PF,surfaceMesh> >(
//     bool correctBoundaryField,
//     typename GeometricField<T,PF,surfaceMesh> &field
// )
// {
//     // no correctBoundaryConditions for surface-fields
// }

// this is embarassing. But I can't get the above specialization to work

#define NO_CORRECT_SPEC(FType) template<> \
    void doCorrectBoundaryConditions(     \
        bool correctBoundaryField,        \
        FType &field                      \
    )                                     \
    {                                     \
    }

NO_CORRECT_SPEC(surfaceScalarField)
NO_CORRECT_SPEC(surfaceVectorField)
NO_CORRECT_SPEC(surfaceTensorField)
NO_CORRECT_SPEC(surfaceSymmTensorField)
NO_CORRECT_SPEC(surfaceSphericalTensorField)

#undef NO_CORRECT_SPEC

template<class T,class Mesh>
void setField
(
    const string &name,
    const fvMesh &mesh,
    const Mesh &actualMesh,
    const string &time,
    const T &result,
    const scalarField &cond,
    bool create,
    const dimensionSet &dim,
    bool correctBoundaryField,
    bool keepPatches,
    const wordList &valuePatches
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
                mesh,
		IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            actualMesh,
            init
        );
    } else {
      pTemp=new T
        (
            IOobject
            (
                name,
                time,
                mesh,
		IOobject::MUST_READ,
                IOobject::NO_WRITE
            ),
            actualMesh
        );
    }

    FieldValueExpressionDriver::makePatches(*pTemp,keepPatches,valuePatches);
    FieldValueExpressionDriver::copyCalculatedPatches(*pTemp,result);

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

    FieldValueExpressionDriver::setValuePatches(*pTemp,keepPatches,valuePatches);

    forAll(result.boundaryField(),patchI) {
        typename T::PatchFieldType &pf=pTemp->boundaryField()[patchI];
        const typename T::PatchFieldType &pfOrig=result.boundaryField()[patchI];

        if(pf.patch().coupled()) {
            pf==pfOrig;
        }
    }

    Info << " Setting " << setCells << " of " << totalCells << " cells" << endl;

    doCorrectBoundaryConditions(correctBoundaryField,*pTemp);

    Info << " Writing to " << name << endl;

    pTemp->write();

    delete pTemp;
}

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
    bool correctBoundaryField,
    bool keepPatches,
    const wordList &valuePatches
) {
    setField(
        name,
        mesh,
        mesh,
        time,
        result,
        cond,
        create,
        dim,
        correctBoundaryField,
        keepPatches,
        valuePatches
    );
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
    bool correctBoundaryField,
    bool keepPatches,
    const wordList &valuePatches,
    const bool correctPatches
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
    bool conditionIsPoint=false;
    autoPtr<pointMesh> pMesh;

    if(condition!="true") {
        evaluatedCondition=true;

        if (doDebug) {
            Info << "funkySetFields : Parsing condition:"
                << condition << endl;
        }
        driver.parse(condition);
        if (doDebug) {
            Info << "funkySetFields : Parsed condition" << endl;
        }

        if(
            !driver.resultIsTyp<volScalarField>(true)
            &&
            !driver.resultIsTyp<surfaceScalarField>(true)
            &&
            !driver.resultIsTyp<pointScalarField>(true)
        ) {
            FatalErrorIn("doAnExpression()")
                << " condition: " << condition
                    << " does not evaluate to a logical expression"
                    << exit(FatalError);
        }

        if(driver.resultIsTyp<volScalarField>(true)) {
            conditionField=driver.getResult<volScalarField>(correctPatches).internalField();
            conditionIsSurface=false;
        } else if(driver.resultIsTyp<surfaceScalarField>(true)){
            conditionField=driver.getResult<surfaceScalarField>(correctPatches).internalField();
            conditionIsSurface=true;
        } else {
            conditionField=driver.getResult<pointScalarField>(correctPatches).internalField();
            conditionIsPoint=true;
        }
    }

    if (doDebug) {
        Info << "funkySetFields : Parsing expression:"
            << expression << endl;
    }
    driver.parse(expression);
    if (doDebug) {
        Info << "funkySetFields : Parsed expression" << endl;
    }

    if(!evaluatedCondition) {
        conditionIsSurface=driver.isSurfaceField();
        conditionIsPoint=driver.isPointField();

        if(conditionIsSurface) {
            conditionField=scalarField(mesh.nInternalFaces(),1);
        } else if(conditionIsPoint) {
            conditionField=scalarField(mesh.nPoints(),1);
        } else {
            conditionField=scalarField(mesh.cells().size(),1);
        }
    }

    if(conditionIsPoint) {
        pMesh.set(
            new pointMesh(mesh)
        );
    }

    if(create) {
        oldFieldType=driver.typ();
    }

    if(
        conditionIsSurface!=driver.isSurfaceField()
        ||
        conditionIsPoint!=driver.isPointField()
    ) {
        FatalErrorIn("doAnExpression()")
            << "Inconsistent expression and condition. "
                << "Expression " << expression << " is defined on the "
                << (
                    driver.isSurfaceField()
                    ?
                    "faces"
                    : (
                        driver.isPointField()
                        ?
                        "points"
                        :
                        "cells"
                    )
                )
                << " while condition " << condition << " is defined on "
                << (
                    conditionIsSurface
                    ?
                    "faces"
                    : (
                        conditionIsPoint
                        ?
                        "points"
                        :
                        "cells"
                    )
                )
                << endl
                << exit(FatalError);
    }

    if(
        oldFieldType=="IOobject"
        &&
        !create
    ) {
        FatalErrorIn("doAnExpression()")
            //            << args.executable()
                << " The type of the " << field << " is  "
                    << oldFieldType
                    << ". Seems that it doesn't exist. Use 'create'"
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
        if(
            driver.typ()==pTraits<volScalarField>::typeName
            ||
            driver.typ()=="volLogicalField"
        ) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volScalarField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volVectorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volVectorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volSymmTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volSymmTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<volSphericalTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<volSphericalTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(
            driver.typ()==pTraits<surfaceScalarField>::typeName
            ||
            driver.typ()=="surfaceLogicalField"
        ) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceScalarField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceVectorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceVectorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceSymmTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceSymmTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<surfaceSphericalTensorField>::typeName) {
            setField(
                field,
                mesh,
                time,
                driver.getResult<surfaceSphericalTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(
            driver.typ()==pTraits<pointScalarField>::typeName
            ||
            driver.typ()=="pointLogicalField"
        ) {
            setField(
                field,
                mesh,
                pMesh,
                time,
                driver.getResult<pointScalarField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<pointVectorField>::typeName) {
            setField(
                field,
                mesh,
                pMesh,
                time,
                driver.getResult<pointVectorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<pointTensorField>::typeName) {
            setField(
                field,
                mesh,
                pMesh,
                time,
                driver.getResult<pointTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<pointSymmTensorField>::typeName) {
            setField(
                field,
                mesh,
                pMesh,
                time,
                driver.getResult<pointSymmTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
                keepPatches,
                valuePatches
            );
        } else if(driver.typ()==pTraits<pointSphericalTensorField>::typeName) {
            setField(
                field,
                mesh,
                pMesh,
                time,
                driver.getResult<pointSphericalTensorField>(correctPatches),
                conditionField,
                create,
                dim,
                correctBoundaryField,
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

template<class FieldType>
void preLoadFieldsFunction(
    const DynamicList<fvMesh*> &meshes,
    const wordList &fieldNames,
    PtrList<FieldType> &fieldList
)
{
    forAll(meshes,m) {
        const fvMesh &mesh=*(meshes[m]);

        forAll(fieldNames,i) {
            const word &name=fieldNames[i];

            IOobject fieldHeader
                (
                    name,
                    mesh.time().timeName(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                );

            if
            (
                fieldHeader.headerOk()
                && fieldHeader.headerClassName() == pTraits<FieldType>::typeName
            )
            {
                Info << " Preloading " << name << " of type "
                    << pTraits<FieldType>::typeName
                    << " for mesh " << mesh.name() << endl;

                label sz=fieldList.size();
                fieldList.setSize(sz+1);
                fieldList.set(sz, new FieldType(fieldHeader, mesh));
            }
        }
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"

#   include "addLoadFunctionPlugins.H"

    argList::validOptions.insert("field","field to overwrite");
    argList::validOptions.insert("expression","expression to write");
    argList::validOptions.insert("condition","logical condition");
    argList::validOptions.insert("dimension","dimension of created field");
    argList::validOptions.insert("debugParser","");
    argList::validOptions.insert("noCacheVariables","");
    argList::validOptions.insert("create","");
    argList::validOptions.insert("keepPatches","");
    argList::validOptions.insert("valuePatches","list of patches that get a fixed value");
    argList::validOptions.insert("dictExt","extension to the default funkySetFieldsDict-dictionary");
    argList::validOptions.insert("allowFunctionObjects","");
    argList::validOptions.insert("addDummyPhi","");
    argList::validOptions.insert("otherCase","path to other case");
    argList::validOptions.insert("otherRegion","region in other case");
    argList::validOptions.insert("otherAdditionalRegions","region in other case that may be needed by coupled boundary conditions");
    argList::validOptions.insert("additionalRegions","regions that may be needed by coupled boundary conditions");
    argList::validOptions.insert("otherTime","time to use in other case");
    argList::validOptions.insert("otherHasSameTime","");
    argList::validOptions.insert("otherInterpolateOrder","order");
    argList::validOptions.insert("preloadFields","List of fields to preload");
    argList::validOptions.insert("noCorrectPatches","");
    argList::validOptions.insert("correctResultBoundaryFields","");

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

    autoPtr<surfaceScalarField> dummyPhi;

#   include "createNamedMesh.H"

#   include "loadFunctionPlugins.H"

    DynamicList<fvMesh*> allMeshes;
    allMeshes.append(&mesh);

    PtrList<fvMesh> additionalRegions;

    bool correctPatches=true;

    if (args.options().found("noCorrectPatches")) {
        correctPatches=false;
    }

    bool correctResultBoundaryFields=args.options().found("correctResultBoundaryFields");

    if (args.options().found("additionalRegions")) {
        string regionsString(args.options()["additionalRegions"]);
        IStringStream regionsStream("("+regionsString+")");
        wordList additionalRegionsNames(regionsStream);

        additionalRegions.resize(additionalRegionsNames.size());
        forAll(additionalRegionsNames,i)
        {
            const word &region=additionalRegionsNames[i];

            Info << "Loading additional mesh region " << region << endl;

            additionalRegions.set(
                i,
                new fvMesh(
                    IOobject(
                        region,
                        runTime.timeName(),
                        runTime,
                        Foam::IOobject::MUST_READ
                    )
                )
            );

            allMeshes.append(&(additionalRegions[i]));
        }
    }

    if(!args.options().found("allowFunctionObjects")) {
        runTime.functionObjects().off();
    }

    bool otherHasSameTime=args.options().found("otherHasSameTime");
    wordList otherAdditionalRegions;

    if(args.options().found("otherCase")) {
        word otherRegion(polyMesh::defaultRegion);
        if(args.options().found("otherRegion")) {
            otherRegion=word(args.options()["otherRegion"]);
        }
        string otherTime;
        if(!otherHasSameTime) {
            otherTime=string(args.options()["otherTime"]);
        }
        fileName otherCase(args.options()["otherCase"]);

        Info<< "Adding case " << otherCase << ", region "
            << otherRegion;
        if(otherHasSameTime) {
            Info << " with same time as 'real' time. ";
        } else {
            Info<< " at t=" << otherTime << ". ";
        }
        Info<< "Fields from that case can be accessed in expression with "
            << "'other(<field>)'\n" << endl;

        fvMesh &other=MeshesRepository::getRepository().addMesh(
            "other",
            otherCase,
            otherRegion
        );
        allMeshes.append(&(other));

        if (args.options().found("otherAdditionalRegions")) {
            string regionsString(args.options()["otherAdditionalRegions"]);
            IStringStream regionsStream("("+regionsString+")");
            otherAdditionalRegions=wordList(regionsStream);
        }

        forAll(otherAdditionalRegions,regionI) {
            const word &name=otherAdditionalRegions[regionI];
            Info << "Adding region " << name << " from other case" << endl;

            fvMesh &added=MeshesRepository::getRepository().addCoupledMesh(
                "other_"+name,
                "other",
                name
            );
            allMeshes.append(&(added));
        }

        if(args.options().found("otherInterpolateOrder")) {
            MeshesRepository::getRepository().setInterpolationOrder(
                "other",
                meshToMeshInterpolationNames[
                    args.options()["otherInterpolateOrder"]
                ]
            );
        }

        if(!otherHasSameTime) {
            scalar time=MeshesRepository::getRepository().setTime(
                "other",
                otherTime
            );
            Info << "Actually using time " << time << " in other case\n"
                << endl;
        }
    } else if(
        args.options().found("otherRegion")
        ||
        args.options().found("otherTime")
        ||
        args.options().found("otherAdditionalRegions")
        ||
        otherHasSameTime
    ) {
        FatalErrorIn(args.executable())
            << "'otherCase' not specified"
                << endl
                << exit(FatalError);

    }

    allMeshes.shrink();

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "\nTime = " << runTime.timeName() << Foam::endl;

        RepositoryBase::updateRepos();

        mesh.readUpdate();

        if(otherHasSameTime) {
            scalar time=MeshesRepository::getRepository().setTime(
                "other",
                runTime.timeName(),
                timeI
            );
            Info << "Other mesh set to time " << time << endl;
        }

        if(
            args.options().found("addDummyPhi")
            &&
            !dummyPhi.valid()
        ) {
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

            PtrList<volScalarField> vsf;
            PtrList<volVectorField> vvf;
            PtrList<volTensorField> vtf;
            PtrList<volSymmTensorField> vyf;
            PtrList<volSphericalTensorField> vhf;

            PtrList<surfaceScalarField> ssf;
            PtrList<surfaceVectorField> svf;
            PtrList<surfaceTensorField> stf;
            PtrList<surfaceSymmTensorField> syf;
            PtrList<surfaceSphericalTensorField> shf;

            if(args.options().found("preloadFields")) {
                IStringStream preloadStream(
                    "("+args.options()["preloadFields"]+")"
                );

                wordList preLoadFields(preloadStream);

                preLoadFieldsFunction(allMeshes,preLoadFields,vsf);
                preLoadFieldsFunction(allMeshes,preLoadFields,vvf);
                preLoadFieldsFunction(allMeshes,preLoadFields,vtf);
                preLoadFieldsFunction(allMeshes,preLoadFields,vyf);
                preLoadFieldsFunction(allMeshes,preLoadFields,vhf);

                preLoadFieldsFunction(allMeshes,preLoadFields,ssf);
                preLoadFieldsFunction(allMeshes,preLoadFields,svf);
                preLoadFieldsFunction(allMeshes,preLoadFields,stf);
                preLoadFieldsFunction(allMeshes,preLoadFields,syf);
                preLoadFieldsFunction(allMeshes,preLoadFields,shf);
            }

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
                correctResultBoundaryFields,
                keepPatches,
                valuePatches,
                correctPatches
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

                PtrList<volScalarField> vsf;
                PtrList<volVectorField> vvf;
                PtrList<volTensorField> vtf;
                PtrList<volSymmTensorField> vyf;
                PtrList<volSphericalTensorField> vhf;

                PtrList<surfaceScalarField> ssf;
                PtrList<surfaceVectorField> svf;
                PtrList<surfaceTensorField> stf;
                PtrList<surfaceSymmTensorField> syf;
                PtrList<surfaceSphericalTensorField> shf;

                if(part.found("preloadFields")) {
                    wordList preLoadFields(part.lookup("preloadFields"));

                    preLoadFieldsFunction(allMeshes,preLoadFields,vsf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,vvf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,vtf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,vyf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,vhf);

                    preLoadFieldsFunction(allMeshes,preLoadFields,ssf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,svf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,stf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,syf);
                    preLoadFieldsFunction(allMeshes,preLoadFields,shf);
                }

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
                bool correctResultBoundaryFieldsLocal=correctResultBoundaryFields;
                if(part.found("correctResultBoundaryFields")) {
                    correctResultBoundaryFieldsLocal=readBool(
                        part["correctResultBoundaryFields"]
                    );
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
                    correctResultBoundaryFieldsLocal,
                    keepPatches,
                    valuePatches,
                    correctPatches
                );
            }
        }
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
