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
    swakExpressionFunctionObject

Description
    Can evaluate any swak-expression that can be produced by runTime-selection

SourceFiles
    swakExpressionFunctionObject.C

Contributors/Copyright:
    2010-2011, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
    exprString expression_;

    //- the mask expression
    exprString maskExpression_;

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
    virtual void writeSimple();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "swakExpressionFunctionObjectTemplates.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
