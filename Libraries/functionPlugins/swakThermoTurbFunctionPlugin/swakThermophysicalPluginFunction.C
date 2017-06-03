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

#include "swakThermophysicalPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "HashPtrTable.H"

#include "addToRunTimeSelectionTable.H"

#ifdef FOAM_HAS_FLUIDTHERMO
#include "solidThermo.H"
#endif

namespace Foam {

#ifdef FOAM_HAS_FLUIDTHERMO
defineTemplateTypeNameAndDebug(swakThermophysicalPluginFunction<swakFluidThermoType>,0);
defineTemplateTypeNameAndDebug(swakThermophysicalPluginFunction<solidThermo>,0);
#endif

defineTemplateTypeNameAndDebug(swakThermophysicalPluginFunction<basicThermo>,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class ThermoType>
swakThermophysicalPluginFunction<ThermoType>::swakThermophysicalPluginFunction(
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

  // this explicit instantiation is needed on Ubunut 14.04
  // the compiler of that crappy pseudo-Linux needs it (but nobody else)
#ifdef FOAM_HAS_FLUIDTHERMO
template class swakThermophysicalPluginFunction<swakFluidThermoType>;
template class swakThermophysicalPluginFunction<solidThermo>;
#endif
template class swakThermophysicalPluginFunction<basicThermo>;

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class ThermoType>
const ThermoType &swakThermophysicalPluginFunction<ThermoType>::thermoInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<ThermoType> thermo_;

    if(reg.foundObject<ThermoType>("thermophysicalProperties")) {
        if(debug) {
            Info << "swakThermophysicalPluginFunction::thermoInternal: "
                << "already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<ThermoType>("thermophysicalProperties");
    }
    if(!thermo_.found(reg.name())) {
        if(debug) {
            Info << "swakThermophysicalPluginFunction::thermoInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }

        bool usePsi=true;
        bool found=false;

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

            word thermoTypeName;
            if (dict.isDict("thermoType")) {
                const dictionary& thermoTypeDict(dict.subDict("thermoType"));

                // Construct the name of the thermo package from the components
                thermoTypeName =
                    word(thermoTypeDict.lookup("type")) + '<'
                    + word(thermoTypeDict.lookup("mixture")) + '<'
                    + word(thermoTypeDict.lookup("transport")) + '<'
                    + word(thermoTypeDict.lookup("thermo")) + '<'
                    + word(thermoTypeDict.lookup("equationOfState")) + '<'
                    + word(thermoTypeDict.lookup("specie")) + ">>,"
                    + word(thermoTypeDict.lookup("energy")) + ">>>";
            } else {
                thermoTypeName=word(dict["thermoType"]);
            }

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
                found=true;
            } else if(debug) {
                Info << "No " << thermoTypeName << " in rhoThermo-types "
#ifdef FOAM_HAS_SORTED_TOC
                    << swakRhoThermoType::fvMeshConstructorTablePtr_->sortedToc()
#else
                    << swakRhoThermoType::fvMeshConstructorTablePtr_->toc()
#endif
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
                    found=true;
                } else if(debug) {
                    Info << "No " << thermoTypeName << " in psiThermo-types "
#ifdef FOAM_HAS_SORTED_TOC
                    << swakPsiThermoType::fvMeshConstructorTablePtr_->sortedToc()
#else
                    << swakPsiThermoType::fvMeshConstructorTablePtr_->toc()
#endif
                        << endl;
                }
            }
        }

        if(!found) {
#ifdef FOAM_BASIC_THERMO_HAS_NO_NEW
            FatalErrorIn("swakThermophysicalPluginFunction<ThermoType>::thermoInternal")
                << "This version of Foam has no basicThermo::New"
                    << endl
                    << exit(FatalError);
#else
            thermo_.set(
                reg.name(),
                ThermoType::New(reg).ptr()
            );
#endif
        } else {
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
    }

    return *(thermo_[reg.name()]);
}

#ifdef FOAM_HAS_FLUIDTHERMO
template<>
const solidThermo &swakThermophysicalPluginFunction<solidThermo>::thermoInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<solidThermo> thermo_;

    if(reg.foundObject<solidThermo>("thermophysicalProperties")) {
        if(debug) {
            Info << "swakThermophysicalPluginFunction::thermoInternal: "
                << "already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<solidThermo>("thermophysicalProperties");
    }
    if(!thermo_.found(reg.name())) {
        if(debug) {
            Info << "swakThermophysicalPluginFunction::thermoInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }

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

            word thermoTypeName;
            if (dict.isDict("thermoType")) {
                const dictionary& thermoTypeDict(dict.subDict("thermoType"));

                // Construct the name of the thermo package from the components
                thermoTypeName =
                    word(thermoTypeDict.lookup("type")) + '<'
                    + word(thermoTypeDict.lookup("mixture")) + '<'
                    + word(thermoTypeDict.lookup("transport")) + '<'
                    + word(thermoTypeDict.lookup("thermo")) + '<'
                    + word(thermoTypeDict.lookup("equationOfState")) + '<'
                    + word(thermoTypeDict.lookup("specie")) + ">>,"
                    + word(thermoTypeDict.lookup("energy")) + ">>>";
            } else {
                thermoTypeName=word(dict["thermoType"]);
            }

            solidThermo::fvMeshConstructorTable::iterator cstrIter =
                solidThermo::fvMeshConstructorTablePtr_->find(
                    thermoTypeName
                );
            if (cstrIter != solidThermo::fvMeshConstructorTablePtr_->end())
            {
                if(debug) {
                    Info << thermoTypeName << " is a solidThermo-type";
                }
            } else if(debug) {
                Info << "No " << thermoTypeName << " in solidThermo-types "
#ifdef FOAM_HAS_SORTED_TOC
                    << solidThermo::fvMeshConstructorTablePtr_->sortedToc()
#else
                    << solidThermo::fvMeshConstructorTablePtr_->toc()
#endif
                    << endl;
            }

        }

        // Create it ourself because nobody registered it
        thermo_.set(
            reg.name(),
            solidThermo::New(reg).ptr()
        );
    }

    return *(thermo_[reg.name()]);
}
#endif

template<class ThermoType>
const ThermoType &swakThermophysicalPluginFunction<ThermoType>::thermo()
{
    return thermoInternal(mesh());
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //

#define concreteThermoFunction(funcName,resultType,tthermo)        \
class swakThermophysicalPluginFunction_ ## funcName                \
: public swakThermophysicalPluginFunction<tthermo>                 \
{                                                                  \
public:                                                            \
    TypeName("swakThermophysicalPluginFunction_" #funcName);       \
    swakThermophysicalPluginFunction_ ## funcName (                \
        const FieldValueExpressionDriver &parentDriver,            \
        const word &name                                           \
    ): swakThermophysicalPluginFunction<tthermo>(                  \
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

concreteThermoFunction(p,volScalarField,basicThermo);
concreteThermoFunction(rho,volScalarField,basicThermo);
concreteThermoFunction(psi,volScalarField,swakFluidThermoType);
#ifdef FOAM_HAS_FLUIDTHERMO
concreteThermoFunction(he,volScalarField,basicThermo);
concreteThermoFunction(Kappa,volVectorField,solidThermo);
#else
concreteThermoFunction(h,volScalarField,basicThermo);
concreteThermoFunction(hs,volScalarField,basicThermo);
concreteThermoFunction(e,volScalarField,basicThermo);
#endif
concreteThermoFunction(hc,volScalarField,basicThermo);
concreteThermoFunction(T,volScalarField,basicThermo);
concreteThermoFunction(Cp,volScalarField,basicThermo);
concreteThermoFunction(Cv,volScalarField,basicThermo);
concreteThermoFunction(mu,volScalarField,swakFluidThermoType);
concreteThermoFunction(alpha,volScalarField,basicThermo);

#ifdef FOAM_HAS_FLUIDTHERMO
concreteThermoFunction(gamma,volScalarField,basicThermo);
concreteThermoFunction(Cpv,volScalarField,basicThermo);
concreteThermoFunction(CpByCpv,volScalarField,basicThermo);
concreteThermoFunction(kappa,volScalarField,basicThermo);
    // concreteThermoFunction(kappaEff,volScalarField,basicThermo);
    // concreteThermoFunction(alphaEff,volScalarField,basicThermo);
#endif

} // namespace

// ************************************************************************* //
