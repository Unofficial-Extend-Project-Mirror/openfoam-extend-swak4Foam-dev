 
%{                                          /* -*- C++ -*- */
#include "FieldValueExpressionDriverYY.H"
#include <errno.h>
%}

%s setname
%s zonename
%s fsetname
%s fzonename
%x needsIntegerParameter

%option noyywrap nounput batch debug 
%option stack
%option prefix="fvexpr"

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
    yylloc->step ();
%}

<INITIAL,setname,zonename,fsetname,fzonename,needsIntegerParameter>[ \t]+             yylloc->step ();
[\n]+                yylloc->lines (yyleng); yylloc->step ();

<INITIAL,setname,zonename,fsetname,fzonename>[-+*/%(),&^<>!?:.]               return yytext[0];

<needsIntegerParameter>[(] return yytext[0];
<needsIntegerParameter>[)] { BEGIN(INITIAL); return yytext[0]; }

%{
    typedef parserField::FieldValueExpressionParser::token token;
%}

&&                   return token::TOKEN_AND;
\|\|                 return token::TOKEN_OR;
==                   return token::TOKEN_EQ;
!=                   return token::TOKEN_NEQ;
\<=                   return token::TOKEN_LEQ;
\>=                   return token::TOKEN_GEQ;

xx                    return token::TOKEN_xx;
xy                    return token::TOKEN_xy;
xz                    return token::TOKEN_xz;
yx                    return token::TOKEN_yx;
yy                    return token::TOKEN_yy;
yz                    return token::TOKEN_yz;
zx                    return token::TOKEN_zx;
zy                    return token::TOKEN_zy;
zz                    return token::TOKEN_zz;
ii                    return token::TOKEN_ii;

pow                   return token::TOKEN_pow;
exp                   return token::TOKEN_exp;
log                   return token::TOKEN_log;
mag                   return token::TOKEN_mag;
sin                   return token::TOKEN_sin;
cos                   return token::TOKEN_cos;
tan                   return token::TOKEN_tan;
min                   return token::TOKEN_min;
max                   return token::TOKEN_max;
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
area                  return token::TOKEN_area;
vol                   return token::TOKEN_volume;
dist                  return token::TOKEN_dist;
nearDist              return token::TOKEN_nearDist;
rdist                 return token::TOKEN_rdist;
rand                  { BEGIN(needsIntegerParameter); return token::TOKEN_rand; }
randFixed             { BEGIN(needsIntegerParameter); return token::TOKEN_randFixed; }
id                    return token::TOKEN_id;
randNormal            { BEGIN(needsIntegerParameter); return token::TOKEN_randNormal; }
randNormalFixed       { BEGIN(needsIntegerParameter); return token::TOKEN_randNormalFixed; }

cpu                   return token::TOKEN_cpu;

set                   {
    BEGIN(setname);
    return token::TOKEN_set;
                      }

zone                  {
    BEGIN(zonename);
    return token::TOKEN_zone;
                      }

fset                   {
    BEGIN(fsetname);
    return token::TOKEN_fset;
                      }

fzone                  {
    BEGIN(fzonename);
    return token::TOKEN_fzone;
                      }

grad                  return token::TOKEN_grad;
curl                  return token::TOKEN_curl;
magSqrGradGrad        return token::TOKEN_magSqrGradGrad;
snGrad                return token::TOKEN_snGrad;
div                   return token::TOKEN_div;
laplacian             return token::TOKEN_laplacian;

integrate             return token::TOKEN_integrate;
surfSum               return token::TOKEN_surfSum;
interpolate           return token::TOKEN_interpolate;
faceAverage           return token::TOKEN_faceAverage;
reconstruct           return token::TOKEN_reconstruct;

deltaT                return token::TOKEN_deltaT;
time                  return token::TOKEN_time;

vector                 return token::TOKEN_VECTOR;
tensor                 return token::TOKEN_TENSOR;
symmTensor             return token::TOKEN_SYMM_TENSOR;
sphericalTensor        return token::TOKEN_SPHERICAL_TENSOR;

surf                   return token::TOKEN_surf;

