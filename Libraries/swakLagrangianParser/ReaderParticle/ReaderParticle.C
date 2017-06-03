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

Class
    Foam::ReaderParticle

Description

SourceFiles

Contributors/Copyright:
    2013, 2016-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "ReaderParticle.H"
#include "ReaderParticleCloud.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

void ReaderParticle::writeFields (const Cloud<ReaderParticle> &cGeneral) {
    const ReaderParticleCloud &c(
        dynamicCast<const ReaderParticleCloud>(cGeneral)
    );

    ReaderParticleBaseClass::writeFields(c);

    c.writeData(c.scalarData_);
    c.writeData(c.labelData_);
    c.writeData(c.vectorData_);
    c.writeData(c.tensorData_);
    c.writeData(c.symmTensorData_);
    c.writeData(c.sphericalTensorData_);
}

void ReaderParticle::readFields (ReaderParticleCloud &c) {
    ReaderParticleBaseClass::readFields(c);

    const objectRegistry &obr=c.parent();

    fileNameList files(
        readDir(c.path(obr.time().timeName(),c.local())/c.name())
    );

    forAll(files,i){
        const word name(files[i]);
        if(
            name=="positions"
        ) {
            continue;
        }

        IOobject header(
            name,
            obr.time().timeName(),
            cloud::prefix/c.name(),
            obr,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );

        if(
#ifdef FOAM_HAS_TYPE_HEADER_OK
            header.typeHeaderOk<IOobject>(false)
#else
            header.headerOk()
#endif
        ) {
            const word className(header.headerClassName());

            word name2=name;
            name2.replace("(","_open_");
            name2.replace(")","_close_");

            if(className=="scalarField") {
                c.addScalarField(name);
                if(name!=name2) {
                    c.setValues(
                        name2,
                        c.getValues<scalar>(name)
                    );
                }
            } else if(className=="labelField") {
                c.addLabelField(name);
                if(name!=name2) {
                    c.setValues(
                        name2,
                        c.getValues<label>(name)
                    );
                }
            } else if(className=="vectorField") {
                c.addVectorField(name);
                if(name!=name2) {
                    c.setValues(
                        name2,
                        c.getValues<vector>(name)
                    );
                }
            } else if(className=="tensorField") {
                c.addTensorField(name);
                if(name!=name2) {
                    c.setValues(
                        name2,
                        c.getValues<tensor>(name)
                    );
                }
            } else if(className=="symmTensorField") {
                c.addSymmTensorField(name);
                if(name!=name2) {
                    c.setValues(
                        name2,
                        c.getValues<symmTensor>(name)
                    );
                }
            } else if(className=="sphericalTensorField") {
                c.addSphericalTensorField(name);
                if(name!=name2) {
                    c.setValues(
                        name2,
                        c.getValues<sphericalTensor>(name)
                    );
                }
            // } else if(className=="boolField") {
            //     c.addBoolField(name);
            } else {
                WarningIn("ReaderParticle::readFields")
                    << "File " << header.objectPath() << " has unknown type "
                        << className << ". Skipping"
                        << endl;
            }
        } else {
            WarningIn("ReaderParticle::readFields")
                << "File " << header.objectPath()
                    << " not existing or inproper header"
                    << endl;

        }
    }


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

// ************************************************************************* //
