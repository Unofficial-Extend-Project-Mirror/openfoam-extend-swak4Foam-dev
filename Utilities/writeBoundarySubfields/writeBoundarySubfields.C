/*---------------------------------------------------------------------------*\
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
    writeBoundarySubfields

Description

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "timeSelector.H"

#include "OFstream.H"

#include "printSwakVersion.H"

#include "RepositoryBase.H"

#include "dlLibraryTable.H"

#include "IFstream.H"

#include "swak.H"

template<class FType>
void makeSubfieldField(
    const dictionary &field,
    const word &name,
    const fvMesh &mesh
) {
    word fieldName(field.name().name()+"_"+name);
    Info << "Creating field " << fieldName << " of type "
        << FType::typeName << endl;

    bool foundInstance=false;
    wordList patchTypes(mesh.boundary().size(),"zeroGradient");
    const dictionary &bDict=field.subDict("boundaryField");

    forAll(mesh.boundary(),patchI) {
        const word &pName=mesh.boundary()[patchI].name();
        if(!bDict.found(pName)) {
            continue;
        }
        const dictionary &pSpec=bDict.subDict(pName);
        if(pSpec.found(name)) {
            patchTypes[patchI]="fixedValue";
            foundInstance=true;
        }
    }

    if(!foundInstance) {
        Info << "No patch has subfield " << name
            <<". Not writing\n" << endl;
        return;
    }

    FType result(
        IOobject (
            fieldName,
            mesh.time().timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::NO_WRITE
        ),
        mesh,
        dimensioned<typename FType::value_type>(
            name,
            dimless,
            pTraits<typename FType::value_type>::zero
        ),
        patchTypes
    );

    forAll(result.boundaryField(),patchI) {
        fvPatchField<typename FType::value_type> &pField=
            const_cast<fvPatchField<typename FType::value_type> &>(
                result.boundaryField()[patchI]
            );
        const word &pName=pField.patch().name();
        if(!bDict.found(pName)) {
            continue;
        }
        const dictionary &pSpec=bDict.subDict(pName);
        if(pSpec.found(name)) {
            Field<typename FType::value_type> values(
                name,
                pSpec,
                pField.size()
            );
            pField==values;
        }
    }

    Info << "Writing\n" << endl;
    result.write();
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);

#   include "addRegionOption.H"

    word defaultSubfieldSpec("refValue:native,refGradient:native,valueFraction:scalar");

    argList::validArgs.append("fieldName");

#ifdef FOAM_DEV
    argList::validOptions.insert("subfields","subfieldSpec");
#else
    argList::addOption(
        "subfields",
        "subfieldSpec",
        "A string with the specificion of the subfields to create fields "
        "for. The specifications are separated by ','. Each specification "
        "is of the form <name>:<type> (with type being 'scalar', 'vector', "
        "'symmTensor' or 'sphericalTensor'. A type 'native' assumes that "
        "the same type as the field should be used). If unset the default "
        "value "+defaultSubfieldSpec+" is used"
    );
#endif

        #   include "setRootCase.H"

    printSwakVersion();

    word fieldName(args.args()[1]);
    word subfieldSpec=defaultSubfieldSpec;
    if(args.options().found("subfields")) {
        subfieldSpec=word(args.options()["subfields"]);
    }

    typedef HashTable<word,word> specType;
    specType subfields;
    {
        string::size_type pnt=0;
        while(pnt<subfieldSpec.length()) {
            string::size_type end=subfieldSpec.find(',',pnt+1);
            word spec;
            if(end==string::npos) {
                spec=subfieldSpec.substr(pnt,subfieldSpec.length()-pnt);
                pnt=subfieldSpec.length();
            } else {
                spec=subfieldSpec.substr(pnt,end-pnt);
                pnt=end+1;
            }
            string::size_type middle=spec.find(":");
            if(middle==string::npos) {
                FatalErrorIn(args.executable())
                    << "The specification " << spec
                        << " is not of the form '<name>:<type>' (missing ':')"
                        << endl
                        << exit(FatalError);
            }
            word name=spec.substr(0,middle);
            word type=spec.substr(middle+1);
            subfields.insert(name,type);
        }
    }

#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Foam::Info<< "Time = " << runTime.timeName() << Foam::endl;

        mesh.readUpdate();

        IOobject header(
            fieldName,
            mesh.time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
        word className;
        if(
#ifdef FOAM_HAS_TYPE_HEADER_OK
	    header.typeHeaderOk<IOobject>(false)
#else
            header.headerOk()
#endif
        ) {
            className=header.headerClassName();
            Info << " Reading Field " << fieldName << " of type "
                << className << endl;
        } else {
            Info << " No " << fieldName << " at this time" << endl;
            continue;
        }
        IOdictionary field(
            IOobject
            (
                fieldName,
                runTime.timeName(),
                mesh,
                IOobject::NO_READ,
                IOobject::NO_WRITE
            )
        );
        // deregister the dictionary so that the field can work on itself
        field.checkOut();
        {
            // this way it doesn't matter that the file is not of the right class
            IFstream inStream(field.filePath());
            field.readHeader(inStream);
            field.readData(inStream);
        }

        forAllConstIter(specType,subfields,iter) {
            const word &name=iter.key();
            const word &type=*iter;

            if(type=="native") {
                if(
                    className=="volScalarField"
                    ||
                    className=="surfaceScalarField"
                ) {
                    makeSubfieldField<volScalarField>(field,name,mesh);
                } else if(
                    className=="volVectorField"
                    ||
                    className=="surfaceVectorField"
                ) {
                        makeSubfieldField<volVectorField>(field,name,mesh);
                } else if(
                    className=="volTensorField"
                    ||
                    className=="surfaceTensorField"
                ) {
                    makeSubfieldField<volTensorField>(field,name,mesh);
                } else if(
                    className=="volSymmTensorField"
                    ||
                    className=="surfaceSymmTensorField"
                ) {
                    makeSubfieldField<volSymmTensorField>(field,name,mesh);
                } else if(
                    className=="volSphericalTensorField"
                    ||
                    className=="surfaceSphericalTensorField"
                ) {
                    makeSubfieldField<volSphericalTensorField>(field,name,mesh);
                } else {
                    FatalErrorIn(args.executable())
                        << "Don't know 'native' type for class " << className
                            << endl
                            << exit(FatalError);
                }
            } else if(type=="scalar") {
                makeSubfieldField<volScalarField>(field,name,mesh);
            } else if(type=="vector") {
                makeSubfieldField<volVectorField>(field,name,mesh);
            } else if(type=="tensor") {
                makeSubfieldField<volTensorField>(field,name,mesh);
            } else if(type=="symmTensor") {
                makeSubfieldField<volSymmTensorField>(field,name,mesh);
            } else if(type=="sphericalTensor") {
                makeSubfieldField<volSphericalTensorField>(field,name,mesh);
            } else {
                FatalErrorIn(args.executable())
                    << "Type " << type << " specified for subfield "
                        << name << " not implemented"
                        << endl
                        << exit(FatalError);
            }
        }
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
