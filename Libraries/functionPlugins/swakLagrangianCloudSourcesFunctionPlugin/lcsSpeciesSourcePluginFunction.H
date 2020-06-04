/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    Foam::lcsSpeciesSourcePluginFunction

Description

SourceFiles
    lcsSpeciesSourcePluginFunction.C

Contributors/Copyright:
    2012-2013, 2016-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id: lcsSpeciesSourcePluginFunction.H,v 909e3e73dc26 2018-06-04 10:14:09Z bgschaid $
\*---------------------------------------------------------------------------*/

#ifndef lcsSpeciesSourcePluginFunction_H
#define lcsSpeciesSourcePluginFunction_H

#include "LagrangianCloudSourcePluginFunction.H"

#include "swakCloudTypes.H"

#include "swak.H"

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

#ifdef FOAM_HASH_WORD_LIST_CONTAINS_DEPRECATED
#define foundOrContains found
#else
#define foundOrContains contains
#endif

#ifdef FOAM_REACTINGCLOUD_TEMPLATED
#define getSpeciesIndex(neededType,usedType)                                \
    if(hasCloudAs<neededType>()) {                                      \
        const speciesTable &spec=                                       \
            getCloudAs<neededType,usedType>().mcCarrierThermo ().species(); \
        speciesList=spec;                                               \
        if(spec.foundOrContains(speciesName_)) {                        \
            return spec[speciesName_];                                  \
        }                                                               \
    }
#else
#define getSpeciesIndex(neededType,usedType)                            \
    if(hasCloudAs<neededType>()) {                                      \
        const speciesTable &spec=                                       \
            getCloudAs<neededType,usedType>().thermo().carrier().species(); \
        speciesList=spec;                                               \
        if(spec.foundOrContains(speciesName_)) {                        \
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
