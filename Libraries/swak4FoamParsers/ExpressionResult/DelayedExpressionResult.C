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

#include "DelayedExpressionResult.H"
#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#include <cassert>

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

DelayedExpressionResult::DelayedExpressionResult()
:
    ExpressionResult(),
    name_("None"),
    startupValueExpression_(""),
    settingResult_(),
    storeIntervall_(1),
    delay_(10)
{
}

DelayedExpressionResult::DelayedExpressionResult(
    const DelayedExpressionResult &rhs
)
:
    ExpressionResult(rhs),
    name_(rhs.name_),
    startupValueExpression_(rhs.startupValueExpression_),
    settingResult_(rhs.settingResult_),
    storedValues_(rhs.storedValues_),
    storeIntervall_(rhs.storeIntervall_),
    delay_(rhs.delay_)
{
}

DelayedExpressionResult::DelayedExpressionResult(const dictionary &dict)
:
    ExpressionResult(dict.subOrEmptyDict("value")),
    name_(dict.lookup("name")),
    startupValueExpression_(dict.lookup("startupValue")),
    storeIntervall_(readScalar(dict.lookup("storeIntervall"))),
    delay_(readScalar(dict.lookup("delay")))
{
    if(dict.found("storedValues")) {
        storedValues_=DLList<ValueAtTime>(
            dict.lookup("storedValues")
        );
    }
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

DelayedExpressionResult::~DelayedExpressionResult()
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //

void DelayedExpressionResult::operator=(const DelayedExpressionResult& rhs)
{
    // Check for assignment to self
    if (this == &rhs)
    {
        FatalErrorIn("DelayedExpressionResult::operator=(const DelayedExpressionResult&)")
            << "Attempted assignment to self"
            << exit(FatalError);
    }

    static_cast<ExpressionResult&>(*this)=rhs;

    name_=rhs.name_;
    startupValueExpression_=rhs.startupValueExpression_;
    settingResult_=rhs.settingResult_;
    storedValues_=rhs.storedValues_;
    storeIntervall_=rhs.storeIntervall_;
    delay_=rhs.delay_;
}

void DelayedExpressionResult::operator=(const ExpressionResult& rhs)
{
    settingResult_=rhs;
}

bool DelayedExpressionResult::updateReadValue(const scalar &time)
{
    if(storedValues_.size()<=0) {
        // no shirt, no data, no service
        return false;
    }
    DLList<ValueAtTime>::const_iterator last=storedValues_.end();
    if(last().first()>(time-delay_)) {
        // we have no matching data yet
        return false;
    }
    if(storedValues_.size()<=1) {
        FatalErrorIn("DelayedExpressionResult::updateReadValue(const scalar &time)")
            << "Only one stored value yet at time " << time
                << " for delayedVariable " << name() << endl
                << "Check the values for the intervall " << storeIntervall_
                << " and delay " << delay_ << endl
                << "Probably the intervall is to big"
                << endl
                << exit(FatalError);

    }
    DLList<ValueAtTime>::const_iterator current=storedValues_.cbegin();
    DLList<ValueAtTime>::const_iterator next=current;
    ++next;
    while(next!=last) {
        if(
            time>=current().first()
            &&
            time<=next().first()
        ) {
            break;
        }
        current=next;
        ++next;
    }

    assert(current!=next);
    assert(time>=current().first() && time<=next().first());

    const scalar step=next().first()-current().first();
    const scalar f=(time-next().first())/step;

    ExpressionResult val((1-f)*current().second()+f*next().second());

    (*this)=val;

    return true;
}

void DelayedExpressionResult::setReadValue(const ExpressionResult &val)
{
    static_cast<ExpressionResult&>(*this)=val;
}

void DelayedExpressionResult::storeValue(const scalar &time)
{
    bool append=false;

    if(storedValues_.size()<=0) {
        append=true;
    } else {
        const scalar lastTime=storedValues_.last().first();
        if(lastTime<=(time+SMALL)) {
            // basically the same. Fall through to replace
        } else if((time-lastTime)>=0.999*storeIntervall_) {
            append=true;
        } else {
            // we're in the middle. Forget it
            return;
        }
    }
    if(append) {
        storedValues_.append(ValueAtTime(time,settingResult_));
    } else {
        storedValues_.last().second()=settingResult_;
    }
}

// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //

Ostream & operator<<(Ostream &out,const DelayedExpressionResult &data)
{
    out << token::BEGIN_BLOCK << endl;

    out.writeKeyword("name");
    out << word(data.name_) << token::END_STATEMENT << nl;

    out.writeKeyword("startupValue");
    out << data.startupValueExpression_ << token::END_STATEMENT << nl;

    out.writeKeyword("settingResult");
    out << data.settingResult_ << token::END_STATEMENT << nl;

    out.writeKeyword("storedValues");
    out << data.storedValues_ << token::END_STATEMENT << nl;

    out.writeKeyword("storeIntervall");
    out << data.storeIntervall_ << token::END_STATEMENT << nl;

    out.writeKeyword("delay");
    out << data.delay_ << token::END_STATEMENT << nl;

    out.writeKeyword("value");
    out << static_cast<const ExpressionResult &>(data);

    out << token::END_BLOCK << endl;

    return out;
}

Istream & operator>>(Istream &in,DelayedExpressionResult &data)
{
    dictionary dict(in);

    data=DelayedExpressionResult(dict);

    return in;
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

bool operator!=(const DelayedExpressionResult &,const DelayedExpressionResult &)
{
    return false;
}

} // namespace

// ************************************************************************* //
