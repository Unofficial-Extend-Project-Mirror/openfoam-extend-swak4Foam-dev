//  OF-extend Revision: $Id$ 
/*---------------------------------------------------------------------------*\
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
    dynamicFunctionObjectListProxy

Description
    Dynamically generates a functionObjectList and executes it

SourceFiles
    dynamicFunctionObjectListProxy.C

\*---------------------------------------------------------------------------*/

#ifndef dynamicFunctionObjectListProxy_H
#define dynamicFunctionObjectListProxy_H

#include "functionObjectListProxy.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class dynamicFunctionObjectListProxy Declaration
\*---------------------------------------------------------------------------*/

class dynamicFunctionObjectListProxy
:
    public functionObjectListProxy
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    dynamicFunctionObjectListProxy(const dynamicFunctionObjectListProxy&);
    
    //- Disallow default bitwise assignment
    void operator=(const dynamicFunctionObjectListProxy&);

public:

    class dynamicDictionaryProvider {
        //- Disallow default bitwise copy construct
        dynamicDictionaryProvider(const dynamicDictionaryProvider&);
        
        //- Disallow default bitwise assignment
        void operator=(const dynamicDictionaryProvider&);

        //- the owning functionObject
        const dynamicFunctionObjectListProxy &owner_;

    public:
        TypeName("dynamicDictionaryProvider");
        
        declareRunTimeSelectionTable
        (
            autoPtr,
            dynamicDictionaryProvider,
            dictionary,
            (
                const dictionary& dict,
                const dynamicFunctionObjectListProxy &owner
            ),
            (dict,owner)
        );
        
        dynamicDictionaryProvider(
            const dictionary&,
            const dynamicFunctionObjectListProxy &
        );

        static autoPtr<dynamicDictionaryProvider> New
        (
            const word& type,
            const dictionary& dict,
            const dynamicFunctionObjectListProxy &owner
        );

        virtual ~dynamicDictionaryProvider()
            {}

        virtual string getDictionaryText() = 0;
    };

protected:

    //- the generated dictionary
    autoPtr<dictionary> dynamicDict_;

    //- the actual provider of the dictionary text
    autoPtr<dynamicDictionaryProvider> provider_;

    //- initialize the function-objects
    virtual void initFunctions();

public:

    //- Runtime type information
    TypeName("dynamicFunctionObjectListProxy");


    // Constructors

    //- Construct from components
    dynamicFunctionObjectListProxy
    (
        const word&,
        const Time&,
        const dictionary&,
        const char *providerName=""
    );

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
