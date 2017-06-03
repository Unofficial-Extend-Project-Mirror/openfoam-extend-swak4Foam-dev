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
    Foam::lcsSpeciesSourcePluginFunction

Description

SourceFiles
    lcsSpeciesSourcePluginFunction.C

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef lcsSpeciesSourcePluginFunction_H
#define lcsSpeciesSourcePluginFunction_H

#include "LagrangianCloudSourcePluginFunction.H"

#include "swakCloudTypes.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class lcsSpeciesSourcePluginFunction Declaration
\*---------------------------------------------------------------------------*/

class lcsSpeciesSourcePluginFunction
:
    public LagrangianCloudSourcePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const lcsSpeciesSourcePluginFunction &);

    lcsSpeciesSourcePluginFunction(const lcsSpeciesSourcePluginFunction &);

protected:
    word speciesName_;

    typedef DimensionedField<scalar,volMesh> dimScalarField;

    virtual autoPtr<dimScalarField> internalEvaluate(const label index);

    virtual label getIndex(wordList &specList);
public:

    lcsSpeciesSourcePluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~lcsSpeciesSourcePluginFunction() {}

    TypeName("lcsSpeciesSourcePluginFunction");

    void doEvaluation();

    void setArgument(label index,const word &);

};

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#define getSpeciesIndex(neededType,usedType)                                \
    if(hasCloudAs<neededType>()) {                                      \
        const speciesTable &spec=                                       \
            getCloudAs<neededType,usedType>().mcCarrierThermo ().species(); \
        speciesList=spec;                                               \
        if(spec.contains(speciesName_)) {                               \
            return spec[speciesName_];                                  \
        }                                                               \
    }
#else
#define getSpeciesIndex(neededType,usedType)                            \
    if(hasCloudAs<neededType>()) {                                      \
        const speciesTable &spec=                                       \
            getCloudAs<neededType,usedType>().thermo().carrier().species(); \
        speciesList=spec;                                               \
        if(spec.contains(speciesName_)) {                               \
            return spec[speciesName_];                                  \
        }                                                               \
    }
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
