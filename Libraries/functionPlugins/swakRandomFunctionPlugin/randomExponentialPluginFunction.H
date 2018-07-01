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
    Foam::randomExponentialPluginFunction

Description

SourceFiles
    randomExponentialPluginFunction.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef randomExponentialPluginFunction_H
#define randomExponentialPluginFunction_H

#include "CommonPluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class randomExponentialPluginFunction Declaration
\*---------------------------------------------------------------------------*/

template <typename FType,typename DType>
class randomExponentialPluginFunction
:
    public FType
{
    //- Disallow default bitwise assignment
    void operator=(const randomExponentialPluginFunction &);

    randomExponentialPluginFunction(const randomExponentialPluginFunction &);

    //- the halfLife of the exponential distribution
    scalar halfLife_;

    //- the seed for the random number generator
    label seed_;

    //- do the actual compuation
    void doEvaluationInternal(scalarField &f);

public:
    randomExponentialPluginFunction(
        const DType &parentDriver,
        const word &name
    );

    virtual ~randomExponentialPluginFunction() {}

    TypeName("randomExponentialPluginFunction");

    void doEvaluation();

    void setArgument(label index,const scalar &);

    void setArgument(label index,const label &);
};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
