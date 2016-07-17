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
    Foam::floodFillGeneralPluginFunction

Description

SourceFiles
    floodFillGeneralPluginFunction.C

Contributors/Copyright:
    2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef floodFillGeneralPluginFunction_H
#define floodFillGeneralPluginFunction_H

#include "FieldValuePluginFunction.H"

#include "FloodFillData.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class floodFillGeneralPluginFunction Declaration
\*---------------------------------------------------------------------------*/
class floodFillGeneralPluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const floodFillGeneralPluginFunction &);

    floodFillGeneralPluginFunction(const floodFillGeneralPluginFunction &);

protected:

    // this data has to be initialized by initFacesAndCells
    List<FloodFillData> cellValues_;
    List<FloodFillData> faceValues_;
    labelList startFaces_;
    List<FloodFillData> startValues_;

    void doEvaluation();

    virtual void initFacesAndCells() = 0;

public:
    floodFillGeneralPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const string &description
    );

    virtual ~floodFillGeneralPluginFunction() {}

    TypeName("floodFillGeneralPluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
