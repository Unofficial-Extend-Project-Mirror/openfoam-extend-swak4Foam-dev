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
    Foam::shiftFieldGeneralPluginFunction

Description

SourceFiles
    shiftFieldGeneralPluginFunction.C

Contributors/Copyright:
    2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef shiftFieldGeneralPluginFunction_H
#define shiftFieldGeneralPluginFunction_H

#include "FieldValuePluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class shiftFieldGeneralPluginFunction Declaration
\*---------------------------------------------------------------------------*/

template<class Type,meshToMeshOrder Order>
class shiftFieldGeneralPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const shiftFieldGeneralPluginFunction &);

    shiftFieldGeneralPluginFunction(const shiftFieldGeneralPluginFunction &);

protected:

    typedef GeometricField<Type,fvPatchField,volMesh> ResultType;

    // this data has to be initialized by initFacesAndCells

    void doEvaluation();

    virtual tmp<pointField> displacement() = 0;

    void setArgument(
        label index,
        const string &content,
        const CommonValueExpressionDriver &driver
    );

    Type defaultValue_;

private:

    autoPtr<ResultType> field_;

public:
    shiftFieldGeneralPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const string &shiftDescription
    );

    virtual ~shiftFieldGeneralPluginFunction() {}

    //    TypeName("shiftFieldGeneralPluginFunction");

};

#ifdef FOAM_MESHTOMESH_HAS_CORRECTEDCELLVOLUMEWEIGHT
#define SHIFT_METHOD meshToMesh::interpolationMethod::imCorrectedCellVolumeWeight
#else
#ifdef FOAM_MESHTOMESH_OLD_STYLE
#define SHIFT_METHOD meshToMesh::INTERPOLATE
#else
#define SHIFT_METHOD meshToMesh::imCellVolumeWeight
#endif
#endif

#ifdef FOAM_MESHTOMESH_OLD_STYLE
#define MAP_METHOD meshToMesh::MAP
#else
#define MAP_METHOD meshToMesh::interpolationMethod::imMapNearest
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
