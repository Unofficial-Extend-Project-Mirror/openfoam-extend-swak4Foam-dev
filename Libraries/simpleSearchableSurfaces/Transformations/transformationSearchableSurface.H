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
    Foam::transformationSearchableSurface

Description
    Searching on transformed surface

SourceFiles
    transformationSearchableSurface.C

Contributors/Copyright:
    2009, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef transformationSearchableSurface_H
#define transformationSearchableSurface_H

#include "searchableSurface.H"

#include "swakSurfaceTypes.H"

#include "boundBox.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// Forward declaration of classes

/*---------------------------------------------------------------------------*\
                           Class transformationSearchableSurface Declaration
\*---------------------------------------------------------------------------*/

class transformationSearchableSurface
:
    public searchableSurface
{
private:

    // Private Member Data

        autoPtr<IOobject> rename_;

        autoPtr<searchableSurface> delegate_;

        //- Disallow default bitwise copy construct
        transformationSearchableSurface(const transformationSearchableSurface&);

        //- Disallow default bitwise assignment
        void operator=(const transformationSearchableSurface&);

protected:

    searchableSurface &delegate() { return delegate_(); }

    const searchableSurface &delegate() const { return delegate_(); }

    // Do the transformation for a point
    virtual point transform(const point &) const =0;

    // Do the inverse transformation for a point
    virtual point inverseTransform(const point &) const =0;

    // Transform a hit to the real world
    pointIndexHit transform(const pointIndexHit &) const;

    // Transform a hit from the real world
    pointIndexHit inverseTransform(const pointIndexHit &) const;

public:

    //- Runtime type information
    TypeName("transformationSearchableSurface");


    // Constructors

        //- Construct from dictionary (used by searchableSurface)
        transformationSearchableSurface
        (
            const IOobject& io,
            const dictionary& dict
        );

    // Destructor

        virtual ~transformationSearchableSurface();


    // Member Functions

    virtual label size() const { return delegate().size(); }

#ifdef FOAM_SEARCHABLE_SURF_USES_TMP
        virtual tmp<pointField> coordinates() const;
#else
        virtual pointField coordinates() const;
#endif

#ifdef FOAM_SEARCHABLE_SURF_HAS_POINTS
        virtual tmp<pointField> points() const;
#endif

        virtual const wordList& regions() const;

        virtual bool overlaps(const boundBox &) const;

        //- Whether supports volume type below
        virtual bool hasVolumeType() const
        {
            return delegate().hasVolumeType();
        }

        //- What is type of points outside bounds
        virtual volumeType outsideVolumeType() const
        {
#ifdef FOAM_SEARCHABLE_SURFACE_HAS_OUTSIDEVOLUMETYPE
            return delegate().outsideVolumeType();
#else
            return VOLTYPE_UNKNOWN;
#endif
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

            void getVolumeType
            (
                const pointField& points,
                List<volumeType>& volType
            ) const;

        // regIOobject implementation

            bool writeData(Ostream&) const
            {
                notImplemented("transformationSearchableSurface::writeData(Ostream&) const");
                return false;
            }

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
