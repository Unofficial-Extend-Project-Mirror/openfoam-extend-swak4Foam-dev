/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
*-----------------------------------------------------------------------------*

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

Class
    Foam::SwakScriptableInjection

Description
    Specify the injection with scripts

SourceFiles
    SwakScriptableInjection.C

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

\*---------------------------------------------------------------------------*/

#ifndef SwakScriptableInjection_H
#define SwakScriptableInjection_H

#include "InjectionModel.H"

#include "generalInterpreterWrapper.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                          Class SwakScriptableInjection Declaration
\*---------------------------------------------------------------------------*/

template<class CloudType>
class SwakScriptableInjection
:
    public InjectionModel<CloudType>
{
    // Private data

    autoPtr<generalInterpreterWrapper> interpreter_;


    string initInjectorCode_;
    string doStartInjectionCode_;
    string doStopInjectionCode_;
    string injectionDurationCode_;
    string startOfInjectionTimeCode_;
    string parcelsToInjectCode_;
    string volumeToInjectCode_;

    string prepareParcelDataCode_;
    string particlePropertiesCode_;

    word parameterStructName_;
    word resultStructName_;

    dictionary particleData_;

    bool injectByEvent_;

    bool isActive_;
    scalar plannedDuration_;

protected:

    generalInterpreterWrapper &interpreter();

    // Protected member functions

        //- Number of parcels to introduce over the time step relative to SOI
        label parcelsToInject
        (
            const scalar time0,
            const scalar time1
        ) const;

        //- Volume of parcels to introduce over the time step relative to SOI
        scalar volumeToInject
        (
            const scalar time0,
            const scalar time1
        ) const;

        //- Return flag to identify whether or not injection of parcelI is
        //  permitted
        bool validInjection(const label parcelI);

        //- Determine properties for next time step/injection interval
        void prepareForNextTimeStep
        (
            const scalar time,
            label& newParcels,
            scalar& newVolume
        );

        //- Post injection checks
        void postInjectCheck
        (
            const label parcelsAdded,
            const scalar massAdded
        );

public:

    //- Runtime type information
    TypeName("SwakScriptableInjection");


    // Constructors

        //- Construct from dictionary
        SwakScriptableInjection
        (
            const dictionary& dict,
            CloudType& owner
        );


    //- Destructor
    ~SwakScriptableInjection();


    // Member Functions

        //- Flag to indicate whether model activates injection model
        bool active() const;

        //- Return the end-of-injection time
        scalar timeEnd() const;


        // Injection geometry

            //- Set the injection position and owner cell
            void setPositionAndCell
            (
                const label parcelI,
                const label nParcels,
                const scalar time,
                vector& position,
                label& cellOwner
            );

            //- Set the parcel properties
            void setProperties
            (
                const label parcelI,
                const label nParcels,
                const scalar time,
                typename CloudType::parcelType& parcel
            );

            //- Flag to identify whether model fully describes the parcel
            bool fullyDescribed() const;

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "SwakScriptableInjection.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
