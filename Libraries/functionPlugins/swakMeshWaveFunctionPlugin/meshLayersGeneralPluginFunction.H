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
    Foam::meshLayersGeneralPluginFunction

Description

SourceFiles
    meshLayersGeneralPluginFunction.C

Contributors/Copyright:
    2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef meshLayersGeneralPluginFunction_H
#define meshLayersGeneralPluginFunction_H

#include "FieldValuePluginFunction.H"

#include "MeshLayersDistFromPatch.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class meshLayersGeneralPluginFunction Declaration
\*---------------------------------------------------------------------------*/
class meshLayersGeneralPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const meshLayersGeneralPluginFunction &);

    meshLayersGeneralPluginFunction(const meshLayersGeneralPluginFunction &);

protected:

    // this data has to be initialized by initFacesAndCells
    List<MeshLayersDistFromPatch> cellValues_;
    List<MeshLayersDistFromPatch> faceValues_;
    labelList startFaces_;
    List<MeshLayersDistFromPatch> startValues_;

    void doEvaluation();

    virtual void initFacesAndCells() = 0;

public:
    meshLayersGeneralPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const string &description
    );

    virtual ~meshLayersGeneralPluginFunction() {}

    TypeName("meshLayersGeneralPluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
