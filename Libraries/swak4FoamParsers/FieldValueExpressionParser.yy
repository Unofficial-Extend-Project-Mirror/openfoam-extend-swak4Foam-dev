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
#include <fvcDdt.H>
#include <fvcD2dt2.H>
#include <fvcMeshPhi.H>
#include <fvcFlux.H>

#include <fvMatrix.H>

#include <functional>
#include <cmath>

    namespace Foam {
        class FieldValueExpressionDriver;
    }

    using Foam::FieldValueExpressionDriver;

#include "foamVersion4swak.H"

#if FOAM_VERSION4SWAK_MAJOR<2 &&  FOAM_VERSION4SWAK_MINOR<7
#define INCOMPLETE_OPERATORS
#endif

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
    Foam::tensor *ten;
    Foam::symmTensor *yten;
    Foam::sphericalTensor *hten;
    Foam::string *name;
    Foam::volVectorField *vfield;
    Foam::volScalarField *sfield;
    Foam::volTensorField *tfield;
    Foam::volSymmTensorField *yfield;
    Foam::volSphericalTensorField *hfield;
    Foam::surfaceScalarField *fsfield;
    Foam::surfaceVectorField *fvfield;
    Foam::surfaceTensorField *ftfield;
    Foam::surfaceSymmTensorField *fyfield;
    Foam::surfaceSphericalTensorField *fhfield;
    Foam::pointScalarField *psfield;
    Foam::pointVectorField *pvfield;
    Foam::pointTensorField *ptfield;
    Foam::pointSymmTensorField *pyfield;
    Foam::pointSphericalTensorField *phfield;
};

