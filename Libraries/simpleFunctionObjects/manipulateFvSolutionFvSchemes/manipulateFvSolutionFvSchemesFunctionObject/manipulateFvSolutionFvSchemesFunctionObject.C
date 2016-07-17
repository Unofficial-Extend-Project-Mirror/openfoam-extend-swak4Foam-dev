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
    2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "manipulateFvSolutionFvSchemesFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "fvMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "swak.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(manipulateFvSolutionFvSchemesFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

    template<class T>
    T &desparate(T &i) {
        Info << "Ho" << endl;
        return i;
    }

manipulateFvSolutionFvSchemesFunctionObject::manipulateFvSolutionFvSchemesFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict),
    fvSolution_(
#ifdef FOAM_FVMESH_NOT_CHILD_OF_FVSCHEMES_SOLUTION
        const_cast<surfaceInterpolation&>(
            dynamic_cast<const surfaceInterpolation&>(
                obr()
            )
        ).solutionDict()
#else
        const_cast<fvSolution&>(
            dynamic_cast<const fvSolution&>(
                obr()
            )
        )
#endif
    ),
    fvSchemes_(
#ifdef FOAM_FVMESH_NOT_CHILD_OF_FVSCHEMES_SOLUTION
        const_cast<surfaceInterpolation&>(
            dynamic_cast<const surfaceInterpolation&>(
                obr()
            )
        ).schemesDict()
#else
        const_cast<fvSchemes&>(
            dynamic_cast<const fvSchemes&>(
                obr()
            )
        )
#endif
    ),
    fvSolutionBackup_(
        fvSolution_
    ),
    fvSchemesBackup_(
        fvSchemes_
    )
{
#ifdef FOAM_SOLUTION_HAS_NO_READ_WITH_DICT
    FatalErrorIn("manipulateFvSolutionFvSchemesFunctionObject::manipulateFvSolutionFvSchemesFunctionObject")
        << "This Foam-version does not have the facilities to overwrite fvSolution in memory."
            << endl
            << exit(FatalError);
#endif
#ifdef FOAM_SCHEMES_HAS_NO_READ_WITH_DICT
    FatalErrorIn("manipulateFvSolutionFvSchemesFunctionObject::manipulateFvSolutionFvSchemesFunctionObject")
        << "This Foam-version does not have the facilities to overwrite fvSchemes in memory."
            << endl
            << exit(FatalError);
#endif
}

void  manipulateFvSolutionFvSchemesFunctionObject::resetFvSolution()
{
    dynamic_cast<dictionary&>(fvSolution_)=fvSolutionBackup_;
}

void  manipulateFvSolutionFvSchemesFunctionObject::resetFvSchemes()
{
    dynamic_cast<dictionary&>(fvSchemes_)=fvSchemesBackup_;
}

void  manipulateFvSolutionFvSchemesFunctionObject::resetDict(dictionary &dict)
{
    if(dict.name()==fvSchemesDict().name()) {
        resetFvSchemes();
    } else if(dict.name()==fvSolutionDict().name()) {
        resetFvSolution();
    } else {
        FatalErrorIn("manipulateFvSolutionFvSchemesFunctionObject::resetDict(dictionary &dict)")
            << "Can't reset unknown dictionary" << dict.name() << nl
                << "Is neither " << fvSchemesDict().name() << " nor "
                << fvSolutionDict().name()
                << endl
                << exit(FatalError);
    }
}

dictionary &manipulateFvSolutionFvSchemesFunctionObject::fvSolutionDict() {
    return dynamic_cast<dictionary &>(fvSolution_);
}

dictionary &manipulateFvSolutionFvSchemesFunctionObject::fvSchemesDict() {
    return dynamic_cast<dictionary &>(fvSchemes_);
}


bool manipulateFvSolutionFvSchemesFunctionObject::start()
{
    simpleFunctionObject::start();

    if(this->manipulateFvSolution(obr().time())) {
        rereadFvSolution();
    }

    if(this->manipulateFvSchemes(obr().time())) {
        rereadFvSchemes();
    }

    return true;
}

void manipulateFvSolutionFvSchemesFunctionObject::write()
{
    if(this->manipulateFvSolution(obr().time())) {
        rereadFvSolution();
    }

    if(this->manipulateFvSchemes(obr().time())) {
        rereadFvSchemes();
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
