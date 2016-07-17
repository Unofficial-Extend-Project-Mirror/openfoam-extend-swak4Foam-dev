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
    \\  /    A nd           | Copyright held by original author
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

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
    2014-2015 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "TimeClone.H"
#include "DebugOStream.H"

// Stuff we "know"
#include "volFields.H"
#include "surfaceFields.H"
#include "pointFields.H"

#include"diagTensorIOField.H"
#include"labelIOField.H"
#include"pointIOField.H"
#include"scalarIOField.H"
#include"sphericalTensorIOField.H"
#include"symmTensorIOField.H"
#include"tensorIOField.H"
#include"vector2DIOField.H"
#include"vectorIOField.H"

namespace Foam {
// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(TimeClone, 0);

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

TimeClone::TimeClone()
{
    Dbug << "Construction" << endl;
}


// * * * * * * * * * * * * * * * * Selectors * * * * * * * * * * * * * * * * //

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

TimeClone::~TimeClone()
{
    Dbug << "Destruction" << endl;
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void TimeClone::copy(const Time &t)
{
    Dbug << "copy: t=" << t.timeName() << endl;
    if(storedTime_.valid()) {
        Dbug << "currently holds data: Clearing" << endl;
        storedTime_.clear();
    }

    dictionary settings;
    if(t.writeCompression()==IOstream::COMPRESSED) {
        settings.add("writeCompression","compressed");
    }
    if(t.writeFormat()==IOstream::BINARY) {
        settings.add("writeFormat","binary");
    }
    settings.add("deltaT",t.deltaT().value());
    settings.add("writeInterval",1);

    storedTime_.set(
        new Time(
            settings,
            t.rootPath(),
            t.caseName()
        )
    );
    Dbug << "Reset to new Time-instance" << endl;
    Time &time=storedTime_();
    time.setTime(t);
    time.setDeltaT(t.deltaT());
    Dbug << "Instances: src " << t.instance() << " Dst " << time.instance() << endl;
    Dbug << "Objects" << t.names();

    label nr=copyObjects(t,time);
    Dbug << "Copyied " << nr << " objects" << endl;
}

label TimeClone::copyObjects(const objectRegistry &src,objectRegistry &dst)
{
    Dbug << "Copying stuff from " << src.name() << " to " << dst.name() << endl;
    Dbug << "t=" << src.time().timeName() << endl;
    Dbug << "Dst AUTO_WRITE: " << (dst.writeOpt()==IOobject::AUTO_WRITE) << endl;

    label cnt=0;

    forAllConstIter(objectRegistry,src,it) {
	const word &name=it.key();
        const regIOobject &obj=*(*it);

        Dbug << name << " is class " << obj.headerClassName() << endl;

        if(isA<objectRegistry>(obj)) {
            Dbug << name << " is objectRegistry. Creating new and cloning" << endl;
            const objectRegistry &orig=dynamicCast<const objectRegistry>(obj);
            if(&src==&orig) {
                Dbug << name << "==" << src.name() << " -> Skipping" << endl;
            } else {
                word dbName=orig.name();
                if(dbName==polyMesh::defaultRegion) {
                    dbName="";
                }
                autoPtr<objectRegistry> newSubp(
                    new objectRegistry(
                        IOobject(
                            dbName,
                            src.time().timeName(),
                            src.local(),
                            dst
                        )
                    )
                );
                objectRegistry &newSub=newSubp();
                Dbug << "AUTO_WRITE: " << (newSub.writeOpt()==IOobject::AUTO_WRITE) << endl;
                Dbug << "old path: " << obj.objectPath() << endl;
                Dbug << "new Path: " << newSub.objectPath() << endl;
                Dbug << "Created registry owned by parent: " << newSub.ownedByRegistry() << endl;
                cnt+=copyObjects(orig,newSub);
                dst.store(newSubp.ptr());
                Dbug << "New registry owned by parent: " << newSub.ownedByRegistry() << endl;
            }
        } else if(obj.writeOpt()==IOobject::AUTO_WRITE) {
            Dbug << name << " set to AUTO_WRITE. Creating copy" << endl;
            autoPtr<regIOobject> newObjP;

            // work around because there is no virtual clone method in IObobject

#define tryClone(Type)                                                  \
            if(!newObjP.valid() && isA<Type>(obj)) {                    \
                newObjP.set(                                            \
                    new Type(                                           \
                        IOobject(                                       \
                            obj.name(),                                 \
                            src.time().timeName(),                      \
                            obj.local(),               \
                            dst                                         \
                        ),                                              \
                        dynamicCast<const Type>(obj)));                 \
            }

            tryClone(volScalarField);
            tryClone(volVectorField);
            tryClone(volTensorField);
            tryClone(volSymmTensorField);
            tryClone(volSphericalTensorField);

            tryClone(surfaceScalarField);
            tryClone(surfaceVectorField);
            tryClone(surfaceTensorField);
            tryClone(surfaceSymmTensorField);
            tryClone(surfaceSphericalTensorField);

            tryClone(pointScalarField);
            tryClone(pointVectorField);
            tryClone(pointTensorField);
            tryClone(pointSymmTensorField);
            tryClone(pointSphericalTensorField);

            tryClone(diagTensorIOField);
            tryClone(labelIOField);
            tryClone(pointIOField);
            tryClone(scalarIOField);
            tryClone(sphericalTensorIOField);
            tryClone(symmTensorIOField);
            tryClone(tensorIOField);
            tryClone(vector2DIOField);
            tryClone(vectorIOField);
            //            tryClone(polyBoundaryMesh);

#undef tryClone

            if(newObjP.valid()) {
                regIOobject &newObj=dynamicCast<regIOobject&>(newObjP());
                Dbug << "Adding " << name << " to registry " << dst.name()
                    << " Class: " << newObj.headerClassName() << endl;
                Dbug << "Owned by old Registry: " << newObj.ownedByRegistry() << endl;
                Dbug << "AUTO_WRITE: " << (newObj.writeOpt()==IOobject::AUTO_WRITE) << endl;
                Dbug << "Old Path: " << obj.objectPath() << endl;
                Dbug << "New Path: " << newObj.objectPath() << endl;
                Dbug << "Local: " << obj.local() << " -> " << newObj.local() << endl;
                newObj.writeOpt()=IOobject::AUTO_WRITE;
                regIOobject *ptr=static_cast<regIOobject*>(newObjP.ptr());
                dst.store(ptr);
                Dbug << "Owned by new Registry: " << newObj.ownedByRegistry() << endl;
                cnt++;
            } else {
                Dbug << "No fitting type found for " << name << endl;
            }
        } else {
            Dbug << name << " not copied" << endl;
        }
    }

    Dbug << "Copying to " << dst.name() << " ended " << cnt << endl;
    Dbug << dst.names() << endl;

    return cnt;
}

bool TimeClone::write(const bool force)
{
    Pbug << "write. Force: " << force << endl;
    if(!storedTime_.valid()) {
        Pbug << "Nothing stored -> nothing written" << endl;
        return false;
    }
    Time &time=storedTime_();

    Pout << "Write t=" << time.timeName() << " to " << time.timePath() << endl;

    if(exists(time.timePath())) {
        if(!force) {
            WarningIn("TimeClone::write(const bool force)")
                << time.timePath() << " already existing. Skipping because no 'force' set"
                    << endl;
            Pbug << "Clearing and exiting" << endl;
            storedTime_.clear();
            return false;
        } else {
            WarningIn("TimeClone::write(const bool force)")
                << time.timePath() << " already existing. Writing because 'force' is set"
                    << endl;
        }
    }
    //    objectRegistry::debug=1;

    bool result=time.writeNow();

    Pbug << "written: " << result << " Releasing time" << endl;
    //     objectRegistry::debug=0;
    storedTime_.clear();

    return true;
}

// * * * * * * * * * * * * * * Member Operators  * * * * * * * * * * * * * * //

const Time &TimeClone::operator()() const {
    if(this->ok()) {
        return storedTime_();
    }
    FatalErrorIn("TimeClone::operator()() const")
        << "No stored time. Should not call this"
            << endl
            << exit(FatalError);

    return *static_cast<const Time *>(NULL);
}

// * * * * * * * * * * * * * * Friend Functions  * * * * * * * * * * * * * * //


// * * * * * * * * * * * * * * Friend Operators * * * * * * * * * * * * * * //

} // end namespace

// ************************************************************************* //
