/*---------------------------------------------------------------------------*\
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
    Foam::shiftFieldExpressionPluginFunction

Description

SourceFiles
    shiftFieldExpressionPluginFunction.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef shiftFieldExpressionPluginFunction_H
#define shiftFieldExpressionPluginFunction_H

#include "shiftFieldGeneralPluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
    Class shiftFieldExpressionPluginFunction Declaration
\*---------------------------------------------------------------------------*/

template<class Type,meshToMeshOrder Order>
class shiftFieldExpressionPluginFunction
:
    public shiftFieldGeneralPluginFunction<Type,Order>
{
    //- Disallow default bitwise assignment
    void operator=(const shiftFieldExpressionPluginFunction &);

    shiftFieldExpressionPluginFunction(const shiftFieldExpressionPluginFunction &);

    autoPtr<pointVectorField> shiftValue_;

protected:

    tmp<pointField> displacement();

    void setArgument(
        label index,
        const string &content,
        const CommonValueExpressionDriver &driver
    );

public:
    shiftFieldExpressionPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~shiftFieldExpressionPluginFunction() {}

    TypeName("shiftFieldExpressionPluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
