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
    2011-2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

%skeleton "lalr1.cc"
/* %require "2.1a" */
%defines
%define "parser_class_name" "FaFieldValueExpressionParser"

// make reentrant to allow sub-parsers
%pure-parser

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

#include "FaFieldValuePluginFunction.H"

#include "swak.H"
%}

%name-prefix="parserFaField"

%parse-param {void * scanner}
%parse-param { FaFieldValueExpressionDriver& driver }
%parse-param { int start_token }
%parse-param { int numberOfFunctionChars }
%lex-param {void * scanner}
%lex-param { FaFieldValueExpressionDriver& driver }
%lex-param { int &start_token }
%lex-param { int &numberOfFunctionChars }

%locations
%initial-action
{
	     // Initialize the initial location.
	     //     @$.begin.filename = @$.end.filename = &driver.file;
    numberOfFunctionChars=0;
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
    Foam::word *name;
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

namespace Foam {
template<class T>
autoPtr<T> FaFieldValueExpressionDriver::evaluatePluginFunction(
    const word &name,
    const parserFaField::location &loc,
    int &scanned
) {
    if(debug || traceParsing()) {
        Info << "Excuting plugin-function " << name << " ( returning type "
            << pTraits<T>::typeName << ") on " << this->content() << " position "
            << loc.end.column-1 << endl;
    }

    autoPtr<FaFieldValuePluginFunction> theFunction(
        FaFieldValuePluginFunction::New(
            *this,
            name
        )
    );

    //    scanned+=1;

    autoPtr<T> result(
        theFunction->evaluate<T>(
            this->content().substr(
                loc.end.column-1
            ),
            scanned
        ).ptr()
    );

    if(debug || traceParsing()) {
        Info << "Scanned: " << scanned << endl;
    }

    return result;
}
}

%}

%token <name>   TOKEN_LINE  "timeline"
%token <name>   TOKEN_LOOKUP  "lookup"
%token <name>   TOKEN_LOOKUP2D  "lookup2D"
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
%token <name>   TOKEN_FUNCTION_SID   "F_scalarID"
%token <name>   TOKEN_FUNCTION_FSID   "F_faceScalarID"
%token <name>   TOKEN_FUNCTION_VID   "F_vectorID"
%token <name>   TOKEN_FUNCTION_FVID   "F_faceVectorID"
%token <name>   TOKEN_FUNCTION_TID   "F_tensorID"
%token <name>   TOKEN_FUNCTION_FTID   "F_faceTensorID"
%token <name>   TOKEN_FUNCTION_YID   "F_symmTensorID"
%token <name>   TOKEN_FUNCTION_FYID   "F_faceSymmTensorID"
%token <name>   TOKEN_FUNCTION_HID   "F_sphericalTensorID"
%token <name>   TOKEN_FUNCTION_FHID   "F_faceSphericalTensorID"
%token <name>   TOKEN_FUNCTION_LID   "F_logicalID"
%token <name>   TOKEN_FUNCTION_FLID   "F_faceLogicalID"
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

%type  <sfield> evaluateScalarFunction;
%type <fsfield> evaluateEdgeScalarFunction;
%type  <vfield> evaluateVectorFunction;
%type <fvfield> evaluateEdgeVectorFunction;
%type  <tfield> evaluateTensorFunction;
%type <ftfield> evaluateEdgeTensorFunction;
%type  <yfield> evaluateSymmTensorFunction;
%type <fyfield> evaluateEdgeSymmTensorFunction;
%type  <hfield> evaluateSphericalTensorFunction;
%type <fhfield> evaluateEdgeSphericalTensorFunction;
%type  <sfield> evaluateLogicalFunction;
%type <fsfield> evaluateEdgeLogicalFunction;

%token START_DEFAULT
%token START_AREA_SCALAR_COMMA
%token START_AREA_SCALAR_CLOSE
%token START_AREA_VECTOR_COMMA
%token START_AREA_VECTOR_CLOSE
%token START_AREA_TENSOR_COMMA
%token START_AREA_TENSOR_CLOSE
%token START_AREA_YTENSOR_COMMA
%token START_AREA_YTENSOR_CLOSE
%token START_AREA_HTENSOR_COMMA
%token START_AREA_HTENSOR_CLOSE
%token START_AREA_LOGICAL_COMMA
%token START_AREA_LOGICAL_CLOSE
%token START_EDGE_SCALAR_COMMA
%token START_EDGE_SCALAR_CLOSE
%token START_EDGE_VECTOR_COMMA
%token START_EDGE_VECTOR_CLOSE
%token START_EDGE_TENSOR_COMMA
%token START_EDGE_TENSOR_CLOSE
%token START_EDGE_YTENSOR_COMMA
%token START_EDGE_YTENSOR_CLOSE
%token START_EDGE_HTENSOR_COMMA
%token START_EDGE_HTENSOR_CLOSE
%token START_EDGE_LOGICAL_COMMA
%token START_EDGE_LOGICAL_CLOSE

%token START_CLOSE_ONLY
%token START_COMMA_ONLY

%token TOKEN_LAST_FUNCTION_CHAR
%token TOKEN_IN_FUNCTION_CHAR

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

%token TOKEN_unitTensor
%token TOKEN_pi
%token TOKEN_rand
%token TOKEN_randFixed
%token TOKEN_id
%token TOKEN_cpu
%token TOKEN_weight
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
%token TOKEN_outputTime

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
%token TOKEN_sph
%token TOKEN_twoSymm
%token TOKEN_dev2
%token TOKEN_eigenValues
%token TOKEN_eigenVectors

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
%left TOKEN_NEG TOKEN_NOT TOKEN_HODGE
// %right '^'
%left '.'

%destructor          {} <val> <integer>
%destructor          { delete $$; } <name> <vec> <ten> <yten> <hten>
       <vfield> <sfield> <tfield> <yfield> <hfield>
       <fvfield> <fsfield> <ftfield> <fyfield> <fhfield>

%printer             { debug_stream () << $$; } <val> <integer>
%printer             { debug_stream () << *$$; } <name>
%printer             {
    Foam::OStringStream buff;
    buff << *$$; debug_stream () << buff.str().c_str();
} <vec> <ten> <yten> <hten>
%printer             { debug_stream () << $$->name().c_str(); }
       <vfield> <sfield> <tfield> <yfield> <hfield>
       <fvfield> <fsfield> <ftfield> <fyfield> <fhfield>

%%
%start switch_start;


switch_start: switch_expr
              { driver.parserLastPos()=@1.end.column; }
;

