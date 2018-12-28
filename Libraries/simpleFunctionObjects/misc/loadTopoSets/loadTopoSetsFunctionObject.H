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

Class
    loadTopoSetsFunctionObject

Description
    Load topoSets like cellSets and faceSets in the beginning and register them
    with the mesh

SourceFiles
    loadTopoSetsFunctionObject.C

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef loadTopoSetsFunctionObject_H
#define loadTopoSetsFunctionObject_H

#include "simpleFunctionObject.H"

#include "HashSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class loadTopoSetsFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class loadTopoSetsFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    loadTopoSetsFunctionObject(const loadTopoSetsFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const loadTopoSetsFunctionObject&);

    //- load the sets
    void loadSets();

    //- load sets of a specific type
    template<class TopoSetType>
    void loadAllSets(wordHashSet &names);

    //- write sets of a specific type
    template<class TopoSetType>
    void writeAllSets(wordHashSet &names);

    //- Load sets even if they are already in memory
    bool forceLoading_;

    //- load cellSets
    bool loadCellSets_;

    //- load faceSets
    bool loadFaceSets_;

    //- load pointSets
    bool loadPointSets_;

    //- should we write?
    bool writeSets_;

    wordHashSet cellSetNames_;
    wordHashSet faceSetNames_;
    wordHashSet pointSetNames_;

public:

    //- Runtime type information
    TypeName("loadTopoSets");


    // Constructors

    //- Construct from components
    loadTopoSetsFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
