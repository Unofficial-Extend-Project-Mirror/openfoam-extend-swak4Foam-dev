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
    2011-2013, 2015-2016 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/
%{
#include "FaFieldValueExpressionDriverYY.H"
#include <errno.h>
#include "FaFieldValuePluginFunction.H"
#include "FaFieldValueExpressionParser.tab.hh"
typedef parserFaField::FaFieldValueExpressionParser::semantic_type YYSTYPE;
%}

%s vectorcomponent
%s tensorcomponent
%x parsedByOtherParser
%x needsIntegerParameter

%option noyywrap nounput batch debug
%option stack
%option prefix="parserFaField"
%option reentrant
%option bison-bridge

id      [[:alpha:]_][[:alnum:]_]*
int     [[:digit:]]+

exponent_part              [eE][-+]?[[:digit:]]+
fractional_constant        (([[:digit:]]*"."[[:digit:]]+)|([[:digit:]]+".")|([[:digit:]]+))
float                      ((({fractional_constant}{exponent_part}?)|([[:digit:]]+{exponent_part}))|0)

%{
# define YY_USER_ACTION yylloc->columns (yyleng);
%}
%%

%{
     typedef parserFaField::FaFieldValueExpressionParser::token token;

     yylloc->step ();

     // recipie from http://www.gnu.org/software/bison/manual/html_node/Multiple-start_002dsymbols.html
     // allows multiple start symbols
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

<INITIAL,needsIntegerParameter>[ \t]+             yylloc->step ();
[\n]+                yylloc->lines (yyleng); yylloc->step ();

<INITIAL>[-+*/%(),&^<>!?:.]               return yytext[0];

<needsIntegerParameter>[(] return yytext[0];
<needsIntegerParameter>[)] { BEGIN(INITIAL); return yytext[0]; }

&&                   return token::TOKEN_AND;
\|\|                 return token::TOKEN_OR;
==                   return token::TOKEN_EQ;
!=                   return token::TOKEN_NEQ;
\<=                   return token::TOKEN_LEQ;
\>=                   return token::TOKEN_GEQ;

<vectorcomponent,tensorcomponent>x    { BEGIN(INITIAL); return token::TOKEN_x; }
<vectorcomponent,tensorcomponent>y    { BEGIN(INITIAL); return token::TOKEN_y; }
<vectorcomponent,tensorcomponent>z    { BEGIN(INITIAL); return token::TOKEN_z; }

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
sum                   return token::TOKEN_sum;
average               return token::TOKEN_average;
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
fpos                  return token::TOKEN_fposition;
fproj                 return token::TOKEN_fprojection;
face                  return token::TOKEN_face;
length                return token::TOKEN_length;
area                  return token::TOKEN_area;
rdist                 return token::TOKEN_rdist;
rand                  { BEGIN(needsIntegerParameter); return token::TOKEN_rand; }
id                    return token::TOKEN_id;
randNormal            { BEGIN(needsIntegerParameter); return token::TOKEN_randNormal; }
randFixed             { BEGIN(needsIntegerParameter); return token::TOKEN_randFixed; }
randNormalFixed       { BEGIN(needsIntegerParameter); return token::TOKEN_randNormalFixed; }

cpu                   return token::TOKEN_cpu;
weight                return token::TOKEN_weight;

grad                  return token::TOKEN_grad;
lnGrad                return token::TOKEN_lnGrad;
div                   return token::TOKEN_div;
laplacian             return token::TOKEN_laplacian;
ddt                   return token::TOKEN_ddt;
oldTime               return token::TOKEN_oldTime;

integrate             return token::TOKEN_integrate;
surfSum               return token::TOKEN_surfSum;
interpolate           return token::TOKEN_interpolate;
faceAverage           return token::TOKEN_faceAverage;

deltaT                return token::TOKEN_deltaT;
time                  return token::TOKEN_time;
outputTime            return token::TOKEN_outputTime;

vector                 return token::TOKEN_VECTOR;
tensor                 return token::TOKEN_TENSOR;
symmTensor             return token::TOKEN_SYMM_TENSOR;
sphericalTensor        return token::TOKEN_SPHERICAL_TENSOR;

surf                   return token::TOKEN_surf;

diag                   return token::TOKEN_diag;
tr                     return token::TOKEN_tr;
dev                    return token::TOKEN_dev;
symm                   return token::TOKEN_symm;
skew                   return token::TOKEN_skew;
det                    return token::TOKEN_det;
cof                    return token::TOKEN_cof;
inv                    return token::TOKEN_inv;
sph                    return token::TOKEN_sph;
twoSymm                return token::TOKEN_twoSymm;
dev2                   return token::TOKEN_dev2;
eigenValues            return token::TOKEN_eigenValues;
eigenVectors           return token::TOKEN_eigenVectors;

true                   return token::TOKEN_TRUE;
false                  return token::TOKEN_FALSE;

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
    } else if(driver.isLookup2D(*ptr)) {
        yylval->name = ptr; return token::TOKEN_LOOKUP2D;
    } else if(
        driver.isVariable<Foam::areaScalarField::value_type>(*ptr)
        ||
        driver.isThere<Foam::areaScalarField>(*ptr)
    ) {
        yylval->name = ptr; return token::TOKEN_SID;
    } else if(
        driver.isVariable<Foam::areaVectorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::areaVectorField>(*ptr)
    ) {
        yylval->name = ptr; return token::TOKEN_VID;
    } else if(
        driver.isVariable<Foam::areaTensorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::areaTensorField>(*ptr)
    ) {
        yylval->name = ptr; return token::TOKEN_TID;
    } else if(
        driver.isVariable<Foam::areaSymmTensorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::areaSymmTensorField>(*ptr)
    ) {
        yylval->name = ptr; return token::TOKEN_YID;
    } else if(
        driver.isVariable<Foam::areaSphericalTensorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::areaSphericalTensorField>(*ptr)
    ) {
        yylval->name = ptr; return token::TOKEN_HID;
    } else if(driver.isThere<Foam::edgeScalarField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FSID;
    } else if(driver.isThere<Foam::edgeVectorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FVID;
    } else if(driver.isThere<Foam::edgeTensorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FTID;
    } else if(driver.isThere<Foam::edgeSymmTensorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FYID;
    } else if(driver.isThere<Foam::edgeSphericalTensorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FHID;
    } else if(Foam::FaFieldValuePluginFunction::exists(driver,*ptr)) {
        // OK. We'll create the function two times. But this is less messy
        // than passing it two times
        Foam::autoPtr<Foam::FaFieldValuePluginFunction> fInfo(
            Foam::FaFieldValuePluginFunction::New(
                driver,
                *ptr
            )
        );

        int tokenTyp=-1;
        if(fInfo->returnType()=="areaScalarField") {
             tokenTyp=token::TOKEN_FUNCTION_SID;
        } else if(fInfo->returnType()=="edgeScalarField") {
             tokenTyp=token::TOKEN_FUNCTION_FSID;
        } else if(fInfo->returnType()=="areaVectorField") {
             tokenTyp=token::TOKEN_FUNCTION_VID;
        } else if(fInfo->returnType()=="edgeVectorField") {
             tokenTyp=token::TOKEN_FUNCTION_FVID;
        } else if(fInfo->returnType()=="areaTensorField") {
             tokenTyp=token::TOKEN_FUNCTION_TID;
        } else if(fInfo->returnType()=="edgeTensorField") {
             tokenTyp=token::TOKEN_FUNCTION_FTID;
        } else if(fInfo->returnType()=="areaSymmTensorField") {
             tokenTyp=token::TOKEN_FUNCTION_YID;
        } else if(fInfo->returnType()=="edgeSymmTensorField") {
             tokenTyp=token::TOKEN_FUNCTION_FYID;
        } else if(fInfo->returnType()=="areaSphericalTensorField") {
             tokenTyp=token::TOKEN_FUNCTION_HID;
        } else if(fInfo->returnType()=="edgeSphericalTensorField") {
             tokenTyp=token::TOKEN_FUNCTION_FHID;
        } else if(fInfo->returnType()=="areaLogicalField") {
             tokenTyp=token::TOKEN_FUNCTION_LID;
        } else if(fInfo->returnType()=="edgeLogicalField") {
             tokenTyp=token::TOKEN_FUNCTION_FLID;
        } else {
            driver.error (
                *yylloc,
                "Function "+*ptr+" returns unsupported type "
                + fInfo->returnType()
            );
        }

        //        BEGIN(parsedByOtherParser);

        yylval->name = ptr;
        return tokenTyp;
    } else {
        if((*ptr)=="I") {
            delete ptr;
            return token::TOKEN_unitTensor;
        }
        driver.error (*yylloc, "faField "+*ptr+" not existing or of wrong type");
    }
                     }

<parsedByOtherParser>. {
    numberOfFunctionChars--;
    if(driver.traceScanning()) {
        Foam::Info << " Remaining characters to be eaten: " << numberOfFunctionChars
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
<needsIntegerParameter>.                    driver.error (*yylloc, "invalid character when only a integer is expeced");

%%

// YY_BUFFER_STATE bufferFaField;

void FaFieldValueExpressionDriver::scan_begin ()
{
    if(traceScanning()) {
        Info << "FaFieldValueExpressionDriver::scan_begin "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    if(scanner_!=NULL) {
        FatalErrorIn("FaFieldValueExpressionDriver::scan_begin")
            << "Already existing scanner " << getHex(scanner_)
                << endl
                << exit(FatalError);

    }

    yylex_init(&scanner_);
    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    yy_flex_debug = traceScanning();
    yy_scan_string(content().c_str(),scanner_);

//    if (!(yyin = fopen (file.c_str (), "r")))
//        error (std::string ("cannot open ") + file);

    if(traceScanning()) {
        Info << "FaFieldValueExpressionDriver::scan_begin - finished "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }
}

void FaFieldValueExpressionDriver::scan_end ()
{
    if(traceScanning()) {
        Info << "FaFieldValueExpressionDriver::scan_end "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    if(scanner_==NULL) {
        FatalErrorIn("FaFieldValueExpressionDriver::scan_end")
            << "Uninitialized Scanner. Can't delete it"
                << endl
                << exit(FatalError);

    }

    yylex_destroy(scanner_);
    // WarningIn("FaFieldValueExpressionDriver::scan_end")
    //     << "Scanner " <<  getHex(scanner_) << " is not deleted"
    //         << endl;

    scanner_=NULL;
//	    fclose (yyin);
}

void FaFieldValueExpressionDriver::startEatCharacters()
{
    if(traceScanning()) {
        Info << "FaFieldValueExpressionDriver::startEatCharacters() "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    BEGIN(parsedByOtherParser);
}


void FaFieldValueExpressionDriver::startVectorComponent()
{
    if(traceScanning()) {
        Info << "FaFieldValueExpressionDriver::startVectorComponent() "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    BEGIN(vectorcomponent);
}

void FaFieldValueExpressionDriver::startTensorComponent()
{
    if(traceScanning()) {
        Info << "FaFieldValueExpressionDriver::startTensorComponent() "
            << getHex(this) << endl;
        Info << "Scanner: " << getHex(scanner_) << endl;
    }

    struct yyguts_t * yyg = (struct yyguts_t*)scanner_;
    BEGIN(tensorcomponent);
}
