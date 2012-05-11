int lineno;
char linebuf[2000];

#undef YY_BUF_SIZE
#define YY_BUF_SIZE 1048576

extern int yylex (GidlScanner *scanner);
#define YY_DECL int yylex (GidlScanner *scanner)
static int yywrap (void);

%option nounput

intsuffix                               ([uU][lL]?[lL]?)|([lL][lL]?[uU]?)
fracconst                               ([0-9]*\.[0-9]+)|([0-9]+\.)
exppart                                 [eE][-+]?[0-9]+
floatsuffix                             [fFlL]
chartext                                ([^\\\'])|(\\.)
stringtext                              ([^\\\"])|(\\.)

%%

\n.*                                    { strncpy(linebuf, yytext+1, sizeof(linebuf)); /* save the next line */
                                                linebuf[sizeof(linebuf)-1]='\0';
                                                /* printf("%4d:%s\n",lineno,linebuf); */
                                                yyless(1);      /* give back all but the \n to rescan */
                                                ++lineno;
                                        }
"\\\n"                                  { ++lineno; }
[\t\f\v\r ]+                            { /* Ignore whitespace. */ }

"module"                                { return MODULE; }
"interface"                             { return INTERFACE; }
"property"                              { return PROPERTY; }
"signal"                                { return SIGNAL; }
"readonly"                              { return READONLY; }
"readwrite"                             { return READWRITE; }
"writeonly"                             { return WRITEONLY; }
"float"                                 { return FLOAT; }
"double"                                { return DOUBLE; }
"const"                                 { return CONST; }
"struct"                                { return STRUCT; }
"union"                                 { return UNION; }
"enum"                                  { return ENUM; }
"errordomain"                           { return ERRORDOMAIN; }

"0"[xX][0-9a-fA-F]+{intsuffix}?         { return INTEGER; }
"0"[0-7]+{intsuffix}?                   { return INTEGER; }
[0-9]+{intsuffix}?                      { return INTEGER; }

{fracconst}{exppart}?{floatsuffix}?     { return FLOATING; }
[0-9]+{exppart}{floatsuffix}?           { return FLOATING; }

"'"{chartext}*"'"                       { return CHARACTER; }
"L'"{chartext}*"'"                      { return CHARACTER; }

"\""{stringtext}*"\""                   { return STRING; }
"L\""{stringtext}*"\""                  { return STRING; }


%%

static int
yywrap (void)
{
  return 1;
}
