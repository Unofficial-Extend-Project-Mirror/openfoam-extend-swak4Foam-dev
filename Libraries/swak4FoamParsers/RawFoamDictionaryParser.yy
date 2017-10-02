/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-----------------------------------------------------------------------------*

License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description
    Parser that is the framework for transforming any OpenFOAM-dictionary
    into another dictionary-like data structure

    Won't be able to read all possible OF-dictionaries

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {RawFoamDictionaryParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert
// %define api.prefix {parserRawDict}
%define api.namespace {parserRawDict}

%code requires
{
    #include "string.H"
    #include "scalar.H"
    #include "word.H"
    #include "label.H"

    namespace Foam {
        class RawFoamDictionaryParserDriver;
    }

}

%param { Foam::RawFoamDictionaryParserDriver& driver }

%locations
%initial-action
{
    // Initialize the initial location.
    @$.begin.filename = @$.end.filename = &(driver.is().name());
};

%define parse.trace
%define parse.error verbose

%code
{
# include "RawFoamDictionaryParserDriver.H"

    namespace parserRawDict {
        RawFoamDictionaryParser::symbol_type yylex(
            Foam::RawFoamDictionaryParserDriver &driver
        );
    }
}

%define api.token.prefix {TOKEN_}

%token
    END_OF_FILE  0   "end of file"
    END_STATEMENT    ";"
    BEGIN_LIST       "("
    END_LIST         ")"
    BEGIN_SQR        "["
    END_SQR          "]"
    BEGIN_BLOCK      "{"
    END_BLOCK        "}"
    COLON            ":"
    COMMA            ","
     ;

%token <Foam::word> WORD "word"
%token <Foam::scalar> SCALAR "scalar"
%token <Foam::label> LABEL "label"
%token <Foam::string> STRING "string"
%token <bool> BOOL "bool"

// %type  <int> exp

%printer { yyoutput << $$; } <*>;

%%

%start assignments;

assignments:
  %empty                     {}
| assignments assignment     {};

assignment:
  "word" "word" ";"          {}
| "word" "scalar" ";"        {}
| "word" "label" ";"         {}
| "word" "bool" ";"         {}
| "word" dictionary          {}
;

dictionary:
  "{" assignments "}"        {}
%%

void
parserRawDict::RawFoamDictionaryParser::error(
    const location_type& l,
    const std::string& m
)
{
}

parserRawDict::RawFoamDictionaryParser::symbol_type parserRawDict::yylex(
    Foam::RawFoamDictionaryParserDriver &driver
) {
    using namespace Foam;

    RawFoamDictionaryParser::location_type loc(
        &(driver.is().name()),
        driver.is().lineNumber(),
        0
    );

    // Info << nl << driver.is().eof() << " " << driver.is().good()
    //     << driver.is().bad() << endl;

    if(driver.is().eof()) {
        return RawFoamDictionaryParser::make_END_OF_FILE(loc);
    }

    token nextToken(driver.is());

    Info << "Next token: " << nextToken.info() << endl;

    switch(nextToken.type()) {
        case token::WORD:
            if(
                nextToken.wordToken()=="yes"
                ||
                nextToken.wordToken()=="true"
                ||
                nextToken.wordToken()=="on"
            ) {
                return RawFoamDictionaryParser::make_BOOL(
                    true,
                    loc
                );
            }
            if(
                nextToken.wordToken()=="no"
                ||
                nextToken.wordToken()=="false"
                ||
                nextToken.wordToken()=="off"
            ) {
                return RawFoamDictionaryParser::make_BOOL(
                    false,
                    loc
                );
            }
            return RawFoamDictionaryParser::make_WORD(
                nextToken.wordToken(),
                loc
            );
            break;
        case token::STRING:
            return RawFoamDictionaryParser::make_STRING(
                nextToken.stringToken(),
                loc
            );
            break;
        case token::FLOAT_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.floatScalarToken()),
                loc
            );
            break;
        case token::DOUBLE_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.doubleScalarToken()),
                loc
            );
            break;
        case token::LONG_DOUBLE_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.longDoubleScalarToken()),
                loc
            );
            break;
        case token::LABEL:
            return RawFoamDictionaryParser::make_LABEL(
                nextToken.labelToken(),
                loc
            );
            break;
        case token::PUNCTUATION:
            switch(nextToken.pToken()) {
                case token::BEGIN_BLOCK:
                    return RawFoamDictionaryParser::make_BEGIN_BLOCK(loc);
                case token::END_BLOCK:
                    return RawFoamDictionaryParser::make_END_BLOCK(loc);
                case token::BEGIN_LIST:
                    return RawFoamDictionaryParser::make_BEGIN_LIST(loc);
                case token::END_LIST:
                    return RawFoamDictionaryParser::make_END_LIST(loc);
                case token::END_STATEMENT:
                    return RawFoamDictionaryParser::make_END_STATEMENT(loc);
            }
            break;
        case token::FATALERROR:
            return RawFoamDictionaryParser::make_END_OF_FILE(loc);
            break;
        case token::COMPOUND:
        case token::UNDEFINED:
            break;
    }
}
