/*----------------------- -*- C++ -*- ---------------------------------------*\
 ##   ####  ######     |
 ##  ##     ##         | Copyright: ICE Stroemungsfoschungs GmbH
 ##  ##     ####       |
 ##  ##     ##         | http://www.ice-sf.at
 ##   ####  ######     |
-------------------------------------------------------------------------------
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


Contributors/Copyright:
    2009-2013 Bernhard F.W. Gschaider <bgschaid@ice-sf.at>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/
%{                                          /* -*- C++ -*- */
#include "PatchValueExpressionDriverYY.H"
#include <errno.h>
#include "PatchValuePluginFunction.H"
#include "PatchValueExpressionParser.tab.hh"
typedef parserPatch::PatchValueExpressionParser::semantic_type YYSTYPE;
%}

%s setname
%s vectorcomponent
%s tensorcomponent
%x parsedByOtherParser
%x needsIntegerParameter

%option noyywrap nounput batch debug
%option stack
%option prefix="parserPatch"
%option reentrant
%option bison-bridge

id      [[:alpha:]_][[:alnum:]_]*
setid   [[:alpha:]_][[:alnum:]_-]*
int     [[:digit:]]+

exponent_part              [eE][-+]?[[:digit:]]+
fractional_constant        (([[:digit:]]*"."[[:digit:]]+)|([[:digit:]]+".")|([[:digit:]]+))
float                      ((({fractional_constant}{exponent_part}?)|([[:digit:]]+{exponent_part}))|0)

%{
# define YY_USER_ACTION yylloc->columns (yyleng);
%}
%%

%{
    typedef parserPatch::PatchValueExpressionParser::token token;

    yylloc->step ();

    if (start_token)
    {
        if(driver.traceScanning()) {
            Foam::Info << "Start token: " << start_token << Foam::endl;
        }

        int t = start_token;
        start_token = 0;
        return t;
    }
%}

<INITIAL,setname,needsIntegerParameter>[ \t]+             yylloc->step ();
[\n]+                yylloc->lines (yyleng); yylloc->step ();

<INITIAL,setname>[-+*/%(),&^<>!?:.]               return yytext[0];

<needsIntegerParameter>[(] return yytext[0];
<needsIntegerParameter>[)] { BEGIN(INITIAL); return yytext[0]; }

&&                   return token::TOKEN_AND;
\|\|                 return token::TOKEN_OR;
==                   return token::TOKEN_EQ;
!=                   return token::TOKEN_NEQ;
\<=                   return token::TOKEN_LEQ;
\>=                   return token::TOKEN_GEQ;

<vectorcomponent>x    { BEGIN(INITIAL); return token::TOKEN_x; }
<vectorcomponent>y    { BEGIN(INITIAL); return token::TOKEN_y; }
<vectorcomponent>z    { BEGIN(INITIAL); return token::TOKEN_z; }

<tensorcomponent>xx    { BEGIN(INITIAL); return token::TOKEN_xx; }
<tensorcomponent>xy    { BEGIN(INITIAL); return token::TOKEN_xy; }
<tensorcomponent>xz    { BEGIN(INITIAL); return token::TOKEN_xz; }
<tensorcomponent>yx    { BEGIN(INITIAL); return token::TOKEN_yx; }
<tensorcomponent>yy    { BEGIN(INITIAL); return token::TOKEN_yy; }
<tensorcomponent>yz    { BEGIN(INITIAL); return token::TOKEN_yz; }
<tensorcomponent>zx    { BEGIN(INITIAL); return token::TOKEN_zx; }
<tensorcomponent>zy    { BEGIN(INITIAL); return token::TOKEN_zy; }
<tensorcomponent>zz    { BEGIN(INITIAL); return token::TOKEN_zz; }
<tensorcomponent>ii    { BEGIN(INITIAL); return token::TOKEN_ii; }
<tensorcomponent>T     { BEGIN(INITIAL); return token::TOKEN_transpose; }

pow                   return token::TOKEN_pow;
exp                   return token::TOKEN_exp;
log                   return token::TOKEN_log;
mag                   return token::TOKEN_mag;
magSqr                return token::TOKEN_magSqr;
sin                   return token::TOKEN_sin;
cos                   return token::TOKEN_cos;
tan                   return token::TOKEN_tan;
min                   return token::TOKEN_min;
max                   return token::TOKEN_max;
minPosition           return token::TOKEN_minPosition;
maxPosition           return token::TOKEN_maxPosition;
average               return token::TOKEN_average;
sum                   return token::TOKEN_sum;
sqr                   return token::TOKEN_sqr;
sqrt                  return token::TOKEN_sqrt;

