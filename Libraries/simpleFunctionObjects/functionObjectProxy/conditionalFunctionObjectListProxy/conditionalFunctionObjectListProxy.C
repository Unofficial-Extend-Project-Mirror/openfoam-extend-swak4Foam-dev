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
    2011, 2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "conditionalFunctionObjectListProxy.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(conditionalFunctionObjectListProxy, 0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

conditionalFunctionObjectListProxy::conditionalFunctionObjectListProxy
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObjectListProxy(
        name,
        t,
        dict,
        dict.lookupOrDefault<bool>("constructUnconditionally",false)
    )
{
    Dbug << "constructing " << name << endl;

    if(!dict.found("constructUnconditionally")) {
        WarningIn("conditionalFunctionObjectListProxy::conditionalFunctionObjectListProxy")
            << "'constructUnconditionally' not set in " << dict.name()
                << ". Assuming 'no'. If you want "
                << "'functions' to be constructed at the start then set to 'yes'"
                << endl;
    }

    if(dict.found("else")) {
        Dbug << "'else' found" << endl;

        readElse(dict);
    }
}

void conditionalFunctionObjectListProxy::readElse(const dictionary &dict)
{
    Dbug << "conditionalFunctionObjectListProxy::readElse" << endl;

    elseDict_.set(
        new dictionary(dict.subDict("else"))
    );
    if(!elseDict_->found("functions")) {
        FatalErrorIn("conditionalFunctionObjectListProxy::readElse(const dictionary &dict)")
            << "Dictionary " << dict.name() << " has an 'else'-entry but that "
                << "has no 'functions'-entry"
                << endl
                << exit(FatalError);
    }

    // get missing entries from the parent
    elseDict_()|=dict;
    elseDict_().remove("else");

    if(debug) {
        Info << elseDict_() << endl;
    }

    else_.set(
        new functionObjectListProxy(
            name()+"_else",
            time(),
            elseDict_(),
            false
        )
    );
    if(debug) {
        Info << "conditionalFunctionObjectListProxy::readElse finished" << endl;
    }
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool conditionalFunctionObjectListProxy::execute(const bool forceWrite)
{
    if(writeDebug() || debug) {
        Info << this->name() << " condition::execute() condition: "
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::execute(forceWrite);
    } else {
        if(else_.valid()) {
            return else_->execute(forceWrite);
        } else {
            return true;
        }
    }
}

bool conditionalFunctionObjectListProxy::start()
{
    if(writeDebug() || debug) {
        Info << this->name() << " condition::start() condition: "
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::start();
    } else {
        if(else_.valid()) {
            return else_->start();
        } else {
            return true;
        }
    }
}

bool conditionalFunctionObjectListProxy::end()
{
    if(writeDebug() || debug) {
        Info << this->name() << " condition::end() condition: "
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::end();
    } else {
        if(else_.valid()) {
            return else_->end();
        } else {
            return true;
        }
    }
}

bool conditionalFunctionObjectListProxy::read(const dictionary& dict)
{
    if(writeDebug() || debug) {
        Info << this->name() << " condition::read() condition: "
            << condition() << endl;
    }

    if(condition()) {
        return functionObjectListProxy::read(dict);
    } else {
        if(else_.valid()) {
            return else_->read(dict);
        } else {
            return true;
        }
    }
}

} // namespace Foam

// ************************************************************************* //
