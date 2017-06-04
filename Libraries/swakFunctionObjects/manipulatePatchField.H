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
    manipulatePatchField

Description
    Function object that manipulates a patch field

SourceFiles
    manipulatePatchField.C

Contributors/Copyright:
    2012-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2013 Bruno Santos <wyldckat@gmail.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef manipulatePatchField_H
#define manipulatePatchField_H

#include "primitiveFieldsFwd.H"
#include "volFieldsFwd.H"
#include "pointFieldFwd.H"
#include "Switch.H"
#include "autoPtr.H"
#include "dictionary.H"

#include "swak.H"

#include "exprString.H"

namespace Foam
{
// Forward declaration of classes
class objectRegistry;
class dictionary;
class mapPolyMesh;
class polyMesh;

class PatchValueExpressionDriver;

class manipulatePatchField
{
    //- Disallow default bitwise copy construct
    manipulatePatchField(const manipulatePatchField&);

    //- Disallow default bitwise assignment
    void operator=(const manipulatePatchField&);

protected:

    // Private data

    //- are we working?
    bool active_;

    //- re-write manipulated field
    bool writeManipulated_;

    //- Name of the field
    word name_;

    //- Name of the patch
    word patchName_;

    //- Expression
    exprString expression_;

    //- Expression that says which cells are to be changed
    exprString maskExpression_;

    const objectRegistry& obr_;

    dictionary dict_;

    template<class TField>
    void manipulate(
        const TField &data,
        const Field<bool> &mask
    );

    template<class TField>
    void manipulatePoint(
        const TField &data,
        const Field<bool> &mask
    );

    // template<class TData,class TMask>
    // void manipulateSurface(
    //     const TData &data,
    //     const TMask &mask
    // );

    autoPtr<PatchValueExpressionDriver> driver_;

public:

    //- Runtime type information
    TypeName("manipulatePatchField");


    // Constructors

    //- Construct for given objectRegistry and dictionary.
    //  Allow the possibility to load fields from files
    manipulatePatchField
    (
        const word& name,
        const objectRegistry&,
        const dictionary&,
        const bool loadFromFiles = false
    );


    //- Destructor
    virtual ~manipulatePatchField();


    // Member Functions

    //- Return name of the field
    virtual const word& name() const
        {
            return name_;
        }

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
};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif
