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
    2010-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>
    2012 Bruno Santos <wyldckat@gmail.com>
    2014 Hrvoje Jasak <h.jasak@wikki.co.uk>
    2017 Mark Olesen <Mark.Olesen@esi-group.com>

 SWAK Revision: $Id$
\*---------------------------------------------------------------------------*/

#include "CommonValueExpressionDriver.H"
#include "GlobalVariablesRepository.H"

#include "ExpressionDriverWriter.H"

#include "Random.H"

#include "entryToExpression.H"

#include "dlLibraryTable.H"

namespace Foam {


// * * * * * * * * * * * * * * Static Data Members * * * * * * * * * * * * * //

defineTypeNameAndDebug(CommonValueExpressionDriver,0);

defineRunTimeSelectionTable(CommonValueExpressionDriver, dictionary);
defineRunTimeSelectionTable(CommonValueExpressionDriver, idName);

    // Currently not working
bool CommonValueExpressionDriver::cacheSets_=true;

const fvMesh *CommonValueExpressionDriver::defaultMeshPtr_=NULL;

const fvMesh &CommonValueExpressionDriver::getDefaultMesh()
{
    if(defaultMeshPtr_==NULL) {
        FatalErrorIn("CommonValueExpressionDriver::getDefaultMesh()")
            << "No default mesh set (value is NULL)" << endl
                << "Try using the 'initSwakFunctionObject' to work   around this"
                << endl
                << abort(FatalError);
    }
    return *defaultMeshPtr_;
}

bool CommonValueExpressionDriver::resetDefaultMesh(const fvMesh &mesh)
{
    bool wasSet=defaultMeshPtr_!=NULL;

    defaultMeshPtr_=&mesh;

    return wasSet;
}

bool CommonValueExpressionDriver::setUnsetDefaultMesh(const fvMesh &mesh)
{
    if(defaultMeshPtr_==NULL) {
        Info << "swak4Foam: Setting default mesh" << endl;
        defaultMeshPtr_=&mesh;

        return true;
    } else {
        return false;
    }
}

dictionary CommonValueExpressionDriver::emptyData_("noDictionary");

    // this should be sufficient
const label CommonValueExpressionDriver::maxVariableRecursionDepth_=100;

// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //


CommonValueExpressionDriver::CommonValueExpressionDriver(
    const CommonValueExpressionDriver& orig
)
:
    dict_(orig.dict_),
    variableStrings_(orig.variableStrings_),
    contextString_(orig.contextString_),
    aliases_(orig.aliases_),
    result_(orig.result_),
    variables_(orig.variables_),
    storedVariables_(orig.storedVariables_),
    delayedVariables_(orig.delayedVariables_),
    specialVariablesIndex_(orig.specialVariablesIndex_),
    globalVariableScopes_(orig.globalVariableScopes_),
    lines_(orig.lines_),
    lookup_(orig.lookup_),
#ifdef FOAM_HAS_INTERPOLATION2DTABLE
    lookup2D_(orig.lookup2D_),
#endif
    content_(""),
    trace_scanning_ (orig.trace_scanning_),
    trace_parsing_ (orig.trace_parsing_),
    variableNameIdenticalToField_(orig.variableNameIdenticalToField_),
    scanner_(NULL),
    prevIterIsOldTime_(orig.prevIterIsOldTime_)
{
    Dbug << "CommonValueExpressionDriver - copy constructor" << endl;
    setSearchBehaviour(
        orig.cacheReadFields_,
        orig.searchInMemory_,
        orig.searchOnDisc_
    );
}

CommonValueExpressionDriver::CommonValueExpressionDriver(
    const dictionary& dict
)
:
    dict_(dict),
    variableStrings_(readVariableStrings(dict)),
    contextString_("- From dictionary: "+dict.name()),
    specialVariablesIndex_(-1),
    globalVariableScopes_(dict.lookupOrDefault("globalScopes",wordList())),
    content_(""),
    trace_scanning_ (dict.lookupOrDefault("traceScanning",false)),
    trace_parsing_ (dict.lookupOrDefault("traceParsing",false)),
    variableNameIdenticalToField_(
        dict.lookupOrDefault("variableNameIdenticalToField",false)
    ),
    scanner_(NULL),
    prevIterIsOldTime_(dict.lookupOrDefault("prevIterIsOldTime",false))
{
#ifdef FOAM_HAS_LOCAL_DEBUGSWITCHES
    debug=dict.lookupOrDefault<label>("debugCommonDriver",debug());
#else
    debug=dict.lookupOrDefault<label>("debugCommonDriver",debug);
#endif

    Pbug << "CommonValueExpressionDriver::CommonValueExpressionDriver(const dictionary& dict)" << endl;


    readPluginLibraries(dict);

    if(dict.found("storedVariables")) {
        List<dictionary> variablesSpec(
            dict.lookup("storedVariables")
        );
        storedVariables_.resize(variablesSpec.size());
        forAll(variablesSpec,i) {
            storedVariables_[i]=StoredExpressionResult(
                dictionary(
                    dict,
                    variablesSpec[i]
                )
            );
        }
        Dbug << "Read stored variables:" << storedVariables_ << endl;
    }

    if(dict.found("delayedVariables")) {
        List<DelayedExpressionResult> readDelays(
            dict.lookup("delayedVariables")
        );
        forAll(readDelays,i)
        {
            delayedVariables_.insert(readDelays[i].name(),readDelays[i]);
        }
        Dbug << "Read delayed variables:" << delayedVariables_ << endl;
    }

    setSearchBehaviour(
        dict.lookupOrDefault("cacheReadFields",false),
        dict.lookupOrDefault("searchInMemory",true),
        dict.lookupOrDefault("searchOnDisc",false)
    );

    readTables(dict);
}

void CommonValueExpressionDriver::readPluginLibraries(const dictionary &dict)
{
    if(dict.found("functionPlugins")) {
        wordList pluginNames(dict["functionPlugins"]);

        forAll(pluginNames,i) {

#ifdef FOAM_DLLIBRARY_USES_STATIC_METHODS
            dlLibraryTable::open("libswak"+pluginNames[i]+"FunctionPlugin.so");
#else
            dlLibraryTable table;
            libraries_.open("libswak"+pluginNames[i]+"FunctionPlugin.so");
#endif
        }
    }
}

CommonValueExpressionDriver::CommonValueExpressionDriver(
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc,
    const dictionary &dict
)
:
    dict_(dict),
    variableStrings_(),
    contextString_("- Driver constructed from scratch"),
    specialVariablesIndex_(-1),
    globalVariableScopes_(),
    content_(""),
    trace_scanning_ (false),
    trace_parsing_ (false),
    variableNameIdenticalToField_(false),
    scanner_(NULL),
    prevIterIsOldTime_(false)
{
    setSearchBehaviour(
        cacheReadFields,
        searchInMemory,
        searchOnDisc
    );
}

void CommonValueExpressionDriver::readVariablesAndTables(const dictionary &dict)
{
#ifdef FOAM_HAS_LOCAL_DEBUGSWITCHES
    debug=dict.lookupOrDefault<label>("debugCommonDriver",debug());
#else
    debug=dict.lookupOrDefault<label>("debugCommonDriver",debug);
#endif

    readPluginLibraries(dict);

    if(dict.found("globalScopes")) {
        setGlobalScopes(wordList(dict.lookup("globalScopes")));
    }

    if(dict.found("storedVariables")) {
        if(
            writer_.valid()
            &&
            storedVariables_.size()>0
        ) {
            WarningIn("CommonValueExpressionDriver::readVariablesAndTables")
                << "Context: " << contextString() << endl
                << "Seems like 'storedVariables' was already read."
                    << " No update from "
                    << dict.lookup("storedVariables")
                    << endl;
        } else {
            storedVariables_=List<StoredExpressionResult>(
                dict.lookup(
                    "storedVariables"
                )
            );
        }
    }

    if(dict.found("delayedVariables")) {
        if(
            writer_.valid()
            &&
            delayedVariables_.size()>0
        ) {
            WarningIn("CommonValueExpressionDriver::readVariablesAndTables")
                << "Context: " << contextString() << endl
                << "Seems like 'delayedVariables' was already read."
                    << " No update from "
                    << dict.lookup("delayedVariables")
                    << endl;
        } else {
            List<DelayedExpressionResult> readDelays(
                dict.lookup("delayedVariables")
            );
            forAll(readDelays,i)
            {
                delayedVariables_[readDelays[i].name()]=readDelays[i];
            }
        }
    }

    setVariableStrings(dict);

    readTables(dict);
}

label CommonValueExpressionDriver::readForeignMeshInfo(
        const dictionary &dict,
        bool verbose
) {
    if(!dict.found("foreignMeshes")) {
        if(verbose) {
            Info << "No information about foreign meshes in "
                << dict.name() << nl << endl;
        }
        return 0;
    }

    label cnt=0;

    const dictionary &foreignMeshInfo=dict.subDict("foreignMeshes");
    const wordList names(foreignMeshInfo.toc());
    forAll(names,i) {
        const word &name=names[i];
        if(verbose) {
            Info << "Adding foreign mesh " << name << flush;
        }
        const dictionary &info=foreignMeshInfo.subDict(name);
        const fileName cDir(info.lookup("case"));
        word region=polyMesh::defaultRegion;
        if(info.found("region")) {
            region=word(info.lookup("region"));
        }
        const scalar time(readScalar(info.lookup("time")));
        const meshToMeshOrder interpolationOrder
            =
            meshToMeshInterpolationNames[
                word(info.lookup("interpolationOrder"))
            ];

        if(verbose) {
            Info << " case " << cDir << " region " << region
                << " at t=" << time << " with interpolation order "
                << " interpolation order "
                << word(info.lookup("interpolationOrder")) << endl;
        }

        MeshesRepository::getRepository().addMesh(
            name,
            cDir,
            region
        );
        MeshesRepository::getRepository().setInterpolationOrder(
            name,
            interpolationOrder
        );
        scalar t=MeshesRepository::getRepository().setTime(
            name,
            time
        );
        Info << "Actual mesh time t=" << t << nl << endl;

        cnt++;
    }

    return cnt;
}

void CommonValueExpressionDriver::readTables(const dictionary &dict)
{
    if(dict.found("timelines")) {
        readTables(dict.lookup("timelines"),lines_);
    }

    if(dict.found("lookuptables")) {
        readTables(dict.lookup("lookuptables"),lookup_);
    }

#ifdef FOAM_HAS_INTERPOLATION2DTABLE
    if(dict.found("lookuptables2D")) {
        readTables(dict.lookup("lookuptables2D"),lookup2D_);
    }
#endif

    if(dict.found("aliases")) {
        dictionary aliasDict(dict.subDict("aliases"));
        wordList toc(aliasDict.toc());
        forAll(toc,i) {
            aliases_.insert(
                toc[i],
                word(aliasDict[toc[i]])
            );
        }
        Dbug << "Reading aliases: " << aliases_ << endl;
    }
}

void CommonValueExpressionDriver::setSearchBehaviour(
    bool cacheReadFields,
    bool searchInMemory,
    bool searchOnDisc
)
{
    cacheReadFields_=cacheReadFields;
    searchInMemory_=
        searchInMemory
        ||
        cacheReadFields;
    searchOnDisc_=searchOnDisc;
}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const dictionary& dict
)
{
    return CommonValueExpressionDriver::New(
        dict,
        getDefaultMesh()
    );
}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const dictionary& dict,
    const fvMesh& mesh
)
{
    setUnsetDefaultMesh(mesh);

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
#ifdef FOAM_HAS_SORTED_TOC
            << dictionaryConstructorTablePtr_->sortedToc()
#else
            << dictionaryConstructorTablePtr_->toc()
#endif
            << exit(FatalError);
    }

    Sbug << "Creating driver of type " << driverType << endl;


    return autoPtr<CommonValueExpressionDriver>
    (
        cstrIter()(dict,mesh)
    );
}

autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New
(
    const word& driverType,
    const word& id,
    const fvMesh& mesh
)
{
    setUnsetDefaultMesh(mesh);

    idNameConstructorTable::iterator cstrIter =
        idNameConstructorTablePtr_->find(driverType);

    if (cstrIter == idNameConstructorTablePtr_->end())
    {
        FatalErrorIn
        (
            "autoPtr<CommonValueExpressionDriver> CommonValueExpressionDriver::New"
        )   << "Unknown  CommonValueExpressionDriver type " << driverType
            << endl << endl
            << "Valid valueTypes are :" << endl
#ifdef FOAM_HAS_SORTED_TOC
            << idNameConstructorTablePtr_->sortedToc()
#else
            << idNameConstructorTablePtr_->toc()
#endif
            << exit(FatalError);
    }

    Sbug << "Creating driver of type " << driverType << endl;


    return autoPtr<CommonValueExpressionDriver>
    (
        cstrIter()(id,mesh)
    );
}

// * * * * * * * * * * * * * * * * Destructor  * * * * * * * * * * * * * * * //

CommonValueExpressionDriver::~CommonValueExpressionDriver()
{}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

