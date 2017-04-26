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

\*---------------------------------------------------------------------------*/

#include "swakMotion.H"
#include "addToRunTimeSelectionTable.H"
#include "mathematicalConstants.H"

#ifdef FOAM_NO_SEPARATE_CONSTANT_NAMESPACE
using namespace Foam::mathematicalConstant;
#else
using namespace Foam::constant::mathematical;
#endif

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
namespace solidBodyMotionFunctions
{
    defineTypeNameAndDebug(swakMotion, 0);
    addToRunTimeSelectionTable
    (
        solidBodyMotionFunction,
        swakMotion,
        dictionary
    );
}
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::solidBodyMotionFunctions::swakMotion::swakMotion
(
    const dictionary& SBMFCoeffs,
    const Time& runTime
)
:
    solidBodyMotionFunction(SBMFCoeffs, runTime)
{
    read(SBMFCoeffs);
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

Foam::solidBodyMotionFunctions::swakMotion::
~swakMotion()
{}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

Foam::CommonValueExpressionDriver &
Foam::solidBodyMotionFunctions::swakMotion::driver() const
{
    if(!driver_.valid()) {
        word regionName=
            SBMFCoeffs_.lookupOrDefault<word>("region",polyMesh::defaultRegion);
        const fvMesh &mesh=dynamic_cast<const fvMesh &>(
            time_.lookupObject<objectRegistry>(regionName)
        );

        const_cast<swakMotion&>(*this).driver_.set(
            CommonValueExpressionDriver::New(
                SBMFCoeffs_,
                mesh
            ).ptr()
        );
    }
    return const_cast<swakMotion&>(*this).driver_();
}

Foam::septernion
Foam::solidBodyMotionFunctions::swakMotion::velocity() const
{
    // dummy implementation
    return septernion::zero;
}

Foam::septernion
Foam::solidBodyMotionFunctions::swakMotion::transformation() const
{
    driver().clearVariables();

    scalar alpha=0;
    vector translation=vector::zero;
    vector axis=vector::zero;
    vector origin=vector::zero;

    if(doTranslation_) {
        translation=driver().evaluateUniform<vector>(translationExpression_);
        Dbug << "Translation: " << translation << endl;
    }
    septernion TR(-translation);

    if(doRotation_) {
        alpha=driver().evaluateUniform<scalar>(alphaExpression_);
        axis=driver().evaluateUniform<vector>(axisExpression_);
        if(mag(axis)<SMALL) {
            WarningIn("Foam::solidBodyMotionFunctions::swakMotion::transformation()")
                << axisExpression_ << " evaluates to vector of zero length" << nl
                    << "No rotation" << endl;
        } else {
            axis/=mag(axis);
            if(alphaIsDegrees_) {
                alpha*=pi/180;
            }

            Dbug << "Rotation axis: " << axis << " alpha: " << alpha << endl;
            quaternion rotation(axis,alpha);
            Dbug << "Rotation: " << rotation << endl;
            TR*=
                septernion(-origin)
                *
                rotation
                *
                septernion(origin);
        }
    }

    Dbug << "Transformation: " << TR << endl;

    return TR;
}


bool Foam::solidBodyMotionFunctions::swakMotion::read
(
    const dictionary& SBMFCoeffs
)
{
    solidBodyMotionFunction::read(SBMFCoeffs);

    if(driver_.valid()) {
        driver_.clear();
    }

    SBMFCoeffs_.lookup("doTranslation") >> doTranslation_;
    SBMFCoeffs_.lookup("doRotation") >> doRotation_;

    if(
        !doTranslation_
        &&
        !doRotation_
    ) {
        WarningIn(SBMFCoeffs.name())
            << "You want to do neither rotation nor translation. What's the point?"
                << endl;
    }
    if(doRotation_) {
        SBMFCoeffs_.lookup("axisExpression") >> axisExpression_;
        SBMFCoeffs_.lookup("originExpression") >> originExpression_;
        SBMFCoeffs_.lookup("alphaExpression") >> alphaExpression_;
        SBMFCoeffs_.lookup("alphaIsDegrees") >> alphaIsDegrees_;
    } else {
        axisExpression_=exprString("vector(0,0,0)");
        alphaExpression_=exprString("0");
    }
    if(doTranslation_) {
        SBMFCoeffs_.lookup("translationExpression") >> translationExpression_;
    } else {
        translationExpression_=exprString("vector(0,0,0)");
    }

    return true;
}


// ************************************************************************* //
