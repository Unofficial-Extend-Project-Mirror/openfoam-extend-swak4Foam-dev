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
    Foam::groovyBCCommon

Description
    Foam::groovyBCCommon

SourceFiles
    groovyBCCommon.C

Contributors/Copyright:
    2011, 2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#ifndef groovyBCCommon_H
#define groovyBCCommon_H

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "dictionary.H"

namespace Foam
{

/*---------------------------------------------------------------------------*\
                     Class groovyBCFvPatch Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class groovyBCCommon
{

    bool evaluateDuringConstruction_;

protected:

    // Protected data

    bool debug_;
    bool hasGradient_;

    // the expressions
    string valueExpression_;
    string gradientExpression_;
    string fractionExpression_;

    string nullValue();

    bool evaluateDuringConstruction() const 
        { return evaluateDuringConstruction_; }

public:

    // Constructors

    //- Construct from nothing
    groovyBCCommon
    (
        bool hasGradient,
        bool isPoint=false,
        string fractionExpression="1"
    );

    //- Construct from dictionary
    groovyBCCommon
    (
        const dictionary&,
        bool hasGradient,
        bool isPoint=false,
        string fractionExpression="1"
    );

    //- Copy constructor
    groovyBCCommon
    (
        const groovyBCCommon<Type> &
    );

    // Member functions

    //- Write
    void write(Ostream&) const;
};


    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #ifdef NoRepository
    #   include "groovyBCCommon.C"
    #endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    #endif

// ************************************************************************* //
