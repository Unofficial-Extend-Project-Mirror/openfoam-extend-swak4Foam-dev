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
    Foam::GeneralSurfaceEvaluationPluginFunction

Description

SourceFiles
    GeneralSurfaceEvaluationPluginFunction.C

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef GeneralSurfaceEvaluationPluginFunction_H
#define GeneralSurfaceEvaluationPluginFunction_H

#include "GeneralSurfacesPluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class GeneralSurfaceEvaluationPluginFunction Declaration
\*---------------------------------------------------------------------------*/

template<class Type>
class GeneralSurfaceEvaluationPluginFunction
:
    public GeneralSurfacesPluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const GeneralSurfaceEvaluationPluginFunction &);

    GeneralSurfaceEvaluationPluginFunction(const GeneralSurfaceEvaluationPluginFunction &);

    autoPtr<Field<Type> > values_;

protected:

    const Field<Type> &values() const { return values_(); }

    void setArgument(
        label index,
        const string &content,
        const CommonValueExpressionDriver &driver
    );

    typedef GeometricField<Type,fvPatchField,volMesh> resultType;

    word getID(
        label index
    );

public:
    GeneralSurfaceEvaluationPluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~GeneralSurfaceEvaluationPluginFunction() {}

    TypeName("GeneralSurfaceEvaluationPluginFunction");

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
