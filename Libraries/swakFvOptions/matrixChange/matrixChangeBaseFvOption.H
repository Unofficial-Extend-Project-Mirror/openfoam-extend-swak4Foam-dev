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
    This file is part of OpenFOAM.

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
    Foam::fv::matrixChangeBaseFvOption

Description
    This fvOption is the basis for two fvOptions that record the change of the
    matrix diagonal/source due to other fvOptions

SourceFiles
    matrixChangeBaseFvOption.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef matrixChangeBaseFvOption_H
#define matrixChangeBaseFvOption_H

#include "autoPtr.H"
#include "topoSetSource.H"
#include "cellSet.H"
#include "fvMesh.H"
#include "volFields.H"
#include "fvOption.H"
#include "functionObjectList.H"

#include "swakFvOption.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{

/*---------------------------------------------------------------------------*\
               Class matrixChangeBaseFvOption Declaration
\*---------------------------------------------------------------------------*/

class matrixChangeBaseFvOption
:
    public option
{
    // Private data

    word fieldName_;

    word namePrefix_;

    bool doAtAddSup_;

    // Private Member Functions

    //- Disallow default bitwise copy construct
    matrixChangeBaseFvOption(const matrixChangeBaseFvOption&);

    //- Disallow default bitwise assignment
    void operator=(const matrixChangeBaseFvOption&);

protected:

    template<class T>
    inline tmp<GeometricField<T,fvPatchField,volMesh> >
    calcResiduum(const fvMatrix<T> &) const;

    word fieldName() const {
        return namePrefix_ + fieldName_;
    }

    bool doAtAddSup() {
        return doAtAddSup_;
    }
public:

    //- Runtime type information
    TypeName("matrixChangeBase");


    // Constructors

    //- Construct from explicit source name and mesh
    matrixChangeBaseFvOption
    (
        const word& sourceName,
        const word& modelType,
        const dictionary& dict,
        const fvMesh& mesh
    );

    ~matrixChangeBaseFvOption();

    // Member Functions

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace fv
} // End namespace Foam

#include "matrixChangeBaseFvOptionI.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
