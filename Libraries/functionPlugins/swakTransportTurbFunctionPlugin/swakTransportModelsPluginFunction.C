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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakTransportModelsPluginFunction.H"
#include "FieldValueExpressionDriver.H"

#include "HashPtrTable.H"
#include "singlePhaseTransportModel.H"
#include "addToRunTimeSelectionTable.H"

namespace Foam {

defineTypeNameAndDebug(swakTransportModelsPluginFunction,0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

swakTransportModelsPluginFunction::swakTransportModelsPluginFunction(
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

const transportModel &swakTransportModelsPluginFunction::transportInternal(
    const fvMesh &reg
)
{
    static HashPtrTable<transportModel> trans_;

    if(reg.foundObject<transportModel>("transportProperties")) {
        if(debug) {
            Info << "swakTransportModelsPluginFunction::transportInternal: "
                << "already in memory" << endl;
        }
        // Somebody else already registered this
        return reg.lookupObject<transportModel>("transportProperties");
    }
    if(!trans_.found(reg.name())) {
        if(debug) {
            Info << "swakTransportModelsPluginFunction::transportInternal: "
                << "not yet in memory for " << reg.name() << endl;
        }


        // Create it ourself because nobody registered it
        trans_.set(
            reg.name(),
            new singlePhaseTransportModel(
                reg.lookupObject<volVectorField>("U"),
                reg.lookupObject<surfaceScalarField>("phi")
            )
        );
    }

    return *(trans_[reg.name()]);
}

const transportModel &swakTransportModelsPluginFunction::transport()
{
    return transportInternal(mesh());
}

// * * * * * * * * * * * * * * * Concrete implementations * * * * * * * * * //

#define concreteTransportFunction(funcName,resultType)                \
class swakTransportModelsPluginFunction_ ## funcName                \
: public swakTransportModelsPluginFunction                          \
{                                                                  \
public:                                                            \
    TypeName("swakTransportModelsPluginFunction_" #funcName);       \
    swakTransportModelsPluginFunction_ ## funcName (                \
        const FieldValueExpressionDriver &parentDriver,            \
        const word &name                                           \
    ): swakTransportModelsPluginFunction(                           \
        parentDriver,                                              \
        name,                                                      \
        #resultType                                                \
    ) {}                                                           \
    void doEvaluation() {                                          \
        result().setObjectResult(                                  \
            autoPtr<resultType>(                                   \
                new resultType(                                    \
                    transport().funcName()                         \
                )                                                  \
            )                                                      \
        );                                                         \
    }                                                              \
};                                                                 \
defineTypeNameAndDebug(swakTransportModelsPluginFunction_ ## funcName,0);  \
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakTransportModelsPluginFunction_ ## funcName,name,trans_ ## funcName);

concreteTransportFunction(nu,volScalarField);

} // namespace

// ************************************************************************* //
