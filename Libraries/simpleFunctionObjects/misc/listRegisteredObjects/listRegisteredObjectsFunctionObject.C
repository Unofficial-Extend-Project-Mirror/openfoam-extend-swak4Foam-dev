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
    2008-2011, 2013, 2015-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "listRegisteredObjectsFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "polyMesh.H"
#include "IOmanip.H"
#include "swakTime.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(listRegisteredObjectsFunctionObject, 0);

    addToRunTimeSelectionTable
    (
        functionObject,
        listRegisteredObjectsFunctionObject,
        dictionary
    );

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

listRegisteredObjectsFunctionObject::listRegisteredObjectsFunctionObject
(
    const word &name,
    const Time& t,
    const dictionary& dict
)
:
    simpleFunctionObject(name,t,dict)
{
#ifdef FOAM_FUNCTIONOBJECT_HAS_SEPARATE_WRITE_METHOD_AND_NO_START
    this->start();
#endif
}

bool listRegisteredObjectsFunctionObject::start()
{
    Info << "Content of object registry " << obr().name()
        << " at start" << endl;

    dumpObr();

    return simpleFunctionObject::start();
}

void listRegisteredObjectsFunctionObject::writeSimple()
{
    Info << "Content of object registry " << obr().name()
        << endl;

    dumpObr();
}

void listRegisteredObjectsFunctionObject::dumpObr()
{
    wordList toc(obr().toc());
    sort(toc);

    const string nameConst("Name");
    unsigned int maxNameLen(nameConst.size());
    const string typeConst("Type");
    unsigned int maxTypeLen(typeConst.size());
    const string autoWriteConst("Autowrite");

    forAll(toc,i) {
        const regIOobject &o=*(obr()[toc[i]]);
        if(o.name().size()>maxNameLen) {
            maxNameLen=o.name().size();
        }
        if(o.headerClassName().size()>maxTypeLen) {
            maxTypeLen=o.headerClassName().size();
        }
    }

    Info().width(maxNameLen);
    Info << nameConst.c_str() << " ";
    Info().width(maxTypeLen);
    Info << typeConst.c_str() << " " << autoWriteConst.c_str() << endl;
    for(unsigned int i=0;i<maxNameLen;i++) Info << "=";
    Info << " ";
    for(unsigned int i=0;i<maxTypeLen;i++) Info << "=";
    Info << " ";
    for(unsigned int i=0;i<autoWriteConst.size();i++) Info << "=";
    Info << endl;
    forAll(toc,i) {
        const regIOobject &o=*(obr()[toc[i]]);
        Info().width(maxNameLen);
        Info << o.name().c_str() << " ";
        Info().width(maxTypeLen);
        Info << o.headerClassName().c_str() << " ";
        if(o.writeOpt()==IOobject::AUTO_WRITE) {
            Info << "Yes";
        } else {
            Info << "No";
        }
        Info << endl;
    }
    Info << endl;
}

// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

} // namespace Foam

// ************************************************************************* //
