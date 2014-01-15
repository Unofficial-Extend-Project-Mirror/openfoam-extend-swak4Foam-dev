/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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
    stl

Description

SourceFiles
    stl.C

Contributors/Copyright:
    2008 Hannes Kroeger (hannes@kroegeronline.net)
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef mtv_H
#define mtv_H

#include "surfaceWriter.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class mtv Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class mtv
:
    public surfaceWriter<Type>
{

public:

    //- Runtime type information
    TypeName("mtv");


    // Constructors

        //- Construct null
        mtv();


    // Destructor

        virtual ~mtv();


    // Member Functions

        // Write

        //- Writes single surface to file.
        virtual void write
        (
            const fileName& samplePath,
            const fileName& timeDir,
            const fileName& surfaceName,
            const pointField& points,
            const faceList& faces,
            const fileName& fieldName,
            const Field<Type>& values,
            const bool verbose = false
        ) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "mtv.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
