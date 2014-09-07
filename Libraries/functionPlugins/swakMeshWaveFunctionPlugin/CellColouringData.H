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
    Foam::CellColouringData

Description
    Helper class for Coloring the cells so that no two have the same colour.
    Two different sets of data
    - for cells
      - a list of the neighbour faces and the values their cells
      - the current colour
    - for faces
      - colour to propagate from the cell
      - Indicator variable that tells the face to propagate to the next cell

SourceFiles
    CellColouringData.C

Contributors/Copyright:
    2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CellColouringData_H
#define CellColouringData_H

#include "FaceCellWaveData.H"

#include "cell.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class FaceCellWave Declaration
\*---------------------------------------------------------------------------*/


class CellColouringData
:
    public FaceCellWaveData
{
    label colour_;

    labelList colours_;

    bool goOn_;

public:

    inline CellColouringData();

    inline CellColouringData(const cell &);

    inline label colour() const;

    inline label calcColour() const;

    inline label nextFace(const cell&c) const;

    inline void setColour(label col,bool goOn=false);

    inline bool valid() const;

    inline bool goOn() const;

    inline bool updateCell
    (
        const polyMesh& mesh,
        const label thisCellI,
        const label neighbourFaceI,
        const CellColouringData& neighbourInfo,
        const scalar tol
    );

    inline bool updateFace
    (
        const polyMesh& mesh,
        const label thisFaceI,
        const label neighbourCellI,
        const CellColouringData& neighbourInfo,
        const scalar tol
    );

    inline bool updateFace
    (
        const polyMesh&,
        const label thisFaceI,
        const CellColouringData& neighbourInfo,
        const scalar tol
    );

    inline bool operator!=(const CellColouringData &rhs);

    friend inline Ostream& operator<<(Ostream&, const CellColouringData&);
    friend inline Istream& operator>>(Istream&, CellColouringData&);

};

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "CellColouringData.C"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
