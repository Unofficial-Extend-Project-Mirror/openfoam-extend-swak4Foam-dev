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
    createSampledSet

Description
    Function object creates a sampledSet and puts it into a repository
    to be used by others

SourceFiles
    createSampledSet.C

Contributors/Copyright:
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef createSampledSet_H
#define createSampledSet_H

#include "primitiveFieldsFwd.H"
#include "pointFieldFwd.H"

#include "swak.H"

namespace Foam
{
// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;
class polyMesh;

class createSampledSet
{
    //- Disallow default bitwise copy construct
    createSampledSet(const createSampledSet&);

    //- Disallow default bitwise assignment
    void operator=(const createSampledSet&);

protected:

    //- are we working?
    bool active_;

    //- mesh where the set goes
    const objectRegistry& obr_;

public:

    //- Runtime type information
    TypeName("createSampledSet");


    // Constructors

    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    createSampledSet
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~createSampledSet();


    // Member Functions

    //- Read the forces data
    virtual void read(const dictionary&);

    //- Execute, currently does nothing
    virtual void execute();

    //- Execute at the final time-loop, currently does nothing
    virtual void end();

    //- Write the forces
    virtual void write();

    //- Update for changes of mesh
    virtual void updateMesh(const mapPolyMesh&)
        {}

    //- Update for changes of mesh
#ifdef FOAM_MOVEPOINTS_GETS_POLYMESH
    virtual void movePoints(const polyMesh&)
#else
    virtual void movePoints(const pointField&)
#endif
        {}

    //- Clear all data
    void clearData();
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
