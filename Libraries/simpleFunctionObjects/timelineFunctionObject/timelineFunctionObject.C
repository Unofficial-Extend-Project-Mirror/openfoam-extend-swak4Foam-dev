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
    2008-2013, 2015-2016, 2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2019 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id: timelineFunctionObject.C,v 93dddbfe713a 2019-03-11 11:50:39Z Mark $
\*---------------------------------------------------------------------------*/

#include "timelineFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "OFstream.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(timelineFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
const Enum<Foam::timelineFunctionObject::outputFileMode>
timelineFunctionObject::outputFileModeNames_
({
    {outputFileMode::ofmFoam,"foam"},
    {outputFileMode::ofmRaw,"raw"},
    {outputFileMode::ofmCsv,"csv"}
});
#else
template<>
const char* NamedEnum<Foam::timelineFunctionObject::outputFileMode,3>::names[]=
{
    "foam",
    "raw",
    "csv"
};
const NamedEnum<timelineFunctionObject::outputFileMode,3> timelineFunctionObject::outputFileModeNames_;
#endif

timelineFunctionObject::timelineFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict,
    bool writeStartTime
)
:
    simpleDataFunctionObject(name,t,dict),
    factor_(
        dict.lookupOrDefault<scalar>("factor",1)
    ),
    outputFileMode_(
        outputFileModeNames_[dict.lookupOrDefault<word>("outputFileMode","foam")]
    ),
    writeStartTime_(
        dict.lookupOrDefault<bool>(
            "writeStartTime",
            writeStartTime
        )
    )
{
    Dbug << name << " - constructor" << endl;

    if(!dict.found("writeStartTime")) {
        WarningIn("timelineFunctionObject::timelineFunctionObject")
            << "No entry 'writeStartTime' in " << dict.name()
                << ". Assuming " << (writeStartTime ? "'yes'" : "'no'")
                << endl;
    }

    switch(outputFileMode_) {
        case ofmFoam:
            fileExtension_="";
            separator_=" ";
            break;
        case ofmRaw:
            fileExtension_=".raw";
            separator_=" ";
            break;
        case ofmCsv:
            fileExtension_=".csv";
            separator_=",";
            break;
        default:
            fileExtension_=".unknownFileMode";
            separator_="?";
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

void timelineFunctionObject::flush()
{
    Dbug << " flush " << name() << endl;

    forAllIter(HashPtrTable<OFstream>, filePtrs_, iter)
    {
        (*iter()).flush();
    }
}

void timelineFunctionObject::closeAllFiles()
{
    Dbug << " closeAllFiles " << name() << endl;

    forAllIter(HashPtrTable<OFstream>, filePtrs_, iter)
    {
#ifdef FOAM_HASH_PTR_TABLE_ACCEPTS_NO_RAW_POINTERS
        filePtrs_.remove(iter);
#else
        delete filePtrs_.remove(iter);
#endif
    }
}

bool timelineFunctionObject::start()
{
    Dbug << name() << ": start() - entering" << endl;

    if (Pstream::master())
    {
        wordList names(fileNames());

        // Check if any fieldNames have been removed. If so close
        // the file.

        forAllIter(HashPtrTable<OFstream>, filePtrs_, iter)
        {
            if (findIndex(names, iter.key()) == -1)
            {
                Dbug << "Closing file " << iter.key() << endl;

                // Field has been removed. Close file
#ifdef FOAM_HASH_PTR_TABLE_ACCEPTS_NO_RAW_POINTERS
                filePtrs_.remove(iter);
#else
                delete filePtrs_.remove(iter);
#endif
            }
        }

        // Open new files for new fields. Keep existing files.

        filePtrs_.resize(2*names.size());

        forAll(names,fileI)
        {
            const word& fldName = names[fileI];
            Dbug << "Checking " << fldName << endl;

            // Check if added field. If so open a stream for it.

            if (!filePtrs_.found(fldName))
            {
                Dbug << "Creating " << fldName << endl;

                fileName theDir=dataDir();
                if(!exists(theDir)) {
                    Dbug << "Creating directory " << theDir << endl;
                    mkDir(theDir);
                }
                OFstream* sPtr = new OFstream(theDir/fldName+fileExtension_);

                filePtrs_.set(fldName, sPtr);

                OFstream &s=*sPtr;

                if(
                    outputFileMode_==ofmRaw
                    ||
                    outputFileMode_==ofmFoam
                ) {
                    s << '#';
                }

                writeTime(fldName,word("Time"));
                writeData(fldName,this->columnNames());

                endData(fldName);
            }
        }
    }

    simpleDataFunctionObject::start();

    if(writeStartTime_) {
        writeSimple();
    }

    return true;
}

void timelineFunctionObject::endData(const word &name) {
    Dbug << " endData " << name << endl;

    (*filePtrs_[name]) << endl;
}


} // namespace Foam

// ************************************************************************* //
