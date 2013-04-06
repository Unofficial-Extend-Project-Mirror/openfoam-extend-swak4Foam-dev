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
    Foam::DelayedExpressionResult

Description

SourceFiles
    DelayedExpressionResultI.H
    DelayedExpressionResult.C
    DelayedExpressionResultIO.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef DelayedExpressionResult_H
#define DelayedExpressionResult_H

#include "ExpressionResult.H"
#include "Tuple2.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class DelayedExpressionResult Declaration
\*---------------------------------------------------------------------------*/

class DelayedExpressionResult
:
    public ExpressionResult
{
    // Private data

    word name_;

    string startupValueExpression_;

    //- the result that is going to be set
    ExpressionResult settingResult_;

    typedef Tuple2<scalar,ExpressionResult> ValueAtTime;

    //- the old results
    DLList<ValueAtTime> storedValues_;

    //- how often should the values be stored
    scalar storeInterval_;

    //- how big is the delay
    scalar delay_;

protected:

    inline ExpressionResult &toBeSet();

public:

     //- Runtime type information
    TypeName("DelayedExpressionResult");

   // Constructors

    //- Construct null
    DelayedExpressionResult();

    //- Construct as copy
    DelayedExpressionResult(const DelayedExpressionResult&);

    //- Construct from a dictionary
    DelayedExpressionResult(const dictionary&);

    // Destructor

    virtual ~DelayedExpressionResult();

    void operator=(const DelayedExpressionResult&);

    void operator=(const ExpressionResult&);

    const word name() const { return name_; }

    const string &startupValueExpression() const
        {
            return startupValueExpression_;
        }

    //- update the read-value

    //- returns true if there was a valid value.
    //- this does not do the work of setReadValue because we have no
    //- access to the Parser
    bool updateReadValue(const scalar &time);

    //- setting the readValue with a calculated value
    void setReadValue(const ExpressionResult &val);

    //- add a stored value
    void storeValue(const scalar &time);

    friend Ostream & operator<<(Ostream &,const DelayedExpressionResult &);

    friend Istream & operator>>(Istream &,DelayedExpressionResult &);

    friend bool operator!=(
        const DelayedExpressionResult &,const DelayedExpressionResult &
    );
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "DelayedExpressionResultI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
