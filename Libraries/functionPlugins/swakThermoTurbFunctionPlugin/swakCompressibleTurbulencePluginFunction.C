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

Contributors/Copyright:
    2012-2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakCompressibleTurbulencePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "HashPtrTable.H"
#include "LESModel.H"
#include "RASModel.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(swakCompressibleTurbulencePluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakCompressibleTurbulencePluginFunction::swakCompressibleTurbulencePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnValueType
):
    swakThermophysicalPluginFunction<swakFluidThermoType>(
        parentDriver,
        name,
        returnValueType
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

    const compressible::turbulenceModel &swakCompressibleTurbulencePluginFunction::turbInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<compressible::turbulenceModel> turb_;

    if(reg.foundObject<compressible::turbulenceModel>("turbulenceProperties")) {
        if(debug) {
            Info << "swakCompressibleTurbulencePluginFunction::turbInternal: "
                << "turbulence already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<compressible::turbulenceModel>("turbulenceProperties");
    }
    if(reg.foundObject<compressible::LESModel>("LESProperties")) {
        if(debug) {
            Info << "swakCompressibleTurbulencePluginFunction::turbInternal: "
                << "LES already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<compressible::LESModel>("LESProperties");
    }
    if(reg.foundObject<compressible::RASModel>("RASProperties")) {
        if(debug) {
            Info << "swakCompressibleTurbulencePluginFunction::turbInternal: "
                << "RAS already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<compressible::RASModel>("RASProperties");
    }

    if(!turb_.found(reg.name())) {
        if(debug) {
            Info << "swakCompressibleTurbulencePluginFunction::turbInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }

        turb_.set(
            reg.name(),
            compressible::turbulenceModel::New(
                // reg.lookupObject<volScalarField>("rho"),
                thermoInternal(reg).rho(),
                reg.lookupObject<volVectorField>("U"),
                reg.lookupObject<surfaceScalarField>("phi"),
                thermoInternal(reg)
            ).ptr()
        );
    }

    return *(turb_[reg.name()]);
}

    const compressible::turbulenceModel &swakCompressibleTurbulencePluginFunction::turb()
{
    return turbInternal(mesh());
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //

#define concreteTurbFunction(funcName,resultType)                  \
class swakCompressibleTurbulencePluginFunction_ ## funcName        \
: public swakCompressibleTurbulencePluginFunction                  \
{                                                                  \
public:                                                            \
    TypeName("swakCompressibleTurbulencePluginFunction_" #funcName);       \
    swakCompressibleTurbulencePluginFunction_ ## funcName (        \
        const FieldValueExpressionDriver &parentDriver,            \
        const word &name                                           \
    ): swakCompressibleTurbulencePluginFunction(                   \
        parentDriver,                                              \
        name,                                                      \
        #resultType                                                \
    ) {}                                                           \
    void doEvaluation() {                                          \
        result().setObjectResult(                                  \
            autoPtr<resultType>(                                   \
                new resultType(                                    \
                    turb().funcName()                              \
                )                                                  \
            )                                                      \
        );                                                         \
    }                                                              \
};                                                                 \
defineTypeNameAndDebug(swakCompressibleTurbulencePluginFunction_ ## funcName,0);  \
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakCompressibleTurbulencePluginFunction_ ## funcName,name,rhoTurb_ ## funcName);

concreteTurbFunction(mut,volScalarField);
concreteTurbFunction(muEff,volScalarField);
concreteTurbFunction(alphaEff,volScalarField);
concreteTurbFunction(k,volScalarField);
concreteTurbFunction(epsilon,volScalarField);
concreteTurbFunction(R,volSymmTensorField);
concreteTurbFunction(devRhoReff,volSymmTensorField);

#ifdef FOAM_HAS_FLUIDTHERMO
concreteTurbFunction(kappaEff,volScalarField);
#ifndef FOAM_NEW_TURBULENCE_STRUCTURE
concreteTurbFunction(rhoEpsilonEff,volScalarField);
#endif
#endif

} // namespace

// ************************************************************************* //
