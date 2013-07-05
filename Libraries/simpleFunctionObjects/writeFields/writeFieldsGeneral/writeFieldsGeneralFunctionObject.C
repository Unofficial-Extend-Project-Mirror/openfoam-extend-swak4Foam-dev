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
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "writeFieldsGeneralFunctionObject.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "Time.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

writeFieldsGeneralFunctionObject::writeFieldsGeneralFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    fieldNames_(0)
{
}

bool writeFieldsGeneralFunctionObject::start()
{
    simpleFunctionObject::start();

    fieldNames_=wordList(dict_.lookup("fieldNames"));

    return true;
}

// bool writeFieldsGeneralFunctionObject::outputTime()
// {
//     return (
//         time().outputTime()
//         &&
//         time().time().value()>=after());
// }


void writeFieldsGeneralFunctionObject::write()
{
    label totalCnt=0;

    forAll(fieldNames_,i) {
        const word &name=fieldNames_[i];
        label cnt=0;

        cnt+=writeField<volScalarField>(name);
        cnt+=writeField<volVectorField>(name);
        cnt+=writeField<volSphericalTensorField>(name);
        cnt+=writeField<volSymmTensorField>(name);
        cnt+=writeField<volTensorField>(name);

        cnt+=writeField<surfaceScalarField>(name);
        cnt+=writeField<surfaceVectorField>(name);
        cnt+=writeField<surfaceSphericalTensorField>(name);
        cnt+=writeField<surfaceSymmTensorField>(name);
        cnt+=writeField<surfaceTensorField>(name);

        if(cnt>1) {
            WarningIn("writeFieldsGeneralFunctionObject::write()")
                << " More than one (" << cnt
                    << ") fields are known by the name " << name << endl;
        } else if(cnt<0) {
            WarningIn("writeFieldsGeneralFunctionObject::write()")
                << " No field with the name " << name
                    << " found" << endl;
        } else {
            totalCnt++;
        }
    }
    Info << name() << " triggered writing of " << totalCnt << " fields" << endl;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
