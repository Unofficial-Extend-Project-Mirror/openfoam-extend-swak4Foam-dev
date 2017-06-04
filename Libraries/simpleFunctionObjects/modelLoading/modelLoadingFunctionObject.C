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

#include "modelLoadingFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    //    defineTemplateTypeNameAndDebug(modelLoadingFunctionObject, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template <class ModelType>
modelLoadingFunctionObject<ModelType>::modelLoadingFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict)
{
    Dbug << name << " constructor for "
        << ModelType::typeName << endl;
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    template <class ModelType>
bool modelLoadingFunctionObject<ModelType>::read(const dictionary &)
{
    return this->start();
}
#endif

template <class ModelType>
bool modelLoadingFunctionObject<ModelType>::start()
{
    Dbug << name() << "::start() - entering" << endl;;

    simpleFunctionObject::start();

    correctModel_=readBool(dict_.lookup("correctModel"));
    allowReload_=readBool(dict_.lookup("allowReload"));
    failIfModelTypeExists_=dict_.lookupOrDefault<bool>(
        "failIfModelTypeExists",true
    );

    if(!model_.valid()) {
        Dbug << name() << "::start() - no model" << endl;

        if(
            this->obr().template lookupClass<ModelType>().size()>0
            &&
            failIfModelTypeExists_
        ) {
            FatalErrorIn("modelLoadingFunctionObject<ModelType>::start()")
                << "Model of type " << ModelType::typeName
                    << " in " << this->name()
                    << " already existing. If this is OK overrule this "
                    << "message by setting 'failIfModelTypeExists' to 'false'"
                    << endl
                    << exit(FatalError);

        } else {
            model_.set(initModel().ptr());
            Info << name() << ": loaded a " << ModelType::typeName << endl;
        }
    } else {
        if(allowReload_) {
            model_.set(initModel().ptr());
            Info << name() << ": reloaded a " << ModelType::typeName << endl;
        } else {
            WarningIn("modelLoadingFunctionObject<ModelType>::start()")
                << "Not reloading model because it is not allowed"
                    << endl;
        }
    }

    Dbug << name() << "::start() - exiting" << endl;

    return true;
}

template <class ModelType>
void modelLoadingFunctionObject<ModelType>::writeSimple()
{
    if(correctModel_) {
        if(model_.valid()) {
            Info << "Correcting model for " << this->name() << endl;

            model_->correct();
        } else {
            FatalErrorIn("modelLoadingFunctionObject::start()")
                << "Model has never been intialized"
                    << endl
                    << exit(FatalError);
        }
    }
}

} // namespace Foam

// ************************************************************************* //
