/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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
    2008-2009, 2012 Martin Beaudoin, Hydro-Quebec (beaudoin.martin@ireq.ca)
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*----------------------------------------------------------------------------*/

#include "trackDictionaryFunctionObject.H"
#include "addToRunTimeSelectionTable.H"
#include "dimensionedConstants.H"
#include "stringListOps.H"
#include "fileStat.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(trackDictionaryFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        trackDictionaryFunctionObject,
        dictionary
    );
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::trackDictionaryFunctionObject::
trackDictionaryFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name, t, dict),
    time_(t),
    dictionaryNames_(dict.lookup("dictionaryNameList")),
    dictionaries_(dictionaryNames_.size()),
    echoControlDictDebugSwitches_(true),
    echoControlDictInfoSwitches_(true),
    echoControlDictOptimisationSwitches_(true),
    echoControlDictTolerances_(true),
    echoControlDictDimensionedConstants_(true),
    sectionStartSeparator_("############ Start of: _sectionIdToken_ ############"),
    sectionEndSeparator_  ("############ End of: _sectionIdToken_ ############")
{
    // Boolean flags for echoing the main controlDict switches
    // DebugSwitches
    if(dict.found("echoControlDictDebugSwitches"))
    {
        dict.lookup("echoControlDictDebugSwitches") >> echoControlDictDebugSwitches_;
    }

    // InfoSwitches
    if(dict.found("echoControlDictInfoSwitches"))
    {
        dict.lookup("echoControlDictInfoSwitches") >> echoControlDictInfoSwitches_;
    }

    // OptimisationSwitches
    if(dict.found("echoControlDictOptimisationSwitches"))
    {
        dict.lookup("echoControlDictOptimisationSwitches") >> echoControlDictOptimisationSwitches_;
    }

    // Tolerances
    if(dict.found("echoControlDictTolerances"))
    {
        dict.lookup("echoControlDictTolerances") >> echoControlDictTolerances_;
    }

    // DimensionedConstants
    if(dict.found("echoControlDictDimensionedConstants"))
    {
        dict.lookup("echoControlDictDimensionedConstants") >> echoControlDictDimensionedConstants_;
    }

    // Optional parameter sectionStartSeparator
    if (dict.found("sectionStartSeparator"))
    {
        dict.lookup("sectionStartSeparator") >> sectionStartSeparator_;
    }

    // Optional parameter sectionEndSeparator
    if (dict.found("sectionEndSeparator"))
    {
        dict.lookup("sectionEndSeparator") >> sectionEndSeparator_;
    }
}

