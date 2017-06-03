/*---------------------------------------------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
 =========                   |
 \\      /   F ield          | OpenFOAM: The Open Source CFD Toolbox
  \\    /    O peration      |
   \\  /     A nd            | Copyright (C) 1991-2005 OpenCFD Ltd.
    \\/      M anipulation   |
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

Application
    testReadingAndWritingExpressionResults

Description

Contributors/Copyright:
    2013, 2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

#include "CommonValueExpressionDriver.H"

#include "timeSelector.H"

#include "IFstream.H"
#include "OFstream.H"

#include "printSwakVersion.H"

#include "ExpressionResult.H"
#include "DelayedExpressionResult.H"
#include "StackExpressionResult.H"
#include "StoredStackExpressionResult.H"
#include "StoredExpressionResult.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //
// Main program:

int main(int argc, char *argv[])
{
    Foam::argList::validArgs.append("fileToWrite");

#   include "setRootCase.H"
#   include "createTime.H"
    runTime.functionObjects().off();
#   include "createMesh.H"

    printSwakVersion();

    fileName theFile(args.args()[1]);

    {
        Info << "Writing to " << theFile << endl;

        scalarField sField(42);
        vectorField vField(42);
        forAll(sField,i) {
            sField[i]=i*i;
            vField[i]=vector(i,i*i,i*i*i);
        }
        ExpressionResult sResult(sField);
        ExpressionResult vResult(vField);
        ExpressionResult uResult(1.2);

        StackExpressionResult stackResult;
        StoredStackExpressionResult storedStackResult;
        ExpressionResult &ssResult=storedStackResult;
        DelayedExpressionResult dResult;
        StoredExpressionResult storedResult;
        storedResult=sResult;

        stackResult=uResult;
        stackResult=uResult;
        ssResult=uResult;
        dResult=vResult;
        dResult.storeValue(runTime.time().value());

        runTime++;
        // stackResult.reset();
        // storedStackResult.reset();
        stackResult=uResult;
        stackResult=uResult;
        ssResult=uResult;
        dResult=1.5*vResult;
        dResult.storeValue(runTime.time().value());

        runTime++;
        // stackResult.reset();
        // storedStackResult.reset();
        stackResult=uResult;
        stackResult=uResult;
        ssResult=uResult;
        dResult=2*vResult;
        dResult.storeValue(runTime.time().value());

        IOdictionary data(
            IOobject(
                theFile,
                mesh.time().timeName(),
                mesh
            )
        );

        data.add("sResult",sResult);
        data.add("stackResult",stackResult);
        data.add("storedStackResult",storedStackResult);
        data.add("vResult",vResult);
        data.add("uResult",uResult);
        data.add("dResult",dResult);
        data.add("storedResult",storedResult);

        data.write();
    }

    {
        Info << "Reading from " << theFile << endl;

        IOdictionary data(
            IOobject(
                theFile,
                mesh.time().timeName(),
                mesh,
                IOobject::MUST_READ
            )
        );

        ExpressionResult sResult(data.subDict("sResult"));
        ExpressionResult vResult(data.subDict("vResult"));
        ExpressionResult uResult(data.subDict("uResult"));
        StackExpressionResult stackResult(data.subDict("stackResult"));
        StoredStackExpressionResult storedStackResult(data.subDict("storedStackResult"));
        DelayedExpressionResult dResult(data.subDict("dResult"));
        StoredExpressionResult storedResult(data.subDict("storedResult"));
    }
    {
        scalarField data(2,0);
        data[0]=1;

        ExpressionResult uResult(data);
        Info << "To write: " << uResult << endl;
        OStringStream o;
        o << uResult;
        IStringStream i(o.str());
        ExpressionResult test;
        i >> test;
        Info << "Read: " << test << endl;;
    }

    if(Pstream::parRun()) {
        bool test2=true;
        reduce(test2,andOp<bool>());
        Pout << "Go on" << endl;

        Info << endl << "Test hand implemented scatter" << endl << endl;
        {
            // compy/pasted from gatherScatter.C

            const List<Pstream::commsStruct>& comms=
                Pstream::linearCommunication();

            ExpressionResult Value;
            if(Pstream::master()) {
                scalarField data(2,0);
                data[0]=1;

                Value=ExpressionResult(data);
            }

            // Get my communication order
            const Pstream::commsStruct& myComm = comms[Pstream::myProcNo()];

            Pout << "From above: " << myComm.above()
                    << endl;

            // Reveive from up
            if (myComm.above() != -1)
            {
                if (contiguous<ExpressionResult>())
                {
                    IPstream::read
                        (
                            Pstream::scheduled,
                            myComm.above(),
                            reinterpret_cast<char*>(&Value),
                            sizeof(ExpressionResult)
                        );
                }
                else
                {
                    ::sleep(1);
                    Pout << "Receiving" << endl;
                    IPstream fromAbove(Pstream::scheduled, myComm.above(),0,IOstream::BINARY);
                    //                    fromAbove.format(IOstream::ASCII);
                    string incoming="";
                    // while(!fromAbove.eof()) {
                    //     char c;
                    //     fromAbove.read(c);
                    //     incoming+=c;
                    // }
                    fromAbove >> incoming;
                    Pout << "Incoming: " << incoming << endl;
                    IStringStream inStream(incoming);
                    inStream >> Value;
                    Pout << "Received" << endl;
                }
            }

            // Send to my downstairs neighbours
            forAll(myComm.below(), belowI)
            {
                Pout << "Below: " << belowI << " " << myComm.below()[belowI]
                    << endl;

                if (contiguous<ExpressionResult>())
                {
                    OPstream::write
                        (
                            Pstream::scheduled,
                            myComm.below()[belowI],
                            reinterpret_cast<const char*>(&Value),
                            sizeof(ExpressionResult)
                        );
                }
                else
                {
                    Pout << "Sending" << endl;
                    OPstream toBelow(Pstream::scheduled,myComm.below()[belowI],0,IOstream::BINARY);
                    OStringStream outgoing;
                    outgoing << Value;
                    Pout << "Outgoing:" << outgoing.str() << endl;
                    toBelow << outgoing.str();
                    Pout << "Sent" << endl;
                }
            }
            Pout << "My scatter: " << Value << endl;
        }
        bool test=true;
        Pout << "Waiting" << endl;
        reduce(test,andOp<bool>());
        Pout << "Go on" << endl;

        Info << endl << "Test scatter" << endl << endl;
        ExpressionResult r;
        if(Pstream::master()) {
            scalarField data(2,0);
            data[0]=1;

            r=ExpressionResult(data);
        }
        Pout << "Pre-Scattered: " << r << endl;
        Pstream::scatter(r);
        Pout << "Scattered: " << r << endl;
    }

    Info << "End\n" << endl;

    return 0;
}


// ************************************************************************* //
