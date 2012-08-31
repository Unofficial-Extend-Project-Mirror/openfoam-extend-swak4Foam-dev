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
    Foam::SwakSetValue

Description
    Fixes field values according to an expression

SourceFiles
    SwakSetValue.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef SwakSetValue_H
#define SwakSetValue_H

#include "SwakBasicSourceCommon.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class SwakSetValue Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class SwakSetValue
:
    public SwakBasicSourceCommon<T>
{
    // Private data

    SwakSetValue(const SwakSetValue&);

     bool useMaskExpression_;

    string maskExpression_;

    bool getMask(DynamicList<label> &,const word &psi);

public:

    //- Runtime type information
    TypeName("SwakSetValue");

    // Constructors

        //- Construct from a dictionary
        SwakSetValue
        (
            const word& name,
            const word& modelType,
            const dictionary& dict,
            const fvMesh& mesh
        );

    // Destructor

        virtual ~SwakSetValue();

        // Evaluation

            //- Set value on vector field
            virtual void setValue(fvMatrix<T>& eqn, const label fieldI);


};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "SwakSetValue.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