Foam::trackDictionaryFunctionObject::~trackDictionaryFunctionObject()
{
    // Clear the dictionary list
    dictionaries_.clear();
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool Foam::trackDictionaryFunctionObject::start()
{
    if(Pstream::master())
    {
        if(debug)
        {
            Pout << "trackDictionaryFunctionObject::start(): dictionaryNames_                    : " << dictionaryNames_                     << endl;
            Pout << "trackDictionaryFunctionObject::start(): echoControlDictDebugSwitches_       : " << echoControlDictDebugSwitches_        << endl;
            Pout << "trackDictionaryFunctionObject::start(): echoControlDictInfoSwitches_        : " << echoControlDictInfoSwitches_         << endl;
            Pout << "trackDictionaryFunctionObject::start(): echoControlDictOptimisationSwitches_: " << echoControlDictOptimisationSwitches_ << endl;
            Pout << "trackDictionaryFunctionObject::start(): echoControlDictTolerances_          : " << echoControlDictTolerances_           << endl;
            Pout << "trackDictionaryFunctionObject::start(): echoControlDictDimensionedConstants_: " << echoControlDictDimensionedConstants_ << endl;
            Pout << "trackDictionaryFunctionObject::start(): sectionStartSeparator_              : " << sectionStartSeparator_               << endl;
            Pout << "trackDictionaryFunctionObject::start(): sectionEndSeparator_                : " << sectionEndSeparator_                 << endl;
        }

        // Echo the main controlDict switches values
        echoSwitchesValues();
    }

    // Allocate the list of required dictionary objects.
    // The timestamp information from these object will help to detect
    // dictionary file modification.
    initializeDictionaryList();

    if(Pstream::master())
    {
        // Write the initial information
        Pout << endl << "Writing values from list of specified dictionaries :" << endl << endl;

        forAll(dictionaries_, dictI)
        {
            echoDictionaryValues(dictI);
        }

        Pout << endl;
    }
    return true;
}

//- Verify if any dictionaries have been modified
bool Foam::trackDictionaryFunctionObject::execute(const bool forceWrite)
{
    forAll(dictionaries_, dictI)
    {
        // Here, watchout for the OptimisationSwitch fileModificationSkew
        // The file is only considered modified after a "fileModificationSkew" delay
        // from the last time the file was read.
        // NB: readIfModified is calling reduce, so all parallel jobs need to run this
        if(
            dictionaries_.set(dictI) &&
            dictionaries_[dictI].readIfModified()
        )
        {
            echoDictionaryValues(dictI);
        }
    }

    return true;
}

void Foam::trackDictionaryFunctionObject::echoDictionaryValues(const label directoryIndex)
{
    if(Pstream::master())
    {
        if(dictionaries_.set(directoryIndex))
        {
            // Get the access time for this file, so we know how old it is.
            time_t tmStamp = lastModified(dictionaries_[directoryIndex].filePath());
            char   c_tmStamp[64];
            string s_tmStamp(ctime_r(&tmStamp, c_tmStamp));

            // Clean up the string
            s_tmStamp.removeTrailing('\n');

            string sectionId = dictionaryNames_[directoryIndex] + " (" + s_tmStamp + ")";

            Pout << echoStartSectionSeparator(sectionId) << endl;
            Pout << dictionaryNames_[directoryIndex] << " : " << dictionaries_[directoryIndex];
            Pout << echoEndSectionSeparator(sectionId) << endl;
            Pout << "#" << endl << endl;
        }
        else
        {
            string sectionId = dictionaryNames_[directoryIndex] ;

            Pout << echoStartSectionSeparator(sectionId) << endl;
            WarningIn
                (
                    "Foam::trackDictionaryFunctionObject::echoDictionaryValues()"
                )   << "--> This dictionary is not accessible: "
                    << dictionaryNames_[directoryIndex]
                    << endl;
            Pout << echoEndSectionSeparator(sectionId) << endl;
            Pout << "#" << endl << endl;
        }
    }
}

bool Foam::trackDictionaryFunctionObject::read(const dictionary& dict)
{
    dictionaryNames_ = stringList(dict.lookup("dictionaryNameList"));

    // Optional parameter sectionStartSeparator
    if (dict.found("sectionStartSeparator"))
    {
        dict.lookup("sectionStartSeparator") >> sectionStartSeparator_;
    }

    // Optional parameter sectionEndSeparator
    if (dict.found("sectionEndSeparator"))
    {
        dict.lookup("sectionEndSeparator") >> sectionEndSeparator_;
    }

    // Update list of dictionaries
    initializeDictionaryList();

    // Usually, this member function is called when the case controlDict file
    // is modified. So let's see if controlDict is in the list of
    // dictionaries of interest
    labelList matchPatternString = findStrings("system/controlDict", dictionaryNames_);

    // We are only interested in the first occurence
    if(matchPatternString.size() > 0)
    {
        echoDictionaryValues(matchPatternString[0]);
    }

    return false;
}

void Foam::trackDictionaryFunctionObject::echoSwitchesValues()
{
    Pout << endl << "Writing global switches values:" << endl;

    Pout << echoStartSectionSeparator("Global Switches") << endl;

    // We check the availablilty of every switchSet, just in case one would be missing
    if(echoControlDictDebugSwitches_ && debug::controlDict().found("DebugSwitches"))
        Pout << "DebugSwitches        : " << Foam::debug::debugSwitches();

    if(echoControlDictInfoSwitches_ && debug::controlDict().found("InfoSwitches"))
        Pout << "InfoSwitches         : " << Foam::debug::infoSwitches();

    if(echoControlDictOptimisationSwitches_ && debug::controlDict().found("OptimisationSwitches"))
        Pout << "OptimisationSwitches : " << Foam::debug::optimisationSwitches();

#ifdef FOAM_DEV
    if(echoControlDictTolerances_ && debug::controlDict().found("Tolerances"))
        Pout << "Tolerance switches   : " << Foam::debug::tolerances();
#endif

    if(echoControlDictDimensionedConstants_ && debug::controlDict().found("DimensionedConstants"))
        Pout << "DimensionedConstants : " << Foam::dimensionedConstants();

    Pout << echoEndSectionSeparator("Global Switches") << endl;

}

void Foam::trackDictionaryFunctionObject::initializeDictionaryList()
{
    // Resize dictionaries_
    dictionaries_.clear();
    dictionaries_.setSize(dictionaryNames_.size());

    forAll(dictionaries_, dictI)
    {
        fileName dictionaryName(dictionaryNames_[dictI]);

        if(Pstream::parRun())
        {
            dictionaryName = ".." / dictionaryName;
        }

        if(debug && Pstream::master())
        {
            Pout << "trackDictionaryFunctionObject::initializeDictionaryList: dictionaryName: " << dictionaryName << endl;
        }

        if(Foam::fileStat(dictionaryName).isValid())
        {
            dictionaries_.set
                (
                    dictI,
                    new IOdictionary
                    (
                        IOobject
                        (
                            dictionaryName.name(),
                            "",
                            dictionaryName.path(),
                            obr_,
                            IOobject::MUST_READ,
                            IOobject::NO_WRITE
                        )
                    )
                );
        }
    }
}

//- Return the start of section separator,
Foam::string Foam::trackDictionaryFunctionObject::echoSectionSeparator(
    const string& separatorString,
    const string& sectionId)
{
    string outputString(separatorString);

    return outputString.replace("_sectionIdToken_", sectionId);
}

void Foam::trackDictionaryFunctionObject::write()
{
    // Do nothing for now
}

void Foam::trackDictionaryFunctionObject::flush()
{
    // Do nothing for now
}

// ************************************************************************* //
