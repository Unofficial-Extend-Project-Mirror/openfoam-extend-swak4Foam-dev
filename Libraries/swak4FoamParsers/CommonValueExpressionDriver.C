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

 ICE Revision: $Id$ 
\*---------------------------------------------------------------------------*/

#include "CommonValueExpressionDriver.H"

#include "FieldValueExpressionDriver.H"

#include "PatchValueExpressionDriver.H"

#include "CellZoneValueExpressionDriver.H"

#include "CellSetValueExpressionDriver.H"

#include "FaceZoneValueExpressionDriver.H"

#include "Random.H"

namespace Foam {


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CommonValueExpressionDriver, 0);
defineRunTimeSelectionTable(CommonValueExpressionDriver, dictionary);

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


CommonValueExpressionDriver::CommonValueExpressionDriver(const CommonValueExpressionDriver& orig)
:
    variableString_(""),
    result_(orig.result_),
    variables_(orig.variables_),
    lines_(orig.lines_),
    content_(""),
    trace_scanning_ (orig.trace_scanning_),
    trace_parsing_ (orig.trace_parsing_)
{}

CommonValueExpressionDriver::CommonValueExpressionDriver(const dictionary& dict)
:
    variableString_(dict.lookupOrDefault("variables",string(""))),
    content_(""),
    trace_scanning_ (dict.lookupOrDefault("traceScanning",false)),
    trace_parsing_ (dict.lookupOrDefault("traceParsing",false))
{
    if(debug) {
        Info << "CommonValueExpressionDriver::CommonValueExpressionDriver(const dictionary& dict)" << endl;
    }

    if(dict.found("timelines")) {
        readLines(dict.lookup("timelines"));
    }
    //    addVariables(variableString_);
}

CommonValueExpressionDriver::CommonValueExpressionDriver()
:
    variableString_(""),
    content_(""),
    trace_scanning_ (false),
    trace_parsing_ (false)
{}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const dictionary& dict,
    const fvMesh& mesh
)
{
    word driverType(dict.lookup("valueType"));
    dictionaryConstructorTable::iterator cstrIter =
        dictionaryConstructorTablePtr_->find(driverType);

    if (cstrIter == dictionaryConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New"
        )   << "Unknown  CommonValueExpressionDriver type " << driverType
            << endl << endl
            << "Valid valueTypes are :" << endl
            << dictionaryConstructorTablePtr_->sortedToc()
            << exit(FatalError);
    }

    if(debug) {
        Info << "Creating driver of type " << driverType << endl;
    }

    return autoPtr<CommonValueExpressionDriver>
    (
        cstrIter()(dict,mesh)
    );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CommonValueExpressionDriver::~CommonValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word CommonValueExpressionDriver::getResultType()
{
    return result_.type();
}

void CommonValueExpressionDriver::clearResult()
{
    result_.clearResult();
}

vectorField *CommonValueExpressionDriver::composeVectorField(scalarField *x,scalarField *y,scalarField *z)
{
    vectorField *result=new vectorField(this->size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::vector((*x)[faceI],(*y)[faceI],(*z)[faceI]);
    }

    return result;
}

tensorField *CommonValueExpressionDriver::composeTensorField(
    scalarField *xx,scalarField *xy,scalarField *xz,
    scalarField *yx,scalarField *yy,scalarField *yz,
    scalarField *zx,scalarField *zy,scalarField *zz
)
{
    tensorField *result=new tensorField(this->size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::tensor(
            (*xx)[faceI],(*xy)[faceI],(*xz)[faceI],
            (*yx)[faceI],(*yy)[faceI],(*yz)[faceI],
            (*zx)[faceI],(*zy)[faceI],(*zz)[faceI]
        );
        
    }

    return result;
}

symmTensorField *CommonValueExpressionDriver::composeSymmTensorField(
    scalarField *xx,scalarField *xy,scalarField *xz,
    scalarField *yy,scalarField *yz,
    scalarField *zz
)
{
    symmTensorField *result=new symmTensorField(this->size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::symmTensor(
            (*xx)[faceI],(*xy)[faceI],(*xz)[faceI],
            (*yy)[faceI],(*yz)[faceI],
            (*zz)[faceI]
        );
        
    }

    return result;
}

sphericalTensorField *CommonValueExpressionDriver::composeSphericalTensorField(
    scalarField *ii
)
{
    sphericalTensorField *result=new sphericalTensorField(this->size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::sphericalTensor(
            (*ii)[faceI]
        );
        
    }

    return result;
}

    /* void CommonValueExpressionDriver::parse (const std::string& f)
{
    content_ = f;
    scan_begin ();
    parserCommon::CommonValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing);
    parser.parse ();
    scan_end ();
}
        */

