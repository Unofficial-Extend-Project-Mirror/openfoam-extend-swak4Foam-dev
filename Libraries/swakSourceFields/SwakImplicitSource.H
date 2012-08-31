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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakImplicitSource_H
#define SwakImplicitSource_H

#include "SwakBasicSourceCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

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

            //- Add implicit contribution to equation
            virtual void addSup(fvMatrix<T>& eqn, const label fieldI);


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "SwakImplicitSource.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
