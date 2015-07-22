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

Contributors/Copyright:
    2008-2015 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "swakExpressionAverageDistributionFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"
#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template <typename Type,typename AType>
autoPtr<PtrList<SimpleDistribution<AType> > >
swakExpressionAverageDistributionFunctionObject::setData(
        const Field<AType> &xValues,
        const Field<Type> &values,
        const Field<scalar> &weights
) {
    Field<bool> mask(values.size(),true);

    return setData(
        xValues,
        values,
        weights,
        mask
    );
}

template <typename Type,typename AType>
autoPtr<PtrList<SimpleDistribution<AType> > >
swakExpressionAverageDistributionFunctionObject::setData(
        const Field<AType> &xValues,
        const Field<Type> &values,
        const Field<scalar> &weights,
        const Field<bool> &mask
) {
    componentNames_=wordList(pTraits<Type>::nComponents);
    for(direction i=0;i<(pTraits<Type>::nComponents);i++) {
        componentNames_[i]=pTraits<Type>::componentNames[i];
    }

    label binNr=readLabel(dict().lookup("binNumber"));

    dict().lookup("valueIfZero"); // provoke an error if unset
    AType valueIfZero=dict().lookupOrDefault(
        "valueIfZero",
        pTraits<AType>::zero
    );

    AType xMin;
    AType xMax;
    if(dynamicExtremesAbscissa_) {
        xMin=gMin(xValues);
        xMax=gMax(xValues);
    } else {
        xMin=dict().lookupOrDefault(
            "minAbscissa",
            pTraits<AType>::zero
        );
        xMax=dict().lookupOrDefault(
            "maxAbscissa",
            pTraits<AType>::zero
        );
    }
    Dbug << "Min xValues " << xMin
        << " Max xValues " << xMax << endl;

    AType span=xMax-xMin;
    AType binSize=pTraits<AType>::zero;
    for(direction i=0;i<pTraits<AType>::nComponents;i++) {
        scalar sz=component(span,i)/(binNr);
        if(sz<SMALL*1e5) {
            sz=1;
        }
        setComponent(binSize,i)=sz;
    }

    if(debug) {
        Info << "swakExpressionAverageDistributionFunctionObject::setData "
            << "span: " << span << " nrBins: " << binNr
            << " binSize: " << binSize << endl;
    }

    autoPtr<PtrList<SimpleDistribution<AType> > > pDists(
        new PtrList<SimpleDistribution<AType> >(pTraits<Type>::nComponents)
    );
    PtrList<SimpleDistribution<AType> > &dists=pDists();
    PtrList<SimpleDistribution<AType> > wDists(pTraits<Type>::nComponents);

    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        dists.set(
            i,
            new SimpleDistribution<AType>(
                binSize
            )
        );
        SimpleDistribution<AType> &dist=dists[i];
        wDists.set(
            i,
            new SimpleDistribution<AType>(
                binSize
            )
        );
        SimpleDistribution<AType> &wDist=wDists[i];
        // addition of 2*VSMALL is a workaround for weights that are
        // equal to 0. Needs proper rewrite in SimpleDistribution
        dist.calcScalarWeight(
            xValues,
            values.component(i)*weights+2*VSMALL,
            mask,
            false // do not reduce yet
        );
        dist.calcMinimumMaximum(xValues,values.component(i),mask);
        wDist.calcScalarWeight(
            xValues,
            weights,
            mask,
            false // do not reduce yet
        );

        if(debug>1) {
            Info << "Dist: " << dist << endl
                << "Weight: " << wDist << endl;
        }
        reduce(dist,plusOp<SimpleDistribution<AType> >());
        reduce(wDist,plusOp<SimpleDistribution<AType> >());
        dist.divideByDistribution(
            wDist,
            valueIfZero
        );
        if(debug>1) {
            Info << "Dist: " << dist << endl;
        }
    }

    return pDists;
}

template <typename Type>
void swakExpressionAverageDistributionFunctionObject::getDistributionInternal(
    PtrList<SimpleDistribution<Type> > &dist
) {
    Field<Type> values(driver_->getResult<Type>());

    driver_->parse(expression_);
    word rType=driver_->CommonValueExpressionDriver::getResultType();
    if(rType==pTraits<scalar>::typeName) {
        dist.transfer(
            setData(
                values,
                driver_->getResult<scalar>()(),
                weightValues_,
                maskValues_
            )()
        );
    } else if(rType==pTraits<vector>::typeName) {
        dist.transfer(
            setData(
                values,
                driver_->getResult<vector>()(),
                weightValues_,
                maskValues_
            )()
        );
    } else if(rType==pTraits<tensor>::typeName) {
        dist.transfer(
            setData(
                values,
                driver_->getResult<tensor>()(),
                weightValues_,
                maskValues_
            )()
        );
    } else if(rType==pTraits<symmTensor>::typeName) {
        dist.transfer(
            setData(
                values,
                driver_->getResult<symmTensor>()(),
                weightValues_,
                maskValues_
            )()
        );
    } else if(rType==pTraits<sphericalTensor>::typeName) {
        dist.transfer(
            setData(
                values,
                driver_->getResult<sphericalTensor>()(),
                weightValues_,
                maskValues_
            )()
        );
    } else {
        FatalErrorIn("swakExpressionAverageDistributionFunctionObject::getDistributionInternal")
            << "Unhandled type " << rType << " for abscissaExpression "
                << abscissaExpression_
                << endl
                << exit(FatalError);
    }
}

template <typename Type>
void swakExpressionAverageDistributionFunctionObject::writeADistribution(
    const PtrList<SimpleDistribution<Type> > &dist
)
{
    if(dist.size()<=0) {
        return;
    }
    if(dist.size()!=componentNames_.size()) {
        FatalErrorIn("swakExpressionAverageDistributionFunctionObject::writeADistribution")
            << "The size of the abszissa-components " << componentNames_
                << " is not equal to the number of stored distributions "
                << dist.size()
                << endl
                << exit(FatalError);

    }
    fileName theDir(this->baseDir()/obr().time().timeName());
    mkDir(theDir);
    forAll(componentNames_,i) {
        fileName fName=theDir/this->baseName()+"_averageDistribution";
        if(componentNames_.size()>1) {
            fName+="_"+componentNames_[i];
        }
        dist[i].writeRaw(
            fName
        );
    }
}

template <typename Type>
void swakExpressionAverageDistributionFunctionObject::reportADistribution(
    const PtrList<SimpleDistribution<Type> > &dist
)
{
    if(dist.size()<=0) {
        return;
    }

    Info<< regionString()
        << " Distribution of " << this->baseName()
        << "(" << pTraits<Type>::typeName << "): ";
    if(componentNames_.size()>1) {
        Info << endl;
    }

    forAll(componentNames_,i) {
        if(componentNames_.size()>1) {
            Info << componentNames_[i] << ": ";
        }
        Info << dist[i].maxNrBins() << " bins. "
            << " Min: " << dist[i].min() << " Max: " << dist[i].max() << endl;
    }

    Info << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
