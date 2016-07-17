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
    Foam::readAndUpdateFields

Description
    Reads fields at startup and updates the boundary conditions at every
    timestep. Also makes sure that the fields are automatically written

SourceFiles
    readAndUpdateFields.C

Contributors/Copyright:
    2012-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef readAndUpdateFields_H
#define readAndUpdateFields_H

#include "OFstream.H"
#include "pointField.H"
#include "volFields.H"
#include "surfaceFields.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;
class polyMesh;

/*---------------------------------------------------------------------------*\
                         Class readAndUpdateFields Declaration
\*---------------------------------------------------------------------------*/

class readAndUpdateFields
{
protected:

    // Protected data

        //- Name of this set of readAndUpdateFields object
        word name_;

        const objectRegistry& obr_;

        //- on/off switch
        bool active_;

        //- Fields to load
        wordList fieldSet_;

        //- Loaded fields
        PtrList<volScalarField> vsf_;
        PtrList<volVectorField> vvf_;
        PtrList<volSphericalTensorField> vSpheretf_;
        PtrList<volSymmTensorField> vSymmtf_;
        PtrList<volTensorField> vtf_;

        PtrList<pointScalarField> psf_;
        PtrList<pointVectorField> pvf_;
        PtrList<pointSphericalTensorField> pSpheretf_;
        PtrList<pointSymmTensorField> pSymmtf_;
        PtrList<pointTensorField> ptf_;

        autoPtr<pointMesh> pMesh_;

        const pointMesh &pMesh(const polyMesh &mesh);

    // Protected Member Functions

        //- Disallow default bitwise copy construct
        readAndUpdateFields(const readAndUpdateFields&);

        //- Disallow default bitwise assignment
        void operator=(const readAndUpdateFields&);

        template<class Type>
        bool loadField
        (
            const word&,
            PtrList<GeometricField<Type, fvPatchField, volMesh> >&,
            PtrList<GeometricField<Type, pointPatchField, pointMesh> >&
        );

        template <class FType>
        void correctBoundaryConditions(
            PtrList<FType> &
        );
public:

    //- Runtime type information
    TypeName("readAndUpdateFields");


    // Constructors

        //- Construct for given objectRegistry and dictionary.
        //  Allow the possibility to load fields from files
        readAndUpdateFields
        (
            const word& name,
            const objectRegistry&,
            const dictionary&,
            const bool loadFromFiles = false
        );


    //- Destructor
    virtual ~readAndUpdateFields();


    // Member Functions

        //- Return name of the readAndUpdateFields object
        virtual const word& name() const
        {
            return name_;
        }

        //- Called when time was set at the end of the Time::operator++ 
        virtual void timeSet();         

        //- Read the field min/max data
        virtual void read(const dictionary&);

        //- Execute, currently does nothing
        virtual void execute();

        //- Execute at the final time-loop, currently does nothing
        virtual void end();

        //- Write
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
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "readAndUpdateFieldsTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
