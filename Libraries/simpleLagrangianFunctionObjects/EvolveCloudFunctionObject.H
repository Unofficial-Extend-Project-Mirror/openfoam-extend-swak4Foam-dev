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
    EvolveCloudFunctionObject

Description
    Basis for the other function objects here. Evaluates the most common options

SourceFiles
    EvolveCloudFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef EvolveCloudFunctionObject_H
#define EvolveCloudFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"

#include "autoPtr.H"
#include "objectRegistry.H"
#include "volFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class EvolveCloudFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class EvolveCloudFunctionObject
:
    public functionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    EvolveCloudFunctionObject(const EvolveCloudFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const EvolveCloudFunctionObject&);

    // Private data
    autoPtr<CloudType> cloud_;

    //- Input dictionary
    dictionary dict_;

    //- name of the region
    word regionName_;

    //- the registry
    const objectRegistry& obr_;

    //- name of the cloud
    word cloudName_;

    //- direction of gravity
    dimensionedVector g_;

protected:

    //- get a reference to the field
    template <class FieldType >
    const FieldType &getField(const word &fieldName);

    const word &cloudName() const { return cloudName_; }

    autoPtr<CloudType> &cloud() { return cloud_; }

    const dimensionedVector g() const { return g_; }

    const objectRegistry &obr() const { return obr_; }

public:

    // Constructors

    //- Construct from components
    EvolveCloudFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    // //- start is called at the start of the time-loop
    // virtual bool start();

    //- execute is called at each ++ or += of the time-loop
    virtual bool execute(bool forceWrite);

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#   include "EvolveCloudFunctionObject.C"
#endif


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