switch_expr:      START_DEFAULT unit
                | START_AREA_SCALAR_COMMA exp ','
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_SCALAR_CLOSE exp ')'
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_VECTOR_COMMA vexp ','
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_VECTOR_CLOSE vexp ')'
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_TENSOR_COMMA texp ','
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_TENSOR_CLOSE texp ')'
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_YTENSOR_COMMA yexp ','
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_YTENSOR_CLOSE yexp ')'
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_HTENSOR_COMMA hexp ','
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_HTENSOR_CLOSE hexp ')'
                  {
                      driver.setResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_LOGICAL_COMMA lexp ','
                  {
                      driver.setLogicalResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_AREA_LOGICAL_CLOSE lexp ')'
                  {
                      driver.setLogicalResult($2,false);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_SCALAR_COMMA fsexp ','
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_SCALAR_CLOSE fsexp ')'
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_VECTOR_COMMA fvexp ','
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_VECTOR_CLOSE fvexp ')'
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_TENSOR_COMMA ftexp ','
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_TENSOR_CLOSE ftexp ')'
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_YTENSOR_COMMA fyexp ','
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_YTENSOR_CLOSE fyexp ')'
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_HTENSOR_COMMA fhexp ','
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_HTENSOR_CLOSE fhexp ')'
                  {
                      driver.setResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_LOGICAL_COMMA flexp ','
                  {
                      driver.setLogicalResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
                | START_EDGE_LOGICAL_CLOSE flexp ')'
                  {
                      driver.setLogicalResult($2,true);
                      driver.parserLastPos()=@3.end.column-1;
                      YYACCEPT;
                  }
;

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

eatCharactersSwitch: /* empty rule */{ driver.startEatCharacters(); }
;

restOfFunction:    TOKEN_LAST_FUNCTION_CHAR
                   | TOKEN_IN_FUNCTION_CHAR restOfFunction;

vexp:   vector                                    { $$ = $1; }
        | vexp '+' vexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 + *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | exp '*' vexp 		                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 * *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '*' exp 		                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 * *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | texp '&' vexp 	                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 & *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '&' texp 	                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 & *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | yexp '&' vexp 	                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 & *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '&' yexp 	                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 & *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | hexp '&' vexp 	                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 & *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '&' hexp 	                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 & *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '/' exp 		                  {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 / *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '^' vexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 ^ *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '-' vexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaVectorField(*$1 - *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | '-' vexp %prec TOKEN_NEG                {
            $$ = new Foam::areaVectorField(-*$2);
            delete $2;
            driver.setCalculatedPatches(*$$);
          }
        | '*' texp %prec TOKEN_HODGE           {
            $$ = new Foam::areaVectorField(*(*$2));
            delete $2;
            driver.setCalculatedPatches(*$$);
          }
        | '*' yexp %prec TOKEN_HODGE           {
            $$ = new Foam::areaVectorField(*(*$2));
            delete $2;
            driver.setCalculatedPatches(*$$);
          }
        | '(' vexp ')'		                  { $$ = $2; }
        | TOKEN_eigenValues '(' texp ')'       {
            $$ = new Foam::areaVectorField(Foam::eigenValues(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_eigenValues '(' yexp ')'       {
            $$ = new Foam::areaVectorField(Foam::eigenValues(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch  TOKEN_x                              {
            $$ = driver.makeVectorField(
                $1->component(Foam::tensor::XX)(),
                $1->component(Foam::tensor::XY)(),
                $1->component(Foam::tensor::XZ)()
            ).ptr();
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch  TOKEN_y                              {
            $$ = driver.makeVectorField(
                $1->component(Foam::tensor::YX)(),
                $1->component(Foam::tensor::YY)(),
                $1->component(Foam::tensor::YZ)()
            ).ptr();
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch  TOKEN_z                              {
            $$ = driver.makeVectorField(
                $1->component(Foam::tensor::ZX)(),
                $1->component(Foam::tensor::ZY)(),
                $1->component(Foam::tensor::ZZ)()
            ).ptr();
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_diag '(' texp ')'       {
            $$ = driver.makeVectorField(
                $3->component(Foam::tensor::XX)(),
                $3->component(Foam::tensor::YY)(),
                $3->component(Foam::tensor::ZZ)()
            ).ptr();
            delete $3;
          }
        | TOKEN_diag '(' yexp ')'       {
            $$ = driver.makeVectorField(
                $3->component(Foam::symmTensor::XX)(),
                $3->component(Foam::symmTensor::YY)(),
                $3->component(Foam::symmTensor::ZZ)()
            ).ptr();
            delete $3;
          }
        | lexp '?' vexp ':' vexp                  {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_position '(' ')'                  {
            $$ = driver.makePositionField().ptr();
          }
        | TOKEN_laplacian '(' fsexp ',' vexp ')'  {
            $$ = new Foam::areaVectorField(Foam::fac::laplacian(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_faceAverage '(' fvexp ')'         {
            $$ = new Foam::areaVectorField(Foam::fac::average(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_integrate '(' fvexp ')'           {
            $$ = new Foam::areaVectorField(Foam::fac::edgeIntegrate(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_surfSum '(' fvexp ')'             {
            $$ = new Foam::areaVectorField(Foam::fac::edgeSum(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' vexp ',' vexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_max '(' vexp ',' vexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' vexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_minPosition '(' exp ')'           {
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.aMesh().areaCentres()
                )
            ).ptr();
            delete $3;
        }
        | TOKEN_max '(' vexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_maxPosition '(' exp ')'           {
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.aMesh().areaCentres()
                )
            ).ptr();
            delete $3;
        }
        | TOKEN_sum '(' vexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' vexp ')'             {
            $$ = driver.makeConstantField<Foam::areaVectorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_grad '(' exp ')'                  {
            $$ = new Foam::areaVectorField(Foam::fac::grad(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_div '(' fsexp ',' vexp ')'        {
            $$ = new Foam::areaVectorField(Foam::fac::div(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | evaluateVectorFunction restOfFunction
        | TOKEN_VID                               {
            $$=driver.getField<Foam::areaVectorField>(*$1).ptr();
            delete $1;
          }
        | TOKEN_ddt '(' TOKEN_VID ')'             {
            $$ = Foam::fac::ddt(
                driver.getField<Foam::areaVectorField>(
                    *$3,true
                )()
            ).ptr();
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
        | TOKEN_oldTime '(' TOKEN_VID ')'         {
            $$=new Foam::areaVectorField(
                driver.getField<Foam::areaVectorField>(*$3,true)->oldTime()
            );
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
;

evaluateVectorFunction: TOKEN_FUNCTION_VID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::areaVectorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

fsexp:  TOKEN_surf '(' scalar ')'           {
            $$ = driver.makeConstantField<Foam::edgeScalarField>($3).ptr();
          }
        | fsexp '+' fsexp 		    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fsexp '*' fsexp 		    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fvexp '&' fvexp 		    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 & *$3);
            delete $1; delete $3;
          }
        | ftexp TOKEN_AND ftexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fyexp TOKEN_AND ftexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fhexp TOKEN_AND ftexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | ftexp TOKEN_AND fyexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fyexp TOKEN_AND fyexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fhexp TOKEN_AND fyexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | ftexp TOKEN_AND fhexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fyexp TOKEN_AND fhexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fhexp TOKEN_AND fhexp 	    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 && *$3);
            delete $1; delete $3;
          }
        | fsexp '/' fsexp 		    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 / *$3);
            delete $1; delete $3;
          }
        | fsexp '-' fsexp 		    {
            sameSize($1,$3);
            $$ = new Foam::edgeScalarField(*$1 - *$3);
            delete $1; delete $3;
          }
        | TOKEN_pow '(' fsexp ',' scalar ')'{
            $$ = new Foam::edgeScalarField(Foam::pow(*$3, $5));
            delete $3;
          }
        | TOKEN_pow '(' fsexp ',' fsexp ')'{
            $$ = new Foam::edgeScalarField(Foam::pow(*$3, *$5));
            delete $3; delete $5;
          }
        | TOKEN_log '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::log(*$3));
            delete $3;
          }
        | TOKEN_exp '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::exp(*$3));
            delete $3;
          }
        | TOKEN_sqr '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::sqr(*$3));
            delete $3;
          }
        | TOKEN_sqrt '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::sqrt(*$3));
            delete $3;
          }
        | TOKEN_sin '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::sin(*$3));
            delete $3;
          }
        | TOKEN_cos '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::cos(*$3));
            delete $3;
          }
        | TOKEN_tan '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::tan(*$3));
            delete $3;
          }
        | TOKEN_log10 '(' fsexp ')'         {
            $$ = new Foam::edgeScalarField(Foam::log10(*$3));
            delete $3;
          }
        | TOKEN_asin '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::asin(*$3));
            delete $3;
          }
        | TOKEN_acos '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::acos(*$3));
            delete $3;
          }
        | TOKEN_atan '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::atan(*$3));
            delete $3;
          }
        | TOKEN_sinh '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::sinh(*$3));
            delete $3;
          }
        | TOKEN_cosh '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::cosh(*$3));
            delete $3;
          }
        | TOKEN_tanh '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::tanh(*$3));
            delete $3;
          }
        | TOKEN_asinh '(' fsexp ')'         {
            $$ = new Foam::edgeScalarField(Foam::asinh(*$3));
            delete $3;
          }
        | TOKEN_acosh '(' fsexp ')'         {
            $$ = new Foam::edgeScalarField(Foam::acosh(*$3));
            delete $3;
          }
        | TOKEN_atanh '(' fsexp ')'         {
            $$ = new Foam::edgeScalarField(Foam::atanh(*$3));
            delete $3;
          }
        | TOKEN_erf '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::erf(*$3));
            delete $3;
          }
        | TOKEN_erfc '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::erfc(*$3));
            delete $3;
          }
        | TOKEN_lgamma '(' fsexp ')'        {
            $$ = new Foam::edgeScalarField(Foam::lgamma(*$3));
            delete $3;
          }
        | TOKEN_besselJ0 '(' fsexp ')'      {
            $$ = new Foam::edgeScalarField(Foam::j0(*$3));
            delete $3;
          }
        | TOKEN_besselJ1 '(' fsexp ')'      {
            $$ = new Foam::edgeScalarField(Foam::j1(*$3));
            delete $3;
          }
        | TOKEN_besselY0 '(' fsexp ')'      {
            $$ = new Foam::edgeScalarField(Foam::y0(*$3));
            delete $3;
          }
        | TOKEN_besselY1 '(' fsexp ')'      {
            $$ = new Foam::edgeScalarField(Foam::y1(*$3));
            delete $3;
          }
        | TOKEN_sign '(' fsexp ')'          {
            $$ = new Foam::edgeScalarField(Foam::sign(*$3));
            delete $3;
          }
        | TOKEN_pos '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::pos(*$3));
            delete $3;
          }
        | TOKEN_neg '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::neg(*$3));
            delete $3;
          }
        | TOKEN_min '(' fsexp ',' fsexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_max '(' fsexp ',' fsexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_min '(' fsexp ')'           {
            $$ = driver.makeConstantField<Foam::edgeScalarField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' fsexp ')'           {
            $$ = driver.makeConstantField<Foam::edgeScalarField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' fsexp ')'           {
            $$ = driver.makeConstantField<Foam::edgeScalarField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' fsexp ')'       {
            $$ = driver.makeConstantField<Foam::edgeScalarField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | '-' fsexp %prec TOKEN_NEG         {
            $$ = new Foam::edgeScalarField(-*$2);
            delete $2;
          }
        | '(' fsexp ')'		            { $$ = $2; }
        | fvexp '.' vectorComponentSwitch TOKEN_x                     {
            $$ = new Foam::edgeScalarField($1->component(0));
            delete $1;
          }
        | fvexp '.' vectorComponentSwitch TOKEN_y                     {
            $$ = new Foam::edgeScalarField($1->component(1));
            delete $1;
          }
        | fvexp '.' vectorComponentSwitch TOKEN_z                     {
            $$ = new Foam::edgeScalarField($1->component(2));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_xx                {
            $$ = new Foam::edgeScalarField($1->component(0));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_xy                {
            $$ = new Foam::edgeScalarField($1->component(1));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_xz                {
            $$ = new Foam::edgeScalarField($1->component(2));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_yx                {
            $$ = new Foam::edgeScalarField($1->component(3));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_yy                {
            $$ = new Foam::edgeScalarField($1->component(4));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_yz                {
            $$ = new Foam::edgeScalarField($1->component(5));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_zx                {
            $$ = new Foam::edgeScalarField($1->component(6));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_zy                {
            $$ = new Foam::edgeScalarField($1->component(7));
            delete $1;
          }
        | ftexp '.' tensorComponentSwitch TOKEN_zz                {
            $$ = new Foam::edgeScalarField($1->component(8));
            delete $1;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_xx                {
            $$ = new Foam::edgeScalarField($1->component(0));
            delete $1;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_xy                {
            $$ = new Foam::edgeScalarField($1->component(1));
            delete $1;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_xz                {
            $$ = new Foam::edgeScalarField($1->component(2));
            delete $1;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_yy                {
            $$ = new Foam::edgeScalarField($1->component(3));
            delete $1;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_yz                {
            $$ = new Foam::edgeScalarField($1->component(4));
            delete $1;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_zz                {
            $$ = new Foam::edgeScalarField($1->component(5));
            delete $1;
          }
        | fhexp '.' tensorComponentSwitch TOKEN_ii                {
            $$ = new Foam::edgeScalarField($1->component(0));
            delete $1;
          }
        | flexp '?' fsexp ':' fsexp         {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
          }
        | TOKEN_mag '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::mag(*$3));
            delete $3;
          }
        | TOKEN_mag '(' fvexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::mag(*$3));
            delete $3;
          }
        | TOKEN_mag '(' ftexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::mag(*$3));
            delete $3;
          }
        | TOKEN_mag '(' fyexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::mag(*$3));
            delete $3;
          }
        | TOKEN_mag '(' fhexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::mag(*$3));
            delete $3;
          }
        | TOKEN_magSqr '(' fsexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::magSqr(*$3));
            delete $3;
          }
        | TOKEN_magSqr '(' fvexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::magSqr(*$3));
            delete $3;
          }
        | TOKEN_magSqr '(' ftexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::magSqr(*$3));
            delete $3;
          }
        | TOKEN_magSqr '(' fyexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::magSqr(*$3));
            delete $3;
          }
        | TOKEN_magSqr '(' fhexp ')'           {
            $$ = new Foam::edgeScalarField(Foam::magSqr(*$3));
            delete $3;
          }
        | TOKEN_length '(' ')'                {
            $$ = driver.makeLengthField().ptr();
          }
        | TOKEN_lnGrad '(' exp ')'          {
            $$ = new Foam::edgeScalarField(Foam::fac::lnGrad(*$3));
            delete $3;
          }
        | TOKEN_interpolate '(' exp ')'     {
            $$ = new Foam::edgeScalarField(Foam::fac::interpolate(*$3));
            delete $3;
          }
        | evaluateEdgeScalarFunction restOfFunction
        | TOKEN_FSID                        {
            $$ = driver.getField<Foam::edgeScalarField>(*$1).ptr();
            delete $1;
          }
        | TOKEN_oldTime '(' TOKEN_FSID ')'  {
            $$=new Foam::edgeScalarField(
                driver.getField<Foam::edgeScalarField>(*$3,true)->oldTime()
            );
            delete $3;
          }
;

evaluateEdgeScalarFunction: TOKEN_FUNCTION_FSID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::edgeScalarField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

fvexp:  fvector                            { $$ = $1; }
        | fvexp '+' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fsexp '*' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fvexp '*' fsexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | ftexp '&' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fvexp '&' ftexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fhexp '&' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fvexp '&' fhexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fyexp '&' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fvexp '&' fyexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fvexp '^' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 ^ *$3);
            delete $1; delete $3;
          }
        | fvexp '/' fsexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 / *$3);
            delete $1; delete $3;
          }
        | fvexp '-' fvexp 		   {
            sameSize($1,$3);
            $$ = new Foam::edgeVectorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | '-' fvexp %prec TOKEN_NEG 	   {
            $$ = new Foam::edgeVectorField(-*$2);
            delete $2;
          }
        | '*' ftexp %prec TOKEN_HODGE          {
            $$ = new Foam::edgeVectorField(*(*$2));
            delete $2;
          }
        | '*' fyexp %prec TOKEN_HODGE          {
            $$ = new Foam::edgeVectorField(*(*$2));
            delete $2;
          }
        | '(' fvexp ')'		           { $$ = $2; }
        | TOKEN_eigenValues '(' ftexp ')'       {
            $$ = new Foam::edgeVectorField(Foam::eigenValues(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_eigenValues '(' fyexp ')'       {
            $$ = new Foam::edgeVectorField(Foam::eigenValues(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | ftexp '.' tensorComponentSwitch  TOKEN_x                   {
            $$ = driver.makeEdgeVectorField(
                $1->component(Foam::tensor::XX)(),
                $1->component(Foam::tensor::XY)(),
                $1->component(Foam::tensor::XZ)()
            ).ptr();
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | ftexp '.' tensorComponentSwitch  TOKEN_y                   {
            $$ = driver.makeEdgeVectorField(
                $1->component(Foam::tensor::YX)(),
                $1->component(Foam::tensor::YY)(),
                $1->component(Foam::tensor::YZ)()
            ).ptr();
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | ftexp '.' tensorComponentSwitch  TOKEN_z                   {
            $$ = driver.makeEdgeVectorField(
                $1->component(Foam::tensor::ZX)(),
                $1->component(Foam::tensor::ZY)(),
                $1->component(Foam::tensor::ZZ)()
            ).ptr();
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_diag '(' ftexp ')'       {
            $$ = driver.makeEdgeVectorField(
                $3->component(Foam::tensor::XX)(),
                $3->component(Foam::tensor::YY)(),
                $3->component(Foam::tensor::ZZ)()
            ).ptr();
            delete $3;
          }
        | TOKEN_diag '(' fyexp ')'       {
            $$ = driver.makeEdgeVectorField(
                $3->component(Foam::symmTensor::XX)(),
                $3->component(Foam::symmTensor::YY)(),
                $3->component(Foam::symmTensor::ZZ)()
            ).ptr();
            delete $3;
          }
        | flexp '?' fvexp ':' fvexp        {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
          }
        | TOKEN_fposition '(' ')'          {
            $$ = driver.makeEdgePositionField().ptr();
          }
        | TOKEN_fprojection '(' ')'        {
            $$ = driver.makeEdgeProjectionField().ptr();
          }
        | TOKEN_face '(' ')'               {
            $$ = driver.makeEdgeField().ptr();
          }
        | TOKEN_lnGrad '(' vexp ')'        {
            $$ = new Foam::edgeVectorField(Foam::fac::lnGrad(*$3));
            delete $3;
          }
        | TOKEN_interpolate '(' vexp ')'   {
            $$ = new Foam::edgeVectorField(Foam::fac::interpolate(*$3));
            delete $3;
          }
        | TOKEN_min '(' fvexp ',' fvexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_max '(' fvexp ',' fvexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_min '(' fvexp ')'          {
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_minPosition '(' fsexp ')'           {
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                driver.getPositionOfMinimum(
                    *$3,
                    driver.aMesh().edgeCentres()
                )
            ).ptr();
            delete $3;
        }
        | TOKEN_max '(' fvexp ')'          {
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_maxPosition '(' fsexp ')'           {
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                driver.getPositionOfMaximum(
                    *$3,
                    driver.aMesh().edgeCentres()
                )
            ).ptr();
            delete $3;
        }
        | TOKEN_sum '(' fvexp ')'          {
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' fvexp ')'      {
            $$ = driver.makeConstantField<Foam::edgeVectorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | evaluateEdgeVectorFunction restOfFunction
        | TOKEN_FVID                       {
            $$ = driver.getField<Foam::edgeVectorField>(*$1).ptr();
            delete $1;
          }
        | TOKEN_oldTime '(' TOKEN_FVID ')' {
            $$=new Foam::edgeVectorField(
                driver.getField<Foam::edgeVectorField>(*$3,true)->oldTime()
            );
            delete $3;
          }
;

evaluateEdgeVectorFunction: TOKEN_FUNCTION_FVID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::edgeVectorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

scalar:	TOKEN_NUM		        { $$ = $1; }
        | '-' TOKEN_NUM         	{ $$ = -$2; }
;

exp:    TOKEN_NUM                                  {
            $$ = driver.makeConstantField<Foam::areaScalarField>($1).ptr();
          }
        | exp '+' exp 		                   {
            sameSize($1,$3);
            $$ = new Foam::areaScalarField(*$1 + *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | exp '-' exp 		                   {
            sameSize($1,$3);
            $$ = new Foam::areaScalarField(*$1 - *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | exp '*' exp 		                   {
            sameSize($1,$3);
            $$ = new Foam::areaScalarField(*$1 * *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | exp '%' exp 		                   {
            sameSize($1,$3);
            $$ = driver.makeModuloField(*$1,*$3).ptr();
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | exp '/' exp 		                   {
            sameSize($1,$3);
            $$ = new Foam::areaScalarField(*$1 / *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
// produces a reduce/reduc-conflict with the rule below
        // | TOKEN_pow '(' exp ',' scalar ')'	   {
        //     $$ = new Foam::areaScalarField(Foam::pow(*$3, $5));
        //     delete $3;
        //     driver.setCalculatedPatches(*$$);
        //   }
        | TOKEN_pow '(' exp ',' exp ')'	   {
            $$ = new Foam::areaScalarField(Foam::pow(*$3, *$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_log '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::log(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_exp '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::exp(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '&' vexp 	                   {
            $$ = new Foam::areaScalarField(*$1 & *$3);
            delete $1; delete $3;
          }
        | texp TOKEN_AND texp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | hexp TOKEN_AND texp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | yexp TOKEN_AND texp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | texp TOKEN_AND yexp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | hexp TOKEN_AND yexp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | yexp TOKEN_AND yexp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | texp TOKEN_AND hexp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | hexp TOKEN_AND hexp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | yexp TOKEN_AND hexp 	                   {
            $$ = new Foam::areaScalarField(*$1 && *$3);
            delete $1; delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | '-' exp %prec TOKEN_NEG 	           {
            $$ = new Foam::areaScalarField(-*$2);
            delete $2;
            driver.setCalculatedPatches(*$$);
          }
	| '(' exp ')'		                   { $$ = $2; }
        | TOKEN_sqr '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::sqr(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sqrt '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::sqrt(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sin '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::sin(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_cos '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::cos(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_tan '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::tan(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_log10 '(' exp ')'                  {
            $$ = new Foam::areaScalarField(Foam::log10(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_asin '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::asin(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_acos '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::acos(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_atan '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::atan(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sinh '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::sinh(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_cosh '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::cosh(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_tanh '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::tanh(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_asinh '(' exp ')'                  {
            $$ = new Foam::areaScalarField(Foam::asinh(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_acosh '(' exp ')'                  {
            $$ = new Foam::areaScalarField(Foam::acosh(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_atanh '(' exp ')'                  {
            $$ = new Foam::areaScalarField(Foam::atanh(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_erf '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::erf(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_erfc '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::erfc(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_lgamma '(' exp ')'                 {
            $$ = new Foam::areaScalarField(Foam::lgamma(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_besselJ0 '(' exp ')'               {
            $$ = new Foam::areaScalarField(Foam::j0(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_besselJ1 '(' exp ')'               {
            $$ = new Foam::areaScalarField(Foam::j1(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_besselY0 '(' exp ')'               {
            $$ = new Foam::areaScalarField(Foam::y0(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_besselY1 '(' exp ')'               {
            $$ = new Foam::areaScalarField(Foam::y1(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sign '(' exp ')'                   {
            $$ = new Foam::areaScalarField(Foam::sign(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_pos '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::pos(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_neg '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::neg(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' exp ',' exp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_max '(' exp ',' exp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' exp ')'                    {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' exp ')'                    {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' exp ')'                    {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' exp ')'                {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_mag '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::mag(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_mag '(' vexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::mag(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_mag '(' texp ')'                   {
            $$ = new Foam::areaScalarField(Foam::mag(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_mag '(' yexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::mag(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_mag '(' hexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::mag(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_magSqr '(' exp ')'                    {
            $$ = new Foam::areaScalarField(Foam::magSqr(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_magSqr '(' vexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::magSqr(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_magSqr '(' texp ')'                   {
            $$ = new Foam::areaScalarField(Foam::magSqr(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_magSqr '(' yexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::magSqr(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_magSqr '(' hexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::magSqr(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_div '(' vexp ')'                   {
            $$ = new Foam::areaScalarField(Foam::fac::div(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_div '(' fsexp ')'                  {
            $$ = new Foam::areaScalarField(Foam::fac::div(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_div '(' fsexp ',' exp ')'          {
            $$ = new Foam::areaScalarField(Foam::fac::div(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_laplacian '(' exp ')'              {
            $$ = new Foam::areaScalarField(Foam::fac::laplacian(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_laplacian '(' exp ',' exp ')'      {
            $$ = new Foam::areaScalarField(Foam::fac::laplacian(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_laplacian '(' fsexp ',' exp ')'    {
            $$ = new Foam::areaScalarField(Foam::fac::laplacian(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_faceAverage '(' fsexp ')'          {
            $$ = new Foam::areaScalarField(Foam::fac::average(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_integrate '(' fsexp ')'            {
            $$ = new Foam::areaScalarField(Foam::fac::edgeIntegrate(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_surfSum '(' fsexp ')'              {
            $$ = new Foam::areaScalarField(Foam::fac::edgeSum(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '.' vectorComponentSwitch TOKEN_x                             {
            $$ = new Foam::areaScalarField($1->component(0));
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '.' vectorComponentSwitch TOKEN_y                             {
            $$ = new Foam::areaScalarField($1->component(1));
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | vexp '.' vectorComponentSwitch TOKEN_z                             {
            $$ = new Foam::areaScalarField($1->component(2));
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_xx                        {
            $$ = new Foam::areaScalarField($1->component(0));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_xy                        {
            $$ = new Foam::areaScalarField($1->component(1));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_xz                        {
            $$ = new Foam::areaScalarField($1->component(2));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_yx                        {
            $$ = new Foam::areaScalarField($1->component(3));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_yy                        {
            $$ = new Foam::areaScalarField($1->component(4));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_yz                        {
            $$ = new Foam::areaScalarField($1->component(5));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_zx                        {
            $$ = new Foam::areaScalarField($1->component(6));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_zy                        {
            $$ = new Foam::areaScalarField($1->component(7));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_zz                        {
            $$ = new Foam::areaScalarField($1->component(8));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_xx                        {
            $$ = new Foam::areaScalarField($1->component(0));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_xy                        {
            $$ = new Foam::areaScalarField($1->component(1));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_xz                        {
            $$ = new Foam::areaScalarField($1->component(2));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_yy                        {
            $$ = new Foam::areaScalarField($1->component(3));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_yz                        {
            $$ = new Foam::areaScalarField($1->component(4));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_zz                        {
            $$ = new Foam::areaScalarField($1->component(5));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | hexp '.' tensorComponentSwitch TOKEN_ii                        {
            $$ = new Foam::areaScalarField($1->component(0));
            delete $1;    driver.setCalculatedPatches(*$$);
          }
        | lexp '?' exp ':' exp                     {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_pi                                 {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
#ifdef FOAM_NO_SEPARATE_CONSTANT_NAMESPACE
                Foam::mathematicalConstant::pi
#else
                Foam::constant::mathematical::pi
#endif
            ).ptr();
          }
        | TOKEN_rdist '(' vexp ')'                 {
            $$ = driver.makeRDistanceField(*$3).ptr();
            delete $3;
          }
        | TOKEN_area '(' ')'                       {
            $$ = driver.makeAreaField().ptr();
          }
        | TOKEN_rand '(' ')'                       {
            $$ = driver.makeRandomField().ptr();
          }
        | TOKEN_randNormal '(' ')'                 {
            $$ = driver.makeGaussRandomField().ptr();
          }
        | TOKEN_rand '(' TOKEN_INT ')'             {
            $$ = driver.makeRandomField(-$3).ptr();
          }
        | TOKEN_randNormal '('  TOKEN_INT ')'      {
            $$ = driver.makeGaussRandomField(-$3).ptr();
          }
        | TOKEN_randFixed '(' ')'                  {
            $$ = driver.makeRandomField(1).ptr();
          }
        | TOKEN_randNormalFixed '(' ')'            {
            $$ = driver.makeGaussRandomField(1).ptr();
          }
        | TOKEN_randFixed '(' TOKEN_INT ')'        {
            $$ = driver.makeRandomField($3+1).ptr();
          }
        | TOKEN_randNormalFixed '('  TOKEN_INT ')' {
            $$ = driver.makeGaussRandomField($3+1).ptr();
          }
        | TOKEN_id '(' ')'                         {
            $$ = driver.makeCellIdField().ptr();
          }
        | TOKEN_cpu'(' ')'                         {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                Foam::Pstream::myProcNo()
            ).ptr();
          }
        | TOKEN_weight'(' ')'                          {
            $$ = driver.makeField<Foam::areaScalarField>(
                driver.weights(driver.size())
            ).ptr();
          }
        | TOKEN_deltaT '(' ')'                     {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                driver.runTime().deltaT().value()
            ).ptr();
          }
        | TOKEN_time '(' ')'                       {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                driver.runTime().time().value()
            ).ptr();
          }
        | evaluateScalarFunction restOfFunction
        | TOKEN_SID		                   {
            $$ = driver.getField<Foam::areaScalarField>(*$1).ptr();
            delete $1;
          }
        | TOKEN_ddt '(' TOKEN_SID ')'              {
            $$ = Foam::fac::ddt(
                driver.getField<Foam::areaScalarField>(
                    *$3,true
                )()
            ).ptr();
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
        | TOKEN_oldTime '(' TOKEN_SID ')'         {
            $$=new Foam::areaScalarField(
                driver.getField<Foam::areaScalarField>(
                    *$3,true
                )->oldTime()
            );
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
        | TOKEN_LINE		                   {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                driver.getLineValue(
                    *$1,driver.runTime().time().value()
                )
            ).ptr();
            delete $1;
          }
        | TOKEN_LOOKUP '(' exp ')'		   {
            $$ = driver.makeField<Foam::areaScalarField>(
                driver.getLookup(*$1,*$3)
            ).ptr();
            delete $1;  delete $3;
          }
        | TOKEN_LOOKUP2D '(' exp ',' exp ')'		   {
            $$ = driver.makeField<Foam::areaScalarField>(
                driver.getLookup2D(*$1,*$3,*$5)
            ).ptr();
            delete $1;  delete $3; delete $5;
  }
;

evaluateScalarFunction: TOKEN_FUNCTION_SID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::areaScalarField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

lexp: TOKEN_TRUE                       {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                driver.TRUE_Value
            ).ptr();
          }
    | TOKEN_FALSE                      {
            $$ = driver.makeConstantField<Foam::areaScalarField>(
                driver.FALSE_Value
            ).ptr();
          }
    | TOKEN_outputTime '(' ')'          {
       $$ = driver.makeConstantField<Foam::areaScalarField>(
                driver.runTime().outputTime()
           ).ptr();
       }
    | exp '<' exp                      {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::less<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | exp '>' exp                      {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::greater<Foam::scalar>(),*
                $3
            ).ptr();
            delete $1; delete $3;
          }
    | exp TOKEN_LEQ exp                {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::less_equal<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | exp TOKEN_GEQ exp                {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::greater_equal<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | exp TOKEN_EQ exp                 {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::equal_to<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | exp TOKEN_NEQ exp                {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::not_equal_to<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | '(' lexp ')'		       { $$ = $2; }
    | lexp TOKEN_AND lexp              {
            sameSize($1,$3);
            $$ = driver.doLogicalOp(
                *$1,
                std::logical_and<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | lexp TOKEN_OR lexp               {
            sameSize($1,$3);
            $$ = driver.doLogicalOp(
                *$1,
                std::logical_or<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | '!' lexp %prec TOKEN_NOT         {
            $$ = driver.doLogicalNot(*$2).ptr();
            delete $2;
          }
    | evaluateLogicalFunction restOfFunction
;

evaluateLogicalFunction: TOKEN_FUNCTION_LID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::areaScalarField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

flexp: TOKEN_surf '(' TOKEN_TRUE ')'  {
            $$ = driver.makeConstantField<Foam::edgeScalarField>(
                driver.TRUE_Value
            ).ptr();
          }
    | TOKEN_surf '(' TOKEN_FALSE ')'  {
            $$ = driver.makeConstantField<Foam::edgeScalarField>(
                driver.FALSE_Value
            ).ptr();
          }
    | fsexp '<' fsexp                 {
            sameSize($1,$3);
            $$ = driver.doCompare(*$1,std::less<Foam::scalar>(),*$3).ptr();
            delete $1; delete $3;
          }
    | fsexp '>' fsexp                 {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::greater<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | fsexp TOKEN_LEQ fsexp           {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::less_equal<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | fsexp TOKEN_GEQ fsexp           {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::greater_equal<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | fsexp TOKEN_EQ fsexp            {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::equal_to<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | fsexp TOKEN_NEQ fsexp           {
            sameSize($1,$3);
            $$ = driver.doCompare(
                *$1,
                std::not_equal_to<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | '(' flexp ')'		      { $$ = $2; }
    | flexp TOKEN_AND flexp           {
            sameSize($1,$3);
            $$ = driver.doLogicalOp(
                *$1,
                std::logical_and<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | flexp TOKEN_OR flexp            {
            sameSize($1,$3);
            $$ = driver.doLogicalOp(
                *$1,
                std::logical_or<Foam::scalar>(),
                *$3
            ).ptr();
            delete $1; delete $3;
          }
    | '!' flexp %prec TOKEN_NOT       {
            $$ = driver.doLogicalNot(*$2).ptr();
            delete $2;
          }
    | evaluateEdgeLogicalFunction restOfFunction
;

evaluateEdgeLogicalFunction: TOKEN_FUNCTION_FLID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::edgeScalarField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

texp:   tensor                  { $$ = $1; }
        | texp '+' texp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '+' yexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '+' hexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '+' texp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '+' texp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | exp '*' texp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '*' exp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | vexp '*' vexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '&' texp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '&' texp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '&' yexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '&' texp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '&' hexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '/' exp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 / *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '-' texp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '-' yexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | texp '-' hexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '-' texp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '-' texp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | '-' texp %prec TOKEN_NEG 	           {
            $$ = new Foam::areaTensorField(-*$2);
            delete $2;    driver.setCalculatedPatches(*$$);
          }
        | '(' texp ')'		        { $$ = $2; }
        | TOKEN_skew '(' texp ')' 	           {
            $$ = new Foam::areaTensorField( Foam::skew(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_eigenVectors '(' texp ')'       {
            $$ = new Foam::areaTensorField(Foam::eigenVectors(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_eigenVectors '(' yexp ')'       {
            $$ = driver.makeField<Foam::areaTensorField>(
                Foam::eigenVectors($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_inv '(' texp ')' 	           {
            $$ = new Foam::areaTensorField( Foam::inv(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_cof '(' texp ')' 	           {
            $$ = driver.makeField<Foam::areaTensorField>(
                Foam::cof($3->internalField())
            ).ptr();
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_dev '(' texp ')' 	           {
            $$ = new Foam::areaTensorField( Foam::dev(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_dev2 '(' texp ')'                  {
            $$ = new Foam::areaTensorField( Foam::dev2(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | texp '.' tensorComponentSwitch TOKEN_transpose '(' ')'  {
            $$ = new Foam::areaTensorField( $1->T() );
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | lexp '?' texp ':' texp                   {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
          }
        | TOKEN_laplacian '(' fsexp ',' texp ')'  {
            $$ = new Foam::areaTensorField(Foam::fac::laplacian(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_faceAverage '(' ftexp ')'         {
            $$ = new Foam::areaTensorField(Foam::fac::average(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_integrate '(' ftexp ')'           {
            $$ = new Foam::areaTensorField(Foam::fac::edgeIntegrate(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_surfSum '(' ftexp ')'             {
            $$ = new Foam::areaTensorField(Foam::fac::edgeSum(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' texp ',' texp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_max '(' texp ',' texp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' texp ')'                 {
            $$ = driver.makeConstantField<Foam::areaTensorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' texp ')'                 {
            $$ = driver.makeConstantField<Foam::areaTensorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' texp ')'                 {
            $$ = driver.makeConstantField<Foam::areaTensorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' texp ')'             {
            $$ = driver.makeConstantField<Foam::areaTensorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_grad '(' vexp ')'                 {
            $$ = new Foam::areaTensorField(Foam::fac::grad(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_div '(' fsexp ',' texp ')'        {
            $$ = new Foam::areaTensorField(Foam::fac::div(*$3,*$5));
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | evaluateTensorFunction restOfFunction
        | TOKEN_TID                               {
            $$=driver.getField<Foam::areaTensorField>(*$1).ptr();
            delete $1;
          }
        | TOKEN_ddt '(' TOKEN_TID ')'		   {
            $$ = Foam::fac::ddt(
                driver.getField<Foam::areaTensorField>(
                    *$3,true
                )()
            ).ptr();
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
        | TOKEN_oldTime '(' TOKEN_TID ')'		   {
            $$ = new Foam::areaTensorField(
                driver.getField<Foam::areaTensorField>(
                    *$3,true
                )->oldTime()
            );
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
;

evaluateTensorFunction: TOKEN_FUNCTION_TID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::areaTensorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

yexp:   symmTensor                  { $$ = $1; }
        | yexp '+' yexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '+' yexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '+' hexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | exp '*' yexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '*' exp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '&' yexp 	   	          {
            sameSize($1,$3);
#ifndef FOAM_SYMMTENSOR_WORKAROUND
            $$ = new Foam::areaSymmTensorField(*$1 & *$3);
#else
            $$ = new Foam::areaSymmTensorField(
                symm(*$1 & *$3)
            );
#endif
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '&' yexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '&' hexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '/' exp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 / *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '-' yexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '-' yexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '-' hexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSymmTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | '-' yexp %prec TOKEN_NEG 	           {
            $$ = new Foam::areaSymmTensorField(-*$2);
            delete $2;    driver.setCalculatedPatches(*$$);
          }
        | '(' yexp ')'		        { $$ = $2; }
        | TOKEN_symm '(' texp ')' 	           {
            $$ = new Foam::areaSymmTensorField( Foam::symm(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_symm '(' yexp ')' 	           {
            $$ = new Foam::areaSymmTensorField( Foam::symm(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_twoSymm '(' texp ')'               {
            $$ = new Foam::areaSymmTensorField( Foam::twoSymm(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_twoSymm '(' yexp ')'               {
            $$ = new Foam::areaSymmTensorField( Foam::twoSymm(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_inv '(' yexp ')' 	           {
            $$ = new Foam::areaSymmTensorField( Foam::inv(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_cof '(' yexp ')' 	           {
            $$ = driver.makeField<Foam::areaSymmTensorField>(
                Foam::cof($3->internalField())
            ).ptr();
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_dev '(' yexp ')' 	           {
            $$ = new Foam::areaSymmTensorField( Foam::dev(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | TOKEN_dev2 '(' yexp ')'                  {
            $$ = new Foam::areaSymmTensorField( Foam::dev2(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sqr '(' vexp ')' 	           {
            $$ = new Foam::areaSymmTensorField( Foam::sqr(*$3) );
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | yexp '.' tensorComponentSwitch TOKEN_transpose '(' ')'  {
            $$ = $1;
          }
        | lexp '?' yexp ':' yexp                   {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
// Not instantiated in 1.6-ext
        // | TOKEN_laplacian '(' fsexp ',' yexp ')'  {
        //     $$ = new Foam::areaSymmTensorField(
        //         Foam::fac::laplacian(*$3,*$5)
        //     );
        //     delete $3; delete $5;
        //     driver.setCalculatedPatches(*$$);
        //   }
        | TOKEN_faceAverage '(' fyexp ')'         {
            $$ = new Foam::areaSymmTensorField(Foam::fac::average(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_integrate '(' fyexp ')'           {
            $$ = new Foam::areaSymmTensorField(Foam::fac::edgeIntegrate(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_surfSum '(' fyexp ')'             {
            $$ = new Foam::areaSymmTensorField(Foam::fac::edgeSum(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' yexp ',' yexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_max '(' yexp ',' yexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' yexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaSymmTensorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' yexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaSymmTensorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' yexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaSymmTensorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' yexp ')'             {
            $$ = driver.makeConstantField<Foam::areaSymmTensorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
// Not instantiated in 1.6-ext
        // | TOKEN_div '(' fsexp ',' yexp ')'        {
        //     $$ = new Foam::areaSymmTensorField(Foam::fac::div(*$3,*$5));
        //     delete $3; delete $5;
        //     driver.setCalculatedPatches(*$$);
        //   }
        | evaluateSymmTensorFunction restOfFunction
        | TOKEN_YID                               {
            $$=driver.getField<Foam::areaSymmTensorField>(*$1).ptr();
            delete $1;
          }
// Not instantiated in 1.6-ext?
        // | TOKEN_ddt '(' TOKEN_YID ')'		   {
        //     $$ = Foam::fac::ddt(
        //         driver.getField<Foam::areaSymmTensorField>(*$3,true)()
        //     ).ptr();
        //     driver.setCalculatedPatches(*$$);
        //     delete $3;
        //   }
        | TOKEN_oldTime '(' TOKEN_YID ')'	   {
            $$ = new Foam::areaSymmTensorField(
                driver.getField<Foam::areaSymmTensorField>(*$3,true)->oldTime()
            );
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
;

evaluateSymmTensorFunction: TOKEN_FUNCTION_YID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::areaSymmTensorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

hexp:   sphericalTensor                  { $$ = $1; }
        | TOKEN_unitTensor                        {
            $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(
                Foam::sphericalTensor(1)
            ).ptr();
          }
        | hexp '+' hexp 		          {
            sameSize($1,$3);
            $$ = new Foam::areaSphericalTensorField(*$1 + *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | exp '*' hexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaSphericalTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '*' exp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaSphericalTensorField(*$1 * *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '&' hexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSphericalTensorField(*$1 & *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '/' exp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::areaSphericalTensorField(*$1 / *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '-' hexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::areaSphericalTensorField(*$1 - *$3);
            delete $1; delete $3;    driver.setCalculatedPatches(*$$);
          }
        | '-' hexp %prec TOKEN_NEG 	           {
            $$ = new Foam::areaSphericalTensorField(-*$2);
            delete $2;    driver.setCalculatedPatches(*$$);
          }
        | '(' hexp ')'		        { $$ = $2; }
        | TOKEN_sph '(' texp ')'              {
            $$ = driver.makeField<Foam::areaSphericalTensorField>(
                Foam::sph($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sph '(' yexp ')'              {
            $$ = driver.makeField<Foam::areaSphericalTensorField>(
                Foam::sph($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sph '(' hexp ')'              {
            $$ = driver.makeField<Foam::areaSphericalTensorField>(
                Foam::sph($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_inv '(' hexp ')' 	           {
            $$ = driver.makeField<Foam::areaSphericalTensorField>(
                Foam::inv($3->internalField())
            ).ptr();
            delete $3;    driver.setCalculatedPatches(*$$);
          }
        | hexp '.' tensorComponentSwitch TOKEN_transpose '(' ')'  {
            $$ = $1;
          }
        | lexp '?' hexp ':' hexp                   {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
// Not instantiated in 1.6-ext
        // | TOKEN_laplacian '(' fsexp ',' hexp ')'  {
        //     $$ = new Foam::areaSphericalTensorField(
        //         Foam::fac::laplacian(*$3,*$5)
        //     );
        //     delete $3; delete $5;
        //     driver.setCalculatedPatches(*$$);
        //   }
        | TOKEN_faceAverage '(' fhexp ')'         {
            $$ = new Foam::areaSphericalTensorField(Foam::fac::average(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_integrate '(' fhexp ')'           {
            $$ = new Foam::areaSphericalTensorField(
                Foam::fac::edgeIntegrate(*$3)
            );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_surfSum '(' fhexp ')'             {
            $$ = new Foam::areaSphericalTensorField(Foam::fac::edgeSum(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' hexp ',' hexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_max '(' hexp ',' hexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_min '(' hexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' hexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' hexp ')'                 {
            $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' hexp ')'             {
            $$ = driver.makeConstantField<Foam::areaSphericalTensorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
// Not instantiated in 1.6-ext
        // | TOKEN_div '(' fsexp ',' hexp ')'        {
        //     $$ = new Foam::areaSphericalTensorField(Foam::fac::div(*$3,*$5));
        //     delete $3; delete $5;
        //     driver.setCalculatedPatches(*$$);
        //   }
        | evaluateSphericalTensorFunction restOfFunction
        | TOKEN_HID                               {
            $$=driver.getField<Foam::areaSphericalTensorField>(*$1).ptr();
            delete $1;
          }
// Not instantiated in 1.6-ext?
        // | TOKEN_ddt '(' TOKEN_HID ')'		   {
        //     $$ = Foam::fac::ddt(
        //         driver.getField<Foam::areaSphericalTensorField>(
        //             *$3,true
        //         )()
        //     ).ptr();
        //     driver.setCalculatedPatches(*$$);
        //     delete $3;
        //   }
        | TOKEN_oldTime '(' TOKEN_HID ')'	   {
            $$ = new Foam::areaSphericalTensorField(
                driver.getField<Foam::areaSphericalTensorField>(
                    *$3,true
                )->oldTime()
            );
            driver.setCalculatedPatches(*$$);
            delete $3;
          }
;

evaluateSphericalTensorFunction: TOKEN_FUNCTION_HID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::areaSphericalTensorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

ftexp:   ftensor                  { $$ = $1; }
        | ftexp '+' ftexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | ftexp '+' fyexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | ftexp '+' fhexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fyexp '+' ftexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fhexp '+' ftexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fsexp '*' ftexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | ftexp '*' fsexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fvexp '*' fvexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | ftexp '&' ftexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fyexp '&' ftexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | ftexp '&' fyexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fhexp '&' ftexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | ftexp '&' fhexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | ftexp '/' fsexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 / *$3);
            delete $1; delete $3;
          }
        | ftexp '-' ftexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | ftexp '-' fyexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | ftexp '-' fhexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | fyexp '-' ftexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | fhexp '-' ftexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | '-' ftexp %prec TOKEN_NEG 	           {
            $$ = new Foam::edgeTensorField(-*$2);
            delete $2;
          }
        | '(' ftexp ')'		        { $$ = $2; }
        | TOKEN_skew '(' ftexp ')' 	           {
            $$ = new Foam::edgeTensorField( Foam::skew(*$3) );
            delete $3;
          }
        | TOKEN_eigenVectors '(' ftexp ')'       {
            $$ = new Foam::edgeTensorField(Foam::eigenVectors(*$3));
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_eigenVectors '(' fyexp ')'       {
            $$ = driver.makeField<Foam::edgeTensorField>(
                Foam::eigenVectors($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_inv '(' ftexp ')' 	           {
            $$ = new Foam::edgeTensorField( Foam::inv(*$3) );
            delete $3;
          }
        | TOKEN_cof '(' ftexp ')' 	           {
            $$ = driver.makeField<Foam::edgeTensorField>(
                Foam::cof($3->internalField())
            ).ptr();
            delete $3;
          }
        | TOKEN_dev '(' ftexp ')' 	           {
            $$ = new Foam::edgeTensorField( Foam::dev(*$3) );
            delete $3;
          }
        | TOKEN_dev2 '(' ftexp ')'                 {
            $$ = new Foam::edgeTensorField( Foam::dev2(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | ftexp '.' tensorComponentSwitch TOKEN_transpose '(' ')'  {
            $$ = new Foam::edgeTensorField( $1->T() );
            delete $1;
            driver.setCalculatedPatches(*$$);
          }
        | flexp '?' ftexp ':' ftexp                   {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
          }
        | TOKEN_lnGrad '(' texp ')'           {
            $$ = new Foam::edgeTensorField(Foam::fac::lnGrad(*$3));
            delete $3;
          }
        | TOKEN_interpolate '(' texp ')'           {
            $$ = new Foam::edgeTensorField(Foam::fac::interpolate(*$3));
            delete $3;
          }
        | TOKEN_min '(' ftexp ',' ftexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_max '(' ftexp ',' ftexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_min '(' ftexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeTensorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' ftexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeTensorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' ftexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeTensorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' ftexp ')'             {
            $$ = driver.makeConstantField<Foam::edgeTensorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | evaluateEdgeTensorFunction restOfFunction
        | TOKEN_FTID                               {
            $$=driver.getField<Foam::edgeTensorField>(*$1).ptr();
            delete $1;
          }
//        | TOKEN_ddt '(' TOKEN_FTID ')'		   {
          //   $$ = Foam::fac::ddt(
          //       driver.getOrReadField<Foam::edgeTensorField>(
          //           *$3,true,true
          //       )()
          //   ).ptr();
          //   delete $3;
          // } // no fac::ddt for edge Fields
        | TOKEN_oldTime '(' TOKEN_FTID ')'	   {
            $$ = new Foam::edgeTensorField(
                driver.getField<Foam::edgeTensorField>(
                    *$3,true
                )->oldTime()
            );
            delete $3;
          }
;

evaluateEdgeTensorFunction: TOKEN_FUNCTION_FTID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::edgeTensorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

fyexp:   fsymmTensor                  { $$ = $1; }
        | fyexp '+' fyexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fhexp '+' fyexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fyexp '+' fhexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fsexp '*' fyexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fyexp '*' fsexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fyexp '&' fyexp 	   	          {
            sameSize($1,$3);
#ifndef FOAM_SYMMTENSOR_WORKAROUND
            $$ = new Foam::edgeSymmTensorField(*$1 & *$3);
#else
            $$ = new Foam::edgeSymmTensorField(
                symm(*$1 & *$3)
            );
#endif
            delete $1; delete $3;
          }
        | fhexp '&' fyexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fyexp '&' fhexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fyexp '/' fsexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 / *$3);
            delete $1; delete $3;
          }
        | fyexp '-' fyexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | fhexp '-' fyexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | fyexp '-' fhexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSymmTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | '-' fyexp %prec TOKEN_NEG 	           {
            $$ = new Foam::edgeSymmTensorField(-*$2);
            delete $2;
          }
        | '(' fyexp ')'		        { $$ = $2; }
        | TOKEN_symm '(' ftexp ')' 	           {
            $$ = new Foam::edgeSymmTensorField( Foam::symm(*$3) );
            delete $3;
          }
        | TOKEN_symm '(' fyexp ')' 	           {
            $$ = new Foam::edgeSymmTensorField( Foam::symm(*$3) );
            delete $3;
          }
        | TOKEN_twoSymm '(' ftexp ')'              {
            $$ = new Foam::edgeSymmTensorField( Foam::twoSymm(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_twoSymm '(' fyexp ')'              {
            $$ = new Foam::edgeSymmTensorField( Foam::twoSymm(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_inv '(' fyexp ')' 	           {
            $$ = new Foam::edgeSymmTensorField( Foam::inv(*$3) );
            delete $3;
          }
        | TOKEN_cof '(' fyexp ')' 	           {
            $$ = driver.makeField<Foam::edgeSymmTensorField>(
                Foam::cof($3->internalField())
            ).ptr();
            delete $3;
          }
        | TOKEN_dev '(' fyexp ')' 	           {
            $$ = new Foam::edgeSymmTensorField( Foam::dev(*$3) );
            delete $3;
          }
        | TOKEN_dev2 '(' fyexp ')'                 {
            $$ = new Foam::edgeSymmTensorField( Foam::dev2(*$3) );
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sqr '(' fvexp ')' 	           {
            $$ = new Foam::edgeSymmTensorField( Foam::sqr(*$3) );
            delete $3;
          }
        | fyexp '.' tensorComponentSwitch TOKEN_transpose '(' ')'  {
            $$ = $1;
          }
        | flexp '?' fyexp ':' fyexp                   {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
          }
//  Not instantiated in 1.6-ext?
        // | TOKEN_lnGrad '(' yexp ')'           {
        //     $$ = new Foam::edgeSymmTensorField(Foam::fac::lnGrad(*$3));
        //     delete $3;
        //   }
//  Not instantiated in 1.6-ext?
        // | TOKEN_interpolate '(' yexp ')'           {
        //     $$ = new Foam::edgeSymmTensorField(Foam::fac::interpolate(*$3));
        //     delete $3;
        //   }
        | TOKEN_min '(' fyexp ',' fyexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_max '(' fyexp ',' fyexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_min '(' fyexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' fyexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' fyexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' fyexp ')'             {
            $$ = driver.makeConstantField<Foam::edgeSymmTensorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | evaluateEdgeSymmTensorFunction restOfFunction
        | TOKEN_FYID                               {
            $$=driver.getField<Foam::edgeSymmTensorField>(*$1).ptr();
            delete $1;
          }
//        | TOKEN_ddt '(' TOKEN_FYID ')'		   {
          //   $$ = Foam::fac::ddt(
          //       driver.getOrReadField<Foam::edgeSymmTensorField>(
          //           *$3,true,true
          //       )()
          //   ).ptr();
          //   delete $3;
          // }// no fac::ddt for edge Fields
        | TOKEN_oldTime '(' TOKEN_FYID ')'	   {
            $$ = new Foam::edgeSymmTensorField(
                driver.getField<Foam::edgeSymmTensorField>(
                    *$3,true
                )->oldTime()
            );
            delete $3;
          }
;

evaluateEdgeSymmTensorFunction: TOKEN_FUNCTION_FYID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::edgeSymmTensorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

fhexp:   fsphericalTensor                  { $$ = $1; }
        | fhexp '+' fhexp 		          {
            sameSize($1,$3);
            $$ = new Foam::edgeSphericalTensorField(*$1 + *$3);
            delete $1; delete $3;
          }
        | fsexp '*' fhexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeSphericalTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fhexp '*' fsexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeSphericalTensorField(*$1 * *$3);
            delete $1; delete $3;
          }
        | fhexp '&' fhexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSphericalTensorField(*$1 & *$3);
            delete $1; delete $3;
          }
        | fhexp '/' fsexp 	   	                  {
            sameSize($1,$3);
            $$ = new Foam::edgeSphericalTensorField(*$1 / *$3);
            delete $1; delete $3;
          }
        | fhexp '-' fhexp 	   	          {
            sameSize($1,$3);
            $$ = new Foam::edgeSphericalTensorField(*$1 - *$3);
            delete $1; delete $3;
          }
        | '-' fhexp %prec TOKEN_NEG 	           {
            $$ = new Foam::edgeSphericalTensorField(-*$2);
            delete $2;
          }
        | '(' fhexp ')'		        { $$ = $2; }
        | TOKEN_sph '(' ftexp ')'              {
            $$ = driver.makeField<Foam::edgeSphericalTensorField>(
                Foam::sph($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sph '(' fyexp ')'              {
            $$ = driver.makeField<Foam::edgeSphericalTensorField>(
                Foam::sph($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_sph '(' fhexp ')'              {
            $$ = driver.makeField<Foam::edgeSphericalTensorField>(
                Foam::sph($3->internalField())
            ).ptr();
            delete $3;
            driver.setCalculatedPatches(*$$);
          }
        | TOKEN_inv '(' fhexp ')' 	           {
            $$ = driver.makeField<Foam::edgeSphericalTensorField>(
                Foam::inv($3->internalField())
            ).ptr();
            delete $3;
          }
        | fhexp '.' tensorComponentSwitch TOKEN_transpose '(' ')'  {
            $$ = $1;
          }
        | flexp '?' fhexp ':' fhexp                   {
            sameSize($1,$3); sameSize($1,$5);
            $$ = driver.doConditional(
                *$1,*$3,*$5
            ).ptr();
            delete $1; delete $3; delete $5;
          }
//  Not instantiated in 1.6-ext?
        // | TOKEN_lnGrad '(' hexp ')'           {
        //     $$ = new Foam::edgeSphericalTensorField(Foam::fac::lnGrad(*$3));
        //     delete $3;
        //   }
// Not instantiated in 1.6-ext?
        // | TOKEN_interpolate '(' hexp ')'           {
        //     $$ = new Foam::edgeSphericalTensorField(
        //         Foam::fac::interpolate(*$3)
        //     );
        //     delete $3;
        //   }
        | TOKEN_min '(' fhexp ',' fhexp  ')'        {
            $$ = Foam::min(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_max '(' fhexp ',' fhexp  ')'        {
            $$ = Foam::max(*$3,*$5).ptr();
            delete $3; delete $5;
          }
        | TOKEN_min '(' fhexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(
                Foam::min(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_max '(' fhexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(
                Foam::max(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_sum '(' fhexp ')'                 {
            $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(
                Foam::sum(*$3).value()
            ).ptr();
            delete $3;
          }
        | TOKEN_average '(' fhexp ')'             {
            $$ = driver.makeConstantField<Foam::edgeSphericalTensorField>(
                Foam::average(*$3).value()
            ).ptr();
            delete $3;
          }
        | evaluateEdgeSphericalTensorFunction restOfFunction
        | TOKEN_FHID                               {
            $$=driver.getField<Foam::edgeSphericalTensorField>(*$1).ptr();
            delete $1;
          }
//        | TOKEN_ddt '(' TOKEN_FHID ')'		   {
          //   $$ = Foam::fac::ddt(
          //       driver.getOrReadField<Foam::edgeSphericalTensorField>(
          //           *$3,true,true
          //       )()
          //   ).ptr();
          //   delete $3;
          // } // no fac::ddt for edge Fields
        | TOKEN_oldTime '(' TOKEN_FHID ')'	   {
            $$ = new Foam::edgeSphericalTensorField(
                driver.getField<Foam::edgeSphericalTensorField>(
                    *$3,true
                )->oldTime()
            );
            delete $3;
          }
;

evaluateEdgeSphericalTensorFunction: TOKEN_FUNCTION_FHID '(' eatCharactersSwitch
  {
      $$=driver.evaluatePluginFunction<Foam::edgeSphericalTensorField>(
          *$1,
          @2,
          numberOfFunctionChars
      ).ptr();
      delete $1;
  }
;

vector: TOKEN_VECTOR '(' exp ',' exp ',' exp ')' {
            $$ = driver.makeVectorField(*$3,*$5,*$7).ptr();
            delete $3; delete $5; delete $7;
          }
;

tensor: TOKEN_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {
            $$ = driver.makeTensorField(
                *$3,*$5,*$7,
                *$9,*$11,*$13,
                *$15,*$17,*$19
            ).ptr();
            delete $3; delete $5; delete $7; delete $9; delete $11;
            delete $13; delete $15; delete $17; delete $19;
          }
;

symmTensor: TOKEN_SYMM_TENSOR '(' exp ',' exp ',' exp ',' exp ',' exp ',' exp ')' {
            $$ = driver.makeSymmTensorField(
                *$3,*$5,*$7,
                *$9,*$11,
                *$13
            ).ptr();
            delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;
          }
;

sphericalTensor: TOKEN_SPHERICAL_TENSOR '(' exp ')' {
            $$ = driver.makeSphericalTensorField(*$3).ptr();
            delete $3;
          }
;

fvector: TOKEN_VECTOR '(' fsexp ',' fsexp ',' fsexp ')' {
            $$ = driver.makeEdgeVectorField(*$3,*$5,*$7).ptr();
            delete $3; delete $5; delete $7;
          }
;

ftensor: TOKEN_TENSOR '(' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ')' {
            $$ = driver.makeEdgeTensorField(
                *$3,*$5,*$7,
                *$9,*$11,*$13,
                *$15,*$17,*$19
            ).ptr();
            delete $3; delete $5; delete $7; delete $9; delete $11;
            delete $13; delete $15; delete $17; delete $19;
          }
;

fsymmTensor: TOKEN_SYMM_TENSOR '(' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ',' fsexp ')' {
            $$ = driver.makeEdgeSymmTensorField(
                *$3,*$5,*$7,
                *$9,*$11,
                *$13
            ).ptr();
            delete $3; delete $5; delete $7; delete $9; delete $11; delete $13;
          }
;

fsphericalTensor: TOKEN_SPHERICAL_TENSOR '(' fsexp ')' {
            $$ = driver.makeEdgeSphericalTensorField(*$3).ptr();
            delete $3;
          }
;

%%

void parserFaField::FaFieldValueExpressionParser::error (
    const parserFaField::FaFieldValueExpressionParser::location_type& l,
    const std::string& m
)
{
     driver.error (l, m);
}
