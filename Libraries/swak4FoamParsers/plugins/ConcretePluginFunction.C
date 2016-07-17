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
    \\  /    A nd           | Copyright (C) 1991-2008 OpenCFD Ltd.
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
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "ConcretePluginFunction.H"

namespace Foam {


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class DriverType>
ConcretePluginFunction<DriverType>::ConcretePluginFunction(
    const DriverType &parentDriver,
    const word &name,
    const word &returnType,
    const string &argumentSpecification
):
    CommonPluginFunction(
        parentDriver,
        name,
        returnType,
        argumentSpecification
    )
{
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class DriverType>
autoPtr<ConcretePluginFunction<DriverType> > ConcretePluginFunction<DriverType>::New (
    const DriverType& driver,
    const word &name
)
{
    if(debug) {
        Info << "ConcretePluginFunction::New looking for "
            << name << " in "
#ifdef FOAM_HAS_SORTED_TOC
            << nameConstructorTablePtr_->sortedToc()
#else
            << nameConstructorTablePtr_->toc()
#endif
            << endl;
    }
    if(nameConstructorTablePtr_==NULL) {
        FatalErrorIn("ConcretePluginFunction<DriverType>::New")
            << "Constructor table of plugin functions for "
                << DriverType::typeName << " is not initialized"
                << endl
                << exit(FatalError);
        }
    typename nameConstructorTable::iterator cstrIter =
        nameConstructorTablePtr_->find(name);
    if(cstrIter==nameConstructorTablePtr_->end()) {
        FatalErrorIn(
            "autoPtr<ConcretePluginFunction> ConcretePluginFunction<"+
            DriverType::typeName+">::New"
        ) << "Unknow plugin function " << name << endl
            << " Available functions are "
#ifdef FOAM_HAS_SORTED_TOC
            << nameConstructorTablePtr_->sortedToc()
#else
            << nameConstructorTablePtr_->toc()
#endif
                << endl
                << exit(FatalError);
    }
    return autoPtr<ConcretePluginFunction>
        (
            cstrIter()(driver,name)
        );
}

template<class DriverType>
bool ConcretePluginFunction<DriverType>::exists (
    const DriverType& driver,
    const word &name
)
{
    static bool firstCall=true;
    if(firstCall) {
        firstCall=false;

        if(nameConstructorTablePtr_==NULL) {
            WarningIn("ConcretePluginFunction<DriverType>::exists")
                << "Constructor table of plugin functions for "
                    << DriverType::typeName << " is not initialized"
                    << endl;
            return false;
        }
        if(nameConstructorTablePtr_->size()>0) {
            Info<< endl << "Loaded plugin functions for '"+
                DriverType::typeName+"':" << endl;
            wordList names(
#ifdef FOAM_HAS_SORTED_TOC
                nameConstructorTablePtr_->sortedToc()
#else
                nameConstructorTablePtr_->toc()
#endif
            );
            forAll(names,nameI)
            {
                const word &theName=names[nameI];
                typename nameConstructorTable::iterator iter =
                    nameConstructorTablePtr_->find(theName);
                Info << "  " << theName << ":" << endl
                    << "    " << iter()(driver,theName)->helpText() << endl;
            }

            Info << endl;
        }
    }

    typename nameConstructorTable::iterator cstrIter =
        nameConstructorTablePtr_->find(name);

    return cstrIter!=nameConstructorTablePtr_->end();
}

// * * * * * * * * * * * * * * * Friend Operators  * * * * * * * * * * * * * //

} // namespace

// ************************************************************************* //
