/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           |
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
    Foam::stepsSinceChangePluginFunction

Description
    Number of steps since the state machine last changed its state

SourceFiles
    stepsSinceChangePluginFunction.C

Contributors/Copyright:
    2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef stepsSinceChangePluginFunction_H
#define stepsSinceChangePluginFunction_H

#include "FieldValuePluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class stepsSinceChangePluginFunction Declaration
\*---------------------------------------------------------------------------*/

template <class PluginType>
class stepsSinceChangePluginFunction
:
    public PluginType
{
    //- Disallow default bitwise assignment
    void operator=(const stepsSinceChangePluginFunction &);

    stepsSinceChangePluginFunction(const stepsSinceChangePluginFunction &);

    word machineName_;

protected:

    void setArgument(
        label index,
        const word &value
    );

public:

    stepsSinceChangePluginFunction(
        const typename PluginType::driverType &parentDriver,
        const word &name
    );

    virtual ~stepsSinceChangePluginFunction() {}

    TypeName("stepsSinceChangePluginFunction");

    void doEvaluation();

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
