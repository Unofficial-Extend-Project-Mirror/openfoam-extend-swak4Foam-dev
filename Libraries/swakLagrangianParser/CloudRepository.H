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
    Foam::CloudsRepository

Description
    Store clouds that nobody else wants

SourceFiles
    CloudRepository.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudRepository_H
#define CloudRepository_H

#include "PtrMap.H"
#include "cloud.H"
#include "RepositoryBase.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class ReaderParticleCloud;

/*---------------------------------------------------------------------------*\
                           Class CloudRepository Declaration
\*---------------------------------------------------------------------------*/

class CloudRepository
:
    public RepositoryBase
{
    // Private data

    //- the actual sets
    HashPtrTable<cloud> clouds_;

    //- Clouds that can be updated
    HashPtrTable<ReaderParticleCloud> updateableClouds_;

   // Constructors

    //- Construct null
    CloudRepository(const IOobject &);

    //- The only instance of this repository
    static CloudRepository *repositoryInstance;

public:
     //- Runtime type information
    TypeName("CloudRepository");

    //- Destructor

    virtual ~CloudRepository();

    //- Get the Singleton of this class
    static CloudRepository &getRepository(const objectRegistry &mesh);

    void addCloud(
        autoPtr<cloud> c
    );

    void addUpdateableCloud(
        autoPtr<ReaderParticleCloud> c
    );

    //- Make sure that this class is not abstract. Write sets
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
