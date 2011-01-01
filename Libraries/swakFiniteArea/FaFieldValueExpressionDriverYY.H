//  ICE Revision: $Id$ 

#ifndef FA_VALUE_EXPRESSION_DRIVER_YY_H
#define FA_VALUE_EXPRESSION_DRIVER_YY_H

#include "FaFieldValueExpressionDriver.H"

#define YY_DECL int parserFaFieldlex(parserFaField::FaFieldValueExpressionParser::semantic_type* yylval,parserFaField::location* yylloc,Foam::FaFieldValueExpressionDriver &driver)

YY_DECL;

#endif
