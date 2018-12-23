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
    2012-2013, 2015-2016, 2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "EvolveCloudFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#ifdef FOAM_MESHOBJECT_GRAVITY
# include "gravityMeshObject.H"
#else
# include "uniformDimensionedFields.H"
#endif

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class CloudType>
EvolveCloudFunctionObject<CloudType>::EvolveCloudFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    functionObject(
        name
    ),
    dict_(dict),
    regionName_(
        dict_.found("region")
        ? word(dict_.lookup("region"))
        : polyMesh::defaultRegion
    ),
    obr_(t.lookupObject<objectRegistry>(regionName_)),
    cloudName_(
        dict.lookup("cloudName")
    ),
    g_("dummy",dimless,vector::zero)
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    ,lastTimeStepExecute_(-1)
#endif
{
    Dbug << "EvolveCloudFunctionObject<CloudType>::EvolveCloudFunctionObject" << endl;

    if(dict_.found("g")) {
        dimensionedVector newG(dict_.lookup("g"));
        g_.dimensions().reset(newG.dimensions());
        g_=newG;
    } else {
        const Time &runTime=t;
        #ifndef FOAM_MESHOBJECT_GRAVITY
        const fvMesh &mesh=dynamic_cast<const fvMesh &>(obr_);
        #endif

        #include "readGravitationalAcceleration.H"
        g_.dimensions().reset(g.dimensions());
        g_=g;
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

// template<class CloudType>
// bool EvolveCloudFunctionObject<CloudType>::start()
// {
//     return true;
// }

template<class CloudType>
template <class FieldType >
const FieldType &EvolveCloudFunctionObject<CloudType>::getField(const word &fieldName)
{
    return obr_.lookupObject<FieldType>(
        word(
            dict_.lookup(fieldName)
        )
    );
}

template<class CloudType>
bool EvolveCloudFunctionObject<CloudType>::execute(bool forceWrite)
{
    Dbug << "EvolveCloudFunctionObject<CloudType>::execute" << endl;

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    if(!cloud_.valid()) {
        this->start();
    }

    if(
        lastTimeStepExecute_
        !=
        obr().time().timeIndex()
    ) {
        lastTimeStepExecute_=obr().time().timeIndex();
    } else {
        return false;
    }
#endif
    cloud_->evolve();

    if(
        obr().time().outputTime()
        ||
        forceWrite
    ) {
        Info << "Writing cloud " << cloud_->name() << endl;
        cloud_->write();
    }

    return true;
}

template<class CloudType>
bool EvolveCloudFunctionObject<CloudType>::read(const dictionary& dict)
{
    Dbug << "EvolveCloudFunctionObject<CloudType>::read" << endl;

    if(!cloud_.valid()) {
        this->start();
    }

    if(dict_!=dict) {
        WarningIn("EvolveCloudFunctionObject<CloudType>::read(const dictionary& dict)")
            << "Can't change the cloud of " << this->name()
                << " during the run"
                << endl;

    }

    return true;
}

} // namespace Foam

// ************************************************************************* //
