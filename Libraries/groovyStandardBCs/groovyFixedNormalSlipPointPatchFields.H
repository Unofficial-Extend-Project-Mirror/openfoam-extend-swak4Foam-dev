/*---------------------------------------------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    swak4Foam is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Contributors/Copyright:
    2009-2010, 2013-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef groovyFixedNormalSlipPointPatchFields_H
#define groovyFixedNormalSlipPointPatchFields_H

#include "groovyFixedNormalSlipPointPatchField.H"
#include "fieldTypes.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef FOAM_HAS_NO_POINT_PATCH_FIELD_TYPEDEF
    // Rebuilds the call
    // makePointPatchFieldTypedefs(groovyFixedNormalSlip);

    typedef groovyFixedNormalSlipPointPatchField<scalar> groovyFixedNormalSlipPointPatchScalarField;
    typedef groovyFixedNormalSlipPointPatchField<vector> groovyFixedNormalSlipPointPatchVectorField;
    typedef groovyFixedNormalSlipPointPatchField<sphericalTensor> groovyFixedNormalSlipPointPatchSphericalTensorField;
    typedef groovyFixedNormalSlipPointPatchField<symmTensor> groovyFixedNormalSlipPointPatchSymmTensorField;
    typedef groovyFixedNormalSlipPointPatchField<tensor> groovyFixedNormalSlipPointPatchTensorField;

#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    typedef groovyFixedNormalSlipPointPatchField<symmTensor4thOrder> groovyFixedNormalSlipPointPatchSymmTensor4thOrderField;
    typedef groovyFixedNormalSlipPointPatchField<diagTensor> groovyFixedNormalSlipPointPatchDiagTensorField;
#endif

#else
    makePointPatchFieldTypedefs(groovyFixedNormalSlip);
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
