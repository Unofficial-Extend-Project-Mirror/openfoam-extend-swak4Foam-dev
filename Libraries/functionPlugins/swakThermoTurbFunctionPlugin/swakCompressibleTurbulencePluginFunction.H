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
    Foam::swakCompressibleTurbulencePluginFunction

Description

SourceFiles
    swakCompressibleTurbulencePluginFunction.C

Contributors/Copyright:
    2012-2013, 2015-2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakCompressibleTurbulencePluginFunction_H
#define swakCompressibleTurbulencePluginFunction_H

#include "swakThermophysicalPluginFunction.H"

#include "swakTurbulence.H"

#ifdef FOAM_NEW_TURBULENCE_STRUCTURE
#include "turbulentFluidThermoModel.H"
#else
#include "turbulenceModel.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class swakCompressibleTurbulencePluginFunction Declaration
\*---------------------------------------------------------------------------*/
class swakCompressibleTurbulencePluginFunction
:
    public swakThermophysicalPluginFunction<swakFluidThermoType>
{
    //- Disallow default bitwise assignment
    void operator=(const swakCompressibleTurbulencePluginFunction &);

    swakCompressibleTurbulencePluginFunction(const swakCompressibleTurbulencePluginFunction &);

protected:
    static const compressible::turbulenceModel &turbInternal(const fvMesh &reg);

    const compressible::turbulenceModel &turb();

public:
    swakCompressibleTurbulencePluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const word &returnValueType
    );

    virtual ~swakCompressibleTurbulencePluginFunction() {}

    TypeName("swakCompressibleTurbulencePluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