transpose              return token::TOKEN_transpose;
diag                   return token::TOKEN_diag;
tr                     return token::TOKEN_tr;
dev                    return token::TOKEN_dev;
symm                   return token::TOKEN_symm;
skew                   return token::TOKEN_skew;
det                    return token::TOKEN_det;
cof                    return token::TOKEN_cof;
inv                    return token::TOKEN_inv;

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

[xyz]                return yytext[0];

<INITIAL>{id}                 {
    Foam::string *ptr=new Foam::string (yytext);
    if(driver.isLine(*ptr)) {
        yylval->name = ptr; return token::TOKEN_LINE;
    } else if(driver.isLookup(*ptr)) {
        yylval->name = ptr; return token::TOKEN_LOOKUP;
    } else if(
        driver.isVariable<Foam::volScalarField::value_type>(*ptr)
        ||
        driver.isThere<Foam::volScalarField>(*ptr)
    ) {
        yylval->name = ptr; return token::TOKEN_SID;
    } else if(       
        driver.isVariable<Foam::volVectorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::volVectorField>(*ptr)
    ) {
        yylval->vname = ptr; return token::TOKEN_VID;
    } else if(       
        driver.isVariable<Foam::volTensorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::volTensorField>(*ptr)
    ) {
        yylval->vname = ptr; return token::TOKEN_TID;
    } else if(       
        driver.isVariable<Foam::volSymmTensorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::volSymmTensorField>(*ptr)
    ) {
        yylval->vname = ptr; return token::TOKEN_YID;
    } else if(       
        driver.isVariable<Foam::volSphericalTensorField::value_type>(*ptr)
        ||
        driver.isThere<Foam::volSphericalTensorField>(*ptr)
    ) {
        yylval->vname = ptr; return token::TOKEN_HID;
    } else if(driver.isThere<Foam::surfaceVectorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FVID;
    } else if(driver.isThere<Foam::surfaceScalarField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FSID;
    } else if(driver.isThere<Foam::surfaceTensorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FTID;
    } else if(driver.isThere<Foam::surfaceSymmTensorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FYID;
    } else if(driver.isThere<Foam::surfaceSphericalTensorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FHID;
    } else {
        driver.error (*yylloc, "field "+*ptr+" not existing or of wrong type");
    }
                     }

<setname>{setid}              {
    Foam::string *ptr=new Foam::string (yytext);
    BEGIN(INITIAL);
    if(driver.isCellSet(*ptr)) {
        yylval->name = ptr; return token::TOKEN_SETID;
    } else {
        driver.error (*yylloc, "cellSet id "+*ptr+" not existing or of wrong type");
    }
                     }
<zonename>{setid}              {
    Foam::string *ptr=new Foam::string (yytext);
    BEGIN(INITIAL);
    if(driver.isCellZone(*ptr)) {
        yylval->name = ptr; return token::TOKEN_ZONEID;
    } else {
        driver.error (*yylloc, "cellZone id "+*ptr+" not existing or of wrong type");
    }
                     }
<fsetname>{setid}              {
    Foam::string *ptr=new Foam::string (yytext);
    BEGIN(INITIAL);
    if(driver.isFaceSet(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FSETID;
    } else {
        driver.error (*yylloc, "faceSet id "+*ptr+" not existing or of wrong type");
    }
                     }
<fzonename>{setid}              {
    Foam::string *ptr=new Foam::string (yytext);
    BEGIN(INITIAL);
    if(driver.isFaceZone(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FZONEID;
    } else {
        driver.error (*yylloc, "faceZone id "+*ptr+" not existing or of wrong type");
    }
                     }

.                    driver.error (*yylloc, "invalid character");
<needsIntegerParameter>.                    driver.error (*yylloc, "invalid character when only an integer parameter is expected");


%%

void FieldValueExpressionDriver::scan_begin ()
{
    yy_flex_debug = trace_scanning_;
    yy_scan_string(content_.c_str());
//    if (!(yyin = fopen (file.c_str (), "r")))
//        error (std::string ("cannot open ") + file);
}

void FieldValueExpressionDriver::scan_end ()
{
//	    fclose (yyin);
}
