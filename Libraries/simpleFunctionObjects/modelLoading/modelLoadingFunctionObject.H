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
    modelLoadingFunctionObject

Description
    Basis for the other function objects the load and correct a model

SourceFiles
    modelLoadingFunctionObject.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef modelLoadingFunctionObject_H
#define modelLoadingFunctionObject_H

#include "simpleFunctionObject.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class modelLoadingFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template <class ModelType>
class modelLoadingFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    modelLoadingFunctionObject(const modelLoadingFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const modelLoadingFunctionObject&);

    //- the actual model we're working with
    autoPtr<ModelType> model_;

    //- whether the model should be corrected
    bool correctModel_;

    //- whether the model can be reloaded later
    bool allowReload_;

    //- fail if a model of this type already exists
    bool failIfModelTypeExists_;
protected:
    virtual autoPtr<ModelType> initModel() = 0;

public:

    //- Runtime type information
    //    TypeName("modelLoadingFunctionObject");


    // Constructors

    //- Construct from components
    modelLoadingFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual inline bool start();

    //- Sample and write
    virtual void write();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

#ifdef NoRepository
#include "modelLoadingFunctionObject.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
