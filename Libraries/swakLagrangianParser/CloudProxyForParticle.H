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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
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
    Foam::CloudProxyForParticle

Description

SourceFiles
    CloudProxyForParticle.C

Contributors/Copyright:
    2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudProxyForParticle_H
#define CloudProxyForParticle_H

#include "CloudProxy.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                       Class CloudProxyForParticle Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class CloudProxyForParticle
:
    public CloudProxy
{
    const CloudType &cloud_;

    //- Disallow default bitwise assignment
    void operator=(const CloudProxyForParticle&);

    //- Disallow copy construct
    CloudProxyForParticle(const CloudProxyForParticle &);

protected:
    template<class RType,class Func>
    tmp<Field<RType> > mapToParticles(
        const Func &f
    ) const;
public:

    typedef CloudType cloudType;
    typedef typename CloudType::particleType particleType;

    // Constructors

    //- Construct from components
    CloudProxyForParticle
    (
        const cloud &
    );

    // Destructor

    virtual ~CloudProxyForParticle();

    const cloudType &theCloud() const
        { return cloud_; }

    virtual tmp<Field<scalar> > getScalarField(
        const word &name
    ) const;

    virtual tmp<Field<vector> > getVectorField(
        const word &name
    ) const;

    virtual tmp<Field<tensor> > getTensorField(
        const word &name
    ) const;

    virtual tmp<Field<symmTensor> > getSymmTensorField(
        const word &name
    ) const;

    virtual tmp<Field<sphericalTensor> > getSphericalTensorField(
        const word &name
    ) const;

    tmp<Field<vector> > getPositions() const;

    tmp<Field<scalar> > weights() const;

    label size() const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "CloudProxyForParticle.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
