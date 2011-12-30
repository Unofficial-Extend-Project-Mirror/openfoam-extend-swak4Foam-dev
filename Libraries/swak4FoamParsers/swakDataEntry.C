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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "swakDataEntry.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class Type>
Foam::swakDataEntry<Type>::swakDataEntry(const word& entryName, const dictionary& dict)
:
    DataEntry<Type>(entryName)
{
    Istream& is(dict.lookup(entryName));
    word entryType(is);
    
    data_.read(is);

    expression_=string(data_.lookup("expression"));

    driver_=CommonValueExpressionDriver::New(
        data_
    );
}


template<class Type>
Foam::swakDataEntry<Type>::swakDataEntry(const swakDataEntry<Type>& cnst)
:
    DataEntry<Type>(cnst),
    data_(cnst.data_),
    driver_(cnst.driver_->clone()),
    expression_(cnst.expression_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
Foam::swakDataEntry<Type>::~swakDataEntry()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Type Foam::swakDataEntry<Type>::value(const scalar x) const
{
    CommonValueExpressionDriver &driver=const_cast<CommonValueExpressionDriver &>(
        driver_()
    );
    // TODO: set x as a variable
    driver.clearVariables();

    return driver.evaluateUniform<Type>(expression_);
}

// * * * * * * * * * * * * * *  IOStream operators * * * * * * * * * * * * * //


template<class Type>
Foam::Ostream& Foam::operator<<
(
    Ostream& os,
    const swakDataEntry<Type>& cnst
)
{
    if (os.format() == IOstream::ASCII)
    {
        os  << static_cast<const DataEntry<Type>& >(cnst)
            << token::SPACE;
    }
    else
    {
        os  << static_cast<const DataEntry<Type>& >(cnst);
    }
    cnst.data_.write(os,true);

    // Check state of Ostream
    os.check
    (
        "Ostream& operator<<(Ostream&, const swakDataEntry<Type>&)"
    );

    return os;
}


template<class Type>
void Foam::swakDataEntry<Type>::writeData(Ostream& os) const
{
    DataEntry<Type>::writeData(os);

    os  << token::SPACE;
    data_.write(os,true);
    os  << token::END_STATEMENT << nl;
}

// ************************************************************************* //
