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

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2017 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "TimelineCollection.H"
#include "DebugOStream.H"

namespace Foam {
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(TimelineCollection, 0);

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

TimelineCollection::TimelineCollection(
    const fileName &dir,
    const Time &time,
    bool useStartTime
):
    outputDirectory_(dir),
    time_(time),
    timeName_("")
{
    Dbug << "Construction" << endl;
    if(useStartTime) {
        timeName_=time.timeName();
    }
}


// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

TimelineCollection::~TimelineCollection()
{
    Dbug << "Destruction" << endl;
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void TimelineCollection::addSpec(
    const string &nameExpr,
    const string &headerSpecString
) {
    headerSpecs_.set(keyType(nameExpr),headerSpecString);
}

// * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * * //

OFstream &TimelineCollection::operator()(
    const word &name,
    bool autoParallel
) {
    word timeName=timeName_;
    if(timeName=="") {
        timeName=time_.timeName();
    }
    word usedName=name;
    if(
        Pstream::parRun()
        &&
        autoParallel
    ) {
        usedName += "Proc" + Foam::name(Pstream::myProcNo());
    }
    if(!outputFilePtr_.found(usedName)) {
        Dbug << "File name" << usedName << "not in table" << endl;
        mkDir(outputDirectory_/timeName);
        outputFilePtr_.set
        (
            usedName,
            new OFstream(outputDirectory_/timeName/usedName)
        );
        //        Pout << headerSpecs_ << endl;
        OFstream &o=*outputFilePtr_[usedName];
        o << "# Time \t " << string(headerSpecs_[usedName]).c_str() << endl;
    }
    *outputFilePtr_[usedName] << time_.value() << tab;
    return *outputFilePtr_[usedName];
}


// * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * Friend Operators * * * * * * * * * * * * * * //

} // end namespace

// ************************************************************************* //
