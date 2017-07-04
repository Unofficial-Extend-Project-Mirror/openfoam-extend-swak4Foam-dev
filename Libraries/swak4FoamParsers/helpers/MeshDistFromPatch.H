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
    Foam::MeshDistFromPatch

Description
    Records the distance from a specified patch

SourceFiles
    MeshDistFromPatch.C

Contributors/Copyright:
    2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef MeshDistFromPatch_H
#define MeshDistFromPatch_H

#include "FaceCellWaveData.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class FaceCellWave Declaration
\*---------------------------------------------------------------------------*/


class MeshDistFromPatch
:
    public FaceCellWaveData
{
    scalar dist_;
public:

    inline MeshDistFromPatch();

    inline MeshDistFromPatch(scalar d);

    inline scalar dist() const;

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    inline bool valid(
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        TrackingData &
#endif
    ) const;

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    inline bool updateCell
    (
        const polyMesh& mesh,
        const label thisCellI,
        const label neighbourFaceI,
        const MeshDistFromPatch& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &
#endif
    );

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    inline bool updateFace
    (
        const polyMesh& mesh,
        const label thisFaceI,
        const label neighbourCellI,
        const MeshDistFromPatch& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &
#endif
    );

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    inline bool updateFace
    (
        const polyMesh&,
        const label thisFaceI,
        const MeshDistFromPatch& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &
#endif
    );

    inline bool operator!=(const MeshDistFromPatch &rhs) const;

    friend inline Ostream& operator<<(Ostream&, const MeshDistFromPatch&);
    friend inline Istream& operator>>(Istream&, MeshDistFromPatch&);

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
    inline bool equal(const MeshDistFromPatch &rhs,TrackingData &td) const;
#endif

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "MeshDistFromPatch.C"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
