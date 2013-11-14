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
    swakExpressionFunctionObject

Description
    Can evaluate any swak-expression that can be produced by runTime-selection

SourceFiles
    swakExpressionFunctionObject.C

Contributors/Copyright:
    2010-2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef swakExpressionFunctionObject_H
#define swakExpressionFunctionObject_H

#include "timelineFunctionObject.H"

#include "CommonValueExpressionDriver.H"

#include "NumericAccumulationNamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class swakExpressionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class swakExpressionFunctionObject
:
    public timelineFunctionObject
{

    // Private Member Functions

    //- Disallow default bitwise copy construct
    swakExpressionFunctionObject(const swakExpressionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const swakExpressionFunctionObject&);

    //- evaluate the expression
    template <class T>
    Field<T> evaluate(const word& fieldName,T preset) const;

    //- the expression that is to be evaluated
    string expression_;

    //- the mask expression
    string maskExpression_;

    //- list with the accumulations of evaluated expression
    List<NumericAccumulationNamedEnum::accuSpecification> accumulations_;

    //- the expression driver
    autoPtr<CommonValueExpressionDriver> driver_;
protected:

    word dirName();

    stringList columnNames();

    wordList fileNames();

    template<class T>
    void writeTheData(CommonValueExpressionDriver &driver);
public:

    //- Runtime type information
    TypeName("swakExpression");


    // Constructors

    //- Construct from components
    swakExpressionFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    //- Do the writing
    virtual void write();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "swakExpressionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
