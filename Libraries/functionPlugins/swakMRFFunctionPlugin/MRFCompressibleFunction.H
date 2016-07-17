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
    Foam::MRFCompressibleFunction

Description

SourceFiles
    MRFCompressibleFunction.C

Contributors/Copyright:
    2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef MRFCompressibleFunction_H
#define MRFCompressibleFunction_H

#include "swakMRFPluginFunctionBasis.H"

#ifdef FOAM_HAS_IOMRFLIST

#include "FieldValuePluginFunction.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class MRFCompressibleFunction Declaration
\*---------------------------------------------------------------------------*/

template<class FieldType>
class MRFCompressibleFunction
:
    public swakMRFPluginFunctionBasis
{
    //- Disallow default bitwise assignment
    void operator=(const MRFCompressibleFunction &);

    MRFCompressibleFunction(const MRFCompressibleFunction &);

protected:

    autoPtr<FieldType> field_;

    autoPtr<FieldType> rho_;

    void doEvaluation();

    virtual void manipulate(FieldType &result) = 0;

    virtual void setArgument(
        label index,
        const string &content,
        const CommonValueExpressionDriver &driver
    );

public:
    MRFCompressibleFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name
    );

    virtual ~MRFCompressibleFunction() {}

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif // FOAM_HAS_IOMRFLIST

#endif

// ************************************************************************* //
