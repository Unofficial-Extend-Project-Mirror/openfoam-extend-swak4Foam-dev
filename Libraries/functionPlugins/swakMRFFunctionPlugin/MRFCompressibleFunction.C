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
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "MRFCompressibleFunction.H"

#ifdef FOAM_HAS_IOMRFLIST

#include "FieldValueExpressionDriver.H"

#include "addToRunTimeSelectionTable.H"

#include "fvc.H"

namespace Foam {


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class FieldType>
MRFCompressibleFunction<FieldType>::MRFCompressibleFunction(
    const FieldValueExpressionDriver &parentDriver,
    const word &name
):
    swakMRFPluginFunctionBasis(
        parentDriver,
        name,
        FieldType::typeName,
        string(
            "rho internalField "+FieldType::typeName+
            ",U internalField "+FieldType::typeName
        )
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class FieldType>
void MRFCompressibleFunction<FieldType>::setArgument(
    label index,
    const string &content,
    const CommonValueExpressionDriver &driver
) {
    assert(index==0 || index==1);

    if(index==0) {
        rho_.set(
            new FieldType(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<FieldType>()
            )
        );
    } else {
        field_.set(
            new FieldType(
                dynamic_cast<const FieldValueExpressionDriver &>(
                    driver
                ).getResult<FieldType>()
            )
        );
    }
}

template<class FieldType>
void MRFCompressibleFunction<FieldType>::doEvaluation()
{
    autoPtr<FieldType> pResult(
        new FieldType(
            IOobject(
                "MRFResult",
                mesh().time().timeName(),
                mesh(),
                IOobject::NO_READ,
                IOobject::NO_WRITE
            ),
            field_()
        )
    );
    FieldType &Result=pResult();

    this->manipulate(Result);

    result().setObjectResult(pResult);
}

// * * * * * * * * * * * * * * * Concrete implementations  * * * * * * * * * * * * * //

#define concreteMRF(fName,FType,methodName)             \
class swakMRFPluginFunction_ ## fName ## _comp                      \
: public MRFCompressibleFunction<FType>                                 \
{                                                                       \
public:                                                                 \
    TypeName("swakMRFPluginFunction_" # fName  "_comp");                \
    swakMRFPluginFunction_ ## fName ## _comp (                          \
        const FieldValueExpressionDriver &parentDriver,                 \
        const word &name                                                \
    ): MRFCompressibleFunction<FType> (                                 \
        parentDriver,                                                   \
        name                                                            \
    ) {}                                                                \
    void manipulate(FType &field) {                                     \
        this->MRF().methodName(rho_(),field);                           \
    }                                                                   \
};                                                                      \
defineTypeNameAndDebug(swakMRFPluginFunction_ ## fName ## _comp,0);     \
addNamedToRunTimeSelectionTable(FieldValuePluginFunction,swakMRFPluginFunction_ ## fName ## _comp,name,MRF_ ## fName ## _compressible)

#ifdef FOAM_MRF_NEW_METHOD_NAME
concreteMRF(makeAbsoluteSurf,surfaceScalarField,makeAbsolute);
concreteMRF(makeRelativeSurf,surfaceScalarField,makeRelative);
#else
concreteMRF(makeAbsoluteSurf,surfaceScalarField,absoluteFlux);
concreteMRF(makeRelativeSurf,surfaceScalarField,relativeFlux);
#endif

} // namespace

#endif // FOAM_HAS_IOMRFLIST

// ************************************************************************* //
