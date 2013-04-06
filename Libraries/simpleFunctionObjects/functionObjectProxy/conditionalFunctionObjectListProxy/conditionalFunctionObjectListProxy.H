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
    conditionalFunctionObjectListProxy

Description
    Basis for the other function objects here. Evaluates the most common options

SourceFiles
    conditionalFunctionObjectListProxy.C

Contributors/Copyright:
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef conditionalFunctionObjectListProxy_H
#define conditionalFunctionObjectListProxy_H

#include "functionObjectListProxy.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class conditionalFunctionObjectListProxy Declaration
\*---------------------------------------------------------------------------*/

class conditionalFunctionObjectListProxy
:
    public functionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    conditionalFunctionObjectListProxy(const conditionalFunctionObjectListProxy&);
    
    //- Disallow default bitwise assignment
    void operator=(const conditionalFunctionObjectListProxy&);

protected:
    
    //- the actual conition that determines whether the FO should be executed
    virtual bool condition()=0;

public:

    //- Runtime type information
    TypeName("conditionalFunctionObjectListProxy");


    // Constructors

    //- Construct from components
    conditionalFunctionObjectListProxy
    (
        const word&,
        const Time&,
        const dictionary&
    );

    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute(const bool forceWrite);

    //- Called when Time::run() determines that the time-loop exits.
    //  By default it simply calls execute().
    virtual bool end();
    
    //- Read and set the function object if its data have changed
    virtual bool read(const dictionary&);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
