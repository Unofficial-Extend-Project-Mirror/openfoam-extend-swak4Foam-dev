//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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
    solvePDECommon

Description
    Common functionality for functionObjects that solve PDEs

SourceFiles
    solvePDECommon.C

\*---------------------------------------------------------------------------*/

#ifndef solvePDECommon_H
#define solvePDECommon_H

#include "primitiveFieldsFwd.H"
#include "pointFieldFwd.H"
#include "NamedEnum.H"
#include "volFields.H"

namespace Foam
{
// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;
class FieldValueExpressionDriver;

class solvePDECommon
{
    //- Disallow default bitwise copy construct
    solvePDECommon(const solvePDECommon&);
    
    //- Disallow default bitwise assignment
    void operator=(const solvePDECommon&);

public:
    enum solveAt {
        saStartup,
        saTimestep,
        saWrite
    };
protected:

    //- are we working?
    bool active_;

    //- mesh where the set goes
    const objectRegistry& obr_;

    //- name of the field that is being solved for 
    word fieldName_;

    //- the actual field
    autoPtr<volScalarField> theField_;

    //- the driver that calculates the coefficients
    autoPtr<FieldValueExpressionDriver> driver_;

    //- is this a steady calculation
    bool steady_;

    //- do the actual solving
    virtual void solve() = 0;

    static const NamedEnum<solveAt,3> solveAtNames_;
    solveAt solveAt_;

public:

    //- Runtime type information
    TypeName("solvePDECommon");


    // Constructors
    
    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    solvePDECommon
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~solvePDECommon();


    // Member Functions

    //- Read the forces data
    virtual void read(const dictionary&);
    
    //- Execute
    virtual void execute();
    
    //- Execute at the final time-loop
    virtual void end();
    
    //- Write the forces
    virtual void write();

    //- Update for changes of mesh
    virtual void updateMesh(const mapPolyMesh&)
        {}
    
    //- Update for changes of mesh
    virtual void movePoints(const pointField&)
        {}

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
