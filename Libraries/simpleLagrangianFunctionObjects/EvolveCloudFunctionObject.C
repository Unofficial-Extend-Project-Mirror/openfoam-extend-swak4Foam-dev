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
    2012-2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "EvolveCloudFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

#include "uniformDimensionedFields.H"

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
        ? dict_.lookup("region")
        : polyMesh::defaultRegion
    ),
    obr_(t.lookupObject<objectRegistry>(regionName_)),
    cloudName_(
        dict.lookup("cloudName")
    ),
    g_("dummy",dimless,vector::zero)
{
    if(dict_.found("g")) {
        dimensionedVector newG(dict_.lookup("g"));
        g_.dimensions().reset(newG.dimensions());
        g_=newG;
    } else {
        const Time &runTime=t;
        //        const fvMesh &mesh=dynamicCast<const fvMesh &>(obr_);
        const fvMesh &mesh=dynamic_cast<const fvMesh &>(obr_);

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
