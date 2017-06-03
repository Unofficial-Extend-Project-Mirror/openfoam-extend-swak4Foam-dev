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
    \\  /    A nd           | Copyright  held by original author
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
    patchExpressionFunctionObject

Description
    Evaluates an expression of the fields on the patches

SourceFiles
    patchExpressionFunctionObject.C

Contributors/Copyright:
    2010-2011, 2013-2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef patchExpressionFunctionObject_H
#define patchExpressionFunctionObject_H

#include "patchFunctionObject.H"

#include "PatchValueExpressionDriver.H"

#include "NumericAccumulationNamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class patchExpressionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class patchExpressionFunctionObject
:
    public patchFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    patchExpressionFunctionObject(const patchExpressionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const patchExpressionFunctionObject&);

    //- integrate single field on all patches
    template <class T>
    Field<T> evaluate(const word& fieldName,T preset) const;

    //- the expression that is to be evaluated
    exprString expression_;

    //- the specification
    dictionary data_;

    //- list with the accumulations of evaluated expression
    List<NumericAccumulationNamedEnum::accuSpecification> accumulations_;

    PtrList<PatchValueExpressionDriver> drivers_;

protected:

    word dirName();

    stringList columnNames();

    wordList fileNames();

    template<class T>
    void writeTheData(const word &pName,PatchValueExpressionDriver &driver);
public:

    //- Runtime type information
    TypeName("patchExpression");


    // Constructors

    //- Construct from components
    patchExpressionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    //- Do the writing
    virtual void writeSimple();

    //- read the stuff
    virtual bool start();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "patchExpressionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
