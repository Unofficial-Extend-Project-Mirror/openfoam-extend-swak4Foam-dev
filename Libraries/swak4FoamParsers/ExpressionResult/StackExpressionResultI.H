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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

namespace Foam {


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template<class T>
void StackExpressionResult::pushInternal(ExpressionResult &atEnd)
{
    if(debug) {
        Info << "StackExpressionResult::pushInternal(ExpressionResult &atEnd) "
            << pTraits<T>::typeName << endl;
    }
    Field<T> oldValue(getResult<T>(false));
    autoPtr<Field<T> > pNewValue(new Field<T>(oldValue.size()+1));
    Field<T> &newValue=pNewValue();

    forAll(oldValue,i) {
        newValue[i]=oldValue[i];
    }
    newValue[oldValue.size()]=atEnd.getResult<T>(true)()[0];

    if(debug) {
        Info << "New value:" << newValue << endl;
    }

    this->setResult(
        pNewValue.ptr(),
        this->isPoint()
    );
}

template<class T>
ExpressionResult StackExpressionResult::popInternal()
{
    Field<T> oldValue(getResult<T>(false));
    autoPtr<Field<T> > pNewValue(new Field<T>(oldValue.size()-1));
    Field<T> &newValue=pNewValue();

    forAll(newValue,i) {
        newValue[i]=oldValue[i];
    }
    Field<T> result(1,oldValue[newValue.size()]);

    this->setResult(
        pNewValue.ptr(),
        this->isPoint()
    );

    return ExpressionResult(result);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}

// ************************************************************************* //
