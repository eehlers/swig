/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     HBLOCK = 259,
     POUND = 260,
     STRING = 261,
     WSTRING = 262,
     INCLUDE = 263,
     IMPORT = 264,
     INSERT = 265,
     CHARCONST = 266,
     WCHARCONST = 267,
     NUM_INT = 268,
     NUM_FLOAT = 269,
     NUM_UNSIGNED = 270,
     NUM_LONG = 271,
     NUM_ULONG = 272,
     NUM_LONGLONG = 273,
     NUM_ULONGLONG = 274,
     NUM_BOOL = 275,
     TYPEDEF = 276,
     TYPE_INT = 277,
     TYPE_UNSIGNED = 278,
     TYPE_SHORT = 279,
     TYPE_LONG = 280,
     TYPE_FLOAT = 281,
     TYPE_DOUBLE = 282,
     TYPE_CHAR = 283,
     TYPE_WCHAR = 284,
     TYPE_VOID = 285,
     TYPE_SIGNED = 286,
     TYPE_BOOL = 287,
     TYPE_COMPLEX = 288,
     TYPE_TYPEDEF = 289,
     TYPE_RAW = 290,
     TYPE_NON_ISO_INT8 = 291,
     TYPE_NON_ISO_INT16 = 292,
     TYPE_NON_ISO_INT32 = 293,
     TYPE_NON_ISO_INT64 = 294,
     LPAREN = 295,
     RPAREN = 296,
     COMMA = 297,
     SEMI = 298,
     EXTERN = 299,
     INIT = 300,
     LBRACE = 301,
     RBRACE = 302,
     PERIOD = 303,
     CONST_QUAL = 304,
     VOLATILE = 305,
     REGISTER = 306,
     STRUCT = 307,
     UNION = 308,
     EQUAL = 309,
     SIZEOF = 310,
     MODULE = 311,
     LBRACKET = 312,
     RBRACKET = 313,
     BEGINFILE = 314,
     ENDOFFILE = 315,
     ILLEGAL = 316,
     CONSTANT = 317,
     NAME = 318,
     RENAME = 319,
     NAMEWARN = 320,
     EXTEND = 321,
     PRAGMA = 322,
     FEATURE = 323,
     VARARGS = 324,
     ENUM = 325,
     CLASS = 326,
     TYPENAME = 327,
     PRIVATE = 328,
     PUBLIC = 329,
     PROTECTED = 330,
     COLON = 331,
     STATIC = 332,
     VIRTUAL = 333,
     FRIEND = 334,
     THROW = 335,
     CATCH = 336,
     EXPLICIT = 337,
     AUTO = 338,
     STATIC_ASSERT = 339,
     CONSTEXPR = 340,
     THREAD_LOCAL = 341,
     DECLTYPE = 342,
     USING = 343,
     NAMESPACE = 344,
     NATIVE = 345,
     INLINE = 346,
     TYPEMAP = 347,
     EXCEPT = 348,
     ECHO = 349,
     APPLY = 350,
     CLEAR = 351,
     SWIGTEMPLATE = 352,
     FRAGMENT = 353,
     WARN = 354,
     LESSTHAN = 355,
     GREATERTHAN = 356,
     DELETE_KW = 357,
     LESSTHANOREQUALTO = 358,
     GREATERTHANOREQUALTO = 359,
     EQUALTO = 360,
     NOTEQUALTO = 361,
     ARROW = 362,
     QUESTIONMARK = 363,
     TYPES = 364,
     PARMS = 365,
     NONID = 366,
     DSTAR = 367,
     DCNOT = 368,
     TEMPLATE = 369,
     OPERATOR = 370,
     COPERATOR = 371,
     PARSETYPE = 372,
     PARSEPARM = 373,
     PARSEPARMS = 374,
     CAST = 375,
     LOR = 376,
     LAND = 377,
     OR = 378,
     XOR = 379,
     AND = 380,
     RSHIFT = 381,
     LSHIFT = 382,
     MINUS = 383,
     PLUS = 384,
     MODULO = 385,
     SLASH = 386,
     STAR = 387,
     LNOT = 388,
     NOT = 389,
     UMINUS = 390,
     DCOLON = 391
   };