%{
#include "FieldValueExpressionDriverYY.H"
#include "FieldValueExpressionDriverLogicalTemplates.H"

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
%token <name> TOKEN_PSID  "pointScalarID"
%token <name> TOKEN_PVID  "pointVectorID"
%token <name>  TOKEN_PTID   "pointTensorID"
%token <name>  TOKEN_PYID   "pointSymmTensorID"
%token <name>  TOKEN_PHID   "pointSphericalTensorID"
%token <name> TOKEN_SETID "cellSetID" 
%token <name> TOKEN_ZONEID "cellZoneID" 
%token <name> TOKEN_FSETID "faceSetID" 
%token <name> TOKEN_FZONEID "faceZoneID" 
%token <name> TOKEN_PSETID "pointSetID" 
%token <name> TOKEN_PZONEID "pointZoneID" 
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
%type  <psfield>   plexp       "plexpression"
%type  <vfield>    vexp       "vexpression"
%type  <fsfield>   fsexp      "fsexpression"
%type  <fvfield>   fvexp      "fvexpression"
%type  <psfield>   psexp      "psexpression"
%type  <pvfield>   pvexp      "pvexpression"
%type  <tfield>    texp       "texpression"
%type  <yfield>    yexp       "yexpression"
%type  <hfield>    hexp       "hexpression"
%type  <ftfield>    ftexp       "ftexpression"
%type  <fyfield>    fyexp       "fyexpression"
%type  <fhfield>    fhexp       "fhexpression"
%type  <ptfield>    ptexp       "ptexpression"
%type  <pyfield>    pyexp       "pyexpression"
%type  <phfield>    phexp       "phexpression"
%type  <vfield>    vector     
%type  <fvfield>    fvector     
%type  <tfield>    tensor     
%type  <yfield>    symmTensor     
%type  <hfield>    sphericalTensor     
%type  <ftfield>    ftensor     
%type  <fyfield>    fsymmTensor     
%type  <fhfield>    fsphericalTensor     
%type  <pvfield>    pvector     
%type  <ptfield>    ptensor     
%type  <pyfield>    psymmTensor     
%type  <phfield>    psphericalTensor     

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
%token TOKEN_pposition
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
%token TOKEN_pset
%token TOKEN_pzone

%token TOKEN_div
%token TOKEN_grad
%token TOKEN_curl
%token TOKEN_snGrad
%token TOKEN_magSqrGradGrad;
%token TOKEN_laplacian
%token TOKEN_ddt
%token TOKEN_oldTime
%token TOKEN_d2dt2
%token TOKEN_meshPhi
%token TOKEN_flux

%token TOKEN_integrate
%token TOKEN_surfSum
%token TOKEN_interpolate
%token TOKEN_interpolateToPoint
%token TOKEN_interpolateToCell
%token TOKEN_faceAverage
%token TOKEN_reconstruct

%token TOKEN_surf
%token TOKEN_point

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

%printer             { debug_stream () << *$$; } "scalarID" "vectorID" "faceScalarID" "faceVectorID" "cellSetID" "cellZoneID" "faceSetID" "faceZoneID" "pointSetID" "pointZoneID" "tensorID" "symmTensorID" "sphericalTensorID" "faceTensorID" "faceSymmTensorID" "faceSphericalTensorID" "pointScalarID" "pointVectorID" "pointTensorID" "pointSymmTensorID" "pointSphericalTensorID"
%printer             { Foam::OStringStream buff; buff << *$$; debug_stream () << buff.str().c_str(); } "vector" "tensor" "symmTensor" "sphericalTensor"
%destructor          { delete $$; } "timeline" "lookup" "scalarID" "faceScalarID" "faceVectorID" "vectorID" "vector" "tensor" "symmTensor" "sphericalTensor" "expression" "vexpression" "fsexpression" "fvexpression" "lexpression" "flexpression" "texpression" "yexpression" "hexpression" "ftexpression" "fyexpression" "fhexpression" "cellSetID"  "cellZoneID"  "faceSetID"  "faceZoneID" "pointSetID" "pointZoneID" "tensorID" "symmTensorID" "sphericalTensorID" "faceTensorID" "faceSymmTensorID" "faceSphericalTensorID" "pointScalarID" "pointVectorID" "pointTensorID" "pointSymmTensorID" "pointSphericalTensorID"
%printer             { debug_stream () << $$; } "number" "integer" "sexpression"
%printer             { debug_stream () << $$->name().c_str(); } "expression"  "vexpression" "lexpression" "flexpression" "fsexpression" "fvexpression" "texpression" "yexpression" "hexpression" "ftexpression" "fyexpression" "fhexpression" "plexpression" "psexpression" "pvexpression" "ptexpression" "pyexpression" "phexpression"


%%
%start unit;

unit:   exp                     { driver.setResult($1,false,false);  }
        | vexp                  { driver.setResult($1,false,false);  }
        | texp                  { driver.setResult($1,false,false);  }
        | yexp                  { driver.setResult($1,false,false);  }
        | hexp                  { driver.setResult($1,false,false);  }
        | lexp                  { driver.setLogicalResult($1,false,false); }
        | fsexp                 { driver.setResult($1,true,false);  }
        | fvexp                 { driver.setResult($1,true,false);  }
        | ftexp                 { driver.setResult($1,true,false);  }
        | fyexp                 { driver.setResult($1,true,false);  }
        | fhexp                 { driver.setResult($1,true,false);  }
        | flexp                 { driver.setLogicalResult($1,true,false); }
        | psexp                 { driver.setResult($1,false,true);  }
        | pvexp                 { driver.setResult($1,false,true);  }
        | ptexp                 { driver.setResult($1,false,true);  }
        | pyexp                 { driver.setResult($1,false,true);  }
        | phexp                 { driver.setResult($1,false,true);  }
        | plexp                 { driver.setLogicalResult($1,false,true); }
;

vectorComponentSwitch: /* empty rule */{ driver.startVectorComponent(); } 
;

tensorComponentSwitch: /* empty rule */{ driver.startTensorComponent(); } 
;


vexp:   vector                                    { $$ = $1; }
        | vexp '+' vexp 		          { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' vexp 		                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 * *$3); delete $1; delete $3; }
        | vexp '*' exp 		                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 * *$3); delete $1; delete $3; }
        | texp '&' vexp 	                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' texp 	                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 & *$3); delete $1; delete $3; }
        | yexp '&' vexp 	                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' yexp 	                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 & *$3); delete $1; delete $3; }
        | hexp '&' vexp 	                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' hexp 	                  { sameSize($1,$3); $$ = new Foam::volVectorField(*$1 & *$3); delete $1; delete $3; }
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
        | TOKEN_interpolateToCell '(' pvexp ')'              { $$ = driver.pointToCellInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' vexp ',' vexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' vexp ',' vexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_minPosition '(' exp ')'           { 
            $$ = driver.makeConstantField<Foam::volVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.mesh().C()
                )
            ); 
            delete $3; 
        }
        | TOKEN_max '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_maxPosition '(' exp ')'           { 
            $$ = driver.makeConstantField<Foam::volVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.mesh().C()
                )
            ); 
            delete $3; 
        }
        | TOKEN_sum '(' vexp ')'                 { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' vexp ')'             { $$ = driver.makeConstantField<Foam::volVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_grad '(' exp ')'                  { $$ = new Foam::volVectorField(Foam::fvc::grad(*$3)); delete $3; }
        | TOKEN_reconstruct '(' fsexp ')'         { $$ = new Foam::volVectorField(Foam::fvc::reconstruct(*$3)); delete $3; }
        | TOKEN_curl '(' vexp ')'                 { $$ = new Foam::volVectorField(Foam::fvc::curl(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ',' vexp ')'        { $$ = new Foam::volVectorField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_VID                               { $$=driver.getField<Foam::volVectorField>(*$1); }
        | TOKEN_ddt '(' TOKEN_VID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::volVectorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_d2dt2 '(' TOKEN_VID ')'		   { $$ = Foam::fvc::d2dt2( driver.getOrReadField<Foam::volVectorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_VID ')'		   { $$ = new Foam::volVectorField( driver.getOrReadField<Foam::volVectorField>(*$3,true,true)->oldTime()); }
;

fsexp:  TOKEN_surf '(' scalar ')'           { $$ = driver.makeConstantField<Foam::surfaceScalarField>($3); }
        | fsexp '+' fsexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 + *$3); delete $1; delete $3; }
        | fsexp '*' fsexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '&' fvexp 		    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 & *$3); delete $1; delete $3; }
        | ftexp TOKEN_AND ftexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | fyexp TOKEN_AND ftexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | fhexp TOKEN_AND ftexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | ftexp TOKEN_AND fyexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | fyexp TOKEN_AND fyexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | fhexp TOKEN_AND fyexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | ftexp TOKEN_AND fhexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | fyexp TOKEN_AND fhexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
        | fhexp TOKEN_AND fhexp 	    { sameSize($1,$3); $$ = new Foam::surfaceScalarField(*$1 && *$3); delete $1; delete $3; }
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
        | fvexp '.' vectorComponentSwitch TOKEN_x                     { $$ = new Foam::surfaceScalarField($1->component(0)); delete $1; }
        | fvexp '.' vectorComponentSwitch TOKEN_y                     { $$ = new Foam::surfaceScalarField($1->component(1)); delete $1; }
        | fvexp '.' vectorComponentSwitch TOKEN_z                     { $$ = new Foam::surfaceScalarField($1->component(2)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_xx                { $$ = new Foam::surfaceScalarField($1->component(0)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_xy                { $$ = new Foam::surfaceScalarField($1->component(1)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_xz                { $$ = new Foam::surfaceScalarField($1->component(2)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_yx                { $$ = new Foam::surfaceScalarField($1->component(3)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_yy                { $$ = new Foam::surfaceScalarField($1->component(4)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_yz                { $$ = new Foam::surfaceScalarField($1->component(5)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_zx                { $$ = new Foam::surfaceScalarField($1->component(6)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_zy                { $$ = new Foam::surfaceScalarField($1->component(7)); delete $1; }
        | ftexp '.' tensorComponentSwitch TOKEN_zz                { $$ = new Foam::surfaceScalarField($1->component(8)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_xx                { $$ = new Foam::surfaceScalarField($1->component(0)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_xy                { $$ = new Foam::surfaceScalarField($1->component(1)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_xz                { $$ = new Foam::surfaceScalarField($1->component(2)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_yy                { $$ = new Foam::surfaceScalarField($1->component(3)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_yz                { $$ = new Foam::surfaceScalarField($1->component(4)); delete $1; }
        | fyexp '.' tensorComponentSwitch TOKEN_zz                { $$ = new Foam::surfaceScalarField($1->component(5)); delete $1; }
        | fhexp '.' tensorComponentSwitch TOKEN_ii                { $$ = new Foam::surfaceScalarField($1->component(0)); delete $1; }
        | flexp '?' fsexp ':' fsexp         { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::surfaceScalarField>(0.)); delete $1; delete $3; delete $5; }
        | TOKEN_mag '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fvexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' ftexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fyexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' fhexp ')'           { $$ = new Foam::surfaceScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_magSqr '(' fsexp ')'           { $$ = new Foam::surfaceScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' fvexp ')'           { $$ = new Foam::surfaceScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' ftexp ')'           { $$ = new Foam::surfaceScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' fyexp ')'           { $$ = new Foam::surfaceScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' fhexp ')'           { $$ = new Foam::surfaceScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_tr '(' ftexp ')'            { $$ = new Foam::surfaceScalarField(Foam::tr(*$3)); delete $3; }
        | TOKEN_tr '(' fyexp ')'            { $$ = new Foam::surfaceScalarField(Foam::tr(*$3)); delete $3; }
// doesn't work directly        | TOKEN_tr '(' fhexp ')'            {$$ = new Foam::surfaceScalarField(Foam::tr(*$3)); delete $3; }  
        | TOKEN_tr '(' fhexp ')'            { 
            $$ = driver.makeField<Foam::surfaceScalarField>(Foam::tr($3->internalField())); 
            delete $3; }
        | TOKEN_det '(' ftexp ')'           { $$ = new Foam::surfaceScalarField(Foam::det(*$3)); delete $3; }
        | TOKEN_det '(' fyexp ')'           { $$ = new Foam::surfaceScalarField(Foam::det(*$3)); delete $3; }
        | TOKEN_det '(' fhexp ')'           { 
            $$ = driver.makeField<Foam::surfaceScalarField>(Foam::det($3->internalField())); 
            delete $3; }
        | TOKEN_area '(' ')'                { $$ = driver.makeAreaField(); }
        | TOKEN_snGrad '(' exp ')'          { $$ = new Foam::surfaceScalarField(Foam::fvc::snGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' exp ')'     { $$ = new Foam::surfaceScalarField(Foam::fvc::interpolate(*$3)); delete $3; }
        | TOKEN_FSID                        { $$ = driver.getField<Foam::surfaceScalarField>(*$1); }
//        | TOKEN_ddt '(' TOKEN_FSID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::surfaceScalarField>(*$3,true,true)() ).ptr(); } // no fvc::ddt for surface Fields
        | TOKEN_oldTime '(' TOKEN_FSID ')'		   { $$ = new Foam::surfaceScalarField( driver.getOrReadField<Foam::surfaceScalarField>(*$3,true,true)->oldTime()); }
        | TOKEN_meshPhi '(' vexp ')'      { $$ = Foam::fvc::meshPhi(*$3).ptr(); delete $3; $$->dimensions().reset(Foam::dimless); }
        | TOKEN_meshPhi '(' exp ',' vexp ')'      { $$ = Foam::fvc::meshPhi(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_flux '(' fsexp ',' exp ')'       { $$ = Foam::fvc::flux(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_LOOKUP '(' fsexp ')'	    { $$ = driver.makeField<Foam::surfaceScalarField>(driver.getLookup(*$1,*$3)); delete $1; delete $3; }
;

fvexp:  fvector                            { $$ = $1; }
        | fvexp '+' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 + *$3); delete $1; delete $3; }
        | fsexp '*' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 * *$3); delete $1; delete $3; }
        | fvexp '*' fsexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 * *$3); delete $1; delete $3; }
        | ftexp '&' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '&' ftexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 & *$3); delete $1; delete $3; }
        | fhexp '&' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '&' fhexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 & *$3); delete $1; delete $3; }
        | fyexp '&' fvexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 & *$3); delete $1; delete $3; }
        | fvexp '&' fyexp 		   { sameSize($1,$3); $$ = new Foam::surfaceVectorField(*$1 & *$3); delete $1; delete $3; }
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
        | TOKEN_minPosition '(' fsexp ')'           { 
            $$ = driver.makeConstantField<Foam::surfaceVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.mesh().Cf()
                )
            ); 
            delete $3; 
        }
        | TOKEN_max '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_maxPosition '(' fsexp ')'           { 
            $$ = driver.makeConstantField<Foam::surfaceVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.mesh().Cf()
                )
            ); 
            delete $3; 
        }
        | TOKEN_sum '(' fvexp ')'          { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fvexp ')'      { $$ = driver.makeConstantField<Foam::surfaceVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FVID                       { $$ = driver.getField<Foam::surfaceVectorField>(*$1); }
//        | TOKEN_ddt '(' TOKEN_FVID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::surfaceVectorField>(*$3,true,true)() ).ptr(); }// no fvc::ddt for surface Fields
        | TOKEN_oldTime '(' TOKEN_FVID ')'		   { $$ = new Foam::surfaceVectorField( driver.getOrReadField<Foam::surfaceVectorField>(*$3,true,true)->oldTime()); }
        | TOKEN_flux '(' fsexp ',' vexp ')'       { $$ = Foam::fvc::flux(*$3,*$5).ptr(); delete $3; delete $5; }
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
        | texp TOKEN_AND texp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | hexp TOKEN_AND texp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | yexp TOKEN_AND texp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | texp TOKEN_AND yexp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | hexp TOKEN_AND yexp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | yexp TOKEN_AND yexp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | texp TOKEN_AND hexp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | hexp TOKEN_AND hexp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
        | yexp TOKEN_AND hexp 	                   { $$ = new Foam::volScalarField(*$1 && *$3); delete $1; delete $3; driver.setCalculatedPatches(*$$); }
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
        | TOKEN_mag '(' vexp ')'                   { $$ = new Foam::volScalarField(Foam::mag(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_mag '(' texp ')'                   { $$ = new Foam::volScalarField(Foam::mag(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_mag '(' yexp ')'                   { $$ = new Foam::volScalarField(Foam::mag(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_mag '(' hexp ')'                   { $$ = new Foam::volScalarField(Foam::mag(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqr '(' exp ')'                    { $$ = new Foam::volScalarField(Foam::magSqr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqr '(' vexp ')'                   { $$ = new Foam::volScalarField(Foam::magSqr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqr '(' texp ')'                   { $$ = new Foam::volScalarField(Foam::magSqr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqr '(' yexp ')'                   { $$ = new Foam::volScalarField(Foam::magSqr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqr '(' hexp ')'                   { $$ = new Foam::volScalarField(Foam::magSqr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_tr '(' texp ')'                    { $$ = new Foam::volScalarField(Foam::tr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_tr '(' yexp ')'                    { $$ = new Foam::volScalarField(Foam::tr(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_tr '(' hexp ')'                    { 
            $$ = driver.makeField<Foam::volScalarField>(Foam::tr($3->internalField())); 
            delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_det '(' texp ')'                   { $$ = new Foam::volScalarField(Foam::det(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_det '(' yexp ')'                   { $$ = new Foam::volScalarField(Foam::det(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_det '(' hexp ')'                   { 
            $$ = driver.makeField<Foam::volScalarField>(Foam::det($3->internalField())); 
            delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_magSqrGradGrad '(' exp ')'         { $$ = new Foam::volScalarField(Foam::fvc::magSqrGradGrad(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_div '(' vexp ')'                   { $$ = new Foam::volScalarField(Foam::fvc::div(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_div '(' fsexp ')'                  { $$ = new Foam::volScalarField(Foam::fvc::div(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_div '(' fsexp ',' exp ')'          { $$ = new Foam::volScalarField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_laplacian '(' exp ')'              { $$ = new Foam::volScalarField(Foam::fvc::laplacian(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_laplacian '(' exp ',' exp ')'      { $$ = new Foam::volScalarField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_laplacian '(' fsexp ',' exp ')'    { $$ = new Foam::volScalarField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5;  driver.setCalculatedPatches(*$$); }
        | TOKEN_faceAverage '(' fsexp ')'          { $$ = new Foam::volScalarField(Foam::fvc::average(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_integrate '(' fsexp ')'            { $$ = new Foam::volScalarField(Foam::fvc::surfaceIntegrate(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_surfSum '(' fsexp ')'              { $$ = new Foam::volScalarField(Foam::fvc::surfaceSum(*$3)); delete $3;  driver.setCalculatedPatches(*$$); }
        | TOKEN_interpolateToCell '(' psexp ')'              { $$ = driver.pointToCellInterpolate(*$3).ptr(); delete $3; }
        | vexp '.' vectorComponentSwitch  TOKEN_x                             { $$ = new Foam::volScalarField($1->component(0)); delete $1;  driver.setCalculatedPatches(*$$); }
        | vexp '.' vectorComponentSwitch  TOKEN_y                             { $$ = new Foam::volScalarField($1->component(1)); delete $1;  driver.setCalculatedPatches(*$$); }
        | vexp '.' vectorComponentSwitch  TOKEN_z                             { $$ = new Foam::volScalarField($1->component(2)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_xx                        { $$ = new Foam::volScalarField($1->component(0)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_xy                        { $$ = new Foam::volScalarField($1->component(1)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_xz                        { $$ = new Foam::volScalarField($1->component(2)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_yx                        { $$ = new Foam::volScalarField($1->component(3)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_yy                        { $$ = new Foam::volScalarField($1->component(4)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_yz                        { $$ = new Foam::volScalarField($1->component(5)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_zx                        { $$ = new Foam::volScalarField($1->component(6)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_zy                        { $$ = new Foam::volScalarField($1->component(7)); delete $1;  driver.setCalculatedPatches(*$$); }
        | texp '.' tensorComponentSwitch TOKEN_zz                        { $$ = new Foam::volScalarField($1->component(8)); delete $1;  driver.setCalculatedPatches(*$$); }
        | yexp '.' tensorComponentSwitch TOKEN_xx                        { $$ = new Foam::volScalarField($1->component(0)); delete $1;  driver.setCalculatedPatches(*$$); }
        | yexp '.' tensorComponentSwitch TOKEN_xy                        { $$ = new Foam::volScalarField($1->component(1)); delete $1;  driver.setCalculatedPatches(*$$); }
        | yexp '.' tensorComponentSwitch TOKEN_xz                        { $$ = new Foam::volScalarField($1->component(2)); delete $1;  driver.setCalculatedPatches(*$$); }
        | yexp '.' tensorComponentSwitch TOKEN_yy                        { $$ = new Foam::volScalarField($1->component(3)); delete $1;  driver.setCalculatedPatches(*$$); }
        | yexp '.' tensorComponentSwitch TOKEN_yz                        { $$ = new Foam::volScalarField($1->component(4)); delete $1;  driver.setCalculatedPatches(*$$); }
        | yexp '.' tensorComponentSwitch TOKEN_zz                        { $$ = new Foam::volScalarField($1->component(5)); delete $1;  driver.setCalculatedPatches(*$$); }
        | hexp '.' tensorComponentSwitch TOKEN_ii                        { $$ = new Foam::volScalarField($1->component(0)); delete $1;  driver.setCalculatedPatches(*$$); }
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
        | TOKEN_ddt '(' TOKEN_SID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::volScalarField>(*$3,true,true)() ).ptr(); }
        | TOKEN_d2dt2 '(' TOKEN_SID ')'		   { $$ = Foam::fvc::d2dt2( driver.getOrReadField<Foam::volScalarField>(*$3,true,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_SID ')'	   { $$ = new Foam::volScalarField( driver.getOrReadField<Foam::volScalarField>(*$3,true,true)->oldTime()); }
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

texp:   tensor                  { $$ = $1; }
        | texp '+' texp 		          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '+' yexp 		          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '+' hexp 		          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '+' texp 		          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '+' texp 		          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | exp '*' texp 	   	                  { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '*' exp 	   	                  { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | vexp '*' vexp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '&' texp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '&' texp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '&' yexp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '&' texp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '&' hexp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '/' exp 	   	                  { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '-' texp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '-' yexp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | texp '-' hexp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '-' texp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '-' texp 	   	          { sameSize($1,$3); $$ = new Foam::volTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' texp %prec TOKEN_NEG 	           { $$ = new Foam::volTensorField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); } 
        | '(' texp ')'		        { $$ = $2; }  
        | TOKEN_skew '(' texp ')' 	           { $$ = new Foam::volTensorField( Foam::skew(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_inv '(' texp ')' 	           { $$ = new Foam::volTensorField( Foam::inv(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_dev '(' texp ')' 	           { $$ = new Foam::volTensorField( Foam::dev(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | lexp '?' texp ':' texp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::volTensorField>(Foam::tensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_laplacian '(' fsexp ',' texp ')'  { $$ = new Foam::volTensorField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' ftexp ')'         { $$ = new Foam::volTensorField(Foam::fvc::average(*$3)); delete $3; }
        | TOKEN_integrate '(' ftexp ')'           { $$ = new Foam::volTensorField(Foam::fvc::surfaceIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' ftexp ')'             { $$ = new Foam::volTensorField(Foam::fvc::surfaceSum(*$3)); delete $3; }
        | TOKEN_interpolateToCell '(' ptexp ')'              { $$ = driver.pointToCellInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' texp ',' texp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' texp ',' texp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' texp ')'                 { $$ = driver.makeConstantField<Foam::volTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' texp ')'                 { $$ = driver.makeConstantField<Foam::volTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' texp ')'                 { $$ = driver.makeConstantField<Foam::volTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' texp ')'             { $$ = driver.makeConstantField<Foam::volTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_grad '(' vexp ')'                 { $$ = new Foam::volTensorField(Foam::fvc::grad(*$3)); delete $3; }
        | TOKEN_div '(' fsexp ',' texp ')'        { $$ = new Foam::volTensorField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_TID                               { $$=driver.getField<Foam::volTensorField>(*$1); }
        | TOKEN_ddt '(' TOKEN_TID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::volTensorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_d2dt2 '(' TOKEN_TID ')'		   { $$ = Foam::fvc::d2dt2( driver.getOrReadField<Foam::volTensorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_TID ')'		   { $$ = new Foam::volTensorField( driver.getOrReadField<Foam::volTensorField>(*$3,true,true)->oldTime()); }
;

yexp:   symmTensor                  { $$ = $1; }
        | yexp '+' yexp 		          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '+' yexp 		          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '+' hexp 		          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | exp '*' yexp 	   	                  { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '*' exp 	   	                  { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
// This worked before gcc4.6 (or the Problem is OF 2.1)
//        | yexp '&' yexp 	   	          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '&' yexp 	   	          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '&' hexp 	   	          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '/' exp 	   	                  { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '-' yexp 	   	          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '-' yexp 	   	          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | yexp '-' hexp 	   	          { sameSize($1,$3); $$ = new Foam::volSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' yexp %prec TOKEN_NEG 	           { $$ = new Foam::volSymmTensorField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); } 
        | '(' yexp ')'		        { $$ = $2; }  
        | TOKEN_symm '(' texp ')' 	           { $$ = new Foam::volSymmTensorField( Foam::symm(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_symm '(' yexp ')' 	           { $$ = new Foam::volSymmTensorField( Foam::symm(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_inv '(' yexp ')' 	           { $$ = new Foam::volSymmTensorField( Foam::inv(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_dev '(' yexp ')' 	           { $$ = new Foam::volSymmTensorField( Foam::dev(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | lexp '?' yexp ':' yexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::volSymmTensorField>(Foam::symmTensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_laplacian '(' fsexp ',' yexp ')'  { $$ = new Foam::volSymmTensorField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fyexp ')'         { $$ = new Foam::volSymmTensorField(Foam::fvc::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fyexp ')'           { $$ = new Foam::volSymmTensorField(Foam::fvc::surfaceIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fyexp ')'             { $$ = new Foam::volSymmTensorField(Foam::fvc::surfaceSum(*$3)); delete $3; }
        | TOKEN_interpolateToCell '(' pyexp ')'              { $$ = driver.pointToCellInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' yexp ',' yexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' yexp ',' yexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' yexp ')'                 { $$ = driver.makeConstantField<Foam::volSymmTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' yexp ')'                 { $$ = driver.makeConstantField<Foam::volSymmTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' yexp ')'                 { $$ = driver.makeConstantField<Foam::volSymmTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' yexp ')'             { $$ = driver.makeConstantField<Foam::volSymmTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_div '(' fsexp ',' yexp ')'        { $$ = new Foam::volSymmTensorField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_YID                               { $$=driver.getField<Foam::volSymmTensorField>(*$1); }
        | TOKEN_ddt '(' TOKEN_YID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::volSymmTensorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_d2dt2 '(' TOKEN_YID ')'		   { $$ = Foam::fvc::d2dt2( driver.getOrReadField<Foam::volSymmTensorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_YID ')'	   { $$ = new Foam::volSymmTensorField( driver.getOrReadField<Foam::volSymmTensorField>(*$3,true,true)->oldTime()); }
;

hexp:   sphericalTensor                  { $$ = $1; }
        | hexp '+' hexp 		          { sameSize($1,$3); $$ = new Foam::volSphericalTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | exp '*' hexp 	   	                  { sameSize($1,$3); $$ = new Foam::volSphericalTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '*' exp 	   	                  { sameSize($1,$3); $$ = new Foam::volSphericalTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '&' hexp 	   	          { sameSize($1,$3); $$ = new Foam::volSphericalTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '/' exp 	   	                  { sameSize($1,$3); $$ = new Foam::volSphericalTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | hexp '-' hexp 	   	          { sameSize($1,$3); $$ = new Foam::volSphericalTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' hexp %prec TOKEN_NEG 	           { $$ = new Foam::volSphericalTensorField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); } 
        | '(' hexp ')'		        { $$ = $2; }  
        | TOKEN_inv '(' hexp ')' 	           { 
            $$ = driver.makeField<Foam::volSphericalTensorField>( Foam::inv($3->internalField()) ); 
            delete $3;  driver.setCalculatedPatches(*$$); } 
        | lexp '?' hexp ':' hexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::volSphericalTensorField>(Foam::sphericalTensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_laplacian '(' fsexp ',' hexp ')'  { $$ = new Foam::volSphericalTensorField(Foam::fvc::laplacian(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_faceAverage '(' fhexp ')'         { $$ = new Foam::volSphericalTensorField(Foam::fvc::average(*$3)); delete $3; }
        | TOKEN_integrate '(' fhexp ')'           { $$ = new Foam::volSphericalTensorField(Foam::fvc::surfaceIntegrate(*$3)); delete $3; }
        | TOKEN_surfSum '(' fhexp ')'             { $$ = new Foam::volSphericalTensorField(Foam::fvc::surfaceSum(*$3)); delete $3; }
        | TOKEN_interpolateToCell '(' phexp ')'              { $$ = driver.pointToCellInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' hexp ',' hexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' hexp ',' hexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' hexp ')'                 { $$ = driver.makeConstantField<Foam::volSphericalTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' hexp ')'                 { $$ = driver.makeConstantField<Foam::volSphericalTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' hexp ')'                 { $$ = driver.makeConstantField<Foam::volSphericalTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' hexp ')'             { $$ = driver.makeConstantField<Foam::volSphericalTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_div '(' fsexp ',' hexp ')'        { $$ = new Foam::volSphericalTensorField(Foam::fvc::div(*$3,*$5)); delete $3; delete $5; }
        | TOKEN_HID                               { $$=driver.getField<Foam::volSphericalTensorField>(*$1); }
        | TOKEN_ddt '(' TOKEN_HID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::volSphericalTensorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_d2dt2 '(' TOKEN_HID ')'		   { $$ = Foam::fvc::d2dt2( driver.getOrReadField<Foam::volSphericalTensorField>(*$3,true,true)() ).ptr(); }
        | TOKEN_oldTime '(' TOKEN_HID ')'	   { $$ = new Foam::volSphericalTensorField( driver.getOrReadField<Foam::volSphericalTensorField>(*$3,true,true)->oldTime()); }
;

ftexp:   ftensor                  { $$ = $1; }
        | ftexp '+' ftexp 		          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '+' fyexp 		          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '+' fhexp 		          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '+' ftexp 		          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '+' ftexp 		          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fsexp '*' ftexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '*' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fvexp '*' fvexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '&' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '&' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '&' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '&' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '&' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '/' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '-' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '-' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ftexp '-' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '-' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '-' ftexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' ftexp %prec TOKEN_NEG 	           { $$ = new Foam::surfaceTensorField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); } 
        | '(' ftexp ')'		        { $$ = $2; }  
        | TOKEN_skew '(' ftexp ')' 	           { $$ = new Foam::surfaceTensorField( Foam::skew(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_inv '(' ftexp ')' 	           { $$ = new Foam::surfaceTensorField( Foam::inv(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_dev '(' ftexp ')' 	           { $$ = new Foam::surfaceTensorField( Foam::dev(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | flexp '?' ftexp ':' ftexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::surfaceTensorField>(Foam::tensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_snGrad '(' texp ')'           { $$ = new Foam::surfaceTensorField(Foam::fvc::snGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' texp ')'           { $$ = new Foam::surfaceTensorField(Foam::fvc::interpolate(*$3)); delete $3; }
        | TOKEN_min '(' ftexp ',' ftexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' ftexp ',' ftexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' ftexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' ftexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' ftexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' ftexp ')'             { $$ = driver.makeConstantField<Foam::surfaceTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FTID                               { $$=driver.getField<Foam::surfaceTensorField>(*$1); }
//        | TOKEN_ddt '(' TOKEN_FTID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::surfaceTensorField>(*$3,true,true)() ).ptr(); } // no fvc::ddt for surface Fields
        | TOKEN_oldTime '(' TOKEN_FTID ')'	   { $$ = new Foam::surfaceTensorField( driver.getOrReadField<Foam::surfaceTensorField>(*$3,true,true)->oldTime()); }
        | TOKEN_flux '(' fsexp ',' texp ')'       { $$ = Foam::fvc::flux(*$3,*$5).ptr(); delete $3; delete $5; }
;

fyexp:   fsymmTensor                  { $$ = $1; }
        | fyexp '+' fyexp 		          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '+' fyexp 		          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '+' fhexp 		          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fsexp '*' fyexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '*' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
// This worked before gcc4.6 (or the Problem is OF 2.1)
//        | fyexp '&' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '&' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '&' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '/' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '-' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '-' fyexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fyexp '-' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' fyexp %prec TOKEN_NEG 	           { $$ = new Foam::surfaceSymmTensorField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); } 
        | '(' fyexp ')'		        { $$ = $2; }  
        | TOKEN_symm '(' ftexp ')' 	           { $$ = new Foam::surfaceSymmTensorField( Foam::symm(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_symm '(' fyexp ')' 	           { $$ = new Foam::surfaceSymmTensorField( Foam::symm(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_inv '(' fyexp ')' 	           { $$ = new Foam::surfaceSymmTensorField( Foam::inv(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_dev '(' fyexp ')' 	           { $$ = new Foam::surfaceSymmTensorField( Foam::dev(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | flexp '?' fyexp ':' fyexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::surfaceSymmTensorField>(Foam::symmTensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_snGrad '(' yexp ')'           { $$ = new Foam::surfaceSymmTensorField(Foam::fvc::snGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' yexp ')'           { $$ = new Foam::surfaceSymmTensorField(Foam::fvc::interpolate(*$3)); delete $3; }
        | TOKEN_min '(' fyexp ',' fyexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fyexp ',' fyexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fyexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceSymmTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fyexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceSymmTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fyexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceSymmTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fyexp ')'             { $$ = driver.makeConstantField<Foam::surfaceSymmTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FYID                               { $$=driver.getField<Foam::surfaceSymmTensorField>(*$1); }
//        | TOKEN_ddt '(' TOKEN_FYID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::surfaceSymmTensorField>(*$3,true,true)() ).ptr(); }// no fvc::ddt for surface Fields
        | TOKEN_oldTime '(' TOKEN_FYID ')'	   { $$ = new Foam::surfaceSymmTensorField( driver.getOrReadField<Foam::surfaceSymmTensorField>(*$3,true,true)->oldTime()); }
        | TOKEN_flux '(' fsexp ',' yexp ')'       { $$ = Foam::fvc::flux(*$3,*$5).ptr(); delete $3; delete $5; }
;

fhexp:   fsphericalTensor                  { $$ = $1; }
        | fhexp '+' fhexp 		          { sameSize($1,$3); $$ = new Foam::surfaceSphericalTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fsexp '*' fhexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceSphericalTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '*' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceSphericalTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '&' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSphericalTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '/' fsexp 	   	                  { sameSize($1,$3); $$ = new Foam::surfaceSphericalTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | fhexp '-' fhexp 	   	          { sameSize($1,$3); $$ = new Foam::surfaceSphericalTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' fhexp %prec TOKEN_NEG 	           { $$ = new Foam::surfaceSphericalTensorField(-*$2); delete $2;  driver.setCalculatedPatches(*$$); } 
        | '(' fhexp ')'		        { $$ = $2; }  
        | TOKEN_inv '(' fhexp ')' 	           { 
            $$ = driver.makeField<Foam::surfaceSphericalTensorField>( Foam::inv($3->internalField()) ); 
            delete $3;  driver.setCalculatedPatches(*$$); } 
        | flexp '?' fhexp ':' fhexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makeConstantField<Foam::surfaceSphericalTensorField>(Foam::sphericalTensor::zero)); delete $1; delete $3; delete $5; }
         | TOKEN_snGrad '(' hexp ')'           { $$ = new Foam::surfaceSphericalTensorField(Foam::fvc::snGrad(*$3)); delete $3; }
        | TOKEN_interpolate '(' hexp ')'           { $$ = new Foam::surfaceSphericalTensorField(Foam::fvc::interpolate(*$3)); delete $3; }
       | TOKEN_min '(' fhexp ',' fhexp  ')'        { $$ = Foam::min(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_max '(' fhexp ',' fhexp  ')'        { $$ = Foam::max(*$3,*$5).ptr(); delete $3; delete $5; }
        | TOKEN_min '(' fhexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceSphericalTensorField>(Foam::min(*$3).value()); delete $3; }
        | TOKEN_max '(' fhexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceSphericalTensorField>(Foam::max(*$3).value()); delete $3; }
        | TOKEN_sum '(' fhexp ')'                 { $$ = driver.makeConstantField<Foam::surfaceSphericalTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' fhexp ')'             { $$ = driver.makeConstantField<Foam::surfaceSphericalTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_FHID                               { $$=driver.getField<Foam::surfaceSphericalTensorField>(*$1); }
//        | TOKEN_ddt '(' TOKEN_FHID ')'		   { $$ = Foam::fvc::ddt( driver.getOrReadField<Foam::surfaceSphericalTensorField>(*$3,true,true)() ).ptr(); } // no fvc::ddt for surface Fields
        | TOKEN_oldTime '(' TOKEN_FHID ')'	   { $$ = new Foam::surfaceSphericalTensorField( driver.getOrReadField<Foam::surfaceSphericalTensorField>(*$3,true,true)->oldTime()); }
        | TOKEN_flux '(' fsexp ',' hexp ')'       { $$ = Foam::fvc::flux(*$3,*$5).ptr(); delete $3; delete $5; }
;

// point fields

psexp:  TOKEN_point '(' scalar ')'           { $$ = driver.makePointConstantField<Foam::pointScalarField>($3); }
        | psexp '+' psexp 		    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 + *$3); delete $1; delete $3; }
        | psexp '*' psexp 		    { sameSize($1,$3); $$ = driver.makePointField<Foam::pointScalarField>( $1->internalField() * $3->internalField()); delete $1; delete $3; }
        | pvexp '&' pvexp 		    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 & *$3); delete $1; delete $3; }
        | ptexp TOKEN_AND ptexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | pyexp TOKEN_AND ptexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | phexp TOKEN_AND ptexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | ptexp TOKEN_AND pyexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | pyexp TOKEN_AND pyexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | phexp TOKEN_AND pyexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | ptexp TOKEN_AND phexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | pyexp TOKEN_AND phexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | phexp TOKEN_AND phexp 	    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 && *$3); delete $1; delete $3; }
        | psexp '/' psexp 		    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 / *$3); delete $1; delete $3; }
        | psexp '-' psexp 		    { sameSize($1,$3); $$ = new Foam::pointScalarField(*$1 - *$3); delete $1; delete $3;}
        | TOKEN_pow '(' psexp ',' scalar ')'{ $$ = driver.makePointField<Foam::pointScalarField>(Foam::pow($3->internalField(),$5)()); delete $3; }
        | TOKEN_log '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::log($3->internalField())()); delete $3; }
        | TOKEN_exp '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::exp($3->internalField())()); delete $3; }
        | TOKEN_sqr '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::sqr($3->internalField())()); delete $3; }
        | TOKEN_sqrt '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::sqrt($3->internalField())()); delete $3; }
        | TOKEN_sin '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::sin($3->internalField())()); delete $3; }
        | TOKEN_cos '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::cos($3->internalField())()); delete $3; }
        | TOKEN_tan '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::tan($3->internalField())()); delete $3; }
        | TOKEN_log10 '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::log10($3->internalField())()); delete $3; }
        | TOKEN_asin '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::asin($3->internalField())()); delete $3; }
        | TOKEN_acos '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::acos($3->internalField())()); delete $3; }
        | TOKEN_atan '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::atan($3->internalField())()); delete $3; }
        | TOKEN_sinh '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::sinh($3->internalField())()); delete $3; }
        | TOKEN_cosh '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::cosh($3->internalField())()); delete $3; }
        | TOKEN_tanh '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::tanh($3->internalField())()); delete $3; }
        | TOKEN_asinh '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::asinh($3->internalField())()); delete $3; }
        | TOKEN_acosh '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::acosh($3->internalField())()); delete $3; }
        | TOKEN_atanh '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::atanh($3->internalField())()); delete $3; }
        | TOKEN_erf '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::erf($3->internalField())()); delete $3; }
        | TOKEN_erfc '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::erfc($3->internalField())()); delete $3; }
        | TOKEN_lgamma '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::lgamma($3->internalField())()); delete $3; }
        | TOKEN_besselJ1 '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::j1($3->internalField())()); delete $3; }
        | TOKEN_besselJ0 '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::j0($3->internalField())()); delete $3; }
        | TOKEN_besselY0 '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::y0($3->internalField())()); delete $3; }
        | TOKEN_besselY1 '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::y1($3->internalField())()); delete $3; }
        | TOKEN_sign '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::sign($3->internalField())()); delete $3; }
        | TOKEN_pos '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::pos($3->internalField())()); delete $3; }
        | TOKEN_neg '(' psexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::neg($3->internalField())()); delete $3; }
        | TOKEN_min '(' psexp ',' psexp  ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::min($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_max '(' psexp ',' psexp  ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::max($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_min '(' psexp ')'           { $$ = driver.makePointConstantField<Foam::pointScalarField>(Foam::gMin($3->internalField())); delete $3; }
        | TOKEN_max '(' psexp ')'           { $$ = driver.makePointConstantField<Foam::pointScalarField>(Foam::gMax($3->internalField())); delete $3; }
        | TOKEN_sum '(' psexp ')'           { $$ = driver.makePointConstantField<Foam::pointScalarField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' psexp ')'       { $$ = driver.makePointConstantField<Foam::pointScalarField>(Foam::average(*$3).value()); delete $3; }
        | '-' psexp %prec TOKEN_NEG         { $$ = driver.makePointField<Foam::pointScalarField>(-$2->internalField()); delete $2; }
        | '(' psexp ')'		            { $$ = $2; }  
        | pvexp '.' vectorComponentSwitch  TOKEN_x                     { $$ = new Foam::pointScalarField($1->component(0)); delete $1; }
        | pvexp '.' vectorComponentSwitch  TOKEN_y                     { $$ = new Foam::pointScalarField($1->component(1)); delete $1; }
        | pvexp '.' vectorComponentSwitch  TOKEN_z                     { $$ = new Foam::pointScalarField($1->component(2)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_xx                { $$ = new Foam::pointScalarField($1->component(0)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_xy                { $$ = new Foam::pointScalarField($1->component(1)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_xz                { $$ = new Foam::pointScalarField($1->component(2)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_yx                { $$ = new Foam::pointScalarField($1->component(3)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_yy                { $$ = new Foam::pointScalarField($1->component(4)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_yz                { $$ = new Foam::pointScalarField($1->component(5)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_zx                { $$ = new Foam::pointScalarField($1->component(6)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_zy                { $$ = new Foam::pointScalarField($1->component(7)); delete $1; }
        | ptexp '.' tensorComponentSwitch TOKEN_zz                { $$ = new Foam::pointScalarField($1->component(8)); delete $1; }
        | pyexp '.' tensorComponentSwitch TOKEN_xx                { $$ = new Foam::pointScalarField($1->component(0)); delete $1; }
        | pyexp '.' tensorComponentSwitch TOKEN_xy                { $$ = new Foam::pointScalarField($1->component(1)); delete $1; }
        | pyexp '.' tensorComponentSwitch TOKEN_xz                { $$ = new Foam::pointScalarField($1->component(2)); delete $1; }
        | pyexp '.' tensorComponentSwitch TOKEN_yy                { $$ = new Foam::pointScalarField($1->component(3)); delete $1; }
        | pyexp '.' tensorComponentSwitch TOKEN_yz                { $$ = new Foam::pointScalarField($1->component(4)); delete $1; }
        | pyexp '.' tensorComponentSwitch TOKEN_zz                { $$ = new Foam::pointScalarField($1->component(5)); delete $1; }
        | phexp '.' tensorComponentSwitch TOKEN_ii                { $$ = new Foam::pointScalarField($1->component(0)); delete $1; }
        | plexp '?' psexp ':' psexp         { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makePointConstantField<Foam::pointScalarField>(0.)); delete $1; delete $3; delete $5; }
        | TOKEN_mag '(' psexp ')'           { $$ = new Foam::pointScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' pvexp ')'           { $$ = new Foam::pointScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' ptexp ')'           { $$ = new Foam::pointScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' pyexp ')'           { $$ = new Foam::pointScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' phexp ')'           { $$ = new Foam::pointScalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_magSqr '(' psexp ')'           { $$ = new Foam::pointScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' pvexp ')'           { $$ = new Foam::pointScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' ptexp ')'           { $$ = new Foam::pointScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' pyexp ')'           { $$ = new Foam::pointScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_magSqr '(' phexp ')'           { $$ = new Foam::pointScalarField(Foam::magSqr(*$3)); delete $3; }
        | TOKEN_tr '(' ptexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::tr($3->internalField())()); delete $3; }
        | TOKEN_tr '(' pyexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::tr($3->internalField())()); delete $3; }
        | TOKEN_tr '(' phexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::tr($3->internalField())()); delete $3; }
        | TOKEN_det '(' ptexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::det($3->internalField())()); delete $3; }
        | TOKEN_det '(' pyexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::det($3->internalField())()); delete $3; }
        | TOKEN_det '(' phexp ')'           { $$ = driver.makePointField<Foam::pointScalarField>(Foam::det($3->internalField())()); delete $3; }
        | TOKEN_interpolateToPoint '(' exp ')'              { $$ = driver.cellToPointInterpolate(*$3).ptr(); delete $3; }
          | TOKEN_PSID                        { $$ = driver.getPointField<Foam::pointScalarField>(*$1); }
          | TOKEN_oldTime '(' TOKEN_PSID ')'		   { $$ = new Foam::pointScalarField( driver.getOrReadPointField<Foam::pointScalarField>(*$3,true,true)->oldTime()); }
          | TOKEN_LOOKUP '(' psexp ')'	    { $$ = driver.makePointField<Foam::pointScalarField>(driver.getLookup(*$1,*$3)); delete $1; delete $3; }
;

pvexp:  pvector                            { $$ = $1; }
        | pvexp '+' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 + *$3); delete $1; delete $3; }
        | psexp '*' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 * *$3); delete $1; delete $3; }
        | pvexp '*' psexp 		   { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
            $$ = driver.makePointField<Foam::pointVectorField>($1->internalField() * $3->internalField());
#else
            $$ = new Foam::pointVectorField(*$1 * *$3); 
#endif
            delete $1; delete $3; }
        | ptexp '&' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '&' ptexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 & *$3); delete $1; delete $3; }
        | phexp '&' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '&' phexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 & *$3); delete $1; delete $3; }
        | pyexp '&' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '&' pyexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '^' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 ^ *$3); delete $1; delete $3; }
        | pvexp '/' psexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 / *$3); delete $1; delete $3; }
        | pvexp '-' pvexp 		   { sameSize($1,$3); $$ = new Foam::pointVectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' pvexp %prec TOKEN_NEG         { $$ = driver.makePointField<Foam::pointVectorField>(-$2->internalField()); delete $2; }
        | '(' pvexp ')'		           { $$ = $2; }  
        | plexp '?' pvexp ':' pvexp        { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makePointConstantField<Foam::pointVectorField>(Foam::vector::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_pposition '(' ')'          { $$ = driver.makePointPositionField(); }
        | TOKEN_interpolateToPoint '(' vexp ')'              { $$ = driver.cellToPointInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' pvexp ',' pvexp  ')'           { $$ = driver.makePointField<Foam::pointVectorField>(Foam::min($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_max '(' pvexp ',' pvexp  ')'           { $$ = driver.makePointField<Foam::pointVectorField>(Foam::max($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_min '(' pvexp ')'           { $$ = driver.makePointConstantField<Foam::pointVectorField>(Foam::gMin($3->internalField())); delete $3; }
        | TOKEN_minPosition '(' psexp ')'           { 
            $$ = driver.makePointConstantField<Foam::pointVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.mesh().points()
                )
            ); 
            delete $3; 
        }
        | TOKEN_max '(' pvexp ')'           { $$ = driver.makePointConstantField<Foam::pointVectorField>(Foam::gMax($3->internalField())); delete $3; }
        | TOKEN_maxPosition '(' psexp ')'           { 
            $$ = driver.makePointConstantField<Foam::pointVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.mesh().points()
                )
            ); 
            delete $3; 
        }
        | TOKEN_sum '(' pvexp ')'          { $$ = driver.makePointConstantField<Foam::pointVectorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' pvexp ')'      { $$ = driver.makePointConstantField<Foam::pointVectorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_PVID                       { $$ = driver.getPointField<Foam::pointVectorField>(*$1); }
        | TOKEN_oldTime '(' TOKEN_PVID ')'		   { $$ = new Foam::pointVectorField( driver.getOrReadPointField<Foam::pointVectorField>(*$3,true,true)->oldTime()); }
;
 
ptexp:   ptensor                  { $$ = $1; }
        | ptexp '+' ptexp 		          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '+' pyexp 		          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() + $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 + *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '+' phexp 		          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
            $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() + $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 + *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '+' ptexp 		          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
            $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() + $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 + *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '+' ptexp 		          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() + $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 + *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | psexp '*' ptexp 	   	                  { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '*' psexp 	   	                  { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() * $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 * *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pvexp '*' pvexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '&' ptexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '&' ptexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '&' pyexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '&' ptexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '&' phexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '/' psexp 	   	                  { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '-' ptexp 	   	          { sameSize($1,$3); $$ = new Foam::pointTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '-' pyexp 	   	          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() - $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 - *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | ptexp '-' phexp 	   	          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() - $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 - *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '-' ptexp 	   	          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() - $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 - *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '-' ptexp 	   	          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointTensorField>($1->internalField() - $3->internalField());
#else
     $$ = new Foam::pointTensorField(*$1 - *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' ptexp %prec TOKEN_NEG         { $$ = driver.makePointField<Foam::pointTensorField>(-$2->internalField()); delete $2; }
        | '(' ptexp ')'		        { $$ = $2; }  
        | TOKEN_skew '(' ptexp ')' 	           { $$ = new Foam::pointTensorField( Foam::skew(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_inv '(' ptexp ')' 	           { $$ = new Foam::pointTensorField( Foam::inv(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | TOKEN_dev '(' ptexp ')' 	           { $$ = new Foam::pointTensorField( Foam::dev(*$3) ); delete $3;  driver.setCalculatedPatches(*$$); } 
        | plexp '?' ptexp ':' ptexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makePointConstantField<Foam::pointTensorField>(Foam::tensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_interpolateToPoint '(' texp ')'              { $$ = driver.cellToPointInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' ptexp ',' ptexp  ')'           { $$ = driver.makePointField<Foam::pointTensorField>(Foam::min($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_max '(' ptexp ',' ptexp  ')'           { $$ = driver.makePointField<Foam::pointTensorField>(Foam::max($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_min '(' ptexp ')'           { $$ = driver.makePointConstantField<Foam::pointTensorField>(Foam::gMin($3->internalField())); delete $3; }
        | TOKEN_max '(' ptexp ')'           { $$ = driver.makePointConstantField<Foam::pointTensorField>(Foam::gMax($3->internalField())); delete $3; }
        | TOKEN_sum '(' ptexp ')'                 { $$ = driver.makePointConstantField<Foam::pointTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' ptexp ')'             { $$ = driver.makePointConstantField<Foam::pointTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_PTID                               { $$=driver.getPointField<Foam::pointTensorField>(*$1); }
        | TOKEN_oldTime '(' TOKEN_PTID ')'	   { $$ = new Foam::pointTensorField( driver.getOrReadPointField<Foam::pointTensorField>(*$3,true,true)->oldTime()); }
;

pyexp:   psymmTensor                  { $$ = $1; }
        | pyexp '+' pyexp 		          { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '+' pyexp 		          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointSymmTensorField>($1->internalField() + $3->internalField());
#else
     $$ = new Foam::pointSymmTensorField(*$1 + *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '+' phexp 		          { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | psexp '*' pyexp 	   	                  { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '*' psexp 	   	                  { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointSymmTensorField>($1->internalField() * $3->internalField());
#else
     $$ = new Foam::pointSymmTensorField(*$1 * *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
// not working in 2.0        | pyexp '&' pyexp 	   	          { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
// not working either | pyexp '&' pyexp 	   	          { sameSize($1,$3); $$ = driver.makePointField<Foam::pointSymmTensorField>(($1->internalField() & $3->internalField())()); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '&' pyexp 	   	          { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '&' phexp 	   	          { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '/' psexp 	   	                  { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '-' pyexp 	   	          { sameSize($1,$3); $$ = new Foam::pointSymmTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '-' pyexp 	   	          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointSymmTensorField>($1->internalField() - $3->internalField());
#else
     $$ = new Foam::pointSymmTensorField(*$1 - *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | pyexp '-' phexp 	   	          { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointSymmTensorField>($1->internalField() - $3->internalField());
#else
     $$ = new Foam::pointSymmTensorField(*$1 - *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' pyexp %prec TOKEN_NEG         { $$ = driver.makePointField<Foam::pointSymmTensorField>(-$2->internalField()); delete $2; }
        | '(' pyexp ')'		        { $$ = $2; }  
        | TOKEN_symm '(' ptexp ')'           { $$ = driver.makePointField<Foam::pointSymmTensorField>(Foam::symm($3->internalField())()); delete $3; }
        | TOKEN_symm '(' pyexp ')'           { $$ = driver.makePointField<Foam::pointSymmTensorField>(Foam::symm($3->internalField())()); delete $3; }
        | TOKEN_inv '(' pyexp ')'           { $$ = driver.makePointField<Foam::pointSymmTensorField>(Foam::inv($3->internalField())()); delete $3; }
        | TOKEN_dev '(' pyexp ')'           { $$ = driver.makePointField<Foam::pointSymmTensorField>(Foam::dev($3->internalField())()); delete $3; }
        | plexp '?' pyexp ':' pyexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makePointConstantField<Foam::pointSymmTensorField>(Foam::symmTensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_interpolateToPoint '(' yexp ')'              { $$ = driver.cellToPointInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' pyexp ',' pyexp  ')'           { $$ = driver.makePointField<Foam::pointSymmTensorField>(Foam::min($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_max '(' pyexp ',' pyexp  ')'           { $$ = driver.makePointField<Foam::pointSymmTensorField>(Foam::max($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_min '(' pyexp ')'           { $$ = driver.makePointConstantField<Foam::pointSymmTensorField>(Foam::gMin($3->internalField())); delete $3; }
        | TOKEN_max '(' pyexp ')'           { $$ = driver.makePointConstantField<Foam::pointSymmTensorField>(Foam::gMax($3->internalField())); delete $3; }
        | TOKEN_sum '(' pyexp ')'                 { $$ = driver.makePointConstantField<Foam::pointSymmTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' pyexp ')'             { $$ = driver.makePointConstantField<Foam::pointSymmTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_PYID                               { $$=driver.getPointField<Foam::pointSymmTensorField>(*$1); }
        | TOKEN_oldTime '(' TOKEN_PYID ')'	   { $$ = new Foam::pointSymmTensorField( driver.getOrReadPointField<Foam::pointSymmTensorField>(*$3,true,true)->oldTime()); }
;

phexp:   psphericalTensor                  { $$ = $1; }
        | phexp '+' phexp 		          { sameSize($1,$3); $$ = new Foam::pointSphericalTensorField(*$1 + *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | psexp '*' phexp 	   	                  { sameSize($1,$3); $$ = new Foam::pointSphericalTensorField(*$1 * *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '*' psexp 	   	                  { sameSize($1,$3); 
#ifdef INCOMPLETE_OPERATORS
     $$ = driver.makePointField<Foam::pointSphericalTensorField>($1->internalField() * $3->internalField());
#else
     $$ = new Foam::pointSphericalTensorField(*$1 * *$3); 
#endif
     delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '&' phexp 	   	          { sameSize($1,$3); $$ = new Foam::pointSphericalTensorField(*$1 & *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '/' psexp 	   	                  { sameSize($1,$3); $$ = new Foam::pointSphericalTensorField(*$1 / *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | phexp '-' phexp 	   	          { sameSize($1,$3); $$ = new Foam::pointSphericalTensorField(*$1 - *$3); delete $1; delete $3;  driver.setCalculatedPatches(*$$); }  
        | '-' phexp %prec TOKEN_NEG         { $$ = driver.makePointField<Foam::pointSphericalTensorField>(-$2->internalField()); delete $2; }
        | '(' phexp ')'		        { $$ = $2; }  
        | TOKEN_inv '(' phexp ')'           { $$ = driver.makePointField<Foam::pointSphericalTensorField>(Foam::inv($3->internalField())()); delete $3; }
        | plexp '?' phexp ':' phexp                   { sameSize($1,$3); sameSize($1,$5); $$ = driver.doConditional($1,$3,$5,driver.makePointConstantField<Foam::pointSphericalTensorField>(Foam::sphericalTensor::zero)); delete $1; delete $3; delete $5; }
        | TOKEN_interpolateToPoint '(' hexp ')'              { $$ = driver.cellToPointInterpolate(*$3).ptr(); delete $3; }
        | TOKEN_min '(' phexp ',' phexp  ')'           { $$ = driver.makePointField<Foam::pointSphericalTensorField>(Foam::min($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_max '(' phexp ',' phexp  ')'           { $$ = driver.makePointField<Foam::pointSphericalTensorField>(Foam::max($3->internalField(),$5->internalField())); delete $3; delete $5; }
        | TOKEN_min '(' phexp ')'           { $$ = driver.makePointConstantField<Foam::pointSphericalTensorField>(Foam::gMin($3->internalField())); delete $3; }
        | TOKEN_max '(' phexp ')'           { $$ = driver.makePointConstantField<Foam::pointSphericalTensorField>(Foam::gMax($3->internalField())); delete $3; }
        | TOKEN_sum '(' phexp ')'                 { $$ = driver.makePointConstantField<Foam::pointSphericalTensorField>(Foam::sum(*$3).value()); delete $3; }
        | TOKEN_average '(' phexp ')'             { $$ = driver.makePointConstantField<Foam::pointSphericalTensorField>(Foam::average(*$3).value()); delete $3; }
        | TOKEN_PHID                               { $$=driver.getPointField<Foam::pointSphericalTensorField>(*$1); }
        | TOKEN_oldTime '(' TOKEN_PHID ')'	   { $$ = new Foam::pointSphericalTensorField( driver.getOrReadPointField<Foam::pointSphericalTensorField>(*$3,true,true)->oldTime()); }
;

plexp: TOKEN_point '(' TOKEN_TRUE ')'  { $$ = driver.makePointConstantField<Foam::pointScalarField>(1); }
    | TOKEN_point '(' TOKEN_FALSE ')'  { $$ = driver.makePointConstantField<Foam::pointScalarField>(0); }
    | TOKEN_pset '(' TOKEN_PSETID ')'    { $$ = driver.makePointSetField(*$3); }
    | TOKEN_pzone '(' TOKEN_PZONEID ')'  { $$ = driver.makePointZoneField(*$3); }
    | psexp '<' psexp                 { sameSize($1,$3); $$ = driver.doPointCompare($1,std::less<Foam::scalar>(),$3);  delete $1; delete $3; }
    | psexp '>' psexp                 { sameSize($1,$3); $$ = driver.doPointCompare($1,std::greater<Foam::scalar>(),$3);  delete $1; delete $3; }
    | psexp TOKEN_LEQ psexp           { sameSize($1,$3); $$ = driver.doPointCompare($1,std::less_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | psexp TOKEN_GEQ psexp           { sameSize($1,$3); $$ = driver.doPointCompare($1,std::greater_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | psexp TOKEN_EQ psexp            { sameSize($1,$3); $$ = driver.doPointCompare($1,std::equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | psexp TOKEN_NEQ psexp           { sameSize($1,$3); $$ = driver.doPointCompare($1,std::not_equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '(' plexp ')'		      { $$ = $2; }
    | plexp TOKEN_AND plexp           { sameSize($1,$3); $$ = driver.doPointLogicalOp($1,std::logical_and<Foam::scalar>(),$3);  delete $1; delete $3; }
    | plexp TOKEN_OR plexp            { sameSize($1,$3); $$ = driver.doPointLogicalOp($1,std::logical_or<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '!' plexp %prec TOKEN_NOT       { $$ = driver.doPointLogicalNot($2); delete $2; }
;


vector: TOKEN_VECTOR '(' exp ',' exp ',' exp ')' {     $$ = driver.makeVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

tensor: TOKEN_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {     $$ = driver.makeTensorField($3,$5,$7,$9,$11,$13,$15,$17,$19);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13; delete $15; delete $17; delete $19; }
;

symmTensor: TOKEN_SYMM_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {     $$ = driver.makeSymmTensorField($3,$5,$7,$9,$11,$13);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;}
;

sphericalTensor: TOKEN_SPHERICAL_TENSOR '(' exp ')' {     $$ = driver.makeSphericalTensorField($3);  delete $3; }
;

fvector: TOKEN_VECTOR '(' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeSurfaceVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

ftensor: TOKEN_TENSOR '(' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeSurfaceTensorField($3,$5,$7,$9,$11,$13,$15,$17,$19);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13; delete $15; delete $17; delete $19; }
;

fsymmTensor: TOKEN_SYMM_TENSOR '(' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ')' {     $$ = driver.makeSurfaceSymmTensorField($3,$5,$7,$9,$11,$13);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;}
;

fsphericalTensor: TOKEN_SPHERICAL_TENSOR '(' fsexp ')' {     $$ = driver.makeSurfaceSphericalTensorField($3);  delete $3; }
;

pvector: TOKEN_VECTOR '(' psexp ',' psexp ',' psexp ')' {     $$ = driver.makePointVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

ptensor: TOKEN_TENSOR '(' psexp ',' psexp ',' psexp ',' psexp ',' psexp ',' psexp ',' psexp ',' psexp ',' psexp ')' {     $$ = driver.makePointTensorField($3,$5,$7,$9,$11,$13,$15,$17,$19);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13; delete $15; delete $17; delete $19; }
;

psymmTensor: TOKEN_SYMM_TENSOR '(' psexp ',' psexp ',' psexp ',' psexp ',' psexp ',' psexp ')' {     $$ = driver.makePointSymmTensorField($3,$5,$7,$9,$11,$13);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;}
;

psphericalTensor: TOKEN_SPHERICAL_TENSOR '(' psexp ')' {     $$ = driver.makePointSphericalTensorField($3);  delete $3; }
;

%%

void parserField::FieldValueExpressionParser::error (const parserField::FieldValueExpressionParser::location_type& l,const std::string& m)
{
     driver.error (l, m);
}
