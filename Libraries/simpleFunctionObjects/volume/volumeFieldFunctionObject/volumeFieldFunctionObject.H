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
    volumeFieldFunctionObject

Description
    Basis for functionObjects that might work on every field on a volume

SourceFiles
    volumeFieldFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef volumeFieldFunctionObject_H
#define volumeFieldFunctionObject_H

#include "timelineFunctionObject.H"

#include "tensor.H"
#include "sphericalTensor.H"
#include "symmTensor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class volumeFieldFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class volumeFieldFunctionObject
:
    public timelineFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    volumeFieldFunctionObject(const volumeFieldFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const volumeFieldFunctionObject&);

    //- Names of fields to probe
    wordList fieldNames_;

    //- Indices (into fieldNames_) of scalar/vector/tensor fields
    wordList scalarFields_;
    wordList vectorFields_;
    wordList sphericalTensorFields_;
    wordList symmTensorFields_;
    wordList tensorFields_;
    
     //- Process and write field
    template<class T>
    void processAndWrite(const word& fieldName);

    //- Process and write field
    template<class T>
    void processAndWrite(const wordList& typeFields);

    template<class T>
    void findFields(wordList& typeFieldNames, boolList& foundFields);

//     //- process single field on all patches
//     template <class T>
//     Field<T> process(const word& fieldName) const;

protected:

    //- Names of the files
    virtual wordList fileNames();

    // This is clumsy, but virtual+templated member functions are not allowed
    virtual scalarField process(const word& fieldName,scalar preset)=0;
    virtual Field<vector> process(const word& fieldName,vector preset)=0;
    virtual Field<sphericalTensor> process(const word& fieldName,sphericalTensor preset)=0;
    virtual Field<symmTensor> process(const word& fieldName,symmTensor preset)=0;
    virtual Field<tensor> process(const word& fieldName,tensor preset)=0;

public:

    //- Runtime type information
    TypeName("volumeFieldFunctionObject");


    // Constructors

    //- Construct from components
    volumeFieldFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );


    // Member Functions

    //- start is called at the start of the time-loop
    virtual bool start();

    virtual void write();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "volumeFieldFunctionObjectTemplates.C"
#endif


#endif

// ************************************************************************* //
