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

 ICE Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swak.H"

#ifndef FOAM_HAS_NO_DATAENTRY_CLASS

#include "swakDataEntry.H"

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

namespace Foam {
#ifdef FOAM_DATAENTRY_IS_NOW_FUNCTION1
#define makeDataEntryType makeFunction1Type
#endif

    makeDataEntryType(swakDataEntry,scalar);
    makeDataEntryType(swakDataEntry,vector);
#ifdef FOAM_DATAENTRY_HAS_TENSOR_INSTANCES
    makeDataEntryType(swakDataEntry,tensor);
    makeDataEntryType(swakDataEntry,sphericalTensor);
    makeDataEntryType(swakDataEntry,symmTensor);
#endif
}

#endif

// ************************************************************************* //