log10                 return token::TOKEN_log10;
asin                  return token::TOKEN_asin;
acos                  return token::TOKEN_acos;
atan                  return token::TOKEN_atan;
sinh                  return token::TOKEN_sinh;
cosh                  return token::TOKEN_cosh;
tanh                  return token::TOKEN_tanh;
asinh                 return token::TOKEN_asinh;
acosh                 return token::TOKEN_acosh;
atanh                 return token::TOKEN_atanh;
erf                   return token::TOKEN_erf;
erfc                  return token::TOKEN_erfc;
lgamma                return token::TOKEN_lgamma;
besselJ0              return token::TOKEN_besselJ0;
besselJ1              return token::TOKEN_besselJ1;
besselY0              return token::TOKEN_besselY0;
besselY1              return token::TOKEN_besselY1;

sign                  return token::TOKEN_sign;
positive              return token::TOKEN_pos;
negative              return token::TOKEN_neg;

pi                    return token::TOKEN_pi;
pos                   return token::TOKEN_position;
area                  return token::TOKEN_area;
pts                   return token::TOKEN_points;
Sf                    return token::TOKEN_Sf;
Cn                    return token::TOKEN_Cn;
delta                 return token::TOKEN_delta;
weights               return token::TOKEN_weights;
snGrad                return token::TOKEN_snGrad;
internalField         return token::TOKEN_internalField;
neighbourField        return token::TOKEN_neighbourField;
oldTime               return token::TOKEN_oldTime;
normal                return token::TOKEN_normal;
rand                  { BEGIN(needsIntegerParameter); return token::TOKEN_rand; }
randFixed             { BEGIN(needsIntegerParameter); return token::TOKEN_randFixed; }
dist                  return token::TOKEN_dist;
id                    return token::TOKEN_id;
cpu                   return token::TOKEN_cpu;
randNormal            { BEGIN(needsIntegerParameter); return token::TOKEN_randNormal; }
randNormalFixed       { BEGIN(needsIntegerParameter); return token::TOKEN_randNormalFixed; }

deltaT                return token::TOKEN_deltaT;
time                  return token::TOKEN_time;

vector                 return token::TOKEN_VECTOR;
tensor                 return token::TOKEN_TENSOR;
symmTensor             return token::TOKEN_SYMM_TENSOR;
sphericalTensor        return token::TOKEN_SPHERICAL_TENSOR;

true                   return token::TOKEN_TRUE;
false                  return token::TOKEN_FALSE;

toPoint                 return token::TOKEN_toPoint;
toFace                  return token::TOKEN_toFace;

mapped                  return token::TOKEN_mapped;
mappedInternal          return token::TOKEN_mappedInternal;

diag                   return token::TOKEN_diag;
tr                     return token::TOKEN_tr;
dev                    return token::TOKEN_dev;
symm                   return token::TOKEN_symm;
skew                   return token::TOKEN_skew;
det                    return token::TOKEN_det;
cof                    return token::TOKEN_cof;
inv                    return token::TOKEN_inv;


{float}                {
                       errno = 0;
                       yylval->val = atof(yytext);
                       return token::TOKEN_NUM;
                     }

<needsIntegerParameter>{int}                {
                       errno = 0;
                       yylval->integer = atoi(yytext);
                       return token::TOKEN_INT;
                     }

