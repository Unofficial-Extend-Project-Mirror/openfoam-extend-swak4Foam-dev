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
    Foam::faForceEquation

Description
    Force a fvMatrix to fixed values in specific places

SourceFiles
    faForceEquation.C

Contributors/Copyright:
    2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef faForceEquation_H
#define faForceEquation_H

#include "FaFieldValueExpressionDriver.H"

#include "faMatrix.H"

#include "DynamicList.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class faForceEquation Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class faForceEquation
:
    protected FaFieldValueExpressionDriver
{
    // Private data

    faForceEquation(const faForceEquation&);

    exprString valueExpression_;

    exprString maskExpression_;

    bool verbose_;

    bool getMask(DynamicList<label> &,const word &psi);

public:

    // Constructors

        //- Construct from a dictionary
        faForceEquation
        (
            const dictionary& ,
            const fvMesh&
        );

    // Destructor

        virtual ~faForceEquation();

    //- fix equations
    void operator()(faMatrix<T> &);

    //- where are the equations fixed
    tmp<areaScalarField> getMask();

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