List<exprString> CommonValueExpressionDriver::readVariableStrings(
    const dictionary &dict,
    const word &name,
    const label recursionDepth
)
{
    Sbug << "::readVariableStrings " << name
        << " depth " << recursionDepth << endl;

    if(!dict.found(name)) {
        return List<exprString>();
    }

    if(recursionDepth>maxVariableRecursionDepth_) {
        FatalErrorIn("CommonValueExpressionDriver::readVariableStrings")
            << "While reading variable list " << name << " in "
                << dict.name() << "the maximum recursion depth "
                << maxVariableRecursionDepth_ << " was exceeded." << nl
                << "Probable cause is that variable lists are referencing "
                << "each other in a circular fashion"
                << endl
                << exit(FatalError);
    }

#ifdef FOAM_HAS_SCOPED_LOOKUP
    const entry *pe=dict.lookupScopedEntryPtr(
        name,
        true, // recursive
        true //  pattern matching
    );
    if(pe==NULL) {
        FatalErrorIn("CommonValueExpressionDriver::readVariableStrings")
            << "Entry " << name << " not found in "
                << dict.name()
                << endl
                << exit(FatalError);
    }
    const entry &e=*pe;
#else
    const entry &e=dict.lookupEntry(
        name,
        true, // recursive
        true //  pattern matching
    );
#endif

    ITstream data(e.stream());
    token nextToken;
    data.read(nextToken);

    List<string> result;

    if(nextToken.isString()) {
        Sbug << name << " is a single string" << endl;
        data.rewind();
        result=expandIncludeStringList(
            List<string>(1,string(data)),
            dict,
            recursionDepth+1
        );
    } else if(
        nextToken.type()==token::PUNCTUATION
        &&
        nextToken.pToken()==token::BEGIN_LIST
    ) {
        Sbug << name << " is a list of strings" << endl;
        data.rewind();
        result=expandIncludeStringList(
            List<string>(data),
            dict,
            recursionDepth+1
        );
    } else if(nextToken.isLabel()) {
        Sbug << name << " is a list of strings with prefix" << endl;
        token anotherToken;
        data.read(anotherToken);
        if(
            anotherToken.type()==token::PUNCTUATION
            &&
            anotherToken.pToken()==token::BEGIN_LIST
        ) {
            data.rewind();
            result=expandIncludeStringList(
                List<string>(data),
                dict,
                recursionDepth+1
            );
        }
    } else {
        FatalErrorIn("CommonValueExpressionDriver::readVariableStrings(const dictionary &dict)")
            << " Entry '"<< name << "' must in dictionary "
                << dict.name() << " either be a string or a list of strings"
                << endl
                << exit(FatalError);

        return List<exprString>();
    }

    List<exprString> toReturn(result.size());

    forAll(result,i) {
        toReturn[i]=exprString(
            result[i],
            dict
        );
    }
    Sbug << "Returns: " << toReturn;
    return toReturn;
}

List<string> CommonValueExpressionDriver::expandIncludeStringList(
    const List<string> &orig,
    const dictionary &dict,
    const label recursionDepth
) {
    Sbug << "::expandIncludeStringList " << orig << endl;

    DynamicList<string> strings;

    forAll(orig,i) {
	string o=orig[i];
        Sbug << "Checking " << o << endl;

        o.removeTrailing(' ');

        std::string::size_type start=0;
        std::string::size_type end=0;
        while(start<o.length()) {
            end=o.find(';',start);
            if(end==std::string::npos) {
            FatalErrorIn("CommonValueExpressionDriver::expandIncludeStringList")
                << "No terminating ';' found in expression '"
                    << o.substr(start) << "' of dictionary "
                    << dict.name() << "\n"
                    << endl
                    << exit(FatalError);
            }
            string sub=o.substr(start,end-start+1);

            Sbug << "Found: " << sub << endl;

            if(sub[0]=='#') {
                std::string::size_type semiPos=sub.find(';');
                assert(semiPos!=std::string::npos);
                string inList=sub.substr(1,semiPos-1);
                Sbug << "Include " << inList << endl;
                List<exprString> expansion(
                    // expansion shoujld happen implicitly here
                    readVariableStrings(
                        dict,
                        inList,
                        recursionDepth
                    )
                );
                Sbug << "Got expansion from " << inList << ": "
                    << expansion << endl;
                forAll(expansion,k){
                    // strings.append(
                    //     expandDictVariables(
                    //         expansion[k],
                    //         dict)
                    // );
                    strings.append(expansion[k]);
                }
            } else {
                strings.append(
                    expandDictVariables(
                        sub,
                        dict
                    )
                );
            }
            start=end+1;
        }
    }

    strings.shrink();
    return List<string>(strings);
}

inline bool is_valid(char c)
{
    return (isalpha(c) || isdigit(c) || (c == '_'));
}

string getEntryString(
    const dictionary &dict,
    const string &replace
) {
#ifdef FOAM_HAS_SCOPED_LOOKUP
    const entry *pe=dict.lookupScopedEntryPtr(
        replace,
        true, // recursive
        true //  pattern matching
    );
    if(pe==NULL) {
        FatalErrorIn("CommonValueExpressionDriver::getEntryString")
            << "Entry " << replace << " not found in "
                << dict.name()
                << endl
                << exit(FatalError);
    }
    const entry &e=*pe;
#else
    const entry &e=dict.lookupEntry(
        replace,
        true, // recursive
        true //  pattern matching
    );
#endif
    if(e.isDict()) {
        FatalErrorIn("getEntryString")
            << "Entry " << replace << " found in dictionary "
                << dict.name() << " but is a dictionary"
                << endl
                << exit(FatalError);
    }
    return entryToExpression::fromEntry(e);
}

exprString CommonValueExpressionDriver::expandDictVariables(
    const string &orig,
    const dictionary &dict
) {
    Sbug << "::expandDictVariables " << orig << endl;

    string result=orig;

    while(result.find('$')!=std::string::npos) {
        std::string::size_type dollarPos=result.find('$');
        if(dollarPos+1>=result.size()) {
            FatalErrorIn("CommonValueExpressionDriver::expandDictVariables")
                << "'$' found at end in " << result << "(originally "
                    << orig << ")"
                    << endl
                    << exit(FatalError);
        }
        std::string::size_type endPos=std::string::npos;

        if(result[dollarPos+1]=='[') {
            // "protected pattern"
            endPos=result.find(']',dollarPos+1);
            if(endPos==std::string::npos) {
                FatalErrorIn("CommonValueExpressionDriver::expandDictVariables")
                    << "No correct terminating ']' found in " << result
                        << " (originally " << orig << ")"
                        << endl
                        << exit(FatalError);
            }
        } else {
            // 'pure' name ... word
            endPos=dollarPos;
            while(
                endPos+1<result.size()
                &&
                is_valid(result[endPos+1])
            ) {
                endPos++;
            }
            if(endPos==dollarPos) {
                FatalErrorIn("CommonValueExpressionDriver::expandDictVariables")
                    << "Not a valid character after the $ in " <<result
                        << "(originally " << orig << ")"
                        << endl
                        << exit(FatalError);
            }
        }

        Sbug << "DollarPos: " << label(dollarPos)
            << " endPos " << label(endPos) << endl;
        string replace=result.substr(dollarPos+1,endPos-dollarPos);
        string replacement="";

        if(replace[0]=='[') {
            string castTo="";
            string entryName="";
            if(replace[1]=='(') {
                std::string::size_type closePos=replace.find(')');
                if(closePos==std::string::npos) {
                    FatalErrorIn("CommonValueExpressionDriver::expandDictVariables")
                        << "No closing ')' found in " << replace
                            << " (" << orig << ")"
                            << endl
                            << exit(FatalError);
                }
                castTo=replace.substr(2,closePos-2);
                entryName=replace.substr(closePos+1,replace.length()-closePos-2);
            } else {
                entryName=replace.substr(1,replace.length()-2);
            }
            if(castTo=="") {
                replacement=getEntryString(
                    dict,
                    entryName
                );
            } else {
                autoPtr<entryToExpression> e2e=entryToExpression::New(castTo);
#ifdef FOAM_HAS_SCOPED_LOOKUP
                const entry *pe=dict.lookupScopedEntryPtr(
                    entryName,
                    true, // recursive
                    true //  pattern matching
                );
                if(pe==NULL) {
                    FatalErrorIn("CommonValueExpressionDriver::expandDictVariables")
                        << "Entry " << entryName << " not found in "
                            << dict.name()
                            << endl
                            << exit(FatalError);
                }
                const entry &e=*pe;
#else
                const entry &e=dict.lookupEntry(
                    entryName,
                    true,
                    true
                );
#endif
                replacement=e2e->toExpr(e);
            }
        } else {
            replacement=getEntryString(
                dict,
                replace
            );
        }
        Sbug << replace << " -> " << replacement << endl;

        string pre=result.substr(0,dollarPos);
        string post=result.substr(endPos+1);

        Sbug << "Pre: " << pre << " Post: " << post << endl;
        result= pre
            + replacement
            + post;
        Sbug << "Resulting: " << result << endl;
    }

    Sbug << orig << " expanded to " << result << endl;

    exprString ret(result.c_str());

    return ret;
}

