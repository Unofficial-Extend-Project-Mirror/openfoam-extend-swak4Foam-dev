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

Contributors/Copyright:
    2012-2013, 2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CommonValueExpressionDriver.H"

namespace Foam {


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

template <typename T>
tmp<T> CommonPluginFunction::evaluate(
    const string &content,
    int &consumed
)
{
    evaluateInternal(content,consumed);

    return tmp<T>(result_.getObjectResult<T>(false));
}

template <typename T>
tmp<Field<T> > CommonPluginFunction::evaluate(
    const string &content,
    int &consumed,
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
