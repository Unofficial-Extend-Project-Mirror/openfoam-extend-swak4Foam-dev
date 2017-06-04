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

#include "volumeFieldFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(volumeFieldFunctionObject, 0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

volumeFieldFunctionObject::volumeFieldFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    timelineFunctionObject(name,t,dict),
    fieldNames_(0),
    scalarFields_(0),
    vectorFields_(0),
    sphericalTensorFields_(0),
    symmTensorFields_(0),
    tensorFields_(0)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void volumeFieldFunctionObject::writeSimple()
{
    processAndWrite<scalar>(scalarFields_);
    processAndWrite<vector>(vectorFields_);
    processAndWrite<sphericalTensor>(sphericalTensorFields_);
    processAndWrite<symmTensor>(symmTensorFields_);
    processAndWrite<tensor>(tensorFields_);
}

bool volumeFieldFunctionObject::start()
{
    fieldNames_ = wordList(dict_.lookup("fields"));

    boolList foundFields(fieldNames_.size(), false);
    findFields<volScalarField>(scalarFields_, foundFields);
    findFields<volVectorField>(vectorFields_, foundFields);
    findFields<volSphericalTensorField>(sphericalTensorFields_, foundFields);
    findFields<volSymmTensorField>(symmTensorFields_, foundFields);
    findFields<volTensorField>(tensorFields_, foundFields);

    label validFieldi=0;
    forAll(fieldNames_, fieldi)
    {
        if (foundFields[fieldi])
        {
            fieldNames_[validFieldi++] = fieldNames_[fieldi];
        }
        else
        {
            WarningIn("probes::read()")
                << "Unknown field " << fieldNames_[fieldi]
                << " when reading dictionary " << dict_.name() << endl
                << "    Can only probe registered volScalar, volVector, "
                << "volSphericalTensor, volSymmTensor and volTensor fields"
                << nl << endl;
        }
    }

    fieldNames_.setSize(validFieldi);


    if (Pstream::master())
    {
        if (debug)
        {
            Pout<< "Probing fields:" << fieldNames_ << nl;
        }

        // Check if any fieldNames have been removed. If so close
        // the file.
    }

    timelineFunctionObject::start();

    return true;
}

wordList volumeFieldFunctionObject::fileNames()
{
    return fieldNames_;
}

} // namespace Foam

// ************************************************************************* //
