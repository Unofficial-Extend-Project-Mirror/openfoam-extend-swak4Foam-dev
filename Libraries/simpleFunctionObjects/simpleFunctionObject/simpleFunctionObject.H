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
    simpleFunctionObject

Description
    Basis for the other function objects here. Evaluates the most common options

SourceFiles
    simpleFunctionObject.C

\*---------------------------------------------------------------------------*/

#ifndef simpleFunctionObject_H
#define simpleFunctionObject_H

#include "functionObject.H"
#include "dictionary.H"
#include "OFstream.H"
#include "HashPtrTable.H"
#include "boolList.H"
#include "Field.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class simpleFunctionObject Declaration
\*---------------------------------------------------------------------------*/

    class objectRegistry;

class simpleFunctionObject
:
    public functionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    simpleFunctionObject(const simpleFunctionObject&);
    
    //- Disallow default bitwise assignment
    void operator=(const simpleFunctionObject&);

    // Private data

    //- Do output on the terminal
    bool verbose_;

    //- Time after which calculation should start
    scalar after_;

    //- how many time-steps since last output
    label timeSteps_;

    //- how many time-steps between outputs
    label outputInterval_;

    const Time& time_;

protected:

    dictionary dict_;

    word regionName_;

    const objectRegistry& obr_;

    //- Do output on the terminal
    bool verbose() const { return verbose_; }

    //- Do output this time?
    virtual bool outputTime(const bool forceWrite);

public:

    //- Runtime type information
    TypeName("simpleFunctionObject");


    // Constructors

    //- Construct from components
    simpleFunctionObject
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

    //- Read and set the function object if it's data has changed
    virtual bool read(const dictionary& dict);

    //- Sample and write
    virtual void write()=0;

    //- Optional stuff to do after write
    virtual void flush();

    //- the actual time
    const Time &time() const { return time_; }

    //- the actual time
    scalar after() const { return after_; }
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
