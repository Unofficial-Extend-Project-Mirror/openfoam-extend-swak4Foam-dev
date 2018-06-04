/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    foam-extend is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    foam-extend is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with foam-extend.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "SwakScriptableInjection.H"
#include "mathematicalConstants.H"

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class CloudType>
Foam::label Foam::SwakScriptableInjection<CloudType>::parcelsToInject
(
    const scalar time0,
    const scalar time1
)
#ifdef FOAM_INJECT_TOINJECT_IS_CONST
    const
#endif
{
    SwakScriptableInjection<CloudType>* wThis=
        const_cast<SwakScriptableInjection<CloudType>*>(this);

    dictionary d;
    d.add("time0",time0);
    d.add("time1",time1);
    wThis->interpreter().insertDictionary(
        parameterStructName_,
        d
    );
    label result=wThis->interpreter().evaluateCodeLabel(
        parcelsToInjectCode_,
        true
    );
    return result;
}


template<class CloudType>
Foam::scalar Foam::SwakScriptableInjection<CloudType>::volumeToInject
(
    const scalar time0,
    const scalar time1
)
#ifdef FOAM_INJECT_TOINJECT_IS_CONST
    const
#endif
{
    SwakScriptableInjection<CloudType>* wThis=
        const_cast<SwakScriptableInjection<CloudType>*>(this);

    dictionary d;
    d.add("time0",time0);
    d.add("time1",time1);
    wThis->interpreter().insertDictionary(
        parameterStructName_,
        d
    );
    scalar result=wThis->interpreter().evaluateCodeScalar(
        volumeToInjectCode_,
        true
    );

    wThis->volumeTotal_=min(result,SMALL);

    return result;
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
Foam::SwakScriptableInjection<CloudType>::SwakScriptableInjection
(
    const SwakScriptableInjection<CloudType>& im
)
    :
    InjectionModel<CloudType>(im),
    interpreter_(
        generalInterpreterWrapper::New(
            this->owner().mesh(),
            im.dict()
        )
    ),
    initInjectorCode_(
        im.initInjectorCode_
    ),
    doStartInjectionCode_(
        im.doStartInjectionCode_
    ),
    doStopInjectionCode_(
        im.doStopInjectionCode_
    ),
    injectionDurationCode_(
        im.injectionDurationCode_
    ),
    startOfInjectionTimeCode_(
        im.startOfInjectionTimeCode_
    ),
    parcelsToInjectCode_(
        im.parcelsToInjectCode_
    ),
    volumeToInjectCode_(
        im.volumeToInjectCode_
    ),
    prepareParcelDataCode_(
        im.prepareParcelDataCode_
    ),
    particlePropertiesCode_(
        im.particlePropertiesCode_
    ),
    parameterStructName_(
        im.parameterStructName_
    ),
    resultStructName_(
        im.resultStructName_
    ),
    injectByEvent_(
        im.injectByEvent_
    ),
    isActive_(
        im.isActive_
    ),
    plannedDuration_(
        im.plannedDuration_
    )
{}

template<class CloudType>
Foam::SwakScriptableInjection<CloudType>::SwakScriptableInjection
(
    const dictionary& dict,
    CloudType& owner
#ifdef FOAM_INJECT_CONSTRUCTOR_HAS_MODELNAME
    ,const word& modelName
#endif
)
:
    InjectionModel<CloudType>(
        dict, owner, typeName
#ifdef FOAM_INJECT_CONSTRUCTOR_HAS_MODELNAME
    ,modelName
#endif
    ),
    interpreter_(
        generalInterpreterWrapper::New(
            owner.mesh(),
            this->coeffDict()
        )
    ),
    parameterStructName_(
        this->coeffDict().lookup("parameterStructName")
    ),
    resultStructName_(
        this->coeffDict().lookup("resultStructName")
    ),
    injectByEvent_(
        readBool(
            this->coeffDict().lookup("injectByEvent")
        )
    ),
    isActive_(false),
    plannedDuration_(1e10)
{
    interpreter().readCode(
        this->coeffDict(),
        "initInjector",
        initInjectorCode_
    );
    if(injectByEvent_) {
        interpreter().readCode(
            this->coeffDict(),
            "doStartInjection",
            doStartInjectionCode_
        );
        interpreter().readCode(
            this->coeffDict(),
            "doStopInjection",
            doStopInjectionCode_
        );
    } else {
        interpreter().readCode(
            this->coeffDict(),
            "injectionDuration",
            injectionDurationCode_
        );
        interpreter().readCode(
            this->coeffDict(),
            "startOfInjectionTime",
            startOfInjectionTimeCode_
        );
    }

    interpreter().readCode(
        this->coeffDict(),
        "parcelsToInject",
        parcelsToInjectCode_
    );
    interpreter().readCode(
        this->coeffDict(),
        "volumeToInject",
        volumeToInjectCode_
    );

    interpreter().readCode(
        this->coeffDict(),
        "prepareParcelData",
        prepareParcelDataCode_
    );
    interpreter().readCode(
        this->coeffDict(),
        "particleProperties",
        particlePropertiesCode_
    );

    Info << "Executing initialization code for "
        << this->owner().name() << endl;
    interpreter().executeCode(
        initInjectorCode_,
        true // set global variables if necessary
    );
}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

template<class CloudType>
Foam::SwakScriptableInjection<CloudType>::~SwakScriptableInjection()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class CloudType>
Foam::generalInterpreterWrapper &Foam::SwakScriptableInjection<CloudType>::interpreter()
{
    return interpreter_();
}

template<class CloudType>
bool Foam::SwakScriptableInjection<CloudType>::active() const
{
    scalar now=this->owner().db().time().value();
    SwakScriptableInjection<CloudType>* wThis=
        const_cast<SwakScriptableInjection<CloudType>*>(this);

    wThis->interpreter().setRunTime(this->owner().db().time());

    if(isActive_) {
        if(injectByEvent_) {
            bool stopInject=wThis->interpreter().evaluateCodeTrueOrFalse(
                doStopInjectionCode_
            );
            if(stopInject) {
                wThis->isActive_=false;
                Info << "Stopping injection" << endl;
            }
        } else {
            if(this->time0_<timeEnd()) {
                Info << "Reached the end" << endl;
                wThis->isActive_=false;
            }
        }
    } else {
        if(injectByEvent_) {
            bool inject=wThis->interpreter().evaluateCodeTrueOrFalse(
                doStartInjectionCode_
            );
            if(inject) {
                Info << "Starting injection" << endl;
                wThis->isActive_=true;
                const_cast<scalar&>(wThis->SOI_)=now;
            }
        } else {
            scalar soiNext=wThis->interpreter().evaluateCodeScalar(
                startOfInjectionTimeCode_
            );
            if(soiNext<now) {
                Info << "Start of injection reached" << endl;
                wThis->isActive_=true;
                const_cast<scalar&>(wThis->SOI_)=soiNext;
                wThis->plannedDuration_=wThis->interpreter().evaluateCodeScalar(
                    injectionDurationCode_
                );
            }
        }
    }
    return isActive_;
}


template<class CloudType>
Foam::scalar Foam::SwakScriptableInjection<CloudType>::timeEnd() const
{
    return this->SOI_+plannedDuration_;
}


template<class CloudType>
void Foam::SwakScriptableInjection<CloudType>::setPositionAndCell
(
    const label parcelI,
    const label nParcels,
    const scalar time,
    vector& position,
    label& cellOwner
#ifdef FOAM_INJECT_SETPOSITION_HAS_TET_PARAMETERS
    ,label& tetFacei
    ,label& tetPti
#endif
)
{
    dictionary d;
    d.add("nParcels",nParcels);
    d.add("parcelI",parcelI);
    interpreter().insertDictionary(
        parameterStructName_,
        d
    );

    interpreter().executeCode(
        particlePropertiesCode_,
        true
    );

    interpreter().extractDictionary(
        resultStructName_,
        particleData_
    );

    scalarField posVals(particleData_["position"]);
    if(posVals.size()<3) {
        FatalErrorIn("void Foam::SwakScriptableInjection<CloudType>::setPositionAndCell")
            << "Expected a list with at least 3 values. Got " << posVals
                << endl
                << exit(FatalError);
    }
    position=vector(posVals[0],posVals[1],posVals[2]);

    cellOwner=this->owner().mesh().findCell(position);
}


template<class CloudType>
void Foam::SwakScriptableInjection<CloudType>::setProperties
(
    const label parcelI,
    const label,
    const scalar,
    typename CloudType::parcelType& parcel
)
{
    // set particle velocity
    scalarField U0Vals(particleData_["U0"]);
    if(U0Vals.size()<3) {
        FatalErrorIn("void Foam::SwakScriptableInjection<CloudType>::setPositionAndCell")
            << "Expected a list with at least 3 values. Got " << U0Vals
                << endl
                << exit(FatalError);
    }
    parcel.U() = vector(U0Vals[0],U0Vals[1],U0Vals[2]);

    // set particle diameter
    parcel.d() = readScalar(particleData_["diameter"]);

    //    Info << parcel << endl;
}


template<class CloudType>
bool Foam::SwakScriptableInjection<CloudType>::fullyDescribed() const
{
    return false;
}


template<class CloudType>
bool Foam::SwakScriptableInjection<CloudType>::validInjection(const label)
{
    return true;
}

template<class CloudType>
#ifdef    FOAM_INJECT_PREPAREFORNEXT_RETURNS_BOOL
bool
#else
void
#endif
Foam::SwakScriptableInjection<CloudType>::prepareForNextTimeStep
(
    const scalar time,
    label& newParcels,
    scalar& newVolume
) {
#ifdef    FOAM_INJECT_PREPAREFORNEXT_RETURNS_BOOL
    bool result=
#endif
    InjectionModel<CloudType>::prepareForNextTimeStep(
        time,
        newParcels,
        newVolume
    );

    dictionary d;
    d.add("newParcels",newParcels);
    d.add("newVolume",newVolume);
    interpreter().insertDictionary(
        parameterStructName_,
        d
    );

    interpreter().executeCode(
        prepareParcelDataCode_,
        true
    );
#ifdef    FOAM_INJECT_PREPAREFORNEXT_RETURNS_BOOL
    return result;
#endif
}

template<class CloudType>
void Foam::SwakScriptableInjection<CloudType>::postInjectCheck
(
    const label parcelsAdded,
    const scalar massAdded
) {
    InjectionModel<CloudType>::postInjectCheck(
        parcelsAdded,
        massAdded
    );
}

// ************************************************************************* //
