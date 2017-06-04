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
    2008-2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fieldDistributionFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(fieldDistributionFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        fieldDistributionFunctionObject,
        dictionary
    );



// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

fieldDistributionFunctionObject::fieldDistributionFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    distributionFunctionObject(name,t,dict),
    fieldName_(dict.lookup("fieldName"))
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word fieldDistributionFunctionObject::dirName()
{
    return typeName+"_"+fieldName_;
}

word fieldDistributionFunctionObject::baseName()
{
    return fieldName_;
}

void fieldDistributionFunctionObject::getDistribution()
{
    getDistributionInternal(distScalar_);
    getDistributionInternal(distVector_);
    getDistributionInternal(distTensor_);
    getDistributionInternal(distSymmTensor_);
    getDistributionInternal(distSphericalTensor_);
}


} // namespace Foam

// ************************************************************************* //
