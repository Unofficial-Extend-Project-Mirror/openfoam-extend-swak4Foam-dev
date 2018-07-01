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
    Foam::ConcretePluginFunction

Description

SourceFiles
    ConcretePluginFunction.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef ConcretePluginFunction_H
#define ConcretePluginFunction_H

#include "CommonPluginFunction.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class ConcretePluginFunction Declaration
\*---------------------------------------------------------------------------*/
template<class DriverType>
class ConcretePluginFunction
:
    public CommonPluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const ConcretePluginFunction &);

    ConcretePluginFunction(const ConcretePluginFunction &);

public:
    ConcretePluginFunction(
        const DriverType &parentDriver,
        const word &name,
        const word &returnType,
        const string &argumentSpecification
    );

    virtual ~ConcretePluginFunction() {}

    TypeName("ConcretePluginFunction");

    typedef DriverType driverType;

    declareRunTimeSelectionTable
    (
        autoPtr,
        ConcretePluginFunction,
        name,
        (
            const DriverType& driver,
            const word &name
        ),
        (driver,name)
    );

    static bool exists(
        const DriverType& driver,
        const word &name
    );

    static autoPtr<ConcretePluginFunction> New
    (
        const DriverType& driver,
        const word &name
    );
};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "ConcretePluginFunction.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
