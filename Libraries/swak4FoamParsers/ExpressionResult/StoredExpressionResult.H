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
    Foam::StoredExpressionResult

Description

SourceFiles
    StoredExpressionResultI.H
    StoredExpressionResult.C
    StoredExpressionResultIO.C

Contributors/Copyright:
    2012-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef StoredExpressionResult_H
#define StoredExpressionResult_H

#include "ExpressionResult.H"
#include "swakExprString.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class StoredExpressionResult Declaration
\*---------------------------------------------------------------------------*/

class StoredExpressionResult
:
    public ExpressionResult
{
    // Private data

    word name_;

    exprString initialValueExpression_;

public:

     //- Runtime type information
    TypeName("StoredExpressionResult");

    // Constructors

    //- Construct null
    StoredExpressionResult();

    //- Construct as copy
    StoredExpressionResult(const StoredExpressionResult&);

    //- Construct from a dictionary
    StoredExpressionResult(const dictionary&);

    // Destructor

    virtual ~StoredExpressionResult();

    void operator=(const StoredExpressionResult&);

    void operator=(const ExpressionResult&);

    virtual autoPtr<ExpressionResult> clone() const {
        return autoPtr<ExpressionResult>(
            new StoredExpressionResult(*this)
        );
    }

    const word name() const { return name_; }

    const exprString &initialValueExpression() const
        {
            return initialValueExpression_;
        }

    friend Ostream & operator<<(Ostream &,const StoredExpressionResult &);

    friend Istream & operator>>(Istream &,StoredExpressionResult &);

    friend bool operator!=(
        const StoredExpressionResult &,const StoredExpressionResult &
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "StoredExpressionResultI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
