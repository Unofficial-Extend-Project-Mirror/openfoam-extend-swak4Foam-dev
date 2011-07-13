/*  -*- C++ -*- */

%skeleton "lalr1.cc"
/* %require "2.1a" */
%defines
%define "parser_class_name" "FieldValueExpressionParser"

%{
#include <volFields.H>
#include <surfaceFields.H>
#include <fvcGrad.H>
#include <fvcCurl.H>
#include <fvcMagSqrGradGrad.H>
#include <fvcSnGrad.H>
#include <fvcDiv.H>
#include <fvcSurfaceIntegrate.H>
#include <fvcReconstruct.H>
#include <fvcAverage.H>
#include <surfaceInterpolate.H>
#include <fvcLaplacian.H>

#include <fvMatrix.H>

#include <functional>
#include <cmath>

    namespace Foam {
        class FieldValueExpressionDriver;
    }

    using Foam::FieldValueExpressionDriver;
%}

%name-prefix="parserField"

%parse-param { FieldValueExpressionDriver& driver }
%lex-param { FieldValueExpressionDriver& driver }

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
    Foam::string *name;
    Foam::string *vname;
    Foam::string *fsname;
    Foam::string *fvname;
    Foam::string *setname;
    Foam::string *zonename;
    Foam::volVectorField *vfield;
    Foam::volScalarField *sfield;
    Foam::surfaceScalarField *fsfield;
    Foam::surfaceVectorField *fvfield;
};

%{
#include "FieldValueExpressionDriverYY.H"
#include "FieldValueExpressionDriverLogicalTemplates.H"

#include "swakChecks.H"
%}

%token <name>   TOKEN_LINE  "timeline"
%token <name>   TOKEN_LOOKUP  "lookup"
%token <name>   TOKEN_SID   "scalarID"
%token <vname>  TOKEN_VID   "vectorID"
%token <fsname> TOKEN_FSID  "faceScalarID"
%token <fvname> TOKEN_FVID  "faceVectorID"
%token <setname> TOKEN_SETID "cellSetID" 
%token <zonename> TOKEN_ZONEID "cellZoneID" 
%token <setname> TOKEN_FSETID "faceSetID" 
%token <zonename> TOKEN_FZONEID "faceZoneID" 
%token <val>    TOKEN_NUM   "number"
%token <integer>    TOKEN_INT   "integer"
%token <vec>    TOKEN_VEC   "vector"
%type  <val>    scalar      "sexpression"  
%type  <sfield>    exp        "expression"
%type  <sfield>    lexp       "lexpression"
%type  <fsfield>   flexp       "flexpression"
%type  <vfield>    vexp       "vexpression"
%type  <fsfield>   fsexp      "fsexpression"
%type  <fvfield>   fvexp      "fvexpression"
%type  <vfield>    vector     
%type  <fvfield>    fvector     

%token TOKEN_VECTOR

%token TOKEN_TRUE
%token TOKEN_FALSE

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
%token TOKEN_area
%token TOKEN_volume
%token TOKEN_dist
%token TOKEN_nearDist
%token TOKEN_rdist

%token TOKEN_set
%token TOKEN_zone
%token TOKEN_fset
%token TOKEN_fzone

%token TOKEN_div
%token TOKEN_grad
%token TOKEN_curl
%token TOKEN_snGrad
%token TOKEN_magSqrGradGrad;
%token TOKEN_laplacian

%token TOKEN_integrate
%token TOKEN_surfSum
%token TOKEN_interpolate
%token TOKEN_faceAverage
%token TOKEN_reconstruct

%token TOKEN_surf

%token TOKEN_deltaT
%token TOKEN_time

%token TOKEN_pow
%token TOKEN_log
%token TOKEN_exp
%token TOKEN_mag
%token TOKEN_sin
%token TOKEN_cos
%token TOKEN_tan
%token TOKEN_min
%token TOKEN_max
%token TOKEN_sum
%token TOKEN_average
%token TOKEN_sqr
%token TOKEN_sqrt

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

%printer             { debug_stream () << *$$; } "scalarID" "vectorID" "faceScalarID" "faceVectorID" "cellSetID" "cellZoneID" "faceSetID" "faceZoneID"
%printer             { Foam::OStringStream buff; buff << *$$; debug_stream () << buff.str().c_str(); } "vector"
%destructor          { delete $$; } "timeline" "lookup" "scalarID" "faceScalarID" "faceVectorID" "vectorID" "vector" "expression" "vexpression" "fsexpression" "fvexpression" "lexpression" "flexpression"  "cellSetID"  "cellZoneID"  "faceSetID"  "faceZoneID"
%printer             { debug_stream () << $$; } "number" "integer" "sexpression"
%printer             { debug_stream () << $$->name().c_str(); } "expression"  "vexpression" "lexpression" "flexpression" "fsexpression" "fvexpression"


