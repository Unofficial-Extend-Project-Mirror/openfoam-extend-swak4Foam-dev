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

Class
    executeIfSetOrZoneFunctionObject

Description
    This proxy-functionObject only executes its childrens if the sets and zones
    listed in a dictionary exist

SourceFiles
    executeIfSetOrZoneFunctionObject.C

Contributors/Copyright:
    2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef executeIfSetOrZoneFunctionObject_H
#define executeIfSetOrZoneFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class polyMesh;

/*---------------------------------------------------------------------------*\
                           Class executeIfSetOrZoneFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class executeIfSetOrZoneFunctionObject
:
    public conditionalFunctionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeIfSetOrZoneFunctionObject(const executeIfSetOrZoneFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const executeIfSetOrZoneFunctionObject&);

    //- the dictionary with the sets and zones
    dictionary setsAndZones_;

    //- reread the regular expression
    void readSetsAndZones(const dictionary& dict);

    const polyMesh &mesh_;

    // check if set is there and load if missing
    template<class T>
    bool hasSet(const word &name);

    bool loadAndCacheMissingSets_;

protected:

    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("executeIfSetOrZone");


    // Constructors

    //- Construct from components
    executeIfSetOrZoneFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary& dict);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
