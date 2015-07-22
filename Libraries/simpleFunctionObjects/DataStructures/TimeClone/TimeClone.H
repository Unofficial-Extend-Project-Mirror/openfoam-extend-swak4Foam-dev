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
    TimeClone

Description
    Object that on request creates a full copy of all fields etc and writes them on
    request

    Only copies objectRegistries or objects that are auto-writeable

SourceFiles
    TimeClone.C

Contributors/Copyright:
    2014-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef TimeClone_H
#define TimeClone_H

#include "objectRegistry.H"
#include "Time.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class TimeClone Declaration
\*---------------------------------------------------------------------------*/

class TimeClone
{
    // Private data

    //- the actual time-thing
    autoPtr<Time> storedTime_;


    // Private Member Functions

    //- Disallow default bitwise copy construct
    TimeClone(const TimeClone&);

    //- Disallow default bitwise assignment
    void operator=(const TimeClone&);

    //- Do the actual copying
    label copyObjects(const objectRegistry &src,objectRegistry &dst);

public:

    // Static data members

    TypeName("TimeClone");

    // Constructors

    //- Construct null
    TimeClone();

    // Selectors

    //- Make a copy
    void copy(const Time &);

    //- Write the time-directory
    bool write(const bool force=false);

    //- Destructor
    virtual ~TimeClone();

    bool ok() const {
        return storedTime_.valid();
    }

    const Time &operator()() const;

    // Member Functions

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
