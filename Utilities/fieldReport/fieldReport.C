/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright (C) 1991-2005 OpenCFD Ltd.
    \\/      M anipulation   |
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

Application
    fieldReport

Description

Contributors/Copyright:
    2011-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "FieldValueExpressionDriver.H"

#include "timeSelector.H"

#include "IFstream.H"

#include "printSwakVersion.H"

#include "AccumulationCalculation.H"

// Yeah. I know. Global variables. Eeeeevil. But convenient
label nrOfQuantiles=9;

template <typename Type>
void reportValues(
    const word &fieldName,
    CommonValueExpressionDriver &driver
) {
    Field<Type> result(
        driver.evaluate<Type>(fieldName)
    );

    AccumulationCalculation<Type> calculator(
        result,
        false,
        driver
    );

    Info << "Min: " << calculator.minimum() << endl;
    Info << "Max: " << calculator.maximum() << endl;
}

template <typename Type>
bool reportAField(
    const word &fieldName,
    const fvMesh &mesh
) {
    IOobject header
        (
            fieldName,
            mesh.time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
    if(
        header.headerOk()
        &&
        header.headerClassName()==Type::typeName
    ) {
        Info << " Reading Field " << fieldName << " of type "
            << Type::typeName << endl;
    } else {
        return false;
    }

    Type theField(
        IOobject
        (
            fieldName,
            mesh.time().timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        ),
        mesh
    );

    Info << "\nInternal field:" << endl;
    FieldValueExpressionDriver fieldDriver(
        mesh,
        false,
        true,     // search in Memory
        false
    );
    reportValues<typename Type::value_type>(
        fieldName,
        fieldDriver
    );

    return true;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{

    Foam::timeSelector::addOptions(false);
    Foam::argList::validArgs.append("fieldName");
#   include "addRegionOption.H"
    argList::validOptions.insert(
        "nrOfQuantiles",
        "<nr of quantiles - defaults to 9>"
    );

#   include "setRootCase.H"

    printSwakVersion();

    word fieldName(args.args()[1]);
    nrOfQuantiles=9;
    if(args.options().found("nrOfQuantiles")) {
        nrOfQuantiles=readLabel(
            IStringStream(args.options()["nrOfQuantiles"])()
        );
    }

    if (!args.options().found("time") && !args.options().found("latestTime")) {
        FatalErrorIn("main()")
            << args.executable()
                << ": time/latestTime option is required" << endl
            << exit(FatalError);
    }


#   include "createTime.H"
    Foam::instantList timeDirs = Foam::timeSelector::select0(runTime, args);

#   include "createNamedMesh.H"

    forAll(timeDirs, timeI)
    {
        runTime.setTime(timeDirs[timeI], timeI);

        Info << "\nTime = " << runTime.timeName() << endl;
        Info << endl;

        mesh.readUpdate();

        bool found=false;
        found = found || reportAField<volScalarField>(fieldName,mesh);
        found = found || reportAField<volVectorField>(fieldName,mesh);

        found = found || reportAField<surfaceScalarField>(fieldName,mesh);

        //        found = found || reportAField<pointScalarField>(fieldName,mesh);

        if(!found) {
            WarningIn(args.executable())
                << "No field " << fieldName << " found at "
                    << mesh.time().timeName()
                    << endl;
        }

        Info << endl;
   }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