%%
%start unit;

unit:   exp                     { driver.setScalarResult($1);  }
        | vexp                  { driver.setVectorResult($1);  }
        | lexp                  { driver.setLogicalResult($1); }
;

vexp:   vector                                    { $$ = $1; }
        | vexp '+' vexp 		          { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' vexp 		                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 * *$3); delete $1; delete $3; }
        | vexp '*' exp 		                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 * *$3); delete $1; delete $3; }
        | vexp '/' exp 		                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 / *$3); delete $1; delete $3; }
        | vexp '^' vexp 		          { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 ^ *$3); delete $1; delete $3; }
        | vexp '-' vexp 		          { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' vexp %prec TOKEN_NEG                { $$ = new Foam::volVectorField(-*$2); delete $2; }
        | '(' vexp ')'		                  { $$ = $2; }  
        | lexp '?' vexp ':' vexp                  { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::volVectorField>(Foam::vector())); delete $1; delete $3; delete $5; }
        | TOKEN_position '(' ')'                  { $$ = driver.makePositionField(); }
        | TOKEN_laplacian '(' fsexp ',' vexp ')'  { $$ = new Foam::volVectorField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fvexp ')'         { $$ = new Foam::volVectorField(Foam::fvc::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fvexp ')'           { $$ = new Foam::volVectorField(Foam::fvc::surfaceIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fvexp ')'             { $$ = new Foam::volVectorField(Foam::fvc::surfaceSum(*$3)); delete $3; }
        | TOKEN_min '(' vexp ',' vexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' vexp ',' vexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' vexp ')'             { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_grad '(' exp ')'                  { $$ = new Foam::volVectorField(Foam::fvc::grad(*$3)); delete $3; }
        | TOKEN_reconstruct '(' fsexp ')'         { $$ = new Foam::volVectorField(Foam::fvc::reconstruct(*$3)); delete $3; }
        | TOKEN_curl '(' vexp ')'                 { $$ = new Foam::volVectorField(Foam::fvc::curl(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ',' vexp ')'        { $$ = new Foam::volVectorField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_VID                               { $$=driver.getField<Foam::volVectorField>(*$1); }
;

fsexp:  TOKEN_surf '(' scalar ')'           { $$ = driver.makeConstantField<Foam::surfaceScalarField>($3); }
        | fsexp '+' fsexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 + *$3); delete $1; delete $3; }
        | fsexp '*' fsexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '&' fvexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 & *$3); delete $1; delete $3; }
        | fsexp '/' fsexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 / *$3); delete $1; delete $3; }
        | fsexp '-' fsexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 - *$3); delete $1; delete $3;}
        | TOKEN_pow '(' fsexp ',' scalar ')'{ $$ = new Foam::surfaceScalarField(Foam::pow(*$3, $5)); delete $3; }
        | TOKEN_log '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::log(*$3)); delete $3; }
        | TOKEN_exp '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::exp(*$3)); delete $3; }
        | TOKEN_sqr '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::sqr(*$3)); delete $3; }
        | TOKEN_sqrt '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::sqrt(*$3)); delete $3; }
        | TOKEN_sin '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::sin(*$3)); delete $3; }
        | TOKEN_cos '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::cos(*$3)); delete $3; }
        | TOKEN_tan '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::tan(*$3)); delete $3; }
        | TOKEN_log10 '(' fsexp ')'         { $$ = new Foam::surfaceScalarField(Foam::log10(*$3)); delete $3; }
        | TOKEN_asin '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::asin(*$3)); delete $3; }
        | TOKEN_acos '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::acos(*$3)); delete $3; }
        | TOKEN_atan '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::atan(*$3)); delete $3; }
        | TOKEN_sinh '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::sinh(*$3)); delete $3; }
        | TOKEN_cosh '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::cosh(*$3)); delete $3; }
        | TOKEN_tanh '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::tanh(*$3)); delete $3; }
        | TOKEN_asinh '(' fsexp ')'         { $$ = new Foam::surfaceScalarField(Foam::asinh(*$3)); delete $3; }
        | TOKEN_acosh '(' fsexp ')'         { $$ = new Foam::surfaceScalarField(Foam::acosh(*$3)); delete $3; }
        | TOKEN_atanh '(' fsexp ')'         { $$ = new Foam::surfaceScalarField(Foam::atanh(*$3)); delete $3; }
        | TOKEN_erf '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::erf(*$3)); delete $3; }
        | TOKEN_erfc '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::erfc(*$3)); delete $3; }
        | TOKEN_lgamma '(' fsexp ')'        { $$ = new Foam::surfaceScalarField(Foam::lgamma(*$3)); delete $3; }
        | TOKEN_besselJ0 '(' fsexp ')'      { $$ = new Foam::surfaceScalarField(Foam::j0(*$3)); delete $3; }
        | TOKEN_besselJ1 '(' fsexp ')'      { $$ = new Foam::surfaceScalarField(Foam::j1(*$3)); delete $3; }
        | TOKEN_besselY0 '(' fsexp ')'      { $$ = new Foam::surfaceScalarField(Foam::y0(*$3)); delete $3; }
        | TOKEN_besselY1 '(' fsexp ')'      { $$ = new Foam::surfaceScalarField(Foam::y1(*$3)); delete $3; }
        | TOKEN_sign '(' fsexp ')'          { $$ = new Foam::surfaceScalarField(Foam::sign(*$3)); delete $3; }
        | TOKEN_pos '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::pos(*$3)); delete $3; }
        | TOKEN_neg '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::neg(*$3)); delete $3; }
        | TOKEN_min '(' fsexp ',' fsexp  ')'           { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fsexp ',' fsexp  ')'           { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fsexp ')'           { $$ = driver.makeConstantField<Foam::surfaceScalarField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fsexp ')'           { $$ = driver.makeConstantField<Foam::surfaceScalarField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fsexp ')'           { $$ = driver.makeConstantField<Foam::surfaceScalarField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fsexp ')'       { $$ = driver.makeConstantField<Foam::surfaceScalarField>(Foam::average(*$3).value()); delete $3; }
        | '-' fsexp %prec TOKEN_NEG         { $$ = new Foam::surfaceScalarField(-*$2); delete $2; }
        | '(' fsexp ')'		            { $$ = $2; }  
        | fvexp '.' 'x'                     { $$ = new Foam::surfaceScalarField($1->component(0)); delete $1; }
        | fvexp '.' 'y'                     { $$ = new Foam::surfaceScalarField($1->component(1)); delete $1; }
        | fvexp '.' 'z'                     { $$ = new Foam::surfaceScalarField($1->component(2)); delete $1; }
        | flexp '?' fsexp ':' fsexp         { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::surfaceScalarField>(0.)); delete $1; delete $3; delete $5; }
        | TOKEN_mag '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fvexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_area '(' ')'                { $$ = driver.makeAreaField(); }
        | TOKEN_snGrad '(' exp ')'          { $$ = new Foam::surfaceScalarField(Foam::fvc::snGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' exp ')'     { $$ = new Foam::surfaceScalarField(Foam::fvc::interpolate(*$3)); delete $3; }
        | TOKEN_FSID                        { $$ = driver.getField<Foam::surfaceScalarField>(*$1); }
        | TOKEN_LOOKUP '(' fsexp ')'	    { $$ = driver.makeField<Foam::surfaceScalarField>(driver.getLookup(*$1,*$3)); delete $1; delete $3; }
;
;
 
fvexp:  fvector                            { $$ = $1; }
        | fvexp '+' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 + *$3); delete $1; delete $3; }
        | fsexp '*' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '*' fsexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '^' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 ^ *$3); delete $1; delete $3; }
        | fvexp '/' fsexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 / *$3); delete $1; delete $3; }
        | fvexp '-' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' fvexp %prec TOKEN_NEG 	   { $$ = new Foam::surfaceVectorField(-*$2); delete $2; }
        | '(' fvexp ')'		           { $$ = $2; }  
        | flexp '?' fvexp ':' fvexp        { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::surfaceVectorField>(Foam::vector::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_fposition '(' ')'          { $$ = driver.makeFacePositionField(); }
        | TOKEN_fprojection '(' ')'        { $$ = driver.makeFaceProjectionField(); }
        | TOKEN_face '(' ')'               { $$ = driver.makeFaceField(); }
        | TOKEN_snGrad '(' vexp ')'        { $$ = new Foam::surfaceVectorField(Foam::fvc::snGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' vexp ')'   { $$ = new Foam::surfaceVectorField(Foam::fvc::interpolate(*$3)); delete $3; }
        | TOKEN_min '(' fvexp ',' fvexp  ')'           { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fvexp ',' fvexp  ')'           { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fvexp ')'      { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FVID                       { $$ = driver.getField<Foam::surfaceVectorField>(*$1); }
;
 
scalar:	TOKEN_NUM		        { $$ = $1; }
        | '-' TOKEN_NUM         	{ $$ = -$2; } 
;

exp:    TOKEN_NUM                                  { $$ = driver.makeConstantField<Foam::volScalarField>($1); }
        | exp '+' exp 		                   { sameSize($1,$3); $$ = new Foam::volScalarField(*$1 + *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | exp '-' exp 		                   { sameSize($1,$3); $$ = new Foam::volScalarField(*$1 - *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | exp '*' exp 		                   { sameSize($1,$3); $$ = new Foam::volScalarField(*$1 * *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | exp '%' exp 		                   { sameSize($1,$3); $$ = driver.makeModuloField(*$1,*$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | exp '/' exp 		                   { sameSize($1,$3); $$ = new Foam::volScalarField(*$1 / *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | TOKEN_pow '(' exp ',' scalar ')'	   { $$ = new Foam::volScalarField(Foam::pow(*$3, $5)); delete $3; driver.setCalculatedPatches(*$$); }
        | TOKEN_log '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::log(*$3)); delete $3; driver.setCalculatedPatches(*$$); }
        | TOKEN_exp '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::exp(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | vexp '&' vexp 	                   { $$ = new Foam::volScalarField(*$1 & *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | '-' exp %prec TOKEN_NEG 	           { $$ = new Foam::volScalarField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); }
	| '(' exp ')'		                   { $$ = $2; }
        | TOKEN_sqr '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::sqr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_sqrt '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::sqrt(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_sin '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::sin(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_cos '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::cos(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_tan '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::tan(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_log10 '(' exp ')'                  { $$ = new Foam::volScalarField(Foam::log10(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_asin '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::asin(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_acos '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::acos(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_atan '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::atan(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_sinh '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::sinh(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_cosh '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::cosh(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_tanh '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::tanh(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_asinh '(' exp ')'                  { $$ = new Foam::volScalarField(Foam::asinh(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_acosh '(' exp ')'                  { $$ = new Foam::volScalarField(Foam::acosh(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_atanh '(' exp ')'                  { $$ = new Foam::volScalarField(Foam::atanh(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_erf '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::erf(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_erfc '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::erfc(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_lgamma '(' exp ')'                 { $$ = new Foam::volScalarField(Foam::lgamma(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_besselJ0 '(' exp ')'               { $$ = new Foam::volScalarField(Foam::j0(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_besselJ1 '(' exp ')'               { $$ = new Foam::volScalarField(Foam::j1(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_besselY0 '(' exp ')'               { $$ = new Foam::volScalarField(Foam::y0(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_besselY1 '(' exp ')'               { $$ = new Foam::volScalarField(Foam::y1(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_sign '(' exp ')'                   { $$ = new Foam::volScalarField(Foam::sign(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_pos '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::pos(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_neg '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::neg(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_min '(' exp ',' exp  ')'           { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_max '(' exp ',' exp  ')'           { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_min '(' exp ')'                    { $$ = driver.makeConstantField<Foam::volScalarField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' exp ')'                    { $$ = driver.makeConstantField<Foam::volScalarField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' exp ')'                    { $$ = driver.makeConstantField<Foam::volScalarField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' exp ')'                { $$ = driver.makeConstantField<Foam::volScalarField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_mag '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::mag(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqrGradGrad '(' exp ')'         { $$ = new Foam::volScalarField(Foam::fvc::magSqrGradGrad(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_mag '(' vexp ')'                   { $$ = new Foam::volScalarField(Foam::mag(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_div '(' vexp ')'                   { $$ = new Foam::volScalarField(Foam::fvc::div(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_div '(' fsexp ')'                  { $$ = new Foam::volScalarField(Foam::fvc::div(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_div '(' fsexp ',' exp ')'          { $$ = new Foam::volScalarField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_laplacian '(' exp ')'              { $$ = new Foam::volScalarField(Foam::fvc::laplacian(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_laplacian '(' exp ',' exp ')'      { $$ = new Foam::volScalarField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_laplacian '(' fsexp ',' exp ')'    { $$ = new Foam::volScalarField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_faceAverage '(' fsexp ')'          { $$ = new Foam::volScalarField(Foam::fvc::average(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_integrate '(' fsexp ')'            { $$ = new Foam::volScalarField(Foam::fvc::surfaceIntegrate(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_surfSum '(' fsexp ')'              { $$ = new Foam::volScalarField(Foam::fvc::surfaceSum(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | vexp '.' 'x'                             { $$ = new Foam::volScalarField($1->component(0)); delete $1;  driver.setCalculatedPatches(*$$); }
        | vexp '.' 'y'                             { $$ = new Foam::volScalarField($1->component(1)); delete $1;  driver.setCalculatedPatches(*$$); }
        | vexp '.' 'z'                             { $$ = new Foam::volScalarField($1->component(2)); delete $1;  driver.setCalculatedPatches(*$$); }
        | lexp '?' exp ':' exp                     { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::volScalarField>(0.)); delete $1; delete $3; delete $5; }
        | TOKEN_pi                                 { $$ = driver.makeConstantField<Foam::volScalarField>(Foam::constant::mathematical::pi); }
        | TOKEN_dist '(' ')'                       { $$ = driver.makeDistanceField(); }
        | TOKEN_nearDist '(' ')'                   { $$ = driver.makeNearDistanceField(); }
        | TOKEN_rdist '(' vexp ')'                 { $$ = driver.makeRDistanceField(*$3); delete $3; }
        | TOKEN_volume '(' ')'                     { $$ = driver.makeVolumeField(); }
        | TOKEN_rand '(' ')'                       { $$ = driver.makeRandomField(); }
        | TOKEN_rand '(' TOKEN_INT ')'                       { $$ = driver.makeRandomField(-$3); }
        | TOKEN_randNormal '(' ')'                 { $$ = driver.makeGaussRandomField(); }
        | TOKEN_randNormal '(' TOKEN_INT ')'                 { $$ = driver.makeGaussRandomField(-$3); }
        | TOKEN_randFixed '(' ')'                       { $$ = driver.makeRandomField(1); }
        | TOKEN_randFixed '(' TOKEN_INT ')'                       { $$ = driver.makeRandomField($3+1); }
        | TOKEN_randNormalFixed '(' ')'                 { $$ = driver.makeGaussRandomField(1); }
        | TOKEN_randNormalFixed '(' TOKEN_INT ')'                 { $$ = driver.makeGaussRandomField($3+1); }
        | TOKEN_id '(' ')'                         { $$ = driver.makeCellIdField(); }
        | TOKEN_cpu'(' ')'                         { $$ = driver.makeConstantField<Foam::volScalarField>(Foam::Pstream::myProcNo()); }
        | TOKEN_deltaT '(' ')'                     { $$ = driver.makeConstantField<Foam::volScalarField>(driver.runTime().deltaT().value()); }
        | TOKEN_time '(' ')'                       { $$ = driver.makeConstantField<Foam::volScalarField>(driver.runTime().time().value()); }
        | TOKEN_SID		                   { $$ = driver.getField<Foam::volScalarField>(*$1); }
        | TOKEN_LINE            		   { $$ = driver.makeConstantField<Foam::volScalarField>(driver.getLineValue(*$1,driver.runTime().time().value())); delete $1; }
        | TOKEN_LOOKUP '(' exp ')'		   { $$ = driver.makeField<Foam::volScalarField>(driver.getLookup(*$1,*$3)); delete $1; delete$3; }
;

lexp: TOKEN_TRUE                       { $$ = driver.makeConstantField<Foam::volScalarField>(1); }
    | TOKEN_FALSE                      { $$ = driver.makeConstantField<Foam::volScalarField>(0); }
    | TOKEN_set '(' TOKEN_SETID ')'    { $$ = driver.makeCellSetField(*$3); }
    | TOKEN_zone '(' TOKEN_ZONEID ')'  { $$ = driver.makeCellZoneField(*$3); }
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

flexp: TOKEN_surf '(' TOKEN_TRUE ')'  { $$ = driver.makeConstantField<Foam::surfaceScalarField>(1); }
    | TOKEN_surf '(' TOKEN_FALSE ')'  { $$ = driver.makeConstantField<Foam::surfaceScalarField>(0); }
    | TOKEN_fset '(' TOKEN_FSETID ')'    { $$ = driver.makeFaceSetField(*$3); }
    | TOKEN_fzone '(' TOKEN_FZONEID ')'  { $$ = driver.makeFaceZoneField(*$3); }
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

vector: TOKEN_VECTOR '(' exp ',' exp ',' exp ')' {     $$ = driver.makeVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

fvector: TOKEN_VECTOR '(' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeSurfaceVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

%%

void parserField::FieldValueExpressionParser::error (const parserField::FieldValueExpressionParser::location_type& l,const std::string& m)
{
     driver.error (l, m);
}
