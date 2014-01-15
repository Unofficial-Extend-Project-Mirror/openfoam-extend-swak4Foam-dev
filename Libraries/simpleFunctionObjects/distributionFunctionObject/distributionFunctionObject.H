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
    \\  /    A nd           | Copyright  held by original author
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
    distributionFunctionObject

Description
    Basis for function objects that generate distributions

SourceFiles
    distributionFunctionObject.C

Contributors/Copyright:
    2008-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef distributionFunctionObject_H
#define distributionFunctionObject_H

#include "timelineFunctionObject.H"
#include "NamedEnum.H"

#include "SimpleDistribution.H"

#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class distributionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

class distributionFunctionObject
:
    public timelineFunctionObject
{
    // Private Member Functions

    //- Disallow default bitwise copy construct
    distributionFunctionObject(const distributionFunctionObject&);

    //- Disallow default bitwise assignment
    void operator=(const distributionFunctionObject&);

    bool writeTimeline_;

    bool writeDistribution_;

    template<class Type>
    void writeATimeline(const autoPtr<SimpleDistribution<Type> > &pDist);

    template<class Type>
    void writeADistribution(const autoPtr<SimpleDistribution<Type> > &pDist);

    template<class Type>
    void reportADistribution(const autoPtr<SimpleDistribution<Type> > &pDist);

    bool startup_;
    word startTime_;
protected:

    autoPtr<SimpleDistribution<scalar> > distScalar_;
    autoPtr<SimpleDistribution<vector> > distVector_;
    autoPtr<SimpleDistribution<tensor> > distTensor_;
    autoPtr<SimpleDistribution<symmTensor> > distSymmTensor_;
    autoPtr<SimpleDistribution<sphericalTensor> > distSphericalTensor_;

    template<class Type>
    autoPtr<SimpleDistribution<Type> > setData(
        const Field<Type> &values,
        const Field<Type> &weights,
        const scalar binWidth=-1
    );

    template<class Type>
    autoPtr<SimpleDistribution<Type> > setData(
        const Field<Type> &values,
        const Field<Type> &weights,
        const Field<bool> &mask,
        const scalar binWidth=-1
    );

    template<class Type>
    autoPtr<SimpleDistribution<Type> > setDataScalar(
        const Field<Type> &values,
        const Field<scalar> &weights,
        const scalar binWidth=-1
    );

    template<class Type>
    autoPtr<SimpleDistribution<Type> > setDataScalar(
        const Field<Type> &values,
        const Field<scalar> &weights,
        const Field<bool> &mask,
        const scalar binWidth=-1
    );

    void clearDistributions();

    virtual fileName dataDir();

public:

    //- Runtime type information
    TypeName("distributionFunctionObject");


    // Constructors

    //- Construct from components
    distributionFunctionObject
    (
        const word &name,
        const Time&,
        const dictionary&
    );


    // Member Functions

    bool writeTimeline() const { return writeTimeline_; }

    bool writeDistribution() const { return writeDistribution_; }

    virtual void flush();

    virtual void write();

    virtual bool start();

protected:
    virtual stringList columnNames();

    virtual wordList fileNames();

    virtual word baseName()=0;

    virtual stringList componentNames();

    virtual void getDistribution()=0;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "distributionFunctionObjectI.H"
#endif

#endif

// ************************************************************************* //
