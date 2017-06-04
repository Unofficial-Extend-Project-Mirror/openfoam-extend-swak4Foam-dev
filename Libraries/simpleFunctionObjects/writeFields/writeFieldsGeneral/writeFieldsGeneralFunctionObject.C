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
    2008-2011, 2013-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2014 David Huckaby <e.david.huckaby@netl.doe.gov>
    2015 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "writeFieldsGeneralFunctionObject.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "swakCloudTypes.H"

#include "basicKinematicCloud.H"
#ifdef FOAM_HAS_COLLIDING_CLOUD
#include "basicKinematicCollidingCloud.H"
#endif
#ifdef FOAM_HAS_MPPIC_CLOUD
#include "basicKinematicMPPICCloud.H"
#endif
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#include "BasicReactingMultiphaseCloud.H"
#include "BasicReactingCloud.H"
#else
#include "basicReactingMultiphaseCloud.H"
#include "basicReactingCloud.H"
#endif

#include "cloud.H"
// #include "const_iterator.H"

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
    fieldNames_(0),
    cloudNames_(0)
{
}

bool writeFieldsGeneralFunctionObject::start()
{
    simpleFunctionObject::start();

    fieldNames_=wordList(dict_.lookup("fieldNames"));
    if(dict_.found("cloudNames")) {
        cloudNames_=wordList(dict_.lookup("cloudNames"));
    } else {
        WarningIn("writeFieldsGeneralFunctionObject::writeFieldsGeneralFunctionObject")
            << "No list of clouds to be written specified in "
                << dict_.name() << nl
                << "If you want to write lagrangian particles add a "
                << "parameter 'cloudNames' with the names of the clouds"
                << endl;
    }
    return true;
}

// bool writeFieldsGeneralFunctionObject::outputTime()
// {
//     return (
//         time().outputTime()
//         &&
//         time().time().value()>=after());
// }


void writeFieldsGeneralFunctionObject::writeSimple()
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
            WarningIn("writeFieldsGeneralFunctionObject::writeSimple()")
                << " More than one (" << cnt
                    << ") fields are known by the name " << name << endl;
        } else if(cnt<0) {
            WarningIn("writeFieldsGeneralFunctionObject::writeSimple()")
                << " No field with the name " << name
                    << " found" << endl;
        } else {
            totalCnt++;
        }
    }

    Info << name() << " triggered writing of " << totalCnt << " fields" << endl;


    totalCnt=0;
    forAll(cloudNames_,i) {

        const word &name=cloudNames_[i];
        label cnt=0;

        cnt += writeCloud<basicKinematicCloud>(name);
#ifdef FOAM_HAS_COLLIDING_CLOUD
        cnt+=writeCloud<basicKinematicCollidingCloud>(name);
#endif
#ifdef FOAM_HAS_MPPIC_CLOUD
        cnt+=writeCloud<basicKinematicMPPICCloud>(name);
#endif
        cnt+=writeCloud<swakFluidThermoCloudType>(name);
#ifdef FOAM_REACTINGCLOUD_TEMPLATED
        cnt+=writeCloud<constThermoReactingCloud>(name);
        cnt+=writeCloud<thermoReactingCloud>(name);
        cnt+=writeCloud<icoPoly8ThermoReactingCloud>(name);
        cnt+=writeCloud<constThermoReactingMultiphaseCloud>(name);
        cnt+=writeCloud<thermoReactingMultiphaseCloud>(name);
        cnt+=writeCloud<icoPoly8ThermoReactingMultiphaseCloud>(name);
#else
        cnt+=writeCloud<basicReactingCloud>(name);
        cnt+=writeCloud<basicReactingMultiphaseCloud>(name);;
#endif
        if(cnt>1) {
            WarningIn("writeFieldsGeneralFunctionObject::writeSimple()")
                << " More than one (" << cnt
                    << ") clouds are known by the name " << name << endl;
        } else if(cnt<0) {
            WarningIn("writeFieldsGeneralFunctionObject::writeSimple()")
                << " No clouds with the name " << name
                    << " found" << endl;
        } else {
            totalCnt++;
        }
    }

    Info << name() << " triggered writing of " << totalCnt << " clouds" << endl;


}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
