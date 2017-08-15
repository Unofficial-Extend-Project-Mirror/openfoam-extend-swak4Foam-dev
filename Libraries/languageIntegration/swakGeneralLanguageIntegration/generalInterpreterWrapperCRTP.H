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
    \\  /    A nd           | Copyright  held by original author
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
    generalInterpreterWrapperCRTP

Description
    Intermediate template class to implement the Curiously Recurring Template Pattern

SourceFiles
    generalInterpreterWrapperCRTP.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

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
