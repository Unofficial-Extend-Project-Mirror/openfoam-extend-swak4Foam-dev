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
    Foam::swakCompressibleTurbulencePluginFunction

Description

SourceFiles
    swakCompressibleTurbulencePluginFunction.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakCompressibleTurbulencePluginFunction_H
#define swakCompressibleTurbulencePluginFunction_H

#include "swakThermophysicalPluginFunction.H"

#include "turbulenceModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class swakCompressibleTurbulencePluginFunction Declaration
\*---------------------------------------------------------------------------*/
class swakCompressibleTurbulencePluginFunction
:
    public swakThermophysicalPluginFunction
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