<INITIAL>{id}                 {
    Foam::word *ptr=new Foam::word (yytext);
    if(driver.isLine(*ptr)) {
        yylval->name = ptr; return token::TOKEN_LINE;
    } else if(driver.isLookup(*ptr)) {
        yylval->name = ptr; return token::TOKEN_LOOKUP;
    } else if(driver.is<Foam::scalar>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_SID;
    } else if(driver.is<Foam::vector>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_VID;
    } else if(driver.is<Foam::tensor>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_TID;
    } else if(driver.is<Foam::symmTensor>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_YID;
    } else if(driver.is<Foam::sphericalTensor>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_HID;
        //    } else if(driver.is<Foam::bool>(*ptr)) {
        //        yylval->name = ptr; return token::TOKEN_LID;
    } else if(driver.is<Foam::scalar>(*ptr,true)) {
        yylval->name = ptr; return token::TOKEN_PSID;
    } else if(driver.is<Foam::vector>(*ptr,true)) {
        yylval->name = ptr; return token::TOKEN_PVID;
    } else if(driver.is<Foam::tensor>(*ptr,true)) {
        yylval->name = ptr; return token::TOKEN_PTID;
    } else if(driver.is<Foam::symmTensor>(*ptr,true)) {
        yylval->name = ptr; return token::TOKEN_PYID;
    } else if(driver.is<Foam::sphericalTensor>(*ptr,true)) {
        yylval->name = ptr; return token::TOKEN_PHID;
        //    } else if(driver.is<Foam::bool>(*ptr,true)) {
        //        yylval->name = ptr; return token::TOKEN_PLID;
    } else if(Foam::PatchValuePluginFunction::exists(driver,*ptr)) {
        // OK. We'll create the function two times. But this is less messy
        // than passing it two times
        Foam::autoPtr<Foam::PatchValuePluginFunction> fInfo(
            Foam::PatchValuePluginFunction::New(
                driver,
                *ptr
            )
        );

        int tokenTyp=-1;
        if(fInfo->returnType()=="scalar") {
             tokenTyp=token::TOKEN_FUNCTION_SID;
        } else if(fInfo->returnType()=="vector") {
             tokenTyp=token::TOKEN_FUNCTION_VID;
        } else if(fInfo->returnType()=="tensor") {
             tokenTyp=token::TOKEN_FUNCTION_TID;
        } else if(fInfo->returnType()=="symmTensor") {
             tokenTyp=token::TOKEN_FUNCTION_YID;
        } else if(fInfo->returnType()=="sphericalTensor") {
             tokenTyp=token::TOKEN_FUNCTION_HID;
        } else if(fInfo->returnType()=="pointScalar") {
             tokenTyp=token::TOKEN_FUNCTION_PSID;
        } else if(fInfo->returnType()=="pointVector") {
             tokenTyp=token::TOKEN_FUNCTION_PVID;
        } else if(fInfo->returnType()=="pointTensor") {
             tokenTyp=token::TOKEN_FUNCTION_PTID;
        } else if(fInfo->returnType()=="pointSymmTensor") {
             tokenTyp=token::TOKEN_FUNCTION_PYID;
        } else if(fInfo->returnType()=="pointSphericalTensor") {
             tokenTyp=token::TOKEN_FUNCTION_PHID;
        } else {
            driver.error (
                *yylloc,
                "Function "+*ptr+" returns unsupported type "
                + fInfo->returnType()
            );
        }

        yylval->name = ptr;
        return tokenTyp;
    } else {
        if((*ptr)=="I") {
            delete ptr;
            return token::TOKEN_unitTensor;
        }
        driver.error (*yylloc, "field "+*ptr+" not existing or of wrong type");
    }
                     }

<parsedByOtherParser>. {
    numberOfFunctionChars--;
    if(driver.traceScanning()) {
        Foam::Info << " Remaining characters to be eaten: "
            << numberOfFunctionChars
            << Foam::endl;
    }
    if(numberOfFunctionChars>0) {
        return token::TOKEN_IN_FUNCTION_CHAR;
    } else {
        BEGIN(INITIAL);
        return token::TOKEN_LAST_FUNCTION_CHAR;
    }
                       }

.                    driver.error (*yylloc, "invalid character");
<needsIntegerParameter>.                    driver.error (*yylloc, "invalid character when only an integer parameter is expected");


%%

// YY_BUFFER_STATE bufferPatch;

void PatchValueExpressionDriver::scan_begin ()
{
    if(traceScanning()) {
        Info << "PatchValueExpressionDriver::scan_begin "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    if(scanner_!=NULL) {
        FatalErrorIn("PatchValueExpressionDriver::scan_begin")
            << "Already existing scanner " << getHex(scanner_)
                << endl
                << exit(FatalError);

    }

    yylex_init(&scanner_);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    yy_flex_debug = traceScanning();
    /* bufferPatch= */ yy_scan_string(content().c_str(),scanner_);

    if(traceScanning()) {
        Info << "PatchValueExpressionDriver::scan_begin - finished "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }
}

void PatchValueExpressionDriver::scan_end ()
{
    if(traceScanning()) {
        Info << "PatchValueExpressionDriver::scan_end "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    if(scanner_==NULL) {
        FatalErrorIn("PatchValueExpressionDriver::scan_end")
            << "Uninitialized Scanner. Can't delete it"
                << endl
                << exit(FatalError);

    }

    yylex_destroy(scanner_);

    scanner_=NULL;
//	    fclose (yyin);
    //    yy_delete_buffer(bufferPatch,scanner_);
}

void PatchValueExpressionDriver::startEatCharacters()
{
    if(traceScanning()) {
        Info << "PatchValueExpressionDriver::startEatCharacters() "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    BEGIN(parsedByOtherParser);
}

void PatchValueExpressionDriver::startVectorComponent()
{
    if(traceScanning()) {
        Info << "PatchValueExpressionDriver::startVectorComponent() "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    BEGIN(vectorcomponent);
}

void PatchValueExpressionDriver::startTensorComponent()
{
    if(traceScanning()) {
        Info << "PatchValueExpressionDriver::startTensorComponent() "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    BEGIN(tensorcomponent);
}
