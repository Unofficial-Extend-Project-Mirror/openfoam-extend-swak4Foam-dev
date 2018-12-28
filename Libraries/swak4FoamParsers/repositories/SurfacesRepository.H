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
    Foam::SurfacesRepository

Description

SourceFiles
    SurfacesRepositoryI.H
    SurfacesRepository.C
    SurfacesRepositoryIO.C

Contributors/Copyright:
    2012-2013, 2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SurfacesRepository_H
#define SurfacesRepository_H

#include "PtrMap.H"
#include "sampledSurface.H"
#include "RepositoryBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SurfacesRepository Declaration
\*---------------------------------------------------------------------------*/

class SurfacesRepository
:
    public RepositoryBase
{
    // Private data

    //- the actual surfaces
    HashPtrTable<sampledSurface> surfaces_;

    //- names of the formats to be written
    HashTable<word> formatNames_;

    // Constructors

    //- Construct null
    SurfacesRepository(const IOobject &);

    //- The only instance of this repository
    static SurfacesRepository *repositoryInstance;

public:
     //- Runtime type information
    TypeName("SurfacesRepository");

    //- Destructor

    virtual ~SurfacesRepository();

    //- Get the Singleton of this class
    static SurfacesRepository &getRepository(const objectRegistry &mesh);

    //- get a surface using a dictionary (surface will be constructed if not existing)
    sampledSurface &getSurface(
        const dictionary &dict,
        const fvMesh &mesh
    );

    //- get a surface using its name (surface must exist)
    sampledSurface &getSurface(
        const word &name,
        const fvMesh &mesh
    );

    //- force an update of a surface
    bool updateSurface(
        const word &name,
        const fvMesh &mesh
    );

    //- Make sure that this class is not abstract
    bool writeData(Ostream &) const;

    //- update repo at the start of a time-step
    void updateRepo();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