ExpressionResult CommonValueExpressionDriver::getUniform(label size,bool noWarning)
{
    return result_.getUniform(size,noWarning);
}

void CommonValueExpressionDriver::error (const std::string& m)
{
    Foam::FatalErrorIn("parsingValue")
        //        << Foam::args.executable()
            << " Parser Error: " << m
            << Foam::exit(Foam::FatalError);
}

const Time &CommonValueExpressionDriver::runTime() const
{
    return this->mesh().time();
}

scalarField *CommonValueExpressionDriver::makeModuloField(const scalarField &a,const scalarField &b)
{
    assert(a.size()==b.size());

    scalarField *result=new scalarField(this->size());
    forAll(*result,i) {
        scalar val=fmod(a[i],b[i]);
        if(fabs(val)>(b[i]/2)) {
            if(val>0) {
                val -= b[i];
            } else {
                val += b[i];
            }
        }
        (*result)[i]=val;
    }

    return result;
}

scalarField *CommonValueExpressionDriver::makeRandomField(label seed)
{
    scalarField *result=new scalarField(this->size());

    if(seed<=0) {
        seed=runTime().timeIndex();
    }

    Foam::Random rand(seed);
    forAll(*result,i) {
        (*result)[i]=rand.scalar01();
    }

    return result;
}

scalarField *CommonValueExpressionDriver::getLine(const string &name,scalar t)
{
    return new scalarField(this->size(),lines_[name](t));
}

scalarField *CommonValueExpressionDriver::makeGaussRandomField(label seed)
{
    scalarField *result=new scalarField(this->size());

    if(seed<=0) {
        seed=runTime().timeIndex();
    }

    Foam::Random rand(seed);
    forAll(*result,i) {
        (*result)[i]=rand.GaussNormal();
    }

    return result;
}

void CommonValueExpressionDriver::clearVariables()
{
    variables_.clear();
    if(variableString_!="") {
        addVariables(variableString_,false);
    }
}

void CommonValueExpressionDriver::evaluateVariable(const word &name,const string &expr)
{
    parse(expr);
    variables_.insert(name,ExpressionResult(result_));
}

void CommonValueExpressionDriver::evaluateVariableRemote(const string &remoteExpr,const word &name,const string &expr)
{
    string remote=remoteExpr;
    word regionName="";
    word id="";
    word type="patch";

    std::string::size_type slashPos=remote.find('/');

    if(slashPos!=std::string::npos) {
        regionName=remote.substr(slashPos+1);
        remote=remote.substr(0,slashPos);
    }
    
    std::string::size_type quotePos=remote.find('\'');
    if(quotePos!=std::string::npos) {
        id=remote.substr(quotePos+1);
        type=remote.substr(0,quotePos);        
    } else {
        id=remote;
    }

    if(
        type=="patch" 
        && 
        (id=="internalField" || id=="")
    ) {
        type="internalField";
    }

    const fvMesh *pRegion=&(this->mesh());

    if(regionName!="") {
        pRegion=&(dynamicCast<const fvMesh&>(
                      pRegion->time().lookupObject<objectRegistry>(regionName))
        );
    }

    const fvMesh &region=*pRegion;

    if(type=="patch") {
        label patchI=region.boundaryMesh().findPatchID(id);
        if(patchI<0) {
            FatalErrorIn("CommonValueExpressionDriver::evaluateVariableRemote(const word &patchName,const word &name,const string &expr)")
                << " This mesh does not have a patch named " << id
                    << endl
                    << abort(FatalError);
        }
        const fvPatch &otherPatch=region.boundary()[patchI];
        PatchValueExpressionDriver otherDriver(otherPatch);
        otherDriver.parse(expr);
        variables_.insert(name,otherDriver.getUniform(this->size(),false));
    } else if(type=="internalField") {
        FieldValueExpressionDriver fieldDriver(
            region.time().timeName(),
            region.time(),
            region,
            false,
            true,
            false
        );
        fieldDriver.parse(expr);

        ExpressionResult result;

        if(fieldDriver.resultIsVector()) {
            result.setResult(
                fieldDriver.getVector()->internalField()
            );            
        } else if(fieldDriver.resultIsScalar()) {
            result.setResult(
                fieldDriver.getScalar()->internalField()
            );            
        } else {
            WarningIn("CommonValueExpressionDriver::evaluateVariableRemote")
                << "Expression '" << expr 
                    << "' evaluated to an unsupported type"
                    << endl;
        }
        variables_.insert(name,result.getUniform(this->size(),false));
    } else if(type=="cellSet") {
        cellSet otherSet(
            region,
            id,
            IOobject::MUST_READ
        );
        CellSetValueExpressionDriver otherDriver(otherSet);
        otherDriver.parse(expr);
        variables_.insert(name,otherDriver.getUniform(this->size(),false)); 
    } else if(type=="cellZone") {
        label zoneI=region.cellZones().findZoneID(id);
        if(zoneI<0) {
            FatalErrorIn("CommonValueExpressionDriver::evaluateVariableRemote(const word &patchName,const word &name,const string &expr)")
                << " This mesh does not have a cellZone named " << id
                    << endl
                    << abort(FatalError);
        }
        const cellZone &otherZone=region.cellZones()[zoneI];
        CellZoneValueExpressionDriver otherDriver(otherZone);
        otherDriver.parse(expr);
        variables_.insert(name,otherDriver.getUniform(this->size(),false));        
    } else if(type=="faceSet") {
        notImplemented("type 'faceSet' not yet implemented");
    } else if(type=="faceZone") {
        label zoneI=region.faceZones().findZoneID(id);
        if(zoneI<0) {
            FatalErrorIn("CommonValueExpressionDriver::evaluateVariableRemote(const word &patchName,const word &name,const string &expr)")
                << " This mesh does not have a faceZone named " << id
                    << endl
                    << abort(FatalError);
        }
        const faceZone &otherZone=region.faceZones()[zoneI];
        FaceZoneValueExpressionDriver otherDriver(otherZone);
        otherDriver.parse(expr);
        variables_.insert(name,otherDriver.getUniform(this->size(),false));        
    } else {
        FatalErrorIn("CommonValueExpressionDriver::evaluateVariableRemote")
            << "The type '" << type << "' is not implemented. " 
                << "Valid types are 'patch', 'internalField', 'cellSet', 'cellZone', 'faceSet' and 'faceZone'"
                << endl
                << abort(FatalError);
    }
}

