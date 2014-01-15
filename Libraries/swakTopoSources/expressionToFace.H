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
    \\  /    A nd           | Copyright  held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::expressionToFace

Description
    A topoSetSource to select a faceSet from a logical expression on the cells

    Face is added if owner and neighbour cell have two different values

SourceFiles
    expressionToFace.C

Contributors/Copyright:
    2010-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef expressionToFace_H
#define expressionToFace_H

#include "topoSetSource.H"

#include "swak.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class expressionToFace Declaration
\*---------------------------------------------------------------------------*/

class expressionToFace
:
    public topoSetSource
{

    //- Add usage string
    static addToUsageTable usage_;

    //- The logical expression to use
    string expression_;

    //- dictionary with additional data
    autoPtr<dictionary> dict_;

    // Private Member Functions

    //- Depending on face to cell option add to or delete from cellSet.
    void combine(topoSet& set, const bool add) const;

public:

    //- Runtime type information
    TypeName("expressionToFace");

    // Constructors

        //- Construct from components
        expressionToFace
        (
            const polyMesh& mesh,
            const string& expression
        );

        //- Construct from dictionary
        expressionToFace
        (
            const polyMesh& mesh,
            const dictionary& dict
        );

        //- Construct from Istream
        expressionToFace
        (
            const polyMesh& mesh,
            Istream&
        );


    // Destructor

        virtual ~expressionToFace();


    // Member Functions

        virtual void applyToSet
        (
            const topoSetSource::setAction action,
            topoSet&
        ) const;

#ifdef FOAM_TOPOSETSOURCE_HAS_SETTYPE
        virtual sourceType setType() const;
#endif

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
