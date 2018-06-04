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
    patchExpressionDistributionFunctionObject

Description
    Distribution of an expression on a patch

SourceFiles
    patchExpressionDistributionFunctionObject.C

Contributors/Copyright:
    2008-2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef patchExpressionDistributionFunctionObject_H
#define patchExpressionDistributionFunctionObject_H

#include "distributionFunctionObject.H"
#include "PatchValueExpressionDriver.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class patchExpressionDistributionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class patchExpressionDistributionFunctionObject
:
    public distributionFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    patchExpressionDistributionFunctionObject(const patchExpressionDistributionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const patchExpressionDistributionFunctionObject&);

    wordList patchNames_;

    labelList patchIDs_;

    template <typename T>
    void getDistributionInternal(autoPtr<SimpleDistribution<T> > &dist);
protected:

    word dirName();

    virtual word baseName();

    virtual void getDistribution();

    //- the expression that is to be evaluated
    exprString expression_;

    //- the specification
    dictionary data_;

    PtrList<PatchValueExpressionDriver> drivers_;

public:

    //- Runtime type information
    TypeName("patchExpressionDistribution");


    // Constructors

    //- Construct from components
    patchExpressionDistributionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "patchExpressionDistributionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
