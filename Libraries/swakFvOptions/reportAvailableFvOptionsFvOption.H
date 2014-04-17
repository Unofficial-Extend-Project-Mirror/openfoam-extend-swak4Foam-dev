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
    This file is part of OpenFOAM.

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
    Foam::fv::reportAvailableFvOptionsFvOption

Description
    This fvOption does not contribute to the equation but only reports
    for *every* possible call that the call was done

SourceFiles
    reportAvailableFvOptionsFvOption.C

Contributors/Copyright:
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef reportAvailableFvOptionsFvOption_H
#define reportAvailableFvOptionsFvOption_H

#include "autoPtr.H"
#include "topoSetSource.H"
#include "cellSet.H"
#include "fvMesh.H"
#include "volFields.H"
#include "fvOption.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class reportAvailableFvOptionsFvOption Declaration
\*---------------------------------------------------------------------------*/

class reportAvailableFvOptionsFvOption
:
    public option
{
    // Private data

   // Private Member Functions

    //- Disallow default bitwise copy construct
    reportAvailableFvOptionsFvOption(const reportAvailableFvOptionsFvOption&);

    //- Disallow default bitwise assignment
    void operator=(const reportAvailableFvOptionsFvOption&);

    void report(const string &message);

public:

    //- Runtime type information
    TypeName("reportAvailableFvOptions");


    // Constructors

    //- Construct from explicit source name and mesh
    reportAvailableFvOptionsFvOption
    (
        const word& sourceName,
        const word& modelType,
        const dictionary& dict,
        const fvMesh& mesh
    );

    ~reportAvailableFvOptionsFvOption();

    // Member Functions

    virtual bool alwaysApply() const;

    // Evaluate

    // Correct

    //- Scalar
    virtual void correct(volScalarField& fld);

    //- Vector
    virtual void correct(volVectorField& fld);

    //- Spherical tensor
    virtual void correct(volSphericalTensorField& fld);

    //- Symmetric tensor
    virtual void correct(volSymmTensorField& fld);

    //- Tensor
    virtual void correct(volTensorField& fld);

    // Add explicit and implicit contributions

    //- Scalar
    virtual void addSup
    (
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void addSup
    (
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void addSup
    (
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void addSup
    (
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void addSup
    (
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

    // Set values directly

    //- Scalar
    virtual void setValue
    (
        fvMatrix<scalar>& eqn,
        const label fieldI
    );

    //- Vector
    virtual void setValue
    (
        fvMatrix<vector>& eqn,
        const label fieldI
    );

    //- Spherical tensor
    virtual void setValue
    (
        fvMatrix<sphericalTensor>& eqn,
        const label fieldI
    );

    //- Symmetric tensor
    virtual void setValue
    (
        fvMatrix<symmTensor>& eqn,
        const label fieldI
    );

    //- Tensor
    virtual void setValue
    (
        fvMatrix<tensor>& eqn,
        const label fieldI
    );

    // Flux manipulations

    //- Make the given absolute flux relative
    virtual void makeRelative(surfaceScalarField& phi) const;

    //- Make the given absolute boundary flux relative
    virtual void makeRelative
    (
        FieldField<fvsPatchField, scalar>& phi
    ) const;

    //- Make the given absolute mass-flux relative
    virtual void makeRelative
    (
        const surfaceScalarField& rho,
        surfaceScalarField& phi
    ) const;

    //- Make the given relative flux absolute
    virtual void makeAbsolute(surfaceScalarField& phi) const;

    //- Make the given relative mass-flux absolute
    virtual void makeAbsolute
    (
        const surfaceScalarField& rho,
        surfaceScalarField& phi
    ) const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
