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
    Foam::SwakImplicitSource

Description
    Sets an implicit source term according to an expression

SourceFiles
    SwakImplicitSource.C

Contributors/Copyright:
    2010, 2012-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakImplicitSource_H
#define SwakImplicitSource_H

#include "SwakBasicSourceCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

#ifdef FOAM_HAS_FVOPTIONS
    namespace fv {
#endif

/*---------------------------------------------------------------------------*\
                           Class SwakImplicitSource Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class SwakImplicitSource
:
    public SwakBasicSourceCommon<T>
{
    // Private data

    SwakImplicitSource(const SwakImplicitSource&);

    bool switchExplicitImplicit_;

public:

    //- Runtime type information
    TypeName("SwakImplicitSource");

    // Constructors

        //- Construct from a dictionary
        SwakImplicitSource
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

    // Destructor

        virtual ~SwakImplicitSource();

        // Evaluation

    //- Add explicit contribution to equation
    virtual void addSup(
        fvMatrix<T>& eqn,
        const label fieldI
    );

#ifdef FOAM_FVOPTION_HAS_ADDITIONAL_ADDSUP
    // Add explicit and implicit contributions to compressible equations
    virtual void addSup(
        const volScalarField& rho,
        fvMatrix<T>& eqn,
        const label fieldI
    );

    // Add explicit and implicit contributions to phase equations
    virtual void addSup(
        const volScalarField& alpha,
        const volScalarField& rho,
        fvMatrix<T>& eqn,
        const label fieldI
    );
#endif

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
#ifdef FOAM_HAS_FVOPTIONS
    }
#endif

} // End namespace Foam

#ifdef NoRepository
#   include "SwakImplicitSource.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
