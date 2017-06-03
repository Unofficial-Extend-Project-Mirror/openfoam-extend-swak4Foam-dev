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
    executeIfOpenFOAMVersionBiggerEqualFunctionObject

Description
    This proxy-functionObject only executs if the OpenFOAM-version is bigger
    equal to the version specified by a triple
    - majorVersion
    - minorVersion
    - patchVersion

SourceFiles
    executeIfOpenFOAMVersionBiggerEqualFunctionObject.C

Contributors/Copyright:
    2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef executeIfOpenFOAMVersionBiggerEqualFunctionObject_H
#define executeIfOpenFOAMVersionBiggerEqualFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class executeIfOpenFOAMVersionBiggerEqualFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class executeIfOpenFOAMVersionBiggerEqualFunctionObject
:
    public conditionalFunctionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeIfOpenFOAMVersionBiggerEqualFunctionObject(const executeIfOpenFOAMVersionBiggerEqualFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const executeIfOpenFOAMVersionBiggerEqualFunctionObject&);

    //- first part of the version
    label majorVersion_;

    //- second part of the version
    label minorVersion_;

    //- third (optional) part of the version
    label patchVersion_;

    //- reread the data
    void readData(const dictionary& dict);

    //- get a number as a label
    label toLabel(const string &v);

protected:

    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("executeIfOpenFOAMVersionBiggerEqual");


    // Constructors

    //- Construct from components
    executeIfOpenFOAMVersionBiggerEqualFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool read(const dictionary& dict);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
