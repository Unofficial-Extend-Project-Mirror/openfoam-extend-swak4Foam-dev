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
    Foam::swakDataEntry

Description

SourceFiles
    swakDataEntry.C

Contributors/Copyright:
    2011, 2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef swakDataEntry_H
#define swakDataEntry_H

#include "foamVersion4swak.H"

#include "CommonValueExpressionDriver.H"
#include "exprString.H"

#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
#include "Function1.H"
#define DataEntry Function1
#else
#include "DataEntry.H"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
    namespace Function1Types {
#endif

template<class Type>
class swakDataEntry;

template<class Type>
Ostream& operator<<(Ostream&, const swakDataEntry<Type>&);

/*---------------------------------------------------------------------------*\
                           Class swakDataEntry Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class swakDataEntry
:
    public DataEntry<Type>
{
    // Private data

    //- the dictionary with the data (stored for rewriting)
    autoPtr<dictionary> data_;

    //- the actual driver for the Expressions
    autoPtr<CommonValueExpressionDriver> driver_;

    //- the expression that should be evaluated
    exprString expression_;

    //- name of the independent variable
    word independentVariableName_;

    // Private Member Functions

    //- Disallow default bitwise assignment
    void operator=(const swakDataEntry<Type>&);


    //- get (and set if necessary) the driver
    CommonValueExpressionDriver &driver();

public:

    // Runtime type information
    TypeName("swak");


    // Constructors

        //- Construct from entry name and Istream
        swakDataEntry(
            const word& entryName,
            const dictionary& dict,
            bool readEntryType=true
        );

        //- Copy constructor
        swakDataEntry(const swakDataEntry<Type>& cnst);

        //- Construct and return a clone
        virtual tmp<DataEntry<Type> > clone() const
        {
            return tmp<DataEntry<Type> >(new swakDataEntry<Type>(*this));
        }


    //- Destructor
    virtual ~swakDataEntry();


    // Member Functions

        //- Return constant value
        Type value(const scalar) const;

    //- Integrate between two values
    Type integrate(const scalar x1, const scalar x2) const;


    // I/O

        //- Ostream Operator
        friend Ostream& operator<< <Type>
        (
            Ostream& os,
            const swakDataEntry<Type>& cnst
        );

#ifdef FOAM_DATAENTRY_CLASS_NEEDS_WRITEDATA
        //- Write in dictionary format
        virtual void writeData(Ostream& os) const;
#endif

};

#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
    }
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "swakDataEntry.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#endif

// ************************************************************************* //