exprString CommonValueExpressionDriver::readExpression(
    const word &name,
    const dictionary &dict
) {
    Sbug << "::readExpression " << name << endl;

    string result=dict.lookup(name);

    return expandDictVariables(
        result,
        dict
    );
}


exprString CommonValueExpressionDriver::readExpression(
    const word &name
) {
    Dbug << "::readExpression " << name << endl;

    return readExpression(
        name,
        dict()
    );
}

void CommonValueExpressionDriver::setVariableStrings(const dictionary &dict)
{
    variableStrings_=readVariableStrings(dict);
}

Ostream &CommonValueExpressionDriver::writeVariableStrings(Ostream &out) const
{
    if(variableStrings_.size()==0) {
        out << exprString("");
    } else if(variableStrings_.size()==1) {
        out << variableStrings_[0];
    } else {
        out << variableStrings_;
    }

    return out;
}

Ostream &CommonValueExpressionDriver::writeCommon(Ostream &os,bool debug) const
{
    os.writeKeyword("variables");
    writeVariableStrings(os) << token::END_STATEMENT << nl;

    os.writeKeyword("timelines");
    writeTables(os,lines_);
    os << token::END_STATEMENT << nl;

    os.writeKeyword("lookuptables");
    writeTables(os,lookup_);
    os << token::END_STATEMENT << nl;

#ifdef FOAM_HAS_INTERPOLATION2DTABLE
    os.writeKeyword("lookuptables2D");
    writeTables(os,lookup2D_);
    os << token::END_STATEMENT << nl;
#endif

    if(debug) {
        os.writeKeyword("variableValues");
        os << variables_ << endl;
        os << token::END_STATEMENT << nl;
    }

    if(
        storedVariables_.size()>0
        ||
        delayedVariables_.size()>0
    ) {
        const_cast<CommonValueExpressionDriver&>(
            *this
        ).updateSpecialVariables(true);
    }

    if(storedVariables_.size()>0) {
        os.writeKeyword("storedVariables");
        os << storedVariables_ << endl;
        os << token::END_STATEMENT << nl;
    }

    if(delayedVariables_.size()>0) {
        List<DelayedExpressionResult> writeDelays(delayedVariables_.size());

        label cnt=0;
        typedef HashTable<DelayedExpressionResult,word> tableType;
        forAllConstIter(tableType,delayedVariables_,iter)
        {
            writeDelays[cnt]=iter();
            cnt++;
        }

        os.writeKeyword("delayedVariables");
        os << writeDelays << endl;
        os << token::END_STATEMENT << nl;
    }

    if(globalVariableScopes_.size()>0) {
        os.writeKeyword("globalScopes");
        os << globalVariableScopes_ << endl;
        os << token::END_STATEMENT << nl;
    }

    if(aliases_.size()>0) {
        os.writeKeyword("aliases");
        os << token::BEGIN_BLOCK << endl;
        wordList toc(aliases_.toc());
        forAll(toc,i) {
            os.writeKeyword(toc[i]);
            os << aliases_[toc[i]] << token::END_STATEMENT << nl;
        }
        os << token::END_BLOCK << endl;
    }
    return os;
}

word CommonValueExpressionDriver::getResultType()
{
    return result_.valueType();
}

void CommonValueExpressionDriver::clearResult()
{
    result_.clearResult();
}

tmp<vectorField> CommonValueExpressionDriver::composeVectorField(
    const scalarField &x,
    const scalarField &y,
    const scalarField &z
)
{
    if(
        x.size() != y.size()
        ||
        x.size() != z.size()
    ) {
        FatalErrorIn("tmp<vectorField> CommonValueExpressionDriver::composeVectorField")
            << "Context: " << contextString() << endl
            << "Sizes " << x.size() << " " << y.size() << " "
                << z-size() << " of the components do not agree"
                << endl
                << abort(FatalError);
    }

    tmp<vectorField> result(
        new vectorField(x.size())
    );

    forAll(result(),faceI) {
        const_cast<vector&>(result()[faceI])=Foam::vector(x[faceI],y[faceI],z[faceI]);
    }

    return result;
}

tmp<tensorField> CommonValueExpressionDriver::composeTensorField(
    const scalarField &xx,const scalarField &xy,const scalarField &xz,
    const scalarField &yx,const scalarField &yy,const scalarField &yz,
    const scalarField &zx,const scalarField &zy,const scalarField &zz
)
{
    if(
        xx.size() != xy.size()
        ||
        xx.size() != xz.size()
        ||
        xx.size() != yx.size()
        ||
        xx.size() != yy.size()
        ||
        xx.size() != yz.size()
        ||
        xx.size() != zx.size()
        ||
        xx.size() != zy.size()
        ||
        xx.size() != zz.size()
    ) {
        FatalErrorIn("tmp<vectorField> CommonValueExpressionDriver::composeVectorField")
            << "Context: " << contextString() << endl
            << "Sizes of the components do not agree"
                << endl
                << abort(FatalError);
    }

    tmp<tensorField> result(
        new tensorField(xx.size())
    );

    forAll(result(),faceI) {
        const_cast<tensor&>(result()[faceI])=Foam::tensor(
            xx[faceI],xy[faceI],xz[faceI],
            yx[faceI],yy[faceI],yz[faceI],
            zx[faceI],zy[faceI],zz[faceI]
        );

    }

    return result;
}

