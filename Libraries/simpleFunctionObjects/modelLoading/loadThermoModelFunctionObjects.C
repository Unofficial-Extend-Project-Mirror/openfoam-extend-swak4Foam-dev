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

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "loadThermoModelFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "swakThermoTypes.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    addNamedTemplateToRunTimeSelectionTable(functionObject,loadThermoModelFunctionObject,swakPsiThermoType,dictionary,loadPsiThermoModel);

    defineTemplateTypeNameAndDebug(loadThermoModelFunctionObject<swakPsiThermoType>,0);

    template
    class loadThermoModelFunctionObject<swakPsiThermoType>;

    addNamedTemplateToRunTimeSelectionTable(functionObject,loadThermoModelFunctionObject,swakRhoThermoType,dictionary,loadRhoThermoModel);

    defineTemplateTypeNameAndDebug(loadThermoModelFunctionObject<swakRhoThermoType>,0);

    template
    class loadThermoModelFunctionObject<swakRhoThermoType>;

} // namespace Foam

// ************************************************************************* //
