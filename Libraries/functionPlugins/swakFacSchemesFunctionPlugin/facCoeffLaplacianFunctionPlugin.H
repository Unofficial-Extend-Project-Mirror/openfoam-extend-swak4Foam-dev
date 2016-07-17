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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    Foam::facCoeffLaplacianFunctionPlugin

Description

SourceFiles
    facCoeffLaplacianFunctionPlugin.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef facCoeffLaplacianFunctionPlugin_H
#define facCoeffLaplacianFunctionPlugin_H

#include "FaFieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"

#include "fac.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class facCoeffLaplacianFunctionPlugin Declaration
\*---------------------------------------------------------------------------*/

template<class T,class GT>
class facCoeffLaplacianFunctionPlugin
:
    public FaFieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const facCoeffLaplacianFunctionPlugin &);

    facCoeffLaplacianFunctionPlugin(const facCoeffLaplacianFunctionPlugin &);

    typedef GeometricField<T,faPatchField,areaMesh> originalType;
    typedef GeometricField<T,faPatchField,areaMesh> resultType;
    typedef GeometricField<GT,faPatchField,areaMesh> coeffType;

    autoPtr<originalType> original_;
    autoPtr<coeffType> coeff_;

    string specString_;

public:
    facCoeffLaplacianFunctionPlugin(
        const FaFieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~facCoeffLaplacianFunctionPlugin() {}

    TypeName("facCoeffLaplacianFunctionPlugin");

    void doEvaluation();

    void setArgument(
        label index,
        const string &content,
        const CommonValueExpressionDriver &driver
    );

    void setArgument(label index,const string &);

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
