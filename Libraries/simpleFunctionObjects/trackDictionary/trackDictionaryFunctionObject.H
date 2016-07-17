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

Class
    trackDictionaryFunctionObject

Description
    This functionObject is tracking the modifications of a list of OpenFOAM
    dictionaries throughout the duration of a simulation.

    The list of dictionaries to track is supplied as a parameter to the
    functionObject.

    At the start of the simulation, the content of all the specified
    dictionaries will be read and printed out to the console, including
    the list of all the switches values known to the application.
    The echo of the DebugSwitches, InfoSwitches, OptimisationSwitches,
    Tolerances and DimensionedConstants values is enabled through
    boolean parameters.

    If the parameter "runTimeModifiable" is set to "yes" in the case
    system/controlDict file, then this list of dictionaries will also
    be checked at every time step in order to detect possible file modifications.
    If so, their content will be dumped to screen again.

    This feature allows to keep a log of all the selected case dictionary
    parameters throughout the simulation run. Using this information,
    one can keep track of all the modifications that occured during
    a given run, or between successive runs.

    Here is an example of such a functionObject declaration that needs to
    be added to your case controlDict:

///////////////////////////////////////////////////////////////////////////////
// Track the content of a given list of dictionaries for modification
// throughout a simulation run, and print them to the console when
// modified.
//
// The list of all known switches (DebugSwitches, etc) defined in the
// main controlDict and by the application and libraries is also echoed
// at the beginning of the run depending on the values of boolean flags in
// the functionObject dictionary definition.
//
// It is also possible to specify optional user-defined start and end
// of section separators in order to facilitate the recovery of the
// information  at runtime or during the postProcessing of the logged
// information
//
functions
(
    trackDictionaryContent
    {
        type trackDictionary;

        // Where to load it from (if not already in solver)
        functionObjectLibs ("libsimpleFunctionObjects.so");

        // Names of dictionaries to track.
        dictionaryNameList
        (
            "system/controlDict"
            "system/fvSchemes"
            "system/fvSolution"
            "constant/transportProperties"
            "constant/RASProperties"
        );

        // Boolean flags for enabling/disabling the echo of the
        // main controlDict switches
        echoControlDictDebugSwitches        false;
        echoControlDictInfoSwitches         true;
        echoControlDictOptimisationSwitches true;
        echoControlDictTolerances           true;
        echoControlDictDimensionedConstants true;

        // Section separators (optional)
        // If the string "_sectionIdToken_" explicitely appears in the
        // specification of the following section separators, this token
        // string will be replaced by the name of the dictionary beeing
        // dumped to the console, plus the file modification date and time.
        sectionStartSeparator "############ Start of: _sectionIdToken_ ############";
        sectionEndSeparator   "############ End of: _sectionIdToken_ ############";
    }
);
///////////////////////////////////////////////////////////////////////////////

SourceFiles
    trackDictionaryFunctionObject.C

Contributors/Copyright:
    2008-2009 Martin Beaudoin, Hydro-Quebec (beaudoin.martin@ireq.ca)
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef trackDictionaryFunctionObject_H
#define trackDictionaryFunctionObject_H

#include "simpleFunctionObject.H"
#include "dictionary.H"
#include "fvMesh.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
            Class trackDictionaryFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class trackDictionaryFunctionObject
:
    public simpleFunctionObject
{
    // Private data

    //- Reference to main object registry
    const Time& time_;

    //- List of dictionary to track
    stringList dictionaryNames_;

    PtrList<IOdictionary> dictionaries_;

    //- Echo of main controlDict switches
    bool echoControlDictDebugSwitches_;
    bool echoControlDictInfoSwitches_;
    bool echoControlDictOptimisationSwitches_;
    bool echoControlDictTolerances_;
    bool echoControlDictDimensionedConstants_;

    //- Section separator strings
    string sectionStartSeparator_;
    string sectionEndSeparator_;


    // Private Member Functions

    //- Disallow default bitwise copy construct
    trackDictionaryFunctionObject
    (
        const trackDictionaryFunctionObject&
    );

    //- Disallow default bitwise assignment
    void operator=(const trackDictionaryFunctionObject&);

    //- Generate start and end section separators
    //- If the token string "_sectionIdToken_" is present in
    //  the string separator, then the string "sectionId" will
    //  replace this token in the output
    string echoSectionSeparator(
        const string& separatorString,
        const string& sectionId
    );

    string echoStartSectionSeparator(const string& sectionId)
        {
            return echoSectionSeparator(
                sectionStartSeparator_,
                sectionId);
        }

    string echoEndSectionSeparator(const string& sectionId)
        {
            return echoSectionSeparator(
                sectionEndSeparator_,
                sectionId);
        }

    //- Write all the switches information
    void echoSwitchesValues();

    //- Write all the disctionary information
    void echoDictionaryValues(const label directoryIndex);

    //- Initialize the list of IOdictionary objects
    void initializeDictionaryList();


public:

    //- Runtime type information
    TypeName("trackDictionary");


    // Constructors

    //- Construct from components
    trackDictionaryFunctionObject
    (
     const word&,
        const Time&,
        const dictionary&
    );

    ~trackDictionaryFunctionObject();

    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute(const bool forceWrite);

    //- Read and set the function object if its data has changed
    virtual bool read(const dictionary& dict);

    //- Sample and write
    virtual void write();

    //- Optional stuff to do after write
    virtual void flush();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
