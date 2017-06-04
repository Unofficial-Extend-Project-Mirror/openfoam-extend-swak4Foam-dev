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
    executeIfStartTimeFunctionObject

Description
    This function objects only executes its children only if the current time
    is the startTime

SourceFiles
    executeIfStartTimeFunctionObject.C

Contributors/Copyright:
    2011, 2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef executeIfStartTimeFunctionObject_H
#define executeIfStartTimeFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"
#include "regExp.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class executeIfStartTimeFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class executeIfStartTimeFunctionObject
:
    public conditionalFunctionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeIfStartTimeFunctionObject(const executeIfStartTimeFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const executeIfStartTimeFunctionObject&);

    //- execute only if this is the start time
    bool runIfStartTime_;

    //- if this is a restart execute as well
    bool executeOnRestart_;

    //- the time index at which this was read
    label timeIndexRead_;
protected:

    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("executeIfStartTime");


    // Constructors

    //- Construct from components
    executeIfStartTimeFunctionObject
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
