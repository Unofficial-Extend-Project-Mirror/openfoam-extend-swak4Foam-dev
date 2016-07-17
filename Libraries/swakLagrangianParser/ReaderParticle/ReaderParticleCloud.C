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

Contributors/Copyright:
    2013-2014 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "ReaderParticleCloud.H"

#include "GlobalVariablesRepository.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

#ifdef FOAM_PARTICLE_CLASS_IS_TEMPLATE
defineParticleTypeNameAndDebug(ReaderParticle, 0);
#else
defineTypeNameAndDebug(ReaderParticle, 0);
#endif
defineTemplateTypeNameAndDebug(Cloud<ReaderParticle>, 0);

// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

ReaderParticleCloud::ReaderParticleCloud
(
    const polyMesh& mesh,
    const word& cloudName,
    bool readFields
)
:
    Cloud<ReaderParticle>(mesh, cloudName, false)
{
    if (readFields)
    {
        ReaderParticle::readFields(*this);
    }
}

void ReaderParticleCloud::clearData()
{
    labelData_.clear();
    //    boolData_.clear();
    scalarData_.clear();
    vectorData_.clear();
    tensorData_.clear();
    symmTensorData_.clear();
    sphericalTensorData_.clear();
}

template <typename T>
tmp<Field<T> > filterValues(
    const Field<T> &orig,
    const boolList &mask,
    label size
) {
    tmp<Field<T> > pResult(
        new Field<T>(
            size,
            pTraits<T>::zero
        )
    );
    Field<T> &result=pResult();
    label cnt=0;
    forAll(orig,i)
    {
	if(mask[i]) {
            if(cnt>=size) {
                FatalErrorIn("filterValues")
                    << "Mask seems to have more elements than " << size
                        << endl
                        << exit(FatalError);
            }
            result[cnt]=orig[i];
            cnt++;
        }
    }
    if(cnt!=size) {
        FatalErrorIn("filterValues")
            << "Inconsistent amount of elements in mask " << cnt << nl
                << "Need: " << size
                << endl
                << exit(FatalError);

    }
    return pResult;
}


