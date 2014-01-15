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
    executeIfEnvironmentVariableFunctionObject

Description
    This proxy-functionObject only executs its childrens if an environment 
    variable fits a certain condition:
    - exists
    - does not exist
    - fits a regular expression

SourceFiles
    executeIfEnvironmentVariableFunctionObject.C

Contributors/Copyright:
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef executeIfEnvironmentVariableFunctionObject_H
#define executeIfEnvironmentVariableFunctionObject_H

#include "conditionalFunctionObjectListProxy.H"
#include "regExp.H"
#include "NamedEnum.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class executeIfEnvironmentVariableFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class executeIfEnvironmentVariableFunctionObject
:
    public conditionalFunctionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    executeIfEnvironmentVariableFunctionObject(const executeIfEnvironmentVariableFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const executeIfEnvironmentVariableFunctionObject&);

    //- Name of the variable
    word variableName_;

    //- the regular expression against which the content of the environment variable should be matched
    regExp contentRegexp_;

    //- reread the data
    void readData(const dictionary& dict);

    enum fitVariableMode {
        fvmExists,
        fvmDoesNotExist,
        fvmFitsRegexp
    };
    
    static const NamedEnum<fitVariableMode,3> fitVariableModeNames_;

    //- the mode with which the variable should be fitted
    fitVariableMode fitVariableMode_; 
protected:
    
    //- the actual conition that determines whether the FO should be executed
    virtual bool condition();

public:

    //- Runtime type information
    TypeName("executeIfEnvironmentVariable");


    // Constructors

    //- Construct from components
    executeIfEnvironmentVariableFunctionObject
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
