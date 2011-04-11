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
    expressionFaField

Description
    Function object that generates a faField

SourceFiles
    expressionFaField.C

\*---------------------------------------------------------------------------*/

#ifndef expressionFaField_H
#define expressionFaField_H

#include "primitiveFieldsFwd.H"
#include "areaFieldsFwd.H"
#include "pointFieldFwd.H"
#include "Switch.H"
#include "autoPtr.H"

namespace Foam
{
// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;

class FaFieldValueExpressionDriver;

class expressionFaField
{
    //- Disallow default bitwise copy construct
    expressionFaField(const expressionFaField&);
    
    //- Disallow default bitwise assignment
    void operator=(const expressionFaField&);

protected:

    // Private data

    //- are we working?
    bool active_;

    //- Name of the field
    word name_;

    //- Expression
    string expression_;

    //- should the file be autowrite
    Switch autowrite_;

    const objectRegistry& obr_;

    const dictionary &dict_;

    autoPtr<areaVectorField> vectorField_;

    autoPtr<areaScalarField> scalarField_;

    template<class T>
    void storeField(
        const T &data,
        autoPtr<T> &store
    );

    autoPtr<FaFieldValueExpressionDriver> driver_;

public:

    //- Runtime type information
    TypeName("expressionFaField");


    // Constructors
    
    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    expressionFaField
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~expressionFaField();


    // Member Functions

    //- Return name of the set of forces
    virtual const word& name() const
        {
            return name_;
        }

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
    virtual void movePoints(const pointField&)
        {}

    //- Clear all data
    void clearData();
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
