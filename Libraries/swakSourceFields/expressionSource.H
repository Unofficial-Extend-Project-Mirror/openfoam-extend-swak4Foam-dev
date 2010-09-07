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
    Foam::expressionSource

Description
    Generates a field according to an expression

SourceFiles
    expressionSource.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef expressionSource_H
#define expressionSource_H

#include "FieldValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class expressionSource Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class expressionSource
:
    protected FieldValueExpressionDriver
{
    // Private data

    expressionSource(const expressionSource&);

    string expression_;

public:

    // Constructors

        //- Construct from a dictionary
        expressionSource
        (
            const dictionary& ,
            const fvMesh& 
        );

    // Destructor

        virtual ~expressionSource();

    typedef GeometricField<T,fvPatchField,volMesh> resultField;

    //- give back a Field
    tmp<resultField> operator()();
    
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
