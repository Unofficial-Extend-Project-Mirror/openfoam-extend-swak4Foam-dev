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
    Intermediate template class to implement the Curiously Recurring Template Pattern

SourceFiles
    generalInterpreterWrapperCRTP.C

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef generalInterpreterWrapperCRTP_H
#define generalInterpreterWrapperCRTP_H

#include "generalInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class generalInterpreterWrapperCRTP Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class generalInterpreterWrapperCRTP
:
    public generalInterpreterWrapper
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    generalInterpreterWrapperCRTP(const generalInterpreterWrapperCRTP&);

    //- Disallow default bitwise assignment
    void operator=(const generalInterpreterWrapperCRTP&);

protected:
    template <typename Result,class Func>
    Result evaluateCode(
        const string &code,
        bool failOnException
    );

    //- Execute code and see if it returns to true or false
    inline bool evaluateCodeTrueOrFalse(
        const string &code,
        bool failOnException=false
    );

    //- Execute code and use the return value as a float value
    inline scalar evaluateCodeScalar(
        const string &code,
        bool failOnException=false
    );

    //- Execute code and use the return value as a integer value
    inline label evaluateCodeLabel(
        const string &code,
        bool failOnException=false
    );


public:

    // Constructors

    //- Construct from components
    generalInterpreterWrapperCRTP
    (
        const objectRegistry& obr,
        const dictionary&,
        bool forceToNamespace,
        const word interpreterName
    );

    virtual ~generalInterpreterWrapperCRTP();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#include "generalInterpreterWrapperCRTP.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
