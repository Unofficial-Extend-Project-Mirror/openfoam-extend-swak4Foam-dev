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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::DebugOStream

Description
    Stream that prefixes a class name and an adress to the output to make
    distinguishing output from different objects easier. Usually used through
    two preprocessor macros:

    Dbug : for parallel runs only outputs on the master processor
    Pbug : for parallel runs outputs on all processors and prefixes the
           processor number

SourceFiles
    DebugOStream.C

Contributors/Copyright:
    2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef DebugOStream_H
#define DebugOStream_H

#include "Pstream.H"

#include "prefixOSstream.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class DebugOStream Declaration
\*---------------------------------------------------------------------------*/

class DebugOStream
:
    public prefixOSstream
{
public:
    //- Constructor
    DebugOStream(
        ostream &o,
        const word &typeName,
        const void *object,
        const bool parallel=false
    );
    DebugOStream &operator()() { return *this; }

};

// to be used as "streams"
#define Dbug if(debug && Pstream::master()) DebugOStream(cout,typeName,this)()
#define Pbug if(debug) DebugOStream(cout,typeName,this,true)()
    // for static methods
#define Sbug if(debug) DebugOStream(cout,typeName,reinterpret_cast<const void *>(NULL),true)()

//- get the value of the pointer as a word
word getHex(const void *ptr);

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
