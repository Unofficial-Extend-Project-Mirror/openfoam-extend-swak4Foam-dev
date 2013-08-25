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

Description
    Load a field into memory if it exists

Contributors/Copyright:
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

template<class FieldType>
bool loadFieldFunction(
    fvMesh &mesh,
    const word &fName,
    SLPtrList<FieldType> &fieldList
) {
    const Time &runTime=mesh.time();

    IOobject f
        (
            fName,
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
    f.headerOk();
    word className=f.headerClassName();

    if(className==FieldType::typeName) {
        Info << "Reading field " << fName << " of type "
            << FieldType::typeName << endl;
        fieldList.append(
            new FieldType
            (
                IOobject
                (
                    fName,
                    runTime.timeName(),
                    mesh,
                    IOobject::MUST_READ,
                    IOobject::AUTO_WRITE
                ),
                mesh
            )
        );
        return true;
    } else {
        return false;
    }
}

template<class FieldType>
string reloadFieldsFunction(
    fvMesh &mesh,
    SLPtrList<FieldType> &fieldList
) {
    if(fieldList.size()<=0) {
        return "";
    }
    const Time &runTime=mesh.time();
    string result="Reloading type "+FieldType::typeName+": ";

    forAllIter(typename SLPtrList<FieldType>,fieldList,i){
        FieldType &field=(*i);
        const word &fName=field.name();
        result+=fName+" ";

        IOobject f
            (
                fName,
                runTime.timeName(),
                mesh,
                IOobject::MUST_READ,
                IOobject::NO_WRITE
            );
        if(f.headerOk()) {
            word className=f.headerClassName();

            if(className==FieldType::typeName) {
                result+="ok ";
                FieldType newField(
                    IOobject
                    (
                        fName,
                        runTime.timeName(),
                        mesh,
                        IOobject::MUST_READ,
                        IOobject::AUTO_WRITE
                    ),
                    mesh
                );
                field==newField;
            } else {
                result+="wrong:"+className+" ";
            }
        } else {
            result+="missing ";
        }
        result+="...";
    }
    result+="\n";

    return result;
}


// ************************************************************************* //
