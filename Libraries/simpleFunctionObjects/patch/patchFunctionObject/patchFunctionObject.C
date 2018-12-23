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
    2008-2014, 2016-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2018 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "patchFunctionObject.H"
#include "addToRunTimeSelectionTable.H"

#include "volFields.H"
#include "OStringStream.H"
#include "wordReList.H"
#include "stringListOps.H"

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

namespace Foam
{
    defineTypeNameAndDebug(patchFunctionObject, 0);


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

patchFunctionObject::patchFunctionObject
(
    const word& name,
    const Time& t,
    const dictionary& dict
)
:
    timelineFunctionObject(name,t,dict),
    patchNames_(0),
    patchIndizes_(0)
{
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

bool patchFunctionObject::start()
{
    wordList oldPatchNames(patchNames_);
    wordReList newPatches(dict_.lookup("patches"));
    wordHashSet patchNamesNew;
    bool allowCoupled(dict_.lookupOrDefault<bool>("allowCoupled",false));

    const fvMesh &mesh=refCast<const fvMesh>(obr_);
    wordList allPatches(mesh.boundaryMesh().names());
    forAll(newPatches,i) {
        labelList IDs=findStrings(newPatches[i],allPatches);
        forAll(IDs,j) {
            const word &name=allPatches[IDs[j]];
            bool add=true;
            if(!allowCoupled) {
                label patchI=mesh.boundaryMesh().findPatchID(name);
                const polyPatch &thePatch=mesh.boundaryMesh()[patchI];
                if(thePatch.coupled()) {
                    WarningIn("patchFunctionObject::start()")
                        << "Patch " << name << " in " << dict_.name() << " is coupled." << nl
                            << "Disabling. If you want it enabled set 'allowCoupled true;'"
                            << endl;
                    add=false;
                }
            }
            if(add) {
                patchNamesNew.insert(name);
            }
        }
    }

    patchNames_=patchNamesNew.toc();
    Foam::sort(patchNames_);

    if(Pstream::parRun()) {
        List<wordList> patchNames(Pstream::nProcs());
        patchNames[Pstream::myProcNo()]=patchNames_;
        Pstream::gatherList(patchNames);
        Pstream::scatterList(patchNames);
        bool different=false;
        forAll(patchNames,procI) {
            if(patchNames[procI]!=patchNames[0]) {
                if(!different) {
                    different=true;
                    Info << "Patch Names on master processor: " << patchNames[0] << endl;
                }
                Info << "Different patch names on processor " << procI << ": "
                    << patchNames[procI] << endl;
            }
        }
        if(different && Pstream::master()) {
            FatalErrorInFunction
                << "Inconsistent patch names on at least 1 processor. Probably because patterns in "
                    << newPatches << " are too liberal"
                    << endl
                    << exit(FatalError);
        }
    }
    // the patches changed
    if(patchNames_!=oldPatchNames) {
        closeAllFiles();
    }
    timelineFunctionObject::start();

    patchIndizes_.setSize(patchNames_.size());

    forAll(patchNames_,i) {
        const word &name=patchNames_[i];
        patchIndizes_[i]=mesh.boundaryMesh().findPatchID(name);
        if(patchIndizes_[i]<0) {
            WarningIn("patchFunctionObject::start()")
                << " Patch " << name << " does not exist in patches: "
                    << mesh.boundaryMesh().names() << endl;
        }
    }
    if(debug>1) {
        Pout << "Patch names: " << patchNames_ << nl
            << " Index: " << patchIndizes_ << endl;
    }

    if (Pstream::master())
    {
        if (debug)
        {
            Pout<< "Probing patches:" << patchNames_ << nl
                << endl;
        }
    }

    return true;
}

stringList patchFunctionObject::columnNames()
{
    stringList result(patchNames_.size());
    forAll(result,i) {
        result[i]=patchNames_[i];
    }
    return result;
}

} // namespace Foam

// ************************************************************************* //