#endif
/* Tokens.  */
#define ID 258
#define HBLOCK 259
#define POUND 260
#define STRING 261
#define WSTRING 262
#define INCLUDE 263
#define IMPORT 264
#define INSERT 265
#define CHARCONST 266
#define WCHARCONST 267
#define NUM_INT 268
#define NUM_FLOAT 269
#define NUM_UNSIGNED 270
#define NUM_LONG 271
#define NUM_ULONG 272
#define NUM_LONGLONG 273
#define NUM_ULONGLONG 274
#define NUM_BOOL 275
#define TYPEDEF 276
#define TYPE_INT 277
#define TYPE_UNSIGNED 278
#define TYPE_SHORT 279
#define TYPE_LONG 280
#define TYPE_FLOAT 281
#define TYPE_DOUBLE 282
#define TYPE_CHAR 283
#define TYPE_WCHAR 284
#define TYPE_VOID 285
#define TYPE_SIGNED 286
#define TYPE_BOOL 287
#define TYPE_COMPLEX 288
#define TYPE_TYPEDEF 289
#define TYPE_RAW 290
#define TYPE_NON_ISO_INT8 291
#define TYPE_NON_ISO_INT16 292
#define TYPE_NON_ISO_INT32 293
#define TYPE_NON_ISO_INT64 294
#define LPAREN 295
#define RPAREN 296
#define COMMA 297
#define SEMI 298
#define EXTERN 299
#define INIT 300
#define LBRACE 301
#define RBRACE 302
#define PERIOD 303
#define CONST_QUAL 304
#define VOLATILE 305
#define REGISTER 306
#define STRUCT 307
#define UNION 308
#define EQUAL 309
#define SIZEOF 310
#define MODULE 311
#define LBRACKET 312
#define RBRACKET 313
#define BEGINFILE 314
#define ENDOFFILE 315
#define ILLEGAL 316
#define CONSTANT 317
#define NAME 318
#define RENAME 319
#define NAMEWARN 320
#define EXTEND 321
#define PRAGMA 322
#define FEATURE 323
#define VARARGS 324
#define ENUM 325
#define CLASS 326
#define TYPENAME 327
#define PRIVATE 328
#define PUBLIC 329
#define PROTECTED 330
#define COLON 331
#define STATIC 332
#define VIRTUAL 333
#define FRIEND 334
#define THROW 335
#define CATCH 336
#define EXPLICIT 337
#define AUTO 338
#define STATIC_ASSERT 339
#define CONSTEXPR 340
#define THREAD_LOCAL 341
#define DECLTYPE 342
#define USING 343
#define NAMESPACE 344
#define NATIVE 345
#define INLINE 346
#define TYPEMAP 347
#define EXCEPT 348
#define ECHO 349
#define APPLY 350
#define CLEAR 351
#define SWIGTEMPLATE 352
#define FRAGMENT 353
#define WARN 354
#define LESSTHAN 355
#define GREATERTHAN 356
#define DELETE_KW 357
#define LESSTHANOREQUALTO 358
#define GREATERTHANOREQUALTO 359
#define EQUALTO 360
#define NOTEQUALTO 361
#define ARROW 362
#define QUESTIONMARK 363
#define TYPES 364
#define PARMS 365
#define NONID 366
#define DSTAR 367
#define DCNOT 368
#define TEMPLATE 369
#define OPERATOR 370
#define COPERATOR 371
#define PARSETYPE 372
#define PARSEPARM 373
#define PARSEPARMS 374
#define CAST 375
#define LOR 376
#define LAND 377
#define OR 378
#define XOR 379
#define AND 380
#define RSHIFT 381
#define LSHIFT 382
#define MINUS 383
#define PLUS 384
#define MODULO 385
#define SLASH 386
#define STAR 387
#define LNOT 388
#define NOT 389
#define UMINUS 390
#define DCOLON 391




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 1653 "parser.y"

  char  *id;
  List  *bases;
  struct Define {
    String *val;
    String *rawval;
    int     type;
    String *qualifier;
    String *bitfield;
    Parm   *throws;
    String *throwf;
  } dtype;
  struct {
    char *type;
    String *filename;
    int   line;
  } loc;
  struct {
    char      *id;
    SwigType  *type;
    String    *defarg;
    ParmList  *parms;
    short      have_parms;
    ParmList  *throws;
    String    *throwf;
  } decl;
  Parm         *tparms;
  struct {
    String     *method;
    Hash       *kwargs;
  } tmap;
  struct {
    String     *type;
    String     *us;
  } ptype;
  SwigType     *type;
  String       *str;
  Parm         *p;
  ParmList     *pl;
  int           intvalue;
  Node         *node;



/* Line 2068 of yacc.c  */
#line 367 "CParse/parser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


