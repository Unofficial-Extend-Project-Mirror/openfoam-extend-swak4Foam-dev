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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

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

// template <typename S,typename T,class A>
// inline std::const_mem_fun1_t<S,T,A> const_mem_fun1(S (T::*f)(A) const)
// {
//     return std::const_mem_fun1_t<S,T,A>(f);
// }

template <typename S,typename T>
inline void const_fun(S (T::*f)() const)
{
}

template<class CloudType>
class ParticleMethodWrapper
{

public:
    typedef typename CloudType::particleType particleType;
    typedef vector RType;

    ParticleMethodWrapper()
        {}
    virtual ~ParticleMethodWrapper()
        {}

    virtual RType operator()(const particleType &) const = 0;
};

template<class CloudType>
class ParticleMethodWrapperNoParameter
:
    public ParticleMethodWrapper<CloudType>
{

public:
    typedef typename ParticleMethodWrapper<CloudType>::RType RType;
    typedef typename ParticleMethodWrapper<CloudType>::particleType particleType;

    typedef const RType& (particleType::*FSig)() const;

    FSig fPtr;

    ParticleMethodWrapperNoParameter(FSig f):
        ParticleMethodWrapper<CloudType>(),
        fPtr(f)
        {}

    virtual ~ParticleMethodWrapperNoParameter()
        {}

    virtual RType
    operator()(const typename ParticleMethodWrapper<CloudType>::particleType &p) const
        {
            return ((p).*(fPtr))();
        }
};

template <class T,class RType>
RType (T::*use_const_overload(RType (T::*const_member_function)() const))() const
{
    return const_member_function;
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
    // typename ParticleMethodWrapperNoParameter<CloudType>::FSig s=use_const_overload(&particleType::position);
    // const_fun(&particleType::position);
    ParticleMethodWrapper<CloudType> *m=
        new ParticleMethodWrapperNoParameter<CloudType>(
            use_const_overload(&particleType::position)
        );

    forAllConstIter(typename CloudType,theCloud(),it)
    {
        const particleType &p=(*it);
        (*m)(p);
    }
    // typedef std::const_mem_fun_t<const vector&,particleType> methodType;

    // std::unary_function<const vector&,particleType> &t=methodType(
    //             &particleType::position
    // );

    // typedef std::const_mem_fun1_t<tmp<Field<vector> >,CloudProxyForParticle<CloudType>,methodType> mapType;
    // mapType m=mapType(&CloudProxyForParticle<CloudType>::mapToParticles<vector,mapType>);

    //    //    void *t2=&particleType::position;

    addField<scalar>("origProc"   ,"Originating processor");
    addField<scalar>("origId"     ,"Original id");
    addField<vector>("normal"     ,"Normal of the tet the particle occupies");
    addField<vector>("oldNormal"  ,"Old normal of the tet the particle occupies");
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
CloudProxyForParticle<CloudType>::~CloudProxyForParticle()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
tmp<Field<scalar> > CloudProxyForParticle<CloudType>::getScalarField(
    const word &name
) const
{
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
tmp<Field<vector> > CloudProxyForParticle<CloudType>::getVectorField(
    const word &name
) const
{
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
    Field<RType> &result=tResult();
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
label CloudProxyForParticle<CloudType>::size() const
{
    return cloud_.size();
}


} // namespace end

// ************************************************************************* //
