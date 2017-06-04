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
    Foam::fvcSurfCoeffLaplacianFunctionPlugin

Description

SourceFiles
    fvcSurfCoeffLaplacianFunctionPlugin.C

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef fvcSurfCoeffLaplacianFunctionPlugin_H
#define fvcSurfCoeffLaplacianFunctionPlugin_H

#include "FieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class fvcSurfCoeffLaplacianFunctionPlugin Declaration
\*---------------------------------------------------------------------------*/

template<class T,class GT>
class fvcSurfCoeffLaplacianFunctionPlugin
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const fvcSurfCoeffLaplacianFunctionPlugin &);

    fvcSurfCoeffLaplacianFunctionPlugin(const fvcSurfCoeffLaplacianFunctionPlugin &);

    typedef GeometricField<T,fvPatchField,volMesh> originalType;
    typedef GeometricField<T,fvPatchField,volMesh> resultType;
    typedef GeometricField<GT,fvsPatchField,surfaceMesh> coeffType;

    autoPtr<originalType> original_;
    autoPtr<coeffType> coeff_;

    string specString_;

public:
    fvcSurfCoeffLaplacianFunctionPlugin(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~fvcSurfCoeffLaplacianFunctionPlugin() {}

    TypeName("fvcSurfCoeffLaplacianFunctionPlugin");

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
