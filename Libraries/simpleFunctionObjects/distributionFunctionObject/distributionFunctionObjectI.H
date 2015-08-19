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
    2008-2011, 2013-2014 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "distributionFunctionObject.H"
#include "IOmanip.H"
#include "objectRegistry.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam {
/*---------------------------------------------------------------------------*\
                           Class distributionFunctionObject Declaration
\*---------------------------------------------------------------------------*/

template <typename Type>
autoPtr<SimpleDistribution<Type> > distributionFunctionObject::setData(
    const Field<Type> &values,
    const Field<Type> &weights,
        const scalar binWidth
)
{
    Field<bool> mask(values.size(),true);

    return setData(
        values,
        weights,
        mask,
        binWidth
    );
}

template <typename Type>
autoPtr<SimpleDistribution<Type> > distributionFunctionObject::setDataScalar(
    const Field<Type> &values,
    const Field<scalar> &weights,
        const scalar binWidth
)
{
    Field<bool> mask(values.size(),true);

    return setDataScalar(
        values,
        weights,
        mask,
        binWidth
    );
}

template <typename Type>
autoPtr<SimpleDistribution<Type> > distributionFunctionObject::setData(
    const Field<Type> &values,
    const Field<Type> &weights,
    const Field<bool> &mask,
    const scalar binWidth
)
{
    scalar binSize=binWidth;
    if(binSize<0) {
        binSize=readScalar(dict().lookup("distributionBinWidth"));
    }

    label binNrMax=dict().lookupOrDefault<label>(
        "distributionMaxBinNr",
        1000
    );

    autoPtr<SimpleDistribution<Type> > pDist;

    bool goOn=true;
    while(goOn) {
        goOn=false;

        pDist.reset(
            new SimpleDistribution<Type>(
                binSize
            )
        );

        SimpleDistribution<Type> &dist=pDist();

        dist.calc(values,weights,mask);

        if(dist.maxNrBins()>binNrMax) {
            scalar newBinSize=binSize*(1.1*dist.maxNrBins())/binNrMax;
            WarningIn("distributionFunctionObject::setData")
                << "Number of bin " << dist.maxNrBins()
                    << " exceeds allowed number "
                    << binNrMax << ". Increasing binSize from " << binSize
                    << " to " << newBinSize << endl
                    << "If you want to increase the maximum number of bins "
                    << "set the variable 'distributionMaxBinNr' to a higher value"
                    << endl;
            binSize=newBinSize;
            goOn=true;
        }
    }

    return pDist;
}

template <typename Type>
autoPtr<SimpleDistribution<Type> > distributionFunctionObject::setDataScalar(
    const Field<Type> &values,
    const Field<scalar> &weights,
    const Field<bool> &mask,
    const scalar binWidth
)
{
    scalar binSize=binWidth;
    if(binSize<0) {
        binSize=readScalar(dict().lookup("distributionBinWidth"));
    }

    label binNrMax=dict().lookupOrDefault<label>(
        "distributionMaxBinNr",
        1000
    );

    autoPtr<SimpleDistribution<Type> > pDist;

    bool goOn=true;
    while(goOn) {
        goOn=false;

        pDist.reset(
            new SimpleDistribution<Type>(
                binSize
            )
        );

        SimpleDistribution<Type> &dist=pDist();

        dist.calcScalarWeight(values,weights,mask);

        if(dist.maxNrBins()>binNrMax) {
            scalar newBinSize=binSize*(1.1*dist.maxNrBins())/binNrMax;
            WarningIn("distributionFunctionObject::setDataScalar")
                << "Number of bin " << dist.maxNrBins()
                    << " exceeds allowed number "
                    << binNrMax << ". Increasing binSize from " << binSize
                    << " to " << newBinSize << endl
                    << "If you want to increase the maximum number of bins "
                    << "set the variable 'distributionMaxBinNr' to a higher value"
                    << endl;
            binSize=newBinSize;
            goOn=true;
        }
    }

    return pDist;
}

template<class Type>
void distributionFunctionObject::writeATimeline(
    const autoPtr<SimpleDistribution<Type> > &pDist
) {
    if(!pDist.valid()) {
        return;
    }
    const SimpleDistribution<Type> &dist=pDist();

    for(direction i=0;i<pTraits<Type>::nComponents;i++) {
        word name(this->baseName()+"_"+pTraits<Type>::componentNames[i]);
        writeTime(name,this->time().value());

        DynamicList<scalar> data;
        data.append(component(dist.mean(),i));
        data.append(component(dist.median(),i));
        data.append(component(dist.min(),i));
        data.append(component(dist.max(),i));
        data.append(dist.totalWeight(i));

        writeData(
            name,
            data
        );
        endData(name);
    }
}

template<class Type>
void distributionFunctionObject::writeADistribution(
    const autoPtr<SimpleDistribution<Type> > &pDist
) {
    if(!pDist.valid()) {
        return;
    }
    const SimpleDistribution<Type> &dist=pDist();
    fileName theDir(this->baseDir()/"distributions"/obr().time().timeName());

    mkDir(theDir);

    dist.write(
        theDir/(this->baseName()+"_distribution")
    );
}

template<class Type>
void distributionFunctionObject::reportADistribution(
    const autoPtr<SimpleDistribution<Type> > &pDist
) {
    if(!pDist.valid()) {
        return;
    }
    const SimpleDistribution<Type> &dist=pDist();

    Info<< regionString()
        << " Distribution of " << this->baseName()
        << "(" << pTraits<Type>::typeName << "): "
        << dist.maxNrBins() << " bins. Mean: " << dist.mean()
        << " Median: " << dist.median()
        << " Min: " << dist.min() << " Max: " << dist.max() << endl;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// ************************************************************************* //
