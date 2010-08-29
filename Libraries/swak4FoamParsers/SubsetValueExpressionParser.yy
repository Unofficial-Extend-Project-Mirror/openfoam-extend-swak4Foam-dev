/*  -*- C++ -*- */

%skeleton "lalr1.cc"
/* %require "2.1a" */
%defines
%define "parser_class_name" "SubsetValueExpressionParser"

%{
#include <volFields.H>

#include <functional>
#include <cmath>

    namespace Foam {
        class SubsetValueExpressionDriver;
    }

    const Foam::scalar HugeVal=1e40;

    using Foam::SubsetValueExpressionDriver;
%}

%name-prefix="parserSubset"

%parse-param { SubsetValueExpressionDriver& driver }
%lex-param { SubsetValueExpressionDriver& driver }

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
    Foam::vector *vec;
    Foam::string *name;
    Foam::vectorField *vfield;
    Foam::scalarField *sfield;
    Foam::Field<bool> *lfield;
    Foam::tensorField *tfield;
    Foam::symmTensorField *yfield;
    Foam::sphericalTensorField *hfield;
};

%{
#include "SubsetValueExpressionDriverYY.H"
%}

%token <name>   TOKEN_LINE  "timeline"
%token <name>   TOKEN_SID   "scalarID"
%token <name>   TOKEN_VID   "vectorID"
%token <name>   TOKEN_LID   "logicalID"
%token <name>   TOKEN_TID   "tensorID"
%token <name>   TOKEN_YID   "symmTensorID"
%token <name>   TOKEN_HID   "sphericalTensorID"
%token <name>   TOKEN_PSID   "pointScalarID"
%token <name>   TOKEN_PVID   "pointVectorID"
%token <name>   TOKEN_PLID   "pointLogicalID"
%token <name>   TOKEN_PTID   "pointTensorID"
%token <name>   TOKEN_PYID   "pointSymmTensorID"
%token <name>   TOKEN_PHID   "pointSphericalTensorID"
%token <val>    TOKEN_NUM   "value"
%token <vec>    TOKEN_VEC   "vector"
%type  <val>    scalar      "sexpression"  
%type  <val>    sreduced;
%type  <vec>    vreduced;
%type  <sfield>    exp        "expression"
%type  <sfield>    pexp        "pexpression"
%type  <lfield>    lexp       "lexpression"
%type  <lfield>    plexp       "plexpression"
%type  <vfield>    vexp       "vexpression"
%type  <vfield>    pvexp       "pvexpression"
%type  <vfield>    vector     
%type  <tfield>    tensor     
%type  <tfield>    texp       "texpression"     
%type  <tfield>    ptexp       "ptexpression"     
%type  <yfield>    symmTensor     
%type  <yfield>    yexp       "yexpression"     
%type  <yfield>    pyexp       "pyexpression"     
%type  <hfield>    sphericalTensor     
%type  <hfield>    hexp       "hexpression"     
%type  <hfield>    phexp       "phexpression"     

%token TOKEN_VECTOR
%token TOKEN_TENSOR
%token TOKEN_SYMM_TENSOR
%token TOKEN_SPHERICAL_TENSOR

%token TOKEN_TRUE
%token TOKEN_FALSE

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
%token TOKEN_id
%token TOKEN_randNormal
%token TOKEN_position
%token TOKEN_area
%token TOKEN_volume
%token TOKEN_Sf
%token TOKEN_normal

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
%token TOKEN_average
%token TOKEN_sum
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

%token TOKEN_toPoint
%token TOKEN_toFace

%token TOKEN_points

%token TOKEN_transpose
%token TOKEN_diag
%token TOKEN_tr
%token TOKEN_dev
%token TOKEN_symm
%token TOKEN_skew
%token TOKEN_det
%token TOKEN_cof
%token TOKEN_inv

%token TOKEN_cpu

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

%printer             { debug_stream () << *$$; } "scalarID" "vectorID" "logicalID" "pointScalarID" "pointVectorID" "pointLogicalID" "tensorID" "pointTensorID" "symmTensorID" "pointSymmTensorID" "sphericalTensorID" "pointSphericalTensorID"
%printer             { Foam::OStringStream buff; buff << *$$; debug_stream () << buff.str().c_str(); } "vector"
%destructor          { delete $$; } "timeline" "scalarID"  "vectorID"  "logicalID" "pointScalarID" "pointVectorID" "pointLogicalID" "vector" "expression" "vexpression" "lexpression" "pexpression" "pvexpression" "plexpression" "texpression" "ptexpression" "yexpression" "pyexpression" "hexpression" "phexpression"
%printer             { debug_stream () << $$; } "value"  "sexpression"
%printer             { debug_stream () << $$ /* ->name().c_str() */ ; } "expression"  "vexpression" "lexpression" "pexpression" "pvexpression" "plexpression" "texpression" "ptexpression" "yexpression" "pyexpression" "hexpression" "phexpression"


%%
%start unit;

