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
    2008-2011, 2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "SolverInfo.H"

#include "objectRegistry.H"
#include "surfaceFields.H"

namespace Foam {

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

    SolverInfo::SolverInfo(const objectRegistry &obr,const dictionary& dict)
:
    phi_(
        dict.found("phi")
        ? word(dict.lookup("phi"))
        : word("phi")
    ),
    compressible_(false),
    turbulent_(false),
    les_(false)
{
    if(obr.foundObject<surfaceScalarField>(phi_)) {
        const surfaceScalarField &phi=obr.lookupObject<surfaceScalarField>(phi_);

        if(phi.dimensions()==dimensionSet(1,0,-1,0,0,0,0)) {
            compressible_=true;
        } else if(phi.dimensions()!=dimensionSet(0,3,-1,0,0,0,0)) {
        WarningIn("SolverInfo::SolverInfo(const dictionary& dict,const objectRegistry &obr)")
            << " Dimensions " << phi.dimensions() 
                << " of the phi-field with name " << phi_ 
                << "don't fit compressible or incompressible " << nl
                << "Assumin incompressible solver" << endl;            
        }
    } else {
        WarningIn("SolverInfo::SolverInfo(const dictionary& dict,const objectRegistry &obr)")
            << "Can't find phi-field with name " << phi_ << nl
                << "Assumin incompressible solver" << endl;
    }

    if (isFile(obr.time().constantPath()/"turbulenceProperties")) {
        turbulent_=true;

        bool rasFound=obr.foundObject<IOdictionary>("RASProperties");
        bool lesFound=obr.foundObject<IOdictionary>("LESProperties");

        if(rasFound && lesFound) {
            WarningIn("SolverInfo::SolverInfo(const dictionary& dict,const objectRegistry &obr)")
                << "LES and RAS found. Assuming RAS" << endl;
        } else if(lesFound) {
            les_=true;
        } else if(!rasFound && !lesFound) {
            WarningIn("SolverInfo::SolverInfo(const dictionary& dict,const objectRegistry &obr)")
                << "Neither LES nor RAS found. Assuming no turbulence" << endl;
            turbulent_=false;
        }
    }
    printInfo();
}

void SolverInfo::printInfo()
{
    Info << "phi: " << phi_ << endl;
    Info << "Compressible: " << compressible_ << endl;
    Info << "Turbulent: " << turbulent_ << endl;
    Info << "LES: " << les_ << endl;
}

}

// ************************************************************************* //
