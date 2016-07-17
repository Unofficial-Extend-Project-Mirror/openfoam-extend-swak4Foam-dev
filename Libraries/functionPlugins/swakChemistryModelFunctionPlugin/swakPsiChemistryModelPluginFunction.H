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
    Foam::swakPsiChemistryModelPluginFunction

Description

SourceFiles
    swakPsiChemistryModelPluginFunction.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakPsiChemistryModelPluginFunction_H
#define swakPsiChemistryModelPluginFunction_H

#include "FieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"

#include "psiChemistryModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class swakPsiChemistryModelPluginFunction Declaration
\*---------------------------------------------------------------------------*/
class swakPsiChemistryModelPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const swakPsiChemistryModelPluginFunction &);

    swakPsiChemistryModelPluginFunction(const swakPsiChemistryModelPluginFunction &);

protected:
    static const psiChemistryModel &chemistryInternal(const fvMesh &reg);

    const psiChemistryModel &chemistry();

    void updateChemistry(const scalar dt);

#ifdef FOAM_RR_ONLY_DIMENSIONED_FIELD
    tmp<volScalarField> wrapDimField(
        const DimensionedField<scalar,volMesh> &dimField
    );
#endif

public:
    swakPsiChemistryModelPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const word &returnValueType,
        const string &spec=""
    );

    virtual ~swakPsiChemistryModelPluginFunction() {}

    TypeName("swakPsiChemistryModelPluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
