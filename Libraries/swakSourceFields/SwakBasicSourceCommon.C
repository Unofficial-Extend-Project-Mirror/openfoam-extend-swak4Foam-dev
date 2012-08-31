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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "SwakBasicSourceCommon.H"
#include "fvMesh.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// Construct from dictionary
template<class T>
SwakBasicSourceCommon<T>::SwakBasicSourceCommon
(
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
)
:
    basicSource(name, modelType, dict, mesh),
    driver_(coeffs(),mesh),
    verbose_(dict.lookupOrDefault<bool>("verbose",true))
{
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class T>
SwakBasicSourceCommon<T>::~SwakBasicSourceCommon()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class T>
void SwakBasicSourceCommon<T>::setFieldData(const dictionary& dict)
{
    fieldNames_.setSize(dict.toc().size());
    expressions_.setSize(fieldNames_.size());

    applied_.setSize(fieldNames_.size(), false);

    label i = 0;
    forAllConstIter(dictionary, dict, iter)
    {
        fieldNames_[i] = iter().keyword();
        dict.lookup(iter().keyword()) >> expressions_[i];
        i++;
    }
}

template<class T>
void SwakBasicSourceCommon<T>::writeData(Ostream& os) const {
    os  << indent << name_ << endl;
    dict_.write(os);
}


template<class T>
bool SwakBasicSourceCommon<T>::read(const dictionary& dict) {
    if (basicSource::read(dict))
    {
        setFieldData(coeffs_.subDict("expressions"));
        return true;
    }
    else
    {
        return false;
    }
}

} // end namespace

// ************************************************************************* //
