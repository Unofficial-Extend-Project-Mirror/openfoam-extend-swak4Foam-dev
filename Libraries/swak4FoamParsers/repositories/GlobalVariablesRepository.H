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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::GlobalVariablesRepository

Description

SourceFiles
    GlobalVariablesRepository.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef GlobalVariablesRepository_H
#define GlobalVariablesRepository_H

#include "ExpressionResult.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class GlobalVariablesRepository Declaration
\*---------------------------------------------------------------------------*/

class GlobalVariablesRepository
{
public:
   typedef HashTable<ExpressionResult,word> ResultTable;

private:
   // Private data

     HashTable<ResultTable,word> globalVariables_;

    // Constructors

    //- Construct null
    GlobalVariablesRepository();

    //- The only instance of this repository
    static GlobalVariablesRepository *repositoryInstance;

    static ExpressionResult zero_;

public:
     //- Runtime type information
    TypeName("GlobalVariablesRepository");

    //- Destructor

    virtual ~GlobalVariablesRepository();

    //- Get the Singleton of this class
    static GlobalVariablesRepository &getGlobalVariables();

    //- get a global variable (if there is one)
    const ExpressionResult &get(
        const word &name,
        const wordList &scopes
    );

    void addValue(
        const word &name,
        const word &scope,
        const ExpressionResult &value
    );

    void addValue(
        const dictionary &dict,
        const word scope=""
    );

    //- get a global namespace
    ResultTable &getNamespace(const word &name);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
