 
%{                                          /* -*- C++ -*- */
#include "FieldValueExpressionDriverYY.H"
#include <errno.h>
%}

%s setname

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

<INITIAL,setname>[ \t]+             yylloc->step ();
[\n]+                yylloc->lines (yyleng); yylloc->step ();

<INITIAL,setname>[-+*/%(),&^<>!?:.]               return yytext[0];

%{
    typedef parsers::FieldValueExpressionParser::token token;
%}

&&                   return token::TOKEN_AND;
\|\|                 return token::TOKEN_OR;
==                   return token::TOKEN_EQ;
!=                   return token::TOKEN_NEQ;
\<=                   return token::TOKEN_LEQ;
\>=                   return token::TOKEN_GEQ;

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
rdist                 return token::TOKEN_rdist;
rand                  return token::TOKEN_rand;
id                    return token::TOKEN_id;
randNormal            return token::TOKEN_randNormal;

cpu                   return token::TOKEN_cpu;

set                   {
    BEGIN(setname);
    return token::TOKEN_set;
                      }

zone                  {
    BEGIN(setname);
    return token::TOKEN_zone;
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
average               return token::TOKEN_average;
reconstruct           return token::TOKEN_reconstruct;

deltaT                return token::TOKEN_deltaT;
time                  return token::TOKEN_time;

vector                 return token::TOKEN_VECTOR;

surf                   return token::TOKEN_surf;

true                   return token::TOKEN_TRUE;
false                  return token::TOKEN_FALSE;

{float}                {
                       errno = 0;
                       yylval->val = atof(yytext);
                       return token::TOKEN_NUM;
                     }

[xyz]                return yytext[0];

<INITIAL>{id}                 {
    Foam::string *ptr=new Foam::string (yytext);
    if(driver.isThere<Foam::volVectorField>(*ptr)) {
        yylval->vname = ptr; return token::TOKEN_VID;
    } else if(driver.isThere<Foam::volScalarField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_SID;
    } else if(driver.isThere<Foam::surfaceScalarField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FSID;
    } else if(driver.isThere<Foam::surfaceVectorField>(*ptr)) {
        yylval->name = ptr; return token::TOKEN_FVID;
    } else {
        driver.error (*yylloc, "field "+*ptr+" not existing or of wrong type");
    }
                     }

<setname>{setid}              {
    Foam::string *ptr=new Foam::string (yytext);
    BEGIN(INITIAL);
    if(driver.isCellSet(*ptr)) {
        yylval->name = ptr; return token::TOKEN_SETID;
    } else if(driver.isCellZone(*ptr)) {
        yylval->name = ptr; return token::TOKEN_ZONEID;
    } else {
        driver.error (*yylloc, "cell/zone id "+*ptr+" not existing or of wrong type");
    }
                     }

.                    driver.error (*yylloc, "invalid character");


%%

void FieldValueExpressionDriver::scan_begin ()
{
    yy_flex_debug = trace_scanning;
    yy_scan_string(content.c_str());
//    if (!(yyin = fopen (file.c_str (), "r")))
//        error (std::string ("cannot open ") + file);
}

void FieldValueExpressionDriver::scan_end ()
{
//	    fclose (yyin);
}
