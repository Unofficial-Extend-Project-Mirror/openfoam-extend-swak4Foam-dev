/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "readAndUpdateFields.H"
#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <class FType>
void Foam::readAndUpdateFields::correctBoundaryConditions(
    PtrList<FType> &flst
) {
    forAll(flst,i)
    {
	flst[i].correctBoundaryConditions();
        if(
            this->obr_.time().outputTime()
            &&
            flst[i].writeOpt()==IOobject::AUTO_WRITE
        ) {
            flst[i].write();
        }
    }
}

template<class Type>
bool Foam::readAndUpdateFields::loadField
(
    const word& fieldName,
    PtrList<GeometricField<Type, fvPatchField, volMesh> >& vflds,
    PtrList<GeometricField<Type, pointPatchField, pointMesh> >& pflds
)
{
    typedef GeometricField<Type, fvPatchField, volMesh> vfType;
    typedef GeometricField<Type, fvsPatchField, surfaceMesh> sfType;
    typedef GeometricField<Type, pointPatchField, pointMesh> pfType;

    if (obr_.foundObject<vfType>(fieldName))
    {
        if (debug)
        {
            Info<< "readAndUpdateFields : Field " << fieldName << " already in database"
                << endl;
        }
    }
    else if (obr_.foundObject<sfType>(fieldName))
    {
        if (debug)
        {
            Info<< "readAndUpdateFields : Field " << fieldName << " already in database"
                << endl;
        }
    }
    else if (obr_.foundObject<pfType>(fieldName))
    {
        if (debug)
        {
            Info<< "readAndUpdateFields : Field " << fieldName << " already in database"
                << endl;
        }
    }
    else
    {
        const fvMesh& mesh = refCast<const fvMesh>(obr_);

        IOobject fieldHeader
        (
            fieldName,
            mesh.time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        );

        if
        (
            fieldHeader.headerOk()
         && fieldHeader.headerClassName() == vfType::typeName
        )
        {
            // store field locally
            Info<< "    Reading " << fieldName << endl;
            label sz = vflds.size();
            vflds.setSize(sz+1);
            vflds.set(sz, new vfType(fieldHeader, mesh));
            //            vflds[sz].writeOpt()=IOobject::AUTO_WRITE;

            return true;
        }
        else if
        (
            fieldHeader.headerOk()
         && fieldHeader.headerClassName() == pfType::typeName
        )
        {
            // store field locally
            Info<< "    Reading " << fieldName << endl;
            label sz = pflds.size();
            pflds.setSize(sz+1);
            pflds.set(sz, new pfType(fieldHeader, this->pMesh(mesh)));
            //            pflds[sz].writeOpt()=IOobject::AUTO_WRITE;

            return true;
        }
        else if
        (
            fieldHeader.headerOk()
         && fieldHeader.headerClassName() == sfType::typeName
        )
        {
            WarningIn("Foam::readAndUpdateFields::loadField")
                << "Field " << fieldName << " is a "
                    << sfType::typeName
                    << " and surface-fields don't support correctBoundaryConditions"
                    << endl << "-> Not read"
                    << endl;

            return true;
        }
    }
    return false;
}


// ************************************************************************* //
