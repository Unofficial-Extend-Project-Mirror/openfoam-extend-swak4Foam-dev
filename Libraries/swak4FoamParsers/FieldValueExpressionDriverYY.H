//  ICE Revision: $Id$ 

#ifndef VALUE_EXPRESSION_DRIVER_YY_H
#define VALUE_EXPRESSION_DRIVER_YY_H

#include "FieldValueExpressionDriver.H"

#define YY_DECL int parserFieldlex(parserField::FieldValueExpressionParser::semantic_type* yylval,parserField::location* yylloc,Foam::FieldValueExpressionDriver &driver)

YY_DECL;

#endif