autoPtr<ReaderParticleCloud> ReaderParticleCloud::makeCloudFromVariables(
    const polyMesh &mesh,
    const word &cloudName,
    const wordList & globalNameSpacesToUse,
    const word &positionVar
) {
    GlobalVariablesRepository &repo=GlobalVariablesRepository::getGlobalVariables(
        mesh
    );
    label minSize=pTraits<label>::max;
    label maxSize=pTraits<label>::min;
    bool foundPos=false;
    vectorField positions;
    DynamicList<word> varNames;

    forAll(globalNameSpacesToUse,nsI)
    {
	const word &nsName=globalNameSpacesToUse[nsI];
        GlobalVariablesRepository::ResultTable &ns=repo.getNamespace(nsName);
        forAllIter(GlobalVariablesRepository::ResultTable,ns,iter) {
            ExpressionResult &val=*(*iter);
            const word &name=iter.key();
            minSize=min(minSize,val.size());
            maxSize=max(maxSize,val.size());
            if(name==positionVar) {
                foundPos=true;
                positions=val.getResult<vector>();
            } else {
                varNames.append(name);
            }
        }
    }

    Info << "Variables to be added: " << varNames << endl;

    if(!foundPos) {
        FatalErrorIn("ReaderParticleCloud::makeCloudFromVariables")
            << "No position variable " << positionVar << " found"
                << endl
                << exit(FatalError);

    }
    label globalMin=minSize;
    label globalMax=maxSize;
    reduce(globalMin,minOp<label>());
    reduce(globalMax,maxOp<label>());
    bool ok=(globalMin==minSize && globalMax==maxSize && minSize==maxSize);
    reduce(ok,andOp<bool>());
    if(!ok) {
        Pout << "Minimum variable size: " << minSize << " maximum: "
            << maxSize << endl;
        FatalErrorIn("ReaderParticleCloud::makeCloudFromVariables")
            << "Variable size should be the same on all processors"
                << endl
                << exit(FatalError);

    }

    Info << positions.size() << " particle positions found" << endl;

    autoPtr<ReaderParticleCloud> pCloud(
        new ReaderParticleCloud(
            mesh,
            cloudName
        )
    );
    boolList usePos(positions.size(),false);
    DynamicList<label> swakIDs;

    ReaderParticleCloud &cloud=pCloud();
    forAll(positions,i)
    {
	const vector &pos=positions[i];
        const label cellI=mesh.findCell(pos);
        usePos[i]=(cellI>=0);
        if(usePos[i]) {
            cloud.append(
                new ReaderParticle(
                    mesh,
                    pos,
                    cellI
                )
            );
            swakIDs.append(i);
       }
    }
    swakIDs.shrink();
    label size=swakIDs.size();
    Pout << size << " particle positions used" << endl;
    reduce(size,plusOp<label>());
    Info << "Total of " << size << " positions" << endl;

    cloud.setValues("swakCreationsID",labelField(swakIDs));

    forAll(globalNameSpacesToUse,nsI)
    {
	const word &nsName=globalNameSpacesToUse[nsI];
        GlobalVariablesRepository::ResultTable &ns=repo.getNamespace(nsName);
        forAllIter(GlobalVariablesRepository::ResultTable,ns,iter) {
            ExpressionResult &val=*(*iter);
            const word &name=iter.key();

           if(name!=positionVar) {
               word type=val.valueType();
               Info << "Adding " << name << " of type " << type << endl;

               if(type==pTraits<scalar>::typeName) {
                   cloud.setValues(
                       name,
                       filterValues(
                           val.getResult<scalar>()(),
                           usePos,
                           cloud.size()
                       )()
                   );
               } else if(type==pTraits<label>::typeName) {
                   cloud.setValues(
                       name,
                       filterValues(
                           val.getResult<label>()(),
                           usePos,
                           cloud.size()
                       )()
                   );
               } else if(type==pTraits<vector>::typeName) {
                   cloud.setValues(
                       name,
                       filterValues(
                           val.getResult<vector>()(),
                           usePos,
                           cloud.size()
                       )()
                   );
               } else if(type==pTraits<tensor>::typeName) {
                   cloud.setValues(
                       name,
                       filterValues(
                           val.getResult<tensor>()(),
                           usePos,
                           cloud.size()
                       )()
                   );
               } else if(type==pTraits<symmTensor>::typeName) {
                   cloud.setValues(
                       name,
                       filterValues(
                           val.getResult<symmTensor>()(),
                           usePos,
                           cloud.size()
                       )()
                   );
               } else if(type==pTraits<sphericalTensor>::typeName) {
                   cloud.setValues(
                       name,
                       filterValues(
                           val.getResult<sphericalTensor>()(),
                           usePos,
                           cloud.size()
                       )()
                   );
               } else {
                   FatalErrorIn("ReaderParticleCloud::makeCloudFromVariables")
                       << "Unhandled type " << type
                           << endl
                           << exit(FatalError);
               }
            }
        }
    }

    return pCloud;
}

void ReaderParticleCloud::addScalarField(const word &name)
{
    IOField<scalar> data(this->fieldIOobject(name,IOobject::MUST_READ));
    this->checkFieldIOobject(*this,data);
    addDataInternal(
        name,
        data,
        scalarData_
    );
}

void ReaderParticleCloud::addLabelField(const word &name)
{

    IOField<label> data(this->fieldIOobject(name,IOobject::MUST_READ));
    this->checkFieldIOobject(*this,data);
    addDataInternal(
        name,
        data,
        labelData_
    );
}

void ReaderParticleCloud::addVectorField(const word &name)
{
    IOField<vector> data(this->fieldIOobject(name,IOobject::MUST_READ));
    this->checkFieldIOobject(*this,data);
    addDataInternal(
        name,
        data,
        vectorData_
    );
}

void ReaderParticleCloud::addTensorField(const word &name)
{
    IOField<tensor> data(this->fieldIOobject(name,IOobject::MUST_READ));
    this->checkFieldIOobject(*this,data);
    addDataInternal(
        name,
        data,
        tensorData_
    );
}

void ReaderParticleCloud::addSymmTensorField(const word &name)
{
    IOField<symmTensor> data(this->fieldIOobject(name,IOobject::MUST_READ));
    this->checkFieldIOobject(*this,data);
    addDataInternal(
        name,
        data,
        symmTensorData_
    );
}

void ReaderParticleCloud::addSphericalTensorField(const word &name)
{
    IOField<sphericalTensor> data(this->fieldIOobject(name,IOobject::MUST_READ));
    this->checkFieldIOobject(*this,data);
    addDataInternal(
        name,
        data,
        sphericalTensorData_
    );
}

// void ReaderParticleCloud::addBoolField(const word &name)
// {
//     IOField<bool> data(this->fieldIOobject(name,IOobject::MUST_READ));
//     this->checkFieldIOobject(*this,data);
//     addDataInternal(
//         name,
//         data,
//         boolData_
//     );
// }

};

// ************************************************************************* //
