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

Class
    manipulateFvSolutionFvSchemesFunctionObject

Description
    Basis for functionObjects that manipulate the data from
    - fvScheme
    - fvSolution
    in memory. Provides a framework and utility methods but the actual
    logic is in the subclasses

SourceFiles
    manipulateFvSolutionFvSchemesFunctionObject.C

Contributors/Copyright:
    2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef manipulateFvSolutionFvSchemesFunctionObject_H
#define manipulateFvSolutionFvSchemesFunctionObject_H

#include "simpleFunctionObject.H"
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

    class fvSolution;
    class fvSchemes;

/*---------------------------------------------------------------------------*\
                           Class manipulateFvSolutionFvSchemesFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class manipulateFvSolutionFvSchemesFunctionObject
:
    public simpleFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    manipulateFvSolutionFvSchemesFunctionObject(const manipulateFvSolutionFvSchemesFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const manipulateFvSolutionFvSchemesFunctionObject&);

    fvSolution &fvSolution_;

    fvSchemes &fvSchemes_;

    dictionary fvSolutionBackup_;

    dictionary fvSchemesBackup_;

    void rereadFvSolution();

    void rereadFvSchemes();

protected:

    void resetFvSolution();

    void resetFvSchemes();

    void resetDict(dictionary &dict);

    dictionary &fvSolutionDict();

    dictionary &fvSchemesDict();

public:

    //- Runtime type information
    TypeName("manipulateFvSolutionFvSchemes");


    // Constructors

    //- Construct from components
    manipulateFvSolutionFvSchemesFunctionObject
    (
        const word&,
        const Time&,
        const dictionary&
    );

    bool start();

    void writeSimple();

    virtual bool manipulateFvSolution(const Time &) = 0;

    virtual bool manipulateFvSchemes(const Time &) = 0;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