tmp<symmTensorField> CommonValueExpressionDriver::composeSymmTensorField(
    const scalarField &xx,const scalarField &xy,const scalarField &xz,
    const scalarField &yy,const scalarField &yz,
    const scalarField &zz
)
{
    if(
        xx.size() != xy.size()
        ||
        xx.size() != xz.size()
        ||
        xx.size() != yy.size()
        ||
        xx.size() != yz.size()
        ||
        xx.size() != zz.size()
    ) {
        FatalErrorIn("tmp<vectorField> CommonValueExpressionDriver::composeVectorField")
            << "Context: " << contextString() << endl
            << "Sizes of the components do not agree"
                << endl
                << abort(FatalError);
    }

    tmp<symmTensorField> result(
        new symmTensorField(xx.size())
    );

    forAll(result(),faceI) {
        const_cast<symmTensor&>(result()[faceI])=Foam::symmTensor(
            xx[faceI],xy[faceI],xz[faceI],
            yy[faceI],yz[faceI],
            zz[faceI]
        );

    }

    return result;
}

tmp<sphericalTensorField> CommonValueExpressionDriver::composeSphericalTensorField(
    const scalarField &ii
)
{
    tmp<sphericalTensorField> result(
        new sphericalTensorField(ii.size())
    );

    forAll(result(),faceI) {
        const_cast<sphericalTensor&>(result()[faceI])=Foam::sphericalTensor(
            ii[faceI]
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

ExpressionResult CommonValueExpressionDriver::getUniform(
    label size,
    bool noWarning
)
{
    return result_.getUniform(size,noWarning);
}

void CommonValueExpressionDriver::error (const std::string& m)
{
    FatalErrorIn("parsingValue")
        //        << Foam::args.executable()
            << " Parser Error: " << m
            << Foam::exit(Foam::FatalError);
}

const Time &CommonValueExpressionDriver::runTime() const
{
    return this->mesh().time();
}

tmp<scalarField> CommonValueExpressionDriver::makeModuloField(
    const scalarField &a,
    const scalarField &b
) const
{
    assert(a.size()==b.size());

    tmp<scalarField> result(
        new scalarField(this->size())
    );

    forAll(result(),i) {
        scalar val=fmod(a[i],b[i]);
        if(fabs(val)>(b[i]/2)) {
            if(val>0) {
                val -= b[i];
            } else {
                val += b[i];
            }
        }
        const_cast<scalar&>(result()[i])=val;
    }

    return result;
}

tmp<scalarField> CommonValueExpressionDriver::makeRandomField(label seed) const
{
    tmp<scalarField> result(
        new scalarField(this->size())
    );

    if(seed<=0) {
        seed=runTime().timeIndex()-seed;
    }

    Foam::Random rnd(seed);
    forAll(result(),i) {
#ifdef FOAM_RANDOM_CLASS_NEW_INTERFACE
        const_cast<scalar&>(result()[i]) = rnd.sample01<scalar>();
#else
        const_cast<scalar&>(result()[i]) = rnd.scalar01();
#endif
    }

    return result;
}

tmp<scalarField> CommonValueExpressionDriver::getLine(
    const word &name,
    scalar t
)
{
    return tmp<scalarField>(
        new scalarField(this->size(),lines_[name](t))
    );
}

tmp<scalarField> CommonValueExpressionDriver::getLookup(
    const word &name,
    const scalarField &val
)
{
    tmp<scalarField> result(
        new scalarField(val.size())
    );
    const interpolationTable<scalar> &table=lookup_[name];

    forAll(val,i) {
        const_cast<scalar&>(result()[i])=table(val[i]);
    }

    return tmp<scalarField>(result);
}

tmp<scalarField> CommonValueExpressionDriver::getLookup2D(
    const word &name,
    const scalarField &val,
    const scalarField &val2
)
{
    tmp<scalarField> result(
        new scalarField(val.size())
    );
#ifdef FOAM_HAS_INTERPOLATION2DTABLE
    const interpolation2DTable<scalar> &table=lookup2D_[name];

    forAll(val,i) {
        const_cast<scalar&>(result()[i])=table(val[i],val2[i]);
    }
#else
    FatalErrorIn("CommonValueExpressionDriver::getLookup2D")
        << "This Foam-version has no class interpolation2DTable" << nl
            << "Two-dimensional lookup is therefor not supported"
            << endl
            << exit(FatalError);
#endif
    return tmp<scalarField>(result);
}

scalar CommonValueExpressionDriver::getLineValue(const word &name,scalar t)
{
    return lines_[name](t);
}

tmp<scalarField> CommonValueExpressionDriver::makeGaussRandomField(
    label seed
) const
{
    tmp<scalarField> result(
        new scalarField(this->size())
    );

    if(seed<=0) {
        seed=runTime().timeIndex()-seed;
    }

    Foam::Random rnd(seed);
    forAll(result(),i) {
#ifdef FOAM_RANDOM_CLASS_NEW_INTERFACE
        const_cast<scalar&>(result()[i]) = rnd.GaussNormal<scalar>();
#else
        const_cast<scalar&>(result()[i]) = rnd.GaussNormal();
#endif
    }

    return result;
}

bool CommonValueExpressionDriver::update()
{
    return true;
}

void CommonValueExpressionDriver::updateSpecialVariables(bool force)
{
    Dbug << "CommonValueExpressionDriver::updateSpecialVariables(bool force)"
        << " Force: " << force << endl;

    bool updated=this->update();
    Dbug << "Updated: " << updated << endl;

    if(specialVariablesIndex_<0) {
        Pbug << "First update: " << mesh().time().timeIndex() << endl;

        specialVariablesIndex_=mesh().time().timeIndex();
        forAll(storedVariables_,i) {
            StoredExpressionResult &v=storedVariables_[i];
            Pbug << v.name() << " = " << v.initialValueExpression() << " (has value "
                << v.hasValue() << ")" << endl;
            if(!v.hasValue()) {
                Pbug << "First value: " << v.initialValueExpression()
                    << " -> " << v.name() << endl;

                parse(v.initialValueExpression());
                v=result_;
                Dbug << "Parser size: " << this->size() << endl;
                Dbug << "Calculated: " << result_ << endl;
                Dbug << "Stored: " << v << endl;
            }
        }
    }

    if(
        force
        ||
        specialVariablesIndex_!=mesh().time().timeIndex()
    ) {
        Pbug << "Store variables: " << force << " "
            << specialVariablesIndex_ << " "
            << mesh().time().timeIndex() << endl;

        forAll(storedVariables_,i) {
            StoredExpressionResult &v=storedVariables_[i];
            if(variables_.found(v.name())) {
                Pbug << "Storing variable: " << v.name() << " "
                    << variables_[v.name()] << endl;

                v=variables_[v.name()];
            }
        }
        specialVariablesIndex_=mesh().time().timeIndex();
    }

    typedef HashTable<DelayedExpressionResult,word> tableType;
    forAllIter(tableType,delayedVariables_,iter)
    {
        Pbug << "Updating delayed variable " << iter().name() << endl;

        if(!iter().updateReadValue(mesh().time().value())) {
            const exprString &expr=iter().startupValueExpression();
            Pbug << "Evaluate: " << expr << endl;

            parse(expr);
            iter().setReadValue(result_);
            Pbug << "Value " << iter() << endl;
            Pbug << "Type " << iter().valueType() << "("
                << result_.valueType() << ")" << endl;
        } else {
            Pbug << iter().name() << " updated without problem" << endl;
        }
    }
}

void CommonValueExpressionDriver::clearVariables()
{
    Pbug << "Clearing variables" << endl;


    this->update();

    updateSpecialVariables();
    variables_.clear();
    forAll(storedVariables_,i) {
        StoredExpressionResult &v=storedVariables_[i];
        variables_.insert(v.name(),v);
    }

    addVariables(variableStrings_,false);

    typedef HashTable<DelayedExpressionResult,word> tableType;
    forAllIter(tableType,delayedVariables_,iter)
    {
        iter().storeValue(mesh().time().value());
    }
}

void CommonValueExpressionDriver::evaluateVariable(
    const word &name,
    const exprString &expr
)
{
    if(
        mesh().foundObject<regIOobject>(name)
        &&
        !variableNameIdenticalToField_
    ) {
        const regIOobject &ob=mesh().lookupObject<regIOobject>(name);

        WarningIn("CommonValueExpressionDriver::evaluateVariable")
            << "Context: " << contextString() << endl
            << "There is a field named " << name << " of type "
                << ob.headerClassName() << " found which may be shadowed "
                << "by the variable of the same name." << nl
                << "This may lead to trouble" << nl
                << "If this is OK set 'variableNameIdenticalToField'"
                << " in the relevant parser" << nl
                << endl;

    }

    parse(expr);
    result_.calcIsSingleValue();

    Pbug << "Evaluating: " << expr << " -> " << name << endl
        << result_;

    if(delayedVariables_.found(name)) {
        Pbug << name << " is delayed" << endl;

        delayedVariables_[name]=result_;
    } else {
        variables_.set(name,ExpressionResult(result_));
    }
    if(variables_.found(name)) {
        Pbug << "Value stored: " << variables_[name] << endl;
    } else {
        Pbug << "Nothing stored ... yet" << endl;
    }
}

void CommonValueExpressionDriver::evaluateVariableRemote(
    const exprString &remoteExpr,
    const word &name,
    const exprString &expr
)
{
    Pbug << "Evaluating remote " << remoteExpr
        << " : " << expr << " -> " << name << endl;

    exprString remote=remoteExpr;
    word regionName="";
    word id="";
    word type="patch";

    std::string::size_type slashPos=remote.find('/');

    if(slashPos!=std::string::npos) {
        regionName=string::validate<word>(
            remote.substr(slashPos+1)
        );
        remote=remote.substr(0,slashPos);
    }

    std::string::size_type quotePos=remote.find('\'');
    if(quotePos!=std::string::npos) {
        id=string::validate<word>(
            remote.substr(quotePos+1)
        );
        type=string::validate<word>(
            remote.substr(0,quotePos)
        );
    } else {
        id=string::validate<word>(
            remote
        );
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
        //        pRegion=&(dynamicCast<const fvMesh&>( // doesn't work with gcc 4.2
        pRegion=&(dynamic_cast<const fvMesh&>(
                      pRegion->time().lookupObject<objectRegistry>(regionName))
        );
    }

    const fvMesh &region=*pRegion;

    autoPtr<CommonValueExpressionDriver> otherDriver=
        CommonValueExpressionDriver::New(
            type,
            id,
            region
        );

    otherDriver->setSearchBehaviour(
        this->cacheReadFields(),
        this->searchInMemory(),
        this->searchOnDisc()
    );
    otherDriver->setGlobalScopes(
        this->globalVariableScopes_
    );
    otherDriver->setAliases(
        this->aliases()
    );

    otherDriver->parse(expr);

    autoPtr<ExpressionResult> otherResult(this->getRemoteResult(otherDriver()));
    otherResult->calcIsSingleValue();

    Pbug << "Remote result: " << otherResult() << endl;

    if(delayedVariables_.found(name)) {
        Pbug << name << " is delayed - setting" << endl;

        delayedVariables_[name]=otherResult();
    } else {
        variables_.insert(name,otherResult());
    }
}

autoPtr<ExpressionResult> CommonValueExpressionDriver::getRemoteResult(
        CommonValueExpressionDriver &otherDriver
)
{
    return autoPtr<ExpressionResult>(
        new ExpressionResult(
            otherDriver.getUniform(this->size(),false)
        )
    );
}

void CommonValueExpressionDriver::addVariables(
    const List<exprString> &exprList,
    bool clear
)
{
    if(clear) {
        clearVariables();
    }
    forAll(exprList,i) {
        addVariables(exprList[i],false);
    }
}

void CommonValueExpressionDriver::addVariables(
    const exprString &exprListIn,
    bool clear
)
{
    exprString exprList(exprListIn);
    exprList.removeTrailing(' ');

    if(clear) {
        clearVariables();
    }

    std::string::size_type start=0;
    std::string::size_type end=0;

    while(start<exprList.length()) {
        end=exprList.find(';',start);
        if(end==std::string::npos) {
            FatalErrorIn("CommonValueExpressionDriver::addVariables(const string &exprList,bool clear)")
                << "Context: " << contextString() << endl
                << "No terminating ';' found in expression '"
                    << exprList.substr(start) << "'\n"
                    << endl
                    << exit(FatalError);
        }
        std::string::size_type  eqPos=exprList.find('=',start);
        if(eqPos==std::string::npos || eqPos > end) {
            FatalErrorIn("CommonValueExpressionDriver::addVariables(const string &exprList,bool clear)")
                << "Context: " << contextString() << endl
                << "No '=' found in expression '"
                    << exprList.substr(start,end-start) << "'\n"
                    << endl
                    << exit(FatalError);
        }
        exprString expr(
            exprString::toExpr(
                exprList.substr(eqPos+1,end-eqPos-1)
            )
        );

        std::string::size_type  startPos=exprList.find('{',start);
        if(startPos!=std::string::npos && startPos<eqPos) {
            std::string::size_type  endPos=exprList.find('}',start);
            if(endPos>=eqPos) {
                FatalErrorIn("CommonValueExpressionDriver::addVariables")
                    << "Context: " << contextString() << endl
                    << "No closing '}' found in "
                        << exprList.substr(start,eqPos-start)
                        << endl
                        << exit(FatalError);
            }
            word name(
                string::validate<word>(
                    exprList.substr(start,startPos-start)
                )
            );
            exprString remoteExpr(
                exprString::toExpr(
                    exprList.substr(startPos+1,endPos-startPos-1)
                )
            );
            evaluateVariableRemote(remoteExpr,name,expr);
        } else {
            word name(exprList.substr(start,eqPos-start));
            evaluateVariable(name,expr);
        }
        start=end+1;
    }
}

template<class TableType>
void CommonValueExpressionDriver::readTables(
    Istream &is,
    HashTable<TableType> &tables,
    bool clear
)
{
    if(clear) {
        tables.clear();
    }
    List<dictionary> lines(is);

    forAll(lines,i) {
        const dictionary &dict=lines[i];
        tables.insert(dict.lookup("name"),TableType(dict));
    }
}

template<class TableType>
void CommonValueExpressionDriver::writeTables(
    Ostream &os,
    const HashTable<TableType> &tables
) const
{
    os << token::BEGIN_LIST << nl;
    forAllConstIter(typename HashTable<TableType>,tables,it) {
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
    const fvMesh &mesh,
    bool readIfNecessary
)
{
    if(!dict.found("region")) {
        Sbug << "Using original mesh " << endl;

        return mesh;
    }

    Sbug << "Using mesh " << dict.lookup("region")  << endl;

    if(
        !mesh.time().foundObject<objectRegistry>(
            dict.lookup("region")
        )
        &&
        readIfNecessary
    ) {
        WarningIn("CommonValueExpressionDriver::regionMesh")
            << "Region " << dict.lookup("region")
                << " not in memory. Trying to register it"
                << endl;

        autoPtr<polyMesh> temporary(
            new fvMesh
            (
                IOobject(
                    word(dict.lookup("region")),
                    mesh.time().constant(),
                    mesh.time(),
                    IOobject::MUST_READ
                )
            )
        );
        //        Info << "Hepp: "<< temporary->polyMesh::ownedByRegistry() << endl;
        temporary->polyMesh::store(temporary.ptr());
    }

    //     return dynamicCast<const fvMesh&>( // soesn't work with gcc 3.2
    return dynamic_cast<const fvMesh&>(
        mesh.time().lookupObject<objectRegistry>(
            dict.lookup("region")
        )
    );
}

word CommonValueExpressionDriver::getTypeOfField(const word &name) const
{
    return getTypeOfFieldInternal(mesh(),name);
}

word CommonValueExpressionDriver::getTypeOfFieldInternal(
    const fvMesh &theMesh,
    const word &name
) const
{
    IOobject f
        (
            name,
            theMesh.time().timeName(),
            theMesh,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
#ifdef FOAM_HAS_TYPE_HEADER_OK
    f.typeHeaderOk<IOobject>(false);
#else
    f.headerOk();
#endif

    Pbug << "Mesh: " << theMesh.polyMesh::path()
        << " Name: " << name << " Time: " << mesh().time().timeName()
        << " Path: "
#ifdef FOAM_HAS_LOCAL_FILEPATH
        << f.localFilePath()
#else
        << f.filePath()
#endif
        << " Class: " << f.headerClassName() << endl;

    return f.headerClassName();
}

word CommonValueExpressionDriver::getTypeOfSet(const word &inName) const
{
    word name(inName);
    if(this->hasAlias(name)) {
        Pbug << "CommonValueExpressionDriver::getTypeOfSet. Name: " << name
            << " is an alias for " << this->getAlias(name) << endl;

        name=this->getAlias(name);
    }

    Pbug << "Looking for set named " << name << endl;

    IOobject f
        (
            name,
            mesh().time().timeName(),
            polyMesh::meshSubDir/"sets",
	    mesh(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );

    if(
#ifdef FOAM_HAS_TYPE_HEADER_OK
        f.typeHeaderOk<IOobject>(false)
#else
        f.headerOk()
#endif
    ) {;
        return f.headerClassName();
    } else {
        Pout << "No set " << name << " at t=" << mesh().time().timeName()
            << " falling back to 'constant'" << endl;
        f=IOobject
        (
            name,
            "constant",
            polyMesh::meshSubDir/"sets",
	    mesh(),
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        );
#ifdef FOAM_HAS_TYPE_HEADER_OK
        f.typeHeaderOk<IOobject>(false);
#else
        f.headerOk();
#endif

        return f.headerClassName();
    }
}

bool CommonValueExpressionDriver::isCellSet(const word &name)
{
    if(getTypeOfSet(name)=="cellSet") {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isCellZone(const word &name)
{
    if(mesh().cellZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isFaceSet(const word &name)
{
    if(getTypeOfSet(name)=="faceSet") {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isFaceZone(const word &name)
{
    if(mesh().faceZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isPointSet(const word &name)
{
    if(getTypeOfSet(name)=="pointSet") {
        return true;
    } else {
        return false;
    }
}

bool CommonValueExpressionDriver::isPointZone(const word &name)
{
    if(mesh().pointZones().findZoneID(name)>=0) {
        return true;
    } else {
        return false;
    }
}

void CommonValueExpressionDriver::setTrace(
    bool scanning,
    bool parsing
)
{
    trace_parsing_=parsing;
    trace_scanning_=scanning;
}

label CommonValueExpressionDriver::parse(
    const exprString &f,
    const word &start
)
{
    Dbug << "::parse() - expr: " << f << " start: " << start << endl;

    int start_token=startupSymbol(start);

    parserLastPos()=-1;

    content_ = f;
    scan_begin ();

    Dbug << "Start parsing" << endl;
    parseInternal(start_token);
    Dbug << "Parsed to " << parserLastPos() << " of " << label(f.size()) << endl;

    scan_end ();
    Dbug << "Ended at position " << parserLastPos() << endl;

    return parserLastPos();
}


void CommonValueExpressionDriver::outputResult(Ostream &o)
{
    word rType=getResultType();

    if(rType==pTraits<scalar>::typeName) {
        o << getResult<scalar>();
    } else if(rType==pTraits<vector>::typeName) {
        o << getResult<vector>();
    } else if(rType==pTraits<tensor>::typeName) {
        o << getResult<tensor>();
    } else if(rType==pTraits<symmTensor>::typeName) {
        o << getResult<symmTensor>();
    } else if(rType==pTraits<sphericalTensor>::typeName) {
        o << getResult<sphericalTensor>();
    } else {
        o << "No implementation for " << rType;
    }
}

string CommonValueExpressionDriver::outputEntry()
{
    OStringStream o;

    word rType=getResultType();
    if(rType==pTraits<scalar>::typeName) {
        result_.getResult<scalar>(true)().writeEntry("",o);
    } else if(rType==pTraits<vector>::typeName) {
        result_.getResult<vector>(true)().writeEntry("",o);
    } else if(rType==pTraits<tensor>::typeName) {
        result_.getResult<tensor>(true)().writeEntry("",o);
    } else if(rType==pTraits<symmTensor>::typeName) {
        result_.getResult<symmTensor>(true)().writeEntry("",o);
    } else if(rType==pTraits<sphericalTensor>::typeName) {
        result_.getResult<sphericalTensor>(true)().writeEntry("",o);
    } else {
        o << "No implementation for " << rType << ";";
    }

    return o.str();
}

const word CommonValueExpressionDriver::time() const
{
    return this->mesh().time().timeName();
}

const ExpressionResult &CommonValueExpressionDriver::lookupGlobal(
    const word &name
) const
{
    const ExpressionResult &result(
        GlobalVariablesRepository::getGlobalVariables(
            this->mesh()
        ).get(
            name,
            globalVariableScopes_
        )
    );

    // Pout << name << " Size: " << result.size()
    //     << " Single: " << result.isSingleValue() << endl;

    return result;
}

void CommonValueExpressionDriver::setGlobalScopes(const wordList &other)
{
    globalVariableScopes_=other;
}

void CommonValueExpressionDriver::createWriterAndRead(const word &name)
{
    if(
        hasDataToWrite()
        &&
        !writer_.valid()
    ) {
        writer_.set(
            new ExpressionDriverWriter(
                name+"_"+this->type(),
                *this
            )
        );
    }
}

void CommonValueExpressionDriver::tryWrite() const
{
    if(
        writer_.valid()
        &&
        mesh().time().outputTime()
    ) {
        writer_->write();
    }
}

bool CommonValueExpressionDriver::hasDataToWrite() const
{
    if(
        storedVariables_.size()>0
        ||
        delayedVariables_.size()>0
    ) {
        return true;
    }

    return false;
}

void CommonValueExpressionDriver::getData(const dictionary &dict)
{
    Dbug << "CommonValueExpressionDriver::getData(const dictionary &dict)" << endl;

    if(dict.found("storedVariables")) {
        storedVariables_=List<StoredExpressionResult>(
            dict.lookup("storedVariables")
        );
    }
}

void CommonValueExpressionDriver::prepareData(dictionary &dict) const
{
    Dbug << "CommonValueExpressionDriver::prepareData(dictionary &dict)" << endl;
    bool updated=const_cast<CommonValueExpressionDriver&>(*this).update();
    if(debug && updated) {
        Dbug << "Updated before write" << endl;
    }

    if(storedVariables_.size()>0) {
        const_cast<CommonValueExpressionDriver&>(
            *this
        ).updateSpecialVariables(true);

        dict.add(
            "storedVariables",
            storedVariables_
        );
    }
}

class littlerOp {
public:
    bool operator()(scalar a,scalar b) {
        return a<b;
    }
};

class biggerOp {
public:
    bool operator()(scalar a,scalar b) {
        return a>b;
    }
};


template<class Op>
class  extremeOp {
    Op &op;
public:
    extremeOp(Op &op)
        : op(op)
        {}
    Tuple2<scalar,vector> operator()(
        const Tuple2<scalar,vector> &a,
        const Tuple2<scalar,vector> &b
    ) const {
        if(op(a.first(),b.first())) {
            return a;
        } else {
            return b;
        }
    }
};

template<class Op>
vector getExtremePosition(
    Op op,
    const scalarField &vals,
    const vectorField &locs
) {
    assert(vals.size()==locs.size());

    vector pos(pTraits<scalar>::max,pTraits<scalar>::max,pTraits<scalar>::max);
    scalar val=op(1,-1) ? pTraits<scalar>::min : pTraits<scalar>::max;
    forAll(vals,i) {
        if(op(vals[i],val)) {
            val=vals[i];
            pos=locs[i];
        }
    }
    if(Pstream::parRun()) {
        Tuple2<scalar,vector> info(val,pos);

        reduce(info,extremeOp<Op>(op));

        pos=info.second();
    }
    return pos;
}

vector CommonValueExpressionDriver::getPositionOfMinimum(
    const scalarField &vals,
    const vectorField &locs
) const
{
    return getExtremePosition(littlerOp(),vals,locs);
}

vector CommonValueExpressionDriver::getPositionOfMaximum(
    const scalarField &vals,
    const vectorField &locs
) const
{
    return getExtremePosition(biggerOp(),vals,locs);

}

std::string CommonValueExpressionDriver::getContextString()
{
    if(contextString_=="") {
        return contextString_;
    }

    std::string result("\nContext of the error:\n\n");
    result+="\n"+contextString_;
    result+="\n  Evaluating expression \""+content()+"\"\n";

    return result;
}

void CommonValueExpressionDriver::appendToContext(const string &value)
{
    contextString_=value+"\n"+contextString_;
}

// ************************************************************************* //

const ExpressionResult &CommonValueExpressionDriver::variable(
    const word &name
) const
{
    if(delayedVariables_.found(name)) {
        return delayedVariables_[name];
    } else {
        return variables_[name];
    }
}

ExpressionResult &CommonValueExpressionDriver::variable(
    const word &name
)
{
    if(delayedVariables_.found(name)) {
        return delayedVariables_[name];
    } else {
        return variables_[name];
    }
}

bool CommonValueExpressionDriver::hasVariable(
    const word &name
) const
{
    Pbug << "(var:" << delayedVariables_.found(name)
        << " " << variables_.found(name) << ")";

    if(delayedVariables_.found(name)) {
        return true;
    } else {
        return variables_.found(name);
    }
}

bool CommonValueExpressionDriver::isForeignMesh(
    const word &name
) const
{
    return MeshesRepository::getRepository().hasMesh(name);
}

tmp<scalarField> CommonValueExpressionDriver::weights(
        label size,
        bool point
    ) const
{
    if(point) {
        const label pSize=this->pointSize();
        bool isCorrect=(size==pSize);
        reduce(isCorrect,andOp<bool>());
        if(!isCorrect) {
            Pout << "Expected Size: " << size << " PointSize:" << pSize << endl;
            FatalErrorIn("CommonValueExpressionDriver::weights()")
                << "Context: " << contextString() << endl
                << "At least one processor wants the wrong field size. "
                    << "Check above"
                    << endl
                    << exit(FatalError);
        }
        // points have weight 1 per default
        tmp<scalarField> result(
            new scalarField(
                size,
                1.
            )
        );
        return result;
    } else {
        return this->weightsNonPoint(size);
    }
}

bool CommonValueExpressionDriver::hasAlias(const word &name) const
{
    Dbug << "CommonValueExpressionDriver::hasAlias " << name
        << " : " << aliases_.found(name) << " of "
        << aliases_.size() << endl;

    return aliases_.found(name);
}

const word &CommonValueExpressionDriver::getAlias(const word &name) const
{
    if(!aliases_.found(name)){
        FatalErrorIn("CommonValueExpressionDriver::getAlias(const word &name) const")
            << "Context: " << contextString() << endl
            << "No alias of name " << name << " found." << endl
                << "Available aliases are " << aliases_.toc()
                << endl
                << exit(FatalError);
        return word::null;

    } else {
        return aliases_[name];
    }
}

const word &CommonValueExpressionDriver::resolveAlias(const word &name) const {
    if(hasAlias(name)) {
        return getAlias(name);
    }
    return name;
}

bool CommonValueExpressionDriver::addAlias(
    const word &name,
    const word &field,
    bool overwrite
)
{
    bool isThere=hasAlias(name);

    if(
        isThere
        &&
        !overwrite
    ) {
        FatalErrorIn("CommonValueExpressionDriver::addAlias")
            << name << " already an alias and no overwrite specified"
                << endl
                << exit(FatalError);
    }

    aliases_.set(name,field);

    return !isThere;
}

} // namespace
