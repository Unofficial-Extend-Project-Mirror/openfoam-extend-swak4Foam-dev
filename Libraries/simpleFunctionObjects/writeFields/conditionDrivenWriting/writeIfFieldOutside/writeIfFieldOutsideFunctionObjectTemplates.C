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

Contributors/Copyright:
    2008-2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: writeIfFieldOutsideFunctionObjectTemplates.C,v 78b0d113b99b 2013-02-25 16:12:41Z bgschaid $
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

template<>
scalar writeIfFieldOutsideFunctionObject::getMin<volScalarField>() const
{
    const volScalarField &fld=obr_.lookupObject<volScalarField>(fieldName_);

    return min(fld).value();
}

template<>
scalar writeIfFieldOutsideFunctionObject::getMax<volScalarField>() const
{
    const volScalarField &fld=obr_.lookupObject<volScalarField>(fieldName_);

    return max(fld).value();
}

template<class T>
scalar writeIfFieldOutsideFunctionObject::getMin() const
{
    const T &fld=obr_.lookupObject<T>(fieldName_);

    return min(mag(fld)).value();
}

template<class T>
scalar writeIfFieldOutsideFunctionObject::getMax() const
{
    const T &fld=obr_.lookupObject<T>(fieldName_);

    return max(mag(fld)).value();
}

template<class T>
bool writeIfFieldOutsideFunctionObject::check() const
{
    if(obr_.foundObject<T>(fieldName_)) {
        scalar mini=getMin<T>();
        scalar maxi=getMax<T>();

        if(mini<minimum_ || maxi>maximum_) {
            return true;
        }
    }

    return false;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
