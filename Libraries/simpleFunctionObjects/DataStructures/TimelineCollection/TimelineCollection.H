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
    This file is based on OpenFOAM.

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

Class
    TimelineCollection

Description
    Collects a HashTable of Timelines.

    If no timeline is currently present then one is added with a header according
    to the specification

SourceFiles
    TimelineCollection.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef TimelineCollection_H
#define TimelineCollection_H

#include "objectRegistry.H"
#include "swakTime.H"

#include "OFstream.H"
#include "HashPtrTable.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class TimelineCollection Declaration
\*---------------------------------------------------------------------------*/

class TimelineCollection
{
    // Private data

    //- the output-directory
    fileName outputDirectory_;

    //- the time object
    const Time &time_;

    //- header specifications
    dictionary headerSpecs_;

    //- Output file pointers
    HashPtrTable<OFstream> outputFilePtr_;

    // Private Member Functions

    //- Disallow default bitwise copy construct
    TimelineCollection(const TimelineCollection&);

    //- Disallow default bitwise assignment
    void operator=(const TimelineCollection&);

    //- Default time name
    word timeName_;

public:

    // Static data members

    TypeName("TimelineCollection");

    // Constructors

    //- Construct null
    TimelineCollection(
        const fileName &dir,
        const Time &time,
        bool useStartTime=true
    );

    //- Destructor
    virtual ~TimelineCollection();

    void addSpec(
        const string &nameExpr,
        const string &headerSpecString
    );

    OFstream &operator()(
        const word &name,
        bool autoParallel=true
    );

    OFstream &operator[](
        const word &name
    ) {
        return (*this)(name,false);
    }

    // Member Functions

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
