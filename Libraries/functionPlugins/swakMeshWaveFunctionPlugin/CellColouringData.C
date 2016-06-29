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
    2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CellColouringData.H"

namespace Foam {

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
#define TRACKDATA td
#else
#define TRACKDATA
#endif

    CellColouringData::CellColouringData()
        :
        colour_(-1),
        goOn_(false)
        {}

    CellColouringData::CellColouringData(const cell &c)
        :
        colour_(-1),
        colours_(c.size(),-1),
        goOn_(false)
        {}

    label CellColouringData::colour() const
        { return colour_; }

    bool CellColouringData::goOn() const
        { return goOn_; }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool CellColouringData::valid(
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        TrackingData &
#endif
) const
        { return colour_>=0; }

    void CellColouringData::setColour(label col,bool goOn)
    {
        colour_=col;
        goOn_=goOn;
    }

    label CellColouringData::calcColour() const
    {
        // at maximum all other cells have a different colour. So we need one more
        for(int i=0;i<=colours_.size();i++) {
            bool found=false;
            forAll(colours_,j) {
                if(colours_[j]==i) {
                    found=true;
                    break;
                }
            }
            if(!found) {
                return i;
            }
        }
        return -1;
    }

    label CellColouringData::nextFace(const cell &c) const
    {
        forAll(colours_,i) {
            if(colours_[i]<0) {
                return c[i];
            }
        }
        return -1;
    }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool CellColouringData::updateCell
    (
        const polyMesh& mesh,
        const label thisCellI,
        const label neighbourFaceI,
        const CellColouringData& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            const cell &c=mesh.cells()[thisCellI];
            bool found=false;
            forAll(c,i) {
                if(c[i]==neighbourFaceI) {
                    found=true;
                    if(colours_[i]>=0) {
                        FatalErrorIn("CellColouringData::updateCell")
                            << "Face " << i << " already coloured." << nl
                                << "Colours: " << colours_ << " Faces: "
                                << c << endl
                                << abort(FatalError);
                    }
                    colours_[i]=neighbourInfo.colour();
                }
            }

            if(neighbourInfo.goOn()) {
                if(colour_>=0) {
                    FatalErrorIn("CellColouringData::updateCell")
                        << "Cell " << thisCellI  << " already coloured: "
                            << colour_
                            << abort(FatalError);
                }
                colour_=calcColour();
                return true;
            } else {
                return false;
            }
        }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool CellColouringData::updateFace
    (
        const polyMesh& mesh,
        const label thisFaceI,
        const label neighbourCellI,
        const CellColouringData& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            if(neighbourInfo.colour()<0) {
                FatalErrorIn("CellColouringData::updateFace")
                    << "Cell with negative value signaled." << nl
                        << "This shouldn't happen" << endl
                        << abort(FatalError);
            }
            colour_=neighbourInfo.colour();
            if(
                neighbourInfo.nextFace(mesh.cells()[neighbourCellI])
                ==
                thisFaceI
            ) {
                goOn_=true;
            } else {
                goOn_=false;
            }
            return true;
        }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
#endif
    bool CellColouringData::updateFace
    (
        const polyMesh&,
        const label thisFaceI,
        const CellColouringData& neighbourInfo,
        const scalar tol
#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
        ,TrackingData &td
#endif
    )
        {
            (*this)=neighbourInfo;

            return true;
        }

    bool CellColouringData::operator!=(const CellColouringData &rhs) const{
        return colour_<0 || colour_ != rhs.colour();
    }

    Ostream& operator<<
    (
        Ostream& os,
        const CellColouringData& wDist
    )
    {
        return os << wDist.colour_
            << token::SPACE << wDist.colours_ << token::SPACE
            << wDist.goOn_;
    }


    Istream& operator>>(Istream& is, CellColouringData& wDist)
    {
        return is >> wDist.colour_ >> wDist.colours_
            >> wDist.goOn_;
    }

#ifdef FOAM_FACECELLWAVE_HAS_TRACKINGDATA
    template<class TrackingData>
    bool CellColouringData::equal(
        const CellColouringData &rhs,
        TrackingData &td
    ) const
    {
        return !(operator!=(rhs));
    }
#endif

#undef TRACKDATA

} // namespace

// ************************************************************************* //
