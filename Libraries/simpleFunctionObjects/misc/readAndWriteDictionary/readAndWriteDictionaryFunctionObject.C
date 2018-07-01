/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "readAndWriteDictionaryFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(readAndWriteDictionaryFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        readAndWriteDictionaryFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

readAndWriteDictionaryFunctionObject::readAndWriteDictionaryFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    inConstant_(
        readBool(
            dict.lookup("inConstant")
        )
    ),
    inSystem_(
        readBool(
            dict.lookup("inSystem")
        )
    ),
    mustExist_(
        (inConstant_ || inSystem_)
        ?
        true
        :
        readBool(
            dict.lookup("mustExist")
        )
    ),
    autowrite_(
        (inConstant_ || inSystem_)
        ?
        false
        :
        readBool(
            dict.lookup("autowrite")
        )
    ),
    dictName_(
        dict.lookup("dictName")
    ),
    localName_(
        dict.lookupOrDefault<word>(
            "localName",
            (inConstant_ || inSystem_)
            ? ""
            : "swak4Foam"
        )
    )
{
    if(inConstant_ && inSystem_) {
        FatalErrorIn("readAndWriteDictionaryFunctionObject::readAndWriteDictionaryFunctionObject")
            << "Options 'inSystem' and 'inConstant' both set in " << dict.name()
                << " although they are mutual exclusive"
                << endl
                << exit(FatalError);
    }
    if(!dict.found("localName")) {
        WarningIn("readAndWriteDictionaryFunctionObject::readAndWriteDictionaryFunctionObject")
            << "In " << dict.name() << " no entry 'localName' defined. "
                << "Assuming 'swak4Foam'"
                << endl;
    }

    dict_.set(
        new IOdictionary(
            IOobject(
                dictName_,
                (
                    inConstant_
                    ? "constant"
                    : (
                        inSystem_
                        ? "system"
                        :obr_.time().timeName()
                    )
                ),
                localName_,
                obr_,
                mustExist_ ? IOobject::MUST_READ : IOobject::READ_IF_PRESENT,
                autowrite_ ? IOobject::AUTO_WRITE : IOobject::NO_WRITE
            )
        )
    );

    Info << "Read dictionary " << dict_->path() << endl;

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    this->start();
#endif
}

readAndWriteDictionaryFunctionObject::~readAndWriteDictionaryFunctionObject()
{
    IOdictionary *d=dict_.ptr();  // this is a 'memory hole'
    //delete d;  // for some reason this gives an error message
    // not freeing the memory avoids the error message
}

bool readAndWriteDictionaryFunctionObject::start()
{
    return simpleFunctionObject::start();
}

void readAndWriteDictionaryFunctionObject::writeSimple()
{
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
