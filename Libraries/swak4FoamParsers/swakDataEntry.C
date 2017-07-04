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

Contributors/Copyright:
    2011, 2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakDataEntry.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

namespace Foam {

#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
namespace Function1Types {
#endif

template<class Type>
swakDataEntry<Type>::swakDataEntry(
    const word& entryName,
    const dictionary& dict,
    bool readEntryType
)
:
    DataEntry<Type>(entryName)
{
    if(readEntryType) {
        Istream& is(dict.lookup(entryName));
        word entryType(is);

        dictionary d(is);
        data_.set(
            new dictionary(
                dict,
                d
            )
        );
    } else {
        data_.set(
            new dictionary(
                dict,
                dict.subDict(entryName)
            )
        );
    }
    expression_=exprString(
        data_->lookup("expression"),
        data_()
    );
    independentVariableName_=word(data_->lookup("independentVariableName"));
}


template<class Type>
swakDataEntry<Type>::swakDataEntry(const swakDataEntry<Type>& cnst)
:
    DataEntry<Type>(cnst),
    data_(new dictionary(cnst.data_())),
    //    driver_(cnst.driver_->clone()),
    expression_(cnst.expression_),
    independentVariableName_(cnst.independentVariableName_)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class Type>
swakDataEntry<Type>::~swakDataEntry()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class Type>
CommonValueExpressionDriver &swakDataEntry<Type>::driver()
{
    if(!driver_.valid()) {
        driver_=CommonValueExpressionDriver::New(
            data_()
        );
        driver_->createWriterAndRead(
            "dataEntry_"+data_->name().name()+"_"+this->name()
        );
    }

    return driver_();
}

template<class Type>
Type swakDataEntry<Type>::value(const scalar x) const
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
Type swakDataEntry<Type>::integrate(const scalar x1,const scalar x2) const
{
    CommonValueExpressionDriver &theDriver=const_cast<swakDataEntry<Type> &>(
        *this
    ).driver();

    theDriver.clearVariables();
    label intervalls=readLabel(data_->lookup("integrationIntervalls"));
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
Ostream& operator<<
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
    cnst.data_->write(os,true);

    // Check state of Ostream
    os.check
    (
        "Ostream& operator<<(Ostream&, const swakDataEntry<Type>&)"
    );

    return os;
}

#ifdef FOAM_DATAENTRY_CLASS_NEEDS_WRITEDATA
template<class Type>
void swakDataEntry<Type>::writeData(Ostream& os) const
{
    DataEntry<Type>::writeData(os);

    os  << token::SPACE;
    data_->write(os,true);
    os  << token::END_STATEMENT << nl;
}
#endif

#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
}
#endif

}

// ************************************************************************* //
