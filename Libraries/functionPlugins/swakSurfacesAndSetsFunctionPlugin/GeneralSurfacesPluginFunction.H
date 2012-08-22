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
    Foam::GeneralSurfacesPluginFunction

Description

SourceFiles
    GeneralSurfacesPluginFunction.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef GeneralSurfacesPluginFunction_H
#define GeneralSurfacesPluginFunction_H

#include "FieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"

#include "sampledSurface.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class GeneralSurfacesPluginFunction Declaration
\*---------------------------------------------------------------------------*/
class GeneralSurfacesPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const GeneralSurfacesPluginFunction &);

    GeneralSurfacesPluginFunction(const GeneralSurfacesPluginFunction &);

    word name_;

    autoPtr<labelList> meshCells_;

protected:

    inline const word &name() const {return name_; }

    const sampledSurface &theSurface() const;

    const labelList &meshCells() ;

public:
    GeneralSurfacesPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const word &resultType,
        const string &arguments
    );

    virtual ~GeneralSurfacesPluginFunction() {}

    TypeName("GeneralSurfacesPluginFunction");

    void setArgument(
        label index,
        const word &content
    );

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
