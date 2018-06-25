/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Class
    SolverInfo

Description
    Determines the type of the Solver (whether it is 
    compressible/incompressible, turbulent etc) 

SourceFiles
    SolverInfo.C

Contributors/Copyright:
    2008-2011, 2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef SolverInfo_H
#define SolverInfo_H

#include "dictionary.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class objectRegistry;

/*---------------------------------------------------------------------------*\
                           Class SolverInfo Declaration
\*---------------------------------------------------------------------------*/

class SolverInfo
{
    // Private data

    word phi_;
    
    bool compressible_;

    bool turbulent_;

    bool les_;

    // Private Member Functions

    //- Disallow default bitwise copy construct
    SolverInfo(const SolverInfo&);

    //- Disallow default bitwise assignment
    void operator=(const SolverInfo&);

public:

    //- Construct from basic information and try to determine properties 
    SolverInfo(const objectRegistry &obr,const dictionary& dict);

    word phi() const { return phi_; }

    bool compressible() const { return compressible_; }

    bool turbulent() const { return turbulent_; }

    bool les() const { return les_; }

    void printInfo();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
