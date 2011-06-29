//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
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

\*---------------------------------------------------------------------------*/

#include "volumeFieldFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class T>
void volumeFieldFunctionObject::findFields(wordList& typeFieldNames, boolList& foundFields)
{
    typeFieldNames.setSize(fieldNames_.size());
    label typeFieldI = 0;

    forAll(fieldNames_, fieldI)
    {
        const word& fldName = fieldNames_[fieldI];

        if (obr_.foundObject<T>(fldName))
        {
            typeFieldNames[typeFieldI++] = fldName;
            foundFields[fieldI] = true;
        }
    }

    typeFieldNames.setSize(typeFieldI);
}


template <class T>
void volumeFieldFunctionObject::processAndWrite(const word& fieldName)
{
    const GeometricField<T, fvPatchField, volMesh>& fld =
        obr_.lookupObject<GeometricField<T, fvPatchField, volMesh> >
        (
            fieldName
        );

    // Make sure all processors call sample
    Field<T> vals(process(fieldName,-VGREAT*pTraits<T>::one));

    if (Pstream::master())
    {
        unsigned int w = IOstream::defaultPrecision() + 7;

        OFstream& probeStream = *filePtrs_[fieldName];

        probeStream << setw(w) << fld.time().value();

        forAll(vals, probeI)
        {
            probeStream << setw(w) << vals[probeI];
        }
        probeStream << nl;
    }
}

template <class T>
void volumeFieldFunctionObject::processAndWrite(const wordList& typeFields)
{
    forAll(typeFields, i)
    {
        processAndWrite<T>(typeFields[i]);
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