void CommonValueExpressionDriver::addVariables(const string &exprList,bool clear)
{
    if(clear) {
        clearVariables();
    }

    std::string::size_type start=0;
    std::string::size_type end=0;

    while(start<exprList.length()) {
        end=exprList.find(';',start);
        if(end==std::string::npos) {
            FatalErrorIn("CommonValueExpressionDriver::addVariables(const string &exprList,bool clear)")
                << "No terminating ';' found in expression '"
                    << exprList.substr(start) << "'\n"
                    << endl
                    << abort(FatalError);            
        }
        std::string::size_type  eqPos=exprList.find('=',start);
        if(eqPos==std::string::npos || eqPos > end) {
            FatalErrorIn("CommonValueExpressionDriver::addVariables(const string &exprList,bool clear)")
                << "No '=' found in expression '"
                    << exprList.substr(start,end-start) << "'\n"
                    << endl
                    << abort(FatalError);            
        }
        string expr(exprList.substr(eqPos+1,end-eqPos-1));

        std::string::size_type  startPos=exprList.find('{',start);
        if(startPos!=std::string::npos && startPos<eqPos) {
            std::string::size_type  endPos=exprList.find('}',start);
            if(endPos!=(eqPos-1)) {
                FatalErrorIn("CommonValueExpressionDriver::addVariables")
                    << "No closing '}' found in " 
                        << exprList.substr(start,eqPos-start)
                        << endl
                        << abort(FatalError);
            }
            word name(exprList.substr(start,startPos-start));
            string remoteExpr(exprList.substr(startPos+1,endPos-startPos-1));
            evaluateVariableRemote(remoteExpr,name,expr);
        } else {
            word name(exprList.substr(start,eqPos-start));
            evaluateVariable(name,expr);
        }
        start=end+1;
    }
}

void CommonValueExpressionDriver::readLines(Istream &is,bool clear)
{
    if(clear) {
        lines_.clear();
    }
    List<dictionary> lines(is);

    forAll(lines,i) {
        const dictionary &dict=lines[i];
        lines_.insert(dict.lookup("name"),interpolationTable<scalar>(dict));
    }
}

void CommonValueExpressionDriver::writeLines(Ostream &os) const
{
    os << token::BEGIN_LIST << nl;
    forAllConstIter(HashTable<interpolationTable<scalar> >,lines_,it) {
        os << token::BEGIN_BLOCK << nl;
        os.writeKeyword("name") << it.key() << token::END_STATEMENT << nl;
        (*it).write(os);
        os << token::END_BLOCK << nl;
    }
    os << token::END_LIST;
}

const fvMesh &CommonValueExpressionDriver::regionMesh
(
    const dictionary &dict,
    const fvMesh &mesh
)
{
    if(!dict.found("region")) {
        if(debug) {
            Info << "Using original mesh " << endl;
        }

        return mesh;
    }

    if(debug) {
        Info << "Using mesh " << dict.lookup("region")  << endl;
    }
    
    return dynamicCast<const fvMesh&>(
        mesh.time().lookupObject<objectRegistry>(
            dict.lookup("region")
        )
    );    
}

// ************************************************************************* //

} // namespace
