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

#include "CommonValueExpressionDriver.H"

namespace Foam {


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <typename T>
tmp<T> CommonPluginFunction::evaluate(
    const string &content,
    label &consumed
)
{
    evaluateInternal(content,consumed);

    return tmp<T>(result_.getObjectResult<T>(false));
}

template <typename T>
tmp<Field<T> > CommonPluginFunction::evaluate(
    const string &content,
    label &consumed,
    bool isPoint
)
{
    evaluateInternal(content,consumed);

    if(isPoint!=result_.isPoint()) {
        FatalErrorIn("CommonPluginFunction::evaluate")
            << "Expected 'pointiness' of result " << result_.isPoint()
                << " is not as expected " << isPoint
                << endl
                << exit(FatalError);

    }
    return tmp<Field<T> >(result_.getResult<T>(false));
}

inline CommonValueExpressionDriver &CommonPluginFunction::parentDriver()
{
    return const_cast<CommonValueExpressionDriver &>(parentDriver_);
}

inline const fvMesh &CommonPluginFunction::mesh() const
{
    return parentDriver_.mesh();
}

inline ExpressionResult &CommonPluginFunction::result()
{
    return result_;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

}

// ************************************************************************* //
