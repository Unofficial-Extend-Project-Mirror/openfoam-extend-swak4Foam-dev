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
    Foam::randomExponentialPluginFunction

Description

SourceFiles
    randomExponentialPluginFunction.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
