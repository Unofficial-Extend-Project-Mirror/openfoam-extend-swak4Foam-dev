/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Foam::GeneralSetsPluginFunction

Description

SourceFiles
    GeneralSetsPluginFunction.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
