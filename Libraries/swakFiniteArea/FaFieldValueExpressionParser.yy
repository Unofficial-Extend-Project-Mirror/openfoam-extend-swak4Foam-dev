/*  -*- C++ -*- */

%skeleton "lalr1.cc"
/* %require "2.1a" */
%defines
%define "parser_class_name" "FaFieldValueExpressionParser"

%{
#include <areaFields.H>
#include <edgeFields.H>
#include <facGrad.H>
    // #include <facCurl.H>
    //#include <facMagSqrGradGrad.H>
#include <facLnGrad.H>
#include <facDiv.H>
#include <facEdgeIntegrate.H>
    //#include <facReconstruct.H>
#include <facAverage.H>
#include <edgeInterpolate.H>
#include <facLaplacian.H>
#include <facDdt.H>

#include <faMatrix.H>

#include <functional>
#include <cmath>

    namespace Foam {
        class FaFieldValueExpressionDriver;
    }

    using Foam::FaFieldValueExpressionDriver;
%}

%name-prefix="parserFaField"

%parse-param { FaFieldValueExpressionDriver& driver }
%lex-param { FaFieldValueExpressionDriver& driver }

%locations
%initial-action
{
	     // Initialize the initial location.
	     //     @$.begin.filename = @$.end.filename = &driver.file;
};

%debug
%error-verbose

%union
{
    Foam::scalar val;
    Foam::label integer;
    Foam::vector *vec;
    Foam::tensor *ten;
    Foam::symmTensor *yten;
    Foam::sphericalTensor *hten;
    Foam::string *name;
    Foam::areaVectorField *vfield;
    Foam::areaScalarField *sfield;
    Foam::areaTensorField *tfield;
    Foam::areaSymmTensorField *yfield;
    Foam::areaSphericalTensorField *hfield;
    Foam::edgeScalarField *fsfield;
    Foam::edgeVectorField *fvfield;
    Foam::edgeTensorField *ftfield;
    Foam::edgeSymmTensorField *fyfield;
    Foam::edgeSphericalTensorField *fhfield;
};

%{
#include "FaFieldValueExpressionDriverYY.H"
#include "FaFieldValueExpressionDriverLogicalTemplates.H"

#include "swakChecks.H"
%}

%token <name>   TOKEN_LINE  "timeline"
%token <name>   TOKEN_LOOKUP  "lookup"
%token <name>   TOKEN_SID   "scalarID"
%token <name>  TOKEN_VID   "vectorID"
%token <name>  TOKEN_TID   "tensorID"
%token <name>  TOKEN_YID   "symmTensorID"
%token <name>  TOKEN_HID   "sphericalTensorID"
%token <name> TOKEN_FSID  "faceScalarID"
%token <name> TOKEN_FVID  "faceVectorID"
%token <name>  TOKEN_FTID   "faceTensorID"
%token <name>  TOKEN_FYID   "faceSymmTensorID"
%token <name>  TOKEN_FHID   "faceSphericalTensorID"
%token <val>    TOKEN_NUM   "number"
%token <integer>    TOKEN_INT   "integer"
%token <vec>    TOKEN_VEC   "vector"
%token <ten>    TOKEN_TEN   "tensor"
%token <yten>    TOKEN_YTEN   "symmTensor"
%token <hten>    TOKEN_HTEN   "sphericalTensor"
%type  <val>    scalar      "sexpression"  
%type  <sfield>    exp        "expression"
%type  <sfield>    lexp       "lexpression"
%type  <fsfield>   flexp       "flexpression"
%type  <vfield>    vexp       "vexpression"
%type  <tfield>    texp       "texpression"
%type  <yfield>    yexp       "yexpression"
%type  <hfield>    hexp       "hexpression"
%type  <fsfield>   fsexp      "fsexpression"
%type  <fvfield>   fvexp      "fvexpression"
%type  <ftfield>   ftexp      "ftexpression"
%type  <fyfield>   fyexp      "fyexpression"
%type  <fhfield>   fhexp      "fhexpression"
%type  <vfield>    vector     
%type  <fvfield>    fvector     
%type  <tfield>    tensor     
%type  <yfield>    symmTensor     
%type  <hfield>    sphericalTensor     
%type  <ftfield>    ftensor     
%type  <fyfield>    fsymmTensor     
%type  <fhfield>    fsphericalTensor     

%token TOKEN_VECTOR
%token TOKEN_TENSOR
%token TOKEN_SYMM_TENSOR
%token TOKEN_SPHERICAL_TENSOR

%token TOKEN_TRUE
%token TOKEN_FALSE

%token TOKEN_x 
%token TOKEN_y 
%token TOKEN_z 

%token TOKEN_xx
%token TOKEN_xy
%token TOKEN_xz
%token TOKEN_yx
%token TOKEN_yy
%token TOKEN_yz
%token TOKEN_zx
%token TOKEN_zy
%token TOKEN_zz
%token TOKEN_ii

%token TOKEN_pi
%token TOKEN_rand
%token TOKEN_randFixed
%token TOKEN_id
%token TOKEN_cpu
%token TOKEN_randNormal
%token TOKEN_randNormalFixed
%token TOKEN_position
%token TOKEN_fposition
%token TOKEN_fprojection
%token TOKEN_face
%token TOKEN_length
%token TOKEN_area
%token TOKEN_rdist

%token TOKEN_div
%token TOKEN_grad
%token TOKEN_lnGrad
%token TOKEN_laplacian
%token TOKEN_ddt
%token TOKEN_oldTime

%token TOKEN_integrate
%token TOKEN_surfSum
%token TOKEN_interpolate
%token TOKEN_faceAverage

%token TOKEN_surf

%token TOKEN_deltaT
%token TOKEN_time

%token TOKEN_pow
%token TOKEN_log
%token TOKEN_exp
%token TOKEN_mag
%token TOKEN_magSqr
%token TOKEN_sin
%token TOKEN_cos
%token TOKEN_tan
%token TOKEN_min
%token TOKEN_max
%token TOKEN_minPosition
%token TOKEN_maxPosition
%token TOKEN_sum
%token TOKEN_average
%token TOKEN_sqr
%token TOKEN_sqrt

%token TOKEN_transpose
%token TOKEN_diag
%token TOKEN_tr
%token TOKEN_dev
%token TOKEN_symm
%token TOKEN_skew
%token TOKEN_det
%token TOKEN_cof
%token TOKEN_inv

%token TOKEN_log10
%token TOKEN_asin
%token TOKEN_acos
%token TOKEN_atan
%token TOKEN_sinh
%token TOKEN_cosh
%token TOKEN_tanh
%token TOKEN_asinh
%token TOKEN_acosh
%token TOKEN_atanh
%token TOKEN_erf
%token TOKEN_erfc
%token TOKEN_lgamma
%token TOKEN_besselJ0
%token TOKEN_besselJ1
%token TOKEN_besselY0
%token TOKEN_besselY1

