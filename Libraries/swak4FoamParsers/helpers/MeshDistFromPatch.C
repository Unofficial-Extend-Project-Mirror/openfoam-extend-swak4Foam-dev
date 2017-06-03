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

Contributors/Copyright:
    2014, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "MeshDistFromPatch.H"

namespace Foam {

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
#define TRACKDATA td
#else
#define TRACKDATA
#endif

    MeshDistFromPatch::MeshDistFromPatch()
        :
        dist_(-1)
        {}

    MeshDistFromPatch::MeshDistFromPatch(scalar d)
        :
        dist_(d)
        {}

    scalar MeshDistFromPatch::dist() const
        { return dist_; }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool MeshDistFromPatch::valid(
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        TrackingData &
#endif
    ) const
        { return dist_>=0; }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool MeshDistFromPatch::updateCell
    (
        const polyMesh& mesh,
        const label thisCellI,
        const label neighbourFaceI,
        const MeshDistFromPatch& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            const scalar d=mag(
                mesh.cellCentres()[thisCellI]
                -
                mesh.faceCentres()[neighbourFaceI]
            );

            if(!valid(TRACKDATA)) {
                dist_=d+neighbourInfo.dist();
                return true;
            } else {
                const scalar nd=d+neighbourInfo.dist();
                if(nd<dist_) {
                    dist_=nd;
                    return true;
                } else {
                    return false;
                }
            }
        }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool MeshDistFromPatch::updateFace
    (
        const polyMesh& mesh,
        const label thisFaceI,
        const label neighbourCellI,
        const MeshDistFromPatch& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            const scalar d=mag(
                mesh.cellCentres()[neighbourCellI]
                -
                mesh.faceCentres()[thisFaceI]
            );

            if(!valid(TRACKDATA)) {
                dist_=d+neighbourInfo.dist();
                return true;
            } else {
                const scalar nd=d+neighbourInfo.dist();
                if(nd<dist_) {
                    dist_=nd;
                    return true;
                } else {
                    return false;
                }
            }
        }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool MeshDistFromPatch::updateFace
    (
        const polyMesh&,
        const label thisFaceI,
        const MeshDistFromPatch& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            if(!valid(TRACKDATA)) {
                dist_=neighbourInfo.dist();
                return true;
            } else {
                if(dist()>neighbourInfo.dist()) {
                    dist_=neighbourInfo.dist();
                    return true;
                } else {
                    return false;
                }
            }
        }

    bool MeshDistFromPatch::operator!=(const MeshDistFromPatch &rhs) const {
        return dist_<0 || dist_ != rhs.dist();
    }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
    bool MeshDistFromPatch::equal(const MeshDistFromPatch &rhs,TrackingData &td) const
    {
        return !(operator!=(rhs));
    }
#endif

    Ostream& operator<<
    (
        Ostream& os,
        const MeshDistFromPatch& wDist
    )
    {
        return os << wDist.dist_;
    }


    Istream& operator>>(Istream& is, MeshDistFromPatch& wDist)
    {
        return is >> wDist.dist_;
    }

#undef TRACKDATA

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
