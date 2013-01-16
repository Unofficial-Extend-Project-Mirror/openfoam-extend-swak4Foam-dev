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
    Foam::GeneralSetsPluginFunction

Description

SourceFiles
    GeneralSetsPluginFunction.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef GeneralSetsPluginFunction_H
#define GeneralSetsPluginFunction_H

#include "FieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"

#include "sampledSet.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class GeneralSetsPluginFunction Declaration
\*---------------------------------------------------------------------------*/
class GeneralSetsPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const GeneralSetsPluginFunction &);

    GeneralSetsPluginFunction(const GeneralSetsPluginFunction &);

    word name_;

    autoPtr<labelList> meshCells_;

protected:

    inline const word &name() const {return name_; }

    const sampledSet &theSet() const;

public:
    GeneralSetsPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const word &resultType,
        const string &arguments
    );

    virtual ~GeneralSetsPluginFunction() {}

    TypeName("GeneralSetsPluginFunction");

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