%token TOKEN_sign
%token TOKEN_pos
%token TOKEN_neg

%left '?' ':'
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_EQ TOKEN_NEQ
%left TOKEN_LEQ TOKEN_GEQ '<' '>'
%left '-' '+'
%left '%' '*' '/' '&' '^'
%left TOKEN_NEG TOKEN_NOT
// %right '^'
%left '.'

%printer             { debug_stream () << *$$; } "scalarID" "vectorID" "faceScalarID" "faceVectorID" "tensorID" "symmTensorID" "sphericalTensorID" "faceTensorID" "faceSymmTensorID" "faceSphericalTensorID" 
%printer             { Foam::OStringStream buff; buff << *$$; debug_stream () << buff.str().c_str(); } "vector" "tensor" "symmTensor" "sphericalTensor"
%destructor          { delete $$; } "timeline" "lookup" "scalarID" "faceScalarID" "faceVectorID" "vectorID" "vector" "symmTensor" "sphericalTensor" "expression" "vexpression" "fsexpression" "fvexpression" "lexpression" "flexpression" "texpression" "yexpression" "hexpression" "ftexpression" "fyexpression" "fhexpression" "symmTensorID" "sphericalTensorID" "faceTensorID" "faceSymmTensorID" "faceSphericalTensorID"
%printer             { debug_stream () << $$; } "number"  "sexpression" "integer"
%printer             { debug_stream () << $$->name().c_str(); } "expression"  "vexpression" "lexpression" "flexpression" "fsexpression" "fvexpression" "texpression" "yexpression" "hexpression" "ftexpression" "fyexpression" "fhexpression"


%%
%start unit;

unit:   exp                     { driver.setResult($1,false);  }
        | vexp                  { driver.setResult($1,false);  }
        | texp                  { driver.setResult($1,false);  }
        | yexp                  { driver.setResult($1,false);  }
        | hexp                  { driver.setResult($1,false);  }
        | lexp                  { driver.setLogicalResult($1,false); }
        | fsexp                 { driver.setResult($1,true);  }
        | fvexp                 { driver.setResult($1,true);  }
        | ftexp                 { driver.setResult($1,true);  }
        | fyexp                 { driver.setResult($1,true);  }
        | fhexp                 { driver.setResult($1,true);  }
        | flexp                 { driver.setLogicalResult($1,true); }
;

vectorComponentSwitch: /* empty rule */{ driver.startVectorComponent(); } 
;

tensorComponentSwitch: /* empty rule */{ driver.startTensorComponent(); } 
;

