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
    2008-2011, 2013, 2015-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2014 David Huckaby <e.david.huckaby@netl.doe.gov>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "volumeFieldFunctionObject.H"
#include "volFields.H"
#include "IOmanip.H"
#include "fvMesh.H"

#include "basicKinematicCloud.H"
#include "objectRegistryUtility.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

template<class T>
bool writeFieldsGeneralFunctionObject::writeField(const word &name) const
{
    if(obr_.foundObject<T>(name)) {
        obr_.lookupObject<T>(name).write();
        return true;
    } else {
        return false;
    }
}

template<class Type>
bool writeFieldsGeneralFunctionObject::writeCloud(const word &name) const
{
    if(obr_.foundObject<Type>(name)) {
        Info << "\twriting Cloud: " << name << endl;
        lookupObject<Type>(obr_,name).write();
        return true;
    } else {
        return false;
    }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// ************************************************************************* //
