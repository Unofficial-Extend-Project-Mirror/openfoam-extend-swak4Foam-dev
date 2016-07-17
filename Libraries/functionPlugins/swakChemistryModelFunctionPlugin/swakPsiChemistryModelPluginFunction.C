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
    2012-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakPsiChemistryModelPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "HashPtrTable.H"
#include "swakThermoTypes.H"

#include "addToRunTimeSelectionTable.H"

#include "swak.H"

namespace Foam {

defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakPsiChemistryModelPluginFunction::swakPsiChemistryModelPluginFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name,
    const word &returnValueType,
    const string &spec
):
    FieldValuePluginFunction(
        parentDriver,
        name,
        returnValueType,
        spec
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

const psiChemistryModel &swakPsiChemistryModelPluginFunction::chemistryInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<psiChemistryModel> chemistry_;

    if(reg.foundObject<psiChemistryModel>("chemistryProperties")) {
        if(debug) {
            Info << "swakPsiChemistryModelPluginFunction::chemistryInternal: "
                << "already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<psiChemistryModel>("chemistryProperties");
    }
    if(!chemistry_.found(reg.name())) {
        if(debug) {
            Info << "swakPsiChemistryModelPluginFunction::chemistryInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }

        // Create it ourself because nobody registered it
        chemistry_.set(
                reg.name(),
                psiChemistryModel::New(reg).ptr()
        );

        Info << "Created chemistry model. Calculating to get values ..."
            << endl;

        chemistry_[reg.name()]->solve(
#ifdef FOAM_CHEMISTRYMODEL_SOLVE_NEEDS_TIME
            reg.time().value(),
#endif
            reg.time().deltaT().value()
        );
        //        chemistry_[reg.name()]->calculate();
    }

    return *(chemistry_[reg.name()]);
}

void swakPsiChemistryModelPluginFunction::updateChemistry(const scalar dt)
{
    const_cast<psiChemistryModel&>(
        chemistry()
    ).solve(
#ifdef FOAM_CHEMISTRYMODEL_SOLVE_NEEDS_TIME
        mesh().time().value(),
#endif
        dt
    );
}

#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
tmp<volScalarField> swakPsiChemistryModelPluginFunction::wrapDimField(
        const DimensionedField<scalar,volMesh> &dimField
)
{
    tmp<volScalarField> result(
            new volScalarField(
                IOobject(
                    dimField.name(),
                    mesh().time().timeName(),
                    mesh(),
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh(),
                dimensionedScalar(dimField.name(),dimField.dimensions(),0),
                "zeroGradient"
            )
    );
    result->dimensionedInternalField()=dimField;

    return result;
}
#endif

const psiChemistryModel &swakPsiChemistryModelPluginFunction::chemistry()
{
    return chemistryInternal(mesh());
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //

#define concreteChemistryFunction(funcName,resultType)                \
class swakPsiChemistryModelPluginFunction_ ## funcName                \
: public swakPsiChemistryModelPluginFunction                          \
{                                                                  \
public:                                                            \
    TypeName("swakPsiChemistryModelPluginFunction_" #funcName);       \
    swakPsiChemistryModelPluginFunction_ ## funcName (                \
        const FieldValueExpressionDriver &parentDriver,            \
        const word &name                                           \
    ): swakPsiChemistryModelPluginFunction(                           \
        parentDriver,                                              \
        name,                                                      \
        #resultType                                                \
    ) {}                                                           \
    void doEvaluation() {                                          \
        result().setObjectResult(                                  \
            autoPtr<resultType>(                                   \
                new resultType(                                    \
                    chemistry().funcName()                            \
                )                                                  \
            )                                                      \
        );                                                         \
    }                                                              \
};                                                                 \
defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction_ ## funcName,0);  \
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakPsiChemistryModelPluginFunction_ ## funcName,name,psiChem_ ## funcName);

concreteChemistryFunction(tc,volScalarField);
concreteChemistryFunction(Sh,volScalarField);
concreteChemistryFunction(dQ,volScalarField);

class swakPsiChemistryModelPluginFunction_RR
: public swakPsiChemistryModelPluginFunction
{
    word speciesName_;

public:
    TypeName("swakPsiChemistryModelPluginFunction_RR");
    swakPsiChemistryModelPluginFunction_RR (
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    ): swakPsiChemistryModelPluginFunction(
        parentDriver,
        name,
        "volScalarField",
        "speciesName primitive word"
    ) {}

    void doEvaluation() {
        label specI=chemistry().thermo().composition().species()[speciesName_];

        result().setObjectResult(
            autoPtr<volScalarField>(
#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
                wrapDimField(
                    chemistry().RR(specI)
                ).ptr()
#else
                new volScalarField(
                    chemistry().RR(specI)
                )
#endif
            )
        );
    }

    void setArgument(label index,const word &s) {
        assert(index==0);

        speciesName_=s;
    }
};
defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction_RR,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakPsiChemistryModelPluginFunction_RR,name,psiChem_RR);

class swakPsiChemistryModelPluginFunction_RRError
: public swakPsiChemistryModelPluginFunction
{
public:
    TypeName("swakPsiChemistryModelPluginFunction_RRError");
    swakPsiChemistryModelPluginFunction_RRError (
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    ): swakPsiChemistryModelPluginFunction(
        parentDriver,
        name,
        "volScalarField"
    ) {}

    void doEvaluation() {
        autoPtr<volScalarField> pSum(
            new volScalarField(
#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
                0*wrapDimField(chemistry().RR(0))
#else
                0*chemistry().RR(0)
#endif
            )
        );

        volScalarField &summe=pSum();
        for(
            label specI=0;
            specI<chemistry().thermo().composition().species().size();
            specI++
        ) {
#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
            summe+=wrapDimField(chemistry().RR(specI));
#else
            summe+=chemistry().RR(specI);
#endif
        }

        result().setObjectResult(
            pSum
        );
    }
};

defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction_RRError,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakPsiChemistryModelPluginFunction_RRError,name,psiChem_RRError);

class swakPsiChemistryModelPluginFunction_RRSumPositive
: public swakPsiChemistryModelPluginFunction
{
public:
    TypeName("swakPsiChemistryModelPluginFunction_RRSumPositive");
    swakPsiChemistryModelPluginFunction_RRSumPositive (
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    ): swakPsiChemistryModelPluginFunction(
        parentDriver,
        name,
        "volScalarField"
    ) {}

    void doEvaluation() {
        autoPtr<volScalarField> pSum(
            new volScalarField(
#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
                0*wrapDimField(chemistry().RR(0))
#else
                0*chemistry().RR(0)
#endif
            )
        );

        volScalarField &summe=pSum();
        for(
            label specI=0;
            specI<chemistry().thermo().composition().species().size();
            specI++
        ) {
#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
            const volScalarField &RR=wrapDimField(chemistry().RR(specI));
#else
            const volScalarField &RR=chemistry().RR(specI);
#endif
            forAll(summe,cellI) {
                if(RR[cellI]>0) {
                    summe[cellI]+=RR[cellI];
                }
            }
        }
        summe.correctBoundaryConditions();

        result().setObjectResult(
            pSum
        );
    }
};

defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction_RRSumPositive,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakPsiChemistryModelPluginFunction_RRSumPositive,name,psiChem_RRSumPositive);

class swakPsiChemistryModelPluginFunction_updateChemistry
: public swakPsiChemistryModelPluginFunction
{
    scalar timeStep_;

public:
    TypeName("swakPsiChemistryModelPluginFunction_updateChemistry");
    swakPsiChemistryModelPluginFunction_updateChemistry (
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    ): swakPsiChemistryModelPluginFunction(
        parentDriver,
        name,
        "volScalarField",
        "timestep primitive scalar"
    ) {}

    void doEvaluation() {
        updateChemistry(timeStep_);

        result().setObjectResult(
            autoPtr<volScalarField>(
                new volScalarField(
                    IOobject(
                        "dummyField",
                        mesh().time().timeName(),
                        mesh(),
                        IOobject::NO_READ,
                        IOobject::NO_WRITE
                    ),
                    mesh(),
                    dimensionedScalar("dtChem",dimless,0),
                    "zeroGradient"
                )
            )
        );
    }

    void setArgument(label index,const scalar &s) {
        assert(index==0);

        timeStep_=s;
    }
};
defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction_updateChemistry,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakPsiChemistryModelPluginFunction_updateChemistry,name,psiChem_updateChemistry);

