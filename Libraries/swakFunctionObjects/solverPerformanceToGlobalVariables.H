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
    solverPerformanceToGlobalVariables

Description
    Function object that gets the solverPerformances of specified
    solvers and stores them in global variables

SourceFiles
    solverPerformanceToGlobalVariables.C

Contributors/Copyright:
    2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef solverPerformanceToGlobalVariables_H
#define solverPerformanceToGlobalVariables_H

#include "primitiveFieldsFwd.H"
#include "pointFieldFwd.H"

#include "swak.H"

namespace Foam
{
// Forward declaration of classes
class dictionary;
class objectRegistry;
class mapPolyMesh;
class polyMesh;

class solverPerformanceToGlobalVariables
{
    //- Disallow default bitwise copy construct
    solverPerformanceToGlobalVariables(const solverPerformanceToGlobalVariables&);

    //- Disallow default bitwise assignment
    void operator=(const solverPerformanceToGlobalVariables&);

    const objectRegistry& obr_;

    //- names of the Fields for which the values are added
    wordList fieldNames_;

    //- the global namespace the variables should go to
    word toGlobalNamespace_;

    //- whether or not to reset the variable
    bool noReset_;

protected:

    void addFieldToData(const word &name);

    void setValue(const word &name,scalar value);

    void setValue(const word &name,const scalarField &value);

    void executeAndWriteToGlobal();

public:

    //- Runtime type information
    TypeName("solverPerformanceToGlobalVariables");


    // Constructors

    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    solverPerformanceToGlobalVariables
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~solverPerformanceToGlobalVariables();


    // Member Functions

    //- Called when time was set at the end of the Time::operator++
    virtual void timeSet();

    //- Read the forces data
    virtual void read(const dictionary&);

    //- Execute, currently does nothing
    virtual void execute();

    //- Execute at the final time-loop, currently does nothing
    virtual void end();

    //- Write the forces
#ifdef FOAM_IOFILTER_WRITE_NEEDS_BOOL
    virtual bool write();
#else
    virtual void write();
#endif


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
