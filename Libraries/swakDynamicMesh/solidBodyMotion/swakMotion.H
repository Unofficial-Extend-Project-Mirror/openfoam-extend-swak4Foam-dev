/*---------------------------------------------------------------------------*\
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
    Foam::solidBodyMotionFunctions::swakMotion

Description
    Returns a solid body motion according to an evaluation of swak-functions

SourceFiles
    swakMotion.C

\*---------------------------------------------------------------------------*/

#ifndef swakMotion_H
#define swakMotion_H

#include "solidBodyMotionFunction.H"
#include "primitiveFields.H"
#include "point.H"

#include "CommonValueExpressionDriver.H"
#include "exprString.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace solidBodyMotionFunctions
{

/*---------------------------------------------------------------------------*\
                          Class swakMotion Declaration
\*---------------------------------------------------------------------------*/

class swakMotion
:
    public solidBodyMotionFunction
{
    // Private data

    //- the actual driver for the Expressions
    autoPtr<CommonValueExpressionDriver> driver_;

    bool doTranslation_;

    bool doRotation_;

    bool alphaIsDegrees_;

    exprString translationExpression_;

    exprString axisExpression_;

    exprString originExpression_;

    exprString alphaExpression_;

    // Private Member Functions

        //- Disallow copy construct
        swakMotion(const swakMotion&);

        //- Disallow default bitwise assignment
        void operator=(const swakMotion&);

    CommonValueExpressionDriver &driver() const;

public:

    //- Runtime type information
    TypeName("swakMotion");


    // Constructors

        //- Construct from components
        swakMotion
        (
            const dictionary& SBMFCoeffs,
            const Time& runTime
        );

        //- Construct and return a clone
        virtual autoPtr<solidBodyMotionFunction> clone() const
        {
            return autoPtr<solidBodyMotionFunction>
            (
                new swakMotion
                (
                    SBMFCoeffs_,
                    time_
                )
            );
        }


    //- Destructor
    virtual ~swakMotion();


    // Member Functions

        //- Return the solid-body motion transformation septernion
        virtual septernion transformation() const;

        //- Return the solid-body motion velocity transformation septernion (for foam-extend)
        virtual septernion velocity() const;

        //- Update properties from given dictionary
        virtual bool read(const dictionary& SBMFCoeffs);
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace solidBodyMotionFunctions
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
