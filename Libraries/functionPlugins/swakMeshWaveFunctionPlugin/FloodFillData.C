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
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "FloodFillData.H"

namespace Foam {

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
#define TRACKDATA td
#else
#define TRACKDATA
#endif

    FloodFillData::FloodFillData()
        :
        val_(-2),
        target_(0),
        blocked_(false)
        {}

    FloodFillData::FloodFillData(label target,label d,bool blocked)
        :
        val_(d),
        target_(target),
        blocked_(blocked)
        {}

    void FloodFillData::setTarget(label t)
        { target_=t; }

    label FloodFillData::val() const
        { return val_; }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool FloodFillData::valid(
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        TrackingData &
#endif
    ) const
        { return val_>=0; }

    bool FloodFillData::blocked() const
        { return blocked_; }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool FloodFillData::updateCell
    (
        const polyMesh& mesh,
        const label thisCellI,
        const label neighbourFaceI,
        const FloodFillData& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            if(!valid(TRACKDATA)) {
                if(!blocked()) {
                    val_=target_;
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool FloodFillData::updateFace
    (
        const polyMesh& mesh,
        const label thisFaceI,
        const label neighbourCellI,
        const FloodFillData& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            if(!valid(TRACKDATA)) {
                if(!blocked()) {
                    val_=target_;
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool FloodFillData::updateFace
    (
        const polyMesh&,
        const label thisFaceI,
        const FloodFillData& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            if(!valid(TRACKDATA)) {
                if(!blocked()) {
                    val_=target_;
                    return true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

    bool FloodFillData::operator!=(const FloodFillData &rhs) const {
        return val_<0 || val_ != rhs.val();
    }

    Ostream& operator<<
    (
        Ostream& os,
        const FloodFillData& wDist
    )
    {
        return os << wDist.val_ << token::SPACE << wDist.target_
            << token::SPACE << wDist.blocked_;
    }


    Istream& operator>>(Istream& is, FloodFillData& wDist)
    {
        return is >> wDist.val_ >> wDist.target_ >> wDist.blocked_;
    }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
    bool FloodFillData::equal(const FloodFillData &rhs,TrackingData &td) const
    {
        return !(operator!=(rhs));
    }
#endif

#undef TRACKDATA

} // namespace

// ************************************************************************* //
