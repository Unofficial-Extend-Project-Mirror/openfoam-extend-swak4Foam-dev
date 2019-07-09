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

    swak4Foam is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::groovyCyclicACMIPolyPatch

Description
    Cyclic patch for Arbitrarily Coupled Mesh Interface (ACMI).
    In addition to the regular coupling a patch value is used to determine
    whether faces are "really" open

SourceFiles
    groovyCyclicACMIPolyPatch.C

Contributors/Copyright:
    2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyCyclicACMIPolyPatch_H
#define groovyCyclicACMIPolyPatch_H

#include "swak.H"

#if defined(FOAM_HAS_ACMI_INTERFACE) && !defined(FOAM_OLD_AMI_ACMI)

#include "cyclicACMIPolyPatch.H"
#ifdef FOAM_AMIINTERPOLATION_NOT_PATCH2PATCH
#include "AMIInterpolation.H"
#else
#include "AMIPatchToPatchInterpolation.H"
#endif
#include "polyBoundaryMesh.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

#ifdef FOAM_AMIINTERPOLATION_NOT_PATCH2PATCH
    typedef AMIInterpolation AMIPatchToPatchInterpolation;
#endif

/*---------------------------------------------------------------------------*\
                     Class groovyCyclicACMIPolyPatch Declaration
\*---------------------------------------------------------------------------*/

class groovyCyclicACMIPolyPatch
:
    public cyclicACMIPolyPatch
{

private:

    // Private data

    //- Mesh currently being created. The field can't possibly be there
    bool meshReady_;

    //- name of the field where the patch value defines if a face is actually open
    word openField_;

protected:

    // Protected Member Functions

        //- Reset the AMI interpolator
        virtual void resetAMI
        (
#ifndef FOAM_ACMI_RESETAMI_NO_PARAMETERS
            const AMIPatchToPatchInterpolation::interpolationMethod& AMIMethod =
                AMIPatchToPatchInterpolation::imFaceAreaWeight
#endif
        ) const;

public:

    //- Runtime type information
    TypeName("groovyCyclicACMI");


    // Constructors

        //- Construct from (base couped patch) components
        groovyCyclicACMIPolyPatch
        (
            const word& name,
            const label size,
            const label start,
            const label index,
            const polyBoundaryMesh& bm,
            const word& patchType,
            const transformType transform = UNKNOWN
        );

        //- Construct from dictionary
        groovyCyclicACMIPolyPatch
        (
            const word& name,
            const dictionary& dict,
            const label index,
            const polyBoundaryMesh& bm,
            const word& patchType
        );

        //- Construct as copy, resetting the boundary mesh
        groovyCyclicACMIPolyPatch
        (
            const groovyCyclicACMIPolyPatch&,
            const polyBoundaryMesh&
        );

        //- Construct given the original patch and resetting the
        //  face list and boundary mesh information
        groovyCyclicACMIPolyPatch
        (
            const groovyCyclicACMIPolyPatch& pp,
            const polyBoundaryMesh& bm,
            const label index,
            const label newSize,
            const label newStart,
            const word& nbrPatchName,
            const word& nonOverlapPatchName,
            const word& openField
        );

        //- Construct given the original patch and a map
        groovyCyclicACMIPolyPatch
        (
            const groovyCyclicACMIPolyPatch& pp,
            const polyBoundaryMesh& bm,
            const label index,
            const labelUList& mapAddressing,
            const label newStart
        );


        //- Construct and return a clone, resetting the boundary mesh
        virtual autoPtr<polyPatch> clone(const polyBoundaryMesh& bm) const
        {
            return autoPtr<polyPatch>(new groovyCyclicACMIPolyPatch(*this, bm));
        }

        //- Construct and return a clone, resetting the face list
        //  and boundary mesh
        virtual autoPtr<polyPatch> clone
        (
            const polyBoundaryMesh& bm,
            const label index,
            const label newSize,
            const label newStart
        ) const
        {
            return autoPtr<polyPatch>
            (
                new groovyCyclicACMIPolyPatch
                (
                    *this,
                    bm,
                    index,
                    newSize,
                    newStart,
                    neighbPatchName(),
                    nonOverlapPatchName(),
                    openField_
                )
            );
        }

        //- Construct and return a clone, resetting the face list
        //  and boundary mesh
        virtual autoPtr<polyPatch> clone
        (
            const polyBoundaryMesh& bm,
            const label index,
            const labelUList& mapAddressing,
            const label newStart
        ) const
        {
            return autoPtr<polyPatch>
            (
                new groovyCyclicACMIPolyPatch
                (
                    *this,
                    bm,
                    index,
                    mapAddressing,
                    newStart
                )
            );
        }


    //- Destructor
    virtual ~groovyCyclicACMIPolyPatch();


    // Member Functions

        // Access

        //- Write the polyPatch data as a dictionary
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "groovyCyclicACMIPolyPatchI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

#endif

// ************************************************************************* //
