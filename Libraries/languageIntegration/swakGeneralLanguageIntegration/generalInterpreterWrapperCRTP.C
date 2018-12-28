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

Class
    generalInterpreterWrapperCRTP

Description
    General wrapperCRTP for interpreters

SourceFiles
    generalInterpreterWrapperCRTP.C

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/


#include "generalInterpreterWrapperCRTP.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

template<class T>
generalInterpreterWrapperCRTP<T>::generalInterpreterWrapperCRTP
(
    const objectRegistry& obr,
    const dictionary& dict,
    bool forceToNamespace,
    const word interpreterName
):
    generalInterpreterWrapper(
        obr,
        dict,
        forceToNamespace,
        interpreterName
    )
{}

template<class T>
generalInterpreterWrapperCRTP<T>::~generalInterpreterWrapperCRTP()
{
}

template<class T>
bool generalInterpreterWrapperCRTP<T>::evaluateCodeTrueOrFalse(
    const string &code,
    bool failOnException
) {
    Pbug << "evaluateCodeTrueOrFalse" << endl;

    return static_cast<T*>(this)->template evaluateCode<bool,typename T::toBool>(
        code,
        failOnException
    );
}

template<class T>
scalar generalInterpreterWrapperCRTP<T>::evaluateCodeScalar(
    const string &code,
    bool failOnException
) {
    Pbug << "evaluateCodeScalar" << endl;

    return static_cast<T*>(this)->template evaluateCode<scalar,typename T::toScalar>(
        code,
        failOnException
    );
}

template<class T>
label generalInterpreterWrapperCRTP<T>::evaluateCodeLabel(
    const string &code,
    bool failOnException
) {
    Pbug << "evaluateCodeLabel" << endl;

    return static_cast<T*>(this)->template evaluateCode<label,typename T::toLabel>(
        code,
        failOnException
    );
}

template <typename T>
template <typename Result,class Func>
Result generalInterpreterWrapperCRTP<T>::evaluateCode(
    const string &code,
    bool failOnException
)
{
    Pbug << "evaluateCode: " << code << endl;
    syncParallel();

    Result result=pTraits<Result>::zero;
    bool success=false;

    if(!parallelNoRun()) {
        setInterpreter();

        getGlobals();

        result=static_cast<T*>(this)->template evaluateCodeInternal<Result,Func>(
            code,
            success
        );

        Pbug << "Success of execution " << success << endl;

        doAfterExecution(!success,code,false,failOnException);

        releaseInterpreter();
    }

    if(parallelMustBroadcast()) {
        Pbug << "Prescatter: " << result << endl;
        Pstream::scatter(result);
        Pbug << "Postscatter: " << result << endl;
        //        scatterGlobals();
    }

    return result;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// ************************************************************************* //
