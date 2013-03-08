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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakThermophysicalPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "HashPtrTable.H"

#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(swakThermophysicalPluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakThermophysicalPluginFunction::swakThermophysicalPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnValueType
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        returnValueType,
        string("")
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const swakFluidThermoType &swakThermophysicalPluginFunction::thermoInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<swakFluidThermoType> thermo_;

    if(reg.foundObject<swakFluidThermoType>("thermophysicalProperties")) {
        if(debug) {
            Info << "swakThermophysicalPluginFunction::thermoInternal: "
                << "already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<swakFluidThermoType>("thermophysicalProperties");
    }
    if(!thermo_.found(reg.name())) {
        if(debug) {
            Info << "swakThermophysicalPluginFunction::thermoInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }

        bool usePsi=true;

        {
            // make sure it is gone before we create the object
            IOdictionary dict
                (
                    IOobject
                    (
                        "thermophysicalProperties",
                        reg.time().constant(),
                        reg,
                        IOobject::MUST_READ,
                        IOobject::NO_WRITE
                    )
                );

            word thermoTypeName=dict["thermoType"];

            swakRhoThermoType::fvMeshConstructorTable::iterator cstrIter =
                swakRhoThermoType::fvMeshConstructorTablePtr_->find(
                    thermoTypeName
                );
            if (cstrIter != swakRhoThermoType::fvMeshConstructorTablePtr_->end())
            {
                if(debug) {
                    Info << thermoTypeName << " is a rhoThermo-type";
                }
                usePsi=false;
            } else if(debug) {
                Info << "No " << thermoTypeName << " in rhoThermo-types "
                    << swakRhoThermoType::fvMeshConstructorTablePtr_->sortedToc()
                    << endl;
            }
            if(usePsi) {
                swakPsiThermoType::fvMeshConstructorTable::iterator cstrIter =
                    swakPsiThermoType::fvMeshConstructorTablePtr_->find(
                        thermoTypeName
                    );
                if(cstrIter != swakPsiThermoType::fvMeshConstructorTablePtr_->end())
                {
                    if(debug) {
                        Info << thermoTypeName << " is a psiThermo-type";
                    }
                } else if(debug) {
                    Info << "No " << thermoTypeName << " in psiThermo-types "
                    << swakPsiThermoType::fvMeshConstructorTablePtr_->sortedToc()
                        << endl;
                }
            }
        }

        // Create it ourself because nobody registered it
        if(usePsi) {
            thermo_.set(
                reg.name(),
                swakPsiThermoType::New(reg).ptr()
            );
        } else {
            thermo_.set(
                reg.name(),
                swakRhoThermoType::New(reg).ptr()
            );
        }
    }

    return *(thermo_[reg.name()]);
}

const swakFluidThermoType &swakThermophysicalPluginFunction::thermo()
{
    return thermoInternal(mesh());
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //

#define concreteThermoFunction(funcName,resultType)                \
class swakThermophysicalPluginFunction_ ## funcName                \
: public swakThermophysicalPluginFunction                          \
{                                                                  \
public:                                                            \
    TypeName("swakThermophysicalPluginFunction_" #funcName);       \
    swakThermophysicalPluginFunction_ ## funcName (                \
        const FieldValueExpressionDriver &parentDriver,            \
        const word &name                                           \
    ): swakThermophysicalPluginFunction(                           \
        parentDriver,                                              \
        name,                                                      \
        #resultType                                                \
    ) {}                                                           \
    void doEvaluation() {                                          \
        result().setObjectResult(                                  \
            autoPtr<resultType>(                                   \
                new resultType(                                    \
                    thermo().funcName()                            \
                )                                                  \
            )                                                      \
        );                                                         \
    }                                                              \
};                                                                 \
defineTypeNameAndDebug(swakThermophysicalPluginFunction_ ## funcName,0);  \
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakThermophysicalPluginFunction_ ## funcName,name,thermo_ ## funcName);

concreteThermoFunction(p,volScalarField);
concreteThermoFunction(rho,volScalarField);
concreteThermoFunction(psi,volScalarField);
#ifdef FOAM_HAS_FLUIDTHERMO
concreteThermoFunction(he,volScalarField);
#else
concreteThermoFunction(h,volScalarField);
concreteThermoFunction(hs,volScalarField);
concreteThermoFunction(e,volScalarField);
#endif
concreteThermoFunction(hc,volScalarField);
concreteThermoFunction(T,volScalarField);
concreteThermoFunction(Cp,volScalarField);
concreteThermoFunction(Cv,volScalarField);
concreteThermoFunction(mu,volScalarField);
concreteThermoFunction(alpha,volScalarField);

} // namespace

// ************************************************************************* //
