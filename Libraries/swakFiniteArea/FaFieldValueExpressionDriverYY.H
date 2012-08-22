//  ICE Revision: $Id$

#ifndef FA_VALUE_EXPRESSION_DRIVER_YY_H
#define FA_VALUE_EXPRESSION_DRIVER_YY_H

#include "FaFieldValueExpressionDriver.H"

#define YY_DECL int parserFaFieldlex(parserFaField::FaFieldValueExpressionParser::semantic_type* yylval_param,parserFaField::location* yylloc,void *yyscanner,Foam::FaFieldValueExpressionDriver &driver,int& start_token,int& numberOfFunctionChars)

YY_DECL;

#endif
