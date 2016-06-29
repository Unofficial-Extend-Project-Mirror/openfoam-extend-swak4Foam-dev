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
    2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef CloudProxyForParticle_H
#define CloudProxyForParticle_H

#include "CloudProxy.H"

#include "HashPtrTable.H"

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

    //- abstract class to store pointers to member functions of the particle class
    template<class RType>
    class ParticleMethodWrapper
    {
    public:
        typedef typename CloudType::particleType particleType;

        ParticleMethodWrapper()
            {}
        virtual ~ParticleMethodWrapper()
            {}

        virtual RType operator()(const particleType &) const = 0;

        RType operator()(const particleType *p) const
            { return operator()(*p); }
    };

    template<class RType>
    class ParticleMethodWrapperReference
    :
    public ParticleMethodWrapper<RType>
    {

    public:
        typedef typename ParticleMethodWrapper<RType>::particleType particleType;

        typedef const RType& (particleType::*FSig)() const;

        FSig fPtr_;

        ParticleMethodWrapperReference(FSig f):
            ParticleMethodWrapper<RType>(),
            fPtr_(f)
            {}

        virtual ~ParticleMethodWrapperReference()
            {}

        virtual RType
        operator()(const particleType &p) const
            {
                return ((p).*(fPtr_))();
            }
    };

    template<class RType,class FType=RType>
    class ParticleMethodWrapperValue
    :
    public ParticleMethodWrapper<RType>
    {

    public:
        typedef typename ParticleMethodWrapper<RType>::particleType particleType;

        typedef FType (particleType::*FSig)() const;

        FSig fPtr_;

        ParticleMethodWrapperValue(FSig f):
            ParticleMethodWrapper<RType>(),
            fPtr_(f)
            {}

        virtual ~ParticleMethodWrapperValue()
            {}

        virtual RType
        operator()(const particleType &p) const
            {
                return ((p).*(fPtr_))();
            }
    };

    template<class RType>
    class ParticleMethodWrapperConstant
    :
    public ParticleMethodWrapper<RType>
    {
        RType value_;

    public:
        typedef typename ParticleMethodWrapper<RType>::particleType particleType;

        ParticleMethodWrapperConstant(RType value):
            ParticleMethodWrapper<RType>(),
            value_(value)
            {}

        virtual ~ParticleMethodWrapperConstant()
            {}

        virtual RType
        operator()(const particleType &p) const
            {
                return value_;
            }
    };

    template<class RType>
    class ParticleMethodWrapperFieldElement
    :
    public ParticleMethodWrapper<RType>
    {

    public:
        typedef typename ParticleMethodWrapper<RType>::particleType particleType;

        typedef const Field<RType>& (particleType::*FSig)() const;

        FSig fPtr_;
        label index_;

        ParticleMethodWrapperFieldElement(
            FSig f,
            label index
        ):
            ParticleMethodWrapper<RType>(),
            fPtr_(f),
            index_(index)
            {}

        virtual ~ParticleMethodWrapperFieldElement()
            {}

        virtual RType
        operator()(const particleType &p) const
            {
                //                Info << ((p).*(fPtr_))() << " " << index_ << " " << endl;
                return ((p).*(fPtr_))()[index_];
            }
    };

    HashPtrTable<ParticleMethodWrapper<scalar>,word,string::hash>
      scalarFunctions_;

    void addScalarFunction(
        const word &name,
        const string &description,
        ParticleMethodWrapper<scalar> *ptr
    );

    HashPtrTable<ParticleMethodWrapper<bool>,word,string::hash>
      boolFunctions_;

    void addBoolFunction(
        const word &name,
        const string &description,
        ParticleMethodWrapper<bool> *ptr
    );

    HashPtrTable<ParticleMethodWrapper<vector>,word,string::hash>
      vectorFunctions_;

    void addVectorFunction(
        const word &name,
        const string &description,
        ParticleMethodWrapper<vector> *ptr
    );

    HashPtrTable<ParticleMethodWrapper<tensor>,word,string::hash>
      tensorFunctions_;

    void addTensorFunction(
        const word &name,
        const string &description,
        ParticleMethodWrapper<tensor> *ptr
    );

    HashPtrTable<ParticleMethodWrapper<symmTensor>,word,string::hash>
      symmTensorFunctions_;

    void addSymmTensorFunction(
        const word &name,
        const string &description,
        ParticleMethodWrapper<symmTensor> *ptr
    );

    HashPtrTable<ParticleMethodWrapper<sphericalTensor>,word,string::hash>
      sphericalTensorFunctions_;

    void addSphericalTensorFunction(
        const word &name,
        const string &description,
        ParticleMethodWrapper<sphericalTensor> *ptr
    );

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

    virtual tmp<Field<bool> > getBoolField(
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

    tmp<Field<label> > getCells() const;

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
