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
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
        usedName+="Proc"+name(Pstream::myProcNo());
    }
    if(!outputFilePtr_.found(usedName)) {
        Dbug << "File name" << usedName << "not in table" << endl;
        mkDir(outputDirectory_/timeName);
        outputFilePtr_.insert(
            usedName,
            new OFstream(
                outputDirectory_/timeName/usedName
            )
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
