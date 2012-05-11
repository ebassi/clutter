%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "gidl-scanner.h"
#include "idlscannerparser.h"

extern FILE *yyin;
extern int lineno;
extern char linebuf[2000];
extern char *yytext;

extern int yylex (GidlScanner *scanner);
static void yyerror (GidlScanner *scanner, const char *str);
%}

%error-verbose

%parse-param { GidlScanner* scanner }
%lex-param { GidlScanner* scanner }

%token <str> IDENTIFIER "identifier"
%token <str> TYPEDEF_NAME "typedef-name"

%token INTEGER FLOATING CHARACTER STRING BOOLEAN
%token MODULE INTERFACE PROPERTY SIGNAL DIRECTION VISIBILITY
%token ANNOTATION
