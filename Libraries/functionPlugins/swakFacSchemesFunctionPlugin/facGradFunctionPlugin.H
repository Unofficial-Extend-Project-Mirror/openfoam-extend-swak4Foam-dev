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
    Foam::facGradFunctionPlugin

Description

SourceFiles
    facGradFunctionPlugin.C

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef facGradFunctionPlugin_H
#define facGradFunctionPlugin_H

#include "FaFieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"

#include "fac.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class facGradFunctionPlugin Declaration
\*---------------------------------------------------------------------------*/

template<class T>
class facGradFunctionPlugin
:
    public FaFieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const facGradFunctionPlugin &);

    facGradFunctionPlugin(const facGradFunctionPlugin &);

    typedef GeometricField<T,faPatchField,areaMesh> originalType;
    typedef GeometricField<typename outerProduct<vector, T>::type,faPatchField,areaMesh> resultType;

    autoPtr<originalType> original_;

    string specString_;

public:
    facGradFunctionPlugin(
        const FaFieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~facGradFunctionPlugin() {}

    TypeName("facGradFunctionPlugin");

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
