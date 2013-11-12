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
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "ReaderParticleCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template <typename T>
void ReaderParticleCloud::addDataInternal(
    const word &name,
    const Field<T> &inData,
    HashTable<Field<T>,word> &data,
    bool overwrite
) {
    if(data.found("name") && !overwrite) {
        FatalErrorIn("ReaderParticleCloud::addDataInternal")
            << "Already got a field named " << name << " in the table for Field<"
                << pTraits<T>::typeName << ">"
                << endl
                << exit(FatalError);
    }

    if(inData.size()!=this->size()) {
        FatalErrorIn("ReaderParticleCloud::addDataInternal")
            << "Length " << inData.size() << " of field " << name << " of type "
                << pTraits<T>::typeName << " does not fit the size "
                << this->size() << " of the cloud " << this->name()
                << endl
                << exit(FatalError);
    }
    if(data.found("name")) {
        data.set(name,inData);
    } else {
        data.insert(name,inData);
    }
}

template <typename T>
void ReaderParticleCloud::setValues(
    const word &name,
    const Field<T> &values
) {
    const word typeName=pTraits<T>::typeName;
    if(typeName==pTraits<scalar>::typeName) {
        addDataInternal(name,values,scalarData_,true);
    } else {
        FatalErrorIn("ReaderParticleCloud::setValues")
            << "Unsupported type " << typeName
                << endl
                << exit(FatalError);
    }
}

template <typename T>
const Field<T> &ReaderParticleCloud::getValues(
    const word &name
) {
    const word typeName=pTraits<T>::typeName;
    if(typeName==pTraits<scalar>::typeName) {
        return scalarData_[name];
    } else {
        FatalErrorIn("ReaderParticleCloud::getValues")
            << "Unsupported type " << typeName
                << endl
                << exit(FatalError);

        Field<T> dummy;
        return dummy;
    }
}

template <typename T>
void ReaderParticleCloud::writeData(
    const HashTable<Field<T>,word> &data
) const {
    const wordList toc(data.toc());
    forAll(toc,i)
    {
	IOField<T> v(
            this->fieldIOobject(
                toc[i],
                IOobject::NO_READ
            ),
            data[toc[i]]
        );
        v.write();
    }
}

}

// ************************************************************************* //
