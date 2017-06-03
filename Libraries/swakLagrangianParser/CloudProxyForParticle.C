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

Contributors/Copyright:
    2012-2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxyForParticle.H"

#include "DebugOStream.H"

#include <functional>

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

    // workaround for a template not defined in <functional>
template <typename S,typename T>
inline std::const_mem_fun_t<S,T> const_mem_fun(S (T::*f)() const)
{
    return std::const_mem_fun_t<S,T>(f);
}

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForParticle<CloudType>::CloudProxyForParticle
(
    const cloud& c
)
:
    CloudProxy(c),
    cloud_(
        dynamicCast<const CloudType&>(
            c
        )
    )

{
    addScalarFunction(
        "origProc",
        "Originating processor",
        new ParticleMethodWrapperValue<scalar,label>(
            &particleType::origProc
        )
    );
    addScalarFunction(
        "origId",
        "Original id",
        new ParticleMethodWrapperValue<scalar,label>(
            &particleType::origId
        )
    );
    addScalarFunction(
        "cell",
        "number of the cell",
        new ParticleMethodWrapperValue<scalar,label>(
            &particleType::cell
        )
    );
    addScalarFunction(
        "face",
        "number of the face",
        new ParticleMethodWrapperValue<scalar,label>(
            &particleType::face
        )
    );
    addBoolFunction(
        "softImpact",
        "used impact model",
        new ParticleMethodWrapperValue<bool>(
            &particleType::softImpact
        )
    );
    addBoolFunction(
        "onBoundary",
        "is this currently on the boundary",
        new ParticleMethodWrapperValue<bool>(
            &particleType::onBoundary
        )
    );
    addScalarFunction(
        "currentTime",
        "current time of the particle",
        new ParticleMethodWrapperValue<scalar>(
            &particleType::currentTime
        )
    );
    addScalarFunction(
        "stepFraction",
        "fraction of the time-step completed",
        new ParticleMethodWrapperValue<scalar>(
            &particleType::stepFraction
        )
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForParticle<CloudType>::~CloudProxyForParticle()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
void CloudProxyForParticle<CloudType>::addScalarFunction(
    const word &name,
    const string &description,
    ParticleMethodWrapper<scalar> *ptr
)
{
    addField<scalar>(name,description);
    scalarFunctions_.set(
        name,
        ptr
    );
}

template<class CloudType>
void CloudProxyForParticle<CloudType>::addBoolFunction(
    const word &name,
    const string &description,
    ParticleMethodWrapper<bool> *ptr
)
{
    addField<bool>(name,description);
    boolFunctions_.set(
        name,
        ptr
    );
}

template<class CloudType>
void CloudProxyForParticle<CloudType>::addVectorFunction(
    const word &name,
    const string &description,
    ParticleMethodWrapper<vector> *ptr
)
{
    addField<vector>(name,description);
    vectorFunctions_.set(
        name,
        ptr
    );
}

template<class CloudType>
void CloudProxyForParticle<CloudType>::addTensorFunction(
    const word &name,
    const string &description,
    ParticleMethodWrapper<tensor> *ptr
)
{
    addField<tensor>(name,description);
    tensorFunctions_.set(
        name,
        ptr
    );
}

template<class CloudType>
void CloudProxyForParticle<CloudType>::addSymmTensorFunction(
    const word &name,
    const string &description,
    ParticleMethodWrapper<symmTensor> *ptr
)
{
    addField<symmTensor>(name,description);
    symmTensorFunctions_.set(
        name,
        ptr
    );
}

template<class CloudType>
void CloudProxyForParticle<CloudType>::addSphericalTensorFunction(
    const word &name,
    const string &description,
    ParticleMethodWrapper<sphericalTensor> *ptr
)
{
    addField<sphericalTensor>(name,description);
    sphericalTensorFunctions_.set(
        name,
        ptr
    );
}

template<class CloudType>
tmp<Field<scalar> > CloudProxyForParticle<CloudType>::getScalarField(
    const word &name
) const
{
    if(scalarFunctions_.found(name)) {
        Dbug << "Found " << name << " in scalar table" << endl;
        return mapToParticles<scalar>(
            *scalarFunctions_[name]
        );
    }

    FatalErrorIn("tmp<Field<scalar> > CloudProxyForParticle<CloudType>::getScalarField")
        << "No scalar field with name " << name << " defined for cloud "
            << cloud_.name() << " of type " // << cloud_.CloudTyptype()
            << endl
            << exit(FatalError);
    return tmp<Field<scalar> >(
        new Field<scalar>(0)
    );
}

template<class CloudType>
tmp<Field<bool> > CloudProxyForParticle<CloudType>::getBoolField(
    const word &name
) const
{
    if(boolFunctions_.found(name)) {
        Dbug << "Found " << name << " in bool table" << endl;
        return mapToParticles<bool>(
            *boolFunctions_[name]
        );
    }

    FatalErrorIn("tmp<Field<bool> > CloudProxyForParticle<CloudType>::getBoolField")
        << "No bool field with name " << name << " defined for cloud "
            << cloud_.name() << " of type " // << cloud_.CloudTyptype()
            << endl
            << exit(FatalError);
    return tmp<Field<bool> >(
        new Field<bool>(0)
    );
}

template<class CloudType>
tmp<Field<vector> > CloudProxyForParticle<CloudType>::getVectorField(
    const word &name
) const
{
    if(vectorFunctions_.found(name)) {
        Dbug << "Found " << name << " in vector table" << endl;
        return mapToParticles<vector>(
            *vectorFunctions_[name]
        );
    }

    FatalErrorIn("tmp<Field<vector> > CloudProxyForParticle<CloudType>::getVectorField")
        << "No vector field with name " << name << " defined for cloud "
            << cloud_.name() << " of type " // << cloud_.CloudTyptype()
            << endl
            << exit(FatalError);
    return tmp<Field<vector> >(
        new Field<vector>(0)
    );
}

template<class CloudType>
tmp<Field<tensor> > CloudProxyForParticle<CloudType>::getTensorField(
    const word &name
) const
{
    if(tensorFunctions_.found(name)) {
        Dbug << "Found " << name << " in tensor table" << endl;
        return mapToParticles<tensor>(
            *tensorFunctions_[name]
        );
    }

    FatalErrorIn("tmp<Field<tensor> > CloudProxyForParticle<CloudType>::getTensorField")
        << "No tensor field with name " << name << " defined for cloud "
            << cloud_.name() << " of type " // << cloud_.CloudTyptype()
            << endl
            << exit(FatalError);
    return tmp<Field<tensor> >(
        new Field<tensor>(0)
    );
}

template<class CloudType>
tmp<Field<symmTensor> > CloudProxyForParticle<CloudType>::getSymmTensorField(
    const word &name
) const
{
    if(symmTensorFunctions_.found(name)) {
        Dbug << "Found " << name << " in symmTensor table" << endl;
        return mapToParticles<symmTensor>(
            *symmTensorFunctions_[name]
        );
    }

    FatalErrorIn("tmp<Field<symmTensor> > CloudProxyForParticle<CloudType>::getSymmTensorField")
        << "No symmTensor field with name " << name << " defined for cloud "
            << cloud_.name() << " of type " // << cloud_.CloudTyptype()
            << endl
            << exit(FatalError);
    return tmp<Field<symmTensor> >(
        new Field<symmTensor>(0)
    );
}

template<class CloudType>
tmp<Field<sphericalTensor> > CloudProxyForParticle<CloudType>::getSphericalTensorField(
    const word &name
) const
{
    if(sphericalTensorFunctions_.found(name)) {
        Dbug << "Found " << name << " in sphericalTensor table" << endl;
        return mapToParticles<sphericalTensor>(
            *sphericalTensorFunctions_[name]
        );
    }

    FatalErrorIn("tmp<Field<sphericalTensor> > CloudProxyForParticle<CloudType>::getSphericalTensorField")
        << "No sphericalTensor field with name " << name << " defined for cloud "
            << cloud_.name() << " of type " // << cloud_.CloudTyptype()
            << endl
            << exit(FatalError);
    return tmp<Field<sphericalTensor> >(
        new Field<sphericalTensor>(0)
    );
}

template<class CloudType>
tmp<Field<scalar> > CloudProxyForParticle<CloudType>::weights() const
{
    return tmp<Field<scalar> >(
        new Field<scalar>(theCloud().size(),1.0)
    );
}

template<class CloudType>
template<class RType,class Func>
tmp<Field<RType> > CloudProxyForParticle<CloudType>::mapToParticles(
    const Func &f
) const
{
    tmp<Field<RType> > tResult(
        new Field<RType>(theCloud().size())
    );
    Field<RType> &result=const_cast<Field<RType>&>(tResult());
    label i=0;
    forAllConstIter(typename CloudType,theCloud(),it)
    {
	const particleType &p=(*it);
        result[i]=f(&p);
        i++;
    }

    return tResult;
}

template<class CloudType>
tmp<Field<vector> > CloudProxyForParticle<CloudType>::getPositions() const
{
    return mapToParticles<vector>(
        const_mem_fun(
            &particleType::position
        )
    );
}

template<class CloudType>
tmp<Field<label> > CloudProxyForParticle<CloudType>::getCells() const
{
    return mapToParticles<label>(
        const_mem_fun(
            &particleType::cell
        )
    );
}

template<class CloudType>
label CloudProxyForParticle<CloudType>::size() const
{
    return cloud_.size();
}


} // namespace end

// ************************************************************************* //