class swakPsiChemistryModelPluginFunction_deltaTChem
: public swakPsiChemistryModelPluginFunction
{
public:
    TypeName("swakPsiChemistryModelPluginFunction_deltaTChem");
    swakPsiChemistryModelPluginFunction_deltaTChem (
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    ): swakPsiChemistryModelPluginFunction(
        parentDriver,
        name,
        "volScalarField"
    ) {}

    void doEvaluation() {
#ifdef FOAM_DELTATCHEM_NOT_DIMENSIONED
        const scalarField &dtChem=chemistry().deltaTChem();
#else
        const DimensionedField<scalar,volMesh> &dtChem=chemistry().deltaTChem();
#endif

        autoPtr<volScalarField> val(
            new volScalarField(
                IOobject(
                    "deltaTChem",
                    mesh().time().timeName(),
                    mesh(),
                    IOobject::NO_READ,
                    IOobject::NO_WRITE
                ),
                mesh(),
#ifdef FOAM_DELTATCHEM_NOT_DIMENSIONED
                dimensionedScalar("dtChem",dimless,0),
#else
                dimensionedScalar("dtChem",dtChem.dimensions(),0),
#endif
                "zeroGradient"
            )
        );
#ifdef FOAM_DELTATCHEM_NOT_DIMENSIONED
        val->internalField()=dtChem;
#else
        val->dimensionedInternalField()=dtChem;
#endif

        result().setObjectResult(
            val
        );
    }
};
defineTypeNameAndDebug(swakPsiChemistryModelPluginFunction_deltaTChem,0);
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakPsiChemistryModelPluginFunction_deltaTChem,name,psiChem_deltaTChem);

} // namespace

// ************************************************************************* //
