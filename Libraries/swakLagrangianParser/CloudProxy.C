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
    \\  /    A nd           | Copyright (C) 1991-2010 OpenCFD Ltd.
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is based on OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Contributors/Copyright:
    2012-2013 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CloudProxy.H"

#include "DebugOStream.H"

#include "IOmanip.H"

#include "swak.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{

defineTypeNameAndDebug(CloudProxy, 0);

defineRunTimeSelectionTable(CloudProxy,cloud);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

CloudProxy::CloudProxy
(
    const cloud& c
)
:
    cloud_(
        c
    )
{
}

autoPtr<CloudProxy> CloudProxy::New(
    const cloud &c,
    const word& alternateType
) {
    Sbug << "CloudProxy::New with " << c.name() << " Type: " << c.type() << endl;

    word cloudType(c.type());
    if(
        alternateType!=""
        &&
        cloudType!=alternateType
    ) {
        Sbug << "Using alternate type " << alternateType << endl;

        cloudType=alternateType;
    }

    cloudConstructorTable::iterator cstrIter =
        cloudConstructorTablePtr_->find(cloudType);

    if (cstrIter == cloudConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "autoPtr<CloudProxy> CloudProxy::New"
        )   << "Unknown  cloud type " << cloudType
            << " (cloud: " << c.type() << " alternate: " << alternateType << ")"
            << endl << endl
            << "Valid cloudType are :" << endl
#ifdef FOAM_HAS_SORTED_TOC
            << cloudConstructorTablePtr_->sortedToc() // does not work in 1.6
#else
            << cloudConstructorTablePtr_->toc()
#endif
            << exit(FatalError);
    }

    if(debug) {
        Pout << "Creating cloud proxy of type " << cloudType << endl;
    }

    return autoPtr<CloudProxy>
    (
        cstrIter()(c)
    );
}

autoPtr<CloudProxy> CloudProxy::New(
    const cloud &c,
    const dictionary& dict
) {
    Sbug << "CloudProxy::New - dict with " << c.name() << " Type: " << c.type() << endl;

    word cloudType(c.type());
    if(
        dict.found("alternateCloudType")
    ) {
        cloudType=word(dict.lookup("alternateCloudType"));

        Sbug << "Using alternate type " << cloudType << endl;
    }

    cloudConstructorTable::iterator cstrIter =
        cloudConstructorTablePtr_->find(cloudType);

    if (cstrIter == cloudConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "autoPtr<CloudProxy> CloudProxy::New"
        )   << "Unknown  cloud type " << cloudType
            << " (cloud: " << c.type() <<") " << endl
            << "Consider setting the parameter 'alternateCloudType' in "
            << dict.name() << " to a valid type"
            << endl << endl
            << "Valid cloudType are :" << endl
#ifdef FOAM_HAS_SORTED_TOC
            << cloudConstructorTablePtr_->sortedToc() // does not work in 1.6
#else
            << cloudConstructorTablePtr_->toc()
#endif
            << exit(FatalError);
    }

    if(debug) {
        Pout << "Creating cloud proxy of type " << cloudType << endl;
    }

    return autoPtr<CloudProxy>
    (
        cstrIter()(c)
    );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CloudProxy::~CloudProxy()
{}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool CloudProxy::isScalarField(const word &name) const
{
    return
        fieldTypes_.found(name)
        &&
        fieldTypes_[name]==pTraits<scalar>::typeName;
}

bool CloudProxy::isBoolField(const word &name) const
{
    return
        fieldTypes_.found(name)
        &&
        fieldTypes_[name]==pTraits<bool>::typeName;
}

bool CloudProxy::isVectorField(const word &name) const
{
    return
        fieldTypes_.found(name)
        &&
        fieldTypes_[name]==pTraits<vector>::typeName;
}

bool CloudProxy::isTensorField(const word &name) const
{
    return
        fieldTypes_.found(name)
        &&
        fieldTypes_[name]==pTraits<tensor>::typeName;
}

bool CloudProxy::isSymmTensorField(const word &name) const
{
    return
        fieldTypes_.found(name)
        &&
        fieldTypes_[name]==pTraits<symmTensor>::typeName;
}

bool CloudProxy::isSphericalTensorField(const word &name) const
{
    return
        fieldTypes_.found(name)
        &&
        fieldTypes_[name]==pTraits<sphericalTensor>::typeName;
}

Ostream &operator<<(Ostream &o,const CloudProxy &p)
{
    const int nameWidth=20;
    const int typeWidth=15;

    o << "Driver for cloud " << p.theCloud().name()
        << " of type " << p.theCloud().type() << " (Proxy type: "
        << p.type() << ")" << endl;
    o << "     List of functions:" << endl;
    o << setw(nameWidth) << "Name" << " | " << setw(typeWidth)
        << "Type" << " | Description" << endl;
    o << "--------------------------------------------------------------"
        << endl;
    wordList names(
#ifdef FOAM_HAS_SORTED_TOC
        p.fieldTypes_.sortedToc()
#else
        p.fieldTypes_.toc()
#endif
    );
    forAll(names,i) {
    o << setw(nameWidth) << names[i] << " | " << setw(typeWidth)
        << p.fieldTypes_[names[i]] << " | "
        << p.fieldDescriptions_[names[i]].c_str() << endl;
    }

    return o;
}

} // namespace end

// ************************************************************************* //
