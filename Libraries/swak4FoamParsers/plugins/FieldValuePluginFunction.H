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
    Foam::FieldValuePluginFunction

Description

SourceFiles
    FieldValuePluginFunction.C

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef FieldValuePluginFunction_H
#define FieldValuePluginFunction_H

#include "CommonPluginFunction.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class FieldValueExpressionDriver;

/*---------------------------------------------------------------------------*\
                           Class FieldValuePluginFunction Declaration
\*---------------------------------------------------------------------------*/
class FieldValuePluginFunction 
:
    public CommonPluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const FieldValuePluginFunction &);

    FieldValuePluginFunction(const FieldValuePluginFunction &);

public:
    FieldValuePluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const word &returnType,
        const stringList &argumentSpecification
    );

    virtual ~FieldValuePluginFunction() {}

    TypeName("FieldValuePluginFunction");

    declareRunTimeSelectionTable
    (
        autoPtr,
        FieldValuePluginFunction,
        name,
        (
            const FieldValueExpressionDriver& driver,
            const word &name
        ),
        (driver,name)
    );

    static bool exists(const word &name);

    static autoPtr<FieldValuePluginFunction> New
    (
        const FieldValueExpressionDriver& driver,
        const word &name
    );
};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
