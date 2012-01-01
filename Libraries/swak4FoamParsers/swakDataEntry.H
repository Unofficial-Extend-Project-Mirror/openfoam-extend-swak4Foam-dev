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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef swakDataEntry_H
#define swakDataEntry_H

#include "foamVersion4swak.H"

#include "CommonValueExpressionDriver.H"

#include "DataEntry.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

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
    dictionary data_;

    //- the actual driver for the Expressions
    autoPtr<CommonValueExpressionDriver> driver_;

    //- the expression that should be evaluated
    string expression_;

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
        swakDataEntry(const word& entryName, const dictionary& dict);

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

//         //- Integrate between two values
//         Type integrate(const scalar x1, const scalar x2) const;


    // I/O

        //- Ostream Operator
        friend Ostream& operator<< <Type>
        (
            Ostream& os,
            const swakDataEntry<Type>& cnst
        );

#if (FOAM_VERSION4SWAK_MAJOR>=2) && (FOAM_VERSION4SWAK_MINOR>0)
        //- Write in dictionary format
        virtual void writeData(Ostream& os) const;
#endif

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "swakDataEntry.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


#endif

// ************************************************************************* //
