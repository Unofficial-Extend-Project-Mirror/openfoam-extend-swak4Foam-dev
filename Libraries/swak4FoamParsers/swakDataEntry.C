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
    independentVariableName_=word(data_.lookup("independentVariableName"));
}


template<class Type>
Foam::swakDataEntry<Type>::swakDataEntry(const swakDataEntry<Type>& cnst)
:
    DataEntry<Type>(cnst),
    data_(cnst.data_),
    //    driver_(cnst.driver_->clone()),
    expression_(cnst.expression_),
    independentVariableName_(cnst.independentVariableName_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
Foam::swakDataEntry<Type>::~swakDataEntry()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
Foam::CommonValueExpressionDriver &Foam::swakDataEntry<Type>::driver()
{
    if(!driver_.valid()) {
        driver_=CommonValueExpressionDriver::New(
            data_
        );
    }

    return driver_();
}

template<class Type>
Type Foam::swakDataEntry<Type>::value(const scalar x) const
{
    CommonValueExpressionDriver &theDriver=const_cast<swakDataEntry<Type> &>(
        *this
    ).driver();

    theDriver.clearVariables();
    theDriver.addUniformVariable(
        independentVariableName_,
        x
    );

    return theDriver.evaluateUniform<Type>(expression_);
}

template<class Type>
Type Foam::swakDataEntry<Type>::integrate(const scalar x1,const scalar x2) const
{
    CommonValueExpressionDriver &theDriver=const_cast<swakDataEntry<Type> &>(
        *this
    ).driver();

    theDriver.clearVariables();
    label intervalls=readLabel(data_.lookup("integrationIntervalls"));
    scalar dx=(x2-x1)/intervalls;

    scalar x=x1;
    theDriver.addUniformVariable(
        independentVariableName_,
        x
    );
    Type valOld=theDriver.evaluateUniform<Type>(expression_);
    Type result=pTraits<Type>::zero;
    while((x+SMALL)<x2) {
        x+=dx;
        theDriver.addUniformVariable(
            independentVariableName_,
            x
        );
        Type valNew=theDriver.evaluateUniform<Type>(expression_);
        result+=0.5*(valOld+valNew)*dx;
        valOld=valNew;
    }
    // Info << "Integrate " << expression_ << " from " << x1 << " to "
    //     << x2 << " -> " << result << endl;

    return result;
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

#if (FOAM_VERSION4SWAK_MAJOR>=2) && (FOAM_VERSION4SWAK_MINOR>0)
template<class Type>
void Foam::swakDataEntry<Type>::writeData(Ostream& os) const
{
    DataEntry<Type>::writeData(os);

    os  << token::SPACE;
    data_.write(os,true);
    os  << token::END_STATEMENT << nl;
}
#endif

// ************************************************************************* //
