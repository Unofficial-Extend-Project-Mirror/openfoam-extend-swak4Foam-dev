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
    2012-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakIncompressibleTurbulencePluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "HashPtrTable.H"
#include "LESModel.H"
#include "RASModel.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(swakIncompressibleTurbulencePluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakIncompressibleTurbulencePluginFunction::swakIncompressibleTurbulencePluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnValueType
):
    swakTransportModelsPluginFunction(
        parentDriver,
        name,
        returnValueType
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const incompressible::turbulenceModel &swakIncompressibleTurbulencePluginFunction::turbInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<incompressible::turbulenceModel> turb_;

    if(reg.foundObject<incompressible::turbulenceModel>("turbulenceProperties")) {
        if(debug) {
            Info << "swakIncompressibleTurbulencePluginFunction::turbInternal: "
                << "turbulence model already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<incompressible::turbulenceModel>("turbulenceProperties");
    }
    if(reg.foundObject<incompressible::LESModel>("LESProperties")) {
        if(debug) {
            Info << "swakIncompressibleTurbulencePluginFunction::turbInternal: "
                << "LES already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<incompressible::LESModel>("LESProperties");
    }
    if(reg.foundObject<incompressible::RASModel>("RASProperties")) {
        if(debug) {
            Info << "swakIncompressibleTurbulencePluginFunction::turbInternal: "
                << "RAS already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<incompressible::RASModel>("RASProperties");
    }

    if(!turb_.found(reg.name())) {
        if(debug) {
            Info << "swakIncompressibleTurbulencePluginFunction::turbInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }

        turb_.set(
            reg.name(),
            incompressible::turbulenceModel::New(
                reg.lookupObject<volVectorField>("U"),
                reg.lookupObject<surfaceScalarField>("phi"),
                const_cast<transportModel &>(transportInternal(reg))
            ).ptr()
        );
    }

    return *(turb_[reg.name()]);
}

const incompressible::turbulenceModel &swakIncompressibleTurbulencePluginFunction::turb()
{
    return turbInternal(mesh());
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //

#define concreteTurbFunction(funcName,resultType)                  \
class swakIncompressibleTurbulencePluginFunction_ ## funcName      \
: public swakIncompressibleTurbulencePluginFunction                \
{                                                                  \
public:                                                            \
    TypeName("swakIncompressibleTurbulencePluginFunction_" #funcName);       \
    swakIncompressibleTurbulencePluginFunction_ ## funcName (           \
        const FieldValueExpressionDriver &parentDriver,                 \
        const word &name                                                \
    ): swakIncompressibleTurbulencePluginFunction(                      \
        parentDriver,                                                   \
        name,                                                           \
        #resultType                                                     \
    ) {}                                                                \
        void doEvaluation() {                                           \
        autoPtr<resultType> rField(                                     \
            new resultType(                                             \
                turb().funcName()                                       \
            )                                                           \
        );                                                              \
        rField->correctBoundaryConditions();                            \
        result().setObjectResult(                                       \
            rField                                                      \
        );                                                              \
    }                                                                   \
};                                                                      \
defineTypeNameAndDebug(swakIncompressibleTurbulencePluginFunction_ ## funcName,0);  \
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakIncompressibleTurbulencePluginFunction_ ## funcName,name,turb_ ## funcName);

concreteTurbFunction(nut,volScalarField);
concreteTurbFunction(nuEff,volScalarField);
concreteTurbFunction(k,volScalarField);
concreteTurbFunction(epsilon,volScalarField);
concreteTurbFunction(R,volSymmTensorField);
concreteTurbFunction(devReff,volSymmTensorField);


} // namespace

// ************************************************************************* //
