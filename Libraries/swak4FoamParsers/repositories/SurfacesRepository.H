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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::SurfacesRepository

Description

SourceFiles
    SurfacesRepositoryI.H
    SurfacesRepository.C
    SurfacesRepositoryIO.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SurfacesRepository_H
#define SurfacesRepository_H

#include "PtrMap.H"
#include "sampledSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SurfacesRepository Declaration
\*---------------------------------------------------------------------------*/

class SurfacesRepository
:
    public regIOobject
{
    // Private data

    //- the actual surfaces
    HashPtrTable<sampledSurface,word> surfaces_;

    //- names of the formats to be written
    HashTable<word,word> formatNames_;

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

    //- Make sure that this class is not abstract
    bool writeData(Ostream &) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
