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

#include "PatchValueExpressionDriver.H"

#include "FieldValueExpressionDriver.H"

#include "Random.H"

namespace Foam {

// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


PatchValueExpressionDriver::PatchValueExpressionDriver(const PatchValueExpressionDriver& orig)
:
    patch_(orig.patch_),
    result_(orig.result_),
    variables_(orig.variables_),
    lines_(orig.lines_),
    trace_scanning (orig.trace_scanning),
    content(""),
    trace_parsing (orig.trace_parsing)
{}

PatchValueExpressionDriver::PatchValueExpressionDriver(const fvPatch& patch)
:
    patch_(patch),
    trace_scanning (false),
    content(""),
    trace_parsing (false)
{}

PatchValueExpressionDriver::PatchValueExpressionDriver(const fvPatch& patch,const PatchValueExpressionDriver& old)
:
    patch_(patch),
    lines_(old.lines_),
    trace_scanning (old.trace_scanning),
    content(""),
    trace_parsing (old.trace_parsing)
{}


// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

PatchValueExpressionDriver::~PatchValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

word PatchValueExpressionDriver::getResultType()
{
    return result_.type();
}

void PatchValueExpressionDriver::clearResult()
{
    result_.clearResult();
}

vectorField *PatchValueExpressionDriver::composeVectorField(scalarField *x,scalarField *y,scalarField *z)
{
    vectorField *result=new vectorField(patch_.size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::vector((*x)[faceI],(*y)[faceI],(*z)[faceI]);
    }

    return result;
}

tensorField *PatchValueExpressionDriver::composeTensorField(
    scalarField *xx,scalarField *xy,scalarField *xz,
    scalarField *yx,scalarField *yy,scalarField *yz,
    scalarField *zx,scalarField *zy,scalarField *zz
)
{
    tensorField *result=new tensorField(patch_.size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::tensor(
            (*xx)[faceI],(*xy)[faceI],(*xz)[faceI],
            (*yx)[faceI],(*yy)[faceI],(*yz)[faceI],
            (*zx)[faceI],(*zy)[faceI],(*zz)[faceI]
        );
        
    }

    return result;
}

symmTensorField *PatchValueExpressionDriver::composeSymmTensorField(
    scalarField *xx,scalarField *xy,scalarField *xz,
    scalarField *yy,scalarField *yz,
    scalarField *zz
)
{
    symmTensorField *result=new symmTensorField(patch_.size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::symmTensor(
            (*xx)[faceI],(*xy)[faceI],(*xz)[faceI],
            (*yy)[faceI],(*yz)[faceI],
            (*zz)[faceI]
        );
        
    }

    return result;
}

sphericalTensorField *PatchValueExpressionDriver::composeSphericalTensorField(
    scalarField *ii
)
{
    sphericalTensorField *result=new sphericalTensorField(patch_.size());

    forAll(*result,faceI) {
        (*result)[faceI]=Foam::sphericalTensor(
            (*ii)[faceI]
        );
        
    }

    return result;
}

void PatchValueExpressionDriver::parse (const std::string& f)
{
    content = f;
    scan_begin ();
    parsers::PatchValueExpressionParser parser (*this);
    parser.set_debug_level (trace_parsing);
    parser.parse ();
    scan_end ();
}

PatchResult PatchValueExpressionDriver::getUniform(label size,bool noWarning)
{
    return result_.getUniform(size,noWarning);
}

void PatchValueExpressionDriver::error (const parsers::location& l, const std::string& m)
{
    std::ostringstream buff;
    buff << l;
    std::string place="";
    for(unsigned int i=0;i<l.begin.column;i++) {
        place+=" ";
    }
    for(unsigned int i=l.begin.column;i<l.end.column;i++) {
        place+="^";
    }
    for(unsigned int i=l.end.column;i<content.size();i++) {
        place+=" ";
    }

    Foam::FatalErrorIn("parsingValue")
        //        << Foam::args.executable()
        << " Parser Error at " << buff.str() << " :"  << m << Foam::endl
            << content << Foam::endl << place
            << Foam::exit(Foam::FatalError);
    //    Foam::Info << buff.str() << ": " << m << Foam::endl;
}

void PatchValueExpressionDriver::error (const std::string& m)
{
    Foam::FatalErrorIn("parsingValue")
        //        << Foam::args.executable()
            << " Parser Error: " << m
            << Foam::exit(Foam::FatalError);
}

const Time &PatchValueExpressionDriver::runTime() const
{
    return patch_.boundaryMesh().mesh().time();
}

vectorField *PatchValueExpressionDriver::makePositionField()
{
    return new vectorField(patch_.Cf());
}

vectorField *PatchValueExpressionDriver::makePointField()
{
    return new vectorField(patch_.patch().localPoints());
}

vectorField *PatchValueExpressionDriver::makeFaceNormalField()
{
    return new vectorField(patch_.nf());
}

vectorField *PatchValueExpressionDriver::makeFaceAreaField()
{
    return new vectorField(patch_.Sf());
}

vectorField *PatchValueExpressionDriver::makeCellNeighbourField()
{
    return new vectorField(patch_.Cn());
}

vectorField *PatchValueExpressionDriver::makeDeltaField()
{
    return new vectorField(patch_.delta());
}

scalarField *PatchValueExpressionDriver::makeWeightsField()
{
    return new scalarField(patch_.weights());
}

scalarField *PatchValueExpressionDriver::makeModuloField(const scalarField &a,const scalarField &b)
{
    assert(a.size()==b.size());

    scalarField *result=new scalarField(patch_.size());
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

scalarField *PatchValueExpressionDriver::makeFaceIdField()
{
    scalarField *result=new scalarField(patch_.size());
    forAll(*result,i) {
        (*result)[i]=i;
    }
    return result;
}

scalarField *PatchValueExpressionDriver::makeRandomField(label seed)
{
    scalarField *result=new scalarField(patch_.size());

    if(seed<=0) {
        seed=runTime().timeIndex();
    }

    Foam::Random rand(seed);
    forAll(*result,i) {
        (*result)[i]=rand.scalar01();
    }

    return result;
}

scalarField *PatchValueExpressionDriver::getLine(const string &name,scalar t)
{
    return new scalarField(patch_.size(),lines_[name](t));
}

scalarField *PatchValueExpressionDriver::makeGaussRandomField(label seed)
{
    scalarField *result=new scalarField(patch_.size());

    if(seed<=0) {
        seed=runTime().timeIndex();
    }

    Foam::Random rand(seed);
    forAll(*result,i) {
        (*result)[i]=rand.GaussNormal();
    }

    return result;
}

void PatchValueExpressionDriver::clearVariables()
{
    variables_.clear();
}

void PatchValueExpressionDriver::evaluateVariable(const word &name,const string &expr)
{
    parse(expr);
    variables_.insert(name,PatchResult(result_));
}

void PatchValueExpressionDriver::evaluateVariableRemote(const string &remoteExpr,const word &name,const string &expr)
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

    const fvMesh *pRegion=&(patch_.boundaryMesh().mesh());

    if(regionName!="") {
        pRegion=&(dynamicCast<const fvMesh&>(
                      pRegion->time().lookupObject<objectRegistry>(regionName))
        );
    }

    const fvMesh &region=*pRegion;

    if(type=="patch") {
        label patchI=region.boundaryMesh().findPatchID(id);
        if(patchI<0) {
            FatalErrorIn("PatchValueExpressionDriver::evaluateVariableRemote(const word &patchName,const word &name,const string &expr)")
                << " This mesh does not have a patch named " << id
                    << endl
                    << abort(FatalError);
        }
        const fvPatch &otherPatch=region.boundary()[patchI];
        PatchValueExpressionDriver otherDriver(otherPatch);
        otherDriver.parse(expr);
        variables_.insert(name,otherDriver.getUniform(patch_.size(),false));
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

        PatchResult result;

        if(fieldDriver.resultIsVector()) {
            result.setResult(
                fieldDriver.getVector()->internalField()
            );            
        } else if(fieldDriver.resultIsScalar()) {
            result.setResult(
                fieldDriver.getScalar()->internalField()
            );            
        } else {
            WarningIn("PatchValueExpressionDriver::evaluateVariableRemote")
                << "Expression '" << expr 
                    << "' evaluated to an unsupported type"
                    << endl;
        }
        variables_.insert(name,result.getUniform(patch_.size(),false));
    } else if(type=="cellSet") {
        notImplemented("type 'cellSet' not yet implemented");
    } else if(type=="cellZone") {
        notImplemented("type 'cellZone' not yet implemented");
    } else if(type=="faceSet") {
        notImplemented("type 'faceSet' not yet implemented");
    } else if(type=="faceZone") {
        notImplemented("type 'faceZone' not yet implemented");
    } else {
        FatalErrorIn("PatchValueExpressionDriver::evaluateVariableRemote")
            << "The type '" << type << "' is not implemented. " 
                << "Valid types are 'patch', 'internalField', 'cellSet', 'cellZone', 'faceSet' and 'faceZone'"
                << endl
                << abort(FatalError);
    }
}

void PatchValueExpressionDriver::addVariables(const string &exprList,bool clear)
{
    if(clear) {
        clearVariables();
    }

    std::string::size_type start=0;
    std::string::size_type end=0;

    while(start<exprList.length()) {
        end=exprList.find(';',start);
        if(end==std::string::npos) {
            FatalErrorIn("PatchValueExpressionDriver::addVariables(const string &exprList,bool clear)")
                << "No terminating ';' found in expression '"
                    << exprList.substr(start) << "'\n"
                    << endl
                    << abort(FatalError);            
        }
        std::string::size_type  eqPos=exprList.find('=',start);
        if(eqPos==std::string::npos || eqPos > end) {
            FatalErrorIn("PatchValueExpressionDriver::addVariables(const string &exprList,bool clear)")
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
                FatalErrorIn("PatchValueExpressionDriver::addVariables")
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

void PatchValueExpressionDriver::readLines(Istream &is,bool clear)
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

void PatchValueExpressionDriver::writeLines(Ostream &os) const
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

// ************************************************************************* //

} // namespace