unit:   exp                     { driver.setResult<Foam::scalar>($1);  }
        | vexp                  { driver.setResult<Foam::vector>($1);  }
        | lexp                  { driver.setResult<bool>($1); }
        | pexp                  { driver.setResult<Foam::scalar>($1,true);  }
        | pvexp                 { driver.setResult<Foam::vector>($1,true);  }
        | plexp                 { driver.setResult<bool>($1,true); }
        | texp                  { driver.setResult<Foam::tensor>($1);  }
        | ptexp                 { driver.setResult<Foam::tensor>($1,true);  }
        | yexp                  { driver.setResult<Foam::symmTensor>($1);  }
        | pyexp                 { driver.setResult<Foam::symmTensor>($1,true);  }
        | hexp                  { driver.setResult<Foam::sphericalTensor>($1);  }
        | phexp                 { driver.setResult<Foam::sphericalTensor>($1,true);  }
;

vexp:   vector                  { $$ = $1; }
        | vreduced              { $$ = driver.makeField<Foam::vector>(*$1); delete $1; }
        | vexp '+' vexp 		{ $$ = new Foam::vectorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' vexp 		        { $$ = new Foam::vectorField(*$1 * *$3); delete $1; delete $3; }
        | vexp '*' exp 		        { $$ = new Foam::vectorField(*$1 * *$3); delete $1; delete $3; }
        | texp '&' vexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' texp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | yexp '&' vexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' yexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | hexp '&' vexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '&' hexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | vexp '/' exp 		        { $$ = new Foam::vectorField(*$1 / *$3); delete $1; delete $3; }
        | vexp '^' vexp 		{ $$ = new Foam::vectorField(*$1 ^ *$3); delete $1; delete $3; }
        | vexp '-' vexp 		{ $$ = new Foam::vectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' vexp %prec TOKEN_NEG 	        { $$ = new Foam::vectorField(-*$2); delete $2; }
        | '(' vexp ')'		        { $$ = $2; }  
//        | TOKEN_diag '(' texp ')'       { $$ = new Foam::vectorField( Foam::diag(*$3) ); delete $3; }
        | lexp '?' vexp ':' vexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
        | TOKEN_position '(' ')'        { $$ = driver.makePositionField(); }
        | TOKEN_normal '(' ')'          { $$ = driver.makeFaceNormalField(); }
        | TOKEN_Sf '(' ')'              { $$ = driver.makeFaceAreaField(); }
//        | TOKEN_toFace '(' pvexp ')'        { $$ = driver.toFace(*$3); delete $3; }
        | TOKEN_VID {
            $$=driver.getVectorField(*$1); delete $1;
                    }
;

  
scalar:	TOKEN_NUM		        { $$ = $1; }
        | '-' TOKEN_NUM         	{ $$ = -$2; } 
;

sreduced: TOKEN_min '(' exp ')'       { $$ = Foam::gMin(*$3); delete $3; }
        | TOKEN_max '(' exp ')'       { $$ = Foam::gMax(*$3); delete $3; }
        | TOKEN_min '(' pexp ')'      { $$ = Foam::gMin(*$3); delete $3; }
        | TOKEN_max '(' pexp ')'      { $$ = Foam::gMax(*$3); delete $3; }
        | TOKEN_sum '(' exp ')'       { $$ = Foam::gSum(*$3); delete $3; }
        | TOKEN_sum '(' pexp ')'      { $$ = Foam::gSum(*$3); delete $3; }
        | TOKEN_average '(' exp ')'   { $$ = Foam::gAverage(*$3); delete $3; }
        | TOKEN_average '(' pexp ')'  { $$ = Foam::gAverage(*$3); delete $3; }
;

vreduced: TOKEN_min '(' vexp ')'       { 
          Foam::vector tmp(HugeVal,HugeVal,HugeVal);
            if(($3->size())>0) {
                tmp=Foam::min(*$3);
            }
            Foam::reduce(tmp,Foam::minOp<Foam::vector>());
            $$ = new Foam::vector(tmp);
            delete $3; }
        | TOKEN_max '(' vexp ')'       { 
            Foam::vector tmp(-HugeVal,-HugeVal,-HugeVal);
            if(($3->size())>0) {
                tmp=Foam::max(*$3);
            }
            Foam::reduce(tmp,Foam::maxOp<Foam::vector>());
            $$ = new Foam::vector(tmp);
            delete $3; }
        | TOKEN_min '(' pvexp ')'       { 
          Foam::vector tmp(HugeVal,HugeVal,HugeVal);
            if(($3->size())>0) {
                tmp=Foam::min(*$3);
            }
            Foam::reduce(tmp,Foam::minOp<Foam::vector>());
            $$ = new Foam::vector(tmp);
            delete $3; }
        | TOKEN_max '(' pvexp ')'       { 
            Foam::vector tmp(-HugeVal,-HugeVal,-HugeVal);
            if(($3->size())>0) {
                tmp=Foam::max(*$3);
            }
            Foam::reduce(tmp,Foam::maxOp<Foam::vector>());
            $$ = new Foam::vector(tmp);
            delete $3; }
        | TOKEN_sum '(' vexp ')'       { $$ = new Foam::vector(Foam::gSum(*$3)); delete $3; }
        | TOKEN_sum '(' pvexp ')'      { $$ = new Foam::vector(Foam::gSum(*$3)); delete $3; }
        | TOKEN_average '(' vexp ')'   { $$ = new Foam::vector(Foam::gAverage(*$3)); delete $3; }
        | TOKEN_average '(' pvexp ')'  { $$ = new Foam::vector(Foam::gAverage(*$3)); delete $3; }
;

exp:    TOKEN_NUM                  { $$ = driver.makeField($1); }
        | sreduced                 { $$ = driver.makeField($1); }
        | exp '+' exp 		{ $$ = new Foam::scalarField(*$1 + *$3); delete $1; delete $3; }
        | exp '-' exp 		{ $$ = new Foam::scalarField(*$1 - *$3); delete $1; delete $3; }
        | exp '*' exp 		{ $$ = new Foam::scalarField(*$1 * *$3); delete $1; delete $3; }
        | exp '%' exp 		{ $$ = driver.makeModuloField(*$1,*$3); delete $1; delete $3; }
        | exp '/' exp 		{ $$ = new Foam::scalarField(*$1 / *$3); delete $1; delete $3; }
        | TOKEN_pow '(' exp ',' scalar ')'		{ $$ = new Foam::scalarField(Foam::pow(*$3, $5)); delete $3; }
        | TOKEN_log '(' exp ')'       { $$ = new Foam::scalarField(Foam::log(*$3)); delete $3; }
        | TOKEN_exp '(' exp ')'       { $$ = new Foam::scalarField(Foam::exp(*$3)); delete $3; }
        | vexp '&' vexp 	{ $$ = new Foam::scalarField(*$1 & *$3); delete $1; delete $3;}
        | texp TOKEN_AND texp 	{ $$ = new Foam::scalarField(*$1 && *$3); delete $1; delete $3;}
        | '-' exp %prec TOKEN_NEG 	{ $$ = new Foam::scalarField(-*$2); delete $2; }
	| '(' exp ')'		{ $$ = $2; }
        | TOKEN_sqr '(' exp ')'       { $$ = new Foam::scalarField(Foam::sqr(*$3)); delete $3; }
        | TOKEN_sqrt '(' exp ')'       { $$ = new Foam::scalarField(Foam::sqrt(*$3)); delete $3; }
        | TOKEN_sin '(' exp ')'       { $$ = new Foam::scalarField(Foam::sin(*$3)); delete $3; }
        | TOKEN_cos '(' exp ')'       { $$ = new Foam::scalarField(Foam::cos(*$3)); delete $3; }
        | TOKEN_tan '(' exp ')'       { $$ = new Foam::scalarField(Foam::tan(*$3)); delete $3; }
        | TOKEN_log10 '(' exp ')'         { $$ = new Foam::scalarField(Foam::log10(*$3)); delete $3; }
        | TOKEN_asin '(' exp ')'          { $$ = new Foam::scalarField(Foam::asin(*$3)); delete $3; }
        | TOKEN_acos '(' exp ')'          { $$ = new Foam::scalarField(Foam::acos(*$3)); delete $3; }
        | TOKEN_atan '(' exp ')'          { $$ = new Foam::scalarField(Foam::atan(*$3)); delete $3; }
        | TOKEN_sinh '(' exp ')'          { $$ = new Foam::scalarField(Foam::sinh(*$3)); delete $3; }
        | TOKEN_cosh '(' exp ')'          { $$ = new Foam::scalarField(Foam::cosh(*$3)); delete $3; }
        | TOKEN_tanh '(' exp ')'          { $$ = new Foam::scalarField(Foam::tanh(*$3)); delete $3; }
        | TOKEN_asinh '(' exp ')'         { $$ = new Foam::scalarField(Foam::asinh(*$3)); delete $3; }
        | TOKEN_acosh '(' exp ')'         { $$ = new Foam::scalarField(Foam::acosh(*$3)); delete $3; }
        | TOKEN_atanh '(' exp ')'         { $$ = new Foam::scalarField(Foam::atanh(*$3)); delete $3; }
        | TOKEN_erf '(' exp ')'           { $$ = new Foam::scalarField(Foam::erf(*$3)); delete $3; }
        | TOKEN_erfc '(' exp ')'          { $$ = new Foam::scalarField(Foam::erfc(*$3)); delete $3; }
        | TOKEN_lgamma '(' exp ')'        { $$ = new Foam::scalarField(Foam::lgamma(*$3)); delete $3; }
        | TOKEN_besselJ0 '(' exp ')'      { $$ = new Foam::scalarField(Foam::j0(*$3)); delete $3; }
        | TOKEN_besselJ1 '(' exp ')'      { $$ = new Foam::scalarField(Foam::j1(*$3)); delete $3; }
        | TOKEN_besselY0 '(' exp ')'      { $$ = new Foam::scalarField(Foam::y0(*$3)); delete $3; }
        | TOKEN_besselY1 '(' exp ')'      { $$ = new Foam::scalarField(Foam::y1(*$3)); delete $3; }
        | TOKEN_sign '(' exp ')'          { $$ = new Foam::scalarField(Foam::sign(*$3)); delete $3; }
        | TOKEN_pos '(' exp ')'           { $$ = new Foam::scalarField(Foam::pos(*$3)); delete $3; }
        | TOKEN_neg '(' exp ')'           { $$ = new Foam::scalarField(Foam::neg(*$3)); delete $3; }
        | TOKEN_mag '(' exp ')'       { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' vexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' texp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' yexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' hexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_tr '(' texp ')'       { $$ = new Foam::scalarField( Foam::tr(*$3) ); delete $3; }
        | TOKEN_tr '(' yexp ')'       { $$ = new Foam::scalarField( Foam::tr(*$3) ); delete $3; }
        | TOKEN_tr '(' hexp ')'       { $$ = new Foam::scalarField( Foam::tr(*$3) ); delete $3; }
        | TOKEN_det '(' texp ')'      { $$ = new Foam::scalarField( Foam::det(*$3) ); delete $3; }
        | TOKEN_det '(' yexp ')'      { $$ = new Foam::scalarField( Foam::det(*$3) ); delete $3; }
        | TOKEN_det '(' hexp ')'      { $$ = new Foam::scalarField( Foam::det(*$3) ); delete $3; }
        | vexp '.' 'x'            { $$ = new Foam::scalarField($1->component(0)); delete $1; }
        | vexp '.' 'y'            { $$ = new Foam::scalarField($1->component(1)); delete $1; }
        | vexp '.' 'z'            { $$ = new Foam::scalarField($1->component(2)); delete $1; }
        | texp '.' TOKEN_xx       { $$ = new Foam::scalarField($1->component(0)); delete $1; }
        | texp '.' TOKEN_xy       { $$ = new Foam::scalarField($1->component(1)); delete $1; }
        | texp '.' TOKEN_xz       { $$ = new Foam::scalarField($1->component(2)); delete $1; }
        | texp '.' TOKEN_yx       { $$ = new Foam::scalarField($1->component(3)); delete $1; }
        | texp '.' TOKEN_yy       { $$ = new Foam::scalarField($1->component(4)); delete $1; }
        | texp '.' TOKEN_yz       { $$ = new Foam::scalarField($1->component(5)); delete $1; }
        | texp '.' TOKEN_zx       { $$ = new Foam::scalarField($1->component(6)); delete $1; }
        | texp '.' TOKEN_zy       { $$ = new Foam::scalarField($1->component(7)); delete $1; }
        | texp '.' TOKEN_zz       { $$ = new Foam::scalarField($1->component(8)); delete $1; }
        | yexp '.' TOKEN_xx       { $$ = new Foam::scalarField($1->component(0)); delete $1; }
        | yexp '.' TOKEN_xy       { $$ = new Foam::scalarField($1->component(1)); delete $1; }
        | yexp '.' TOKEN_xz       { $$ = new Foam::scalarField($1->component(2)); delete $1; }
        | yexp '.' TOKEN_yy       { $$ = new Foam::scalarField($1->component(3)); delete $1; }
        | yexp '.' TOKEN_yz       { $$ = new Foam::scalarField($1->component(4)); delete $1; }
        | yexp '.' TOKEN_zz       { $$ = new Foam::scalarField($1->component(5)); delete $1; }
        | hexp '.' TOKEN_ii       { $$ = new Foam::scalarField($1->component(0)); delete $1; }
        | lexp '?' exp ':' exp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
        | TOKEN_pi { $$ = driver.makeField(M_PI); }
        | TOKEN_id '(' ')'                         { $$ = driver.makeIdField(); }
        | TOKEN_cpu '(' ')'       { $$ = driver.makeField(Foam::scalar(Foam::Pstream::myProcNo())); }
        | TOKEN_rand '(' ')'        { $$ = driver.makeRandomField(); }
        | TOKEN_randNormal '(' ')'        { $$ = driver.makeGaussRandomField(); }
        | TOKEN_deltaT '(' ')'   { $$ = driver.makeField(driver.runTime().deltaT().value()); }
        | TOKEN_time '(' ')'   { $$ = driver.makeField(driver.runTime().time().value()); }
//        | TOKEN_toFace '(' pexp ')'        { $$ = driver.toFace(*$3); delete $3;}
        | TOKEN_area '(' ')'        { $$ = driver.makeFaceAreaMagField(); }
        | TOKEN_volume '(' ')'        { $$ = driver.makeCellVolumeField(); }
	| TOKEN_SID		{ 
            $$=driver.getScalarField(*$1);delete $1;
				}
	| TOKEN_LINE		{ 
            $$=driver.getLine(*$1,driver.runTime().time().value());delete $1;
				}
;

texp:   tensor                  { $$ = $1; }
        | texp '+' texp 		{ $$ = new Foam::tensorField(*$1 + *$3); delete $1; delete $3; }
        | texp '+' yexp 		{ $$ = new Foam::tensorField(*$1 + *$3); delete $1; delete $3; }
        | texp '+' hexp 		{ $$ = new Foam::tensorField(*$1 + *$3); delete $1; delete $3; }
        | yexp '+' texp 		{ $$ = new Foam::tensorField(*$1 + *$3); delete $1; delete $3; }
        | hexp '+' texp 		{ $$ = new Foam::tensorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' texp 		        { $$ = new Foam::tensorField(*$1 * *$3); delete $1; delete $3; }
        | texp '*' exp 		        { $$ = new Foam::tensorField(*$1 * *$3); delete $1; delete $3; }
        | vexp '*' vexp 	        { $$ = new Foam::tensorField(*$1 * *$3); delete $1; delete $3; }
        | texp '&' texp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | yexp '&' texp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | texp '&' yexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | hexp '&' texp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | texp '&' hexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | texp '/' exp 		        { $$ = new Foam::tensorField(*$1 / *$3); delete $1; delete $3; }
        | texp '-' texp 		{ $$ = new Foam::tensorField(*$1 - *$3); delete $1; delete $3;}
        | texp '-' yexp 		{ $$ = new Foam::tensorField(*$1 - *$3); delete $1; delete $3;}
        | texp '-' hexp 		{ $$ = new Foam::tensorField(*$1 - *$3); delete $1; delete $3;}
        | yexp '-' texp 		{ $$ = new Foam::tensorField(*$1 - *$3); delete $1; delete $3;}
        | hexp '-' texp 		{ $$ = new Foam::tensorField(*$1 - *$3); delete $1; delete $3;}
        | '-' texp %prec TOKEN_NEG 	        { $$ = new Foam::tensorField(-*$2); delete $2; }
        | '(' texp ')'		        { $$ = $2; }  
        | TOKEN_skew '(' texp ')'       { $$ = new Foam::tensorField( Foam::skew(*$3) ); delete $3; }
        | TOKEN_inv '(' texp ')'       { $$ = new Foam::tensorField( Foam::inv(*$3) ); delete $3; }
        | TOKEN_dev '(' texp ')'       { $$ = new Foam::tensorField( Foam::dev(*$3) ); delete $3; }
        | lexp '?' texp ':' texp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toFace '(' ptexp ')'        { $$ = driver.toFace(*$3); delete $3; }
        | TOKEN_TID {
            $$=driver.getTensorField(*$1); delete $1;
                    }
;

yexp:   symmTensor                  { $$ = $1; }
        | yexp '+' yexp 		{ $$ = new Foam::symmTensorField(*$1 + *$3); delete $1; delete $3; }
        | yexp '+' hexp 		{ $$ = new Foam::symmTensorField(*$1 + *$3); delete $1; delete $3; }
        | hexp '+' yexp 		{ $$ = new Foam::symmTensorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' yexp 		        { $$ = new Foam::symmTensorField(*$1 * *$3); delete $1; delete $3; }
        | yexp '*' exp 		        { $$ = new Foam::symmTensorField(*$1 * *$3); delete $1; delete $3; }
        | yexp '&' yexp 		{ $$ = new Foam::symmTensorField(*$1 & *$3); delete $1; delete $3; }
        | hexp '&' yexp 		{ $$ = new Foam::symmTensorField(*$1 & *$3); delete $1; delete $3; }
        | yexp '&' hexp 		{ $$ = new Foam::symmTensorField(*$1 & *$3); delete $1; delete $3; }
        | yexp '/' exp 		        { $$ = new Foam::symmTensorField(*$1 / *$3); delete $1; delete $3; }
        | yexp '-' yexp 		{ $$ = new Foam::symmTensorField(*$1 - *$3); delete $1; delete $3;}
        | yexp '-' hexp 		{ $$ = new Foam::symmTensorField(*$1 - *$3); delete $1; delete $3;}
        | hexp '-' yexp 		{ $$ = new Foam::symmTensorField(*$1 - *$3); delete $1; delete $3;}
        | '-' yexp %prec TOKEN_NEG 	        { $$ = new Foam::symmTensorField(-*$2); delete $2; }
        | '(' yexp ')'		        { $$ = $2; }  
        | TOKEN_symm '(' yexp ')'       { $$ = new Foam::symmTensorField( Foam::symm(*$3) ); delete $3; }
        | TOKEN_symm '(' texp ')'       { $$ = new Foam::symmTensorField( Foam::symm(*$3) ); delete $3; }
        | TOKEN_inv '(' yexp ')'       { $$ = new Foam::symmTensorField( Foam::inv(*$3) ); delete $3; }
        | lexp '?' yexp ':' yexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toFace '(' pyexp ')'        { $$ = driver.toFace(*$3); delete $3; }
        | TOKEN_YID {
            $$=driver.getSymmTensorField(*$1); delete $1;
                    }
;

hexp:   sphericalTensor                  { $$ = $1; }
        | hexp '+' hexp 		{ $$ = new Foam::sphericalTensorField(*$1 + *$3); delete $1; delete $3; }
        | exp '*' hexp 		        { $$ = new Foam::sphericalTensorField(*$1 * *$3); delete $1; delete $3; }
        | hexp '*' exp 		        { $$ = new Foam::sphericalTensorField(*$1 * *$3); delete $1; delete $3; }
        | hexp '&' hexp 		{ $$ = new Foam::sphericalTensorField(*$1 & *$3); delete $1; delete $3; }
        | hexp '/' exp 		        { $$ = new Foam::sphericalTensorField(*$1 / *$3); delete $1; delete $3; }
        | hexp '-' hexp 		{ $$ = new Foam::sphericalTensorField(*$1 - *$3); delete $1; delete $3;}
        | '-' hexp %prec TOKEN_NEG 	        { $$ = new Foam::sphericalTensorField(-*$2); delete $2; }
        | '(' hexp ')'		        { $$ = $2; }  
        | TOKEN_inv '(' hexp ')'       { $$ = new Foam::sphericalTensorField( Foam::inv(*$3) ); delete $3; }
        | lexp '?' hexp ':' hexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toFace '(' phexp ')'        { $$ = driver.toFace(*$3); delete $3; }
        | TOKEN_HID {
            $$=driver.getSphericalTensorField(*$1); delete $1;
                    }
;


lexp: TOKEN_TRUE   { $$ = driver.makeField(true); }
    | TOKEN_FALSE  { $$ = driver.makeField(false); }
    | exp '<' exp  { $$ = driver.doCompare($1,std::less<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp '>' exp  { $$ = driver.doCompare($1,std::greater<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_LEQ exp  { $$ = driver.doCompare($1,std::less_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_GEQ exp  { $$ = driver.doCompare($1,std::greater_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_EQ exp  { $$ = driver.doCompare($1,std::equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | exp TOKEN_NEQ exp  { $$ = driver.doCompare($1,std::not_equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '(' lexp ')'		{ $$ = $2; }
    | lexp TOKEN_AND lexp  { $$ = driver.doLogicalOp($1,std::logical_and<Foam::scalar>(),$3);  delete $1; delete $3; }
    | lexp TOKEN_OR lexp   { $$ = driver.doLogicalOp($1,std::logical_or<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '!' lexp %prec TOKEN_NOT { $$ = driver.doLogicalNot($2); delete $2; }
//    | TOKEN_LID		{ 
//            $$=driver.getField<Foam::bool>(*$1);delete $1;
//    }
;

vector: TOKEN_VECTOR '(' exp ',' exp ',' exp ')' {     $$ = driver.composeVectorField($3,$5,$7);  delete $3; delete $5; delete $7;}
;

tensor: TOKEN_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {     $$ = driver.composeTensorField($3,$5,$7,$9,$11,$13,$15,$17,$19);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13; delete $15; delete $17; delete $19;}

symmTensor: TOKEN_SYMM_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {     $$ = driver.composeSymmTensorField($3,$5,$7,$9,$11,$13);  delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;}

sphericalTensor: TOKEN_SPHERICAL_TENSOR '(' exp ')' {     $$ = driver.composeSphericalTensorField($3);  delete $3; }

pvexp:  pvexp '+' pvexp 		{ $$ = new Foam::vectorField(*$1 + *$3); delete $1; delete $3; }
        | pexp '*' pvexp 		        { $$ = new Foam::vectorField(*$1 * *$3); delete $1; delete $3; }
        | pvexp '*' pexp 		        { $$ = new Foam::vectorField(*$1 * *$3); delete $1; delete $3; }
        | ptexp '&' pvexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '&' ptexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | pyexp '&' pvexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '&' pyexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | phexp '&' pvexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '&' phexp 		{ $$ = new Foam::vectorField(*$1 & *$3); delete $1; delete $3; }
        | pvexp '/' pexp 		        { $$ = new Foam::vectorField(*$1 / *$3); delete $1; delete $3; }
        | pvexp '^' pvexp 		{ $$ = new Foam::vectorField(*$1 ^ *$3); delete $1; delete $3; }
        | pvexp '-' pvexp 		{ $$ = new Foam::vectorField(*$1 - *$3); delete $1; delete $3;}
        | '-' pvexp %prec TOKEN_NEG 	        { $$ = new Foam::vectorField(-*$2); delete $2; }
        | '(' pvexp ')'		        { $$ = $2; }  
        | plexp '?' pvexp ':' pvexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_points '(' ')'        { $$ = driver.makePointField(); }
//        | TOKEN_toPoint '(' vexp ')'        { $$ = driver.toPoint(*$3); delete $3;}
        | TOKEN_PVID {
            $$=driver.getVectorField(*$1); delete $1;
                    }
;
  
pexp:   pexp '+' pexp 		{ $$ = new Foam::scalarField(*$1 + *$3); delete $1; delete $3; }
        | pexp '-' pexp 		{ $$ = new Foam::scalarField(*$1 - *$3); delete $1; delete $3; }
        | pexp '*' pexp 		{ $$ = new Foam::scalarField(*$1 * *$3); delete $1; delete $3; }
        | pexp '%' pexp 		{ $$ = driver.makeModuloField(*$1,*$3); delete $1; delete $3; }
        | pexp '/' pexp 		{ $$ = new Foam::scalarField(*$1 / *$3); delete $1; delete $3; }
        | TOKEN_pow '(' pexp ',' scalar ')'		{ $$ = new Foam::scalarField(Foam::pow(*$3, $5)); delete $3; }
        | TOKEN_log '(' pexp ')'       { $$ = new Foam::scalarField(Foam::log(*$3)); delete $3; }
        | TOKEN_exp '(' pexp ')'       { $$ = new Foam::scalarField(Foam::exp(*$3)); delete $3; }
        | pvexp '&' pvexp 	{ $$ = new Foam::scalarField(*$1 & *$3); delete $1; delete $3;}
        | '-' pexp %prec TOKEN_NEG 	{ $$ = new Foam::scalarField(-*$2); delete $2; }
	| '(' pexp ')'		{ $$ = $2; }
        | TOKEN_sqr '(' pexp ')'       { $$ = new Foam::scalarField(Foam::sqr(*$3)); delete $3; }
        | TOKEN_sqrt '(' pexp ')'       { $$ = new Foam::scalarField(Foam::sqrt(*$3)); delete $3; }
        | TOKEN_sin '(' pexp ')'       { $$ = new Foam::scalarField(Foam::sin(*$3)); delete $3; }
        | TOKEN_cos '(' pexp ')'       { $$ = new Foam::scalarField(Foam::cos(*$3)); delete $3; }
        | TOKEN_tan '(' pexp ')'       { $$ = new Foam::scalarField(Foam::tan(*$3)); delete $3; }
        | TOKEN_log10 '(' pexp ')'         { $$ = new Foam::scalarField(Foam::log10(*$3)); delete $3; }
        | TOKEN_asin '(' pexp ')'          { $$ = new Foam::scalarField(Foam::asin(*$3)); delete $3; }
        | TOKEN_acos '(' pexp ')'          { $$ = new Foam::scalarField(Foam::acos(*$3)); delete $3; }
        | TOKEN_atan '(' pexp ')'          { $$ = new Foam::scalarField(Foam::atan(*$3)); delete $3; }
        | TOKEN_sinh '(' pexp ')'          { $$ = new Foam::scalarField(Foam::sinh(*$3)); delete $3; }
        | TOKEN_cosh '(' pexp ')'          { $$ = new Foam::scalarField(Foam::cosh(*$3)); delete $3; }
        | TOKEN_tanh '(' pexp ')'          { $$ = new Foam::scalarField(Foam::tanh(*$3)); delete $3; }
        | TOKEN_asinh '(' pexp ')'         { $$ = new Foam::scalarField(Foam::asinh(*$3)); delete $3; }
        | TOKEN_acosh '(' pexp ')'         { $$ = new Foam::scalarField(Foam::acosh(*$3)); delete $3; }
        | TOKEN_atanh '(' pexp ')'         { $$ = new Foam::scalarField(Foam::atanh(*$3)); delete $3; }
        | TOKEN_erf '(' pexp ')'           { $$ = new Foam::scalarField(Foam::erf(*$3)); delete $3; }
        | TOKEN_erfc '(' pexp ')'          { $$ = new Foam::scalarField(Foam::erfc(*$3)); delete $3; }
        | TOKEN_lgamma '(' pexp ')'        { $$ = new Foam::scalarField(Foam::lgamma(*$3)); delete $3; }
        | TOKEN_besselJ0 '(' pexp ')'      { $$ = new Foam::scalarField(Foam::j0(*$3)); delete $3; }
        | TOKEN_besselJ1 '(' pexp ')'      { $$ = new Foam::scalarField(Foam::j1(*$3)); delete $3; }
        | TOKEN_besselY0 '(' pexp ')'      { $$ = new Foam::scalarField(Foam::y0(*$3)); delete $3; }
        | TOKEN_besselY1 '(' pexp ')'      { $$ = new Foam::scalarField(Foam::y1(*$3)); delete $3; }
        | TOKEN_sign '(' pexp ')'          { $$ = new Foam::scalarField(Foam::sign(*$3)); delete $3; }
        | TOKEN_pos '(' pexp ')'           { $$ = new Foam::scalarField(Foam::pos(*$3)); delete $3; }
        | TOKEN_neg '(' pexp ')'           { $$ = new Foam::scalarField(Foam::neg(*$3)); delete $3; }
        | TOKEN_mag '(' pexp ')'       { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' pvexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' ptexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' pyexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | TOKEN_mag '(' phexp ')'      { $$ = new Foam::scalarField(Foam::mag(*$3)); delete $3; }
        | pvexp '.' 'x'            { $$ = new Foam::scalarField($1->component(0)); delete $1; }
        | pvexp '.' 'y'            { $$ = new Foam::scalarField($1->component(1)); delete $1; }
        | pvexp '.' 'z'            { $$ = new Foam::scalarField($1->component(2)); delete $1; }
        | plexp '?' pexp ':' pexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toPoint '(' exp ')'        { $$ = driver.toPoint(*$3); delete $3;}
	| TOKEN_PSID		{ 
            $$=driver.getScalarField(*$1);delete $1;
				}
;

ptexp:  ptexp '+' ptexp 		{ $$ = new Foam::tensorField(*$1 + *$3); delete $1; delete $3; }
        | pexp '*' ptexp 		        { $$ = new Foam::tensorField(*$1 * *$3); delete $1; delete $3; }
        | ptexp '*' pexp 		        { $$ = new Foam::tensorField(*$1 * *$3); delete $1; delete $3; }
        | pvexp '*' pvexp 	        { $$ = new Foam::tensorField(*$1 * *$3); delete $1; delete $3; }
        | ptexp '&' ptexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | pyexp '&' ptexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | ptexp '&' pyexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | phexp '&' ptexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | ptexp '&' phexp 		{ $$ = new Foam::tensorField(*$1 & *$3); delete $1; delete $3; }
        | ptexp '/' pexp 		        { $$ = new Foam::tensorField(*$1 / *$3); delete $1; delete $3; }
        | ptexp '-' ptexp 		{ $$ = new Foam::tensorField(*$1 - *$3); delete $1; delete $3;}
        | '-' ptexp %prec TOKEN_NEG 	        { $$ = new Foam::tensorField(-*$2); delete $2; }
        | '(' ptexp ')'		        { $$ = $2; }  
        | plexp '?' ptexp ':' ptexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toPoint '(' texp ')'        { $$ = driver.toPoint(*$3); delete $3;}
        | TOKEN_PTID {
            $$=driver.getTensorField(*$1); delete $1;
                    }
;
  
pyexp:  pyexp '+' pyexp 		{ $$ = new Foam::symmTensorField(*$1 + *$3); delete $1; delete $3; }
        | pexp '*' pyexp 		        { $$ = new Foam::symmTensorField(*$1 * *$3); delete $1; delete $3; }
        | pyexp '*' pexp 		        { $$ = new Foam::symmTensorField(*$1 * *$3); delete $1; delete $3; }
        | pyexp '&' pyexp 		{ $$ = new Foam::symmTensorField(*$1 & *$3); delete $1; delete $3; }
        | phexp '&' pyexp 		{ $$ = new Foam::symmTensorField(*$1 & *$3); delete $1; delete $3; }
        | pyexp '&' phexp 		{ $$ = new Foam::symmTensorField(*$1 & *$3); delete $1; delete $3; }
        | pyexp '/' pexp 		        { $$ = new Foam::symmTensorField(*$1 / *$3); delete $1; delete $3; }
        | pyexp '-' pyexp 		{ $$ = new Foam::symmTensorField(*$1 - *$3); delete $1; delete $3;}
        | '-' pyexp %prec TOKEN_NEG 	        { $$ = new Foam::symmTensorField(-*$2); delete $2; }
        | '(' pyexp ')'		        { $$ = $2; }  
        | plexp '?' pyexp ':' pyexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toPoint '(' yexp ')'        { $$ = driver.toPoint(*$3); delete $3;}
        | TOKEN_PYID {
            $$=driver.getSymmTensorField(*$1); delete $1;
                    }
;
  
phexp:  phexp '+' phexp 		{ $$ = new Foam::sphericalTensorField(*$1 + *$3); delete $1; delete $3; }
        | pexp '*' phexp 		        { $$ = new Foam::sphericalTensorField(*$1 * *$3); delete $1; delete $3; }
        | phexp '*' pexp 		        { $$ = new Foam::sphericalTensorField(*$1 * *$3); delete $1; delete $3; }
        | phexp '&' phexp 		{ $$ = new Foam::sphericalTensorField(*$1 & *$3); delete $1; delete $3; }
        | phexp '/' pexp 		        { $$ = new Foam::sphericalTensorField(*$1 / *$3); delete $1; delete $3; }
        | phexp '-' phexp 		{ $$ = new Foam::sphericalTensorField(*$1 - *$3); delete $1; delete $3;}
        | '-' phexp %prec TOKEN_NEG 	        { $$ = new Foam::sphericalTensorField(-*$2); delete $2; }
        | '(' phexp ')'		        { $$ = $2; }  
        | plexp '?' phexp ':' phexp        { $$ = driver.doConditional($1,$3,$5); delete $1; delete $3; delete $5; }
//        | TOKEN_toPoint '(' hexp ')'        { $$ = driver.toPoint(*$3); delete $3;}
        | TOKEN_PHID {
            $$=driver.getSphericalTensorField(*$1); delete $1;
                    }
;
  
plexp: pexp '<' pexp  { $$ = driver.doCompare($1,std::less<Foam::scalar>(),$3);  delete $1; delete $3; }
    | pexp '>' pexp  { $$ = driver.doCompare($1,std::greater<Foam::scalar>(),$3);  delete $1; delete $3; }
    | pexp TOKEN_LEQ pexp  { $$ = driver.doCompare($1,std::less_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | pexp TOKEN_GEQ pexp  { $$ = driver.doCompare($1,std::greater_equal<Foam::scalar>(),$3);  delete $1; delete $3; }
    | pexp TOKEN_EQ pexp  { $$ = driver.doCompare($1,std::equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | pexp TOKEN_NEQ pexp  { $$ = driver.doCompare($1,std::not_equal_to<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '(' plexp ')'		{ $$ = $2; }
    | plexp TOKEN_AND plexp  { $$ = driver.doLogicalOp($1,std::logical_and<Foam::scalar>(),$3);  delete $1; delete $3; }
    | plexp TOKEN_OR plexp   { $$ = driver.doLogicalOp($1,std::logical_or<Foam::scalar>(),$3);  delete $1; delete $3; }
    | '!' plexp %prec TOKEN_NOT { $$ = driver.doLogicalNot($2); delete $2; }
//    | TOKEN_PLID		{ 
//            $$=driver.getField<Foam::bool>(*$1);delete $1;
//    }
;

%%

void parserSubset::SubsetValueExpressionParser::error (const parserSubset::SubsetValueExpressionParser::location_type& l,const std::string& m)
{
     driver.error (l, m);
}
