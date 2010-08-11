//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_YY_H
#define VALUE_EXPRESSION_DRIVER_YY_H

#include "FieldValueExpressionDriver.H"

#define YY_DECL int parserslex(parsers::FieldValueExpressionParser::semantic_type* yylval,parsers::location* yylloc,FieldValueExpressionDriver &driver)

YY_DECL;

#endif
