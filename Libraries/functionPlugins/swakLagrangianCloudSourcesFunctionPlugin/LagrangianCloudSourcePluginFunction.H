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

Class
    Foam::LagrangianCloudSourcePluginFunction

Description

SourceFiles
    LagrangianCloudSourcePluginFunction.C

Contributors/Copyright:
    2012-2013, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#ifndef LagrangianCloudSourcePluginFunction_H
#define LagrangianCloudSourcePluginFunction_H

#include "FieldValuePluginFunction.H"

#include "runTimeSelectionTables.H"


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                           Class LagrangianCloudSourcePluginFunction Declaration
\*---------------------------------------------------------------------------*/

    // template<class CloudType>
class LagrangianCloudSourcePluginFunction
:
    public FieldValuePluginFunction
{
    //- Disallow default bitwise assignment
    void operator=(const LagrangianCloudSourcePluginFunction &);

    LagrangianCloudSourcePluginFunction(const LagrangianCloudSourcePluginFunction &);

    word cloudName_;

    wordHashSet supportedTypes_;

protected:
    //    typedef CloudType cloudType;

    template<class LookupCloudType>
    bool hasCloudAs();

    Ostream& listAvailableClouds(Ostream &o);

    template<class OtherCloudType,class LookupCloudType>
    const OtherCloudType &getCloudAs();

    const word &cloudName() const { return cloudName_; }

    const wordHashSet &supportedTypes() const
        { return supportedTypes_; }

public:
    LagrangianCloudSourcePluginFunction(
        const FieldValueExpressionDriver &parentDriver,
        const word &name,
        const word &returnValueType,
        const string &additionalArgs=""
    );

    virtual ~LagrangianCloudSourcePluginFunction() {}

    //    TypeName("LagrangianCloudSourcePluginFunction");

    void setArgument(label index,const word &);

};



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
#   include "LagrangianCloudSourcePluginFunctionI.H"
#endif

// return a autoPtr to the needed field. This is required because
// "similar" clouds don't really inherit (because of the template paramter)
#define tryCall(rType,neededType,usedType,theCall)    \
    if(hasCloudAs<neededType>()) {                             \
        return autoPtr<rType> (                                \
            new rType(                                         \
                getCloudAs<neededType,usedType>().theCall      \
            )                                                  \
        );                                                     \
    }

// Fail if no cloud data was found
#define noCloudFound(pVar) \
    if(!pVar.valid()) {                                                 \
        listAvailableClouds(Info);                                      \
        FatalErrorIn("Plugin function "+this->name())                   \
            << "No matching cloud " << this->cloudName() << " found"    \
                << endl                                                 \
                << "Supported cloud types (typeid): "                   \
                << supportedTypes() << endl                             \
                << exit(FatalError);                                    \
    }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