vexp:   vector                                    { $$ = $1; }
        | vexp '+' vexp 		          { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' vexp 		                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 * *$3); delete $1; delete $3; }
        | vexp '*' exp 		                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 * *$3); delete $1; delete $3; }
        | texp '&' vexp 	                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' texp 	                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 & *$3); delete $1; delete $3; }
        | yexp '&' vexp 	                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' yexp 	                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 & *$3); delete $1; delete $3; }
        | hexp '&' vexp 	                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' hexp 	                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '/' exp 		                  { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 / *$3); delete $1; delete $3; }
        | vexp '^' vexp 		          { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 ^ *$3); delete $1; delete $3; }
        | vexp '-' vexp 		          { sameSize($1,$3); $$ = new Foam::areaVectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' vexp %prec TOKEN_NEG                { $$ = new Foam::areaVectorField(-*$2); delete $2; }
        | '(' vexp ')'		                  { $$ = $2; }  
        | lexp '?' vexp ':' vexp                  { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::areaVectorField>(Foam::vector())); delete $1; delete $3; delete $5; }
        | TOKEN_position '(' ')'                  { $$ = driver.makePositionField(); }
        | TOKEN_laplacian '(' fsexp ',' vexp ')'  { $$ = new Foam::areaVectorField(Foam::fac::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fvexp ')'         { $$ = new Foam::areaVectorField(Foam::fac::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fvexp ')'           { $$ = new Foam::areaVectorField(Foam::fac::edgeIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fvexp ')'             { $$ = new Foam::areaVectorField(Foam::fac::edgeSum(*$3)); delete $3; }
        | TOKEN_min '(' vexp ',' vexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' vexp ',' vexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::areaVectorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_minPosition '(' exp ')'           { 
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.aMesh().areaCentres()
                )
            ); 
            delete $3; 
        }
        | TOKEN_max '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::areaVectorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_maxPosition '(' exp ')'           { 
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.aMesh().areaCentres()
                )
            ); 
            delete $3; 
        }
        | TOKEN_sum '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::areaVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' vexp ')'             { $$ = driver.makeConstantField<Foam::areaVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_grad '(' exp ')'                  { $$ = new Foam::areaVectorField(Foam::fac::grad(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ',' vexp ')'        { $$ = new Foam::areaVectorField(Foam::fac::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_VID                               { $$=driver.getField<Foam::areaVectorField>(*$1).ptr(); }
        | TOKEN_ddt '(' TOKEN_VID ')'             { $$ = Foam::fac::ddt( driver.getField<Foam::areaVectorField>(*$3,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_VID ')'         { $$=new Foam::areaVectorField(driver.getField<Foam::areaVectorField>(*$3,true)->oldTime()); }
;

fsexp:  TOKEN_surf '(' scalar ')'           { $$ = driver.makeConstantField<Foam::edgeScalarField>($3); }
        | fsexp '+' fsexp 		    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 + *$3); delete $1; delete $3; }
        | fsexp '*' fsexp 		    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '&' fvexp 		    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 & *$3); delete $1; delete $3; }
        | ftexp TOKEN_AND ftexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fyexp TOKEN_AND ftexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fhexp TOKEN_AND ftexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | ftexp TOKEN_AND fyexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fyexp TOKEN_AND fyexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fhexp TOKEN_AND fyexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | ftexp TOKEN_AND fhexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fyexp TOKEN_AND fhexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fhexp TOKEN_AND fhexp 	    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 && *$3); delete $1; delete $3; }
        | fsexp '/' fsexp 		    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 / *$3); delete $1; delete $3; }
        | fsexp '-' fsexp 		    { sameSize($1,$3); $$ = new Foam::edgeScalarField(*$1 - *$3); delete $1; delete $3;}
        | TOKEN_pow '(' fsexp ',' scalar ')'{ $$ = new Foam::edgeScalarField(Foam::pow(*$3, $5)); delete $3; }
        | TOKEN_log '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::log(*$3)); delete $3; }
        | TOKEN_exp '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::exp(*$3)); delete $3; }
        | TOKEN_sqr '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::sqr(*$3)); delete $3; }
        | TOKEN_sqrt '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::sqrt(*$3)); delete $3; }
        | TOKEN_sin '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::sin(*$3)); delete $3; }
        | TOKEN_cos '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::cos(*$3)); delete $3; }
        | TOKEN_tan '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::tan(*$3)); delete $3; }
        | TOKEN_log10 '(' fsexp ')'         { $$ = new Foam::edgeScalarField(Foam::log10(*$3)); delete $3; }
        | TOKEN_asin '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::asin(*$3)); delete $3; }
        | TOKEN_acos '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::acos(*$3)); delete $3; }
        | TOKEN_atan '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::atan(*$3)); delete $3; }
        | TOKEN_sinh '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::sinh(*$3)); delete $3; }
        | TOKEN_cosh '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::cosh(*$3)); delete $3; }
        | TOKEN_tanh '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::tanh(*$3)); delete $3; }
        | TOKEN_asinh '(' fsexp ')'         { $$ = new Foam::edgeScalarField(Foam::asinh(*$3)); delete $3; }
        | TOKEN_acosh '(' fsexp ')'         { $$ = new Foam::edgeScalarField(Foam::acosh(*$3)); delete $3; }
        | TOKEN_atanh '(' fsexp ')'         { $$ = new Foam::edgeScalarField(Foam::atanh(*$3)); delete $3; }
        | TOKEN_erf '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::erf(*$3)); delete $3; }
        | TOKEN_erfc '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::erfc(*$3)); delete $3; }
        | TOKEN_lgamma '(' fsexp ')'        { $$ = new Foam::edgeScalarField(Foam::lgamma(*$3)); delete $3; }
        | TOKEN_besselJ0 '(' fsexp ')'      { $$ = new Foam::edgeScalarField(Foam::j0(*$3)); delete $3; }
        | TOKEN_besselJ1 '(' fsexp ')'      { $$ = new Foam::edgeScalarField(Foam::j1(*$3)); delete $3; }
        | TOKEN_besselY0 '(' fsexp ')'      { $$ = new Foam::edgeScalarField(Foam::y0(*$3)); delete $3; }
        | TOKEN_besselY1 '(' fsexp ')'      { $$ = new Foam::edgeScalarField(Foam::y1(*$3)); delete $3; }
        | TOKEN_sign '(' fsexp ')'          { $$ = new Foam::edgeScalarField(Foam::sign(*$3)); delete $3; }
        | TOKEN_pos '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::pos(*$3)); delete $3; }
        | TOKEN_neg '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::neg(*$3)); delete $3; }
        | TOKEN_min '(' fsexp ',' fsexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fsexp ',' fsexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fsexp ')'           { $$ = driver.makeConstantField<Foam::edgeScalarField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fsexp ')'           { $$ = driver.makeConstantField<Foam::edgeScalarField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fsexp ')'           { $$ = driver.makeConstantField<Foam::edgeScalarField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fsexp ')'       { $$ = driver.makeConstantField<Foam::edgeScalarField>(Foam::average(*$3).value()); delete $3; }
        | '-' fsexp %prec TOKEN_NEG         { $$ = new Foam::edgeScalarField(-*$2); delete $2; }
        | '(' fsexp ')'		            { $$ = $2; }  
        | fvexp '.' vectorComponentSwitch TOKEN_x                     { $$ = new Foam::edgeScalarField($1->component(0)); delete $1; }
        | fvexp '.' vectorComponentSwitch TOKEN_y                     { $$ = new Foam::edgeScalarField($1->component(1)); delete $1; }
        | fvexp '.' vectorComponentSwitch TOKEN_z                     { $$ = new Foam::edgeScalarField($1->component(2)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_xx                { $$ = new Foam::edgeScalarField($1->component(0)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_xy                { $$ = new Foam::edgeScalarField($1->component(1)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_xz                { $$ = new Foam::edgeScalarField($1->component(2)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_yx                { $$ = new Foam::edgeScalarField($1->component(3)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_yy                { $$ = new Foam::edgeScalarField($1->component(4)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_yz                { $$ = new Foam::edgeScalarField($1->component(5)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_zx                { $$ = new Foam::edgeScalarField($1->component(6)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_zy                { $$ = new Foam::edgeScalarField($1->component(7)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_zz                { $$ = new Foam::edgeScalarField($1->component(8)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_xx                { $$ = new Foam::edgeScalarField($1->component(0)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_xy                { $$ = new Foam::edgeScalarField($1->component(1)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_xz                { $$ = new Foam::edgeScalarField($1->component(2)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_yy                { $$ = new Foam::edgeScalarField($1->component(3)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_yz                { $$ = new Foam::edgeScalarField($1->component(4)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_zz                { $$ = new Foam::edgeScalarField($1->component(5)); delete $1; }
        | fhexp '.' tensorComponentSwitch TOKEN_ii                { $$ = new Foam::edgeScalarField($1->component(0)); delete $1; }
        | flexp '?' fsexp ':' fsexp         { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::edgeScalarField>(0.)); delete $1; delete $3; delete $5; }
        | TOKEN_mag '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fvexp ')'           { $$ = new Foam::edgeScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' ftexp ')'           { $$ = new Foam::edgeScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fyexp ')'           { $$ = new Foam::edgeScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fhexp ')'           { $$ = new Foam::edgeScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_magSqr '(' fsexp ')'           { $$ = new Foam::edgeScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' fvexp ')'           { $$ = new Foam::edgeScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' ftexp ')'           { $$ = new Foam::edgeScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' fyexp ')'           { $$ = new Foam::edgeScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' fhexp ')'           { $$ = new Foam::edgeScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_length '(' ')'                { $$ = driver.makeLengthField(); }
        | TOKEN_lnGrad '(' exp ')'          { $$ = new Foam::edgeScalarField(Foam::fac::lnGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' exp ')'     { $$ = new Foam::edgeScalarField(Foam::fac::interpolate(*$3)); delete $3; }
        | TOKEN_FSID                        { $$ = driver.getField<Foam::edgeScalarField>(*$1).ptr(); }
        | TOKEN_oldTime '(' TOKEN_FSID ')'  { $$=new Foam::edgeScalarField(driver.getField<Foam::edgeScalarField>(*$3,true)->oldTime()); }
;
 
fvexp:  fvector                            { $$ = $1; }
        | fvexp '+' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 + *$3); delete $1; delete $3; }
        | fsexp '*' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '*' fsexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 * *$3); delete $1; delete $3; }
        | ftexp '&' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '&' ftexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 & *$3); delete $1; delete $3; }
        | fhexp '&' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '&' fhexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 & *$3); delete $1; delete $3; }
        | fyexp '&' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '&' fyexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '^' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 ^ *$3); delete $1; delete $3; }
        | fvexp '/' fsexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 / *$3); delete $1; delete $3; }
        | fvexp '-' fvexp 		   { sameSize($1,$3); $$ = new Foam::edgeVectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' fvexp %prec TOKEN_NEG 	   { $$ = new Foam::edgeVectorField(-*$2); delete $2; }
        | '(' fvexp ')'		           { $$ = $2; }  
        | flexp '?' fvexp ':' fvexp        { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::edgeVectorField>(Foam::vector::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_fposition '(' ')'          { $$ = driver.makeEdgePositionField(); }
        | TOKEN_fprojection '(' ')'        { $$ = driver.makeEdgeProjectionField(); }
        | TOKEN_face '(' ')'               { $$ = driver.makeEdgeField(); }
        | TOKEN_lnGrad '(' vexp ')'        { $$ = new Foam::edgeVectorField(Foam::fac::lnGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' vexp ')'   { $$ = new Foam::edgeVectorField(Foam::fac::interpolate(*$3)); delete $3; }
        | TOKEN_min '(' fvexp ',' fvexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fvexp ',' fvexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::edgeVectorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_minPosition '(' fsexp ')'           { 
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.aMesh().edgeCentres()
                )
            ); 
            delete $3; 
        }
        | TOKEN_max '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::edgeVectorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_maxPosition '(' fsexp ')'           { 
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.aMesh().edgeCentres()
                )
            ); 
            delete $3; 
        }
        | TOKEN_sum '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::edgeVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fvexp ')'      { $$ = driver.makeConstantField<Foam::edgeVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FVID                       { $$ = driver.getField<Foam::edgeVectorField>(*$1).ptr(); }
        | TOKEN_oldTime '(' TOKEN_FVID ')' { $$=new Foam::edgeVectorField(driver.getField<Foam::edgeVectorField>(*$3,true)->oldTime()); }
;
 
scalar:	TOKEN_NUM		        { $$ = $1; }
        | '-' TOKEN_NUM         	{ $$ = -$2; } 
;

exp:    TOKEN_NUM                                  { $$ = driver.makeConstantField<Foam::areaScalarField>($1); }
        | exp '+' exp 		                   { sameSize($1,$3); $$ = new Foam::areaScalarField(*$1 + *$3); delete $1; delete $3; }
        | exp '-' exp 		                   { sameSize($1,$3); $$ = new Foam::areaScalarField(*$1 - *$3); delete $1; delete $3; }
        | exp '*' exp 		                   { sameSize($1,$3); $$ = new Foam::areaScalarField(*$1 * *$3); delete $1; delete $3; }
        | exp '%' exp 		                   { sameSize($1,$3); $$ = driver.makeModuloField(*$1,*$3); delete $1; delete $3; }
        | exp '/' exp 		                   { sameSize($1,$3); $$ = new Foam::areaScalarField(*$1 / *$3); delete $1; delete $3; }
        | TOKEN_pow '(' exp ',' scalar ')'	   { $$ = new Foam::areaScalarField(Foam::pow(*$3, $5)); delete $3; }
        | TOKEN_log '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::log(*$3)); delete $3; }
        | TOKEN_exp '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::exp(*$3)); delete $3; }
        | vexp '&' vexp 	                   { $$ = new Foam::areaScalarField(*$1 & *$3); delete $1; delete $3;}
        | texp TOKEN_AND texp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | hexp TOKEN_AND texp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | yexp TOKEN_AND texp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | texp TOKEN_AND yexp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | hexp TOKEN_AND yexp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | yexp TOKEN_AND yexp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | texp TOKEN_AND hexp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | hexp TOKEN_AND hexp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | yexp TOKEN_AND hexp 	                   { $$ = new Foam::areaScalarField(*$1 && *$3); delete $1; delete $3; }
        | '-' exp %prec TOKEN_NEG 	           { $$ = new Foam::areaScalarField(-*$2); delete $2; }
	| '(' exp ')'		                   { $$ = $2; }
        | TOKEN_sqr '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::sqr(*$3)); delete $3; }
        | TOKEN_sqrt '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::sqrt(*$3)); delete $3; }
        | TOKEN_sin '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::sin(*$3)); delete $3; }
        | TOKEN_cos '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::cos(*$3)); delete $3; }
        | TOKEN_tan '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::tan(*$3)); delete $3; }
        | TOKEN_log10 '(' exp ')'                  { $$ = new Foam::areaScalarField(Foam::log10(*$3)); delete $3; }
        | TOKEN_asin '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::asin(*$3)); delete $3; }
        | TOKEN_acos '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::acos(*$3)); delete $3; }
        | TOKEN_atan '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::atan(*$3)); delete $3; }
        | TOKEN_sinh '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::sinh(*$3)); delete $3; }
        | TOKEN_cosh '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::cosh(*$3)); delete $3; }
        | TOKEN_tanh '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::tanh(*$3)); delete $3; }
        | TOKEN_asinh '(' exp ')'                  { $$ = new Foam::areaScalarField(Foam::asinh(*$3)); delete $3; }
        | TOKEN_acosh '(' exp ')'                  { $$ = new Foam::areaScalarField(Foam::acosh(*$3)); delete $3; }
        | TOKEN_atanh '(' exp ')'                  { $$ = new Foam::areaScalarField(Foam::atanh(*$3)); delete $3; }
        | TOKEN_erf '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::erf(*$3)); delete $3; }
        | TOKEN_erfc '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::erfc(*$3)); delete $3; }
        | TOKEN_lgamma '(' exp ')'                 { $$ = new Foam::areaScalarField(Foam::lgamma(*$3)); delete $3; }
        | TOKEN_besselJ0 '(' exp ')'               { $$ = new Foam::areaScalarField(Foam::j0(*$3)); delete $3; }
        | TOKEN_besselJ1 '(' exp ')'               { $$ = new Foam::areaScalarField(Foam::j1(*$3)); delete $3; }
        | TOKEN_besselY0 '(' exp ')'               { $$ = new Foam::areaScalarField(Foam::y0(*$3)); delete $3; }
        | TOKEN_besselY1 '(' exp ')'               { $$ = new Foam::areaScalarField(Foam::y1(*$3)); delete $3; }
        | TOKEN_sign '(' exp ')'                   { $$ = new Foam::areaScalarField(Foam::sign(*$3)); delete $3; }
        | TOKEN_pos '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::pos(*$3)); delete $3; }
        | TOKEN_neg '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::neg(*$3)); delete $3; }
        | TOKEN_min '(' exp ',' exp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' exp ',' exp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' exp ')'                    { $$ = driver.makeConstantField<Foam::areaScalarField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' exp ')'                    { $$ = driver.makeConstantField<Foam::areaScalarField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' exp ')'                    { $$ = driver.makeConstantField<Foam::areaScalarField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' exp ')'                { $$ = driver.makeConstantField<Foam::areaScalarField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_mag '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' vexp ')'                   { $$ = new Foam::areaScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' texp ')'                   { $$ = new Foam::areaScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' yexp ')'                   { $$ = new Foam::areaScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' hexp ')'                   { $$ = new Foam::areaScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_magSqr '(' exp ')'                    { $$ = new Foam::areaScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' vexp ')'                   { $$ = new Foam::areaScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' texp ')'                   { $$ = new Foam::areaScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' yexp ')'                   { $$ = new Foam::areaScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' hexp ')'                   { $$ = new Foam::areaScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_div '(' vexp ')'                   { $$ = new Foam::areaScalarField(Foam::fac::div(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ')'                  { $$ = new Foam::areaScalarField(Foam::fac::div(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ',' exp ')'          { $$ = new Foam::areaScalarField(Foam::fac::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_laplacian '(' exp ')'              { $$ = new Foam::areaScalarField(Foam::fac::laplacian(*$3)); delete $3; }
        | TOKEN_laplacian '(' exp ',' exp ')'      { $$ = new Foam::areaScalarField(Foam::fac::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_laplacian '(' fsexp ',' exp ')'    { $$ = new Foam::areaScalarField(Foam::fac::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fsexp ')'          { $$ = new Foam::areaScalarField(Foam::fac::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fsexp ')'            { $$ = new Foam::areaScalarField(Foam::fac::edgeIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fsexp ')'              { $$ = new Foam::areaScalarField(Foam::fac::edgeSum(*$3)); delete $3; }
        | vexp '.' vectorComponentSwitch TOKEN_x                             { $$ = new Foam::areaScalarField($1->component(0)); delete $1; }
        | vexp '.' vectorComponentSwitch TOKEN_y                             { $$ = new Foam::areaScalarField($1->component(1)); delete $1; }
        | vexp '.' vectorComponentSwitch TOKEN_z                             { $$ = new Foam::areaScalarField($1->component(2)); delete $1; }
        | texp '.' tensorComponentSwitch TOKEN_xx                        { $$ = new Foam::areaScalarField($1->component(0)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_xy                        { $$ = new Foam::areaScalarField($1->component(1)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_xz                        { $$ = new Foam::areaScalarField($1->component(2)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_yx                        { $$ = new Foam::areaScalarField($1->component(3)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_yy                        { $$ = new Foam::areaScalarField($1->component(4)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_yz                        { $$ = new Foam::areaScalarField($1->component(5)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_zx                        { $$ = new Foam::areaScalarField($1->component(6)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_zy                        { $$ = new Foam::areaScalarField($1->component(7)); delete $1;  }
        | texp '.' tensorComponentSwitch TOKEN_zz                        { $$ = new Foam::areaScalarField($1->component(8)); delete $1;  }
        | yexp '.' tensorComponentSwitch TOKEN_xx                        { $$ = new Foam::areaScalarField($1->component(0)); delete $1;  }
        | yexp '.' tensorComponentSwitch TOKEN_xy                        { $$ = new Foam::areaScalarField($1->component(1)); delete $1;  }
        | yexp '.' tensorComponentSwitch TOKEN_xz                        { $$ = new Foam::areaScalarField($1->component(2)); delete $1;  }
        | yexp '.' tensorComponentSwitch TOKEN_yy                        { $$ = new Foam::areaScalarField($1->component(3)); delete $1;  }
        | yexp '.' tensorComponentSwitch TOKEN_yz                        { $$ = new Foam::areaScalarField($1->component(4)); delete $1;  }
        | yexp '.' tensorComponentSwitch TOKEN_zz                        { $$ = new Foam::areaScalarField($1->component(5)); delete $1;  }
        | hexp '.' tensorComponentSwitch TOKEN_ii                        { $$ = new Foam::areaScalarField($1->component(0)); delete $1;  }
        | lexp '?' exp ':' exp                     { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::areaScalarField>(0.)); delete $1; delete $3; delete $5; }
        | TOKEN_pi                                 { $$ = driver.makeConstantField<Foam::areaScalarField>(M_PI); }
        | TOKEN_rdist '(' vexp ')'                 { $$ = driver.makeRDistanceField(*$3); delete $3; }
        | TOKEN_area '(' ')'                       { $$ = driver.makeAreaField(); }
        | TOKEN_rand '(' ')'                       { $$ = driver.makeRandomField(); }
        | TOKEN_randNormal '(' ')'                 { $$ = driver.makeGaussRandomField(); }
        | TOKEN_rand '(' TOKEN_INT ')'             { $$ = driver.makeRandomField(-$3); }
        | TOKEN_randNormal '('  TOKEN_INT ')'      { $$ = driver.makeGaussRandomField(-$3); }
        | TOKEN_randFixed '(' ')'                  { $$ = driver.makeRandomField(1); }
        | TOKEN_randNormalFixed '(' ')'            { $$ = driver.makeGaussRandomField(1); }
        | TOKEN_randFixed '(' TOKEN_INT ')'        { $$ = driver.makeRandomField($3+1); }
        | TOKEN_randNormalFixed '('  TOKEN_INT ')' { $$ = driver.makeGaussRandomField($3+1); }
        | TOKEN_id '(' ')'                         { $$ = driver.makeCellIdField(); }
        | TOKEN_cpu'(' ')'                         { $$ = driver.makeConstantField<Foam::areaScalarField>(Foam::Pstream::myProcNo()); }
        | TOKEN_deltaT '(' ')'                     { $$ = driver.makeConstantField<Foam::areaScalarField>(driver.runTime().deltaT().value()); }
        | TOKEN_time '(' ')'                       { $$ = driver.makeConstantField<Foam::areaScalarField>(driver.runTime().time().value()); }
        | TOKEN_SID		                   { $$ = driver.getField<Foam::areaScalarField>(*$1).ptr(); }
        | TOKEN_ddt '(' TOKEN_SID ')'              { $$ = Foam::fac::ddt( driver.getField<Foam::areaScalarField>(*$3,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_SID ')'         { $$=new Foam::areaScalarField(driver.getField<Foam::areaScalarField>(*$3,true)->oldTime()); }
        | TOKEN_LINE		                   { $$ = driver.makeConstantField<Foam::areaScalarField>(driver.getLineValue(*$1,driver.runTime().time().value())); delete $1; }
        | TOKEN_LOOKUP '(' exp ')'		   { $$ = driver.makeField<Foam::areaScalarField>(driver.getLookup(*$1,*$3)); delete $1;  delete$3;}
;

lexp: TOKEN_TRUE                       { $$ = driver.makeConstantField<Foam::areaScalarField>(1); }
    | TOKEN_FALSE                      { $$ = driver.makeConstantField<Foam::areaScalarField>(0); }
    | exp '<' exp                      { sameSize($1,$3); $$ = driver.doCompare($1,std::less<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp '>' exp                      { sameSize($1,$3); $$ = driver.doCompare($1,std::greater<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_LEQ exp                { sameSize($1,$3); $$ = driver.doCompare($1,std::less_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_GEQ exp                { sameSize($1,$3); $$ = driver.doCompare($1,std::greater_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_EQ exp                 { sameSize($1,$3); $$ = driver.doCompare($1,std::equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_NEQ exp                { sameSize($1,$3); $$ = driver.doCompare($1,std::not_equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '(' lexp ')'		       { $$ = $2; }
    | lexp TOKEN_AND lexp              { sameSize($1,$3); $$ = driver.doLogicalOp($1,std::logical_and<Foam::scalar>(),$3);  delete $1; delete $3; }
    | lexp TOKEN_OR lexp               { sameSize($1,$3); $$ = driver.doLogicalOp($1,std::logical_or<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '!' lexp %prec TOKEN_NOT         { $$ = driver.doLogicalNot($2); delete $2; }
;

flexp: TOKEN_surf '(' TOKEN_TRUE ')'  { $$ = driver.makeConstantField<Foam::edgeScalarField>(1); }
    | TOKEN_surf '(' TOKEN_FALSE ')'  { $$ = driver.makeConstantField<Foam::edgeScalarField>(0); }
    | fsexp '<' fsexp                 { sameSize($1,$3); $$ = driver.doCompare($1,std::less<Foam::scalar>(),$3);  delete $1; delete $3; }
    | fsexp '>' fsexp                 { sameSize($1,$3); $$ = driver.doCompare($1,std::greater<Foam::scalar>(),$3);  delete $1; delete $3; }
    | fsexp TOKEN_LEQ fsexp           { sameSize($1,$3); $$ = driver.doCompare($1,std::less_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | fsexp TOKEN_GEQ fsexp           { sameSize($1,$3); $$ = driver.doCompare($1,std::greater_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | fsexp TOKEN_EQ fsexp            { sameSize($1,$3); $$ = driver.doCompare($1,std::equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | fsexp TOKEN_NEQ fsexp           { sameSize($1,$3); $$ = driver.doCompare($1,std::not_equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '(' flexp ')'		      { $$ = $2; }
    | flexp TOKEN_AND flexp           { sameSize($1,$3); $$ = driver.doLogicalOp($1,std::logical_and<Foam::scalar>(),$3);  delete $1; delete $3; }
    | flexp TOKEN_OR flexp            { sameSize($1,$3); $$ = driver.doLogicalOp($1,std::logical_or<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '!' flexp %prec TOKEN_NOT       { $$ = driver.doLogicalNot($2); delete $2; }
;

texp:   tensor                  { $$ = $1; }
        | texp '+' texp 		          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | texp '+' yexp 		          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | texp '+' hexp 		          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | yexp '+' texp 		          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | hexp '+' texp 		          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | exp '*' texp 	   	                  { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | texp '*' exp 	   	                  { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | vexp '*' vexp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | texp '&' texp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | yexp '&' texp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | texp '&' yexp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | hexp '&' texp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | texp '&' hexp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | texp '/' exp 	   	                  { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 / *$3); delete $1; delete $3;  }  
        | texp '-' texp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | texp '-' yexp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | texp '-' hexp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | yexp '-' texp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | hexp '-' texp 	   	          { sameSize($1,$3); $$ = new Foam::areaTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | '-' texp %prec TOKEN_NEG 	           { $$ = new Foam::areaTensorField(-*$2); delete $2;  } 
        | '(' texp ')'		        { $$ = $2; }  
        | TOKEN_skew '(' texp ')' 	           { $$ = new Foam::areaTensorField( Foam::skew(*$3) ); delete $3;  } 
        | TOKEN_inv '(' texp ')' 	           { $$ = new Foam::areaTensorField( Foam::inv(*$3) ); delete $3;  } 
        | TOKEN_dev '(' texp ')' 	           { $$ = new Foam::areaTensorField( Foam::dev(*$3) ); delete $3;  } 
        | lexp '?' texp ':' texp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::areaTensorField>(Foam::tensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_laplacian '(' fsexp ',' texp ')'  { $$ = new Foam::areaTensorField(Foam::fac::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' ftexp ')'         { $$ = new Foam::areaTensorField(Foam::fac::average(*$3)); delete $3; }
        | TOKEN_integrate '(' ftexp ')'           { $$ = new Foam::areaTensorField(Foam::fac::edgeIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' ftexp ')'             { $$ = new Foam::areaTensorField(Foam::fac::edgeSum(*$3)); delete $3; }
        | TOKEN_min '(' texp ',' texp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' texp ',' texp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' texp ')'                 { $$ = driver.makeConstantField<Foam::areaTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' texp ')'                 { $$ = driver.makeConstantField<Foam::areaTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' texp ')'                 { $$ = driver.makeConstantField<Foam::areaTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' texp ')'             { $$ = driver.makeConstantField<Foam::areaTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_grad '(' vexp ')'                 { $$ = new Foam::areaTensorField(Foam::fac::grad(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ',' texp ')'        { $$ = new Foam::areaTensorField(Foam::fac::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_TID                               { $$=driver.getField<Foam::areaTensorField>(*$1).ptr(); }
        | TOKEN_ddt '(' TOKEN_TID ')'		   { $$ = Foam::fac::ddt( driver.getField<Foam::areaTensorField>(*$3,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_TID ')'		   { $$ = new Foam::areaTensorField( driver.getField<Foam::areaTensorField>(*$3,true)->oldTime()); }
;

yexp:   symmTensor                  { $$ = $1; }
        | yexp '+' yexp 		          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | hexp '+' yexp 		          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | yexp '+' hexp 		          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | exp '*' yexp 	   	                  { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | yexp '*' exp 	   	                  { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | yexp '&' yexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | hexp '&' yexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | yexp '&' hexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | yexp '/' exp 	   	                  { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 / *$3); delete $1; delete $3;  }  
        | yexp '-' yexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | hexp '-' yexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | yexp '-' hexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSymmTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | '-' yexp %prec TOKEN_NEG 	           { $$ = new Foam::areaSymmTensorField(-*$2); delete $2;  } 
        | '(' yexp ')'		        { $$ = $2; }  
        | TOKEN_symm '(' texp ')' 	           { $$ = new Foam::areaSymmTensorField( Foam::symm(*$3) ); delete $3;  } 
        | TOKEN_symm '(' yexp ')' 	           { $$ = new Foam::areaSymmTensorField( Foam::symm(*$3) ); delete $3;  } 
        | TOKEN_inv '(' yexp ')' 	           { $$ = new Foam::areaSymmTensorField( Foam::inv(*$3) ); delete $3;  } 
        | TOKEN_dev '(' yexp ')' 	           { $$ = new Foam::areaSymmTensorField( Foam::dev(*$3) ); delete $3;  } 
        | lexp '?' yexp ':' yexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::areaSymmTensorField>(Foam::symmTensor::zero)); delete $1; delete $3; delete $5; }
// Not instantiated in 1.6-ext        | TOKEN_laplacian '(' fsexp ',' yexp ')'  { $$ = new Foam::areaSymmTensorField(Foam::fac::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fyexp ')'         { $$ = new Foam::areaSymmTensorField(Foam::fac::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fyexp ')'           { $$ = new Foam::areaSymmTensorField(Foam::fac::edgeIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fyexp ')'             { $$ = new Foam::areaSymmTensorField(Foam::fac::edgeSum(*$3)); delete $3; }
        | TOKEN_min '(' yexp ',' yexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' yexp ',' yexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' yexp ')'                 { $$ = driver.makeConstantField<Foam::areaSymmTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' yexp ')'                 { $$ = driver.makeConstantField<Foam::areaSymmTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' yexp ')'                 { $$ = driver.makeConstantField<Foam::areaSymmTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' yexp ')'             { $$ = driver.makeConstantField<Foam::areaSymmTensorField>(Foam::average(*$3).value()); delete $3; }
// Not instantiated in 1.6-ext         | TOKEN_div '(' fsexp ',' yexp ')'        { $$ = new Foam::areaSymmTensorField(Foam::fac::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_YID                               { $$=driver.getField<Foam::areaSymmTensorField>(*$1).ptr(); }
// Not instantiated in 1.6-ext?        | TOKEN_ddt '(' TOKEN_YID ')'		   { $$ = Foam::fac::ddt( driver.getField<Foam::areaSymmTensorField>(*$3,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_YID ')'	   { $$ = new Foam::areaSymmTensorField( driver.getField<Foam::areaSymmTensorField>(*$3,true)->oldTime()); }
;

hexp:   sphericalTensor                  { $$ = $1; }
        | hexp '+' hexp 		          { sameSize($1,$3); $$ = new Foam::areaSphericalTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | exp '*' hexp 	   	                  { sameSize($1,$3); $$ = new Foam::areaSphericalTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | hexp '*' exp 	   	                  { sameSize($1,$3); $$ = new Foam::areaSphericalTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | hexp '&' hexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSphericalTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | hexp '/' exp 	   	                  { sameSize($1,$3); $$ = new Foam::areaSphericalTensorField(*$1 / *$3); delete $1; delete $3;  }  
        | hexp '-' hexp 	   	          { sameSize($1,$3); $$ = new Foam::areaSphericalTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | '-' hexp %prec TOKEN_NEG 	           { $$ = new Foam::areaSphericalTensorField(-*$2); delete $2;  } 
        | '(' hexp ')'		        { $$ = $2; }  
        | TOKEN_inv '(' hexp ')' 	           { 
            $$ = driver.makeField<Foam::areaSphericalTensorField>( Foam::inv($3->internalField()) ); 
            delete $3;  } 
        | lexp '?' hexp ':' hexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::areaSphericalTensorField>(Foam::sphericalTensor::zero)); delete $1; delete $3; delete $5; }
// Not instantiated in 1.6-ext         | TOKEN_laplacian '(' fsexp ',' hexp ')'  { $$ = new Foam::areaSphericalTensorField(Foam::fac::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fhexp ')'         { $$ = new Foam::areaSphericalTensorField(Foam::fac::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fhexp ')'           { $$ = new Foam::areaSphericalTensorField(Foam::fac::edgeIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fhexp ')'             { $$ = new Foam::areaSphericalTensorField(Foam::fac::edgeSum(*$3)); delete $3; }
        | TOKEN_min '(' hexp ',' hexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' hexp ',' hexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' hexp ')'                 { $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' hexp ')'                 { $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' hexp ')'                 { $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' hexp ')'             { $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(Foam::average(*$3).value()); delete $3; }
// Not instantiated in 1.6-ext         | TOKEN_div '(' fsexp ',' hexp ')'        { $$ = new Foam::areaSphericalTensorField(Foam::fac::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_HID                               { $$=driver.getField<Foam::areaSphericalTensorField>(*$1).ptr(); }
// Not instantiated in 1.6-ext?        | TOKEN_ddt '(' TOKEN_HID ')'		   { $$ = Foam::fac::ddt( driver.getField<Foam::areaSphericalTensorField>(*$3,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_HID ')'	   { $$ = new Foam::areaSphericalTensorField( driver.getField<Foam::areaSphericalTensorField>(*$3,true)->oldTime()); }
;

ftexp:   ftensor                  { $$ = $1; }
        | ftexp '+' ftexp 		          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | ftexp '+' fyexp 		          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | ftexp '+' fhexp 		          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fyexp '+' ftexp 		          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fhexp '+' ftexp 		          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fsexp '*' ftexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | ftexp '*' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | fvexp '*' fvexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | ftexp '&' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | fyexp '&' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | ftexp '&' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | fhexp '&' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | ftexp '&' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | ftexp '/' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 / *$3); delete $1; delete $3;  }  
        | ftexp '-' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | ftexp '-' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | ftexp '-' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | fyexp '-' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | fhexp '-' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | '-' ftexp %prec TOKEN_NEG 	           { $$ = new Foam::edgeTensorField(-*$2); delete $2;  } 
        | '(' ftexp ')'		        { $$ = $2; }  
        | TOKEN_skew '(' ftexp ')' 	           { $$ = new Foam::edgeTensorField( Foam::skew(*$3) ); delete $3;  } 
        | TOKEN_inv '(' ftexp ')' 	           { $$ = new Foam::edgeTensorField( Foam::inv(*$3) ); delete $3;  } 
        | TOKEN_dev '(' ftexp ')' 	           { $$ = new Foam::edgeTensorField( Foam::dev(*$3) ); delete $3;  } 
        | flexp '?' ftexp ':' ftexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::edgeTensorField>(Foam::tensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_lnGrad '(' texp ')'           { $$ = new Foam::edgeTensorField(Foam::fac::lnGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' texp ')'           { $$ = new Foam::edgeTensorField(Foam::fac::interpolate(*$3)); delete $3; }
        | TOKEN_min '(' ftexp ',' ftexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' ftexp ',' ftexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' ftexp ')'                 { $$ = driver.makeConstantField<Foam::edgeTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' ftexp ')'                 { $$ = driver.makeConstantField<Foam::edgeTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' ftexp ')'                 { $$ = driver.makeConstantField<Foam::edgeTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' ftexp ')'             { $$ = driver.makeConstantField<Foam::edgeTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FTID                               { $$=driver.getField<Foam::edgeTensorField>(*$1).ptr(); }
//        | TOKEN_ddt '(' TOKEN_FTID ')'		   { $$ = Foam::fac::ddt( driver.getOrReadField<Foam::edgeTensorField>(*$3,true,true)() ).ptr(); } // no fac::ddt for edge Fields
        | TOKEN_oldTime '(' TOKEN_FTID ')'	   { $$ = new Foam::edgeTensorField( driver.getField<Foam::edgeTensorField>(*$3,true)->oldTime()); }
;

fyexp:   fsymmTensor                  { $$ = $1; }
        | fyexp '+' fyexp 		          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fhexp '+' fyexp 		          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fyexp '+' fhexp 		          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fsexp '*' fyexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | fyexp '*' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | fyexp '&' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | fhexp '&' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | fyexp '&' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | fyexp '/' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 / *$3); delete $1; delete $3;  }  
        | fyexp '-' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | fhexp '-' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | fyexp '-' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSymmTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | '-' fyexp %prec TOKEN_NEG 	           { $$ = new Foam::edgeSymmTensorField(-*$2); delete $2;  } 
        | '(' fyexp ')'		        { $$ = $2; }  
        | TOKEN_symm '(' ftexp ')' 	           { $$ = new Foam::edgeSymmTensorField( Foam::symm(*$3) ); delete $3;  } 
        | TOKEN_symm '(' fyexp ')' 	           { $$ = new Foam::edgeSymmTensorField( Foam::symm(*$3) ); delete $3;  } 
        | TOKEN_inv '(' fyexp ')' 	           { $$ = new Foam::edgeSymmTensorField( Foam::inv(*$3) ); delete $3;  } 
        | TOKEN_dev '(' fyexp ')' 	           { $$ = new Foam::edgeSymmTensorField( Foam::dev(*$3) ); delete $3;  } 
        | flexp '?' fyexp ':' fyexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::edgeSymmTensorField>(Foam::symmTensor::zero)); delete $1; delete $3; delete $5; }
//  Not instantiated in 1.6-ext?        | TOKEN_lnGrad '(' yexp ')'           { $$ = new Foam::edgeSymmTensorField(Foam::fac::lnGrad(*$3)); delete $3; }
//  Not instantiated in 1.6-ext?       | TOKEN_interpolate '(' yexp ')'           { $$ = new Foam::edgeSymmTensorField(Foam::fac::interpolate(*$3)); delete $3; }
        | TOKEN_min '(' fyexp ',' fyexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fyexp ',' fyexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fyexp ')'                 { $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fyexp ')'                 { $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fyexp ')'                 { $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fyexp ')'             { $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FYID                               { $$=driver.getField<Foam::edgeSymmTensorField>(*$1).ptr(); }
//        | TOKEN_ddt '(' TOKEN_FYID ')'		   { $$ = Foam::fac::ddt( driver.getOrReadField<Foam::edgeSymmTensorField>(*$3,true,true)() ).ptr(); }// no fac::ddt for edge Fields
        | TOKEN_oldTime '(' TOKEN_FYID ')'	   { $$ = new Foam::edgeSymmTensorField( driver.getField<Foam::edgeSymmTensorField>(*$3,true)->oldTime()); }
;

fhexp:   fsphericalTensor                  { $$ = $1; }
        | fhexp '+' fhexp 		          { sameSize($1,$3); $$ = new Foam::edgeSphericalTensorField(*$1 + *$3); delete $1; delete $3;  }  
        | fsexp '*' fhexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeSphericalTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | fhexp '*' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeSphericalTensorField(*$1 * *$3); delete $1; delete $3;  }  
        | fhexp '&' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSphericalTensorField(*$1 & *$3); delete $1; delete $3;  }  
        | fhexp '/' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::edgeSphericalTensorField(*$1 / *$3); delete $1; delete $3;  }  
        | fhexp '-' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::edgeSphericalTensorField(*$1 - *$3); delete $1; delete $3;  }  
        | '-' fhexp %prec TOKEN_NEG 	           { $$ = new Foam::edgeSphericalTensorField(-*$2); delete $2;  } 
        | '(' fhexp ')'		        { $$ = $2; }  
        | TOKEN_inv '(' fhexp ')' 	           { 
            $$ = driver.makeField<Foam::edgeSphericalTensorField>( Foam::inv($3->internalField()) ); 
            delete $3;  } 
        | flexp '?' fhexp ':' fhexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::edgeSphericalTensorField>(Foam::sphericalTensor::zero)); delete $1; delete $3; delete $5; }
//  Not instantiated in 1.6-ext?         | TOKEN_lnGrad '(' hexp ')'           { $$ = new Foam::edgeSphericalTensorField(Foam::fac::lnGrad(*$3)); delete $3; }
// Not instantiated in 1.6-ext?       | TOKEN_interpolate '(' hexp ')'           { $$ = new Foam::edgeSphericalTensorField(Foam::fac::interpolate(*$3)); delete $3; }
       | TOKEN_min '(' fhexp ',' fhexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fhexp ',' fhexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fhexp ')'                 { $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fhexp ')'                 { $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fhexp ')'                 { $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fhexp ')'             { $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FHID                               { $$=driver.getField<Foam::edgeSphericalTensorField>(*$1).ptr(); }
//        | TOKEN_ddt '(' TOKEN_FHID ')'		   { $$ = Foam::fac::ddt( driver.getOrReadField<Foam::edgeSphericalTensorField>(*$3,true,true)() ).ptr(); } // no fac::ddt for edge Fields
        | TOKEN_oldTime '(' TOKEN_FHID ')'	   { $$ = new Foam::edgeSphericalTensorField( driver.getField<Foam::edgeSphericalTensorField>(*$3,true)->oldTime()); }
;

vector: TOKEN_VECTOR '(' exp ',' exp ',' exp ')' {     $$ = driver.makeVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

tensor: TOKEN_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {     $$ = driver.makeTensorField($3,$5,$7,$9,$11,$13,$15,$17,$19);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13; delete $15; delete $17; delete $19; }
;

symmTensor: TOKEN_SYMM_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {     $$ = driver.makeSymmTensorField($3,$5,$7,$9,$11,$13);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;}
;

sphericalTensor: TOKEN_SPHERICAL_TENSOR '(' exp ')' {     $$ = driver.makeSphericalTensorField($3);  delete $3; }
;

fvector: TOKEN_VECTOR '(' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeEdgeVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

ftensor: TOKEN_TENSOR '(' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeEdgeTensorField($3,$5,$7,$9,$11,$13,$15,$17,$19);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13; delete $15; delete $17; delete $19; }
;

fsymmTensor: TOKEN_SYMM_TENSOR '(' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeEdgeSymmTensorField($3,$5,$7,$9,$11,$13);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;}
;

fsphericalTensor: TOKEN_SPHERICAL_TENSOR '(' fsexp ')' {     $$ = driver.makeEdgeSphericalTensorField($3);  delete $3; }
;

%%

void parserFaField::FaFieldValueExpressionParser::error (const parserFaField::FaFieldValueExpressionParser::location_type& l,const std::string& m)
{
     driver.error (l, m);
}
