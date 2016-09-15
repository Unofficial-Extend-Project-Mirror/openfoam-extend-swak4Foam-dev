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

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "panicDumpFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(panicDumpFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        panicDumpFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

panicDumpFunctionObject::panicDumpFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    fieldName_(""),
    maximum_(pTraits<scalar>::max),
    minimum_(pTraits<scalar>::min),
    storeAndWritePreviousState_(false)
{
}

bool panicDumpFunctionObject::start()
{
    simpleFunctionObject::start();

    fieldName_=word(dict_.lookup("fieldName"));
    minimum_=readScalar(dict_.lookup("minimum"));
    maximum_=readScalar(dict_.lookup("maximum"));

    Info << "Checking for field " << fieldName_ << " in range [ " << minimum_
        << " , " << maximum_ << " ] " << endl;

    if(dict_.found("storeAndWritePreviousState")) {
        storeAndWritePreviousState_=readBool(
            dict_.lookup("storeAndWritePreviousState")
        );
        if(storeAndWritePreviousState_) {
            Info << name() << " stores the previous time-steps" << endl;
            lastTimes_.set(
                new TimeCloneList(
                    dict_
                )
            );
        }
    } else {
        WarningIn("panicDumpFunctionObject::start()")
            << "storeAndWritePreviousState not set in" << dict_.name() << endl
                << "Assuming 'false'"
                << endl;

    }

    return true;
}

void panicDumpFunctionObject::writeSimple()
{
    check<volScalarField>();
    check<volVectorField>();
    check<volSphericalTensorField>();
    check<volSymmTensorField>();
    check<volTensorField>();

    if(lastTimes_.valid()) {
        lastTimes_->copy(time());
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
