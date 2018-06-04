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
    Foam::findShiftedFieldPluginFunction

Description

SourceFiles
    findShiftedFieldPluginFunction.C

Contributors/Copyright:
    2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef findShiftedFieldPluginFunction_H
#define findShiftedFieldPluginFunction_H

#include "FieldValuePluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class findShiftedFieldPluginFunction Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class findShiftedFieldPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const findShiftedFieldPluginFunction &);

    findShiftedFieldPluginFunction(const findShiftedFieldPluginFunction &);

protected:

    typedef GeometricField<Type,fvPatchField,volMesh> ResultType;

    // this data has to be initialized by initFacesAndCells

    void doEvaluation();

    void setArgument(
        label index,
        const string &content,
        const CommonValueExpressionDriver &driver
    );

    Type defaultValue_;

private:

    autoPtr<ResultType> field_;
    autoPtr<ResultType> default_;
    autoPtr<volVectorField> shift_;

public:
    findShiftedFieldPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~findShiftedFieldPluginFunction() {}

    TypeName("findShiftedFieldPluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
