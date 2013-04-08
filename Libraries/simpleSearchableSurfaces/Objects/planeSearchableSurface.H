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
    Foam::planeSearchableSurface

Description
    Searching on sphere

SourceFiles
    planeSearchableSurface.C

Contributors/Copyright:
    2009, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef planeSearchableSurface_H
#define planeSearchableSurface_H

#include "searchableSurface.H"
#include "treeBoundBox.H"
#include "plane.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                           Class planeSearchableSurface Declaration
\*---------------------------------------------------------------------------*/

class planeSearchableSurface
:
    public searchableSurface
{
private:

    // Private Member Data

    //- Centre point
    plane plane_;

    //- Names of regions
    mutable wordList regions_;


    // Private Member Functions

        //- Find nearest point on sphere.
        pointIndexHit findNearest
        (
            const point& sample,
            const scalar nearestDistSqr
        ) const;

        //- Find intersection with sphere
        void findLineAll
        (
            const point& start,
            const point& end,
            pointIndexHit& hit
        ) const;


        //- Disallow default bitwise copy construct
        planeSearchableSurface(const planeSearchableSurface&);

        //- Disallow default bitwise assignment
        void operator=(const planeSearchableSurface&);


public:

    //- Runtime type information
    TypeName("plane");


    // Constructors

        //- Construct from components
        planeSearchableSurface(const IOobject& io, const point&, const scalar radius);

        //- Construct from dictionary (used by searchableSurface)
        planeSearchableSurface
        (
            const IOobject& io,
            const dictionary& dict
        );

    // Destructor

        virtual ~planeSearchableSurface();


    // Member Functions

        virtual label size() const { return 1; }

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
        virtual tmp<pointField> coordinates() const;
#else
        virtual pointField coordinates() const;
#endif

        virtual const wordList& regions() const;

        virtual tmp<pointField> points() const;

        virtual bool overlaps(const boundBox &) const;

        //- Whether supports volume type below
        virtual bool hasVolumeType() const
        {
            return true;
        }


        // Multiple point queries.

            virtual void findNearest
            (
                const pointField& sample,
                const scalarField& nearestDistSqr,
                List<pointIndexHit>&
            ) const;

            virtual void findLine
            (
                const pointField& start,
                const pointField& end,
                List<pointIndexHit>&
            ) const;

            virtual void findLineAny
            (
                const pointField& start,
                const pointField& end,
                List<pointIndexHit>&
            ) const;

            //- Get all intersections in order from start to end.
            virtual void findLineAll
            (
                const pointField& start,
                const pointField& end,
                List<List<pointIndexHit> >&
            ) const;

            //- From a set of points and indices get the region
            virtual void getRegion
            (
                const List<pointIndexHit>&,
                labelList& region
            ) const;

            //- From a set of points and indices get the normal
            virtual void getNormal
            (
                const List<pointIndexHit>&,
                vectorField& normal
            ) const;

            //- Determine type (inside/outside/mixed) for point. unknown if
            //  cannot be determined (e.g. non-manifold surface)
            virtual void getVolumeType
            (
                const pointField&,
                List<volumeType>&
            ) const;


        // regIOobject implementation

            bool writeData(Ostream&) const
            {
                notImplemented("planeSearchableSurface::writeData(Ostream&) const");
                return false;
            }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
