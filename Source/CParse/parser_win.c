/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 22 "parser.y"

#define yylex yylex

#include "swig.h"
#include "cparse.h"
#include "preprocessor.h"
#include <ctype.h>

/* We do this for portability */
#undef alloca
#define alloca malloc

/* -----------------------------------------------------------------------------
 *                               Externals
 * ----------------------------------------------------------------------------- */

int  yyparse();

/* NEW Variables */

static Node    *top = 0;      /* Top of the generated parse tree */
static int      unnamed = 0;  /* Unnamed datatype counter */
static Hash    *extendhash = 0;     /* Hash table of added methods */
static Hash    *classes = 0;        /* Hash table of classes */
static Hash    *classes_typedefs = 0; /* Hash table of typedef classes: typedef struct X {...} Y; */
static Symtab  *prev_symtab = 0;
static Node    *current_class = 0;
String  *ModuleName = 0;
static Node    *module_node = 0;
static String  *Classprefix = 0;  
static String  *Namespaceprefix = 0;
static int      inclass = 0;
static int      nested_template = 0; /* template class/function definition within a class */
static char    *last_cpptype = 0;
static int      inherit_list = 0;
static Parm    *template_parameters = 0;
static int      extendmode   = 0;
static int      compact_default_args = 0;
static int      template_reduce = 0;
static int      cparse_externc = 0;

static int      max_class_levels = 0;
static int      class_level = 0;
static Node   **class_decl = NULL;

/* -----------------------------------------------------------------------------
 *                            Assist Functions
 * ----------------------------------------------------------------------------- */


 
/* Called by the parser (yyparse) when an error is found.*/
static void yyerror (const char *e) {
  (void)e;
}

static Node *new_node(const_String_or_char_ptr tag) {
  Node *n = NewHash();
  set_nodeType(n,tag);
  Setfile(n,cparse_file);
  Setline(n,cparse_line);
  return n;
}

/* Copies a node.  Does not copy tree links or symbol table data (except for
   sym:name) */

static Node *copy_node(Node *n) {
  Node *nn;
  Iterator k;
  nn = NewHash();
  Setfile(nn,Getfile(n));
  Setline(nn,Getline(n));
  for (k = First(n); k.key; k = Next(k)) {
    String *ci;
    String *key = k.key;
    char *ckey = Char(key);
    if ((strcmp(ckey,"nextSibling") == 0) ||
	(strcmp(ckey,"previousSibling") == 0) ||
	(strcmp(ckey,"parentNode") == 0) ||
	(strcmp(ckey,"lastChild") == 0)) {
      continue;
    }
    if (Strncmp(key,"csym:",5) == 0) continue;
    /* We do copy sym:name.  For templates */
    if ((strcmp(ckey,"sym:name") == 0) || 
	(strcmp(ckey,"sym:weak") == 0) ||
	(strcmp(ckey,"sym:typename") == 0)) {
      String *ci = Copy(k.item);
      Setattr(nn,key, ci);
      Delete(ci);
      continue;
    }
    if (strcmp(ckey,"sym:symtab") == 0) {
      Setattr(nn,"sym:needs_symtab", "1");
    }
    /* We don't copy any other symbol table attributes */
    if (strncmp(ckey,"sym:",4) == 0) {
      continue;
    }
    /* If children.  We copy them recursively using this function */
    if (strcmp(ckey,"firstChild") == 0) {
      /* Copy children */
      Node *cn = k.item;
      while (cn) {
	Node *copy = copy_node(cn);
	appendChild(nn,copy);
	Delete(copy);
	cn = nextSibling(cn);
      }
      continue;
    }
    /* We don't copy the symbol table.  But we drop an attribute 
       requires_symtab so that functions know it needs to be built */

    if (strcmp(ckey,"symtab") == 0) {
      /* Node defined a symbol table. */
      Setattr(nn,"requires_symtab","1");
      continue;
    }
    /* Can't copy nodes */
    if (strcmp(ckey,"node") == 0) {
      continue;
    }
    if ((strcmp(ckey,"parms") == 0) || (strcmp(ckey,"pattern") == 0) || (strcmp(ckey,"throws") == 0)
	|| (strcmp(ckey,"kwargs") == 0)) {
      ParmList *pl = CopyParmList(k.item);
      Setattr(nn,key,pl);
      Delete(pl);
      continue;
    }
    /* Looks okay.  Just copy the data using Copy */
    ci = Copy(k.item);
    Setattr(nn, key, ci);
    Delete(ci);
  }
  return nn;
}

/* -----------------------------------------------------------------------------
 *                              Variables
 * ----------------------------------------------------------------------------- */

static char  *typemap_lang = 0;    /* Current language setting */

static int cplus_mode  = 0;
static String  *class_rename = 0;

/* C++ modes */

#define  CPLUS_PUBLIC    1
#define  CPLUS_PRIVATE   2
#define  CPLUS_PROTECTED 3

/* include types */
static int   import_mode = 0;

void SWIG_typemap_lang(const char *tm_lang) {
  typemap_lang = Swig_copy_string(tm_lang);
}

void SWIG_cparse_set_compact_default_args(int defargs) {
  compact_default_args = defargs;
}

int SWIG_cparse_template_reduce(int treduce) {
  template_reduce = treduce;
  return treduce;  
}

/* -----------------------------------------------------------------------------
 *                           Assist functions
 * ----------------------------------------------------------------------------- */

static int promote_type(int t) {
  if (t <= T_UCHAR || t == T_CHAR) return T_INT;
  return t;
}

/* Perform type-promotion for binary operators */
static int promote(int t1, int t2) {
  t1 = promote_type(t1);
  t2 = promote_type(t2);
  return t1 > t2 ? t1 : t2;
}

static String *yyrename = 0;

/* Forward renaming operator */

static String *resolve_create_node_scope(String *cname);


Hash *Swig_cparse_features(void) {
  static Hash   *features_hash = 0;
  if (!features_hash) features_hash = NewHash();
  return features_hash;
}

/* Fully qualify any template parameters */
static String *feature_identifier_fix(String *s) {
  String *tp = SwigType_istemplate_templateprefix(s);
  if (tp) {
    String *ts, *ta, *tq;
    ts = SwigType_templatesuffix(s);
    ta = SwigType_templateargs(s);
    tq = Swig_symbol_type_qualify(ta,0);
    Append(tp,tq);
    Append(tp,ts);
    Delete(ts);
    Delete(ta);
    Delete(tq);
    return tp;
  } else {
    return NewString(s);
  }
}

/* Generate the symbol table name for an object */
/* This is a bit of a mess. Need to clean up */
static String *add_oldname = 0;



static String *make_name(Node *n, String *name,SwigType *decl) {
  int destructor = name && (*(Char(name)) == '~');

  if (yyrename) {
    String *s = NewString(yyrename);
    Delete(yyrename);
    yyrename = 0;
    if (destructor  && (*(Char(s)) != '~')) {
      Insert(s,0,"~");
    }
    return s;
  }

  if (!name) return 0;
  return Swig_name_make(n,Namespaceprefix,name,decl,add_oldname);
}

/* Generate an unnamed identifier */
static String *make_unnamed() {
  unnamed++;
  return NewStringf("$unnamed%d$",unnamed);
}

/* Return if the node is a friend declaration */
static int is_friend(Node *n) {
  return Cmp(Getattr(n,"storage"),"friend") == 0;
}

static int is_operator(String *name) {
  return Strncmp(name,"operator ", 9) == 0;
}


/* Add declaration list to symbol table */
static int  add_only_one = 0;

static void add_symbols(Node *n) {
  String *decl;
  String *wrn = 0;

  if (nested_template) {
    if (!(n && Equal(nodeType(n), "template"))) {
      return;
    }
    /* continue if template function, but not template class, declared within a class */
  }

  if (inclass && n) {
    cparse_normalize_void(n);
  }
  while (n) {
    String *symname = 0;
    /* for friends, we need to pop the scope once */
    String *old_prefix = 0;
    Symtab *old_scope = 0;
    int isfriend = inclass && is_friend(n);
    int iscdecl = Cmp(nodeType(n),"cdecl") == 0;
    int only_csymbol = 0;
    if (extendmode) {
      Setattr(n,"isextension","1");
    }
    
    if (inclass) {
      String *name = Getattr(n, "name");
      if (isfriend) {
	/* for friends, we need to add the scopename if needed */
	String *prefix = name ? Swig_scopename_prefix(name) : 0;
	old_prefix = Namespaceprefix;
	old_scope = Swig_symbol_popscope();
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	if (!prefix) {
	  if (name && !is_operator(name) && Namespaceprefix) {
	    String *nname = NewStringf("%s::%s", Namespaceprefix, name);
	    Setattr(n,"name",nname);
	    Delete(nname);
	  }
	} else {
	  Symtab *st = Swig_symbol_getscope(prefix);
	  String *ns = st ? Getattr(st,"name") : prefix;
	  String *base  = Swig_scopename_last(name);
	  String *nname = NewStringf("%s::%s", ns, base);
	  Setattr(n,"name",nname);
	  Delete(nname);
	  Delete(base);
	  Delete(prefix);
	}
	Namespaceprefix = 0;
      } else {
	/* for member functions, we need to remove the redundant
	   class scope if provided, as in
	   
	   struct Foo {
	   int Foo::method(int a);
	   };
	   
	*/
	String *prefix = name ? Swig_scopename_prefix(name) : 0;
	if (prefix) {
	  if (Classprefix && (Equal(prefix,Classprefix))) {
	    String *base = Swig_scopename_last(name);
	    Setattr(n,"name",base);
	    Delete(base);
	  }
	  Delete(prefix);
	}

        /*
	if (!Getattr(n,"parentNode") && class_level) set_parentNode(n,class_decl[class_level - 1]);
        */
	Setattr(n,"ismember","1");
      }
    }
    if (!isfriend && inclass) {
      if ((cplus_mode != CPLUS_PUBLIC)) {
	only_csymbol = 1;
	if (cplus_mode == CPLUS_PROTECTED) {
	  Setattr(n,"access", "protected");
	  only_csymbol = !Swig_need_protected(n);
	} else {
	  Setattr(n,"access", "private");
	  /* private are needed only when they are pure virtuals - why? */
	  if ((Cmp(Getattr(n,"storage"),"virtual") == 0) && (Cmp(Getattr(n,"value"),"0") == 0)) {
	    only_csymbol = 0;
	  }
	}
      } else {
	  Setattr(n,"access", "public");
      }
    }
    if (Getattr(n,"sym:name")) {
      n = nextSibling(n);
      continue;
    }
    decl = Getattr(n,"decl");
    if (!SwigType_isfunction(decl)) {
      String *name = Getattr(n,"name");
      String *makename = Getattr(n,"parser:makename");
      if (iscdecl) {	
	String *storage = Getattr(n, "storage");
	if (Cmp(storage,"typedef") == 0) {
	  Setattr(n,"kind","typedef");
	} else {
	  SwigType *type = Getattr(n,"type");
	  String *value = Getattr(n,"value");
	  Setattr(n,"kind","variable");
	  if (value && Len(value)) {
	    Setattr(n,"hasvalue","1");
	  }
	  if (type) {
	    SwigType *ty;
	    SwigType *tmp = 0;
	    if (decl) {
	      ty = tmp = Copy(type);
	      SwigType_push(ty,decl);
	    } else {
	      ty = type;
	    }
	    if (!SwigType_ismutable(ty)) {
	      SetFlag(n,"hasconsttype");
	      SetFlag(n,"feature:immutable");
	    }
	    if (tmp) Delete(tmp);
	  }
	  if (!type) {
	    Printf(stderr,"notype name %s\n", name);
	  }
	}
      }
      Swig_features_get(Swig_cparse_features(), Namespaceprefix, name, 0, n);
      if (makename) {
	symname = make_name(n, makename,0);
        Delattr(n,"parser:makename"); /* temporary information, don't leave it hanging around */
      } else {
        makename = name;
	symname = make_name(n, makename,0);
      }
      
      if (!symname) {
	symname = Copy(Getattr(n,"unnamed"));
      }
      if (symname) {
	wrn = Swig_name_warning(n, Namespaceprefix, symname,0);
      }
    } else {
      String *name = Getattr(n,"name");
      SwigType *fdecl = Copy(decl);
      SwigType *fun = SwigType_pop_function(fdecl);
      if (iscdecl) {	
	Setattr(n,"kind","function");
      }
      
      Swig_features_get(Swig_cparse_features(),Namespaceprefix,name,fun,n);

      symname = make_name(n, name,fun);
      wrn = Swig_name_warning(n, Namespaceprefix,symname,fun);
      
      Delete(fdecl);
      Delete(fun);
      
    }
    if (!symname) {
      n = nextSibling(n);
      continue;
    }
    if (only_csymbol || GetFlag(n,"feature:ignore")) {
      /* Only add to C symbol table and continue */
      Swig_symbol_add(0, n);
    } else if (strncmp(Char(symname),"$ignore",7) == 0) {
      char *c = Char(symname)+7;
      SetFlag(n,"feature:ignore");
      if (strlen(c)) {
	SWIG_WARN_NODE_BEGIN(n);
	Swig_warning(0,Getfile(n), Getline(n), "%s\n",c+1);
	SWIG_WARN_NODE_END(n);
      }
      Swig_symbol_add(0, n);
    } else {
      Node *c;
      if ((wrn) && (Len(wrn))) {
	String *metaname = symname;
	if (!Getmeta(metaname,"already_warned")) {
	  SWIG_WARN_NODE_BEGIN(n);
	  Swig_warning(0,Getfile(n),Getline(n), "%s\n", wrn);
	  SWIG_WARN_NODE_END(n);
	  Setmeta(metaname,"already_warned","1");
	}
      }
      c = Swig_symbol_add(symname,n);

      if (c != n) {
        /* symbol conflict attempting to add in the new symbol */
        if (Getattr(n,"sym:weak")) {
          Setattr(n,"sym:name",symname);
        } else {
          String *e = NewStringEmpty();
          String *en = NewStringEmpty();
          String *ec = NewStringEmpty();
          int redefined = Swig_need_redefined_warn(n,c,inclass);
          if (redefined) {
            Printf(en,"Identifier '%s' redefined (ignored)",symname);
            Printf(ec,"previous definition of '%s'",symname);
          } else {
            Printf(en,"Redundant redeclaration of '%s'",symname);
            Printf(ec,"previous declaration of '%s'",symname);
          }
          if (Cmp(symname,Getattr(n,"name"))) {
            Printf(en," (Renamed from '%s')", SwigType_namestr(Getattr(n,"name")));
          }
          Printf(en,",");
          if (Cmp(symname,Getattr(c,"name"))) {
            Printf(ec," (Renamed from '%s')", SwigType_namestr(Getattr(c,"name")));
          }
          Printf(ec,".");
	  SWIG_WARN_NODE_BEGIN(n);
          if (redefined) {
            Swig_warning(WARN_PARSE_REDEFINED,Getfile(n),Getline(n),"%s\n",en);
            Swig_warning(WARN_PARSE_REDEFINED,Getfile(c),Getline(c),"%s\n",ec);
          } else if (!is_friend(n) && !is_friend(c)) {
            Swig_warning(WARN_PARSE_REDUNDANT,Getfile(n),Getline(n),"%s\n",en);
            Swig_warning(WARN_PARSE_REDUNDANT,Getfile(c),Getline(c),"%s\n",ec);
          }
	  SWIG_WARN_NODE_END(n);
          Printf(e,"%s:%d:%s\n%s:%d:%s\n",Getfile(n),Getline(n),en,
                 Getfile(c),Getline(c),ec);
          Setattr(n,"error",e);
	  Delete(e);
          Delete(en);
          Delete(ec);
        }
      }
    }
    /* restore the class scope if needed */
    if (isfriend) {
      Swig_symbol_setscope(old_scope);
      if (old_prefix) {
	Delete(Namespaceprefix);
	Namespaceprefix = old_prefix;
      }
    }
    Delete(symname);

    if (add_only_one) return;
    n = nextSibling(n);
  }
}


/* add symbols a parse tree node copy */

static void add_symbols_copy(Node *n) {
  String *name;
  int    emode = 0;
  while (n) {
    char *cnodeType = Char(nodeType(n));

    if (strcmp(cnodeType,"access") == 0) {
      String *kind = Getattr(n,"kind");
      if (Strcmp(kind,"public") == 0) {
	cplus_mode = CPLUS_PUBLIC;
      } else if (Strcmp(kind,"private") == 0) {
	cplus_mode = CPLUS_PRIVATE;
      } else if (Strcmp(kind,"protected") == 0) {
	cplus_mode = CPLUS_PROTECTED;
      }
      n = nextSibling(n);
      continue;
    }

    add_oldname = Getattr(n,"sym:name");
    if ((add_oldname) || (Getattr(n,"sym:needs_symtab"))) {
      int old_inclass = -1;
      Node *old_current_class = 0;
      if (add_oldname) {
	DohIncref(add_oldname);
	/*  Disable this, it prevents %rename to work with templates */
	/* If already renamed, we used that name  */
	/*
	if (Strcmp(add_oldname, Getattr(n,"name")) != 0) {
	  Delete(yyrename);
	  yyrename = Copy(add_oldname);
	}
	*/
      }
      Delattr(n,"sym:needs_symtab");
      Delattr(n,"sym:name");

      add_only_one = 1;
      add_symbols(n);

      if (Getattr(n,"partialargs")) {
	Swig_symbol_cadd(Getattr(n,"partialargs"),n);
      }
      add_only_one = 0;
      name = Getattr(n,"name");
      if (Getattr(n,"requires_symtab")) {
	Swig_symbol_newscope();
	Swig_symbol_setscopename(name);
	Delete(Namespaceprefix);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (strcmp(cnodeType,"class") == 0) {
	old_inclass = inclass;
	inclass = 1;
	old_current_class = current_class;
	current_class = n;
	if (Strcmp(Getattr(n,"kind"),"class") == 0) {
	  cplus_mode = CPLUS_PRIVATE;
	} else {
	  cplus_mode = CPLUS_PUBLIC;
	}
      }
      if (strcmp(cnodeType,"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (strcmp(cnodeType,"extend") == 0) {
	cplus_mode = emode;
      }
      if (Getattr(n,"requires_symtab")) {
	Setattr(n,"symtab", Swig_symbol_popscope());
	Delattr(n,"requires_symtab");
	Delete(Namespaceprefix);
	Namespaceprefix = Swig_symbol_qualifiedscopename(0);
      }
      if (add_oldname) {
	Delete(add_oldname);
	add_oldname = 0;
      }
      if (strcmp(cnodeType,"class") == 0) {
	inclass = old_inclass;
	current_class = old_current_class;
      }
    } else {
      if (strcmp(cnodeType,"extend") == 0) {
	emode = cplus_mode;
	cplus_mode = CPLUS_PUBLIC;
      }
      add_symbols_copy(firstChild(n));
      if (strcmp(cnodeType,"extend") == 0) {
	cplus_mode = emode;
      }
    }
    n = nextSibling(n);
  }
}

/* Extension merge.  This function is used to handle the %extend directive
   when it appears before a class definition.   To handle this, the %extend
   actually needs to take precedence.  Therefore, we will selectively nuke symbols
   from the current symbol table, replacing them with the added methods */

static void merge_extensions(Node *cls, Node *am) {
  Node *n;
  Node *csym;

  n = firstChild(am);
  while (n) {
    String *symname;
    if (Strcmp(nodeType(n),"constructor") == 0) {
      symname = Getattr(n,"sym:name");
      if (symname) {
	if (Strcmp(symname,Getattr(n,"name")) == 0) {
	  /* If the name and the sym:name of a constructor are the same,
             then it hasn't been renamed.  However---the name of the class
             itself might have been renamed so we need to do a consistency
             check here */
	  if (Getattr(cls,"sym:name")) {
	    Setattr(n,"sym:name", Getattr(cls,"sym:name"));
	  }
	}
      } 
    }

    symname = Getattr(n,"sym:name");
    DohIncref(symname);
    if ((symname) && (!Getattr(n,"error"))) {
      /* Remove node from its symbol table */
      Swig_symbol_remove(n);
      csym = Swig_symbol_add(symname,n);
      if (csym != n) {
	/* Conflict with previous definition.  Nuke previous definition */
	String *e = NewStringEmpty();
	String *en = NewStringEmpty();
	String *ec = NewStringEmpty();
	Printf(ec,"Identifier '%s' redefined by %%extend (ignored),",symname);
	Printf(en,"%%extend definition of '%s'.",symname);
	SWIG_WARN_NODE_BEGIN(n);
	Swig_warning(WARN_PARSE_REDEFINED,Getfile(csym),Getline(csym),"%s\n",ec);
	Swig_warning(WARN_PARSE_REDEFINED,Getfile(n),Getline(n),"%s\n",en);
	SWIG_WARN_NODE_END(n);
	Printf(e,"%s:%d:%s\n%s:%d:%s\n",Getfile(csym),Getline(csym),ec, 
	       Getfile(n),Getline(n),en);
	Setattr(csym,"error",e);
	Delete(e);
	Delete(en);
	Delete(ec);
	Swig_symbol_remove(csym);              /* Remove class definition */
	Swig_symbol_add(symname,n);            /* Insert extend definition */
      }
    }
    n = nextSibling(n);
  }
}

static void append_previous_extension(Node *cls, Node *am) {
  Node *n, *ne;
  Node *pe = 0;
  Node *ae = 0;

  if (!am) return;
  
  n = firstChild(am);
  while (n) {
    ne = nextSibling(n);
    set_nextSibling(n,0);
    /* typemaps and fragments need to be prepended */
    if (((Cmp(nodeType(n),"typemap") == 0) || (Cmp(nodeType(n),"fragment") == 0)))  {
      if (!pe) pe = new_node("extend");
      appendChild(pe, n);
    } else {
      if (!ae) ae = new_node("extend");
      appendChild(ae, n);
    }    
    n = ne;
  }
  if (pe) prependChild(cls,pe);
  if (ae) appendChild(cls,ae);
}
 

/* Check for unused %extend.  Special case, don't report unused
   extensions for templates */
 
static void check_extensions() {
  Iterator ki;

  if (!extendhash) return;
  for (ki = First(extendhash); ki.key; ki = Next(ki)) {
    if (!Strchr(ki.key,'<')) {
      SWIG_WARN_NODE_BEGIN(ki.item);
      Swig_warning(WARN_PARSE_EXTEND_UNDEF,Getfile(ki.item), Getline(ki.item), "%%extend defined for an undeclared class %s.\n", SwigType_namestr(ki.key));
      SWIG_WARN_NODE_END(ki.item);
    }
  }
}

/* Check a set of declarations to see if any are pure-abstract */

static List *pure_abstracts(Node *n) {
  List *abstracts = 0;
  while (n) {
    if (Cmp(nodeType(n),"cdecl") == 0) {
      String *decl = Getattr(n,"decl");
      if (SwigType_isfunction(decl)) {
	String *init = Getattr(n,"value");
	if (Cmp(init,"0") == 0) {
	  if (!abstracts) {
	    abstracts = NewList();
	  }
	  Append(abstracts,n);
	  SetFlag(n,"abstract");
	}
      }
    } else if (Cmp(nodeType(n),"destructor") == 0) {
      if (Cmp(Getattr(n,"value"),"0") == 0) {
	if (!abstracts) {
	  abstracts = NewList();
	}
	Append(abstracts,n);
	SetFlag(n,"abstract");
      }
    }
    n = nextSibling(n);
  }
  return abstracts;
}

/* Make a classname */

static String *make_class_name(String *name) {
  String *nname = 0;
  String *prefix;
  if (Namespaceprefix) {
    nname= NewStringf("%s::%s", Namespaceprefix, name);
  } else {
    nname = NewString(name);
  }
  prefix = SwigType_istemplate_templateprefix(nname);
  if (prefix) {
    String *args, *qargs;
    args   = SwigType_templateargs(nname);
    qargs  = Swig_symbol_type_qualify(args,0);
    Append(prefix,qargs);
    Delete(nname);
    Delete(args);
    Delete(qargs);
    nname = prefix;
  }
  return nname;
}

static List *make_inherit_list(String *clsname, List *names) {
  int i, ilen;
  String *derived;
  List *bases = NewList();

  if (Namespaceprefix) derived = NewStringf("%s::%s", Namespaceprefix,clsname);
  else derived = NewString(clsname);

  ilen = Len(names);
  for (i = 0; i < ilen; i++) {
    Node *s;
    String *base;
    String *n = Getitem(names,i);
    /* Try to figure out where this symbol is */
    s = Swig_symbol_clookup(n,0);
    if (s) {
      while (s && (Strcmp(nodeType(s),"class") != 0)) {
	/* Not a class.  Could be a typedef though. */
	String *storage = Getattr(s,"storage");
	if (storage && (Strcmp(storage,"typedef") == 0)) {
	  String *nn = Getattr(s,"type");
	  s = Swig_symbol_clookup(nn,Getattr(s,"sym:symtab"));
	} else {
	  break;
	}
      }
      if (s && ((Strcmp(nodeType(s),"class") == 0) || (Strcmp(nodeType(s),"template") == 0))) {
	String *q = Swig_symbol_qualified(s);
	Append(bases,s);
	if (q) {
	  base = NewStringf("%s::%s", q, Getattr(s,"name"));
	  Delete(q);
	} else {
	  base = NewString(Getattr(s,"name"));
	}
      } else {
	base = NewString(n);
      }
    } else {
      base = NewString(n);
    }
    if (base) {
      Swig_name_inherit(base,derived);
      Delete(base);
    }
  }
  return bases;
}

/* If the class name is qualified.  We need to create or lookup namespace entries */

static Symtab *set_scope_to_global() {
  Symtab *symtab = Swig_symbol_global_scope();
  Swig_symbol_setscope(symtab);
  return symtab;
}
 
/* Remove the block braces, { and }, if the 'noblock' attribute is set.
 * Node *kw can be either a Hash or Parmlist. */
static String *remove_block(Node *kw, const String *inputcode) {
  String *modified_code = 0;
  while (kw) {
   String *name = Getattr(kw,"name");
   if (name && (Cmp(name,"noblock") == 0)) {
     char *cstr = Char(inputcode);
     size_t len = Len(inputcode);
     if (len && cstr[0] == '{') {
       --len; ++cstr; 
       if (len && cstr[len - 1] == '}') { --len; }
       /* we now remove the extra spaces */
       while (len && isspace((int)cstr[0])) { --len; ++cstr; }
       while (len && isspace((int)cstr[len - 1])) { --len; }
       modified_code = NewStringWithSize(cstr, len);
       break;
     }
   }
   kw = nextSibling(kw);
  }
  return modified_code;
}


static Node *nscope = 0;
static Node *nscope_inner = 0;

/* Remove the scope prefix from cname and return the base name without the prefix.
 * The scopes required for the symbol name are resolved and/or created, if required.
 * For example AA::BB::CC as input returns CC and creates the namespace AA then inner 
 * namespace BB in the current scope. If cname is found to already exist as a weak symbol
 * (forward reference) then the scope might be changed to match, such as when a symbol match 
 * is made via a using reference. */
static String *resolve_create_node_scope(String *cname) {
  Symtab *gscope = 0;
  Node *cname_node = 0;
  int skip_lookup = 0;
  nscope = 0;
  nscope_inner = 0;  

  if (Strncmp(cname,"::",2) == 0)
    skip_lookup = 1;

  cname_node = skip_lookup ? 0 : Swig_symbol_clookup_no_inherit(cname, 0);

  if (cname_node) {
    /* The symbol has been defined already or is in another scope.
       If it is a weak symbol, it needs replacing and if it was brought into the current scope
       via a using declaration, the scope needs adjusting appropriately for the new symbol.
       Similarly for defined templates. */
    Symtab *symtab = Getattr(cname_node, "sym:symtab");
    Node *sym_weak = Getattr(cname_node, "sym:weak");
    if ((symtab && sym_weak) || Equal(nodeType(cname_node), "template")) {
      /* Check if the scope is the current scope */
      String *current_scopename = Swig_symbol_qualifiedscopename(0);
      String *found_scopename = Swig_symbol_qualifiedscopename(symtab);
      int len;
      if (!current_scopename)
	current_scopename = NewString("");
      if (!found_scopename)
	found_scopename = NewString("");
      len = Len(current_scopename);
      if ((len > 0) && (Strncmp(current_scopename, found_scopename, len) == 0)) {
	if (Len(found_scopename) > len + 2) {
	  /* A matching weak symbol was found in non-global scope, some scope adjustment may be required */
	  String *new_cname = NewString(Char(found_scopename) + len + 2); /* skip over "::" prefix */
	  String *base = Swig_scopename_last(cname);
	  Printf(new_cname, "::%s", base);
	  cname = new_cname;
	  Delete(base);
	} else {
	  /* A matching weak symbol was found in the same non-global local scope, no scope adjustment required */
	  assert(len == Len(found_scopename));
	}
      } else {
	String *base = Swig_scopename_last(cname);
	if (Len(found_scopename) > 0) {
	  /* A matching weak symbol was found in a different scope to the local scope - probably via a using declaration */
	  cname = NewStringf("%s::%s", found_scopename, base);
	} else {
	  /* Either:
	      1) A matching weak symbol was found in a different scope to the local scope - this is actually a
	      symbol with the same name in a different scope which we don't want, so no adjustment required.
	      2) A matching weak symbol was found in the global scope - no adjustment required.
	  */
	  cname = Copy(base);
	}
	Delete(base);
      }
      Delete(current_scopename);
      Delete(found_scopename);
    }
  }

  if (Swig_scopename_check(cname)) {
    Node   *ns;
    String *prefix = Swig_scopename_prefix(cname);
    String *base = Swig_scopename_last(cname);
    if (prefix && (Strncmp(prefix,"::",2) == 0)) {
/* I don't think we can use :: global scope to declare classes and hence neither %template. - consider reporting error instead - wsfulton. */
      /* Use the global scope */
      String *nprefix = NewString(Char(prefix)+2);
      Delete(prefix);
      prefix= nprefix;
      gscope = set_scope_to_global();
    }
    if (Len(prefix) == 0) {
      /* Use the global scope, but we need to add a 'global' namespace.  */
      if (!gscope) gscope = set_scope_to_global();
      /* note that this namespace is not the "unnamed" one,
	 and we don't use Setattr(nscope,"name", ""),
	 because the unnamed namespace is private */
      nscope = new_node("namespace");
      Setattr(nscope,"symtab", gscope);;
      nscope_inner = nscope;
      return base;
    }
    /* Try to locate the scope */
    ns = Swig_symbol_clookup(prefix,0);
    if (!ns) {
      Swig_error(cparse_file,cparse_line,"Undefined scope '%s'\n", prefix);
    } else {
      Symtab *nstab = Getattr(ns,"symtab");
      if (!nstab) {
	Swig_error(cparse_file,cparse_line, "'%s' is not defined as a valid scope.\n", prefix);
	ns = 0;
      } else {
	/* Check if the node scope is the current scope */
	String *tname = Swig_symbol_qualifiedscopename(0);
	String *nname = Swig_symbol_qualifiedscopename(nstab);
	if (tname && (Strcmp(tname,nname) == 0)) {
	  ns = 0;
	  cname = base;
	}
	Delete(tname);
	Delete(nname);
      }
      if (ns) {
	/* we will try to create a new node using the namespaces we
	   can find in the scope name */
	List *scopes;
	String *sname;
	Iterator si;
	String *name = NewString(prefix);
	scopes = NewList();
	while (name) {
	  String *base = Swig_scopename_last(name);
	  String *tprefix = Swig_scopename_prefix(name);
	  Insert(scopes,0,base);
	  Delete(base);
	  Delete(name);
	  name = tprefix;
	}
	for (si = First(scopes); si.item; si = Next(si)) {
	  Node *ns1,*ns2;
	  sname = si.item;
	  ns1 = Swig_symbol_clookup(sname,0);
	  assert(ns1);
	  if (Strcmp(nodeType(ns1),"namespace") == 0) {
	    if (Getattr(ns1,"alias")) {
	      ns1 = Getattr(ns1,"namespace");
	    }
	  } else {
	    /* now this last part is a class */
	    si = Next(si);
	    /*  or a nested class tree, which is unrolled here */
	    for (; si.item; si = Next(si)) {
	      if (si.item) {
		Printf(sname,"::%s",si.item);
	      }
	    }
	    /* we get the 'inner' class */
	    nscope_inner = Swig_symbol_clookup(sname,0);
	    /* set the scope to the inner class */
	    Swig_symbol_setscope(Getattr(nscope_inner,"symtab"));
	    /* save the last namespace prefix */
	    Delete(Namespaceprefix);
	    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	    /* and return the node name, including the inner class prefix */
	    break;
	  }
	  /* here we just populate the namespace tree as usual */
	  ns2 = new_node("namespace");
	  Setattr(ns2,"name",sname);
	  Setattr(ns2,"symtab", Getattr(ns1,"symtab"));
	  add_symbols(ns2);
	  Swig_symbol_setscope(Getattr(ns1,"symtab"));
	  Delete(Namespaceprefix);
	  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	  if (nscope_inner) {
	    if (Getattr(nscope_inner,"symtab") != Getattr(ns2,"symtab")) {
	      appendChild(nscope_inner,ns2);
	      Delete(ns2);
	    }
	  }
	  nscope_inner = ns2;
	  if (!nscope) nscope = ns2;
	}
	cname = base;
	Delete(scopes);
      }
    }
    Delete(prefix);
  }

  return cname;
}
 


/* Structures for handling code fragments built for nested classes */

typedef struct Nested {
  String   *code;        /* Associated code fragment */
  int      line;         /* line number where it starts */
  const char *name;      /* Name associated with this nested class */
  const char *kind;      /* Kind of class */
  int      unnamed;      /* unnamed class */
  SwigType *type;        /* Datatype associated with the name */
  struct Nested   *next; /* Next code fragment in list */
} Nested;

/* Some internal variables for saving nested class information */

static Nested      *nested_list = 0;

/* Add a function to the nested list */

static void add_nested(Nested *n) {
  if (!nested_list) {
    nested_list = n;
  } else {
    Nested *n1 = nested_list;
    while (n1->next)
      n1 = n1->next;
    n1->next = n;
  }
}

/* -----------------------------------------------------------------------------
 * nested_new_struct()
 *
 * Nested struct handling for C code only creates a global struct from the nested struct.
 *
 * Nested structure. This is a sick "hack". If we encounter
 * a nested structure, we're going to grab the text of its definition and
 * feed it back into the scanner.  In the meantime, we need to grab
 * variable declaration information and generate the associated wrapper
 * code later.  Yikes!
 *
 * This really only works in a limited sense.   Since we use the
 * code attached to the nested class to generate both C code
 * it can't have any SWIG directives in it.  It also needs to be parsable
 * by SWIG or this whole thing is going to puke.
 * ----------------------------------------------------------------------------- */

static void nested_new_struct(const char *kind, String *struct_code, Node *cpp_opt_declarators) {
  String *name;
  String *decl;

  /* Create a new global struct declaration which is just a copy of the nested struct */
  Nested *nested = (Nested *) malloc(sizeof(Nested));
  Nested *n = nested;

  name = Getattr(cpp_opt_declarators, "name");
  decl = Getattr(cpp_opt_declarators, "decl");

  n->code = NewStringEmpty();
  Printv(n->code, "typedef ", kind, " ", struct_code, " $classname_", name, ";\n", NIL);
  n->name = Swig_copy_string(Char(name));
  n->line = cparse_start_line;
  n->type = NewStringEmpty();
  n->kind = kind;
  n->unnamed = 0;
  SwigType_push(n->type, decl);
  n->next = 0;

  /* Repeat for any multiple instances of the nested struct */
  {
    Node *p = cpp_opt_declarators;
    p = nextSibling(p);
    while (p) {
      Nested *nn = (Nested *) malloc(sizeof(Nested));

      name = Getattr(p, "name");
      decl = Getattr(p, "decl");

      nn->code = NewStringEmpty();
      Printv(nn->code, "typedef ", kind, " ", struct_code, " $classname_", name, ";\n", NIL);
      nn->name = Swig_copy_string(Char(name));
      nn->line = cparse_start_line;
      nn->type = NewStringEmpty();
      nn->kind = kind;
      nn->unnamed = 0;
      SwigType_push(nn->type, decl);
      nn->next = 0;
      n->next = nn;
      n = nn;
      p = nextSibling(p);
    }
  }

  add_nested(nested);
}

/* -----------------------------------------------------------------------------
 * nested_forward_declaration()
 * 
 * Nested struct handling for C++ code only.
 *
 * Treat the nested class/struct/union as a forward declaration until a proper 
 * nested class solution is implemented.
 * ----------------------------------------------------------------------------- */

static Node *nested_forward_declaration(const char *storage, const char *kind, String *sname, String *name, Node *cpp_opt_declarators) {
  Node *nn = 0;
  int warned = 0;

  if (sname) {
    /* Add forward declaration of the nested type */
    Node *n = new_node("classforward");
    Setattr(n, "kind", kind);
    Setattr(n, "name", sname);
    Setattr(n, "storage", storage);
    Setattr(n, "sym:weak", "1");
    add_symbols(n);
    nn = n;
  }

  /* Add any variable instances. Also add in any further typedefs of the nested type.
     Note that anonymous typedefs (eg typedef struct {...} a, b;) are treated as class forward declarations */
  if (cpp_opt_declarators) {
    int storage_typedef = (storage && (strcmp(storage, "typedef") == 0));
    int variable_of_anonymous_type = !sname && !storage_typedef;
    if (!variable_of_anonymous_type) {
      int anonymous_typedef = !sname && (storage && (strcmp(storage, "typedef") == 0));
      Node *n = cpp_opt_declarators;
      SwigType *type = name;
      while (n) {
	Setattr(n, "type", type);
	Setattr(n, "storage", storage);
	if (anonymous_typedef) {
	  Setattr(n, "nodeType", "classforward");
	  Setattr(n, "sym:weak", "1");
	}
	n = nextSibling(n);
      }
      add_symbols(cpp_opt_declarators);

      if (nn) {
	set_nextSibling(nn, cpp_opt_declarators);
      } else {
	nn = cpp_opt_declarators;
      }
    }
  }

  if (nn && Equal(nodeType(nn), "classforward")) {
    Node *n = nn;
    if (GetFlag(n, "feature:nestedworkaround")) {
      Swig_symbol_remove(n);
      nn = 0;
      warned = 1;
    } else {
      SWIG_WARN_NODE_BEGIN(n);
      Swig_warning(WARN_PARSE_NAMED_NESTED_CLASS, cparse_file, cparse_line,"Nested %s not currently supported (%s ignored)\n", kind, sname ? sname : name);
      SWIG_WARN_NODE_END(n);
      warned = 1;
    }
  }

  if (!warned)
    Swig_warning(WARN_PARSE_UNNAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", kind);

  return nn;
}

/* Strips C-style and C++-style comments from string in-place. */
static void strip_comments(char *string) {
  int state = 0; /* 
                  * 0 - not in comment
                  * 1 - in c-style comment
                  * 2 - in c++-style comment
                  * 3 - in string
                  * 4 - after reading / not in comments
                  * 5 - after reading * in c-style comments
                  * 6 - after reading \ in strings
                  */
  char * c = string;
  while (*c) {
    switch (state) {
    case 0:
      if (*c == '\"')
        state = 3;
      else if (*c == '/')
        state = 4;
      break;
    case 1:
      if (*c == '*')
        state = 5;
      *c = ' ';
      break;
    case 2:
      if (*c == '\n')
        state = 0;
      else
        *c = ' ';
      break;
    case 3:
      if (*c == '\"')
        state = 0;
      else if (*c == '\\')
        state = 6;
      break;
    case 4:
      if (*c == '/') {
        *(c-1) = ' ';
        *c = ' ';
        state = 2;
      } else if (*c == '*') {
        *(c-1) = ' ';
        *c = ' ';
        state = 1;
      } else
        state = 0;
      break;
    case 5:
      if (*c == '/')
        state = 0;
      else 
        state = 1;
      *c = ' ';
      break;
    case 6:
      state = 3;
      break;
    }
    ++c;
  }
}

/* Dump all of the nested class declarations to the inline processor
 * However.  We need to do a few name replacements and other munging
 * first.  This function must be called before closing a class! */

static Node *dump_nested(const char *parent) {
  Nested *n,*n1;
  Node *ret = 0;
  Node *last = 0;
  n = nested_list;
  if (!parent) {
    nested_list = 0;
    return 0;
  }
  while (n) {
    Node *retx;
    SwigType *nt;
    /* Token replace the name of the parent class */
    Replace(n->code, "$classname", parent, DOH_REPLACE_ANY);

    /* Fix up the name of the datatype (for building typedefs and other stuff) */
    Append(n->type,parent);
    Append(n->type,"_");
    Append(n->type,n->name);

    /* Add the appropriate declaration to the C++ processor */
    retx = new_node("cdecl");
    Setattr(retx,"name",n->name);
    nt = Copy(n->type);
    Setattr(retx,"type",nt);
    Delete(nt);
    Setattr(retx,"nested",parent);
    if (n->unnamed) {
      Setattr(retx,"unnamed","1");
    }
    
    add_symbols(retx);
    if (ret) {
      set_nextSibling(last, retx);
      Delete(retx);
    } else {
      ret = retx;
    }
    last = retx;

    /* Strip comments - further code may break in presence of comments. */
    strip_comments(Char(n->code));

    /* Make all SWIG created typedef structs/unions/classes unnamed else 
       redefinition errors occur - nasty hack alert.*/

    {
      const char* types_array[3] = {"struct", "union", "class"};
      int i;
      for (i=0; i<3; i++) {
	char* code_ptr = Char(n->code);
	while (code_ptr) {
	  /* Replace struct name (as in 'struct name {...}' ) with whitespace
	     name will be between struct and opening brace */
	
	  code_ptr = strstr(code_ptr, types_array[i]);
	  if (code_ptr) {
	    char *open_bracket_pos;
	    code_ptr += strlen(types_array[i]);
	    open_bracket_pos = strchr(code_ptr, '{');
	    if (open_bracket_pos) { 
	      /* Make sure we don't have something like struct A a; */
	      char* semi_colon_pos = strchr(code_ptr, ';');
	      if (!(semi_colon_pos && (semi_colon_pos < open_bracket_pos)))
		while (code_ptr < open_bracket_pos)
		  *code_ptr++ = ' ';
	    }
	  }
	}
      }
    }
    
    {
      /* Remove SWIG directive %constant which may be left in the SWIG created typedefs */
      char* code_ptr = Char(n->code);
      while (code_ptr) {
	code_ptr = strstr(code_ptr, "%constant");
	if (code_ptr) {
	  char* directive_end_pos = strchr(code_ptr, ';');
	  if (directive_end_pos) { 
            while (code_ptr <= directive_end_pos)
              *code_ptr++ = ' ';
	  }
	}
      }
    }
    {
      Node *newnode = new_node("insert");
      String *code = NewStringEmpty();
      Wrapper_pretty_print(n->code, code);
      Setattr(newnode,"code", code);
      Delete(code);
      set_nextSibling(last, newnode);
      Delete(newnode);      
      last = newnode;
    }
      
    /* Dump the code to the scanner */
    start_inline(Char(Getattr(last, "code")),n->line);

    n1 = n->next;
    Delete(n->code);
    free(n);
    n = n1;
  }
  nested_list = 0;
  return ret;
}

Node *Swig_cparse(File *f) {
  scanner_file(f);
  top = 0;
  yyparse();
  return top;
}

static void single_new_feature(const char *featurename, String *val, Hash *featureattribs, char *declaratorid, SwigType *type, ParmList *declaratorparms, String *qualifier) {
  String *fname;
  String *name;
  String *fixname;
  SwigType *t = Copy(type);

  /* Printf(stdout, "single_new_feature: [%s] [%s] [%s] [%s] [%s] [%s]\n", featurename, val, declaratorid, t, ParmList_str_defaultargs(declaratorparms), qualifier); */

  fname = NewStringf("feature:%s",featurename);
  if (declaratorid) {
    fixname = feature_identifier_fix(declaratorid);
  } else {
    fixname = NewStringEmpty();
  }
  if (Namespaceprefix) {
    name = NewStringf("%s::%s",Namespaceprefix, fixname);
  } else {
    name = fixname;
  }

  if (declaratorparms) Setmeta(val,"parms",declaratorparms);
  if (!Len(t)) t = 0;
  if (t) {
    if (qualifier) SwigType_push(t,qualifier);
    if (SwigType_isfunction(t)) {
      SwigType *decl = SwigType_pop_function(t);
      if (SwigType_ispointer(t)) {
	String *nname = NewStringf("*%s",name);
	Swig_feature_set(Swig_cparse_features(), nname, decl, fname, val, featureattribs);
	Delete(nname);
      } else {
	Swig_feature_set(Swig_cparse_features(), name, decl, fname, val, featureattribs);
      }
      Delete(decl);
    } else if (SwigType_ispointer(t)) {
      String *nname = NewStringf("*%s",name);
      Swig_feature_set(Swig_cparse_features(),nname,0,fname,val, featureattribs);
      Delete(nname);
    }
  } else {
    /* Global feature, that is, feature not associated with any particular symbol */
    Swig_feature_set(Swig_cparse_features(),name,0,fname,val, featureattribs);
  }
  Delete(fname);
  Delete(name);
}

/* Add a new feature to the Hash. Additional features are added if the feature has a parameter list (declaratorparms)
 * and one or more of the parameters have a default argument. An extra feature is added for each defaulted parameter,
 * simulating the equivalent overloaded method. */
static void new_feature(const char *featurename, String *val, Hash *featureattribs, char *declaratorid, SwigType *type, ParmList *declaratorparms, String *qualifier) {

  ParmList *declparms = declaratorparms;

  /* remove the { and } braces if the noblock attribute is set */
  String *newval = remove_block(featureattribs, val);
  val = newval ? newval : val;

  /* Add the feature */
  single_new_feature(featurename, val, featureattribs, declaratorid, type, declaratorparms, qualifier);

  /* Add extra features if there are default parameters in the parameter list */
  if (type) {
    while (declparms) {
      if (ParmList_has_defaultargs(declparms)) {

        /* Create a parameter list for the new feature by copying all
           but the last (defaulted) parameter */
        ParmList* newparms = CopyParmListMax(declparms, ParmList_len(declparms)-1);

        /* Create new declaration - with the last parameter removed */
        SwigType *newtype = Copy(type);
        Delete(SwigType_pop_function(newtype)); /* remove the old parameter list from newtype */
        SwigType_add_function(newtype,newparms);

        single_new_feature(featurename, Copy(val), featureattribs, declaratorid, newtype, newparms, qualifier);
        declparms = newparms;
      } else {
        declparms = 0;
      }
    }
  }
}

/* check if a function declaration is a plain C object */
static int is_cfunction(Node *n) {
  if (!cparse_cplusplus || cparse_externc)
    return 1;
  if (Swig_storage_isexternc(n)) {
    return 1;
  }
  return 0;
}

/* If the Node is a function with parameters, check to see if any of the parameters
 * have default arguments. If so create a new function for each defaulted argument. 
 * The additional functions form a linked list of nodes with the head being the original Node n. */
static void default_arguments(Node *n) {
  Node *function = n;

  if (function) {
    ParmList *varargs = Getattr(function,"feature:varargs");
    if (varargs) {
      /* Handles the %varargs directive by looking for "feature:varargs" and 
       * substituting ... with an alternative set of arguments.  */
      Parm     *p = Getattr(function,"parms");
      Parm     *pp = 0;
      while (p) {
	SwigType *t = Getattr(p,"type");
	if (Strcmp(t,"v(...)") == 0) {
	  if (pp) {
	    ParmList *cv = Copy(varargs);
	    set_nextSibling(pp,cv);
	    Delete(cv);
	  } else {
	    ParmList *cv =  Copy(varargs);
	    Setattr(function,"parms", cv);
	    Delete(cv);
	  }
	  break;
	}
	pp = p;
	p = nextSibling(p);
      }
    }

    /* Do not add in functions if kwargs is being used or if user wants old default argument wrapping
       (one wrapped method per function irrespective of number of default arguments) */
    if (compact_default_args 
	|| is_cfunction(function) 
	|| GetFlag(function,"feature:compactdefaultargs") 
	|| GetFlag(function,"feature:kwargs")) {
      ParmList *p = Getattr(function,"parms");
      if (p) 
        Setattr(p,"compactdefargs", "1"); /* mark parameters for special handling */
      function = 0; /* don't add in extra methods */
    }
  }

  while (function) {
    ParmList *parms = Getattr(function,"parms");
    if (ParmList_has_defaultargs(parms)) {

      /* Create a parameter list for the new function by copying all
         but the last (defaulted) parameter */
      ParmList* newparms = CopyParmListMax(parms,ParmList_len(parms)-1);

      /* Create new function and add to symbol table */
      {
	SwigType *ntype = Copy(nodeType(function));
	char *cntype = Char(ntype);
        Node *new_function = new_node(ntype);
        SwigType *decl = Copy(Getattr(function,"decl"));
        int constqualifier = SwigType_isconst(decl);
	String *ccode = Copy(Getattr(function,"code"));
	String *cstorage = Copy(Getattr(function,"storage"));
	String *cvalue = Copy(Getattr(function,"value"));
	SwigType *ctype = Copy(Getattr(function,"type"));
	String *cthrow = Copy(Getattr(function,"throw"));

        Delete(SwigType_pop_function(decl)); /* remove the old parameter list from decl */
        SwigType_add_function(decl,newparms);
        if (constqualifier)
          SwigType_add_qualifier(decl,"const");

        Setattr(new_function,"name", Getattr(function,"name"));
        Setattr(new_function,"code", ccode);
        Setattr(new_function,"decl", decl);
        Setattr(new_function,"parms", newparms);
        Setattr(new_function,"storage", cstorage);
        Setattr(new_function,"value", cvalue);
        Setattr(new_function,"type", ctype);
        Setattr(new_function,"throw", cthrow);

	Delete(ccode);
	Delete(cstorage);
	Delete(cvalue);
	Delete(ctype);
	Delete(cthrow);
	Delete(decl);

        {
          Node *throws = Getattr(function,"throws");
	  ParmList *pl = CopyParmList(throws);
          if (throws) Setattr(new_function,"throws",pl);
	  Delete(pl);
        }

        /* copy specific attributes for global (or in a namespace) template functions - these are not templated class methods */
        if (strcmp(cntype,"template") == 0) {
          Node *templatetype = Getattr(function,"templatetype");
          Node *symtypename = Getattr(function,"sym:typename");
          Parm *templateparms = Getattr(function,"templateparms");
          if (templatetype) {
	    Node *tmp = Copy(templatetype);
	    Setattr(new_function,"templatetype",tmp);
	    Delete(tmp);
	  }
          if (symtypename) {
	    Node *tmp = Copy(symtypename);
	    Setattr(new_function,"sym:typename",tmp);
	    Delete(tmp);
	  }
          if (templateparms) {
	    Parm *tmp = CopyParmList(templateparms);
	    Setattr(new_function,"templateparms",tmp);
	    Delete(tmp);
	  }
        } else if (strcmp(cntype,"constructor") == 0) {
          /* only copied for constructors as this is not a user defined feature - it is hard coded in the parser */
          if (GetFlag(function,"feature:new")) SetFlag(new_function,"feature:new");
        }

        add_symbols(new_function);
        /* mark added functions as ones with overloaded parameters and point to the parsed method */
        Setattr(new_function,"defaultargs", n);

        /* Point to the new function, extending the linked list */
        set_nextSibling(function, new_function);
	Delete(new_function);
        function = new_function;
	
	Delete(ntype);
      }
    } else {
      function = 0;
    }
  }
}

/* -----------------------------------------------------------------------------
 * tag_nodes()
 *
 * Used by the parser to mark subtypes with extra information.
 * ----------------------------------------------------------------------------- */

static void tag_nodes(Node *n, const_String_or_char_ptr attrname, DOH *value) {
  while (n) {
    Setattr(n, attrname, value);
    tag_nodes(firstChild(n), attrname, value);
    n = nextSibling(n);
  }
}



/* Line 268 of yacc.c  */
#line 1703 "CParse/parser.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


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

/* Line 293 of yacc.c  */
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



/* Line 293 of yacc.c  */
#line 2056 "CParse/parser.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 2068 "CParse/parser.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  58
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   4938

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  137
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  163
/* YYNRULES -- Number of rules.  */
#define YYNRULES  550
/* YYNRULES -- Number of states.  */
#define YYNSTATES  1103

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   391

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     9,    12,    16,    19,    25,    29,
      32,    34,    36,    38,    40,    42,    44,    46,    49,    51,
      53,    55,    57,    59,    61,    63,    65,    67,    69,    71,
      73,    75,    77,    79,    81,    83,    85,    87,    89,    91,
      92,   100,   106,   110,   116,   122,   126,   129,   132,   138,
     141,   147,   150,   155,   157,   159,   167,   175,   181,   182,
     190,   192,   194,   197,   200,   202,   208,   214,   220,   224,
     229,   233,   241,   250,   256,   260,   262,   264,   268,   270,
     275,   283,   290,   292,   294,   302,   312,   321,   332,   338,
     346,   353,   362,   364,   366,   372,   377,   383,   391,   393,
     397,   404,   411,   420,   422,   425,   429,   431,   434,   438,
     445,   451,   461,   464,   466,   468,   470,   471,   478,   480,
     484,   492,   498,   506,   508,   513,   515,   517,   520,   526,
     533,   535,   537,   539,   542,   544,   546,   548,   560,   574,
     582,   584,   586,   588,   589,   593,   595,   598,   601,   604,
     606,   612,   621,   632,   639,   641,   643,   645,   647,   649,
     651,   652,   662,   663,   673,   675,   679,   684,   685,   692,
     696,   701,   703,   705,   707,   709,   711,   713,   715,   717,
     720,   722,   724,   726,   730,   732,   736,   741,   742,   749,
     750,   756,   762,   765,   766,   773,   775,   777,   778,   782,
     784,   786,   788,   790,   792,   794,   796,   798,   800,   804,
     806,   808,   810,   812,   814,   816,   818,   820,   822,   829,
     836,   844,   853,   862,   871,   881,   889,   895,   898,   901,
     904,   907,   908,   916,   917,   924,   926,   928,   930,   932,
     934,   936,   938,   940,   942,   944,   946,   949,   952,   955,
     960,   963,   969,   971,   973,   975,   978,   980,   982,   984,
     987,   991,   993,   995,   997,   999,  1001,  1003,  1005,  1008,
    1011,  1014,  1017,  1019,  1021,  1024,  1026,  1030,  1032,  1035,
    1043,  1047,  1049,  1052,  1054,  1058,  1060,  1062,  1064,  1067,
    1073,  1076,  1079,  1081,  1084,  1087,  1089,  1091,  1093,  1095,
    1098,  1102,  1106,  1108,  1111,  1114,  1118,  1123,  1129,  1134,
    1140,  1147,  1154,  1159,  1165,  1171,  1178,  1186,  1195,  1204,
    1212,  1220,  1222,  1225,  1229,  1234,  1240,  1244,  1249,  1254,
    1256,  1259,  1264,  1269,  1274,  1280,  1284,  1289,  1294,  1300,
    1302,  1305,  1308,  1311,  1315,  1319,  1321,  1324,  1327,  1329,
    1331,  1334,  1338,  1343,  1347,  1352,  1355,  1359,  1363,  1368,
    1372,  1376,  1379,  1382,  1384,  1386,  1389,  1391,  1393,  1395,
    1397,  1400,  1402,  1405,  1409,  1411,  1413,  1415,  1418,  1421,
    1423,  1425,  1428,  1430,  1435,  1437,  1439,  1442,  1444,  1446,
    1448,  1450,  1452,  1454,  1456,  1458,  1460,  1462,  1464,  1466,
    1468,  1470,  1471,  1474,  1476,  1478,  1480,  1482,  1488,  1492,
    1496,  1498,  1500,  1504,  1506,  1508,  1510,  1512,  1514,  1520,
    1529,  1531,  1533,  1535,  1537,  1541,  1546,  1552,  1558,  1564,
    1571,  1578,  1581,  1584,  1587,  1589,  1591,  1593,  1595,  1597,
    1599,  1601,  1603,  1607,  1611,  1615,  1619,  1623,  1627,  1631,
    1635,  1639,  1643,  1647,  1651,  1655,  1659,  1663,  1667,  1673,
    1676,  1679,  1682,  1685,  1688,  1692,  1694,  1696,  1698,  1699,
    1703,  1705,  1707,  1711,  1712,  1717,  1718,  1725,  1727,  1729,
    1731,  1733,  1735,  1740,  1745,  1747,  1749,  1751,  1753,  1755,
    1757,  1762,  1768,  1770,  1774,  1778,  1783,  1788,  1792,  1795,
    1797,  1799,  1803,  1808,  1815,  1818,  1821,  1825,  1827,  1829,
    1831,  1833,  1835,  1838,  1843,  1845,  1849,  1851,  1855,  1859,
    1862,  1865,  1868,  1871,  1874,  1879,  1881,  1885,  1887,  1891,
    1895,  1898,  1901,  1904,  1907,  1909,  1912,  1914,  1916,  1918,
    1920,  1924,  1926,  1930,  1936,  1938,  1942,  1946,  1952,  1954,
    1956
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     138,     0,    -1,   139,    -1,   117,   233,    43,    -1,   117,
       1,    -1,   118,   233,    43,    -1,   118,     1,    -1,   119,
      40,   230,    41,    43,    -1,   119,     1,    43,    -1,   139,
     140,    -1,   299,    -1,   141,    -1,   178,    -1,   194,    -1,
      43,    -1,     1,    -1,   193,    -1,     1,   116,    -1,   142,
      -1,   144,    -1,   145,    -1,   146,    -1,   147,    -1,   148,
      -1,   151,    -1,   152,    -1,   155,    -1,   156,    -1,   157,
      -1,   158,    -1,   159,    -1,   160,    -1,   163,    -1,   165,
      -1,   168,    -1,   170,    -1,   175,    -1,   176,    -1,   177,
      -1,    -1,    66,   296,   288,    46,   143,   211,    47,    -1,
      95,   174,    46,   172,    47,    -1,    96,   172,    43,    -1,
      62,     3,    54,   256,    43,    -1,    62,   249,   241,   238,
      43,    -1,    62,     1,    43,    -1,    94,     4,    -1,    94,
     293,    -1,    93,    40,     3,    41,    46,    -1,    93,    46,
      -1,    93,    40,     3,    41,    43,    -1,    93,    43,    -1,
     293,    46,   233,    47,    -1,   293,    -1,   149,    -1,    98,
      40,   150,    42,   297,    41,     4,    -1,    98,    40,   150,
      42,   297,    41,    46,    -1,    98,    40,   150,    41,    43,
      -1,    -1,   154,   296,   293,    59,   153,   139,    60,    -1,
       8,    -1,     9,    -1,    91,     4,    -1,    91,    46,    -1,
       4,    -1,    10,    40,   286,    41,   293,    -1,    10,    40,
     286,    41,     4,    -1,    10,    40,   286,    41,    46,    -1,
      56,   296,   286,    -1,    63,    40,   286,    41,    -1,    63,
      40,    41,    -1,    90,    40,     3,    41,   229,     3,    43,
      -1,    90,    40,     3,    41,   229,   249,   241,    43,    -1,
      67,   162,     3,    54,   161,    -1,    67,   162,     3,    -1,
     293,    -1,     4,    -1,    40,     3,    41,    -1,   299,    -1,
     164,   241,   286,    43,    -1,   164,    40,   297,    41,   241,
     280,    43,    -1,   164,    40,   297,    41,   293,    43,    -1,
      64,    -1,    65,    -1,    68,    40,   286,    41,   241,   280,
     166,    -1,    68,    40,   286,    42,   298,    41,   241,   280,
      43,    -1,    68,    40,   286,   167,    41,   241,   280,   166,
      -1,    68,    40,   286,    42,   298,   167,    41,   241,   280,
      43,    -1,    68,    40,   286,    41,   166,    -1,    68,    40,
     286,    42,   298,    41,    43,    -1,    68,    40,   286,   167,
      41,   166,    -1,    68,    40,   286,    42,   298,   167,    41,
      43,    -1,   295,    -1,    43,    -1,   110,    40,   230,    41,
      43,    -1,    42,   286,    54,   298,    -1,    42,   286,    54,
     298,   167,    -1,    69,    40,   169,    41,   241,   280,    43,
      -1,   230,    -1,    13,    42,   233,    -1,    92,    40,   171,
      41,   172,   295,    -1,    92,    40,   171,    41,   172,    43,
      -1,    92,    40,   171,    41,   172,    54,   174,    43,    -1,
     297,    -1,   174,   173,    -1,    42,   174,   173,    -1,   299,
      -1,   249,   240,    -1,    40,   230,    41,    -1,    40,   230,
      41,    40,   230,    41,    -1,   109,    40,   230,    41,   166,
      -1,    97,    40,   287,    41,   291,   100,   234,   101,    43,
      -1,    99,   293,    -1,   180,    -1,   192,    -1,   191,    -1,
      -1,    44,   293,    46,   179,   139,    47,    -1,   184,    -1,
      88,   288,    54,    -1,   114,   100,   203,   101,    88,   288,
      54,    -1,   229,   249,   241,   182,   181,    -1,   229,    83,
     241,   107,   183,   182,   181,    -1,    43,    -1,    42,   241,
     182,   181,    -1,    46,    -1,   238,    -1,   247,   238,    -1,
      80,    40,   230,    41,   238,    -1,   247,    80,    40,   230,
      41,   238,    -1,   253,    -1,    32,    -1,    30,    -1,    34,
     285,    -1,    35,    -1,   288,    -1,   252,    -1,   229,    83,
     288,    54,   185,    40,   230,    41,   280,   186,   187,    -1,
     229,    83,   288,    54,   185,    40,   230,    41,   280,   107,
     249,   186,   187,    -1,   229,    83,   288,    54,   185,   186,
     187,    -1,    57,    -1,    46,    -1,    43,    -1,    -1,    40,
     188,    43,    -1,    70,    -1,    70,    71,    -1,    70,    52,
      -1,    76,   251,    -1,   299,    -1,   229,   189,   258,   190,
      43,    -1,   229,   189,   258,   190,    46,   260,    47,    43,
      -1,   229,   189,   258,   190,    46,   260,    47,   241,   182,
     181,    -1,   229,   249,    40,   230,    41,   281,    -1,   195,
      -1,   199,    -1,   200,    -1,   207,    -1,   208,    -1,   218,
      -1,    -1,   229,   278,   288,   269,    46,   196,   211,    47,
     198,    -1,    -1,   229,   278,    46,   197,   211,    47,   241,
     182,   181,    -1,    43,    -1,   241,   182,   181,    -1,   229,
     278,   288,    43,    -1,    -1,   114,   100,   203,   101,   201,
     202,    -1,   114,   278,   288,    -1,    44,   114,   278,   288,
      -1,   180,    -1,   195,    -1,   215,    -1,   219,    -1,   200,
      -1,   199,    -1,   217,    -1,   204,    -1,   205,   206,    -1,
     299,    -1,   277,    -1,   233,    -1,    42,   205,   206,    -1,
     299,    -1,    88,   288,    43,    -1,    88,    89,   288,    43,
      -1,    -1,    89,   288,    46,   209,   139,    47,    -1,    -1,
      89,    46,   210,   139,    47,    -1,    89,     3,    54,   288,
      43,    -1,   214,   211,    -1,    -1,    66,    46,   212,   211,
      47,   211,    -1,   152,    -1,   299,    -1,    -1,     1,   213,
     211,    -1,   178,    -1,   215,    -1,   216,    -1,   219,    -1,
     220,    -1,   224,    -1,   217,    -1,   199,    -1,   221,    -1,
     229,   288,    43,    -1,   207,    -1,   200,    -1,   218,    -1,
     176,    -1,   177,    -1,   227,    -1,   151,    -1,   175,    -1,
      43,    -1,   229,   249,    40,   230,    41,   281,    -1,   134,
     290,    40,   230,    41,   225,    -1,    78,   134,   290,    40,
     230,    41,   226,    -1,   229,   116,   249,   246,    40,   230,
      41,   226,    -1,   229,   116,   249,   125,    40,   230,    41,
     226,    -1,   229,   116,   249,   122,    40,   230,    41,   226,
      -1,   229,   116,   249,   246,   125,    40,   230,    41,   226,
      -1,   229,   116,   249,    40,   230,    41,   226,    -1,    81,
      40,   230,    41,    46,    -1,    84,    40,    -1,    74,    76,
      -1,    73,    76,    -1,    75,    76,    -1,    -1,   229,   278,
     288,   269,    46,   222,   198,    -1,    -1,   229,   278,   269,
      46,   223,   198,    -1,   160,    -1,   146,    -1,   158,    -1,
     163,    -1,   165,    -1,   168,    -1,   156,    -1,   170,    -1,
     144,    -1,   145,    -1,   147,    -1,   280,    43,    -1,   280,
      46,    -1,   280,    43,    -1,   280,    54,   256,    43,    -1,
     280,    46,    -1,   229,   228,    76,   263,    43,    -1,   253,
      -1,    32,    -1,    30,    -1,    34,   285,    -1,    35,    -1,
     288,    -1,    44,    -1,    44,   293,    -1,    44,   293,    86,
      -1,    77,    -1,    21,    -1,    78,    -1,    79,    -1,    82,
      -1,    85,    -1,    86,    -1,    86,    77,    -1,    77,    86,
      -1,    44,    86,    -1,    86,    44,    -1,   299,    -1,   231,
      -1,   233,   232,    -1,   299,    -1,    42,   233,   232,    -1,
     299,    -1,   250,   239,    -1,   114,   100,   278,   101,   278,
     288,   238,    -1,    48,    48,    48,    -1,   235,    -1,   237,
     236,    -1,   299,    -1,    42,   237,   236,    -1,   299,    -1,
     233,    -1,   264,    -1,    54,   256,    -1,    54,   256,    57,
     263,    58,    -1,    54,    46,    -1,    76,   263,    -1,   299,
      -1,   241,   238,    -1,   244,   238,    -1,   238,    -1,   241,
      -1,   244,    -1,   299,    -1,   246,   242,    -1,   246,   125,
     242,    -1,   246,   122,   242,    -1,   243,    -1,   125,   242,
      -1,   122,   242,    -1,   288,   112,   242,    -1,   246,   288,
     112,   242,    -1,   246,   288,   112,   125,   242,    -1,   288,
     112,   125,   242,    -1,   246,    48,    48,    48,   242,    -1,
     246,   125,    48,    48,    48,   242,    -1,   246,   122,    48,
      48,    48,   242,    -1,    48,    48,    48,   243,    -1,   125,
      48,    48,    48,   242,    -1,   122,    48,    48,    48,   242,
      -1,   288,   112,    48,    48,    48,   242,    -1,   246,   288,
     112,    48,    48,    48,   242,    -1,   246,   288,   112,   125,
      48,    48,    48,   242,    -1,   246,   288,   112,   122,    48,
      48,    48,   242,    -1,   288,   112,   125,    48,    48,    48,
     242,    -1,   288,   112,   122,    48,    48,    48,   242,    -1,
     288,    -1,   134,   288,    -1,    40,   288,    41,    -1,    40,
     246,   242,    41,    -1,    40,   288,   112,   242,    41,    -1,
     242,    57,    58,    -1,   242,    57,   263,    58,    -1,   242,
      40,   230,    41,    -1,   288,    -1,   134,   288,    -1,    40,
     246,   243,    41,    -1,    40,   125,   243,    41,    -1,    40,
     122,   243,    41,    -1,    40,   288,   112,   243,    41,    -1,
     243,    57,    58,    -1,   243,    57,   263,    58,    -1,   243,
      40,   230,    41,    -1,   115,     3,    40,   230,    41,    -1,
     246,    -1,   246,   245,    -1,   246,   125,    -1,   246,   122,
      -1,   246,   125,   245,    -1,   246,   122,   245,    -1,   245,
      -1,   125,   245,    -1,   122,   245,    -1,   125,    -1,   122,
      -1,   288,   112,    -1,   246,   288,   112,    -1,   246,   288,
     112,   245,    -1,   245,    57,    58,    -1,   245,    57,   263,
      58,    -1,    57,    58,    -1,    57,   263,    58,    -1,    40,
     244,    41,    -1,   245,    40,   230,    41,    -1,    40,   230,
      41,    -1,   132,   247,   246,    -1,   132,   246,    -1,   132,
     247,    -1,   132,    -1,   248,    -1,   248,   247,    -1,    49,
      -1,    50,    -1,    51,    -1,   250,    -1,   247,   251,    -1,
     251,    -1,   251,   247,    -1,   247,   251,   247,    -1,   253,
      -1,    32,    -1,    30,    -1,    34,   285,    -1,   189,   288,
      -1,    35,    -1,   288,    -1,   278,   288,    -1,   252,    -1,
      87,    40,   288,    41,    -1,   254,    -1,   255,    -1,   255,
     254,    -1,    22,    -1,    24,    -1,    25,    -1,    28,    -1,
      29,    -1,    26,    -1,    27,    -1,    31,    -1,    23,    -1,
      33,    -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,
      -1,   257,   263,    -1,     3,    -1,   299,    -1,   146,    -1,
     299,    -1,   260,    42,   259,   261,   259,    -1,   260,    42,
     259,    -1,   259,   261,   259,    -1,   259,    -1,     3,    -1,
       3,    54,   262,    -1,   263,    -1,   264,    -1,   249,    -1,
     265,    -1,   293,    -1,    55,    40,   249,   239,    41,    -1,
      55,    48,    48,    48,    40,   249,   239,    41,    -1,   266,
      -1,   294,    -1,    11,    -1,    12,    -1,    40,   263,    41,
      -1,    40,   263,    41,   263,    -1,    40,   263,   246,    41,
     263,    -1,    40,   263,   125,    41,   263,    -1,    40,   263,
     122,    41,   263,    -1,    40,   263,   246,   125,    41,   263,
      -1,    40,   263,   246,   122,    41,   263,    -1,   125,   263,
      -1,   122,   263,    -1,   132,   263,    -1,    13,    -1,    14,
      -1,    15,    -1,    16,    -1,    17,    -1,    18,    -1,    19,
      -1,    20,    -1,   263,   129,   263,    -1,   263,   128,   263,
      -1,   263,   132,   263,    -1,   263,   131,   263,    -1,   263,
     130,   263,    -1,   263,   125,   263,    -1,   263,   123,   263,
      -1,   263,   124,   263,    -1,   263,   127,   263,    -1,   263,
     126,   263,    -1,   263,   122,   263,    -1,   263,   121,   263,
      -1,   263,   105,   263,    -1,   263,   106,   263,    -1,   263,
     104,   263,    -1,   263,   103,   263,    -1,   263,   108,   263,
      76,   263,    -1,   128,   263,    -1,   129,   263,    -1,   134,
     263,    -1,   133,   263,    -1,   249,    40,    -1,    48,    48,
      48,    -1,   267,    -1,   299,    -1,   270,    -1,    -1,    76,
     271,   272,    -1,   299,    -1,   273,    -1,   272,    42,   273,
      -1,    -1,   279,   274,   288,   268,    -1,    -1,   279,   276,
     275,   279,   288,   268,    -1,    74,    -1,    73,    -1,    75,
      -1,    71,    -1,    72,    -1,    71,    48,    48,    48,    -1,
      72,    48,    48,    48,    -1,   277,    -1,    52,    -1,    53,
      -1,    78,    -1,   299,    -1,   247,    -1,    80,    40,   230,
      41,    -1,   247,    80,    40,   230,    41,    -1,   299,    -1,
     280,   282,    43,    -1,   280,   282,    46,    -1,    40,   230,
      41,    43,    -1,    40,   230,    41,    46,    -1,    54,   256,
      43,    -1,    76,   283,    -1,   299,    -1,   284,    -1,   283,
      42,   284,    -1,   284,    48,    48,    48,    -1,   283,    42,
     284,    48,    48,    48,    -1,   288,    40,    -1,   288,    46,
      -1,   100,   234,   101,    -1,   299,    -1,     3,    -1,   293,
      -1,   286,    -1,   299,    -1,   290,   289,    -1,   111,   136,
     290,   289,    -1,   290,    -1,   111,   136,   290,    -1,   115,
      -1,   111,   136,   115,    -1,   136,   290,   289,    -1,   136,
     290,    -1,   136,   115,    -1,   113,   290,    -1,     3,   285,
      -1,     3,   292,    -1,   111,   136,     3,   292,    -1,     3,
      -1,   111,   136,     3,    -1,   115,    -1,   111,   136,   115,
      -1,   136,     3,   292,    -1,   136,     3,    -1,   136,   115,
      -1,   113,     3,    -1,   293,     6,    -1,     6,    -1,   294,
       7,    -1,     7,    -1,   293,    -1,    46,    -1,     4,    -1,
      40,   297,    41,    -1,   299,    -1,   286,    54,   298,    -1,
     286,    54,   298,    42,   297,    -1,   286,    -1,   286,    42,
     297,    -1,   286,    54,   149,    -1,   286,    54,   149,    42,
     297,    -1,   293,    -1,   265,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,  1813,  1813,  1826,  1830,  1833,  1836,  1839,  1842,  1847,
    1852,  1857,  1858,  1859,  1860,  1861,  1867,  1883,  1893,  1894,
    1895,  1896,  1897,  1898,  1899,  1900,  1901,  1902,  1903,  1904,
    1905,  1906,  1907,  1908,  1909,  1910,  1911,  1912,  1913,  1920,
    1920,  2006,  2016,  2027,  2048,  2070,  2081,  2090,  2109,  2115,
    2121,  2126,  2133,  2140,  2144,  2157,  2166,  2181,  2194,  2194,
    2249,  2250,  2257,  2276,  2307,  2311,  2321,  2326,  2344,  2384,
    2390,  2403,  2409,  2435,  2441,  2448,  2449,  2452,  2453,  2461,
    2507,  2553,  2564,  2567,  2594,  2600,  2606,  2612,  2620,  2626,
    2632,  2638,  2646,  2647,  2648,  2651,  2656,  2666,  2702,  2703,
    2738,  2755,  2763,  2776,  2801,  2807,  2811,  2814,  2825,  2830,
    2843,  2855,  3137,  3147,  3154,  3155,  3159,  3159,  3184,  3190,
    3201,  3218,  3277,  3334,  3338,  3360,  3366,  3372,  3378,  3384,
    3392,  3393,  3394,  3395,  3396,  3397,  3398,  3409,  3414,  3419,
    3426,  3432,  3437,  3440,  3440,  3453,  3456,  3459,  3468,  3471,
    3478,  3500,  3529,  3627,  3678,  3679,  3680,  3681,  3682,  3683,
    3688,  3688,  3937,  3937,  4060,  4061,  4073,  4091,  4091,  4382,
    4388,  4394,  4397,  4400,  4403,  4406,  4409,  4412,  4417,  4453,
    4457,  4460,  4463,  4468,  4472,  4477,  4487,  4518,  4518,  4547,
    4547,  4569,  4596,  4611,  4611,  4621,  4622,  4623,  4623,  4639,
    4640,  4657,  4658,  4659,  4660,  4661,  4662,  4663,  4664,  4665,
    4666,  4667,  4668,  4669,  4670,  4671,  4672,  4673,  4682,  4707,
    4731,  4762,  4777,  4794,  4812,  4831,  4850,  4857,  4864,  4871,
    4879,  4900,  4900,  4927,  4927,  4963,  4966,  4970,  4973,  4974,
    4975,  4976,  4977,  4978,  4979,  4980,  4983,  4988,  4995,  5003,
    5011,  5022,  5025,  5028,  5029,  5030,  5031,  5033,  5042,  5043,
    5053,  5063,  5064,  5065,  5066,  5067,  5068,  5069,  5070,  5071,
    5072,  5073,  5074,  5081,  5092,  5096,  5099,  5103,  5107,  5117,
    5125,  5133,  5146,  5150,  5153,  5157,  5161,  5189,  5197,  5208,
    5222,  5231,  5239,  5249,  5253,  5257,  5264,  5281,  5298,  5306,
    5314,  5323,  5332,  5336,  5345,  5356,  5367,  5379,  5389,  5403,
    5411,  5420,  5429,  5433,  5442,  5453,  5464,  5476,  5486,  5496,
    5507,  5520,  5527,  5535,  5551,  5559,  5570,  5581,  5592,  5611,
    5619,  5636,  5644,  5651,  5658,  5669,  5680,  5691,  5711,  5732,
    5738,  5744,  5751,  5758,  5767,  5776,  5779,  5788,  5797,  5804,
    5811,  5818,  5828,  5839,  5850,  5861,  5868,  5875,  5878,  5895,
    5905,  5912,  5918,  5923,  5929,  5933,  5939,  5940,  5941,  5947,
    5953,  5957,  5958,  5962,  5969,  5972,  5973,  5974,  5975,  5976,
    5978,  5981,  5984,  5989,  5999,  6024,  6027,  6081,  6085,  6089,
    6093,  6097,  6101,  6105,  6109,  6113,  6117,  6121,  6125,  6129,
    6133,  6139,  6139,  6165,  6166,  6169,  6170,  6174,  6180,  6183,
    6187,  6192,  6200,  6212,  6227,  6228,  6247,  6248,  6252,  6257,
    6262,  6263,  6268,  6280,  6294,  6301,  6318,  6325,  6332,  6339,
    6347,  6355,  6359,  6363,  6369,  6370,  6371,  6372,  6373,  6374,
    6375,  6376,  6379,  6383,  6387,  6391,  6395,  6399,  6403,  6407,
    6411,  6415,  6419,  6423,  6427,  6431,  6445,  6449,  6453,  6459,
    6463,  6467,  6471,  6475,  6491,  6496,  6499,  6504,  6509,  6509,
    6510,  6513,  6530,  6539,  6539,  6557,  6557,  6575,  6576,  6577,
    6581,  6585,  6589,  6593,  6599,  6602,  6606,  6612,  6613,  6616,
    6621,  6626,  6631,  6638,  6645,  6652,  6660,  6668,  6676,  6677,
    6680,  6681,  6682,  6683,  6686,  6698,  6704,  6710,  6713,  6714,
    6717,  6718,  6721,  6726,  6730,  6733,  6736,  6739,  6744,  6748,
    6751,  6758,  6764,  6773,  6778,  6782,  6785,  6788,  6791,  6796,
    6800,  6803,  6806,  6812,  6817,  6820,  6832,  6835,  6838,  6842,
    6847,  6860,  6864,  6869,  6875,  6879,  6884,  6888,  6895,  6898,
    6903
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "HBLOCK", "POUND", "STRING",
  "WSTRING", "INCLUDE", "IMPORT", "INSERT", "CHARCONST", "WCHARCONST",
  "NUM_INT", "NUM_FLOAT", "NUM_UNSIGNED", "NUM_LONG", "NUM_ULONG",
  "NUM_LONGLONG", "NUM_ULONGLONG", "NUM_BOOL", "TYPEDEF", "TYPE_INT",
  "TYPE_UNSIGNED", "TYPE_SHORT", "TYPE_LONG", "TYPE_FLOAT", "TYPE_DOUBLE",
  "TYPE_CHAR", "TYPE_WCHAR", "TYPE_VOID", "TYPE_SIGNED", "TYPE_BOOL",
  "TYPE_COMPLEX", "TYPE_TYPEDEF", "TYPE_RAW", "TYPE_NON_ISO_INT8",
  "TYPE_NON_ISO_INT16", "TYPE_NON_ISO_INT32", "TYPE_NON_ISO_INT64",
  "LPAREN", "RPAREN", "COMMA", "SEMI", "EXTERN", "INIT", "LBRACE",
  "RBRACE", "PERIOD", "CONST_QUAL", "VOLATILE", "REGISTER", "STRUCT",
  "UNION", "EQUAL", "SIZEOF", "MODULE", "LBRACKET", "RBRACKET",
  "BEGINFILE", "ENDOFFILE", "ILLEGAL", "CONSTANT", "NAME", "RENAME",
  "NAMEWARN", "EXTEND", "PRAGMA", "FEATURE", "VARARGS", "ENUM", "CLASS",
  "TYPENAME", "PRIVATE", "PUBLIC", "PROTECTED", "COLON", "STATIC",
  "VIRTUAL", "FRIEND", "THROW", "CATCH", "EXPLICIT", "AUTO",
  "STATIC_ASSERT", "CONSTEXPR", "THREAD_LOCAL", "DECLTYPE", "USING",
  "NAMESPACE", "NATIVE", "INLINE", "TYPEMAP", "EXCEPT", "ECHO", "APPLY",
  "CLEAR", "SWIGTEMPLATE", "FRAGMENT", "WARN", "LESSTHAN", "GREATERTHAN",
  "DELETE_KW", "LESSTHANOREQUALTO", "GREATERTHANOREQUALTO", "EQUALTO",
  "NOTEQUALTO", "ARROW", "QUESTIONMARK", "TYPES", "PARMS", "NONID",
  "DSTAR", "DCNOT", "TEMPLATE", "OPERATOR", "COPERATOR", "PARSETYPE",
  "PARSEPARM", "PARSEPARMS", "CAST", "LOR", "LAND", "OR", "XOR", "AND",
  "RSHIFT", "LSHIFT", "MINUS", "PLUS", "MODULO", "SLASH", "STAR", "LNOT",
  "NOT", "UMINUS", "DCOLON", "$accept", "program", "interface",
  "declaration", "swig_directive", "extend_directive", "$@1",
  "apply_directive", "clear_directive", "constant_directive",
  "echo_directive", "except_directive", "stringtype", "fname",
  "fragment_directive", "include_directive", "$@2", "includetype",
  "inline_directive", "insert_directive", "module_directive",
  "name_directive", "native_directive", "pragma_directive", "pragma_arg",
  "pragma_lang", "rename_directive", "rename_namewarn",
  "feature_directive", "stringbracesemi", "featattr", "varargs_directive",
  "varargs_parms", "typemap_directive", "typemap_type", "tm_list",
  "tm_tail", "typemap_parm", "types_directive", "template_directive",
  "warn_directive", "c_declaration", "$@3", "c_decl", "c_decl_tail",
  "initializer", "cpp_alternate_rettype", "cpp_lambda_decl",
  "lambda_introducer", "lambda_body", "lambda_tail", "$@4", "c_enum_key",
  "c_enum_inherit", "c_enum_forward_decl", "c_enum_decl",
  "c_constructor_decl", "cpp_declaration", "cpp_class_decl", "@5", "@6",
  "cpp_opt_declarators", "cpp_forward_class_decl", "cpp_template_decl",
  "$@7", "cpp_temp_possible", "template_parms", "templateparameters",
  "templateparameter", "templateparameterstail", "cpp_using_decl",
  "cpp_namespace_decl", "$@8", "$@9", "cpp_members", "$@10", "$@11",
  "cpp_member", "cpp_constructor_decl", "cpp_destructor_decl",
  "cpp_conversion_operator", "cpp_catch_decl", "cpp_static_assert",
  "cpp_protection_decl", "cpp_nested", "@12", "@13", "cpp_swig_directive",
  "cpp_end", "cpp_vend", "anonymous_bitfield", "anon_bitfield_type",
  "storage_class", "parms", "rawparms", "ptail", "parm", "valparms",
  "rawvalparms", "valptail", "valparm", "def_args", "parameter_declarator",
  "typemap_parameter_declarator", "declarator", "notso_direct_declarator",
  "direct_declarator", "abstract_declarator", "direct_abstract_declarator",
  "pointer", "type_qualifier", "type_qualifier_raw", "type", "rawtype",
  "type_right", "decltype", "primitive_type", "primitive_type_list",
  "type_specifier", "definetype", "$@14", "ename",
  "optional_constant_directive", "enumlist", "edecl", "etype", "expr",
  "valexpr", "exprnum", "exprcompound", "ellipsis", "variadic", "inherit",
  "raw_inherit", "$@15", "base_list", "base_specifier", "@16", "@17",
  "access_specifier", "templcpptype", "cpptype", "opt_virtual",
  "cpp_const", "ctor_end", "ctor_initializer", "mem_initializer_list",
  "mem_initializer", "template_decl", "idstring", "idstringopt", "idcolon",
  "idcolontail", "idtemplate", "idcolonnt", "idcolontailnt", "string",
  "wstring", "stringbrace", "options", "kwargs", "stringnum", "empty", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   137,   138,   138,   138,   138,   138,   138,   138,   139,
     139,   140,   140,   140,   140,   140,   140,   140,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   143,
     142,   144,   145,   146,   146,   146,   147,   147,   148,   148,
     148,   148,   149,   150,   150,   151,   151,   151,   153,   152,
     154,   154,   155,   155,   156,   156,   156,   156,   157,   158,
     158,   159,   159,   160,   160,   161,   161,   162,   162,   163,
     163,   163,   164,   164,   165,   165,   165,   165,   165,   165,
     165,   165,   166,   166,   166,   167,   167,   168,   169,   169,
     170,   170,   170,   171,   172,   173,   173,   174,   174,   174,
     175,   176,   177,   178,   178,   178,   179,   178,   178,   178,
     178,   180,   180,   181,   181,   181,   182,   182,   182,   182,
     183,   183,   183,   183,   183,   183,   183,   184,   184,   184,
     185,   186,   187,   188,   187,   189,   189,   189,   190,   190,
     191,   192,   192,   193,   194,   194,   194,   194,   194,   194,
     196,   195,   197,   195,   198,   198,   199,   201,   200,   200,
     200,   202,   202,   202,   202,   202,   202,   202,   203,   204,
     204,   205,   205,   206,   206,   207,   207,   209,   208,   210,
     208,   208,   211,   212,   211,   211,   211,   213,   211,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   214,   214,
     214,   214,   214,   214,   214,   214,   214,   214,   215,   216,
     216,   217,   217,   217,   217,   217,   218,   219,   220,   220,
     220,   222,   221,   223,   221,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   225,   225,   226,   226,
     226,   227,   228,   228,   228,   228,   228,   228,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   230,   231,   231,   232,   232,   233,   233,
     233,   234,   235,   235,   236,   236,   237,   237,   238,   238,
     238,   238,   238,   239,   239,   239,   240,   240,   240,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   241,   241,   241,   241,   241,   241,   241,   241,   241,
     241,   242,   242,   242,   242,   242,   242,   242,   242,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   245,   245,   245,   245,   245,   245,   245,
     246,   246,   246,   246,   247,   247,   248,   248,   248,   249,
     250,   250,   250,   250,   251,   251,   251,   251,   251,   251,
     251,   251,   251,   252,   253,   254,   254,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   255,   255,   255,   255,
     255,   257,   256,   258,   258,   259,   259,   260,   260,   260,
     260,   261,   261,   262,   263,   263,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   264,
     264,   264,   264,   264,   265,   265,   265,   265,   265,   265,
     265,   265,   266,   266,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   266,   266,   266,   266,   266,   266,
     266,   266,   266,   266,   267,   268,   268,   269,   271,   270,
     270,   272,   272,   274,   273,   275,   273,   276,   276,   276,
     277,   277,   277,   277,   278,   278,   278,   279,   279,   280,
     280,   280,   280,   281,   281,   281,   281,   281,   282,   282,
     283,   283,   283,   283,   284,   284,   285,   285,   286,   286,
     287,   287,   288,   288,   288,   288,   288,   288,   289,   289,
     289,   289,   290,   291,   291,   291,   291,   291,   291,   292,
     292,   292,   292,   293,   293,   294,   294,   295,   295,   295,
     296,   296,   297,   297,   297,   297,   297,   297,   298,   298,
     299
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     3,     2,     3,     2,     5,     3,     2,
       1,     1,     1,     1,     1,     1,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       7,     5,     3,     5,     5,     3,     2,     2,     5,     2,
       5,     2,     4,     1,     1,     7,     7,     5,     0,     7,
       1,     1,     2,     2,     1,     5,     5,     5,     3,     4,
       3,     7,     8,     5,     3,     1,     1,     3,     1,     4,
       7,     6,     1,     1,     7,     9,     8,    10,     5,     7,
       6,     8,     1,     1,     5,     4,     5,     7,     1,     3,
       6,     6,     8,     1,     2,     3,     1,     2,     3,     6,
       5,     9,     2,     1,     1,     1,     0,     6,     1,     3,
       7,     5,     7,     1,     4,     1,     1,     2,     5,     6,
       1,     1,     1,     2,     1,     1,     1,    11,    13,     7,
       1,     1,     1,     0,     3,     1,     2,     2,     2,     1,
       5,     8,    10,     6,     1,     1,     1,     1,     1,     1,
       0,     9,     0,     9,     1,     3,     4,     0,     6,     3,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     3,     1,     3,     4,     0,     6,     0,
       5,     5,     2,     0,     6,     1,     1,     0,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     6,
       7,     8,     8,     8,     9,     7,     5,     2,     2,     2,
       2,     0,     7,     0,     6,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     4,
       2,     5,     1,     1,     1,     2,     1,     1,     1,     2,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     2,     1,     1,     2,     1,     3,     1,     2,     7,
       3,     1,     2,     1,     3,     1,     1,     1,     2,     5,
       2,     2,     1,     2,     2,     1,     1,     1,     1,     2,
       3,     3,     1,     2,     2,     3,     4,     5,     4,     5,
       6,     6,     4,     5,     5,     6,     7,     8,     8,     7,
       7,     1,     2,     3,     4,     5,     3,     4,     4,     1,
       2,     4,     4,     4,     5,     3,     4,     4,     5,     1,
       2,     2,     2,     3,     3,     1,     2,     2,     1,     1,
       2,     3,     4,     3,     4,     2,     3,     3,     4,     3,
       3,     2,     2,     1,     1,     2,     1,     1,     1,     1,
       2,     1,     2,     3,     1,     1,     1,     2,     2,     1,
       1,     2,     1,     4,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     2,     1,     1,     1,     1,     5,     3,     3,
       1,     1,     3,     1,     1,     1,     1,     1,     5,     8,
       1,     1,     1,     1,     3,     4,     5,     5,     5,     6,
       6,     2,     2,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     2,
       2,     2,     2,     2,     3,     1,     1,     1,     0,     3,
       1,     1,     3,     0,     4,     0,     6,     1,     1,     1,
       1,     1,     4,     4,     1,     1,     1,     1,     1,     1,
       4,     5,     1,     3,     3,     4,     4,     3,     2,     1,
       1,     3,     4,     6,     2,     2,     3,     1,     1,     1,
       1,     1,     2,     4,     1,     3,     1,     3,     3,     2,
       2,     2,     2,     2,     4,     1,     3,     1,     3,     3,
       2,     2,     2,     2,     1,     2,     1,     1,     1,     1,
       3,     1,     3,     5,     1,     3,     3,     5,     1,     1,
       0
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
     550,     0,     0,     0,     0,     0,    10,     4,   550,   387,
     395,   388,   389,   392,   393,   390,   391,   376,   394,   375,
     396,   550,   379,   397,   398,   399,   400,     0,   366,   367,
     368,   485,   486,   145,   480,   481,     0,     0,     0,   516,
       0,     0,     0,   364,   550,   371,   382,   374,   384,   385,
     484,     0,   380,   514,     6,     0,     0,   550,     1,    15,
      64,    60,    61,     0,   262,    14,   258,   550,     0,     0,
      82,    83,   550,   550,     0,     0,   261,   263,   264,     0,
     265,   266,   267,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     9,    11,    18,
      19,    20,    21,    22,    23,    24,    25,   550,    26,    27,
      28,    29,    30,    31,    32,     0,    33,    34,    35,    36,
      37,    38,    12,   113,   118,   115,   114,    16,    13,   154,
     155,   156,   157,   158,   159,     0,   272,   550,   522,   507,
     377,     0,   147,   146,     0,     0,     0,     0,     0,   378,
       3,   370,   365,   550,     0,   401,     0,     0,   516,   349,
     348,   363,     0,   295,   278,   550,   302,   550,   345,   339,
     329,   292,   372,   386,   381,     0,     0,   512,     5,     8,
       0,   273,   550,   275,    17,     0,   534,   270,     0,   259,
       0,     0,   541,     0,   550,     0,   369,     0,     0,     0,
       0,    78,     0,   550,   269,   550,   271,   268,     0,     0,
     550,   189,     0,     0,    62,    63,     0,     0,    51,    49,
      46,    47,   550,     0,   550,     0,   550,   550,     0,   112,
     550,   550,     0,     0,     0,     0,     0,     0,     0,   329,
       0,   550,     0,     0,   536,   422,   423,   434,   435,   436,
     437,   438,   439,   440,   441,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   286,     0,   281,   550,   415,   369,
       0,   414,   416,   420,   417,   421,   283,   280,     0,     0,
       0,   517,   515,     0,   373,   550,   349,   348,     0,     0,
     339,   380,     0,   290,   288,     0,   355,     0,   414,   291,
       0,   550,     0,     0,   304,   347,   321,     0,   303,   346,
     361,   362,   330,   293,   550,     0,   294,   550,     0,     0,
     342,   341,   299,   340,   321,   350,   521,   520,   519,     0,
       0,   274,   277,   508,     0,   509,     0,   533,   116,   260,
     544,     0,    68,    45,   401,     0,   550,    70,     0,     0,
       0,    74,     0,     0,     0,    98,     0,     0,   185,   119,
       0,   550,   187,     0,     0,   103,     0,     0,     0,   107,
     296,   297,   298,    42,     0,   104,   106,   510,     0,   511,
      54,     0,    53,     0,     0,   178,   550,   182,   484,   180,
     169,     0,   508,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   321,     0,     0,   329,   550,   550,   404,   550,
     550,   162,   381,     0,     0,     0,   432,   431,   459,   460,
     433,   462,   461,   506,     0,   282,   285,   463,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   535,   482,   483,   383,   513,
       0,   349,   348,   339,   380,     0,   329,     0,   359,   357,
     342,   341,     0,   329,   350,     0,     0,   402,   356,   550,
     339,   380,     0,   322,   550,     0,     0,   360,     0,   335,
       0,     0,   353,     0,     0,     0,   301,   344,     0,   300,
     343,   351,     0,     0,     0,   305,   518,     7,   550,     0,
     170,   550,     0,     0,   540,     0,     0,    69,    39,    77,
       0,     0,     0,     0,     0,     0,     0,   186,     0,     0,
     550,   550,     0,     0,   108,     0,   550,     0,     0,     0,
       0,     0,   167,     0,   179,   184,    58,     0,     0,     0,
       0,    79,     0,     0,     0,     0,     0,   149,     0,   380,
       0,     0,   126,   550,     0,   166,   468,     0,   467,   470,
     424,     0,     0,   363,     0,   550,     0,   550,   457,   456,
     454,   455,     0,   453,   452,   448,   449,   447,   451,   450,
     443,   442,   446,   445,   444,     0,     0,   350,   333,   332,
     331,   351,     0,   312,     0,     0,     0,   321,   323,   350,
       0,     0,   326,     0,     0,   337,   336,   358,   354,     0,
       0,     0,     0,     0,     0,   306,   352,     0,     0,     0,
     308,   276,    66,    67,    65,     0,   545,   546,   549,   548,
     542,    43,    44,     0,    76,    73,    75,   539,    93,   538,
       0,    88,   550,   537,    92,     0,   548,     0,     0,    99,
     550,   226,   191,   190,     0,   258,     0,     0,    50,    48,
     550,    41,   105,   525,     0,   527,     0,    57,     0,     0,
     110,     0,   550,   550,   550,   550,     0,     0,   132,   131,
     550,   134,   550,   136,   130,   135,   140,     0,   148,   150,
     550,   550,   550,     0,   123,   125,   121,     0,   127,   197,
     217,     0,     0,     0,     0,   263,     0,     0,   243,   244,
     236,   245,   215,   195,   241,   237,   235,   238,   239,   240,
     242,   216,   212,   213,   199,   206,   210,   209,     0,     0,
     200,   201,   205,   211,   202,   203,   207,   204,   214,     0,
     272,   550,   160,   425,     0,     0,   363,   362,     0,     0,
       0,     0,     0,   284,     0,   550,   334,   289,   338,   324,
       0,   314,   328,   327,   313,   309,     0,     0,     0,     0,
       0,   307,     0,     0,     0,   117,     0,     0,     0,   550,
       0,   489,     0,   492,     0,     0,     0,     0,    90,   550,
       0,   188,   259,   550,     0,   101,     0,   100,     0,     0,
       0,   523,     0,   550,     0,    52,     0,   258,     0,   171,
     172,   176,   175,   168,   173,   177,   174,     0,   183,     0,
       0,    81,   133,     0,   550,   141,     0,   405,   410,     0,
     406,   550,   401,   550,   153,     0,   550,   550,     0,   193,
     229,   228,   230,     0,   227,     0,     0,   192,   376,   375,
     550,   379,     0,     0,     0,   374,   550,   380,   487,   469,
     471,   473,   488,     0,   428,   427,   426,     0,     0,   418,
       0,   458,   279,   325,   311,   310,     0,     0,     0,   315,
       0,     0,   547,   543,    40,     0,   550,     0,    84,   548,
      95,    89,   550,     0,     0,    97,    71,     0,     0,   109,
     532,   530,   531,   526,   528,     0,    55,    56,   120,   550,
       0,    59,    80,   122,     0,   143,   142,   139,   411,   550,
     550,     0,     0,     0,     0,     0,   499,   550,     0,     0,
     198,     0,     0,   550,   550,   377,     0,     0,   550,     0,
     381,   208,   550,   478,   477,   479,     0,   475,     0,   430,
     429,   550,   316,     0,     0,   320,   319,     0,     0,   550,
      96,     0,    91,   550,    86,    72,   102,   529,   524,     0,
       0,   550,     0,     0,   409,   408,   151,   550,     0,   497,
     498,   500,     0,   493,   494,   128,   124,   550,     0,   550,
       0,     0,   550,     0,     0,     0,     0,     0,   233,     0,
     472,   550,   550,     0,     0,   318,   317,    94,   490,     0,
      85,     0,   111,   167,     0,   144,   412,   413,   550,     0,
     495,   496,     0,     0,   504,   505,   129,     0,     0,   550,
     163,     0,   550,   550,   550,     0,   251,   550,     0,   231,
       0,   465,   474,   466,     0,   164,   161,   550,   419,   491,
      87,     0,     0,   407,   152,   501,     0,   194,   550,   219,
       0,   550,     0,     0,     0,   550,   218,   234,     0,     0,
     550,     0,     0,   137,     0,   502,   220,     0,   246,   247,
     225,   550,   550,   550,     0,   232,   464,   476,   165,     0,
       0,   248,   250,   401,   223,   222,   221,   550,   138,   503,
       0,   224,   249
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,    97,    98,    99,   633,   708,   709,   710,
     711,   104,   380,   381,   712,   713,   674,   107,   108,   714,
     110,   715,   112,   716,   635,   200,   717,   115,   718,   641,
     513,   719,   354,   720,   364,   225,   375,   226,   721,   722,
     723,   724,   501,   123,   696,   551,   682,   124,   687,   826,
     917,   972,    40,   546,   125,   126,   127,   128,   129,   863,
     554,  1046,   725,   726,   672,   813,   384,   385,   386,   534,
     727,   133,   520,   361,   728,   931,   838,   729,   730,   731,
     732,   733,   734,   735,   736,  1068,  1038,   737,  1059,  1076,
     738,   853,   739,   288,   181,   331,   182,   265,   266,   425,
     267,   552,   164,   369,   165,   304,   166,   167,   168,   238,
      42,    43,   268,   196,    45,    46,    47,    48,    49,   294,
     295,   407,   828,   829,   919,  1016,   270,   298,   272,   273,
    1041,  1042,   557,   558,   741,   859,   860,   946,  1002,   947,
      50,    51,   861,  1077,   834,   925,   980,   981,   138,   340,
     378,    52,   177,    53,   666,   801,   274,   275,   644,   191,
     341,   630,   183
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -914
static const yytype_int16 yypact[] =
{
     680,  3942,  4036,   123,   146,  3407,  -914,  -914,   -23,  -914,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,
    -914,   -23,  -914,  -914,  -914,  -914,  -914,   158,  -914,  -914,
    -914,  -914,  -914,   362,   207,   216,   114,   -45,   198,  -914,
      85,   299,  4757,   810,  1105,   810,  -914,  -914,  -914,  1961,
    -914,    85,  -914,   267,  -914,   320,   383,  4411,  -914,   318,
    -914,  -914,  -914,   429,  -914,  -914,   107,   433,  4130,   452,
    -914,  -914,   433,   489,   493,   505,   469,  -914,  -914,   510,
    -914,  -914,   237,   212,    87,   519,   262,   528,   592,   574,
    4481,  4481,   546,   553,   630,   603,   706,  -914,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,   433,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  1320,  -914,  -914,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  4551,  -914,  2166,  -914,  -914,
    -914,   616,  -914,  -914,   619,   623,    85,    37,   682,  -914,
    -914,   810,  -914,  1750,   640,   646,  2290,  3034,   703,   161,
     341,    65,    85,  -914,  -914,   261,   327,   261,   331,   361,
     599,  -914,  -914,  -914,  -914,   709,    40,  -914,  -914,  -914,
     676,  -914,   681,  -914,  -914,   438,  -914,  -914,   682,   199,
     438,   438,  -914,   684,   233,  1353,  -914,   354,    85,   730,
     736,  -914,   438,  4341,  -914,  4411,  -914,  -914,    85,   278,
     248,  -914,   698,   744,  -914,  -914,   438,   761,  -914,  -914,
    -914,   766,  4411,   721,   356,   742,   737,   438,   630,   766,
    4411,  4411,    85,   630,   641,   460,   888,   438,   137,   692,
    1353,   163,  1371,   219,  -914,  -914,  -914,  -914,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  3034,   368,  3034,  3034,  3034,
    3034,  3034,  3034,  3034,  -914,   687,  -914,   750,   767,  1177,
    1482,   157,  -914,  -914,   766,   802,  -914,  -914,   762,   764,
     774,  -914,   267,   717,  -914,  1908,   117,   117,   779,   780,
     447,   712,   778,  -914,   770,  3034,  -914,  3895,  -914,  1482,
     790,  1908,   786,    85,   345,   331,  -914,   793,   345,   331,
    -914,   715,  -914,  -914,  4411,  2414,  -914,  4411,  2538,   796,
    1084,  1335,   345,   331,   741,   185,  -914,  -914,   267,   805,
    4411,  -914,  -914,  -914,   821,   766,    85,  -914,  -914,  -914,
     394,   823,  -914,  -914,  -914,   313,   261,  -914,   825,   826,
     828,   817,    81,   836,   838,  -914,   840,   845,  -914,  -914,
      85,  -914,  -914,   852,   854,  -914,   859,   861,  4481,  -914,
    -914,  -914,  -914,  -914,  4481,  -914,  -914,  -914,   865,  -914,
    -914,   443,    80,   871,   804,  -914,   873,  -914,   162,  -914,
    -914,    47,   241,   139,   139,   139,   801,   877,    58,   880,
     975,  1117,   818,   185,   822,    31,   293,   856,  -914,  3228,
    1330,  -914,   351,  1548,  4621,   885,  2632,  1665,  -914,  -914,
    -914,  -914,  -914,  -914,  2166,  -914,  -914,  -914,  3034,  3034,
    3034,  3034,  3034,  3034,  3034,  3034,  3034,  3034,  3034,  3034,
    3034,  3034,  3034,  3034,  3034,  -914,  -914,  -914,  -914,  -914,
     682,   397,   397,   981,   831,   439,  -914,   499,  -914,  -914,
     397,   397,   507,   832,   139,   139,  3034,  1482,  -914,  4411,
     864,    13,   887,  -914,  4411,  2662,   889,  -914,   897,  -914,
    3973,   906,  -914,  4067,   902,   904,   345,   331,   909,   345,
     331,   765,   912,   914,  1325,   345,  -914,  -914,   681,   369,
    -914,  -914,   438,  1881,  -914,   911,   922,  -914,  -914,  -914,
     611,   240,  1731,   923,  4411,  1353,   924,  -914,   929,  3516,
    -914,   513,  4481,   456,   933,   934,   737,   232,   940,   438,
    4411,   503,   900,  4411,  -914,  -914,  -914,   139,   834,   328,
      35,  -914,   921,  4823,   919,  4757,   520,  -914,   944,   801,
     952,   738,  -914,   366,  2042,  -914,  -914,   948,  -914,  -914,
    3034,  2786,  2910,  3158,   112,  1105,   957,   750,  1259,  1259,
    1586,  1586,  2260,  2508,  2632,  2739,  1493,  1665,  1055,  1055,
     754,   754,  -914,  -914,  -914,    85,   832,  -914,  -914,  -914,
    -914,   397,   547,   327,  4294,   966,   605,   832,  -914,   328,
     328,   967,  -914,  4434,   328,  -914,  -914,  -914,  -914,   328,
     962,   964,   968,   971,  1356,   345,   331,   977,   978,   980,
     345,  -914,  -914,  -914,   766,  3625,  -914,   972,  -914,    80,
     988,  -914,  -914,  2042,  -914,  -914,   766,  -914,  -914,  -914,
     991,  -914,   464,   766,  -914,   963,    43,   470,   240,  -914,
     464,  -914,  -914,  -914,  3734,   105,  4691,   531,  -914,  -914,
    4411,  -914,  -914,   269,   899,  -914,   949,  -914,   992,   997,
    -914,    85,  1460,   873,  -914,   464,   152,   328,  -914,  -914,
     -23,  -914,  1330,  -914,  -914,  -914,  -914,   265,  -914,  -914,
     983,   720,  4411,  1353,  -914,  -914,  -914,  1010,  -914,  -914,
    -914,  1005,   984,   986,   987,   920,  1017,   709,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  1012,  2042,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  -914,  4200,
    1018,   990,  -914,  1482,  3034,  3034,  3158,  3294,  3034,  1023,
    1028,  1030,  1032,  -914,  3034,   261,  -914,  -914,  -914,  -914,
     658,   345,  -914,  -914,   345,   345,   328,   328,  1026,  1027,
    1031,   345,   328,  1037,  1040,  -914,   438,   438,  1029,  4411,
    1038,  1009,   503,  -914,  1881,   743,   438,  1050,  -914,   464,
    1051,  -914,   106,    17,  1353,  -914,  4481,  -914,  1052,  1092,
      60,  -914,    71,  2166,   280,  -914,  1044,   107,   723,  -914,
    -914,  -914,  -914,  -914,  -914,  -914,  -914,  4270,  -914,  3843,
    1056,  -914,  -914,   738,  4411,  -914,   463,  -914,  1101,   485,
    -914,  4411,  -914,  1034,  -914,  1064,  1330,  4411,  2042,  -914,
    -914,  -914,  -914,   709,  -914,  1067,  1353,  -914,  1035,  1041,
     -23,  1042,  4621,  1045,  1425,  1047,    92,   335,  -914,  1071,
    -914,   385,  -914,  2042,  1482,  1482,  1482,  3034,  3034,  -914,
    4621,  2384,  -914,  -914,   345,   345,   328,  1066,  1078,   345,
     328,   328,  -914,  -914,  -914,  1086,  4411,  1093,  -914,   766,
    1095,  -914,   464,  1149,   503,  -914,  -914,  1091,  1096,  -914,
    -914,   269,  -914,   269,  -914,  1039,  -914,  -914,  -914,  4411,
    1353,  -914,  -914,  -914,  1102,  -914,  -914,  -914,  1090,   983,
     983,  1193,  1107,  1103,    85,   563,  -914,   261,   738,  1108,
    -914,  2042,  1110,  4411,  1330,  1075,    -4,  3034,  3228,  1109,
     351,  -914,   990,  -914,  -914,  -914,    85,  -914,  1111,  1482,
    1482,  1105,   345,   328,   328,   345,   345,  1113,  1119,  4411,
    -914,  1120,  -914,   464,  -914,  -914,  -914,  -914,  -914,  1123,
    1053,   464,  1125,  3034,  -914,  1101,  -914,  1330,   578,  -914,
    1127,  1122,   409,  -914,  -914,  -914,  -914,   261,  1124,  4411,
    1134,   738,  4411,  1136,  1148,    10,  2119,  1150,  -914,  1144,
    -914,  1146,   990,  1292,  1157,   345,   345,  -914,  -914,  1159,
    -914,  1163,  -914,  -914,    25,  -914,  -914,  1482,   983,   738,
    -914,  -914,    85,  1154,  -914,  -914,  -914,  2042,  1167,   464,
    -914,  1172,  4411,  4411,  4411,  1174,  -914,   720,  1292,  -914,
    1175,  -914,  -914,  -914,    85,  -914,  -914,  1330,  -914,  -914,
    -914,  4621,   463,  -914,  -914,  1176,  1181,  -914,   464,  -914,
     610,   464,  1194,  1199,  1201,  4411,  -914,  -914,  1292,  1197,
    1146,   738,  1169,  -914,  1198,  -914,  -914,   451,  -914,  -914,
    -914,   464,   464,   464,  1208,  -914,  -914,  -914,  -914,   463,
    1202,  -914,  -914,  -914,  -914,  -914,  -914,   464,  -914,  -914,
    1179,  -914,  -914
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -914,  -914,  -339,  -914,  -914,  -914,  -914,    -3,    23,     6,
      24,  -914,   749,  -914,    32,    33,  -914,  -914,  -914,    51,
    -914,    54,  -914,    64,  -914,  -914,    70,  -914,    77,  -507,
    -622,    78,  -914,    82,  -914,  -338,   732,   -77,    84,    88,
      89,    91,  -914,   589,  -777,  -664,  -914,  -914,  -914,  -913,
    -840,  -914,  -128,  -914,  -914,  -914,  -914,  -914,   590,  -914,
    -914,  -815,    -2,    12,  -914,  -914,   357,  -914,   734,   595,
      95,  -914,  -914,  -914,  -602,  -914,  -914,  -914,   591,  -914,
     593,    97,   597,  -914,  -914,  -914,  -914,  -914,  -914,  -442,
    -914,  -914,     9,   288,  -914,   775,    46,   472,  -914,   718,
     855,   -29,  -561,  -914,   -64,  1088,  -245,  -114,   -30,    55,
      19,  -914,   -56,     8,   -22,   733,  -510,  1233,  -914,  -343,
    -914,  -914,  -862,  -914,   309,  -914,  1087,  -118,  -480,  -914,
    -914,   217,  -751,  -914,  -914,  -914,   344,  -914,  -914,  -914,
    -205,   -80,   289,  -572,   256,  -914,  -914,   274,   -15,   -93,
    -914,   969,    21,   -95,  -914,  -273,   945,  -914,   643,   175,
    -150,  -485,     0
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -551
static const yytype_int16 yytable[] =
{
       6,   505,   100,   130,   751,   136,   140,   241,   139,    44,
      44,   102,   195,   223,   135,   163,   232,   131,   823,   271,
     151,   139,   519,   628,   670,   787,   388,   647,   101,   103,
     525,   778,   628,   684,   224,   224,   992,   105,   106,   289,
       8,   455,   457,     8,   171,   462,   913,    41,    55,   337,
    1034,   237,   282,   337,   598,   243,   109,   974,   975,   111,
     896,     8,   152,   901,   172,    44,   365,   192,   283,   113,
     782,   825,   192,   201,   903,   114,   598,   137,   790,   242,
     326,   328,   116,   117,   397,   544,   337,   118,     8,   119,
     210,   147,   334,   120,   121,     8,   122,  -509,   342,   169,
     132,  1052,   134,   820,   348,   939,   536,   192,   336,   352,
     371,   186,   337,   186,    28,    29,    30,   137,   993,   833,
       8,   994,   511,   512,    56,   599,   530,   847,   161,   305,
     309,   346,  1051,   211,   377,  1035,   313,   276,   316,   323,
       8,   788,     8,   403,   399,   269,    58,   677,   455,   457,
     462,   986,   281,   748,   146,   327,  1053,   153,   337,  1089,
     370,    44,   625,    57,     8,   171,   406,   171,   556,    37,
     284,   289,   928,    39,   156,   902,   404,   398,   410,   345,
     311,   654,   332,   264,   657,   319,   904,   289,     8,   999,
     161,   187,   339,   187,   139,   821,    37,   161,    37,  -287,
      39,   301,    39,    37,  -181,   337,   141,    39,   290,   302,
     139,    44,  1073,    44,  1030,     8,   310,   894,   156,   592,
     593,   188,     8,  1067,   372,   398,   376,   379,    37,   855,
      44,   389,   158,   492,   749,   663,   930,   750,    44,    44,
     163,   408,  1054,     8,   637,   338,   186,   198,    37,  1098,
      37,   162,    39,  1085,   158,   144,   305,   309,  -287,   400,
     323,   948,   401,  -181,   145,   411,   214,   426,   960,   171,
     991,   303,    37,   162,    37,   888,    39,   387,    39,   169,
     345,   206,   233,   638,   906,   339,   639,   344,   154,   395,
     487,   490,   592,    44,  1088,   303,    37,   526,   148,   890,
      39,   208,   360,   449,   628,   824,   271,   493,   215,    44,
     494,   825,   224,  1019,   207,   155,     8,   506,   224,   303,
     961,   358,    44,    37,   169,    44,   907,    39,   388,   988,
      37,     8,   359,   137,    39,   819,  -403,   157,    44,  -403,
     453,   137,   150,   664,     8,   180,   171,   665,   137,   496,
     640,    37,   626,  -550,  -550,   158,   470,   333,   565,     8,
     186,     6,   235,   178,     8,   236,   477,   314,   398,  -403,
     585,   317,   161,   622,   162,   186,   498,  -550,   941,   668,
     175,   301,   799,  1071,   315,   474,   535,   964,   318,   307,
    1004,  1011,   139,   137,   555,   347,   153,  -550,   156,  1014,
     395,   301,   475,   176,   154,   800,   139,   547,   414,   319,
     171,  -257,   559,   156,   142,   623,   415,    44,   156,   645,
     155,   305,   309,   323,    37,  1057,   179,   556,    39,   553,
     487,   490,   269,   143,   184,   393,   502,   285,   394,    37,
     323,   333,   157,    39,   186,   161,   697,   642,   503,  1024,
       8,   650,    37,   539,   156,  1025,    39,  1060,   943,   944,
     945,   616,   303,     8,   395,   833,   224,    37,   564,   185,
     264,   158,    37,   190,   675,   303,    39,    44,   159,   314,
     588,   160,    44,   320,   528,   529,   321,   153,   161,   923,
     162,   355,   197,   356,  1091,   303,   315,  1092,   332,   658,
     398,     6,   659,   915,   156,  1093,   916,   637,   302,   186,
     367,   785,   786,    28,    29,    30,   100,   130,   383,   136,
       6,   136,    44,   688,   698,   102,   376,   920,   135,   199,
     656,   131,   921,   202,    64,   637,   163,   186,    44,   314,
     589,    44,   101,   103,   780,   203,   638,   314,   590,   639,
     205,   105,   106,   171,   740,   204,   315,   655,    37,   213,
     649,   616,   158,   689,   315,   171,   690,   426,   216,   460,
     109,    37,   461,   111,   795,    39,   669,   639,   220,   387,
     186,   162,   747,   113,   789,   796,   227,   314,   756,   114,
      76,    77,    78,   228,   303,    80,   116,   117,    81,    82,
     794,   118,   478,   119,   315,   481,   983,   120,   121,   984,
     122,   241,   845,   640,   132,   634,   134,   186,   310,  1080,
     169,  1020,   100,   130,  1021,   136,   882,   883,   967,   836,
     968,   102,   217,   740,   135,   218,   186,   131,   219,  1094,
    1095,  1096,   783,   230,   392,   474,   759,   186,   101,   103,
     783,   100,   130,  1078,   136,  1101,  1079,   105,   106,   856,
     102,   781,   475,   135,   277,   822,   131,   278,    44,   781,
     811,   279,   136,   535,     6,   783,   109,   101,   103,   111,
     139,   817,   171,   854,   812,   271,   105,   106,   292,   113,
     830,   783,   293,   645,   781,   114,   827,   548,   474,   873,
      44,   553,   116,   117,   388,   109,   300,   118,   111,   119,
     781,   325,     8,   120,   121,   475,   122,   329,   113,   898,
     132,   892,   134,   330,   114,   151,   872,   343,   232,   740,
     897,   116,   117,   350,    31,    32,   118,   243,   119,   351,
     224,   862,   120,   121,   362,   122,     8,   363,   932,   132,
    1100,   134,    37,    34,    35,   171,    39,   595,    31,    32,
     831,   854,   601,   393,   366,   747,   394,   368,     8,    28,
      29,    30,   337,   161,   832,    31,    32,    34,    35,   374,
     693,   694,   934,   345,   695,   373,   891,    44,   423,   783,
     410,   154,   424,   139,    34,    35,   936,     1,     2,     3,
     780,   310,   477,   276,   403,   301,   231,   427,   781,   445,
     446,   269,   447,   612,   951,   448,   100,   130,   450,   136,
     458,   459,   156,   909,   464,   102,   465,   466,   135,   963,
     469,   131,    44,   926,   472,   935,   171,     8,   740,    44,
     186,   476,   101,   103,   484,    44,   404,   161,   497,   264,
     139,   105,   106,   491,    37,   553,   559,   977,   158,    28,
      29,    30,   499,   740,   504,   235,   507,     8,   236,   509,
     109,   510,   508,   111,   345,   161,    37,   162,   514,   515,
      39,   516,   154,   113,   442,   443,   444,   613,   517,   114,
     614,     8,   783,   521,    44,   522,   116,   117,   985,   303,
     523,   118,   524,   119,   301,   532,   527,   120,   121,   389,
     122,   781,   531,   537,   132,   533,   134,    44,   538,   830,
     830,   156,   163,   541,     8,   827,   827,   171,   398,   543,
     542,   740,   545,   566,   171,   600,   307,   604,   605,  1047,
     559,    44,   862,   587,   591,    37,    44,   607,   798,   158,
     609,   171,   610,   553,   631,   387,   235,   611,  1026,   236,
     617,   398,   618,   783,   648,   632,   161,    44,   162,   612,
     651,   783,   652,   660,  1047,    37,   686,   171,     8,    39,
     835,   661,   781,   667,     8,   691,   460,   171,   671,   461,
     781,   995,   692,   395,   742,  1072,   553,    44,   303,    37,
      44,  1043,   862,    39,  1047,   752,   169,   758,   762,   149,
     766,   189,   767,   170,   776,   398,   768,   784,   830,   769,
     174,   285,   303,   485,   827,   772,   773,   740,   774,   783,
     777,   779,    37,   804,   221,   802,    39,   783,   156,   229,
      44,    44,    44,   613,   805,    68,   614,   171,   781,   803,
     837,   839,   209,   212,   843,   303,   781,   844,   783,   846,
     840,   783,   841,   842,   867,  -196,   553,   885,   858,   868,
    1043,   869,   870,    44,   876,   877,   884,   781,   886,   878,
     781,   783,   783,   783,   239,   880,    37,     8,   881,   887,
      39,   893,    37,   899,   895,   900,    39,   783,   908,   912,
     781,   781,   781,   460,   918,   927,   461,   933,     8,   303,
     924,  -254,   914,   942,   953,   280,   781,  -253,  -256,   922,
       8,   937,   291,  -252,   301,   929,   954,   957,   306,   306,
     335,   312,   485,   959,   965,   335,   335,   786,   324,   966,
     969,   156,   335,   971,   973,   153,   979,   335,   978,   987,
     989,  -255,     8,   154,  1013,   998,  1007,   398,  1003,   155,
    1008,   335,   156,  1010,   239,   488,  1012,   349,  1015,  1022,
    1023,  1027,   335,   382,   958,  1029,  1032,   357,   391,   335,
       8,   157,   335,   440,   441,   442,   443,   444,  1033,   345,
    1039,  1037,   962,   170,  1040,    37,     8,   154,  1048,    39,
    1049,   390,  1056,   396,   306,   306,  1050,   402,  1058,   405,
     149,   239,   412,  1061,  1065,   825,    37,   153,   303,  -550,
     158,   990,  1102,  1069,  1074,   154,   997,   159,    37,  1075,
     160,   155,    39,   345,   156,  1081,   976,   161,   170,   162,
    1082,   154,  1083,   297,   299,  1086,  1090,  1009,   308,  1097,
    1099,   303,   627,   157,   454,   456,   456,   322,   662,   463,
      37,   809,   810,   814,   158,   815,   970,   673,   818,   816,
     471,   235,   473,   621,   236,   905,   683,  1028,  -550,   567,
    1031,   161,   173,   162,  1018,   753,  1000,  1087,    37,   306,
     306,  1044,   158,  1066,   306,     8,  1055,     0,     0,   159,
     797,     0,   160,     0,    37,   500,     0,     0,   158,   161,
       0,   162,     0,     0,   396,   235,     0,     0,   236,     0,
    1062,  1063,  1064,     8,   308,   161,   322,   162,     8,   518,
       0,     0,   345,     0,     0,  1045,     0,     0,     8,     0,
     154,     0,   413,     0,   416,   417,   418,   419,   420,   421,
     422,     0,     0,  1084,     0,     0,     8,     0,     0,     8,
     234,     0,   456,   456,   456,   398,     0,   540,   154,   306,
     306,     0,   306,   619,     8,   301,     0,     0,   549,    28,
      29,    30,   467,   488,   155,   438,   439,   440,   441,   442,
     443,   444,   156,   345,     0,     0,   398,     0,     0,     0,
       0,   154,   480,    37,   770,   483,   157,   158,   486,   489,
     550,   409,     0,   495,   235,     0,     0,   236,     0,   154,
       0,     0,   586,     0,   161,     0,   162,     0,     8,     0,
       0,    37,     0,   456,   456,   158,    37,     0,     0,   597,
      39,     0,   235,     0,   624,   236,    37,   335,   629,     0,
      39,     0,   161,     0,   162,   636,   643,   646,     0,   303,
     306,     0,     0,   306,    37,   938,     0,    37,   158,   303,
       0,    39,     0,   154,   335,   235,   643,     0,   236,     0,
     239,    64,    37,   676,   239,   161,   158,   162,   486,   489,
     303,   495,     0,   235,     0,     0,   236,     0,     0,     0,
       0,     0,     0,   161,   807,   162,   456,   239,   306,     0,
       0,   306,   685,     0,     0,   568,   569,   570,   571,   572,
     573,   574,   575,   576,   577,   578,   579,   580,   581,   582,
     583,   584,     0,     0,   170,     0,    37,    76,    77,    78,
     158,     0,    80,     0,   706,    81,    82,   235,     0,     0,
     236,     0,     0,   594,   755,     0,     0,   161,   596,   162,
       0,     0,   603,     0,     0,     0,     0,     0,   306,   306,
       0,     0,     0,   306,   808,     0,     0,     0,   306,   615,
       0,     0,   620,   306,     0,   428,   429,   430,   431,   560,
     432,     0,     0,   643,     0,     0,   428,   429,   430,   431,
     792,     0,   643,   433,   434,   435,   436,   437,   438,   439,
     440,   441,   442,   443,   444,     0,     0,   239,   437,   438,
     439,   440,   441,   442,   443,   444,     0,   596,     0,     0,
     615,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     806,     0,     0,     0,     0,     0,   306,   743,   574,   577,
     584,   428,   429,   430,   431,     0,   432,     0,     0,     0,
       0,     0,   239,     0,     0,     0,     0,     0,     0,   433,
     561,   435,   436,   562,   438,   439,   440,   441,   442,   443,
     563,     0,     0,     0,     0,     0,     0,   760,   761,   428,
     429,     0,   764,     0,     0,     0,     0,   765,     0,     0,
       0,     0,   771,     0,     0,     0,     0,     0,   857,     0,
       0,     0,   438,   439,   440,   441,   442,   443,   444,     0,
       0,   335,   335,     0,     0,     0,     0,   643,     0,   889,
       0,   335,     0,     0,   333,   306,   306,   186,     0,     0,
       0,   306,     0,     0,   247,   248,   249,   250,   251,   252,
     253,   254,   792,     8,   239,     0,     0,     0,     0,     0,
       0,     0,     0,   239,     0,   760,     0,     0,   428,   429,
     430,   431,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     285,   438,   439,   440,   441,   442,   443,   444,    27,    28,
      29,    30,    31,    32,     0,     0,     0,   156,     0,     0,
       0,     0,     0,     0,     0,   239,     0,     0,     0,     0,
      33,    34,    35,   239,     0,   940,     0,     0,     0,     0,
       0,   864,   865,   420,     0,   866,     0,    36,     0,   643,
       0,   871,     0,     0,     0,   306,     0,     0,     0,   306,
     306,     0,     0,     0,   874,   875,     0,     0,     0,     0,
     879,    37,   239,     0,    38,    39,     0,     0,     0,     0,
       0,     0,   286,     0,     0,   287,     0,     0,     0,   239,
       0,     0,   161,     0,     0,     0,     0,   186,     0,     0,
     239,     0,     0,   982,   247,   248,   249,   250,   251,   252,
     253,   254,     0,     0,     0,     0,     0,   549,     0,     0,
       0,     8,     0,     0,     0,  1001,     0,     0,     0,     0,
     170,     0,   306,   306,     0,     0,     0,     0,     0,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,   285,     0,
       0,     0,     0,     0,   949,   950,    27,    28,    29,    30,
      31,    32,     0,     0,   952,   156,     0,     0,   955,   956,
       0,     0,   239,     0,     0,     0,     0,     0,    33,    34,
      35,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,   982,    18,     0,    20,    36,     0,    23,    24,    25,
      26,     0,     0,     0,     0,     0,     0,   239,     0,     0,
       0,     0,     0,  1070,     0,     0,     0,     0,     0,    37,
       0,     0,    38,    39,   996,     0,     0,     0,     0,     0,
     451,     0,     0,   452,     0,     0,     0,   239,     0,     0,
     161,  1005,  1006,   699,     0,  -550,    60,     0,     0,     0,
      61,    62,    63,     0,     0,     0,     0,     0,     0,     0,
    1017,     0,     0,    64,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,     0,     0,     0,   700,    66,     0,     0,  -550,
       0,  -550,  -550,  -550,  -550,  -550,     0,     0,     0,     0,
       0,     0,     0,     0,    68,    69,    70,    71,   701,    73,
      74,    75,  -550,  -550,  -550,   702,   703,   704,     0,    76,
     705,    78,     0,    79,    80,  -550,   706,    81,    82,  -550,
      83,     0,     0,     0,    87,     0,    89,    90,    91,    92,
      93,    94,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    95,     0,  -550,     0,     0,    96,  -550,  -550,     0,
       0,     0,  1036,     0,     0,     0,     0,     0,     0,     8,
       0,     0,   186,   244,     0,     0,   707,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,     0,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,   255,     0,     0,     0,
       0,     0,     0,     0,    27,    28,    29,    30,    31,    32,
       0,   256,   428,   429,   430,   431,     0,   432,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,     0,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,     0,    36,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    37,     0,     0,
      38,    39,     0,     0,     0,     0,     0,     0,   257,     0,
       0,   258,     0,     8,   259,   260,   186,   244,   261,   262,
     263,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     255,     0,     0,     0,     0,     0,   754,     0,     0,    28,
      29,    30,    31,    32,     0,   256,     0,     0,   296,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,   428,   429,   430,   431,     0,   432,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
       0,   433,   434,   435,   436,   437,   438,   439,   440,   441,
     442,   443,   444,     0,     0,     0,     0,     0,     0,     0,
       0,    37,     0,     0,     0,    39,     0,     0,     0,     0,
       0,     0,   257,     0,     0,   258,     0,     8,   259,   260,
     186,   244,   261,   262,   263,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   255,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,     0,   256,
       0,     0,   479,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,    34,    35,   428,   429,   430,
     431,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,     0,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,     0,     0,     0,
       0,     0,     0,     0,     0,    37,     0,     0,     0,    39,
       0,     0,     0,     0,     0,     0,   257,     0,     0,   258,
       0,     8,   259,   260,   186,   244,   261,   262,   263,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,   255,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,     0,   256,     0,     0,   482,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,   428,   429,   430,   431,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,     0,     0,
     434,   435,   436,   437,   438,   439,   440,   441,   442,   443,
     444,     0,     0,     0,     0,     0,     0,     0,     0,    37,
       0,     0,     0,    39,     0,     0,     0,     0,     0,     0,
     257,     0,     0,   258,     0,     8,   259,   260,   186,   244,
     261,   262,   263,   245,   246,   247,   248,   249,   250,   251,
     252,   253,   254,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,   255,     0,     0,     0,     0,     0,     0,     0,
       0,    28,    29,    30,    31,    32,     0,   256,     0,     0,
     602,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,   428,   429,   430,   431,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
       0,     0,     0,     0,     0,   435,   436,   437,   438,   439,
     440,   441,   442,   443,   444,     0,     0,     0,     0,     0,
       0,     0,     0,    37,     0,     0,     0,    39,     0,     0,
       0,     0,     0,     0,   257,     0,     0,   258,     0,     8,
     259,   260,   186,   244,   261,   262,   263,   245,   246,   247,
     248,   249,   250,   251,   252,   253,   254,     0,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,   255,   744,     0,     0,
       0,     0,     0,     0,     0,    28,    29,    30,    31,    32,
       0,   256,   428,   429,   430,   431,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,     0,
       0,     0,     0,   436,   437,   438,   439,   440,   441,   442,
     443,   444,     0,    36,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    37,     0,     0,
       0,    39,     0,     0,     0,     0,     0,     0,   257,     0,
       0,   258,     0,     8,   259,   260,   186,   244,   261,   262,
     263,   245,   246,   247,   248,   249,   250,   251,   252,   253,
     254,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     255,   745,     0,     0,     0,     0,     0,     0,     0,    28,
      29,    30,    31,    32,     0,   256,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    37,     0,     0,     0,    39,     0,     0,     0,     0,
       0,     0,   257,     0,     0,   258,     0,     8,   259,   260,
     186,   244,   261,   262,   263,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   254,     0,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,   255,     0,     0,     0,     0,     0,
       0,     0,     0,    28,    29,    30,    31,    32,     0,   256,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,     0,     0,     0,    39,
       0,     0,     0,     0,     0,     0,   257,     0,     0,   258,
       0,     8,   259,   260,   186,   244,   261,   262,   263,   245,
     246,   247,   248,   249,   250,   251,   252,   253,   254,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,   255,     0,
       0,     0,     0,     0,     0,     0,     0,    28,    29,    30,
      31,    32,     0,   256,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,     8,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,     0,     0,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,     0,    37,
       0,     0,     0,    39,     0,     0,    27,    28,    29,    30,
      31,    32,     0,     0,     0,     0,   259,   260,     0,     0,
     746,   262,   263,     0,     0,     0,     0,     8,    33,    34,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,     0,     0,     0,     0,     0,    37,
       0,     0,    38,    39,     0,     0,    31,    32,     0,     0,
     393,     0,     0,   394,     0,     0,     0,     0,     0,     0,
     161,     0,     0,     0,    33,    34,    35,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    37,     0,    -2,    59,    39,
    -550,    60,     0,     0,     0,    61,    62,    63,     0,     0,
       0,     0,     0,     0,     0,     0,   161,     0,    64,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,     0,     0,     0,
      65,    66,     0,     0,     0,     0,  -550,  -550,  -550,  -550,
    -550,     0,     0,    67,     0,     0,     0,     0,     0,    68,
      69,    70,    71,    72,    73,    74,    75,  -550,  -550,  -550,
       0,     0,     0,     0,    76,    77,    78,     0,    79,    80,
    -550,     0,    81,    82,  -550,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    95,    59,  -550,  -550,
      60,    96,  -550,     0,    61,    62,    63,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    64,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,     0,     0,     0,    65,
      66,     0,     0,   653,     0,  -550,  -550,  -550,  -550,  -550,
       0,     0,    67,     0,     0,     0,     0,     0,    68,    69,
      70,    71,    72,    73,    74,    75,  -550,  -550,  -550,     0,
       0,     0,     0,    76,    77,    78,     0,    79,    80,  -550,
       0,    81,    82,  -550,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    95,    59,  -550,  -550,    60,
      96,  -550,     0,    61,    62,    63,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    64,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,     0,     0,     0,    65,    66,
       0,     0,   775,     0,  -550,  -550,  -550,  -550,  -550,     0,
       0,    67,     0,     0,     0,     0,     0,    68,    69,    70,
      71,    72,    73,    74,    75,  -550,  -550,  -550,     0,     0,
       0,     0,    76,    77,    78,     0,    79,    80,  -550,     0,
      81,    82,  -550,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,    59,  -550,  -550,    60,    96,
    -550,     0,    61,    62,    63,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    64,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,     0,     0,     0,    65,    66,     0,
       0,   791,     0,  -550,  -550,  -550,  -550,  -550,     0,     0,
      67,     0,     0,     0,     0,     0,    68,    69,    70,    71,
      72,    73,    74,    75,  -550,  -550,  -550,     0,     0,     0,
       0,    76,    77,    78,     0,    79,    80,  -550,     0,    81,
      82,  -550,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    95,    59,  -550,  -550,    60,    96,  -550,
       0,    61,    62,    63,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    64,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,  -550,
    -550,  -550,  -550,     0,     0,     0,    65,    66,     0,     0,
       0,     0,  -550,  -550,  -550,  -550,  -550,     0,     0,    67,
       0,     0,     0,   911,     0,    68,    69,    70,    71,    72,
      73,    74,    75,  -550,  -550,  -550,     0,     0,     0,     0,
      76,    77,    78,     0,    79,    80,  -550,     0,    81,    82,
    -550,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,     7,     0,     8,     0,     0,     0,     0,
       0,     0,    95,   468,  -550,     0,     0,    96,  -550,     0,
       0,     0,     0,     0,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
      27,    28,    29,    30,    31,    32,     0,     0,   428,   429,
     430,   431,     0,   432,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,     0,   433,   434,   435,   436,
     437,   438,   439,   440,   441,   442,   443,   444,     0,    36,
       0,   606,     0,     0,     0,     0,     0,    54,     0,     8,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    37,     0,     0,    38,    39,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,   428,   429,   430,   431,
       0,   432,     0,     0,    27,    28,    29,    30,    31,    32,
       0,     0,     0,     0,   433,   434,   435,   436,   437,   438,
     439,   440,   441,   442,   443,   444,    33,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,     0,   608,     0,     0,     0,     0,
       0,   193,     0,   194,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    37,     0,     0,
      38,    39,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
     428,   429,   430,   431,     0,   432,     0,     0,     0,    28,
      29,    30,    31,    32,     0,     0,     0,     0,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
      33,    34,    35,     8,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
       0,     0,     9,    10,    11,    12,    13,    14,    15,    16,
     848,    18,   849,    20,   850,   851,    23,    24,    25,    26,
       0,    37,     0,     0,     0,    39,     0,     0,     0,    28,
      29,    30,    31,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,     8,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   240,     0,     0,     0,    36,     0,     0,
       0,     0,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
       0,    37,     0,     0,     0,    39,   852,     0,     0,    28,
      29,    30,    31,    32,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,    35,     0,     8,     0,     0,     0,     0,     0,
       0,     0,   757,   910,   353,     0,     0,    36,     0,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    37,     0,     0,     0,    39,   852,     0,     0,    27,
      28,    29,    30,    31,    32,     0,     0,   428,   429,   430,
     431,     0,   432,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,     8,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,     0,    36,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,    37,     0,     0,    38,    39,     0,     0,    27,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,     8,     0,     0,     0,     0,     0,
       0,     0,   763,     0,     0,     0,     0,     0,    36,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,   222,    37,     0,     0,    38,    39,     0,     0,     0,
      28,    29,    30,    31,    32,     0,     0,   428,   429,   430,
     431,     0,   432,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,     8,   433,   434,   435,   436,   437,
     438,   439,   440,   441,   442,   443,   444,     0,    36,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,    37,     0,     0,     0,    39,     0,     0,     0,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,     8,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   240,     0,     0,     0,    36,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,    37,     0,     0,     0,    39,     0,     0,     0,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,    35,   793,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    36,     0,
       0,     0,     0,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,    37,     0,     0,     0,    39,     0,     0,     0,
      28,    29,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       8,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    36,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,     0,     0,     0,
       0,     0,    37,     0,     0,     0,    39,     0,     0,    31,
      32,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     8,    33,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     9,    10,    11,    12,    13,
      14,    15,    16,   678,    18,   679,    20,   680,   681,    23,
      24,    25,    26,     0,     0,     0,     0,     0,    37,     0,
       0,     0,    39,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      36,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    37,     0,     0,     0,    39
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-914))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       0,   344,     5,     5,   565,     5,    21,   135,     8,     1,
       2,     5,    68,    90,     5,    44,    96,     5,   682,   137,
      42,    21,   361,   503,   531,   647,   231,   512,     5,     5,
     368,   633,   512,   543,    90,    91,    40,     5,     5,   153,
       3,   286,   287,     3,    44,   290,   823,     1,     2,     6,
      40,   115,   147,     6,    41,   135,     5,   919,   920,     5,
      43,     3,    43,     3,    45,    57,   216,    67,   148,     5,
     642,    46,    72,    73,     3,     5,    41,   100,   650,   135,
     175,   176,     5,     5,   234,    54,     6,     5,     3,     5,
       3,   136,   185,     5,     5,     3,     5,    54,   191,    44,
       5,  1014,     5,   675,   197,   856,    59,   107,   188,   202,
     224,     6,     6,     6,    49,    50,    51,   100,   122,   691,
       3,   125,    41,    42,     1,   112,    46,   729,   132,   159,
     160,   195,   107,    46,   227,   125,   165,   137,   167,   169,
       3,   648,     3,   112,   237,   137,     0,   112,   393,   394,
     395,   928,   115,    41,    40,   115,  1018,    40,     6,  1072,
     224,   153,   501,    40,     3,   165,     3,   167,    76,   111,
     151,   285,   836,   115,    57,   115,   240,    40,   242,    40,
     161,   520,   182,   137,   522,    48,   115,   301,     3,   940,
     132,    86,    86,    86,   194,    43,   111,   132,   111,    42,
     115,    40,   115,   111,    42,     6,    48,   115,   153,    48,
     210,   203,  1052,   205,   991,     3,   161,   789,    57,   464,
     465,   114,     3,  1038,   224,    40,   226,   227,   111,   739,
     222,   231,   115,    48,   122,     3,   838,   125,   230,   231,
     269,   241,  1019,     3,     4,    46,     6,    72,   111,  1089,
     111,   134,   115,  1068,   115,    48,   286,   287,   101,   122,
     290,   863,   125,   101,    48,    46,     4,   267,   890,   269,
     934,   134,   111,   134,   111,   782,   115,   231,   115,   224,
      40,    44,   107,    43,     4,    86,    46,    54,    48,   234,
     320,   321,   537,   285,  1071,   134,   111,   374,   100,   784,
     115,    89,    54,   282,   784,    40,   424,   122,    46,   301,
     125,    46,   368,   977,    77,    54,     3,   346,   374,   134,
     892,    43,   314,   111,   269,   317,    46,   115,   533,   931,
     111,     3,    54,   100,   115,   674,    43,    76,   330,    46,
     285,   100,    43,   111,     3,    57,   346,   115,   100,   328,
     110,   111,   502,   112,   113,   115,   301,     3,   414,     3,
       6,   361,   122,    43,     3,   125,   311,    40,    40,    76,
     450,    40,   132,     4,   134,     6,   330,   136,    43,   529,
     113,    40,   113,  1047,    57,    40,   386,   894,    57,    48,
     951,   963,   392,   100,    43,    41,    40,    46,    57,   971,
     345,    40,    57,   136,    48,   136,   406,   407,    40,    48,
     410,    76,   412,    57,    52,    46,    48,   409,    57,   512,
      54,   451,   452,   453,   111,  1027,    43,    76,   115,   410,
     460,   461,   424,    71,   116,   122,    42,    40,   125,   111,
     470,     3,    76,   115,     6,   132,    80,   511,    54,    40,
       3,   515,   111,   398,    57,    46,   115,  1029,    73,    74,
      75,   491,   134,     3,   409,  1037,   522,   111,   413,    40,
     424,   115,   111,    40,   538,   134,   115,   469,   122,    40,
      41,   125,   474,   122,    41,    42,   125,    40,   132,   832,
     134,   203,    40,   205,    43,   134,    57,    46,   498,    43,
      40,   501,    46,    40,    57,    54,    43,     4,    48,     6,
     222,    41,    42,    49,    50,    51,   519,   519,   230,   519,
     520,   521,   514,   545,   553,   519,   526,    42,   519,    40,
     521,   519,    47,    40,    21,     4,   565,     6,   530,    40,
      41,   533,   519,   519,    80,    40,    43,    40,    41,    46,
      40,   519,   519,   553,   554,    86,    57,    44,   111,    40,
     514,   591,   115,    43,    57,   565,    46,   567,    40,   122,
     519,   111,   125,   519,    43,   115,   530,    46,     4,   533,
       6,   134,   563,   519,   648,    54,    40,    40,    41,   519,
      77,    78,    79,    40,   134,    82,   519,   519,    85,    86,
     656,   519,   314,   519,    57,   317,    43,   519,   519,    46,
     519,   739,   707,   110,   519,     4,   519,     6,   563,  1061,
     565,    43,   625,   625,    46,   625,   776,   777,   901,   693,
     903,   625,    40,   633,   625,    43,     6,   625,    46,  1081,
    1082,  1083,   642,    40,     3,    40,    41,     6,   625,   625,
     650,   654,   654,    43,   654,  1097,    46,   625,   625,   739,
     654,   642,    57,   654,    48,   680,   654,    48,   660,   650,
     672,    48,   672,   673,   674,   675,   625,   654,   654,   625,
     680,   672,   682,   739,   672,   803,   654,   654,    48,   625,
     690,   691,    46,   786,   675,   625,   690,   409,    40,    41,
     692,   682,   625,   625,   909,   654,     3,   625,   654,   625,
     691,   112,     3,   625,   625,    57,   625,    41,   654,   796,
     625,   785,   625,    42,   654,   747,   755,    43,   808,   729,
     794,   654,   654,     3,    52,    53,   654,   817,   654,     3,
     796,   741,   654,   654,    46,   654,     3,     3,   843,   654,
    1093,   654,   111,    71,    72,   755,   115,   469,    52,    53,
      40,   817,   474,   122,     3,   746,   125,    46,     3,    49,
      50,    51,     6,   132,    54,    52,    53,    71,    72,    42,
      42,    43,   846,    40,    46,    43,    43,   779,   101,   789,
     854,    48,    42,   793,    71,    72,   852,   117,   118,   119,
      80,   746,   747,   803,   112,    40,   100,    40,   789,     7,
      48,   803,    48,    48,   870,    41,   819,   819,   101,   819,
      41,    41,    57,   100,   112,   819,    48,    57,   819,   893,
      40,   819,   824,   833,    48,   850,   836,     3,   838,   831,
       6,    48,   819,   819,    48,   837,   910,   132,    43,   803,
     850,   819,   819,   112,   111,   836,   856,   921,   115,    49,
      50,    51,    41,   863,    41,   122,    41,     3,   125,    41,
     819,    54,    46,   819,    40,   132,   111,   134,    42,    41,
     115,    41,    48,   819,   130,   131,   132,   122,    43,   819,
     125,     3,   892,    41,   886,    41,   819,   819,   927,   134,
      41,   819,    41,   819,    40,   101,    41,   819,   819,   909,
     819,   892,    41,   112,   819,    42,   819,   909,    41,   919,
     920,    57,   951,    43,     3,   919,   920,   927,    40,   107,
     112,   931,    76,    48,   934,    48,    48,    48,    41,  1003,
     940,   933,   942,   112,   112,   111,   938,    41,   660,   115,
      48,   951,    48,   934,    43,   909,   122,    48,   987,   125,
      48,    40,    48,   963,    41,    43,   132,   959,   134,    48,
      46,   971,    43,    40,  1038,   111,    57,   977,     3,   115,
     692,    47,   963,    43,     3,    41,   122,   987,    88,   125,
     971,   936,    40,   938,    46,  1051,   977,   989,   134,   111,
     992,  1001,  1002,   115,  1068,    48,   951,    41,    41,    40,
      48,    66,    48,    44,    42,    40,    48,    54,  1018,    48,
      51,    40,   134,    48,  1018,    48,    48,  1027,    48,  1029,
      42,    40,   111,    41,    89,   136,   115,  1037,    57,    94,
    1032,  1033,  1034,   122,    47,    62,   125,  1047,  1029,   100,
      40,    46,    83,    84,   134,   134,  1037,    40,  1058,    47,
      76,  1061,    76,    76,    41,    47,  1047,   779,    78,    41,
    1070,    41,    40,  1065,    48,    48,    47,  1058,    40,    48,
    1061,  1081,  1082,  1083,   115,    48,   111,     3,    48,    80,
     115,    41,   111,    41,    43,     3,   115,  1097,    54,    43,
    1081,  1082,  1083,   122,     3,    41,   125,    40,     3,   134,
      76,    76,   824,    42,    48,   146,  1097,    76,    76,   831,
       3,    76,   153,    76,    40,   837,    48,    41,   159,   160,
     185,   162,    48,    40,    43,   190,   191,    42,   169,    43,
     101,    57,   197,    41,    54,    40,    43,   202,    41,    41,
      40,    76,     3,    48,   101,    46,    43,    40,    47,    54,
      41,   216,    57,    43,   195,    48,    43,   198,    43,    42,
      48,    47,   227,   228,   886,    41,    40,   208,   233,   234,
       3,    76,   237,   128,   129,   130,   131,   132,    40,    40,
      46,    41,    43,   224,    48,   111,     3,    48,    41,   115,
      41,   232,    48,   234,   235,   236,    43,   238,    41,   240,
     241,   242,   243,    41,    40,    46,   111,    40,   134,    42,
     115,   933,    43,    48,    48,    48,   938,   122,   111,    48,
     125,    54,   115,    40,    57,    41,    43,   132,   269,   134,
      41,    48,    41,   156,   157,    48,    48,   959,   160,    41,
      48,   134,   503,    76,   285,   286,   287,   169,   526,   290,
     111,   672,   672,   672,   115,   672,   909,   533,   673,   672,
     301,   122,   303,   498,   125,   803,   543,   989,   101,   424,
     992,   132,    49,   134,   975,   567,   942,  1070,   111,   320,
     321,  1002,   115,  1037,   325,     3,  1022,    -1,    -1,   122,
     657,    -1,   125,    -1,   111,   336,    -1,    -1,   115,   132,
      -1,   134,    -1,    -1,   345,   122,    -1,    -1,   125,    -1,
    1032,  1033,  1034,     3,   236,   132,   238,   134,     3,   360,
      -1,    -1,    40,    -1,    -1,    43,    -1,    -1,     3,    -1,
      48,    -1,   255,    -1,   257,   258,   259,   260,   261,   262,
     263,    -1,    -1,  1065,    -1,    -1,     3,    -1,    -1,     3,
      40,    -1,   393,   394,   395,    40,    -1,   398,    48,   400,
     401,    -1,   403,    48,     3,    40,    -1,    -1,   409,    49,
      50,    51,   295,    48,    54,   126,   127,   128,   129,   130,
     131,   132,    57,    40,    -1,    -1,    40,    -1,    -1,    -1,
      -1,    48,   315,   111,    48,   318,    76,   115,   320,   321,
      80,    40,    -1,   325,   122,    -1,    -1,   125,    -1,    48,
      -1,    -1,   453,    -1,   132,    -1,   134,    -1,     3,    -1,
      -1,   111,    -1,   464,   465,   115,   111,    -1,    -1,   470,
     115,    -1,   122,    -1,   499,   125,   111,   502,   503,    -1,
     115,    -1,   132,    -1,   134,   510,   511,   512,    -1,   134,
     491,    -1,    -1,   494,   111,    40,    -1,   111,   115,   134,
      -1,   115,    -1,    48,   529,   122,   531,    -1,   125,    -1,
     511,    21,   111,   538,   515,   132,   115,   134,   400,   401,
     134,   403,    -1,   122,    -1,    -1,   125,    -1,    -1,    -1,
      -1,    -1,    -1,   132,    44,   134,   537,   538,   539,    -1,
      -1,   542,   543,    -1,    -1,   428,   429,   430,   431,   432,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   442,
     443,   444,    -1,    -1,   565,    -1,   111,    77,    78,    79,
     115,    -1,    82,    -1,    84,    85,    86,   122,    -1,    -1,
     125,    -1,    -1,   466,   585,    -1,    -1,   132,   470,   134,
      -1,    -1,   475,    -1,    -1,    -1,    -1,    -1,   599,   600,
      -1,    -1,    -1,   604,   114,    -1,    -1,    -1,   609,   491,
      -1,    -1,   494,   614,    -1,   103,   104,   105,   106,    41,
     108,    -1,    -1,   648,    -1,    -1,   103,   104,   105,   106,
     655,    -1,   657,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,    -1,    -1,   648,   125,   126,
     127,   128,   129,   130,   131,   132,    -1,   539,    -1,    -1,
     542,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     671,    -1,    -1,    -1,    -1,    -1,   677,   560,   561,   562,
     563,   103,   104,   105,   106,    -1,   108,    -1,    -1,    -1,
      -1,    -1,   693,    -1,    -1,    -1,    -1,    -1,    -1,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,    -1,    -1,    -1,    -1,    -1,    -1,   599,   600,   103,
     104,    -1,   604,    -1,    -1,    -1,    -1,   609,    -1,    -1,
      -1,    -1,   614,    -1,    -1,    -1,    -1,    -1,   739,    -1,
      -1,    -1,   126,   127,   128,   129,   130,   131,   132,    -1,
      -1,   776,   777,    -1,    -1,    -1,    -1,   782,    -1,   784,
      -1,   786,    -1,    -1,     3,   766,   767,     6,    -1,    -1,
      -1,   772,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,   807,     3,   785,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   794,    -1,   677,    -1,    -1,   103,   104,
     105,   106,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,   126,   127,   128,   129,   130,   131,   132,    48,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    57,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   846,    -1,    -1,    -1,    -1,
      70,    71,    72,   854,    -1,   856,    -1,    -1,    -1,    -1,
      -1,   744,   745,   746,    -1,   748,    -1,    87,    -1,   894,
      -1,   754,    -1,    -1,    -1,   876,    -1,    -1,    -1,   880,
     881,    -1,    -1,    -1,   766,   767,    -1,    -1,    -1,    -1,
     772,   111,   893,    -1,   114,   115,    -1,    -1,    -1,    -1,
      -1,    -1,   122,    -1,    -1,   125,    -1,    -1,    -1,   910,
      -1,    -1,   132,    -1,    -1,    -1,    -1,     6,    -1,    -1,
     921,    -1,    -1,   924,    13,    14,    15,    16,    17,    18,
      19,    20,    -1,    -1,    -1,    -1,    -1,   938,    -1,    -1,
      -1,     3,    -1,    -1,    -1,   946,    -1,    -1,    -1,    -1,
     951,    -1,   953,   954,    -1,    -1,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,   867,   868,    48,    49,    50,    51,
      52,    53,    -1,    -1,   876,    57,    -1,    -1,   880,   881,
      -1,    -1,  1003,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,  1022,    31,    -1,    33,    87,    -1,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,  1038,    -1,    -1,
      -1,    -1,    -1,  1044,    -1,    -1,    -1,    -1,    -1,   111,
      -1,    -1,   114,   115,   937,    -1,    -1,    -1,    -1,    -1,
     122,    -1,    -1,   125,    -1,    -1,    -1,  1068,    -1,    -1,
     132,   953,   954,     1,    -1,     3,     4,    -1,    -1,    -1,
       8,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     973,    -1,    -1,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    43,    44,    -1,    -1,    47,
      -1,    49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    -1,    77,
      78,    79,    -1,    81,    82,    83,    84,    85,    86,    87,
      88,    -1,    -1,    -1,    92,    -1,    94,    95,    96,    97,
      98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   109,    -1,   111,    -1,    -1,   114,   115,   116,    -1,
      -1,    -1,    43,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      -1,    -1,     6,     7,    -1,    -1,   134,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    48,    49,    50,    51,    52,    53,
      -1,    55,   103,   104,   105,   106,    -1,   108,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,    -1,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,    -1,
     114,   115,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,
      -1,   125,    -1,     3,   128,   129,     6,     7,   132,   133,
     134,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,   103,   104,   105,   106,    -1,   108,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,
      -1,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,
      -1,    -1,   122,    -1,    -1,   125,    -1,     3,   128,   129,
       6,     7,   132,   133,   134,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    71,    72,   103,   104,   105,
     106,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,   115,
      -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,    -1,   125,
      -1,     3,   128,   129,     6,     7,   132,   133,   134,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    -1,    55,    -1,    -1,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,   103,   104,   105,   106,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,
      -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,
     122,    -1,    -1,   125,    -1,     3,   128,   129,     6,     7,
     132,   133,   134,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    50,    51,    52,    53,    -1,    55,    -1,    -1,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,   103,   104,   105,   106,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,
      -1,    -1,    -1,    -1,    -1,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,
      -1,    -1,    -1,    -1,   122,    -1,    -1,   125,    -1,     3,
     128,   129,     6,     7,   132,   133,   134,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    -1,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,    53,
      -1,    55,   103,   104,   105,   106,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,    -1,
      -1,    -1,    -1,   124,   125,   126,   127,   128,   129,   130,
     131,   132,    -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,    -1,
      -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,
      -1,   125,    -1,     3,   128,   129,     6,     7,   132,   133,
     134,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,    -1,
      -1,    -1,   122,    -1,    -1,   125,    -1,     3,   128,   129,
       6,     7,   132,   133,   134,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    -1,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    70,    71,    72,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,   115,
      -1,    -1,    -1,    -1,    -1,    -1,   122,    -1,    -1,   125,
      -1,     3,   128,   129,     6,     7,   132,   133,   134,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,     3,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,    -1,    -1,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,   111,
      -1,    -1,    -1,   115,    -1,    -1,    48,    49,    50,    51,
      52,    53,    -1,    -1,    -1,    -1,   128,   129,    -1,    -1,
     132,   133,   134,    -1,    -1,    -1,    -1,     3,    70,    71,
      72,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    87,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,   111,
      -1,    -1,   114,   115,    -1,    -1,    52,    53,    -1,    -1,
     122,    -1,    -1,   125,    -1,    -1,    -1,    -1,    -1,    -1,
     132,    -1,    -1,    -1,    70,    71,    72,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   111,    -1,     0,     1,   115,
       3,     4,    -1,    -1,    -1,     8,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   132,    -1,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      43,    44,    -1,    -1,    -1,    -1,    49,    50,    51,    52,
      53,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      -1,    -1,    -1,    -1,    77,    78,    79,    -1,    81,    82,
      83,    -1,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,     1,   111,     3,
       4,   114,   115,    -1,     8,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    -1,    -1,    -1,    43,
      44,    -1,    -1,    47,    -1,    49,    50,    51,    52,    53,
      -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    -1,
      -1,    -1,    -1,    77,    78,    79,    -1,    81,    82,    83,
      -1,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   109,     1,   111,     3,     4,
     114,   115,    -1,     8,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    43,    44,
      -1,    -1,    47,    -1,    49,    50,    51,    52,    53,    -1,
      -1,    56,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    -1,    -1,
      -1,    -1,    77,    78,    79,    -1,    81,    82,    83,    -1,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,     1,   111,     3,     4,   114,
     115,    -1,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    -1,    -1,    -1,    43,    44,    -1,
      -1,    47,    -1,    49,    50,    51,    52,    53,    -1,    -1,
      56,    -1,    -1,    -1,    -1,    -1,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    -1,    -1,    -1,
      -1,    77,    78,    79,    -1,    81,    82,    83,    -1,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,     1,   111,     3,     4,   114,   115,
      -1,     8,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    43,    44,    -1,    -1,
      -1,    -1,    49,    50,    51,    52,    53,    -1,    -1,    56,
      -1,    -1,    -1,    60,    -1,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    -1,    -1,    -1,    -1,
      77,    78,    79,    -1,    81,    82,    83,    -1,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,     1,    -1,     3,    -1,    -1,    -1,    -1,
      -1,    -1,   109,    58,   111,    -1,    -1,   114,   115,    -1,
      -1,    -1,    -1,    -1,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    -1,    -1,   103,   104,
     105,   106,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    -1,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,    -1,    87,
      -1,    58,    -1,    -1,    -1,    -1,    -1,     1,    -1,     3,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,    -1,    -1,   114,   115,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,   103,   104,   105,   106,
      -1,   108,    -1,    -1,    48,    49,    50,    51,    52,    53,
      -1,    -1,    -1,    -1,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,    70,    71,    72,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    87,    -1,    58,    -1,    -1,    -1,    -1,
      -1,     1,    -1,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,    -1,
     114,   115,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
     103,   104,   105,   106,    -1,   108,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,   132,
      70,    71,    72,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,    -1,
      -1,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,     3,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    -1,    -1,    -1,    87,    -1,    -1,
      -1,    -1,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      -1,   111,    -1,    -1,    -1,   115,   116,    -1,    -1,    49,
      50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      70,    71,    72,    -1,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    58,    83,    13,    -1,    -1,    87,    -1,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,   111,    -1,    -1,    -1,   115,   116,    -1,    -1,    48,
      49,    50,    51,    52,    53,    -1,    -1,   103,   104,   105,
     106,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,     3,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,    -1,    87,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,   111,    -1,    -1,   114,   115,    -1,    -1,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    58,    -1,    -1,    -1,    -1,    -1,    87,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,   111,    -1,    -1,   114,   115,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    -1,    -1,   103,   104,   105,
     106,    -1,   108,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,     3,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,   132,    -1,    87,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    83,    -1,    -1,    -1,    87,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,     3,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    -1,
      -1,    -1,    -1,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    -1,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,    70,    71,    72,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    87,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    -1,    -1,    -1,
      -1,    -1,   111,    -1,    -1,    -1,   115,    -1,    -1,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,    70,    71,    72,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    87,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    -1,    -1,    -1,    -1,    -1,   111,    -1,
      -1,    -1,   115,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      87,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   111,    -1,    -1,    -1,   115
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,   117,   118,   119,   138,   139,   299,     1,     3,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    48,    49,    50,
      51,    52,    53,    70,    71,    72,    87,   111,   114,   115,
     189,   233,   247,   248,   250,   251,   252,   253,   254,   255,
     277,   278,   288,   290,     1,   233,     1,    40,     0,     1,
       4,     8,     9,    10,    21,    43,    44,    56,    62,    63,
      64,    65,    66,    67,    68,    69,    77,    78,    79,    81,
      82,    85,    86,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   109,   114,   140,   141,   142,
     144,   145,   146,   147,   148,   151,   152,   154,   155,   156,
     157,   158,   159,   160,   163,   164,   165,   168,   170,   175,
     176,   177,   178,   180,   184,   191,   192,   193,   194,   195,
     199,   200,   207,   208,   218,   229,   299,   100,   285,   299,
     285,    48,    52,    71,    48,    48,    40,   136,   100,   288,
      43,   251,   247,    40,    48,    54,    57,    76,   115,   122,
     125,   132,   134,   238,   239,   241,   243,   244,   245,   246,
     288,   299,   247,   254,   288,   113,   136,   289,    43,    43,
     230,   231,   233,   299,   116,    40,     6,    86,   114,   293,
      40,   296,   299,     1,     3,   249,   250,    40,   296,    40,
     162,   299,    40,    40,    86,    40,    44,    77,    89,   288,
       3,    46,   288,    40,     4,    46,    40,    40,    43,    46,
       4,   293,    40,   174,   249,   172,   174,    40,    40,   293,
      40,   100,   278,   296,    40,   122,   125,   241,   246,   288,
      83,   189,   249,   278,     7,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    40,    55,   122,   125,   128,
     129,   132,   133,   134,   233,   234,   235,   237,   249,   250,
     263,   264,   265,   266,   293,   294,   299,    48,    48,    48,
     288,   115,   290,   278,   247,    40,   122,   125,   230,   244,
     246,   288,    48,    46,   256,   257,    58,   263,   264,   263,
       3,    40,    48,   134,   242,   245,   288,    48,   242,   245,
     246,   247,   288,   238,    40,    57,   238,    40,    57,    48,
     122,   125,   242,   245,   288,   112,   290,   115,   290,    41,
      42,   232,   299,     3,   286,   293,   278,     6,    46,    86,
     286,   297,   286,    43,    54,    40,   241,    41,   286,   288,
       3,     3,   286,    13,   169,   230,   230,   288,    43,    54,
      54,   210,    46,     3,   171,   297,     3,   230,    46,   240,
     241,   244,   299,    43,    42,   173,   299,   286,   287,   299,
     149,   150,   293,   230,   203,   204,   205,   233,   277,   299,
     288,   293,     3,   122,   125,   246,   288,   297,    40,   286,
     122,   125,   288,   112,   241,   288,     3,   258,   299,    40,
     241,    46,   288,   263,    40,    48,   263,   263,   263,   263,
     263,   263,   263,   101,    42,   236,   299,    40,   103,   104,
     105,   106,   108,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,   132,     7,    48,    48,    41,   289,
     101,   122,   125,   246,   288,   243,   288,   243,    41,    41,
     122,   125,   243,   288,   112,    48,    57,   263,    58,    40,
     246,   288,    48,   288,    40,    57,    48,   246,   230,    58,
     263,   230,    58,   263,    48,    48,   242,   245,    48,   242,
     245,   112,    48,   122,   125,   242,   289,    43,   233,    41,
     288,   179,    42,    54,    41,   256,   238,    41,    46,    41,
      54,    41,    42,   167,    42,    41,    41,    43,   288,   139,
     209,    41,    41,    41,    41,   172,   174,    41,    41,    42,
      46,    41,   101,    42,   206,   299,    59,   112,    41,   246,
     288,    43,   112,   107,    54,    76,   190,   299,   230,   288,
      80,   182,   238,   247,   197,    43,    76,   269,   270,   299,
      41,   122,   125,   132,   246,   249,    48,   237,   263,   263,
     263,   263,   263,   263,   263,   263,   263,   263,   263,   263,
     263,   263,   263,   263,   263,   278,   288,   112,    41,    41,
      41,   112,   243,   243,   263,   230,   242,   288,    41,   112,
      48,   230,    58,   263,    48,    41,    58,    41,    58,    48,
      48,    48,    48,   122,   125,   242,   245,    48,    48,    48,
     242,   232,     4,    46,   293,   139,   297,   149,   265,   293,
     298,    43,    43,   143,     4,   161,   293,     4,    43,    46,
     110,   166,   241,   293,   295,   286,   293,   298,    41,   233,
     241,    46,    43,    47,   139,    44,   229,   172,    43,    46,
      40,    47,   173,     3,   111,   115,   291,    43,   297,   233,
     166,    88,   201,   205,   153,   241,   293,   112,    30,    32,
      34,    35,   183,   252,   253,   288,    57,   185,   251,    43,
      46,    41,    40,    42,    43,    46,   181,    80,   238,     1,
      43,    66,    73,    74,    75,    78,    84,   134,   144,   145,
     146,   147,   151,   152,   156,   158,   160,   163,   165,   168,
     170,   175,   176,   177,   178,   199,   200,   207,   211,   214,
     215,   216,   217,   218,   219,   220,   221,   224,   227,   229,
     299,   271,    46,   263,    41,    41,   132,   247,    41,   122,
     125,   239,    48,   236,    76,   288,    41,    58,    41,    41,
     242,   242,    41,    58,   242,   242,    48,    48,    48,    48,
      48,   242,    48,    48,    48,    47,    42,    42,   211,    40,
      80,   247,   280,   299,    54,    41,    42,   167,   166,   241,
     280,    47,   293,     3,   249,    43,    54,   295,   230,   113,
     136,   292,   136,   100,    41,    47,   288,    44,   114,   180,
     195,   199,   200,   202,   215,   217,   219,   229,   206,   139,
     280,    43,   285,   182,    40,    46,   186,   146,   259,   260,
     299,    40,    54,   280,   281,   230,   241,    40,   213,    46,
      76,    76,    76,   134,    40,   290,    47,   211,    30,    32,
      34,    35,   116,   228,   249,   253,   278,   288,    78,   272,
     273,   279,   299,   196,   263,   263,   263,    41,    41,    41,
      40,   263,   238,    41,   242,   242,    48,    48,    48,   242,
      48,    48,   297,   297,    47,   230,    40,    80,   166,   293,
     298,    43,   241,    41,   280,    43,    43,   241,   174,    41,
       3,     3,   115,     3,   115,   234,     4,    46,    54,   100,
      83,    60,    43,   181,   230,    40,    43,   187,     3,   261,
      42,    47,   230,   256,    76,   282,   299,    41,   182,   230,
     211,   212,   290,    40,   241,   285,   249,    76,    40,   269,
     288,    43,    42,    73,    74,    75,   274,   276,   211,   263,
     263,   249,   242,    48,    48,   242,   242,    41,   230,    40,
     167,   280,    43,   241,   166,    43,    43,   292,   292,   101,
     203,    41,   188,    54,   259,   259,    43,   241,    41,    43,
     283,   284,   288,    43,    46,   238,   181,    41,   211,    40,
     230,   182,    40,   122,   125,   246,   263,   230,    46,   269,
     273,   288,   275,    47,   239,   242,   242,    43,    41,   230,
      43,   280,    43,   101,   280,    43,   262,   263,   261,   182,
      43,    46,    42,    48,    40,    46,   238,    47,   230,    41,
     181,   230,    40,    40,    40,   125,    43,    41,   223,    46,
      48,   267,   268,   299,   279,    43,   198,   241,    41,    41,
      43,   107,   186,   259,   181,   284,    48,   211,    41,   225,
     280,    41,   230,   230,   230,    40,   281,   198,   222,    48,
     288,   182,   249,   187,    48,    48,   226,   280,    43,    46,
     226,    41,    41,    41,   230,   198,    48,   268,   181,   186,
      48,    43,    46,    54,   226,   226,   226,    41,   187,    48,
     256,   226,    43
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 1813 "parser.y"
    {
                   if (!classes) classes = NewHash();
		   Setattr((yyvsp[(1) - (1)].node),"classes",classes); 
		   Setattr((yyvsp[(1) - (1)].node),"name",ModuleName);
		   
		   if ((!module_node) && ModuleName) {
		     module_node = new_node("module");
		     Setattr(module_node,"name",ModuleName);
		   }
		   Setattr((yyvsp[(1) - (1)].node),"module",module_node);
		   check_extensions();
	           top = (yyvsp[(1) - (1)].node);
               }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 1826 "parser.y"
    {
                 top = Copy(Getattr((yyvsp[(2) - (3)].p),"type"));
		 Delete((yyvsp[(2) - (3)].p));
               }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 1830 "parser.y"
    {
                 top = 0;
               }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 1833 "parser.y"
    {
                 top = (yyvsp[(2) - (3)].p);
               }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 1836 "parser.y"
    {
                 top = 0;
               }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 1839 "parser.y"
    {
                 top = (yyvsp[(3) - (5)].pl);
               }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 1842 "parser.y"
    {
                 top = 0;
               }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 1847 "parser.y"
    {  
                   /* add declaration to end of linked list (the declaration isn't always a single declaration, sometimes it is a linked list itself) */
                   appendChild((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));
                   (yyval.node) = (yyvsp[(1) - (2)].node);
               }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 1852 "parser.y"
    {
                   (yyval.node) = new_node("top");
               }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 1857 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 1858 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 1859 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 1860 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 1861 "parser.y"
    {
                  (yyval.node) = 0;
		  Swig_error(cparse_file, cparse_line,"Syntax error in input(1).\n");
		  exit(1);
               }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 1867 "parser.y"
    { 
                  if ((yyval.node)) {
   		      add_symbols((yyval.node));
                  }
                  (yyval.node) = (yyvsp[(1) - (1)].node); 
	       }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 1883 "parser.y"
    {
                  (yyval.node) = 0;
                  skip_decl();
               }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 1893 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 1894 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 1895 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 1896 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 1897 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 1898 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 1899 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 1900 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 1901 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 1902 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 1903 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 1904 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 1905 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 1906 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 1907 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 1908 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 1909 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 1910 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 1911 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 1912 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 1913 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 1920 "parser.y"
    {
               Node *cls;
	       String *clsname;
	       cplus_mode = CPLUS_PUBLIC;
	       if (!classes) classes = NewHash();
	       if (!classes_typedefs) classes_typedefs = NewHash();
	       if (!extendhash) extendhash = NewHash();
	       clsname = make_class_name((yyvsp[(3) - (4)].str));
	       cls = Getattr(classes,clsname);
	       if (!cls) {
	         cls = Getattr(classes_typedefs, clsname);
		 if (!cls) {
		   /* No previous definition. Create a new scope */
		   Node *am = Getattr(extendhash,clsname);
		   if (!am) {
		     Swig_symbol_newscope();
		     Swig_symbol_setscopename((yyvsp[(3) - (4)].str));
		     prev_symtab = 0;
		   } else {
		     prev_symtab = Swig_symbol_setscope(Getattr(am,"symtab"));
		   }
		   current_class = 0;
		 } else {
		   /* Previous typedef class definition.  Use its symbol table.
		      Deprecated, just the real name should be used. 
		      Note that %extend before the class typedef never worked, only %extend after the class typdef. */
		   prev_symtab = Swig_symbol_setscope(Getattr(cls, "symtab"));
		   current_class = cls;
		   extendmode = 1;
		   SWIG_WARN_NODE_BEGIN(cls);
		   Swig_warning(WARN_PARSE_EXTEND_NAME, cparse_file, cparse_line, "Deprecated %%extend name used - the %s name '%s' should be used instead of the typedef name '%s'.\n", Getattr(cls, "kind"), SwigType_namestr(Getattr(cls, "name")), (yyvsp[(3) - (4)].str));
		   SWIG_WARN_NODE_END(cls);
		 }
	       } else {
		 /* Previous class definition.  Use its symbol table */
		 prev_symtab = Swig_symbol_setscope(Getattr(cls,"symtab"));
		 current_class = cls;
		 extendmode = 1;
	       }
	       Classprefix = NewString((yyvsp[(3) - (4)].str));
	       Namespaceprefix= Swig_symbol_qualifiedscopename(0);
	       Delete(clsname);
	     }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 1962 "parser.y"
    {
               String *clsname;
	       extendmode = 0;
               (yyval.node) = new_node("extend");
	       Setattr((yyval.node),"symtab",Swig_symbol_popscope());
	       if (prev_symtab) {
		 Swig_symbol_setscope(prev_symtab);
	       }
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
               clsname = make_class_name((yyvsp[(3) - (7)].str));
	       Setattr((yyval.node),"name",clsname);

	       /* Mark members as extend */

	       tag_nodes((yyvsp[(6) - (7)].node),"feature:extend",(char*) "1");
	       if (current_class) {
		 /* We add the extension to the previously defined class */
		 appendChild((yyval.node),(yyvsp[(6) - (7)].node));
		 appendChild(current_class,(yyval.node));
	       } else {
		 /* We store the extensions in the extensions hash */
		 Node *am = Getattr(extendhash,clsname);
		 if (am) {
		   /* Append the members to the previous extend methods */
		   appendChild(am,(yyvsp[(6) - (7)].node));
		 } else {
		   appendChild((yyval.node),(yyvsp[(6) - (7)].node));
		   Setattr(extendhash,clsname,(yyval.node));
		 }
	       }
	       current_class = 0;
	       Delete(Classprefix);
	       Delete(clsname);
	       Classprefix = 0;
	       prev_symtab = 0;
	       (yyval.node) = 0;

	     }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 2006 "parser.y"
    {
                    (yyval.node) = new_node("apply");
                    Setattr((yyval.node),"pattern",Getattr((yyvsp[(2) - (5)].p),"pattern"));
		    appendChild((yyval.node),(yyvsp[(4) - (5)].p));
               }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 2016 "parser.y"
    {
		 (yyval.node) = new_node("clear");
		 appendChild((yyval.node),(yyvsp[(2) - (3)].p));
               }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 2027 "parser.y"
    {
		   if (((yyvsp[(4) - (5)].dtype).type != T_ERROR) && ((yyvsp[(4) - (5)].dtype).type != T_SYMBOL)) {
		     SwigType *type = NewSwigType((yyvsp[(4) - (5)].dtype).type);
		     (yyval.node) = new_node("constant");
		     Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
		     Setattr((yyval.node),"type",type);
		     Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
		     if ((yyvsp[(4) - (5)].dtype).rawval) Setattr((yyval.node),"rawval", (yyvsp[(4) - (5)].dtype).rawval);
		     Setattr((yyval.node),"storage","%constant");
		     SetFlag((yyval.node),"feature:immutable");
		     add_symbols((yyval.node));
		     Delete(type);
		   } else {
		     if ((yyvsp[(4) - (5)].dtype).type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value (ignored)\n");
		     }
		     (yyval.node) = 0;
		   }

	       }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 2048 "parser.y"
    {
		 if (((yyvsp[(4) - (5)].dtype).type != T_ERROR) && ((yyvsp[(4) - (5)].dtype).type != T_SYMBOL)) {
		   SwigType_push((yyvsp[(2) - (5)].type),(yyvsp[(3) - (5)].decl).type);
		   /* Sneaky callback function trick */
		   if (SwigType_isfunction((yyvsp[(2) - (5)].type))) {
		     SwigType_add_pointer((yyvsp[(2) - (5)].type));
		   }
		   (yyval.node) = new_node("constant");
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].decl).id);
		   Setattr((yyval.node),"type",(yyvsp[(2) - (5)].type));
		   Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
		   if ((yyvsp[(4) - (5)].dtype).rawval) Setattr((yyval.node),"rawval", (yyvsp[(4) - (5)].dtype).rawval);
		   Setattr((yyval.node),"storage","%constant");
		   SetFlag((yyval.node),"feature:immutable");
		   add_symbols((yyval.node));
		 } else {
		     if ((yyvsp[(4) - (5)].dtype).type == T_ERROR) {
		       Swig_warning(WARN_PARSE_UNSUPPORTED_VALUE,cparse_file,cparse_line,"Unsupported constant value\n");
		     }
		   (yyval.node) = 0;
		 }
               }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 2070 "parser.y"
    {
		 Swig_warning(WARN_PARSE_BAD_VALUE,cparse_file,cparse_line,"Bad constant value (ignored).\n");
		 (yyval.node) = 0;
	       }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 2081 "parser.y"
    {
		 char temp[64];
		 Replace((yyvsp[(2) - (2)].str),"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace((yyvsp[(2) - (2)].str),"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", (yyvsp[(2) - (2)].str));
		 Delete((yyvsp[(2) - (2)].str));
                 (yyval.node) = 0;
	       }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 2090 "parser.y"
    {
		 char temp[64];
		 String *s = NewString((yyvsp[(2) - (2)].id));
		 Replace(s,"$file",cparse_file, DOH_REPLACE_ANY);
		 sprintf(temp,"%d", cparse_line);
		 Replace(s,"$line",temp,DOH_REPLACE_ANY);
		 Printf(stderr,"%s\n", s);
		 Delete(s);
                 (yyval.node) = 0;
               }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 2109 "parser.y"
    {
                    skip_balanced('{','}');
		    (yyval.node) = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 2115 "parser.y"
    {
                    skip_balanced('{','}');
		    (yyval.node) = 0;
		    Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 2121 "parser.y"
    {
		 (yyval.node) = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
               }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 2126 "parser.y"
    {
		 (yyval.node) = 0;
		 Swig_warning(WARN_DEPRECATED_EXCEPT,cparse_file, cparse_line, "%%except is deprecated.  Use %%exception instead.\n");
	       }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 2133 "parser.y"
    {		 
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"value",(yyvsp[(1) - (4)].id));
		 Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (4)].p),"type"));
               }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 2140 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"value",(yyvsp[(1) - (1)].id));
              }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 2144 "parser.y"
    {
                (yyval.node) = (yyvsp[(1) - (1)].node);
              }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 2157 "parser.y"
    {
                   Hash *p = (yyvsp[(5) - (7)].node);
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (7)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (7)].node),"type"));
		   Setattr((yyval.node),"section",Getattr(p,"name"));
		   Setattr((yyval.node),"kwargs",nextSibling(p));
		   Setattr((yyval.node),"code",(yyvsp[(7) - (7)].str));
                 }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 2166 "parser.y"
    {
		   Hash *p = (yyvsp[(5) - (7)].node);
		   String *code;
                   skip_balanced('{','}');
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (7)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (7)].node),"type"));
		   Setattr((yyval.node),"section",Getattr(p,"name"));
		   Setattr((yyval.node),"kwargs",nextSibling(p));
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code",code);
		   Delete(code);
                 }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 2181 "parser.y"
    {
		   (yyval.node) = new_node("fragment");
		   Setattr((yyval.node),"value",Getattr((yyvsp[(3) - (5)].node),"value"));
		   Setattr((yyval.node),"type",Getattr((yyvsp[(3) - (5)].node),"type"));
		   Setattr((yyval.node),"emitonly","1");
		 }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 2194 "parser.y"
    {
                     (yyvsp[(1) - (4)].loc).filename = Copy(cparse_file);
		     (yyvsp[(1) - (4)].loc).line = cparse_line;
		     scanner_set_location(NewString((yyvsp[(3) - (4)].id)),1);
                     if ((yyvsp[(2) - (4)].node)) { 
		       String *maininput = Getattr((yyvsp[(2) - (4)].node), "maininput");
		       if (maininput)
		         scanner_set_main_input_file(NewString(maininput));
		     }
               }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 2203 "parser.y"
    {
                     String *mname = 0;
                     (yyval.node) = (yyvsp[(6) - (7)].node);
		     scanner_set_location((yyvsp[(1) - (7)].loc).filename,(yyvsp[(1) - (7)].loc).line+1);
		     if (strcmp((yyvsp[(1) - (7)].loc).type,"include") == 0) set_nodeType((yyval.node),"include");
		     if (strcmp((yyvsp[(1) - (7)].loc).type,"import") == 0) {
		       mname = (yyvsp[(2) - (7)].node) ? Getattr((yyvsp[(2) - (7)].node),"module") : 0;
		       set_nodeType((yyval.node),"import");
		       if (import_mode) --import_mode;
		     }
		     
		     Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		     /* Search for the module (if any) */
		     {
			 Node *n = firstChild((yyval.node));
			 while (n) {
			     if (Strcmp(nodeType(n),"module") == 0) {
			         if (mname) {
				   Setattr(n,"name", mname);
				   mname = 0;
				 }
				 Setattr((yyval.node),"module",Getattr(n,"name"));
				 break;
			     }
			     n = nextSibling(n);
			 }
			 if (mname) {
			   /* There is no module node in the import
			      node, ie, you imported a .h file
			      directly.  We are forced then to create
			      a new import node with a module node.
			   */			      
			   Node *nint = new_node("import");
			   Node *mnode = new_node("module");
			   Setattr(mnode,"name", mname);
			   appendChild(nint,mnode);
			   Delete(mnode);
			   appendChild(nint,firstChild((yyval.node)));
			   (yyval.node) = nint;
			   Setattr((yyval.node),"module",mname);
			 }
		     }
		     Setattr((yyval.node),"options",(yyvsp[(2) - (7)].node));
               }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 2249 "parser.y"
    { (yyval.loc).type = (char *) "include"; }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 2250 "parser.y"
    { (yyval.loc).type = (char *) "import"; ++import_mode;}
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 2257 "parser.y"
    {
                 String *cpps;
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "%%inline directive inside a namespace is disallowed.\n");
		   (yyval.node) = 0;
		 } else {
		   (yyval.node) = new_node("insert");
		   Setattr((yyval.node),"code",(yyvsp[(2) - (2)].str));
		   /* Need to run through the preprocessor */
		   Seek((yyvsp[(2) - (2)].str),0,SEEK_SET);
		   Setline((yyvsp[(2) - (2)].str),cparse_start_line);
		   Setfile((yyvsp[(2) - (2)].str),cparse_file);
		   cpps = Preprocessor_parse((yyvsp[(2) - (2)].str));
		   start_inline(Char(cpps), cparse_start_line);
		   Delete((yyvsp[(2) - (2)].str));
		   Delete(cpps);
		 }
		 
	       }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 2276 "parser.y"
    {
                 String *cpps;
		 int start_line = cparse_line;
		 skip_balanced('{','}');
		 if (Namespaceprefix) {
		   Swig_error(cparse_file, cparse_start_line, "%%inline directive inside a namespace is disallowed.\n");
		   
		   (yyval.node) = 0;
		 } else {
		   String *code;
                   (yyval.node) = new_node("insert");
		   Delitem(scanner_ccode,0);
		   Delitem(scanner_ccode,DOH_END);
		   code = Copy(scanner_ccode);
		   Setattr((yyval.node),"code", code);
		   Delete(code);		   
		   cpps=Copy(scanner_ccode);
		   start_inline(Char(cpps), start_line);
		   Delete(cpps);
		 }
               }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 2307 "parser.y"
    {
                 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"code",(yyvsp[(1) - (1)].str));
	       }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 2311 "parser.y"
    {
		 String *code = NewStringEmpty();
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"code",code);
		 if (Swig_insert_file((yyvsp[(5) - (5)].id),code) < 0) {
		   Swig_error(cparse_file, cparse_line, "Couldn't find '%s'.\n", (yyvsp[(5) - (5)].id));
		   (yyval.node) = 0;
		 } 
               }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 2321 "parser.y"
    {
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"code",(yyvsp[(5) - (5)].str));
               }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 2326 "parser.y"
    {
		 String *code;
                 skip_balanced('{','}');
		 (yyval.node) = new_node("insert");
		 Setattr((yyval.node),"section",(yyvsp[(3) - (5)].id));
		 Delitem(scanner_ccode,0);
		 Delitem(scanner_ccode,DOH_END);
		 code = Copy(scanner_ccode);
		 Setattr((yyval.node),"code", code);
		 Delete(code);
	       }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 2344 "parser.y"
    {
                 (yyval.node) = new_node("module");
		 if ((yyvsp[(2) - (3)].node)) {
		   Setattr((yyval.node),"options",(yyvsp[(2) - (3)].node));
		   if (Getattr((yyvsp[(2) - (3)].node),"directors")) {
		     Wrapper_director_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"dirprot")) {
		     Wrapper_director_protected_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"allprotected")) {
		     Wrapper_all_protected_mode_set(1);
		   } 
		   if (Getattr((yyvsp[(2) - (3)].node),"templatereduce")) {
		     template_reduce = 1;
		   }
		   if (Getattr((yyvsp[(2) - (3)].node),"notemplatereduce")) {
		     template_reduce = 0;
		   }
		 }
		 if (!ModuleName) ModuleName = NewString((yyvsp[(3) - (3)].id));
		 if (!import_mode) {
		   /* first module included, we apply global
		      ModuleName, which can be modify by -module */
		   String *mname = Copy(ModuleName);
		   Setattr((yyval.node),"name",mname);
		   Delete(mname);
		 } else { 
		   /* import mode, we just pass the idstring */
		   Setattr((yyval.node),"name",(yyvsp[(3) - (3)].id));   
		 }		 
		 if (!module_node) module_node = (yyval.node);
	       }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 2384 "parser.y"
    {
                 Swig_warning(WARN_DEPRECATED_NAME,cparse_file,cparse_line, "%%name is deprecated.  Use %%rename instead.\n");
		 Delete(yyrename);
                 yyrename = NewString((yyvsp[(3) - (4)].id));
		 (yyval.node) = 0;
               }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 2390 "parser.y"
    {
		 Swig_warning(WARN_DEPRECATED_NAME,cparse_file,cparse_line, "%%name is deprecated.  Use %%rename instead.\n");
		 (yyval.node) = 0;
		 Swig_error(cparse_file,cparse_line,"Missing argument to %%name directive.\n");
	       }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 2403 "parser.y"
    {
                 (yyval.node) = new_node("native");
		 Setattr((yyval.node),"name",(yyvsp[(3) - (7)].id));
		 Setattr((yyval.node),"wrap:name",(yyvsp[(6) - (7)].id));
	         add_symbols((yyval.node));
	       }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 2409 "parser.y"
    {
		 if (!SwigType_isfunction((yyvsp[(7) - (8)].decl).type)) {
		   Swig_error(cparse_file,cparse_line,"%%native declaration '%s' is not a function.\n", (yyvsp[(7) - (8)].decl).id);
		   (yyval.node) = 0;
		 } else {
		     Delete(SwigType_pop_function((yyvsp[(7) - (8)].decl).type));
		     /* Need check for function here */
		     SwigType_push((yyvsp[(6) - (8)].type),(yyvsp[(7) - (8)].decl).type);
		     (yyval.node) = new_node("native");
	             Setattr((yyval.node),"name",(yyvsp[(3) - (8)].id));
		     Setattr((yyval.node),"wrap:name",(yyvsp[(7) - (8)].decl).id);
		     Setattr((yyval.node),"type",(yyvsp[(6) - (8)].type));
		     Setattr((yyval.node),"parms",(yyvsp[(7) - (8)].decl).parms);
		     Setattr((yyval.node),"decl",(yyvsp[(7) - (8)].decl).type);
		 }
	         add_symbols((yyval.node));
	       }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 2435 "parser.y"
    {
                 (yyval.node) = new_node("pragma");
		 Setattr((yyval.node),"lang",(yyvsp[(2) - (5)].id));
		 Setattr((yyval.node),"name",(yyvsp[(3) - (5)].id));
		 Setattr((yyval.node),"value",(yyvsp[(5) - (5)].str));
	       }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 2441 "parser.y"
    {
		(yyval.node) = new_node("pragma");
		Setattr((yyval.node),"lang",(yyvsp[(2) - (3)].id));
		Setattr((yyval.node),"name",(yyvsp[(3) - (3)].id));
	      }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 2448 "parser.y"
    { (yyval.str) = NewString((yyvsp[(1) - (1)].id)); }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 2449 "parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 2452 "parser.y"
    { (yyval.id) = (yyvsp[(2) - (3)].id); }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 2453 "parser.y"
    { (yyval.id) = (char *) "swig"; }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 2461 "parser.y"
    {
                SwigType *t = (yyvsp[(2) - (4)].decl).type;
		Hash *kws = NewHash();
		String *fixname;
		fixname = feature_identifier_fix((yyvsp[(2) - (4)].decl).id);
		Setattr(kws,"name",(yyvsp[(3) - (4)].id));
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if ((yyvsp[(1) - (4)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix, nname,decl,kws,(yyvsp[(2) - (4)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,nname,decl,kws);
		      }
		      Delete(nname);
		    } else {
		      if ((yyvsp[(1) - (4)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix,(fixname),decl,kws,(yyvsp[(2) - (4)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,(fixname),decl,kws);
		      }
		    }
		    Delete(decl);
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if ((yyvsp[(1) - (4)].intvalue)) {
		      Swig_name_rename_add(Namespaceprefix,(nname),0,kws,(yyvsp[(2) - (4)].decl).parms);
		    } else {
		      Swig_name_namewarn_add(Namespaceprefix,(nname),0,kws);
		    }
		    Delete(nname);
		  }
		} else {
		  if ((yyvsp[(1) - (4)].intvalue)) {
		    Swig_name_rename_add(Namespaceprefix,(fixname),0,kws,(yyvsp[(2) - (4)].decl).parms);
		  } else {
		    Swig_name_namewarn_add(Namespaceprefix,(fixname),0,kws);
		  }
		}
                (yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 2507 "parser.y"
    {
		String *fixname;
		Hash *kws = (yyvsp[(3) - (7)].node);
		SwigType *t = (yyvsp[(5) - (7)].decl).type;
		fixname = feature_identifier_fix((yyvsp[(5) - (7)].decl).id);
		if (!Len(t)) t = 0;
		/* Special declarator check */
		if (t) {
		  if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push(t,(yyvsp[(6) - (7)].dtype).qualifier);
		  if (SwigType_isfunction(t)) {
		    SwigType *decl = SwigType_pop_function(t);
		    if (SwigType_ispointer(t)) {
		      String *nname = NewStringf("*%s",fixname);
		      if ((yyvsp[(1) - (7)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix, nname,decl,kws,(yyvsp[(5) - (7)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,nname,decl,kws);
		      }
		      Delete(nname);
		    } else {
		      if ((yyvsp[(1) - (7)].intvalue)) {
			Swig_name_rename_add(Namespaceprefix,(fixname),decl,kws,(yyvsp[(5) - (7)].decl).parms);
		      } else {
			Swig_name_namewarn_add(Namespaceprefix,(fixname),decl,kws);
		      }
		    }
		    Delete(decl);
		  } else if (SwigType_ispointer(t)) {
		    String *nname = NewStringf("*%s",fixname);
		    if ((yyvsp[(1) - (7)].intvalue)) {
		      Swig_name_rename_add(Namespaceprefix,(nname),0,kws,(yyvsp[(5) - (7)].decl).parms);
		    } else {
		      Swig_name_namewarn_add(Namespaceprefix,(nname),0,kws);
		    }
		    Delete(nname);
		  }
		} else {
		  if ((yyvsp[(1) - (7)].intvalue)) {
		    Swig_name_rename_add(Namespaceprefix,(fixname),0,kws,(yyvsp[(5) - (7)].decl).parms);
		  } else {
		    Swig_name_namewarn_add(Namespaceprefix,(fixname),0,kws);
		  }
		}
                (yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 2553 "parser.y"
    {
		if ((yyvsp[(1) - (6)].intvalue)) {
		  Swig_name_rename_add(Namespaceprefix,(yyvsp[(5) - (6)].id),0,(yyvsp[(3) - (6)].node),0);
		} else {
		  Swig_name_namewarn_add(Namespaceprefix,(yyvsp[(5) - (6)].id),0,(yyvsp[(3) - (6)].node));
		}
		(yyval.node) = 0;
		scanner_clear_rename();
              }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 2564 "parser.y"
    {
		    (yyval.intvalue) = 1;
                }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 2567 "parser.y"
    {
                    (yyval.intvalue) = 0;
                }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 2594 "parser.y"
    {
                    String *val = (yyvsp[(7) - (7)].str) ? NewString((yyvsp[(7) - (7)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (7)].id), val, 0, (yyvsp[(5) - (7)].decl).id, (yyvsp[(5) - (7)].decl).type, (yyvsp[(5) - (7)].decl).parms, (yyvsp[(6) - (7)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 2600 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (9)].id)) ? NewString((yyvsp[(5) - (9)].id)) : 0;
                    new_feature((yyvsp[(3) - (9)].id), val, 0, (yyvsp[(7) - (9)].decl).id, (yyvsp[(7) - (9)].decl).type, (yyvsp[(7) - (9)].decl).parms, (yyvsp[(8) - (9)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 2606 "parser.y"
    {
                    String *val = (yyvsp[(8) - (8)].str) ? NewString((yyvsp[(8) - (8)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (8)].id), val, (yyvsp[(4) - (8)].node), (yyvsp[(6) - (8)].decl).id, (yyvsp[(6) - (8)].decl).type, (yyvsp[(6) - (8)].decl).parms, (yyvsp[(7) - (8)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 2612 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (10)].id)) ? NewString((yyvsp[(5) - (10)].id)) : 0;
                    new_feature((yyvsp[(3) - (10)].id), val, (yyvsp[(6) - (10)].node), (yyvsp[(8) - (10)].decl).id, (yyvsp[(8) - (10)].decl).type, (yyvsp[(8) - (10)].decl).parms, (yyvsp[(9) - (10)].dtype).qualifier);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 2620 "parser.y"
    {
                    String *val = (yyvsp[(5) - (5)].str) ? NewString((yyvsp[(5) - (5)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (5)].id), val, 0, 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 2626 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (7)].id)) ? NewString((yyvsp[(5) - (7)].id)) : 0;
                    new_feature((yyvsp[(3) - (7)].id), val, 0, 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 2632 "parser.y"
    {
                    String *val = (yyvsp[(6) - (6)].str) ? NewString((yyvsp[(6) - (6)].str)) : NewString("1");
                    new_feature((yyvsp[(3) - (6)].id), val, (yyvsp[(4) - (6)].node), 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 2638 "parser.y"
    {
                    String *val = Len((yyvsp[(5) - (8)].id)) ? NewString((yyvsp[(5) - (8)].id)) : 0;
                    new_feature((yyvsp[(3) - (8)].id), val, (yyvsp[(6) - (8)].node), 0, 0, 0, 0);
                    (yyval.node) = 0;
                    scanner_clear_rename();
                  }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 2646 "parser.y"
    { (yyval.str) = (yyvsp[(1) - (1)].str); }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 2647 "parser.y"
    { (yyval.str) = 0; }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 2648 "parser.y"
    { (yyval.str) = (yyvsp[(3) - (5)].pl); }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 2651 "parser.y"
    {
		  (yyval.node) = NewHash();
		  Setattr((yyval.node),"name",(yyvsp[(2) - (4)].id));
		  Setattr((yyval.node),"value",(yyvsp[(4) - (4)].id));
                }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 2656 "parser.y"
    {
		  (yyval.node) = NewHash();
		  Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
		  Setattr((yyval.node),"value",(yyvsp[(4) - (5)].id));
                  set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
                }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 2666 "parser.y"
    {
                 Parm *val;
		 String *name;
		 SwigType *t;
		 if (Namespaceprefix) name = NewStringf("%s::%s", Namespaceprefix, (yyvsp[(5) - (7)].decl).id);
		 else name = NewString((yyvsp[(5) - (7)].decl).id);
		 val = (yyvsp[(3) - (7)].pl);
		 if ((yyvsp[(5) - (7)].decl).parms) {
		   Setmeta(val,"parms",(yyvsp[(5) - (7)].decl).parms);
		 }
		 t = (yyvsp[(5) - (7)].decl).type;
		 if (!Len(t)) t = 0;
		 if (t) {
		   if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push(t,(yyvsp[(6) - (7)].dtype).qualifier);
		   if (SwigType_isfunction(t)) {
		     SwigType *decl = SwigType_pop_function(t);
		     if (SwigType_ispointer(t)) {
		       String *nname = NewStringf("*%s",name);
		       Swig_feature_set(Swig_cparse_features(), nname, decl, "feature:varargs", val, 0);
		       Delete(nname);
		     } else {
		       Swig_feature_set(Swig_cparse_features(), name, decl, "feature:varargs", val, 0);
		     }
		     Delete(decl);
		   } else if (SwigType_ispointer(t)) {
		     String *nname = NewStringf("*%s",name);
		     Swig_feature_set(Swig_cparse_features(),nname,0,"feature:varargs",val, 0);
		     Delete(nname);
		   }
		 } else {
		   Swig_feature_set(Swig_cparse_features(),name,0,"feature:varargs",val, 0);
		 }
		 Delete(name);
		 (yyval.node) = 0;
              }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 2702 "parser.y"
    { (yyval.pl) = (yyvsp[(1) - (1)].pl); }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 2703 "parser.y"
    { 
		  int i;
		  int n;
		  Parm *p;
		  n = atoi(Char((yyvsp[(1) - (3)].dtype).val));
		  if (n <= 0) {
		    Swig_error(cparse_file, cparse_line,"Argument count in %%varargs must be positive.\n");
		    (yyval.pl) = 0;
		  } else {
		    String *name = Getattr((yyvsp[(3) - (3)].p), "name");
		    (yyval.pl) = Copy((yyvsp[(3) - (3)].p));
		    if (name)
		      Setattr((yyval.pl), "name", NewStringf("%s%d", name, n));
		    for (i = 1; i < n; i++) {
		      p = Copy((yyvsp[(3) - (3)].p));
		      name = Getattr(p, "name");
		      if (name)
		        Setattr(p, "name", NewStringf("%s%d", name, n-i));
		      set_nextSibling(p,(yyval.pl));
		      Delete((yyval.pl));
		      (yyval.pl) = p;
		    }
		  }
                }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 2738 "parser.y"
    {
		   (yyval.node) = 0;
		   if ((yyvsp[(3) - (6)].tmap).method) {
		     String *code = 0;
		     (yyval.node) = new_node("typemap");
		     Setattr((yyval.node),"method",(yyvsp[(3) - (6)].tmap).method);
		     if ((yyvsp[(3) - (6)].tmap).kwargs) {
		       ParmList *kw = (yyvsp[(3) - (6)].tmap).kwargs;
                       code = remove_block(kw, (yyvsp[(6) - (6)].str));
		       Setattr((yyval.node),"kwargs", (yyvsp[(3) - (6)].tmap).kwargs);
		     }
		     code = code ? code : NewString((yyvsp[(6) - (6)].str));
		     Setattr((yyval.node),"code", code);
		     Delete(code);
		     appendChild((yyval.node),(yyvsp[(5) - (6)].p));
		   }
	       }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 2755 "parser.y"
    {
		 (yyval.node) = 0;
		 if ((yyvsp[(3) - (6)].tmap).method) {
		   (yyval.node) = new_node("typemap");
		   Setattr((yyval.node),"method",(yyvsp[(3) - (6)].tmap).method);
		   appendChild((yyval.node),(yyvsp[(5) - (6)].p));
		 }
	       }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 2763 "parser.y"
    {
		   (yyval.node) = 0;
		   if ((yyvsp[(3) - (8)].tmap).method) {
		     (yyval.node) = new_node("typemapcopy");
		     Setattr((yyval.node),"method",(yyvsp[(3) - (8)].tmap).method);
		     Setattr((yyval.node),"pattern", Getattr((yyvsp[(7) - (8)].p),"pattern"));
		     appendChild((yyval.node),(yyvsp[(5) - (8)].p));
		   }
	       }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 2776 "parser.y"
    {
		 Hash *p;
		 String *name;
		 p = nextSibling((yyvsp[(1) - (1)].node));
		 if (p && (!Getattr(p,"value"))) {
 		   /* this is the deprecated two argument typemap form */
 		   Swig_warning(WARN_DEPRECATED_TYPEMAP_LANG,cparse_file, cparse_line,
				"Specifying the language name in %%typemap is deprecated - use #ifdef SWIG<LANG> instead.\n");
		   /* two argument typemap form */
		   name = Getattr((yyvsp[(1) - (1)].node),"name");
		   if (!name || (Strcmp(name,typemap_lang))) {
		     (yyval.tmap).method = 0;
		     (yyval.tmap).kwargs = 0;
		   } else {
		     (yyval.tmap).method = Getattr(p,"name");
		     (yyval.tmap).kwargs = nextSibling(p);
		   }
		 } else {
		   /* one-argument typemap-form */
		   (yyval.tmap).method = Getattr((yyvsp[(1) - (1)].node),"name");
		   (yyval.tmap).kwargs = p;
		 }
                }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 2801 "parser.y"
    {
                 (yyval.p) = (yyvsp[(1) - (2)].p);
		 set_nextSibling((yyval.p),(yyvsp[(2) - (2)].p));
		}
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 2807 "parser.y"
    {
                 (yyval.p) = (yyvsp[(2) - (3)].p);
		 set_nextSibling((yyval.p),(yyvsp[(3) - (3)].p));
                }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 2811 "parser.y"
    { (yyval.p) = 0;}
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 2814 "parser.y"
    {
                  Parm *parm;
		  SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		  (yyval.p) = new_node("typemapitem");
		  parm = NewParmWithoutFileLineInfo((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).id);
		  Setattr((yyval.p),"pattern",parm);
		  Setattr((yyval.p),"parms", (yyvsp[(2) - (2)].decl).parms);
		  Delete(parm);
		  /*		  $$ = NewParmWithoutFileLineInfo($1,$2.id);
				  Setattr($$,"parms",$2.parms); */
                }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 2825 "parser.y"
    {
                  (yyval.p) = new_node("typemapitem");
		  Setattr((yyval.p),"pattern",(yyvsp[(2) - (3)].pl));
		  /*		  Setattr($$,"multitype",$2); */
               }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 2830 "parser.y"
    {
		 (yyval.p) = new_node("typemapitem");
		 Setattr((yyval.p),"pattern", (yyvsp[(2) - (6)].pl));
		 /*                 Setattr($$,"multitype",$2); */
		 Setattr((yyval.p),"parms",(yyvsp[(5) - (6)].pl));
               }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 2843 "parser.y"
    {
                   (yyval.node) = new_node("types");
		   Setattr((yyval.node),"parms",(yyvsp[(3) - (5)].pl));
                   if ((yyvsp[(5) - (5)].str))
		     Setattr((yyval.node),"convcode",NewString((yyvsp[(5) - (5)].str)));
               }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 2855 "parser.y"
    {
                  Parm *p, *tp;
		  Node *n;
		  Symtab *tscope = 0;
		  int     specialized = 0;
		  int     variadic = 0;

		  (yyval.node) = 0;

		  tscope = Swig_symbol_current();          /* Get the current scope */

		  /* If the class name is qualified, we need to create or lookup namespace entries */
		  if (!inclass) {
		    (yyvsp[(5) - (9)].str) = resolve_create_node_scope((yyvsp[(5) - (9)].str));
		  }

		  /*
		    We use the new namespace entry 'nscope' only to
		    emit the template node. The template parameters are
		    resolved in the current 'tscope'.

		    This is closer to the C++ (typedef) behavior.
		  */
		  n = Swig_cparse_template_locate((yyvsp[(5) - (9)].str),(yyvsp[(7) - (9)].p),tscope);

		  /* Patch the argument types to respect namespaces */
		  p = (yyvsp[(7) - (9)].p);
		  while (p) {
		    SwigType *value = Getattr(p,"value");
		    if (!value) {
		      SwigType *ty = Getattr(p,"type");
		      if (ty) {
			SwigType *rty = 0;
			int reduce = template_reduce;
			if (reduce || !SwigType_ispointer(ty)) {
			  rty = Swig_symbol_typedef_reduce(ty,tscope);
			  if (!reduce) reduce = SwigType_ispointer(rty);
			}
			ty = reduce ? Swig_symbol_type_qualify(rty,tscope) : Swig_symbol_type_qualify(ty,tscope);
			Setattr(p,"type",ty);
			Delete(ty);
			Delete(rty);
		      }
		    } else {
		      value = Swig_symbol_type_qualify(value,tscope);
		      Setattr(p,"value",value);
		      Delete(value);
		    }

		    p = nextSibling(p);
		  }

		  /* Look for the template */
		  {
                    Node *nn = n;
                    Node *linklistend = 0;
                    while (nn) {
                      Node *templnode = 0;
                      if (Strcmp(nodeType(nn),"template") == 0) {
                        int nnisclass = (Strcmp(Getattr(nn,"templatetype"),"class") == 0); /* if not a templated class it is a templated function */
                        Parm *tparms = Getattr(nn,"templateparms");
                        if (!tparms) {
                          specialized = 1;
                        } else if (Getattr(tparms,"variadic") && strncmp(Char(Getattr(tparms,"variadic")), "1", 1)==0) {
                          variadic = 1;
                        }
                        if (nnisclass && !variadic && !specialized && (ParmList_len((yyvsp[(7) - (9)].p)) > ParmList_len(tparms))) {
                          Swig_error(cparse_file, cparse_line, "Too many template parameters. Maximum of %d.\n", ParmList_len(tparms));
                        } else if (nnisclass && !specialized && ((ParmList_len((yyvsp[(7) - (9)].p)) < (ParmList_numrequired(tparms) - (variadic?1:0))))) { /* Variadic parameter is optional */
                          Swig_error(cparse_file, cparse_line, "Not enough template parameters specified. %d required.\n", (ParmList_numrequired(tparms)-(variadic?1:0)) );
                        } else if (!nnisclass && ((ParmList_len((yyvsp[(7) - (9)].p)) != ParmList_len(tparms)))) {
                          /* must be an overloaded templated method - ignore it as it is overloaded with a different number of template parameters */
                          nn = Getattr(nn,"sym:nextSibling"); /* repeat for overloaded templated functions */
                          continue;
                        } else {
			  String *tname = Copy((yyvsp[(5) - (9)].str));
                          int def_supplied = 0;
                          /* Expand the template */
			  Node *templ = Swig_symbol_clookup((yyvsp[(5) - (9)].str),0);
			  Parm *targs = templ ? Getattr(templ,"templateparms") : 0;

                          ParmList *temparms;
                          if (specialized) temparms = CopyParmList((yyvsp[(7) - (9)].p));
                          else temparms = CopyParmList(tparms);

                          /* Create typedef's and arguments */
                          p = (yyvsp[(7) - (9)].p);
                          tp = temparms;
                          if (!p && ParmList_len(p) != ParmList_len(temparms)) {
                            /* we have no template parameters supplied in %template for a template that has default args*/
                            p = tp;
                            def_supplied = 1;
                          }

                          while (p) {
                            String *value = Getattr(p,"value");
                            if (def_supplied) {
                              Setattr(p,"default","1");
                            }
                            if (value) {
                              Setattr(tp,"value",value);
                            } else {
                              SwigType *ty = Getattr(p,"type");
                              if (ty) {
                                Setattr(tp,"type",ty);
                              }
                              Delattr(tp,"value");
                            }
			    /* fix default arg values */
			    if (targs) {
			      Parm *pi = temparms;
			      Parm *ti = targs;
			      String *tv = Getattr(tp,"value");
			      if (!tv) tv = Getattr(tp,"type");
			      while(pi != tp && ti && pi) {
				String *name = Getattr(ti,"name");
				String *value = Getattr(pi,"value");
				if (!value) value = Getattr(pi,"type");
				Replaceid(tv, name, value);
				pi = nextSibling(pi);
				ti = nextSibling(ti);
			      }
			    }
                            p = nextSibling(p);
                            tp = nextSibling(tp);
                            if (!p && tp) {
                              p = tp;
                              def_supplied = 1;
                            } else if (p && !tp) { /* Variadic template - tp < p */
			      SWIG_WARN_NODE_BEGIN(nn);
                              Swig_warning(WARN_CPP11_VARIADIC_TEMPLATE,cparse_file, cparse_line,"Only the first variadic template argument is currently supported.\n");
			      SWIG_WARN_NODE_END(nn);
                              break;
                            }
                          }

                          templnode = copy_node(nn);
                          /* We need to set the node name based on name used to instantiate */
                          Setattr(templnode,"name",tname);
			  Delete(tname);
                          if (!specialized) {
                            Delattr(templnode,"sym:typename");
                          } else {
                            Setattr(templnode,"sym:typename","1");
                          }
                          if ((yyvsp[(3) - (9)].id) && !inclass) {
			    /*
			       Comment this out for 1.3.28. We need to
			       re-enable it later but first we need to
			       move %ignore from using %rename to use
			       %feature(ignore).

			       String *symname = Swig_name_make(templnode,0,$3,0,0);
			    */
			    String *symname = (yyvsp[(3) - (9)].id);
                            Swig_cparse_template_expand(templnode,symname,temparms,tscope);
                            Setattr(templnode,"sym:name",symname);
                          } else {
                            static int cnt = 0;
                            String *nname = NewStringf("__dummy_%d__", cnt++);
                            Swig_cparse_template_expand(templnode,nname,temparms,tscope);
                            Setattr(templnode,"sym:name",nname);
			    Delete(nname);
                            Setattr(templnode,"feature:onlychildren", "typemap,typemapitem,typemapcopy,typedef,types,fragment");

			    if ((yyvsp[(3) - (9)].id)) {
			      Swig_warning(WARN_PARSE_NESTED_TEMPLATE, cparse_file, cparse_line, "Named nested template instantiations not supported. Processing as if no name was given to %%template().\n");
			    }
                          }
                          Delattr(templnode,"templatetype");
                          Setattr(templnode,"template",nn);
                          Setfile(templnode,cparse_file);
                          Setline(templnode,cparse_line);
                          Delete(temparms);

                          add_symbols_copy(templnode);

                          if (Strcmp(nodeType(templnode),"class") == 0) {

                            /* Identify pure abstract methods */
                            Setattr(templnode,"abstracts", pure_abstracts(firstChild(templnode)));

                            /* Set up inheritance in symbol table */
                            {
                              Symtab  *csyms;
                              List *baselist = Getattr(templnode,"baselist");
                              csyms = Swig_symbol_current();
                              Swig_symbol_setscope(Getattr(templnode,"symtab"));
                              if (baselist) {
                                List *bases = make_inherit_list(Getattr(templnode,"name"),baselist);
                                if (bases) {
                                  Iterator s;
                                  for (s = First(bases); s.item; s = Next(s)) {
                                    Symtab *st = Getattr(s.item,"symtab");
                                    if (st) {
				      Setfile(st,Getfile(s.item));
				      Setline(st,Getline(s.item));
                                      Swig_symbol_inherit(st);
                                    }
                                  }
				  Delete(bases);
                                }
                              }
                              Swig_symbol_setscope(csyms);
                            }

                            /* Merge in %extend methods for this class */

			    /* !!! This may be broken.  We may have to add the
			       %extend methods at the beginning of the class */

                            if (extendhash) {
                              String *stmp = 0;
                              String *clsname;
                              Node *am;
                              if (Namespaceprefix) {
                                clsname = stmp = NewStringf("%s::%s", Namespaceprefix, Getattr(templnode,"name"));
                              } else {
                                clsname = Getattr(templnode,"name");
                              }
                              am = Getattr(extendhash,clsname);
                              if (am) {
                                Symtab *st = Swig_symbol_current();
                                Swig_symbol_setscope(Getattr(templnode,"symtab"));
                                /*			    Printf(stdout,"%s: %s %p %p\n", Getattr(templnode,"name"), clsname, Swig_symbol_current(), Getattr(templnode,"symtab")); */
                                merge_extensions(templnode,am);
                                Swig_symbol_setscope(st);
				append_previous_extension(templnode,am);
                                Delattr(extendhash,clsname);
                              }
			      if (stmp) Delete(stmp);
                            }
                            /* Add to classes hash */
                            if (!classes) classes = NewHash();

                            {
                              if (Namespaceprefix) {
                                String *temp = NewStringf("%s::%s", Namespaceprefix, Getattr(templnode,"name"));
                                Setattr(classes,temp,templnode);
				Delete(temp);
                              } else {
				String *qs = Swig_symbol_qualifiedscopename(templnode);
                                Setattr(classes, qs,templnode);
				Delete(qs);
                              }
                            }
                          }
                        }

                        /* all the overloaded templated functions are added into a linked list */
                        if (nscope_inner) {
                          /* non-global namespace */
                          if (templnode) {
                            appendChild(nscope_inner,templnode);
			    Delete(templnode);
                            if (nscope) (yyval.node) = nscope;
                          }
                        } else {
                          /* global namespace */
                          if (!linklistend) {
                            (yyval.node) = templnode;
                          } else {
                            set_nextSibling(linklistend,templnode);
			    Delete(templnode);
                          }
                          linklistend = templnode;
                        }
                      }
                      nn = Getattr(nn,"sym:nextSibling"); /* repeat for overloaded templated functions. If a templated class there will never be a sibling. */
                    }
		  }
	          Swig_symbol_setscope(tscope);
		  Delete(Namespaceprefix);
		  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
                }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 3137 "parser.y"
    {
		  Swig_warning(0,cparse_file, cparse_line,"%s\n", (yyvsp[(2) - (2)].id));
		  (yyval.node) = 0;
               }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 3147 "parser.y"
    {
                    (yyval.node) = (yyvsp[(1) - (1)].node); 
                    if ((yyval.node)) {
   		      add_symbols((yyval.node));
                      default_arguments((yyval.node));
   	            }
                }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 3154 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 115:

/* Line 1806 of yacc.c  */
#line 3155 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 116:

/* Line 1806 of yacc.c  */
#line 3159 "parser.y"
    {
		  if (Strcmp((yyvsp[(2) - (3)].id),"C") == 0) {
		    cparse_externc = 1;
		  }
		}
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 3163 "parser.y"
    {
		  cparse_externc = 0;
		  if (Strcmp((yyvsp[(2) - (6)].id),"C") == 0) {
		    Node *n = firstChild((yyvsp[(5) - (6)].node));
		    (yyval.node) = new_node("extern");
		    Setattr((yyval.node),"name",(yyvsp[(2) - (6)].id));
		    appendChild((yyval.node),n);
		    while (n) {
		      SwigType *decl = Getattr(n,"decl");
		      if (SwigType_isfunction(decl) && !Equal(Getattr(n, "storage"), "typedef")) {
			Setattr(n,"storage","externc");
		      }
		      n = nextSibling(n);
		    }
		  } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (6)].id));
		    (yyval.node) = new_node("extern");
		    Setattr((yyval.node),"name",(yyvsp[(2) - (6)].id));
		    appendChild((yyval.node),firstChild((yyvsp[(5) - (6)].node)));
		  }
                }
    break;

  case 118:

/* Line 1806 of yacc.c  */
#line 3184 "parser.y"
    {
		  (yyval.node) = (yyvsp[(1) - (1)].node);
		  SWIG_WARN_NODE_BEGIN((yyval.node));
		  Swig_warning(WARN_CPP11_LAMBDA, cparse_file, cparse_line, "Lambda expressions and closures are not fully supported yet.\n");
		  SWIG_WARN_NODE_END((yyval.node));
		}
    break;

  case 119:

/* Line 1806 of yacc.c  */
#line 3190 "parser.y"
    {
		  skip_decl();
		  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"name",(yyvsp[(2) - (3)].str));
		  add_symbols((yyval.node));
		  SWIG_WARN_NODE_BEGIN((yyval.node));
		  Swig_warning(WARN_CPP11_ALIAS_DECLARATION, cparse_file, cparse_line, "The 'using' keyword in type aliasing is not fully supported yet.\n");
		  SWIG_WARN_NODE_END((yyval.node));

		  (yyval.node) = 0; /* TODO - ignored for now */
		}
    break;

  case 120:

/* Line 1806 of yacc.c  */
#line 3201 "parser.y"
    {
		  skip_decl();
		  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"name",(yyvsp[(6) - (7)].str));
		  add_symbols((yyval.node));
		  SWIG_WARN_NODE_BEGIN((yyval.node));
		  Swig_warning(WARN_CPP11_ALIAS_TEMPLATE, cparse_file, cparse_line, "The 'using' keyword in template aliasing is not fully supported yet.\n");
		  SWIG_WARN_NODE_END((yyval.node));

		  (yyval.node) = 0; /* TODO - ignored for now */
		}
    break;

  case 121:

/* Line 1806 of yacc.c  */
#line 3218 "parser.y"
    {
              (yyval.node) = new_node("cdecl");
	      if ((yyvsp[(4) - (5)].dtype).qualifier) SwigType_push((yyvsp[(3) - (5)].decl).type,(yyvsp[(4) - (5)].dtype).qualifier);
	      Setattr((yyval.node),"type",(yyvsp[(2) - (5)].type));
	      Setattr((yyval.node),"storage",(yyvsp[(1) - (5)].id));
	      Setattr((yyval.node),"name",(yyvsp[(3) - (5)].decl).id);
	      Setattr((yyval.node),"decl",(yyvsp[(3) - (5)].decl).type);
	      Setattr((yyval.node),"parms",(yyvsp[(3) - (5)].decl).parms);
	      Setattr((yyval.node),"value",(yyvsp[(4) - (5)].dtype).val);
	      Setattr((yyval.node),"throws",(yyvsp[(4) - (5)].dtype).throws);
	      Setattr((yyval.node),"throw",(yyvsp[(4) - (5)].dtype).throwf);
	      if (!(yyvsp[(5) - (5)].node)) {
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
	      } else {
		Node *n = (yyvsp[(5) - (5)].node);
		/* Inherit attributes */
		while (n) {
		  String *type = Copy((yyvsp[(2) - (5)].type));
		  Setattr(n,"type",type);
		  Setattr(n,"storage",(yyvsp[(1) - (5)].id));
		  n = nextSibling(n);
		  Delete(type);
		}
	      }
	      if ((yyvsp[(4) - (5)].dtype).bitfield) {
		Setattr((yyval.node),"bitfield", (yyvsp[(4) - (5)].dtype).bitfield);
	      }

	      /* Look for "::" declarations (ignored) */
	      if (Strstr((yyvsp[(3) - (5)].decl).id,"::")) {
                /* This is a special case. If the scope name of the declaration exactly
                   matches that of the declaration, then we will allow it. Otherwise, delete. */
                String *p = Swig_scopename_prefix((yyvsp[(3) - (5)].decl).id);
		if (p) {
		  if ((Namespaceprefix && Strcmp(p,Namespaceprefix) == 0) ||
		      (inclass && Strcmp(p,Classprefix) == 0)) {
		    String *lstr = Swig_scopename_last((yyvsp[(3) - (5)].decl).id);
		    Setattr((yyval.node),"name",lstr);
		    Delete(lstr);
		    set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
		  } else {
		    Delete((yyval.node));
		    (yyval.node) = (yyvsp[(5) - (5)].node);
		  }
		  Delete(p);
		} else {
		  Delete((yyval.node));
		  (yyval.node) = (yyvsp[(5) - (5)].node);
		}
	      } else {
		set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
	      }
           }
    break;

  case 122:

/* Line 1806 of yacc.c  */
#line 3277 "parser.y"
    {
              (yyval.node) = new_node("cdecl");
	      if ((yyvsp[(6) - (7)].dtype).qualifier) SwigType_push((yyvsp[(3) - (7)].decl).type,(yyvsp[(6) - (7)].dtype).qualifier);
	      Setattr((yyval.node),"type",(yyvsp[(5) - (7)].node));
	      Setattr((yyval.node),"storage",(yyvsp[(1) - (7)].id));
	      Setattr((yyval.node),"name",(yyvsp[(3) - (7)].decl).id);
	      Setattr((yyval.node),"decl",(yyvsp[(3) - (7)].decl).type);
	      Setattr((yyval.node),"parms",(yyvsp[(3) - (7)].decl).parms);
	      Setattr((yyval.node),"value",(yyvsp[(6) - (7)].dtype).val);
	      Setattr((yyval.node),"throws",(yyvsp[(6) - (7)].dtype).throws);
	      Setattr((yyval.node),"throw",(yyvsp[(6) - (7)].dtype).throwf);
	      if (!(yyvsp[(7) - (7)].node)) {
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
	      } else {
		Node *n = (yyvsp[(7) - (7)].node);
		while (n) {
		  String *type = Copy((yyvsp[(5) - (7)].node));
		  Setattr(n,"type",type);
		  Setattr(n,"storage",(yyvsp[(1) - (7)].id));
		  n = nextSibling(n);
		  Delete(type);
		}
	      }
	      if ((yyvsp[(6) - (7)].dtype).bitfield) {
		Setattr((yyval.node),"bitfield", (yyvsp[(6) - (7)].dtype).bitfield);
	      }

	      if (Strstr((yyvsp[(3) - (7)].decl).id,"::")) {
                String *p = Swig_scopename_prefix((yyvsp[(3) - (7)].decl).id);
		if (p) {
		  if ((Namespaceprefix && Strcmp(p,Namespaceprefix) == 0) ||
		      (inclass && Strcmp(p,Classprefix) == 0)) {
		    String *lstr = Swig_scopename_last((yyvsp[(3) - (7)].decl).id);
		    Setattr((yyval.node),"name",lstr);
		    Delete(lstr);
		    set_nextSibling((yyval.node),(yyvsp[(7) - (7)].node));
		  } else {
		    Delete((yyval.node));
		    (yyval.node) = (yyvsp[(7) - (7)].node);
		  }
		  Delete(p);
		} else {
		  Delete((yyval.node));
		  (yyval.node) = (yyvsp[(7) - (7)].node);
		}
	      } else {
		set_nextSibling((yyval.node),(yyvsp[(7) - (7)].node));
	      }
           }
    break;

  case 123:

/* Line 1806 of yacc.c  */
#line 3334 "parser.y"
    { 
                   (yyval.node) = 0;
                   Clear(scanner_ccode); 
               }
    break;

  case 124:

/* Line 1806 of yacc.c  */
#line 3338 "parser.y"
    {
		 (yyval.node) = new_node("cdecl");
		 if ((yyvsp[(3) - (4)].dtype).qualifier) SwigType_push((yyvsp[(2) - (4)].decl).type,(yyvsp[(3) - (4)].dtype).qualifier);
		 Setattr((yyval.node),"name",(yyvsp[(2) - (4)].decl).id);
		 Setattr((yyval.node),"decl",(yyvsp[(2) - (4)].decl).type);
		 Setattr((yyval.node),"parms",(yyvsp[(2) - (4)].decl).parms);
		 Setattr((yyval.node),"value",(yyvsp[(3) - (4)].dtype).val);
		 Setattr((yyval.node),"throws",(yyvsp[(3) - (4)].dtype).throws);
		 Setattr((yyval.node),"throw",(yyvsp[(3) - (4)].dtype).throwf);
		 if ((yyvsp[(3) - (4)].dtype).bitfield) {
		   Setattr((yyval.node),"bitfield", (yyvsp[(3) - (4)].dtype).bitfield);
		 }
		 if (!(yyvsp[(4) - (4)].node)) {
		   if (Len(scanner_ccode)) {
		     String *code = Copy(scanner_ccode);
		     Setattr((yyval.node),"code",code);
		     Delete(code);
		   }
		 } else {
		   set_nextSibling((yyval.node),(yyvsp[(4) - (4)].node));
		 }
	       }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 3360 "parser.y"
    { 
                   skip_balanced('{','}');
                   (yyval.node) = 0;
               }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 3366 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(1) - (1)].dtype); 
                   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
              }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 3372 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
		   (yyval.dtype).qualifier = (yyvsp[(1) - (2)].str);
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
	      }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 3378 "parser.y"
    { 
		   (yyval.dtype) = (yyvsp[(5) - (5)].dtype); 
                   (yyval.dtype).qualifier = 0;
		   (yyval.dtype).throws = (yyvsp[(3) - (5)].pl);
		   (yyval.dtype).throwf = NewString("1");
              }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 3384 "parser.y"
    { 
                   (yyval.dtype) = (yyvsp[(6) - (6)].dtype); 
                   (yyval.dtype).qualifier = (yyvsp[(1) - (6)].str);
		   (yyval.dtype).throws = (yyvsp[(4) - (6)].pl);
		   (yyval.dtype).throwf = NewString("1");
              }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 3392 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 3393 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 3394 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 3395 "parser.y"
    { (yyval.node) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 3396 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 3397 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].str); }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 3398 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].type); }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 3409 "parser.y"
    {
		  (yyval.node) = new_node("lambda");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (11)].str));
		  add_symbols((yyval.node));
	        }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 3414 "parser.y"
    {
		  (yyval.node) = new_node("lambda");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (13)].str));
		  add_symbols((yyval.node));
		}
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 3419 "parser.y"
    {
		  (yyval.node) = new_node("lambda");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (7)].str));
		  add_symbols((yyval.node));
		}
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 3426 "parser.y"
    {
		  skip_balanced('[',']');
		  (yyval.node) = 0;
	        }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 3432 "parser.y"
    {
		  skip_balanced('{','}');
		  (yyval.node) = 0;
		}
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 3437 "parser.y"
    {
		  (yyval.pl) = 0;
		}
    break;

  case 143:

/* Line 1806 of yacc.c  */
#line 3440 "parser.y"
    {
		  skip_balanced('(',')');
		}
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 3442 "parser.y"
    {
		  (yyval.pl) = 0;
		}
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 3453 "parser.y"
    {
		   (yyval.node) = (char *)"enum";
	      }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 3456 "parser.y"
    {
		   (yyval.node) = (char *)"enum class";
	      }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 3459 "parser.y"
    {
		   (yyval.node) = (char *)"enum struct";
	      }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 3468 "parser.y"
    {
                   (yyval.node) = (yyvsp[(2) - (2)].type);
              }
    break;

  case 149:

/* Line 1806 of yacc.c  */
#line 3471 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 150:

/* Line 1806 of yacc.c  */
#line 3478 "parser.y"
    {
		   SwigType *ty = 0;
		   int scopedenum = (yyvsp[(3) - (5)].id) && !Equal((yyvsp[(2) - (5)].node), "enum");
		   (yyval.node) = new_node("enumforward");
		   ty = NewStringf("enum %s", (yyvsp[(3) - (5)].id));
		   Setattr((yyval.node),"enumkey",(yyvsp[(2) - (5)].node));
		   if (scopedenum)
		     SetFlag((yyval.node), "scopedenum");
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].id));
		   Setattr((yyval.node),"inherit",(yyvsp[(4) - (5)].node));
		   Setattr((yyval.node),"type",ty);
		   Setattr((yyval.node),"sym:weak", "1");
		   add_symbols((yyval.node));
	      }
    break;

  case 151:

/* Line 1806 of yacc.c  */
#line 3500 "parser.y"
    {
		  SwigType *ty = 0;
		  int scopedenum = (yyvsp[(3) - (8)].id) && !Equal((yyvsp[(2) - (8)].node), "enum");
                  (yyval.node) = new_node("enum");
		  ty = NewStringf("enum %s", (yyvsp[(3) - (8)].id));
		  Setattr((yyval.node),"enumkey",(yyvsp[(2) - (8)].node));
		  if (scopedenum)
		    SetFlag((yyval.node), "scopedenum");
		  Setattr((yyval.node),"name",(yyvsp[(3) - (8)].id));
		  Setattr((yyval.node),"inherit",(yyvsp[(4) - (8)].node));
		  Setattr((yyval.node),"type",ty);
		  appendChild((yyval.node),(yyvsp[(6) - (8)].node));
		  add_symbols((yyval.node));      /* Add to tag space */

		  if (scopedenum) {
		    Swig_symbol_newscope();
		    Swig_symbol_setscopename((yyvsp[(3) - (8)].id));
		    Delete(Namespaceprefix);
		    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		  }

		  add_symbols((yyvsp[(6) - (8)].node));      /* Add enum values to appropriate enum or enum class scope */

		  if (scopedenum) {
		    Setattr((yyval.node),"symtab", Swig_symbol_popscope());
		    Delete(Namespaceprefix);
		    Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		  }
               }
    break;

  case 152:

/* Line 1806 of yacc.c  */
#line 3529 "parser.y"
    {
		 Node *n;
		 SwigType *ty = 0;
		 String   *unnamed = 0;
		 int       unnamedinstance = 0;
		 int scopedenum = (yyvsp[(3) - (10)].id) && !Equal((yyvsp[(2) - (10)].node), "enum");

		 (yyval.node) = new_node("enum");
		 Setattr((yyval.node),"enumkey",(yyvsp[(2) - (10)].node));
		 if (scopedenum)
		   SetFlag((yyval.node), "scopedenum");
		 Setattr((yyval.node),"inherit",(yyvsp[(4) - (10)].node));
		 if ((yyvsp[(3) - (10)].id)) {
		   Setattr((yyval.node),"name",(yyvsp[(3) - (10)].id));
		   ty = NewStringf("enum %s", (yyvsp[(3) - (10)].id));
		 } else if ((yyvsp[(8) - (10)].decl).id) {
		   unnamed = make_unnamed();
		   ty = NewStringf("enum %s", unnamed);
		   Setattr((yyval.node),"unnamed",unnamed);
                   /* name is not set for unnamed enum instances, e.g. enum { foo } Instance; */
		   if ((yyvsp[(1) - (10)].id) && Cmp((yyvsp[(1) - (10)].id),"typedef") == 0) {
		     Setattr((yyval.node),"name",(yyvsp[(8) - (10)].decl).id);
                   } else {
                     unnamedinstance = 1;
                   }
		   Setattr((yyval.node),"storage",(yyvsp[(1) - (10)].id));
		 }
		 if ((yyvsp[(8) - (10)].decl).id && Cmp((yyvsp[(1) - (10)].id),"typedef") == 0) {
		   Setattr((yyval.node),"tdname",(yyvsp[(8) - (10)].decl).id);
                   Setattr((yyval.node),"allows_typedef","1");
                 }
		 appendChild((yyval.node),(yyvsp[(6) - (10)].node));
		 n = new_node("cdecl");
		 Setattr(n,"type",ty);
		 Setattr(n,"name",(yyvsp[(8) - (10)].decl).id);
		 Setattr(n,"storage",(yyvsp[(1) - (10)].id));
		 Setattr(n,"decl",(yyvsp[(8) - (10)].decl).type);
		 Setattr(n,"parms",(yyvsp[(8) - (10)].decl).parms);
		 Setattr(n,"unnamed",unnamed);

                 if (unnamedinstance) {
		   SwigType *cty = NewString("enum ");
		   Setattr((yyval.node),"type",cty);
		   SetFlag((yyval.node),"unnamedinstance");
		   SetFlag(n,"unnamedinstance");
		   Delete(cty);
                 }
		 if ((yyvsp[(10) - (10)].node)) {
		   Node *p = (yyvsp[(10) - (10)].node);
		   set_nextSibling(n,p);
		   while (p) {
		     SwigType *cty = Copy(ty);
		     Setattr(p,"type",cty);
		     Setattr(p,"unnamed",unnamed);
		     Setattr(p,"storage",(yyvsp[(1) - (10)].id));
		     Delete(cty);
		     p = nextSibling(p);
		   }
		 } else {
		   if (Len(scanner_ccode)) {
		     String *code = Copy(scanner_ccode);
		     Setattr(n,"code",code);
		     Delete(code);
		   }
		 }

                 /* Ensure that typedef enum ABC {foo} XYZ; uses XYZ for sym:name, like structs.
                  * Note that class_rename/yyrename are bit of a mess so used this simple approach to change the name. */
                 if ((yyvsp[(8) - (10)].decl).id && (yyvsp[(3) - (10)].id) && Cmp((yyvsp[(1) - (10)].id),"typedef") == 0) {
		   String *name = NewString((yyvsp[(8) - (10)].decl).id);
                   Setattr((yyval.node), "parser:makename", name);
		   Delete(name);
                 }

		 add_symbols((yyval.node));       /* Add enum to tag space */
		 set_nextSibling((yyval.node),n);
		 Delete(n);

		 if (scopedenum) {
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename((yyvsp[(3) - (10)].id));
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 }

		 add_symbols((yyvsp[(6) - (10)].node));      /* Add enum values to appropriate enum or enum class scope */

		 if (scopedenum) {
		   Setattr((yyval.node),"symtab", Swig_symbol_popscope());
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 }

	         add_symbols(n);
		 Delete(unnamed);
	       }
    break;

  case 153:

/* Line 1806 of yacc.c  */
#line 3627 "parser.y"
    {
                   /* This is a sick hack.  If the ctor_end has parameters,
                      and the parms parameter only has 1 parameter, this
                      could be a declaration of the form:

                         type (id)(parms)

			 Otherwise it's an error. */
                    int err = 0;
                    (yyval.node) = 0;

		    if ((ParmList_len((yyvsp[(4) - (6)].pl)) == 1) && (!Swig_scopename_check((yyvsp[(2) - (6)].type)))) {
		      SwigType *ty = Getattr((yyvsp[(4) - (6)].pl),"type");
		      String *name = Getattr((yyvsp[(4) - (6)].pl),"name");
		      err = 1;
		      if (!name) {
			(yyval.node) = new_node("cdecl");
			Setattr((yyval.node),"type",(yyvsp[(2) - (6)].type));
			Setattr((yyval.node),"storage",(yyvsp[(1) - (6)].id));
			Setattr((yyval.node),"name",ty);

			if ((yyvsp[(6) - (6)].decl).have_parms) {
			  SwigType *decl = NewStringEmpty();
			  SwigType_add_function(decl,(yyvsp[(6) - (6)].decl).parms);
			  Setattr((yyval.node),"decl",decl);
			  Setattr((yyval.node),"parms",(yyvsp[(6) - (6)].decl).parms);
			  if (Len(scanner_ccode)) {
			    String *code = Copy(scanner_ccode);
			    Setattr((yyval.node),"code",code);
			    Delete(code);
			  }
			}
			if ((yyvsp[(6) - (6)].decl).defarg) {
			  Setattr((yyval.node),"value",(yyvsp[(6) - (6)].decl).defarg);
			}
			Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].decl).throws);
			Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].decl).throwf);
			err = 0;
		      }
		    }
		    if (err) {
		      Swig_error(cparse_file,cparse_line,"Syntax error in input(2).\n");
		      exit(1);
		    }
                }
    break;

  case 154:

/* Line 1806 of yacc.c  */
#line 3678 "parser.y"
    {  (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 155:

/* Line 1806 of yacc.c  */
#line 3679 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 156:

/* Line 1806 of yacc.c  */
#line 3680 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 157:

/* Line 1806 of yacc.c  */
#line 3681 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 158:

/* Line 1806 of yacc.c  */
#line 3682 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 159:

/* Line 1806 of yacc.c  */
#line 3683 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 160:

/* Line 1806 of yacc.c  */
#line 3688 "parser.y"
    {
                 if (nested_template == 0) {
                   String *prefix;
                   List *bases = 0;
		   Node *scope = 0;
		   (yyval.node) = new_node("class");
		   Setline((yyval.node),cparse_start_line);
		   Setattr((yyval.node),"kind",(yyvsp[(2) - (5)].id));
		   if ((yyvsp[(4) - (5)].bases)) {
		     Setattr((yyval.node),"baselist", Getattr((yyvsp[(4) - (5)].bases),"public"));
		     Setattr((yyval.node),"protectedbaselist", Getattr((yyvsp[(4) - (5)].bases),"protected"));
		     Setattr((yyval.node),"privatebaselist", Getattr((yyvsp[(4) - (5)].bases),"private"));
		   }
		   Setattr((yyval.node),"allows_typedef","1");

		   /* preserve the current scope */
		   prev_symtab = Swig_symbol_current();
		  
		   /* If the class name is qualified.  We need to create or lookup namespace/scope entries */
		   scope = resolve_create_node_scope((yyvsp[(3) - (5)].str));
		   Setfile(scope,cparse_file);
		   Setline(scope,cparse_line);
		   (yyvsp[(3) - (5)].str) = scope;
		   
		   /* support for old nested classes "pseudo" support, such as:

		         %rename(Ala__Ola) Ala::Ola;
			class Ala::Ola {
			public:
			    Ola() {}
		         };

		      this should disappear when a proper implementation is added.
		   */
		   if (nscope_inner && Strcmp(nodeType(nscope_inner),"namespace") != 0) {
		     if (Namespaceprefix) {
		       String *name = NewStringf("%s::%s", Namespaceprefix, (yyvsp[(3) - (5)].str));		       
		       (yyvsp[(3) - (5)].str) = name;
		       Namespaceprefix = 0;
		       nscope_inner = 0;
		     }
		   }
		   Setattr((yyval.node),"name",(yyvsp[(3) - (5)].str));

		   Delete(class_rename);
                   class_rename = make_name((yyval.node),(yyvsp[(3) - (5)].str),0);
		   Classprefix = NewString((yyvsp[(3) - (5)].str));
		   /* Deal with inheritance  */
		   if ((yyvsp[(4) - (5)].bases)) {
		     bases = make_inherit_list((yyvsp[(3) - (5)].str),Getattr((yyvsp[(4) - (5)].bases),"public"));
		   }
		   prefix = SwigType_istemplate_templateprefix((yyvsp[(3) - (5)].str));
		   if (prefix) {
		     String *fbase, *tbase;
		     if (Namespaceprefix) {
		       fbase = NewStringf("%s::%s", Namespaceprefix,(yyvsp[(3) - (5)].str));
		       tbase = NewStringf("%s::%s", Namespaceprefix, prefix);
		     } else {
		       fbase = Copy((yyvsp[(3) - (5)].str));
		       tbase = Copy(prefix);
		     }
		     Swig_name_inherit(tbase,fbase);
		     Delete(fbase);
		     Delete(tbase);
		   }
                   if (strcmp((yyvsp[(2) - (5)].id),"class") == 0) {
		     cplus_mode = CPLUS_PRIVATE;
		   } else {
		     cplus_mode = CPLUS_PUBLIC;
		   }
		   Swig_symbol_newscope();
		   Swig_symbol_setscopename((yyvsp[(3) - (5)].str));
		   if (bases) {
		     Iterator s;
		     for (s = First(bases); s.item; s = Next(s)) {
		       Symtab *st = Getattr(s.item,"symtab");
		       if (st) {
			 Setfile(st,Getfile(s.item));
			 Setline(st,Getline(s.item));
			 Swig_symbol_inherit(st); 
		       }
		     }
		     Delete(bases);
		   }
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		   cparse_start_line = cparse_line;

		   /* If there are active template parameters, we need to make sure they are
                      placed in the class symbol table so we can catch shadows */

		   if (template_parameters) {
		     Parm *tp = template_parameters;
		     while(tp) {
		       String *tpname = Copy(Getattr(tp,"name"));
		       Node *tn = new_node("templateparm");
		       Setattr(tn,"name",tpname);
		       Swig_symbol_cadd(tpname,tn);
		       tp = nextSibling(tp);
		       Delete(tpname);
		     }
		   }
		   if (class_level >= max_class_levels) {
		       if (!max_class_levels) {
			   max_class_levels = 16;
		       } else {
			   max_class_levels *= 2;
		       }
		       class_decl = (Node**) realloc(class_decl, sizeof(Node*) * max_class_levels);
		       if (!class_decl) {
			   Swig_error(cparse_file, cparse_line, "realloc() failed\n");
		       }
		   }
		   class_decl[class_level++] = (yyval.node);
		   Delete(prefix);
		   inclass = 1;
		 }
               }
    break;

  case 161:

/* Line 1806 of yacc.c  */
#line 3805 "parser.y"
    {
	         (void) (yyvsp[(6) - (9)].node);
		 if (nested_template == 0) {
		   Node *p;
		   SwigType *ty;
		   Symtab *cscope = prev_symtab;
		   Node *am = 0;
		   String *scpname = 0;
		   (yyval.node) = class_decl[--class_level];
		   inclass = 0;
		   
		   /* Check for pure-abstract class */
		   Setattr((yyval.node),"abstracts", pure_abstracts((yyvsp[(7) - (9)].node)));
		   
		   /* This bit of code merges in a previously defined %extend directive (if any) */
		   
		   if (extendhash) {
		     String *clsname = Swig_symbol_qualifiedscopename(0);
		     am = Getattr(extendhash,clsname);
		     if (am) {
		       merge_extensions((yyval.node),am);
		       Delattr(extendhash,clsname);
		     }
		     Delete(clsname);
		   }
		   if (!classes) classes = NewHash();
		   scpname = Swig_symbol_qualifiedscopename(0);
		   Setattr(classes,scpname,(yyval.node));

		   appendChild((yyval.node),(yyvsp[(7) - (9)].node));
		   
		   if (am) append_previous_extension((yyval.node),am);

		   p = (yyvsp[(9) - (9)].node);
		   if (p) {
		     set_nextSibling((yyval.node),p);
		   }
		   
		   if (cparse_cplusplus && !cparse_externc) {
		     ty = NewString((yyvsp[(3) - (9)].str));
		   } else {
		     ty = NewStringf("%s %s", (yyvsp[(2) - (9)].id),(yyvsp[(3) - (9)].str));
		   }
		   while (p) {
		     Setattr(p,"storage",(yyvsp[(1) - (9)].id));
		     Setattr(p,"type",ty);
		     p = nextSibling(p);
		   }
		   /* Class typedefs */
		   {
		     String *name = (yyvsp[(3) - (9)].str);
		     if ((yyvsp[(9) - (9)].node)) {
		       SwigType *decltype = Getattr((yyvsp[(9) - (9)].node),"decl");
		       if (Cmp((yyvsp[(1) - (9)].id),"typedef") == 0) {
			 if (!decltype || !Len(decltype)) {
			   String *cname;
			   String *tdscopename;
			   String *class_scope = Swig_symbol_qualifiedscopename(cscope);
			   name = Getattr((yyvsp[(9) - (9)].node),"name");
			   cname = Copy(name);
			   Setattr((yyval.node),"tdname",cname);
			   tdscopename = class_scope ? NewStringf("%s::%s", class_scope, name) : Copy(name);

			   /* Use typedef name as class name */
			   if (class_rename && (Strcmp(class_rename,(yyvsp[(3) - (9)].str)) == 0)) {
			     Delete(class_rename);
			     class_rename = NewString(name);
			   }
			   if (!classes_typedefs) classes_typedefs = NewHash();
			   if (!Equal(scpname, tdscopename) && !Getattr(classes_typedefs, tdscopename)) {
			     Setattr(classes_typedefs, tdscopename, (yyval.node));
			   }
			   Setattr((yyval.node),"decl",decltype);
			   Delete(class_scope);
			   Delete(cname);
			   Delete(tdscopename);
			 }
		       }
		     }
		     appendChild((yyval.node),dump_nested(Char(name)));
		   }
		   Delete(scpname);

		   if (cplus_mode != CPLUS_PUBLIC) {
		   /* we 'open' the class at the end, to allow %template
		      to add new members */
		     Node *pa = new_node("access");
		     Setattr(pa,"kind","public");
		     cplus_mode = CPLUS_PUBLIC;
		     appendChild((yyval.node),pa);
		     Delete(pa);
		   }

		   Setattr((yyval.node),"symtab",Swig_symbol_popscope());

		   Classprefix = 0;
		   if (nscope_inner) {
		     /* this is tricky */
		     /* we add the declaration in the original namespace */
		     appendChild(nscope_inner,(yyval.node));
		     Swig_symbol_setscope(Getattr(nscope_inner,"symtab"));
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		     add_symbols((yyval.node));
		     if (nscope) (yyval.node) = nscope;
		     /* but the variable definition in the current scope */
		     Swig_symbol_setscope(cscope);
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		     add_symbols((yyvsp[(9) - (9)].node));
		   } else {
		     Delete(yyrename);
		     yyrename = Copy(class_rename);
		     Delete(Namespaceprefix);
		     Namespaceprefix = Swig_symbol_qualifiedscopename(0);

		     add_symbols((yyval.node));
		     add_symbols((yyvsp[(9) - (9)].node));
		   }
		   Swig_symbol_setscope(cscope);
		   Delete(Namespaceprefix);
		   Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		 } else {
		    (yyval.node) = new_node("class");
		    Setattr((yyval.node),"kind",(yyvsp[(2) - (9)].id));
		    Setattr((yyval.node),"name",NewString((yyvsp[(3) - (9)].str)));
		    SetFlag((yyval.node),"nestedtemplateclass");
		 }
	       }
    break;

  case 162:

/* Line 1806 of yacc.c  */
#line 3937 "parser.y"
    {
	       String *unnamed;
	       unnamed = make_unnamed();
	       (yyval.node) = new_node("class");
	       Setline((yyval.node),cparse_start_line);
	       Setattr((yyval.node),"kind",(yyvsp[(2) - (3)].id));
	       Setattr((yyval.node),"storage",(yyvsp[(1) - (3)].id));
	       Setattr((yyval.node),"unnamed",unnamed);
	       Setattr((yyval.node),"allows_typedef","1");
	       Delete(class_rename);
	       class_rename = make_name((yyval.node),0,0);
	       if (strcmp((yyvsp[(2) - (3)].id),"class") == 0) {
		 cplus_mode = CPLUS_PRIVATE;
	       } else {
		 cplus_mode = CPLUS_PUBLIC;
	       }
	       Swig_symbol_newscope();
	       cparse_start_line = cparse_line;
	       if (class_level >= max_class_levels) {
		   if (!max_class_levels) {
		       max_class_levels = 16;
		   } else {
		       max_class_levels *= 2;
		   }
		   class_decl = (Node**) realloc(class_decl, sizeof(Node*) * max_class_levels);
		   if (!class_decl) {
		       Swig_error(cparse_file, cparse_line, "realloc() failed\n");
		   }
	       }
	       class_decl[class_level++] = (yyval.node);
	       inclass = 1;
	       Classprefix = NewStringEmpty();
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             }
    break;

  case 163:

/* Line 1806 of yacc.c  */
#line 3971 "parser.y"
    {
	       String *unnamed;
	       Node *n;
	       (void) (yyvsp[(4) - (9)].node);
	       Classprefix = 0;
	       (yyval.node) = class_decl[--class_level];
	       inclass = 0;
	       unnamed = Getattr((yyval.node),"unnamed");

	       /* Check for pure-abstract class */
	       Setattr((yyval.node),"abstracts", pure_abstracts((yyvsp[(5) - (9)].node)));

	       n = new_node("cdecl");
	       Setattr(n,"name",(yyvsp[(7) - (9)].decl).id);
	       Setattr(n,"unnamed",unnamed);
	       Setattr(n,"type",unnamed);
	       Setattr(n,"decl",(yyvsp[(7) - (9)].decl).type);
	       Setattr(n,"parms",(yyvsp[(7) - (9)].decl).parms);
	       Setattr(n,"storage",(yyvsp[(1) - (9)].id));
	       if ((yyvsp[(9) - (9)].node)) {
		 Node *p = (yyvsp[(9) - (9)].node);
		 set_nextSibling(n,p);
		 while (p) {
		   String *type = Copy(unnamed);
		   Setattr(p,"name",(yyvsp[(7) - (9)].decl).id);
		   Setattr(p,"unnamed",unnamed);
		   Setattr(p,"type",type);
		   Delete(type);
		   Setattr(p,"storage",(yyvsp[(1) - (9)].id));
		   p = nextSibling(p);
		 }
	       }
	       set_nextSibling((yyval.node),n);
	       Delete(n);
	       {
		 /* If a proper typedef name was given, we'll use it to set the scope name */
		 String *name = 0;
		 if ((yyvsp[(1) - (9)].id) && (strcmp((yyvsp[(1) - (9)].id),"typedef") == 0)) {
		   if (!Len((yyvsp[(7) - (9)].decl).type)) {	
		     String *scpname = 0;
		     name = (yyvsp[(7) - (9)].decl).id;
		     Setattr((yyval.node),"tdname",name);
		     Setattr((yyval.node),"name",name);
		     Swig_symbol_setscopename(name);

		     /* If a proper name was given, we use that as the typedef, not unnamed */
		     Clear(unnamed);
		     Append(unnamed, name);
		     
		     n = nextSibling(n);
		     set_nextSibling((yyval.node),n);

		     /* Check for previous extensions */
		     if (extendhash) {
		       String *clsname = Swig_symbol_qualifiedscopename(0);
		       Node *am = Getattr(extendhash,clsname);
		       if (am) {
			 /* Merge the extension into the symbol table */
			 merge_extensions((yyval.node),am);
			 append_previous_extension((yyval.node),am);
			 Delattr(extendhash,clsname);
		       }
		       Delete(clsname);
		     }
		     if (!classes) classes = NewHash();
		     scpname = Swig_symbol_qualifiedscopename(0);
		     Setattr(classes,scpname,(yyval.node));
		     Delete(scpname);
		   } else {
		     Swig_symbol_setscopename("<unnamed>");
		   }
		 }
		 appendChild((yyval.node),(yyvsp[(5) - (9)].node));
		 appendChild((yyval.node),dump_nested(Char(name)));
	       }
	       /* Pop the scope */
	       Setattr((yyval.node),"symtab",Swig_symbol_popscope());
	       if (class_rename) {
		 Delete(yyrename);
		 yyrename = NewString(class_rename);
	       }
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols((yyval.node));
	       add_symbols(n);
	       Delete(unnamed);
              }
    break;

  case 164:

/* Line 1806 of yacc.c  */
#line 4060 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 165:

/* Line 1806 of yacc.c  */
#line 4061 "parser.y"
    {
                        (yyval.node) = new_node("cdecl");
                        Setattr((yyval.node),"name",(yyvsp[(1) - (3)].decl).id);
                        Setattr((yyval.node),"decl",(yyvsp[(1) - (3)].decl).type);
                        Setattr((yyval.node),"parms",(yyvsp[(1) - (3)].decl).parms);
			set_nextSibling((yyval.node),(yyvsp[(3) - (3)].node));
                    }
    break;

  case 166:

/* Line 1806 of yacc.c  */
#line 4073 "parser.y"
    {
              if ((yyvsp[(1) - (4)].id) && (Strcmp((yyvsp[(1) - (4)].id),"friend") == 0)) {
		/* Ignore */
                (yyval.node) = 0; 
	      } else {
		(yyval.node) = new_node("classforward");
		Setattr((yyval.node),"kind",(yyvsp[(2) - (4)].id));
		Setattr((yyval.node),"name",(yyvsp[(3) - (4)].str));
		Setattr((yyval.node),"sym:weak", "1");
		add_symbols((yyval.node));
	      }
             }
    break;

  case 167:

/* Line 1806 of yacc.c  */
#line 4091 "parser.y"
    { 
		    template_parameters = (yyvsp[(3) - (4)].tparms); 
		    if (inclass)
		      nested_template++;

		  }
    break;

  case 168:

/* Line 1806 of yacc.c  */
#line 4096 "parser.y"
    {

		    /* Don't ignore templated functions declared within a class, unless the templated function is within a nested class */
		    if (nested_template <= 1) {
		      int is_nested_template_class = (yyvsp[(6) - (6)].node) && GetFlag((yyvsp[(6) - (6)].node), "nestedtemplateclass");
		      if (is_nested_template_class) {
			(yyval.node) = 0;
			/* Nested template classes would probably better be ignored like ordinary nested classes using cpp_nested, but that introduces shift/reduce conflicts */
			if (cplus_mode == CPLUS_PUBLIC) {
			  /* Treat the nested class/struct/union as a forward declaration until a proper nested class solution is implemented */
			  String *kind = Getattr((yyvsp[(6) - (6)].node), "kind");
			  String *name = Getattr((yyvsp[(6) - (6)].node), "name");
			  (yyval.node) = new_node("template");
			  Setattr((yyval.node),"kind",kind);
			  Setattr((yyval.node),"name",name);
			  Setattr((yyval.node),"sym:weak", "1");
			  Setattr((yyval.node),"templatetype","classforward");
			  Setattr((yyval.node),"templateparms", (yyvsp[(3) - (6)].tparms));
			  add_symbols((yyval.node));

			  if (GetFlag((yyval.node), "feature:nestedworkaround")) {
			    Swig_symbol_remove((yyval.node));
			    (yyval.node) = 0;
			  } else {
			    SWIG_WARN_NODE_BEGIN((yyval.node));
			    Swig_warning(WARN_PARSE_NAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested template %s not currently supported (%s ignored).\n", kind, name);
			    SWIG_WARN_NODE_END((yyval.node));
			  }
			}
			Delete((yyvsp[(6) - (6)].node));
		      } else {
			String *tname = 0;
			int     error = 0;

			/* check if we get a namespace node with a class declaration, and retrieve the class */
			Symtab *cscope = Swig_symbol_current();
			Symtab *sti = 0;
			Node *ntop = (yyvsp[(6) - (6)].node);
			Node *ni = ntop;
			SwigType *ntype = ni ? nodeType(ni) : 0;
			while (ni && Strcmp(ntype,"namespace") == 0) {
			  sti = Getattr(ni,"symtab");
			  ni = firstChild(ni);
			  ntype = nodeType(ni);
			}
			if (sti) {
			  Swig_symbol_setscope(sti);
			  Delete(Namespaceprefix);
			  Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			  (yyvsp[(6) - (6)].node) = ni;
			}

			(yyval.node) = (yyvsp[(6) - (6)].node);
			if ((yyval.node)) tname = Getattr((yyval.node),"name");
			
			/* Check if the class is a template specialization */
			if (((yyval.node)) && (Strchr(tname,'<')) && (!is_operator(tname))) {
			  /* If a specialization.  Check if defined. */
			  Node *tempn = 0;
			  {
			    String *tbase = SwigType_templateprefix(tname);
			    tempn = Swig_symbol_clookup_local(tbase,0);
			    if (!tempn || (Strcmp(nodeType(tempn),"template") != 0)) {
			      SWIG_WARN_NODE_BEGIN(tempn);
			      Swig_warning(WARN_PARSE_TEMPLATE_SP_UNDEF, Getfile((yyval.node)),Getline((yyval.node)),"Specialization of non-template '%s'.\n", tbase);
			      SWIG_WARN_NODE_END(tempn);
			      tempn = 0;
			      error = 1;
			    }
			    Delete(tbase);
			  }
			  Setattr((yyval.node),"specialization","1");
			  Setattr((yyval.node),"templatetype",nodeType((yyval.node)));
			  set_nodeType((yyval.node),"template");
			  /* Template partial specialization */
			  if (tempn && ((yyvsp[(3) - (6)].tparms)) && ((yyvsp[(6) - (6)].node))) {
			    List   *tlist;
			    String *targs = SwigType_templateargs(tname);
			    tlist = SwigType_parmlist(targs);
			    /*			  Printf(stdout,"targs = '%s' %s\n", targs, tlist); */
			    if (!Getattr((yyval.node),"sym:weak")) {
			      Setattr((yyval.node),"sym:typename","1");
			    }
			    
			    if (Len(tlist) != ParmList_len(Getattr(tempn,"templateparms"))) {
			      Swig_error(Getfile((yyval.node)),Getline((yyval.node)),"Inconsistent argument count in template partial specialization. %d %d\n", Len(tlist), ParmList_len(Getattr(tempn,"templateparms")));
			      
			    } else {

			    /* This code builds the argument list for the partial template
			       specialization.  This is a little hairy, but the idea is as
			       follows:

			       $3 contains a list of arguments supplied for the template.
			       For example template<class T>.

			       tlist is a list of the specialization arguments--which may be
			       different.  For example class<int,T>.

			       tp is a copy of the arguments in the original template definition.
       
			       The patching algorithm walks through the list of supplied
			       arguments ($3), finds the position in the specialization arguments
			       (tlist), and then patches the name in the argument list of the
			       original template.
			    */

			    {
			      String *pn;
			      Parm *p, *p1;
			      int i, nargs;
			      Parm *tp = CopyParmList(Getattr(tempn,"templateparms"));
			      nargs = Len(tlist);
			      p = (yyvsp[(3) - (6)].tparms);
			      while (p) {
				for (i = 0; i < nargs; i++){
				  pn = Getattr(p,"name");
				  if (Strcmp(pn,SwigType_base(Getitem(tlist,i))) == 0) {
				    int j;
				    Parm *p1 = tp;
				    for (j = 0; j < i; j++) {
				      p1 = nextSibling(p1);
				    }
				    Setattr(p1,"name",pn);
				    Setattr(p1,"partialarg","1");
				  }
				}
				p = nextSibling(p);
			      }
			      p1 = tp;
			      i = 0;
			      while (p1) {
				if (!Getattr(p1,"partialarg")) {
				  Delattr(p1,"name");
				  Setattr(p1,"type", Getitem(tlist,i));
				} 
				i++;
				p1 = nextSibling(p1);
			      }
			      Setattr((yyval.node),"templateparms",tp);
			      Delete(tp);
			    }
  #if 0
			    /* Patch the parameter list */
			    if (tempn) {
			      Parm *p,*p1;
			      ParmList *tp = CopyParmList(Getattr(tempn,"templateparms"));
			      p = (yyvsp[(3) - (6)].tparms);
			      p1 = tp;
			      while (p && p1) {
				String *pn = Getattr(p,"name");
				Printf(stdout,"pn = '%s'\n", pn);
				if (pn) Setattr(p1,"name",pn);
				else Delattr(p1,"name");
				pn = Getattr(p,"type");
				if (pn) Setattr(p1,"type",pn);
				p = nextSibling(p);
				p1 = nextSibling(p1);
			      }
			      Setattr((yyval.node),"templateparms",tp);
			      Delete(tp);
			    } else {
			      Setattr((yyval.node),"templateparms",(yyvsp[(3) - (6)].tparms));
			    }
  #endif
			    Delattr((yyval.node),"specialization");
			    Setattr((yyval.node),"partialspecialization","1");
			    /* Create a specialized name for matching */
			    {
			      Parm *p = (yyvsp[(3) - (6)].tparms);
			      String *fname = NewString(Getattr((yyval.node),"name"));
			      String *ffname = 0;
			      ParmList *partialparms = 0;

			      char   tmp[32];
			      int    i, ilen;
			      while (p) {
				String *n = Getattr(p,"name");
				if (!n) {
				  p = nextSibling(p);
				  continue;
				}
				ilen = Len(tlist);
				for (i = 0; i < ilen; i++) {
				  if (Strstr(Getitem(tlist,i),n)) {
				    sprintf(tmp,"$%d",i+1);
				    Replaceid(fname,n,tmp);
				  }
				}
				p = nextSibling(p);
			      }
			      /* Patch argument names with typedef */
			      {
				Iterator tt;
				Parm *parm_current = 0;
				List *tparms = SwigType_parmlist(fname);
				ffname = SwigType_templateprefix(fname);
				Append(ffname,"<(");
				for (tt = First(tparms); tt.item; ) {
				  SwigType *rtt = Swig_symbol_typedef_reduce(tt.item,0);
				  SwigType *ttr = Swig_symbol_type_qualify(rtt,0);

				  Parm *newp = NewParmWithoutFileLineInfo(ttr, 0);
				  if (partialparms)
				    set_nextSibling(parm_current, newp);
				  else
				    partialparms = newp;
				  parm_current = newp;

				  Append(ffname,ttr);
				  tt = Next(tt);
				  if (tt.item) Putc(',',ffname);
				  Delete(rtt);
				  Delete(ttr);
				}
				Delete(tparms);
				Append(ffname,")>");
			      }
			      {
				Node *new_partial = NewHash();
				String *partials = Getattr(tempn,"partials");
				if (!partials) {
				  partials = NewList();
				  Setattr(tempn,"partials",partials);
				  Delete(partials);
				}
				/*			      Printf(stdout,"partial: fname = '%s', '%s'\n", fname, Swig_symbol_typedef_reduce(fname,0)); */
				Setattr(new_partial, "partialparms", partialparms);
				Setattr(new_partial, "templcsymname", ffname);
				Append(partials, new_partial);
			      }
			      Setattr((yyval.node),"partialargs",ffname);
			      Swig_symbol_cadd(ffname,(yyval.node));
			    }
			    }
			    Delete(tlist);
			    Delete(targs);
			  } else {
			    /* An explicit template specialization */
			    /* add default args from primary (unspecialized) template */
			    String *ty = Swig_symbol_template_deftype(tname,0);
			    String *fname = Swig_symbol_type_qualify(ty,0);
			    Swig_symbol_cadd(fname,(yyval.node));
			    Delete(ty);
			    Delete(fname);
			  }
			}  else if ((yyval.node)) {
			  Setattr((yyval.node),"templatetype",nodeType((yyvsp[(6) - (6)].node)));
			  set_nodeType((yyval.node),"template");
			  Setattr((yyval.node),"templateparms", (yyvsp[(3) - (6)].tparms));
			  if (!Getattr((yyval.node),"sym:weak")) {
			    Setattr((yyval.node),"sym:typename","1");
			  }
			  add_symbols((yyval.node));
			  default_arguments((yyval.node));
			  /* We also place a fully parameterized version in the symbol table */
			  {
			    Parm *p;
			    String *fname = NewStringf("%s<(", Getattr((yyval.node),"name"));
			    p = (yyvsp[(3) - (6)].tparms);
			    while (p) {
			      String *n = Getattr(p,"name");
			      if (!n) n = Getattr(p,"type");
			      Append(fname,n);
			      p = nextSibling(p);
			      if (p) Putc(',',fname);
			    }
			    Append(fname,")>");
			    Swig_symbol_cadd(fname,(yyval.node));
			  }
			}
			(yyval.node) = ntop;
			Swig_symbol_setscope(cscope);
			Delete(Namespaceprefix);
			Namespaceprefix = Swig_symbol_qualifiedscopename(0);
			if (error) (yyval.node) = 0;
		      }
		    } else {
		      (yyval.node) = 0;
		    }
		    template_parameters = 0;
		    if (inclass)
		      nested_template--;
                }
    break;

  case 169:

/* Line 1806 of yacc.c  */
#line 4382 "parser.y"
    {
		  Swig_warning(WARN_PARSE_EXPLICIT_TEMPLATE, cparse_file, cparse_line, "Explicit template instantiation ignored.\n");
                  (yyval.node) = 0; 
		}
    break;

  case 170:

/* Line 1806 of yacc.c  */
#line 4388 "parser.y"
    {
		  Swig_warning(WARN_PARSE_EXPLICIT_TEMPLATE, cparse_file, cparse_line, "Explicit template instantiation ignored.\n");
                  (yyval.node) = 0; 
                }
    break;

  case 171:

/* Line 1806 of yacc.c  */
#line 4394 "parser.y"
    {
		  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 172:

/* Line 1806 of yacc.c  */
#line 4397 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 173:

/* Line 1806 of yacc.c  */
#line 4400 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 174:

/* Line 1806 of yacc.c  */
#line 4403 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 175:

/* Line 1806 of yacc.c  */
#line 4406 "parser.y"
    {
		  (yyval.node) = 0;
                }
    break;

  case 176:

/* Line 1806 of yacc.c  */
#line 4409 "parser.y"
    {
                  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 177:

/* Line 1806 of yacc.c  */
#line 4412 "parser.y"
    {
                  (yyval.node) = (yyvsp[(1) - (1)].node);
                }
    break;

  case 178:

/* Line 1806 of yacc.c  */
#line 4417 "parser.y"
    {
		   /* Rip out the parameter names */
		  Parm *p = (yyvsp[(1) - (1)].pl);
		  (yyval.tparms) = (yyvsp[(1) - (1)].pl);

		  while (p) {
		    String *name = Getattr(p,"name");
		    if (!name) {
		      /* Hmmm. Maybe it's a 'class T' parameter */
		      char *type = Char(Getattr(p,"type"));
		      /* Template template parameter */
		      if (strncmp(type,"template<class> ",16) == 0) {
			type += 16;
		      }
		      if ((strncmp(type,"class ",6) == 0) || (strncmp(type,"typename ", 9) == 0)) {
			char *t = strchr(type,' ');
			Setattr(p,"name", t+1);
		      } else 
                      /* Variadic template args */
		      if ((strncmp(type,"class... ",9) == 0) || (strncmp(type,"typename... ", 12) == 0)) {
			char *t = strchr(type,' ');
			Setattr(p,"name", t+1);
			Setattr(p,"variadic", "1");
		      } else {
			/*
			 Swig_error(cparse_file, cparse_line, "Missing template parameter name\n");
			 $$.rparms = 0;
			 $$.parms = 0;
			 break; */
		      }
		    }
		    p = nextSibling(p);
		  }
                 }
    break;

  case 179:

/* Line 1806 of yacc.c  */
#line 4453 "parser.y"
    {
                      set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].pl));
                      (yyval.pl) = (yyvsp[(1) - (2)].p);
                   }
    break;

  case 180:

/* Line 1806 of yacc.c  */
#line 4457 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 181:

/* Line 1806 of yacc.c  */
#line 4460 "parser.y"
    {
		    (yyval.p) = NewParmWithoutFileLineInfo(NewString((yyvsp[(1) - (1)].id)), 0);
                  }
    break;

  case 182:

/* Line 1806 of yacc.c  */
#line 4463 "parser.y"
    {
                    (yyval.p) = (yyvsp[(1) - (1)].p);
                  }
    break;

  case 183:

/* Line 1806 of yacc.c  */
#line 4468 "parser.y"
    {
                         set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].pl));
                         (yyval.pl) = (yyvsp[(2) - (3)].p);
                       }
    break;

  case 184:

/* Line 1806 of yacc.c  */
#line 4472 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 185:

/* Line 1806 of yacc.c  */
#line 4477 "parser.y"
    {
                  String *uname = Swig_symbol_type_qualify((yyvsp[(2) - (3)].str),0);
		  String *name = Swig_scopename_last((yyvsp[(2) - (3)].str));
                  (yyval.node) = new_node("using");
		  Setattr((yyval.node),"uname",uname);
		  Setattr((yyval.node),"name", name);
		  Delete(uname);
		  Delete(name);
		  add_symbols((yyval.node));
             }
    break;

  case 186:

/* Line 1806 of yacc.c  */
#line 4487 "parser.y"
    {
	       Node *n = Swig_symbol_clookup((yyvsp[(3) - (4)].str),0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Nothing known about namespace '%s'\n", (yyvsp[(3) - (4)].str));
		 (yyval.node) = 0;
	       } else {

		 while (Strcmp(nodeType(n),"using") == 0) {
		   n = Getattr(n,"node");
		 }
		 if (n) {
		   if (Strcmp(nodeType(n),"namespace") == 0) {
		     Symtab *current = Swig_symbol_current();
		     Symtab *symtab = Getattr(n,"symtab");
		     (yyval.node) = new_node("using");
		     Setattr((yyval.node),"node",n);
		     Setattr((yyval.node),"namespace", (yyvsp[(3) - (4)].str));
		     if (current != symtab) {
		       Swig_symbol_inherit(symtab);
		     }
		   } else {
		     Swig_error(cparse_file, cparse_line, "'%s' is not a namespace.\n", (yyvsp[(3) - (4)].str));
		     (yyval.node) = 0;
		   }
		 } else {
		   (yyval.node) = 0;
		 }
	       }
             }
    break;

  case 187:

/* Line 1806 of yacc.c  */
#line 4518 "parser.y"
    { 
                Hash *h;
                (yyvsp[(1) - (3)].node) = Swig_symbol_current();
		h = Swig_symbol_clookup((yyvsp[(2) - (3)].str),0);
		if (h && ((yyvsp[(1) - (3)].node) == Getattr(h,"sym:symtab")) && (Strcmp(nodeType(h),"namespace") == 0)) {
		  if (Getattr(h,"alias")) {
		    h = Getattr(h,"namespace");
		    Swig_warning(WARN_PARSE_NAMESPACE_ALIAS, cparse_file, cparse_line, "Namespace alias '%s' not allowed here. Assuming '%s'\n",
				 (yyvsp[(2) - (3)].str), Getattr(h,"name"));
		    (yyvsp[(2) - (3)].str) = Getattr(h,"name");
		  }
		  Swig_symbol_setscope(Getattr(h,"symtab"));
		} else {
		  Swig_symbol_newscope();
		  Swig_symbol_setscopename((yyvsp[(2) - (3)].str));
		}
		Delete(Namespaceprefix);
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
             }
    break;

  case 188:

/* Line 1806 of yacc.c  */
#line 4536 "parser.y"
    {
                Node *n = (yyvsp[(5) - (6)].node);
		set_nodeType(n,"namespace");
		Setattr(n,"name",(yyvsp[(2) - (6)].str));
                Setattr(n,"symtab", Swig_symbol_popscope());
		Swig_symbol_setscope((yyvsp[(1) - (6)].node));
		(yyval.node) = n;
		Delete(Namespaceprefix);
		Namespaceprefix = Swig_symbol_qualifiedscopename(0);
		add_symbols((yyval.node));
             }
    break;

  case 189:

/* Line 1806 of yacc.c  */
#line 4547 "parser.y"
    {
	       Hash *h;
	       (yyvsp[(1) - (2)].node) = Swig_symbol_current();
	       h = Swig_symbol_clookup((char *)"    ",0);
	       if (h && (Strcmp(nodeType(h),"namespace") == 0)) {
		 Swig_symbol_setscope(Getattr(h,"symtab"));
	       } else {
		 Swig_symbol_newscope();
		 /* we don't use "__unnamed__", but a long 'empty' name */
		 Swig_symbol_setscopename("    ");
	       }
	       Namespaceprefix = 0;
             }
    break;

  case 190:

/* Line 1806 of yacc.c  */
#line 4559 "parser.y"
    {
	       (yyval.node) = (yyvsp[(4) - (5)].node);
	       set_nodeType((yyval.node),"namespace");
	       Setattr((yyval.node),"unnamed","1");
	       Setattr((yyval.node),"symtab", Swig_symbol_popscope());
	       Swig_symbol_setscope((yyvsp[(1) - (5)].node));
	       Delete(Namespaceprefix);
	       Namespaceprefix = Swig_symbol_qualifiedscopename(0);
	       add_symbols((yyval.node));
             }
    break;

  case 191:

/* Line 1806 of yacc.c  */
#line 4569 "parser.y"
    {
	       /* Namespace alias */
	       Node *n;
	       (yyval.node) = new_node("namespace");
	       Setattr((yyval.node),"name",(yyvsp[(2) - (5)].id));
	       Setattr((yyval.node),"alias",(yyvsp[(4) - (5)].str));
	       n = Swig_symbol_clookup((yyvsp[(4) - (5)].str),0);
	       if (!n) {
		 Swig_error(cparse_file, cparse_line, "Unknown namespace '%s'\n", (yyvsp[(4) - (5)].str));
		 (yyval.node) = 0;
	       } else {
		 if (Strcmp(nodeType(n),"namespace") != 0) {
		   Swig_error(cparse_file, cparse_line, "'%s' is not a namespace\n",(yyvsp[(4) - (5)].str));
		   (yyval.node) = 0;
		 } else {
		   while (Getattr(n,"alias")) {
		     n = Getattr(n,"namespace");
		   }
		   Setattr((yyval.node),"namespace",n);
		   add_symbols((yyval.node));
		   /* Set up a scope alias */
		   Swig_symbol_alias((yyvsp[(2) - (5)].id),Getattr(n,"symtab"));
		 }
	       }
             }
    break;

  case 192:

/* Line 1806 of yacc.c  */
#line 4596 "parser.y"
    {
                   (yyval.node) = (yyvsp[(1) - (2)].node);
                   /* Insert cpp_member (including any siblings) to the front of the cpp_members linked list */
		   if ((yyval.node)) {
		     Node *p = (yyval.node);
		     Node *pp =0;
		     while (p) {
		       pp = p;
		       p = nextSibling(p);
		     }
		     set_nextSibling(pp,(yyvsp[(2) - (2)].node));
		   } else {
		     (yyval.node) = (yyvsp[(2) - (2)].node);
		   }
             }
    break;

  case 193:

/* Line 1806 of yacc.c  */
#line 4611 "parser.y"
    { 
                  if (cplus_mode != CPLUS_PUBLIC) {
		     Swig_error(cparse_file,cparse_line,"%%extend can only be used in a public section\n");
		  }
             }
    break;

  case 194:

/* Line 1806 of yacc.c  */
#line 4615 "parser.y"
    {
	       (yyval.node) = new_node("extend");
	       tag_nodes((yyvsp[(4) - (6)].node),"feature:extend",(char*) "1");
	       appendChild((yyval.node),(yyvsp[(4) - (6)].node));
	       set_nextSibling((yyval.node),(yyvsp[(6) - (6)].node));
	     }
    break;

  case 195:

/* Line 1806 of yacc.c  */
#line 4621 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 196:

/* Line 1806 of yacc.c  */
#line 4622 "parser.y"
    { (yyval.node) = 0;}
    break;

  case 197:

/* Line 1806 of yacc.c  */
#line 4623 "parser.y"
    {
	       int start_line = cparse_line;
	       skip_decl();
	       Swig_error(cparse_file,start_line,"Syntax error in input(3).\n");
	       exit(1);
	       }
    break;

  case 198:

/* Line 1806 of yacc.c  */
#line 4628 "parser.y"
    { 
		 (yyval.node) = (yyvsp[(3) - (3)].node);
   	     }
    break;

  case 199:

/* Line 1806 of yacc.c  */
#line 4639 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 200:

/* Line 1806 of yacc.c  */
#line 4640 "parser.y"
    { 
                 (yyval.node) = (yyvsp[(1) - (1)].node); 
		 if (extendmode) {
		   String *symname;
		   symname= make_name((yyval.node),Getattr((yyval.node),"name"), Getattr((yyval.node),"decl"));
		   if (Strcmp(symname,Getattr((yyval.node),"name")) == 0) {
		     /* No renaming operation.  Set name to class name */
		     Delete(yyrename);
		     yyrename = NewString(Getattr(current_class,"sym:name"));
		   } else {
		     Delete(yyrename);
		     yyrename = symname;
		   }
		 }
		 add_symbols((yyval.node));
                 default_arguments((yyval.node));
             }
    break;

  case 201:

/* Line 1806 of yacc.c  */
#line 4657 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 202:

/* Line 1806 of yacc.c  */
#line 4658 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 203:

/* Line 1806 of yacc.c  */
#line 4659 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 204:

/* Line 1806 of yacc.c  */
#line 4660 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 205:

/* Line 1806 of yacc.c  */
#line 4661 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 206:

/* Line 1806 of yacc.c  */
#line 4662 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 207:

/* Line 1806 of yacc.c  */
#line 4663 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 208:

/* Line 1806 of yacc.c  */
#line 4664 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 209:

/* Line 1806 of yacc.c  */
#line 4665 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 210:

/* Line 1806 of yacc.c  */
#line 4666 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 211:

/* Line 1806 of yacc.c  */
#line 4667 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 212:

/* Line 1806 of yacc.c  */
#line 4668 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 213:

/* Line 1806 of yacc.c  */
#line 4669 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 214:

/* Line 1806 of yacc.c  */
#line 4670 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 215:

/* Line 1806 of yacc.c  */
#line 4671 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 216:

/* Line 1806 of yacc.c  */
#line 4672 "parser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 217:

/* Line 1806 of yacc.c  */
#line 4673 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 218:

/* Line 1806 of yacc.c  */
#line 4682 "parser.y"
    {
              if (Classprefix) {
		SwigType *decl = NewStringEmpty();
		(yyval.node) = new_node("constructor");
		Setattr((yyval.node),"storage",(yyvsp[(1) - (6)].id));
		Setattr((yyval.node),"name",(yyvsp[(2) - (6)].type));
		Setattr((yyval.node),"parms",(yyvsp[(4) - (6)].pl));
		SwigType_add_function(decl,(yyvsp[(4) - (6)].pl));
		Setattr((yyval.node),"decl",decl);
		Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].decl).throws);
		Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].decl).throwf);
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
		SetFlag((yyval.node),"feature:new");
	      } else {
		(yyval.node) = 0;
              }
              }
    break;

  case 219:

/* Line 1806 of yacc.c  */
#line 4707 "parser.y"
    {
               String *name = NewStringf("%s",(yyvsp[(2) - (6)].str));
	       if (*(Char(name)) != '~') Insert(name,0,"~");
               (yyval.node) = new_node("destructor");
	       Setattr((yyval.node),"name",name);
	       Delete(name);
	       if (Len(scanner_ccode)) {
		 String *code = Copy(scanner_ccode);
		 Setattr((yyval.node),"code",code);
		 Delete(code);
	       }
	       {
		 String *decl = NewStringEmpty();
		 SwigType_add_function(decl,(yyvsp[(4) - (6)].pl));
		 Setattr((yyval.node),"decl",decl);
		 Delete(decl);
	       }
	       Setattr((yyval.node),"throws",(yyvsp[(6) - (6)].dtype).throws);
	       Setattr((yyval.node),"throw",(yyvsp[(6) - (6)].dtype).throwf);
	       add_symbols((yyval.node));
	      }
    break;

  case 220:

/* Line 1806 of yacc.c  */
#line 4731 "parser.y"
    {
		String *name;
		(yyval.node) = new_node("destructor");
		Setattr((yyval.node),"storage","virtual");
	        name = NewStringf("%s",(yyvsp[(3) - (7)].str));
		if (*(Char(name)) != '~') Insert(name,0,"~");
		Setattr((yyval.node),"name",name);
		Delete(name);
		Setattr((yyval.node),"throws",(yyvsp[(7) - (7)].dtype).throws);
		Setattr((yyval.node),"throw",(yyvsp[(7) - (7)].dtype).throwf);
		if ((yyvsp[(7) - (7)].dtype).val) {
		  Setattr((yyval.node),"value","0");
		}
		if (Len(scanner_ccode)) {
		  String *code = Copy(scanner_ccode);
		  Setattr((yyval.node),"code",code);
		  Delete(code);
		}
		{
		  String *decl = NewStringEmpty();
		  SwigType_add_function(decl,(yyvsp[(5) - (7)].pl));
		  Setattr((yyval.node),"decl",decl);
		  Delete(decl);
		}

		add_symbols((yyval.node));
	      }
    break;

  case 221:

/* Line 1806 of yacc.c  */
#line 4762 "parser.y"
    {
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));

		 SwigType_add_function((yyvsp[(4) - (8)].type),(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push((yyvsp[(4) - (8)].type),(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",(yyvsp[(4) - (8)].type));
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
              }
    break;

  case 222:

/* Line 1806 of yacc.c  */
#line 4777 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));
		 decl = NewStringEmpty();
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 223:

/* Line 1806 of yacc.c  */
#line 4794 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (8)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (8)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (8)].id));
		 decl = NewStringEmpty();
		 SwigType_add_rvalue_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(6) - (8)].pl));
		 if ((yyvsp[(8) - (8)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(8) - (8)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(6) - (8)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 224:

/* Line 1806 of yacc.c  */
#line 4812 "parser.y"
    {
		 SwigType *decl;
                 (yyval.node) = new_node("cdecl");
                 Setattr((yyval.node),"type",(yyvsp[(3) - (9)].type));
		 Setattr((yyval.node),"name",(yyvsp[(2) - (9)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (9)].id));
		 decl = NewStringEmpty();
		 SwigType_add_pointer(decl);
		 SwigType_add_reference(decl);
		 SwigType_add_function(decl,(yyvsp[(7) - (9)].pl));
		 if ((yyvsp[(9) - (9)].dtype).qualifier) {
		   SwigType_push(decl,(yyvsp[(9) - (9)].dtype).qualifier);
		 }
		 Setattr((yyval.node),"decl",decl);
		 Setattr((yyval.node),"parms",(yyvsp[(7) - (9)].pl));
		 Setattr((yyval.node),"conversion_operator","1");
		 add_symbols((yyval.node));
	       }
    break;

  case 225:

/* Line 1806 of yacc.c  */
#line 4831 "parser.y"
    {
		String *t = NewStringEmpty();
		(yyval.node) = new_node("cdecl");
		Setattr((yyval.node),"type",(yyvsp[(3) - (7)].type));
		Setattr((yyval.node),"name",(yyvsp[(2) - (7)].str));
		 Setattr((yyval.node),"storage",(yyvsp[(1) - (7)].id));
		SwigType_add_function(t,(yyvsp[(5) - (7)].pl));
		if ((yyvsp[(7) - (7)].dtype).qualifier) {
		  SwigType_push(t,(yyvsp[(7) - (7)].dtype).qualifier);
		}
		Setattr((yyval.node),"decl",t);
		Setattr((yyval.node),"parms",(yyvsp[(5) - (7)].pl));
		Setattr((yyval.node),"conversion_operator","1");
		add_symbols((yyval.node));
              }
    break;

  case 226:

/* Line 1806 of yacc.c  */
#line 4850 "parser.y"
    {
                 skip_balanced('{','}');
                 (yyval.node) = 0;
               }
    break;

  case 227:

/* Line 1806 of yacc.c  */
#line 4857 "parser.y"
    {
                skip_balanced('(',')');
                (yyval.node) = 0;
              }
    break;

  case 228:

/* Line 1806 of yacc.c  */
#line 4864 "parser.y"
    { 
                (yyval.node) = new_node("access");
		Setattr((yyval.node),"kind","public");
                cplus_mode = CPLUS_PUBLIC;
              }
    break;

  case 229:

/* Line 1806 of yacc.c  */
#line 4871 "parser.y"
    { 
                (yyval.node) = new_node("access");
                Setattr((yyval.node),"kind","private");
		cplus_mode = CPLUS_PRIVATE;
	      }
    break;

  case 230:

/* Line 1806 of yacc.c  */
#line 4879 "parser.y"
    { 
		(yyval.node) = new_node("access");
		Setattr((yyval.node),"kind","protected");
		cplus_mode = CPLUS_PROTECTED;
	      }
    break;

  case 231:

/* Line 1806 of yacc.c  */
#line 4900 "parser.y"
    {
		cparse_start_line = cparse_line;
		skip_balanced('{','}');
		(yyval.str) = NewString(scanner_ccode); /* copied as initializers overwrite scanner_ccode */
	      }
    break;

  case 232:

/* Line 1806 of yacc.c  */
#line 4904 "parser.y"
    {
	        (yyval.node) = 0;
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (cparse_cplusplus) {
		    String *name = Copy((yyvsp[(3) - (7)].str));
		    (yyval.node) = nested_forward_declaration((yyvsp[(1) - (7)].id), (yyvsp[(2) - (7)].id), (yyvsp[(3) - (7)].str), name, (yyvsp[(7) - (7)].node));
		  } else if ((yyvsp[(7) - (7)].node)) {
		    nested_new_struct((yyvsp[(2) - (7)].id), (yyvsp[(6) - (7)].str), (yyvsp[(7) - (7)].node));
		  }
		}
		Delete((yyvsp[(6) - (7)].str));
	      }
    break;

  case 233:

/* Line 1806 of yacc.c  */
#line 4927 "parser.y"
    {
		cparse_start_line = cparse_line;
		skip_balanced('{','}');
		(yyval.str) = NewString(scanner_ccode); /* copied as initializers overwrite scanner_ccode */
	      }
    break;

  case 234:

/* Line 1806 of yacc.c  */
#line 4931 "parser.y"
    {
	        (yyval.node) = 0;
		if (cplus_mode == CPLUS_PUBLIC) {
		  if (cparse_cplusplus) {
		    String *name = (yyvsp[(6) - (6)].node) ? Copy(Getattr((yyvsp[(6) - (6)].node), "name")) : 0;
		    (yyval.node) = nested_forward_declaration((yyvsp[(1) - (6)].id), (yyvsp[(2) - (6)].id), 0, name, (yyvsp[(6) - (6)].node));
		  } else {
		    if ((yyvsp[(6) - (6)].node)) {
		      nested_new_struct((yyvsp[(2) - (6)].id), (yyvsp[(5) - (6)].str), (yyvsp[(6) - (6)].node));
		    } else {
		      Swig_warning(WARN_PARSE_UNNAMED_NESTED_CLASS, cparse_file, cparse_line, "Nested %s not currently supported (ignored).\n", (yyvsp[(2) - (6)].id));
		    }
		  }
		}
		Delete((yyvsp[(5) - (6)].str));
	      }
    break;

  case 235:

/* Line 1806 of yacc.c  */
#line 4963 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 236:

/* Line 1806 of yacc.c  */
#line 4966 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 237:

/* Line 1806 of yacc.c  */
#line 4970 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 238:

/* Line 1806 of yacc.c  */
#line 4973 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 239:

/* Line 1806 of yacc.c  */
#line 4974 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 240:

/* Line 1806 of yacc.c  */
#line 4975 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 241:

/* Line 1806 of yacc.c  */
#line 4976 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 242:

/* Line 1806 of yacc.c  */
#line 4977 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 243:

/* Line 1806 of yacc.c  */
#line 4978 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 244:

/* Line 1806 of yacc.c  */
#line 4979 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 245:

/* Line 1806 of yacc.c  */
#line 4980 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 246:

/* Line 1806 of yacc.c  */
#line 4983 "parser.y"
    {
	            Clear(scanner_ccode);
		    (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
               }
    break;

  case 247:

/* Line 1806 of yacc.c  */
#line 4988 "parser.y"
    { 
		    skip_balanced('{','}'); 
		    (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
		    (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
	       }
    break;

  case 248:

/* Line 1806 of yacc.c  */
#line 4995 "parser.y"
    { 
                     Clear(scanner_ccode);
                     (yyval.dtype).val = 0;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (2)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws;
                     (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf;
                }
    break;

  case 249:

/* Line 1806 of yacc.c  */
#line 5003 "parser.y"
    { 
                     Clear(scanner_ccode);
                     (yyval.dtype).val = (yyvsp[(3) - (4)].dtype).val;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (4)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (4)].dtype).throws; 
                     (yyval.dtype).throwf = (yyvsp[(1) - (4)].dtype).throwf; 
               }
    break;

  case 250:

/* Line 1806 of yacc.c  */
#line 5011 "parser.y"
    { 
                     skip_balanced('{','}');
                     (yyval.dtype).val = 0;
                     (yyval.dtype).qualifier = (yyvsp[(1) - (2)].dtype).qualifier;
                     (yyval.dtype).bitfield = 0;
                     (yyval.dtype).throws = (yyvsp[(1) - (2)].dtype).throws; 
                     (yyval.dtype).throwf = (yyvsp[(1) - (2)].dtype).throwf; 
               }
    break;

  case 251:

/* Line 1806 of yacc.c  */
#line 5022 "parser.y"
    { }
    break;

  case 252:

/* Line 1806 of yacc.c  */
#line 5025 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type);
                  /* Printf(stdout,"primitive = '%s'\n", $$);*/
                }
    break;

  case 253:

/* Line 1806 of yacc.c  */
#line 5028 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 254:

/* Line 1806 of yacc.c  */
#line 5029 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 255:

/* Line 1806 of yacc.c  */
#line 5030 "parser.y"
    { (yyval.type) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 256:

/* Line 1806 of yacc.c  */
#line 5031 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 257:

/* Line 1806 of yacc.c  */
#line 5033 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (1)].str);
               }
    break;

  case 258:

/* Line 1806 of yacc.c  */
#line 5042 "parser.y"
    { (yyval.id) = "extern"; }
    break;

  case 259:

/* Line 1806 of yacc.c  */
#line 5043 "parser.y"
    {
                   if (strcmp((yyvsp[(2) - (2)].id),"C") == 0) {
		     (yyval.id) = "externc";
                   } else if (strcmp((yyvsp[(2) - (2)].id),"C++") == 0) {
		     (yyval.id) = "extern";
		   } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (2)].id));
		     (yyval.id) = 0;
		   }
               }
    break;

  case 260:

/* Line 1806 of yacc.c  */
#line 5053 "parser.y"
    {
                   if (strcmp((yyvsp[(2) - (3)].id),"C") == 0) {
		     (yyval.id) = "externc thread_local";
                   } else if (strcmp((yyvsp[(2) - (3)].id),"C++") == 0) {
		     (yyval.id) = "extern thread_local";
		   } else {
		     Swig_warning(WARN_PARSE_UNDEFINED_EXTERN,cparse_file, cparse_line,"Unrecognized extern type \"%s\".\n", (yyvsp[(2) - (3)].id));
		     (yyval.id) = 0;
		   }
               }
    break;

  case 261:

/* Line 1806 of yacc.c  */
#line 5063 "parser.y"
    { (yyval.id) = "static"; }
    break;

  case 262:

/* Line 1806 of yacc.c  */
#line 5064 "parser.y"
    { (yyval.id) = "typedef"; }
    break;

  case 263:

/* Line 1806 of yacc.c  */
#line 5065 "parser.y"
    { (yyval.id) = "virtual"; }
    break;

  case 264:

/* Line 1806 of yacc.c  */
#line 5066 "parser.y"
    { (yyval.id) = "friend"; }
    break;

  case 265:

/* Line 1806 of yacc.c  */
#line 5067 "parser.y"
    { (yyval.id) = "explicit"; }
    break;

  case 266:

/* Line 1806 of yacc.c  */
#line 5068 "parser.y"
    { (yyval.id) = "constexpr"; }
    break;

  case 267:

/* Line 1806 of yacc.c  */
#line 5069 "parser.y"
    { (yyval.id) = "thread_local"; }
    break;

  case 268:

/* Line 1806 of yacc.c  */
#line 5070 "parser.y"
    { (yyval.id) = "static thread_local"; }
    break;

  case 269:

/* Line 1806 of yacc.c  */
#line 5071 "parser.y"
    { (yyval.id) = "static thread_local"; }
    break;

  case 270:

/* Line 1806 of yacc.c  */
#line 5072 "parser.y"
    { (yyval.id) = "extern thread_local"; }
    break;

  case 271:

/* Line 1806 of yacc.c  */
#line 5073 "parser.y"
    { (yyval.id) = "extern thread_local"; }
    break;

  case 272:

/* Line 1806 of yacc.c  */
#line 5074 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 273:

/* Line 1806 of yacc.c  */
#line 5081 "parser.y"
    {
                 Parm *p;
		 (yyval.pl) = (yyvsp[(1) - (1)].pl);
		 p = (yyvsp[(1) - (1)].pl);
                 while (p) {
		   Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   p = nextSibling(p);
                 }
               }
    break;

  case 274:

/* Line 1806 of yacc.c  */
#line 5092 "parser.y"
    {
                  set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].pl));
                  (yyval.pl) = (yyvsp[(1) - (2)].p);
		}
    break;

  case 275:

/* Line 1806 of yacc.c  */
#line 5096 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 276:

/* Line 1806 of yacc.c  */
#line 5099 "parser.y"
    {
                 set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].pl));
		 (yyval.pl) = (yyvsp[(2) - (3)].p);
                }
    break;

  case 277:

/* Line 1806 of yacc.c  */
#line 5103 "parser.y"
    { (yyval.pl) = 0; }
    break;

  case 278:

/* Line 1806 of yacc.c  */
#line 5107 "parser.y"
    {
                   SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		   (yyval.p) = NewParmWithoutFileLineInfo((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).id);
		   Setfile((yyval.p),cparse_file);
		   Setline((yyval.p),cparse_line);
		   if ((yyvsp[(2) - (2)].decl).defarg) {
		     Setattr((yyval.p),"value",(yyvsp[(2) - (2)].decl).defarg);
		   }
		}
    break;

  case 279:

/* Line 1806 of yacc.c  */
#line 5117 "parser.y"
    {
                  (yyval.p) = NewParmWithoutFileLineInfo(NewStringf("template<class> %s %s", (yyvsp[(5) - (7)].id),(yyvsp[(6) - (7)].str)), 0);
		  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
                  if ((yyvsp[(7) - (7)].dtype).val) {
                    Setattr((yyval.p),"value",(yyvsp[(7) - (7)].dtype).val);
                  }
                }
    break;

  case 280:

/* Line 1806 of yacc.c  */
#line 5125 "parser.y"
    {
		  SwigType *t = NewString("v(...)");
		  (yyval.p) = NewParmWithoutFileLineInfo(t, 0);
		  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
		}
    break;

  case 281:

/* Line 1806 of yacc.c  */
#line 5133 "parser.y"
    {
                 Parm *p;
		 (yyval.p) = (yyvsp[(1) - (1)].p);
		 p = (yyvsp[(1) - (1)].p);
                 while (p) {
		   if (Getattr(p,"type")) {
		     Replace(Getattr(p,"type"),"typename ", "", DOH_REPLACE_ANY);
		   }
		   p = nextSibling(p);
                 }
               }
    break;

  case 282:

/* Line 1806 of yacc.c  */
#line 5146 "parser.y"
    {
                  set_nextSibling((yyvsp[(1) - (2)].p),(yyvsp[(2) - (2)].p));
                  (yyval.p) = (yyvsp[(1) - (2)].p);
		}
    break;

  case 283:

/* Line 1806 of yacc.c  */
#line 5150 "parser.y"
    { (yyval.p) = 0; }
    break;

  case 284:

/* Line 1806 of yacc.c  */
#line 5153 "parser.y"
    {
                 set_nextSibling((yyvsp[(2) - (3)].p),(yyvsp[(3) - (3)].p));
		 (yyval.p) = (yyvsp[(2) - (3)].p);
                }
    break;

  case 285:

/* Line 1806 of yacc.c  */
#line 5157 "parser.y"
    { (yyval.p) = 0; }
    break;

  case 286:

/* Line 1806 of yacc.c  */
#line 5161 "parser.y"
    {
		  (yyval.p) = (yyvsp[(1) - (1)].p);
		  {
		    /* We need to make a possible adjustment for integer parameters. */
		    SwigType *type;
		    Node     *n = 0;

		    while (!n) {
		      type = Getattr((yyvsp[(1) - (1)].p),"type");
		      n = Swig_symbol_clookup(type,0);     /* See if we can find a node that matches the typename */
		      if ((n) && (Strcmp(nodeType(n),"cdecl") == 0)) {
			SwigType *decl = Getattr(n,"decl");
			if (!SwigType_isfunction(decl)) {
			  String *value = Getattr(n,"value");
			  if (value) {
			    String *v = Copy(value);
			    Setattr((yyvsp[(1) - (1)].p),"type",v);
			    Delete(v);
			    n = 0;
			  }
			}
		      } else {
			break;
		      }
		    }
		  }

               }
    break;

  case 287:

/* Line 1806 of yacc.c  */
#line 5189 "parser.y"
    {
                  (yyval.p) = NewParmWithoutFileLineInfo(0,0);
                  Setfile((yyval.p),cparse_file);
		  Setline((yyval.p),cparse_line);
		  Setattr((yyval.p),"value",(yyvsp[(1) - (1)].dtype).val);
               }
    break;

  case 288:

/* Line 1806 of yacc.c  */
#line 5197 "parser.y"
    { 
                  (yyval.dtype) = (yyvsp[(2) - (2)].dtype); 
		  if ((yyvsp[(2) - (2)].dtype).type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    (yyval.dtype).val = 0;
		    (yyval.dtype).rawval = 0;
		    (yyval.dtype).bitfield = 0;
		    (yyval.dtype).throws = 0;
		    (yyval.dtype).throwf = 0;
		  }
               }
    break;

  case 289:

/* Line 1806 of yacc.c  */
#line 5208 "parser.y"
    { 
		  (yyval.dtype) = (yyvsp[(2) - (5)].dtype);
		  if ((yyvsp[(2) - (5)].dtype).type == T_ERROR) {
		    Swig_warning(WARN_PARSE_BAD_DEFAULT,cparse_file, cparse_line, "Can't set default argument (ignored)\n");
		    (yyval.dtype) = (yyvsp[(2) - (5)].dtype);
		    (yyval.dtype).val = 0;
		    (yyval.dtype).rawval = 0;
		    (yyval.dtype).bitfield = 0;
		    (yyval.dtype).throws = 0;
		    (yyval.dtype).throwf = 0;
		  } else {
		    (yyval.dtype).val = NewStringf("%s[%s]",(yyvsp[(2) - (5)].dtype).val,(yyvsp[(4) - (5)].dtype).val); 
		  }		  
               }
    break;

  case 290:

/* Line 1806 of yacc.c  */
#line 5222 "parser.y"
    {
		 skip_balanced('{','}');
		 (yyval.dtype).val = NewString(scanner_ccode);
		 (yyval.dtype).rawval = 0;
                 (yyval.dtype).type = T_INT;
		 (yyval.dtype).bitfield = 0;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
	       }
    break;

  case 291:

/* Line 1806 of yacc.c  */
#line 5231 "parser.y"
    { 
		 (yyval.dtype).val = 0;
		 (yyval.dtype).rawval = 0;
		 (yyval.dtype).type = 0;
		 (yyval.dtype).bitfield = (yyvsp[(2) - (2)].dtype).val;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
	       }
    break;

  case 292:

/* Line 1806 of yacc.c  */
#line 5239 "parser.y"
    {
                 (yyval.dtype).val = 0;
                 (yyval.dtype).rawval = 0;
                 (yyval.dtype).type = T_INT;
		 (yyval.dtype).bitfield = 0;
		 (yyval.dtype).throws = 0;
		 (yyval.dtype).throwf = 0;
               }
    break;

  case 293:

/* Line 1806 of yacc.c  */
#line 5249 "parser.y"
    {
                 (yyval.decl) = (yyvsp[(1) - (2)].decl);
		 (yyval.decl).defarg = (yyvsp[(2) - (2)].dtype).rawval ? (yyvsp[(2) - (2)].dtype).rawval : (yyvsp[(2) - (2)].dtype).val;
            }
    break;

  case 294:

/* Line 1806 of yacc.c  */
#line 5253 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (2)].decl);
	      (yyval.decl).defarg = (yyvsp[(2) - (2)].dtype).rawval ? (yyvsp[(2) - (2)].dtype).rawval : (yyvsp[(2) - (2)].dtype).val;
            }
    break;

  case 295:

/* Line 1806 of yacc.c  */
#line 5257 "parser.y"
    {
   	      (yyval.decl).type = 0;
              (yyval.decl).id = 0;
	      (yyval.decl).defarg = (yyvsp[(1) - (1)].dtype).rawval ? (yyvsp[(1) - (1)].dtype).rawval : (yyvsp[(1) - (1)].dtype).val;
            }
    break;

  case 296:

/* Line 1806 of yacc.c  */
#line 5264 "parser.y"
    {
                 (yyval.decl) = (yyvsp[(1) - (1)].decl);
		 if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		   Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		 } else if (SwigType_isarray((yyvsp[(1) - (1)].decl).type)) {
		   SwigType *ta = SwigType_pop_arrays((yyvsp[(1) - (1)].decl).type);
		   if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		     Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		   } else {
		     (yyval.decl).parms = 0;
		   }
		   SwigType_push((yyvsp[(1) - (1)].decl).type,ta);
		   Delete(ta);
		 } else {
		   (yyval.decl).parms = 0;
		 }
            }
    break;

  case 297:

/* Line 1806 of yacc.c  */
#line 5281 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (1)].decl);
	      if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
	      } else if (SwigType_isarray((yyvsp[(1) - (1)].decl).type)) {
		SwigType *ta = SwigType_pop_arrays((yyvsp[(1) - (1)].decl).type);
		if (SwigType_isfunction((yyvsp[(1) - (1)].decl).type)) {
		  Delete(SwigType_pop_function((yyvsp[(1) - (1)].decl).type));
		} else {
		  (yyval.decl).parms = 0;
		}
		SwigType_push((yyvsp[(1) - (1)].decl).type,ta);
		Delete(ta);
	      } else {
		(yyval.decl).parms = 0;
	      }
            }
    break;

  case 298:

/* Line 1806 of yacc.c  */
#line 5298 "parser.y"
    {
   	      (yyval.decl).type = 0;
              (yyval.decl).id = 0;
	      (yyval.decl).parms = 0;
	      }
    break;

  case 299:

/* Line 1806 of yacc.c  */
#line 5306 "parser.y"
    {
              (yyval.decl) = (yyvsp[(2) - (2)].decl);
	      if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (2)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (2)].type);
           }
    break;

  case 300:

/* Line 1806 of yacc.c  */
#line 5314 "parser.y"
    {
              (yyval.decl) = (yyvsp[(3) - (3)].decl);
	      SwigType_add_reference((yyvsp[(1) - (3)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (3)].type);
           }
    break;

  case 301:

/* Line 1806 of yacc.c  */
#line 5323 "parser.y"
    {
              (yyval.decl) = (yyvsp[(3) - (3)].decl);
	      SwigType_add_rvalue_reference((yyvsp[(1) - (3)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (3)].type);
           }
    break;

  case 302:

/* Line 1806 of yacc.c  */
#line 5332 "parser.y"
    {
              (yyval.decl) = (yyvsp[(1) - (1)].decl);
	      if (!(yyval.decl).type) (yyval.decl).type = NewStringEmpty();
           }
    break;

  case 303:

/* Line 1806 of yacc.c  */
#line 5336 "parser.y"
    {
	     (yyval.decl) = (yyvsp[(2) - (2)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_reference((yyval.decl).type);
	     if ((yyvsp[(2) - (2)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
	       Delete((yyvsp[(2) - (2)].decl).type);
	     }
           }
    break;

  case 304:

/* Line 1806 of yacc.c  */
#line 5345 "parser.y"
    {
	     /* Introduced in C++11, move operator && */
             /* Adds one S/R conflict */
	     (yyval.decl) = (yyvsp[(2) - (2)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_rvalue_reference((yyval.decl).type);
	     if ((yyvsp[(2) - (2)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
	       Delete((yyvsp[(2) - (2)].decl).type);
	     }
           }
    break;

  case 305:

/* Line 1806 of yacc.c  */
#line 5356 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();

	     (yyval.decl) = (yyvsp[(3) - (3)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (3)].str));
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = t;
	     }
    break;

  case 306:

/* Line 1806 of yacc.c  */
#line 5367 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(4) - (4)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(2) - (4)].str));
	     SwigType_push((yyvsp[(1) - (4)].type),t);
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (4)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (4)].type);
	     Delete(t);
	   }
    break;

  case 307:

/* Line 1806 of yacc.c  */
#line 5379 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (5)].type),(yyvsp[(2) - (5)].str));
	     SwigType_add_reference((yyvsp[(1) - (5)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (5)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (5)].type);
	   }
    break;

  case 308:

/* Line 1806 of yacc.c  */
#line 5389 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(4) - (4)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (4)].str));
	     SwigType_add_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 309:

/* Line 1806 of yacc.c  */
#line 5403 "parser.y"
    {
              (yyval.decl) = (yyvsp[(5) - (5)].decl);
	      if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (5)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (5)].type);
           }
    break;

  case 310:

/* Line 1806 of yacc.c  */
#line 5411 "parser.y"
    {
              (yyval.decl) = (yyvsp[(6) - (6)].decl);
	      SwigType_add_reference((yyvsp[(1) - (6)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (6)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (6)].type);
           }
    break;

  case 311:

/* Line 1806 of yacc.c  */
#line 5420 "parser.y"
    {
              (yyval.decl) = (yyvsp[(6) - (6)].decl);
	      SwigType_add_rvalue_reference((yyvsp[(1) - (6)].type));
              if ((yyval.decl).type) {
		SwigType_push((yyvsp[(1) - (6)].type),(yyval.decl).type);
		Delete((yyval.decl).type);
	      }
	      (yyval.decl).type = (yyvsp[(1) - (6)].type);
           }
    break;

  case 312:

/* Line 1806 of yacc.c  */
#line 5429 "parser.y"
    {
              (yyval.decl) = (yyvsp[(4) - (4)].decl);
	      if (!(yyval.decl).type) (yyval.decl).type = NewStringEmpty();
           }
    break;

  case 313:

/* Line 1806 of yacc.c  */
#line 5433 "parser.y"
    {
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_reference((yyval.decl).type);
	     if ((yyvsp[(5) - (5)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(5) - (5)].decl).type);
	       Delete((yyvsp[(5) - (5)].decl).type);
	     }
           }
    break;

  case 314:

/* Line 1806 of yacc.c  */
#line 5442 "parser.y"
    {
	     /* Introduced in C++11, move operator && */
             /* Adds one S/R conflict */
	     (yyval.decl) = (yyvsp[(5) - (5)].decl);
	     (yyval.decl).type = NewStringEmpty();
	     SwigType_add_rvalue_reference((yyval.decl).type);
	     if ((yyvsp[(5) - (5)].decl).type) {
	       SwigType_push((yyval.decl).type,(yyvsp[(5) - (5)].decl).type);
	       Delete((yyvsp[(5) - (5)].decl).type);
	     }
           }
    break;

  case 315:

/* Line 1806 of yacc.c  */
#line 5453 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();

	     (yyval.decl) = (yyvsp[(6) - (6)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (6)].str));
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = t;
	     }
    break;

  case 316:

/* Line 1806 of yacc.c  */
#line 5464 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(7) - (7)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(2) - (7)].str));
	     SwigType_push((yyvsp[(1) - (7)].type),t);
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (7)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (7)].type);
	     Delete(t);
	   }
    break;

  case 317:

/* Line 1806 of yacc.c  */
#line 5476 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(8) - (8)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (8)].type),(yyvsp[(2) - (8)].str));
	     SwigType_add_reference((yyvsp[(1) - (8)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (8)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (8)].type);
	   }
    break;

  case 318:

/* Line 1806 of yacc.c  */
#line 5486 "parser.y"
    { 
	     (yyval.decl) = (yyvsp[(8) - (8)].decl);
	     SwigType_add_memberpointer((yyvsp[(1) - (8)].type),(yyvsp[(2) - (8)].str));
	     SwigType_add_rvalue_reference((yyvsp[(1) - (8)].type));
	     if ((yyval.decl).type) {
	       SwigType_push((yyvsp[(1) - (8)].type),(yyval.decl).type);
	       Delete((yyval.decl).type);
	     }
	     (yyval.decl).type = (yyvsp[(1) - (8)].type);
	   }
    break;

  case 319:

/* Line 1806 of yacc.c  */
#line 5496 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(7) - (7)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (7)].str));
	     SwigType_add_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 320:

/* Line 1806 of yacc.c  */
#line 5507 "parser.y"
    { 
	     SwigType *t = NewStringEmpty();
	     (yyval.decl) = (yyvsp[(7) - (7)].decl);
	     SwigType_add_memberpointer(t,(yyvsp[(1) - (7)].str));
	     SwigType_add_rvalue_reference(t);
	     if ((yyval.decl).type) {
	       SwigType_push(t,(yyval.decl).type);
	       Delete((yyval.decl).type);
	     } 
	     (yyval.decl).type = t;
	   }
    break;

  case 321:

/* Line 1806 of yacc.c  */
#line 5520 "parser.y"
    {
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 (yyval.decl).id = Char((yyvsp[(1) - (1)].str));
		 (yyval.decl).type = 0;
		 (yyval.decl).parms = 0;
		 (yyval.decl).have_parms = 0;
                  }
    break;

  case 322:

/* Line 1806 of yacc.c  */
#line 5527 "parser.y"
    {
                  (yyval.decl).id = Char(NewStringf("~%s",(yyvsp[(2) - (2)].str)));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 323:

/* Line 1806 of yacc.c  */
#line 5535 "parser.y"
    {
                  (yyval.decl).id = Char((yyvsp[(2) - (3)].str));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 324:

/* Line 1806 of yacc.c  */
#line 5551 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(2) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(2) - (4)].type);
                  }
    break;

  case 325:

/* Line 1806 of yacc.c  */
#line 5559 "parser.y"
    {
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(4) - (5)].decl);
		    t = NewStringEmpty();
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (5)].str));
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
		    }
    break;

  case 326:

/* Line 1806 of yacc.c  */
#line 5570 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(char*)"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 327:

/* Line 1806 of yacc.c  */
#line 5581 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 328:

/* Line 1806 of yacc.c  */
#line 5592 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		  }
    break;

  case 329:

/* Line 1806 of yacc.c  */
#line 5611 "parser.y"
    {
                /* Note: This is non-standard C.  Template declarator is allowed to follow an identifier */
                 (yyval.decl).id = Char((yyvsp[(1) - (1)].str));
		 (yyval.decl).type = 0;
		 (yyval.decl).parms = 0;
		 (yyval.decl).have_parms = 0;
                  }
    break;

  case 330:

/* Line 1806 of yacc.c  */
#line 5619 "parser.y"
    {
                  (yyval.decl).id = Char(NewStringf("~%s",(yyvsp[(2) - (2)].str)));
                  (yyval.decl).type = 0;
                  (yyval.decl).parms = 0;
                  (yyval.decl).have_parms = 0;
                  }
    break;

  case 331:

/* Line 1806 of yacc.c  */
#line 5636 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(2) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(2) - (4)].type);
                  }
    break;

  case 332:

/* Line 1806 of yacc.c  */
#line 5644 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = NewStringEmpty();
		    }
		    SwigType_add_reference((yyval.decl).type);
                  }
    break;

  case 333:

/* Line 1806 of yacc.c  */
#line 5651 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(3) - (4)].decl);
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = NewStringEmpty();
		    }
		    SwigType_add_rvalue_reference((yyval.decl).type);
                  }
    break;

  case 334:

/* Line 1806 of yacc.c  */
#line 5658 "parser.y"
    {
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(4) - (5)].decl);
		    t = NewStringEmpty();
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (5)].str));
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
		    }
    break;

  case 335:

/* Line 1806 of yacc.c  */
#line 5669 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(char*)"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 336:

/* Line 1806 of yacc.c  */
#line 5680 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 337:

/* Line 1806 of yacc.c  */
#line 5691 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
                 }
    break;

  case 338:

/* Line 1806 of yacc.c  */
#line 5711 "parser.y"
    {
		    SwigType *t;
                    Append((yyvsp[(1) - (5)].str), " "); /* intervening space is mandatory */
                    Append((yyvsp[(1) - (5)].str), Char((yyvsp[(2) - (5)].id)));
		    (yyval.decl).id = Char((yyvsp[(1) - (5)].str));
		    t = NewStringEmpty();
		    SwigType_add_function(t,(yyvsp[(4) - (5)].pl));
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(4) - (5)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t, (yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		  }
    break;

  case 339:

/* Line 1806 of yacc.c  */
#line 5732 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (1)].type);
                    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                  }
    break;

  case 340:

/* Line 1806 of yacc.c  */
#line 5738 "parser.y"
    { 
                     (yyval.decl) = (yyvsp[(2) - (2)].decl);
                     SwigType_push((yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].decl).type);
		     (yyval.decl).type = (yyvsp[(1) - (2)].type);
		     Delete((yyvsp[(2) - (2)].decl).type);
                  }
    break;

  case 341:

/* Line 1806 of yacc.c  */
#line 5744 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (2)].type);
		    SwigType_add_reference((yyval.decl).type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		  }
    break;

  case 342:

/* Line 1806 of yacc.c  */
#line 5751 "parser.y"
    {
		    (yyval.decl).type = (yyvsp[(1) - (2)].type);
		    SwigType_add_rvalue_reference((yyval.decl).type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		  }
    break;

  case 343:

/* Line 1806 of yacc.c  */
#line 5758 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (3)].decl);
		    SwigType_add_reference((yyvsp[(1) - (3)].type));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (3)].type);
                  }
    break;

  case 344:

/* Line 1806 of yacc.c  */
#line 5767 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(3) - (3)].decl);
		    SwigType_add_rvalue_reference((yyvsp[(1) - (3)].type));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (3)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (3)].type);
                  }
    break;

  case 345:

/* Line 1806 of yacc.c  */
#line 5776 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(1) - (1)].decl);
                  }
    break;

  case 346:

/* Line 1806 of yacc.c  */
#line 5779 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(2) - (2)].decl);
		    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_reference((yyval.decl).type);
		    if ((yyvsp[(2) - (2)].decl).type) {
		      SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
		      Delete((yyvsp[(2) - (2)].decl).type);
		    }
                  }
    break;

  case 347:

/* Line 1806 of yacc.c  */
#line 5788 "parser.y"
    {
		    (yyval.decl) = (yyvsp[(2) - (2)].decl);
		    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_rvalue_reference((yyval.decl).type);
		    if ((yyvsp[(2) - (2)].decl).type) {
		      SwigType_push((yyval.decl).type,(yyvsp[(2) - (2)].decl).type);
		      Delete((yyvsp[(2) - (2)].decl).type);
		    }
                  }
    break;

  case 348:

/* Line 1806 of yacc.c  */
#line 5797 "parser.y"
    {
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_reference((yyval.decl).type);
                  }
    break;

  case 349:

/* Line 1806 of yacc.c  */
#line 5804 "parser.y"
    {
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
                    (yyval.decl).type = NewStringEmpty();
		    SwigType_add_rvalue_reference((yyval.decl).type);
                  }
    break;

  case 350:

/* Line 1806 of yacc.c  */
#line 5811 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
                    SwigType_add_memberpointer((yyval.decl).type,(yyvsp[(1) - (2)].str));
                    (yyval.decl).id = 0;
                    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
      	          }
    break;

  case 351:

/* Line 1806 of yacc.c  */
#line 5818 "parser.y"
    { 
		    SwigType *t = NewStringEmpty();
                    (yyval.decl).type = (yyvsp[(1) - (3)].type);
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_memberpointer(t,(yyvsp[(2) - (3)].str));
		    SwigType_push((yyval.decl).type,t);
		    Delete(t);
                  }
    break;

  case 352:

/* Line 1806 of yacc.c  */
#line 5828 "parser.y"
    { 
		    (yyval.decl) = (yyvsp[(4) - (4)].decl);
		    SwigType_add_memberpointer((yyvsp[(1) - (4)].type),(yyvsp[(2) - (4)].str));
		    if ((yyval.decl).type) {
		      SwigType_push((yyvsp[(1) - (4)].type),(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = (yyvsp[(1) - (4)].type);
                  }
    break;

  case 353:

/* Line 1806 of yacc.c  */
#line 5839 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (3)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(char*)"");
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 354:

/* Line 1806 of yacc.c  */
#line 5850 "parser.y"
    { 
		    SwigType *t;
		    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
		    SwigType_add_array(t,(yyvsp[(3) - (4)].dtype).val);
		    if ((yyval.decl).type) {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		    }
		    (yyval.decl).type = t;
                  }
    break;

  case 355:

/* Line 1806 of yacc.c  */
#line 5861 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_array((yyval.decl).type,(char*)"");
                  }
    break;

  case 356:

/* Line 1806 of yacc.c  */
#line 5868 "parser.y"
    { 
		    (yyval.decl).type = NewStringEmpty();
		    (yyval.decl).id = 0;
		    (yyval.decl).parms = 0;
		    (yyval.decl).have_parms = 0;
		    SwigType_add_array((yyval.decl).type,(yyvsp[(2) - (3)].dtype).val);
		  }
    break;

  case 357:

/* Line 1806 of yacc.c  */
#line 5875 "parser.y"
    {
                    (yyval.decl) = (yyvsp[(2) - (3)].decl);
		  }
    break;

  case 358:

/* Line 1806 of yacc.c  */
#line 5878 "parser.y"
    {
		    SwigType *t;
                    (yyval.decl) = (yyvsp[(1) - (4)].decl);
		    t = NewStringEmpty();
                    SwigType_add_function(t,(yyvsp[(3) - (4)].pl));
		    if (!(yyval.decl).type) {
		      (yyval.decl).type = t;
		    } else {
		      SwigType_push(t,(yyval.decl).type);
		      Delete((yyval.decl).type);
		      (yyval.decl).type = t;
		    }
		    if (!(yyval.decl).have_parms) {
		      (yyval.decl).parms = (yyvsp[(3) - (4)].pl);
		      (yyval.decl).have_parms = 1;
		    }
		  }
    break;

  case 359:

/* Line 1806 of yacc.c  */
#line 5895 "parser.y"
    {
                    (yyval.decl).type = NewStringEmpty();
                    SwigType_add_function((yyval.decl).type,(yyvsp[(2) - (3)].pl));
		    (yyval.decl).parms = (yyvsp[(2) - (3)].pl);
		    (yyval.decl).have_parms = 1;
		    (yyval.decl).id = 0;
                  }
    break;

  case 360:

/* Line 1806 of yacc.c  */
#line 5905 "parser.y"
    { 
             (yyval.type) = NewStringEmpty();
             SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (3)].str));
	     SwigType_push((yyval.type),(yyvsp[(3) - (3)].type));
	     Delete((yyvsp[(3) - (3)].type));
           }
    break;

  case 361:

/* Line 1806 of yacc.c  */
#line 5912 "parser.y"
    {
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (2)].type));
	     Delete((yyvsp[(2) - (2)].type));
	   }
    break;

  case 362:

/* Line 1806 of yacc.c  */
#line 5918 "parser.y"
    { 
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
	     SwigType_push((yyval.type),(yyvsp[(2) - (2)].str));
           }
    break;

  case 363:

/* Line 1806 of yacc.c  */
#line 5923 "parser.y"
    {
	     (yyval.type) = NewStringEmpty();
	     SwigType_add_pointer((yyval.type));
           }
    break;

  case 364:

/* Line 1806 of yacc.c  */
#line 5929 "parser.y"
    {
	          (yyval.str) = NewStringEmpty();
	          if ((yyvsp[(1) - (1)].id)) SwigType_add_qualifier((yyval.str),(yyvsp[(1) - (1)].id));
               }
    break;

  case 365:

/* Line 1806 of yacc.c  */
#line 5933 "parser.y"
    {
		  (yyval.str) = (yyvsp[(2) - (2)].str);
	          if ((yyvsp[(1) - (2)].id)) SwigType_add_qualifier((yyval.str),(yyvsp[(1) - (2)].id));
               }
    break;

  case 366:

/* Line 1806 of yacc.c  */
#line 5939 "parser.y"
    { (yyval.id) = "const"; }
    break;

  case 367:

/* Line 1806 of yacc.c  */
#line 5940 "parser.y"
    { (yyval.id) = "volatile"; }
    break;

  case 368:

/* Line 1806 of yacc.c  */
#line 5941 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 369:

/* Line 1806 of yacc.c  */
#line 5947 "parser.y"
    {
                   (yyval.type) = (yyvsp[(1) - (1)].type);
                   Replace((yyval.type),"typename ","", DOH_REPLACE_ANY);
                }
    break;

  case 370:

/* Line 1806 of yacc.c  */
#line 5953 "parser.y"
    {
                   (yyval.type) = (yyvsp[(2) - (2)].type);
	           SwigType_push((yyval.type),(yyvsp[(1) - (2)].str));
               }
    break;

  case 371:

/* Line 1806 of yacc.c  */
#line 5957 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 372:

/* Line 1806 of yacc.c  */
#line 5958 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (2)].type);
	          SwigType_push((yyval.type),(yyvsp[(2) - (2)].str));
	       }
    break;

  case 373:

/* Line 1806 of yacc.c  */
#line 5962 "parser.y"
    {
		  (yyval.type) = (yyvsp[(2) - (3)].type);
	          SwigType_push((yyval.type),(yyvsp[(3) - (3)].str));
	          SwigType_push((yyval.type),(yyvsp[(1) - (3)].str));
	       }
    break;

  case 374:

/* Line 1806 of yacc.c  */
#line 5969 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type);
                  /* Printf(stdout,"primitive = '%s'\n", $$);*/
               }
    break;

  case 375:

/* Line 1806 of yacc.c  */
#line 5972 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 376:

/* Line 1806 of yacc.c  */
#line 5973 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 377:

/* Line 1806 of yacc.c  */
#line 5974 "parser.y"
    { (yyval.type) = NewStringf("%s%s",(yyvsp[(1) - (2)].type),(yyvsp[(2) - (2)].id)); }
    break;

  case 378:

/* Line 1806 of yacc.c  */
#line 5975 "parser.y"
    { (yyval.type) = NewStringf("enum %s", (yyvsp[(2) - (2)].str)); }
    break;

  case 379:

/* Line 1806 of yacc.c  */
#line 5976 "parser.y"
    { (yyval.type) = (yyvsp[(1) - (1)].type); }
    break;

  case 380:

/* Line 1806 of yacc.c  */
#line 5978 "parser.y"
    {
		  (yyval.type) = (yyvsp[(1) - (1)].str);
               }
    break;

  case 381:

/* Line 1806 of yacc.c  */
#line 5981 "parser.y"
    { 
		 (yyval.type) = NewStringf("%s %s", (yyvsp[(1) - (2)].id), (yyvsp[(2) - (2)].str));
               }
    break;

  case 382:

/* Line 1806 of yacc.c  */
#line 5984 "parser.y"
    {
                 (yyval.type) = (yyvsp[(1) - (1)].type);
               }
    break;

  case 383:

/* Line 1806 of yacc.c  */
#line 5989 "parser.y"
    {
                 Node *n = Swig_symbol_clookup((yyvsp[(3) - (4)].str),0);
                 if (!n) {
		   Swig_error(cparse_file, cparse_line, "Identifier %s not defined.\n", (yyvsp[(3) - (4)].str));
                 } else {
                   (yyval.type) = Getattr(n, "type");
                 }
               }
    break;

  case 384:

/* Line 1806 of yacc.c  */
#line 5999 "parser.y"
    {
		 if (!(yyvsp[(1) - (1)].ptype).type) (yyvsp[(1) - (1)].ptype).type = NewString("int");
		 if ((yyvsp[(1) - (1)].ptype).us) {
		   (yyval.type) = NewStringf("%s %s", (yyvsp[(1) - (1)].ptype).us, (yyvsp[(1) - (1)].ptype).type);
		   Delete((yyvsp[(1) - (1)].ptype).us);
                   Delete((yyvsp[(1) - (1)].ptype).type);
		 } else {
                   (yyval.type) = (yyvsp[(1) - (1)].ptype).type;
		 }
		 if (Cmp((yyval.type),"signed int") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("int");
                 } else if (Cmp((yyval.type),"signed long") == 0) {
		   Delete((yyval.type));
                   (yyval.type) = NewString("long");
                 } else if (Cmp((yyval.type),"signed short") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("short");
		 } else if (Cmp((yyval.type),"signed long long") == 0) {
		   Delete((yyval.type));
		   (yyval.type) = NewString("long long");
		 }
               }
    break;

  case 385:

/* Line 1806 of yacc.c  */
#line 6024 "parser.y"
    { 
                 (yyval.ptype) = (yyvsp[(1) - (1)].ptype);
               }
    break;

  case 386:

/* Line 1806 of yacc.c  */
#line 6027 "parser.y"
    {
                    if ((yyvsp[(1) - (2)].ptype).us && (yyvsp[(2) - (2)].ptype).us) {
		      Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", (yyvsp[(2) - (2)].ptype).us);
		    }
                    (yyval.ptype) = (yyvsp[(2) - (2)].ptype);
                    if ((yyvsp[(1) - (2)].ptype).us) (yyval.ptype).us = (yyvsp[(1) - (2)].ptype).us;
		    if ((yyvsp[(1) - (2)].ptype).type) {
		      if (!(yyvsp[(2) - (2)].ptype).type) (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
		      else {
			int err = 0;
			if ((Cmp((yyvsp[(1) - (2)].ptype).type,"long") == 0)) {
			  if ((Cmp((yyvsp[(2) - (2)].ptype).type,"long") == 0) || (Strncmp((yyvsp[(2) - (2)].ptype).type,"double",6) == 0)) {
			    (yyval.ptype).type = NewStringf("long %s", (yyvsp[(2) - (2)].ptype).type);
			  } else if (Cmp((yyvsp[(2) - (2)].ptype).type,"int") == 0) {
			    (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
			  } else {
			    err = 1;
			  }
			} else if ((Cmp((yyvsp[(1) - (2)].ptype).type,"short")) == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"int") == 0) {
			    (yyval.ptype).type = (yyvsp[(1) - (2)].ptype).type;
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"int") == 0) {
			  (yyval.ptype).type = (yyvsp[(2) - (2)].ptype).type;
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"double") == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"long") == 0) {
			    (yyval.ptype).type = NewString("long double");
			  } else if (Cmp((yyvsp[(2) - (2)].ptype).type,"complex") == 0) {
			    (yyval.ptype).type = NewString("double complex");
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"float") == 0) {
			  if (Cmp((yyvsp[(2) - (2)].ptype).type,"complex") == 0) {
			    (yyval.ptype).type = NewString("float complex");
			  } else {
			    err = 1;
			  }
			} else if (Cmp((yyvsp[(1) - (2)].ptype).type,"complex") == 0) {
			  (yyval.ptype).type = NewStringf("%s complex", (yyvsp[(2) - (2)].ptype).type);
			} else {
			  err = 1;
			}
			if (err) {
			  Swig_error(cparse_file, cparse_line, "Extra %s specifier.\n", (yyvsp[(1) - (2)].ptype).type);
			}
		      }
		    }
               }
    break;

  case 387:

/* Line 1806 of yacc.c  */
#line 6081 "parser.y"
    { 
		    (yyval.ptype).type = NewString("int");
                    (yyval.ptype).us = 0;
               }
    break;

  case 388:

/* Line 1806 of yacc.c  */
#line 6085 "parser.y"
    { 
                    (yyval.ptype).type = NewString("short");
                    (yyval.ptype).us = 0;
                }
    break;

  case 389:

/* Line 1806 of yacc.c  */
#line 6089 "parser.y"
    { 
                    (yyval.ptype).type = NewString("long");
                    (yyval.ptype).us = 0;
                }
    break;

  case 390:

/* Line 1806 of yacc.c  */
#line 6093 "parser.y"
    { 
                    (yyval.ptype).type = NewString("char");
                    (yyval.ptype).us = 0;
                }
    break;

  case 391:

/* Line 1806 of yacc.c  */
#line 6097 "parser.y"
    { 
                    (yyval.ptype).type = NewString("wchar_t");
                    (yyval.ptype).us = 0;
                }
    break;

  case 392:

/* Line 1806 of yacc.c  */
#line 6101 "parser.y"
    { 
                    (yyval.ptype).type = NewString("float");
                    (yyval.ptype).us = 0;
                }
    break;

  case 393:

/* Line 1806 of yacc.c  */
#line 6105 "parser.y"
    { 
                    (yyval.ptype).type = NewString("double");
                    (yyval.ptype).us = 0;
                }
    break;

  case 394:

/* Line 1806 of yacc.c  */
#line 6109 "parser.y"
    { 
                    (yyval.ptype).us = NewString("signed");
                    (yyval.ptype).type = 0;
                }
    break;

  case 395:

/* Line 1806 of yacc.c  */
#line 6113 "parser.y"
    { 
                    (yyval.ptype).us = NewString("unsigned");
                    (yyval.ptype).type = 0;
                }
    break;

  case 396:

/* Line 1806 of yacc.c  */
#line 6117 "parser.y"
    { 
                    (yyval.ptype).type = NewString("complex");
                    (yyval.ptype).us = 0;
                }
    break;

  case 397:

/* Line 1806 of yacc.c  */
#line 6121 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int8");
                    (yyval.ptype).us = 0;
                }
    break;

  case 398:

/* Line 1806 of yacc.c  */
#line 6125 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int16");
                    (yyval.ptype).us = 0;
                }
    break;

  case 399:

/* Line 1806 of yacc.c  */
#line 6129 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int32");
                    (yyval.ptype).us = 0;
                }
    break;

  case 400:

/* Line 1806 of yacc.c  */
#line 6133 "parser.y"
    { 
                    (yyval.ptype).type = NewString("__int64");
                    (yyval.ptype).us = 0;
                }
    break;

  case 401:

/* Line 1806 of yacc.c  */
#line 6139 "parser.y"
    { /* scanner_check_typedef(); */ }
    break;

  case 402:

/* Line 1806 of yacc.c  */
#line 6139 "parser.y"
    {
                   (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
		   if ((yyval.dtype).type == T_STRING) {
		     (yyval.dtype).rawval = NewStringf("\"%(escape)s\"",(yyval.dtype).val);
		   } else if ((yyval.dtype).type != T_CHAR && (yyval.dtype).type != T_WSTRING && (yyval.dtype).type != T_WCHAR) {
		     (yyval.dtype).rawval = 0;
		   }
		   (yyval.dtype).bitfield = 0;
		   (yyval.dtype).throws = 0;
		   (yyval.dtype).throwf = 0;
		   scanner_ignore_typedef();
                }
    break;

  case 403:

/* Line 1806 of yacc.c  */
#line 6165 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 404:

/* Line 1806 of yacc.c  */
#line 6166 "parser.y"
    { (yyval.id) = (char *) 0;}
    break;

  case 405:

/* Line 1806 of yacc.c  */
#line 6169 "parser.y"
    { (yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 406:

/* Line 1806 of yacc.c  */
#line 6170 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 407:

/* Line 1806 of yacc.c  */
#line 6174 "parser.y"
    {
		 Node *leftSibling = Getattr((yyvsp[(1) - (5)].node),"_last");
		 set_nextSibling(leftSibling,(yyvsp[(4) - (5)].node));
		 Setattr((yyvsp[(1) - (5)].node),"_last",(yyvsp[(4) - (5)].node));
		 (yyval.node) = (yyvsp[(1) - (5)].node);
	       }
    break;

  case 408:

/* Line 1806 of yacc.c  */
#line 6180 "parser.y"
    {
		 (yyval.node) = (yyvsp[(1) - (3)].node);
	       }
    break;

  case 409:

/* Line 1806 of yacc.c  */
#line 6183 "parser.y"
    {
		 Setattr((yyvsp[(2) - (3)].node),"_last",(yyvsp[(2) - (3)].node));
		 (yyval.node) = (yyvsp[(2) - (3)].node);
	       }
    break;

  case 410:

/* Line 1806 of yacc.c  */
#line 6187 "parser.y"
    {
		 (yyval.node) = 0;
	       }
    break;

  case 411:

/* Line 1806 of yacc.c  */
#line 6192 "parser.y"
    {
		   SwigType *type = NewSwigType(T_INT);
		   (yyval.node) = new_node("enumitem");
		   Setattr((yyval.node),"name",(yyvsp[(1) - (1)].id));
		   Setattr((yyval.node),"type",type);
		   SetFlag((yyval.node),"feature:immutable");
		   Delete(type);
		 }
    break;

  case 412:

/* Line 1806 of yacc.c  */
#line 6200 "parser.y"
    {
		   SwigType *type = NewSwigType((yyvsp[(3) - (3)].dtype).type == T_BOOL ? T_BOOL : ((yyvsp[(3) - (3)].dtype).type == T_CHAR ? T_CHAR : T_INT));
		   (yyval.node) = new_node("enumitem");
		   Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
		   Setattr((yyval.node),"type",type);
		   SetFlag((yyval.node),"feature:immutable");
		   Setattr((yyval.node),"enumvalue", (yyvsp[(3) - (3)].dtype).val);
		   Setattr((yyval.node),"value",(yyvsp[(1) - (3)].id));
		   Delete(type);
                 }
    break;

  case 413:

/* Line 1806 of yacc.c  */
#line 6212 "parser.y"
    {
                   (yyval.dtype) = (yyvsp[(1) - (1)].dtype);
		   if (((yyval.dtype).type != T_INT) && ((yyval.dtype).type != T_UINT) &&
		       ((yyval.dtype).type != T_LONG) && ((yyval.dtype).type != T_ULONG) &&
		       ((yyval.dtype).type != T_LONGLONG) && ((yyval.dtype).type != T_ULONGLONG) &&
		       ((yyval.dtype).type != T_SHORT) && ((yyval.dtype).type != T_USHORT) &&
		       ((yyval.dtype).type != T_SCHAR) && ((yyval.dtype).type != T_UCHAR) &&
		       ((yyval.dtype).type != T_CHAR) && ((yyval.dtype).type != T_BOOL)) {
		     Swig_error(cparse_file,cparse_line,"Type error. Expecting an integral type\n");
		   }
                }
    break;

  case 414:

/* Line 1806 of yacc.c  */
#line 6227 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 415:

/* Line 1806 of yacc.c  */
#line 6228 "parser.y"
    {
		 Node *n;
		 (yyval.dtype).val = (yyvsp[(1) - (1)].type);
		 (yyval.dtype).type = T_INT;
		 /* Check if value is in scope */
		 n = Swig_symbol_clookup((yyvsp[(1) - (1)].type),0);
		 if (n) {
                   /* A band-aid for enum values used in expressions. */
                   if (Strcmp(nodeType(n),"enumitem") == 0) {
                     String *q = Swig_symbol_qualified(n);
                     if (q) {
                       (yyval.dtype).val = NewStringf("%s::%s", q, Getattr(n,"name"));
                       Delete(q);
                     }
                   }
		 }
               }
    break;

  case 416:

/* Line 1806 of yacc.c  */
#line 6247 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 417:

/* Line 1806 of yacc.c  */
#line 6248 "parser.y"
    {
		    (yyval.dtype).val = NewString((yyvsp[(1) - (1)].id));
                    (yyval.dtype).type = T_STRING;
               }
    break;

  case 418:

/* Line 1806 of yacc.c  */
#line 6252 "parser.y"
    {
		  SwigType_push((yyvsp[(3) - (5)].type),(yyvsp[(4) - (5)].decl).type);
		  (yyval.dtype).val = NewStringf("sizeof(%s)",SwigType_str((yyvsp[(3) - (5)].type),0));
		  (yyval.dtype).type = T_ULONG;
               }
    break;

  case 419:

/* Line 1806 of yacc.c  */
#line 6257 "parser.y"
    {
		  SwigType_push((yyvsp[(6) - (8)].type),(yyvsp[(7) - (8)].decl).type);
		  (yyval.dtype).val = NewStringf("sizeof...(%s)",SwigType_str((yyvsp[(6) - (8)].type),0));
		  (yyval.dtype).type = T_ULONG;
               }
    break;

  case 420:

/* Line 1806 of yacc.c  */
#line 6262 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 421:

/* Line 1806 of yacc.c  */
#line 6263 "parser.y"
    {
		    (yyval.dtype).val = NewString((yyvsp[(1) - (1)].id));
		    (yyval.dtype).rawval = NewStringf("L\"%s\"", (yyval.dtype).val);
                    (yyval.dtype).type = T_WSTRING;
	       }
    break;

  case 422:

/* Line 1806 of yacc.c  */
#line 6268 "parser.y"
    {
		  (yyval.dtype).val = NewString((yyvsp[(1) - (1)].str));
		  if (Len((yyval.dtype).val)) {
		    (yyval.dtype).rawval = NewStringf("'%(escape)s'", (yyval.dtype).val);
		  } else {
		    (yyval.dtype).rawval = NewString("'\\0'");
		  }
		  (yyval.dtype).type = T_CHAR;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
	       }
    break;

  case 423:

/* Line 1806 of yacc.c  */
#line 6280 "parser.y"
    {
		  (yyval.dtype).val = NewString((yyvsp[(1) - (1)].str));
		  if (Len((yyval.dtype).val)) {
		    (yyval.dtype).rawval = NewStringf("L\'%s\'", (yyval.dtype).val);
		  } else {
		    (yyval.dtype).rawval = NewString("L'\\0'");
		  }
		  (yyval.dtype).type = T_WCHAR;
		  (yyval.dtype).bitfield = 0;
		  (yyval.dtype).throws = 0;
		  (yyval.dtype).throwf = 0;
	       }
    break;

  case 424:

/* Line 1806 of yacc.c  */
#line 6294 "parser.y"
    {
   	            (yyval.dtype).val = NewStringf("(%s)",(yyvsp[(2) - (3)].dtype).val);
		    (yyval.dtype).type = (yyvsp[(2) - (3)].dtype).type;
   	       }
    break;

  case 425:

/* Line 1806 of yacc.c  */
#line 6301 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(4) - (4)].dtype);
		 if ((yyvsp[(4) - (4)].dtype).type != T_STRING) {
		   switch ((yyvsp[(2) - (4)].dtype).type) {
		     case T_FLOAT:
		     case T_DOUBLE:
		     case T_LONGDOUBLE:
		     case T_FLTCPLX:
		     case T_DBLCPLX:
		       (yyval.dtype).val = NewStringf("(%s)%s", (yyvsp[(2) - (4)].dtype).val, (yyvsp[(4) - (4)].dtype).val); /* SwigType_str and decimal points don't mix! */
		       break;
		     default:
		       (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (4)].dtype).val,0), (yyvsp[(4) - (4)].dtype).val);
		       break;
		   }
		 }
 	       }
    break;

  case 426:

/* Line 1806 of yacc.c  */
#line 6318 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (5)].dtype).val,(yyvsp[(3) - (5)].type));
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 427:

/* Line 1806 of yacc.c  */
#line 6325 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_add_reference((yyvsp[(2) - (5)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 428:

/* Line 1806 of yacc.c  */
#line 6332 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(5) - (5)].dtype);
		 if ((yyvsp[(5) - (5)].dtype).type != T_STRING) {
		   SwigType_add_rvalue_reference((yyvsp[(2) - (5)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (5)].dtype).val,0), (yyvsp[(5) - (5)].dtype).val);
		 }
 	       }
    break;

  case 429:

/* Line 1806 of yacc.c  */
#line 6339 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(6) - (6)].dtype);
		 if ((yyvsp[(6) - (6)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (6)].dtype).val,(yyvsp[(3) - (6)].type));
		   SwigType_add_reference((yyvsp[(2) - (6)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (6)].dtype).val,0), (yyvsp[(6) - (6)].dtype).val);
		 }
 	       }
    break;

  case 430:

/* Line 1806 of yacc.c  */
#line 6347 "parser.y"
    {
                 (yyval.dtype) = (yyvsp[(6) - (6)].dtype);
		 if ((yyvsp[(6) - (6)].dtype).type != T_STRING) {
		   SwigType_push((yyvsp[(2) - (6)].dtype).val,(yyvsp[(3) - (6)].type));
		   SwigType_add_rvalue_reference((yyvsp[(2) - (6)].dtype).val);
		   (yyval.dtype).val = NewStringf("(%s) %s", SwigType_str((yyvsp[(2) - (6)].dtype).val,0), (yyvsp[(6) - (6)].dtype).val);
		 }
 	       }
    break;

  case 431:

/* Line 1806 of yacc.c  */
#line 6355 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("&%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 432:

/* Line 1806 of yacc.c  */
#line 6359 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("&&%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 433:

/* Line 1806 of yacc.c  */
#line 6363 "parser.y"
    {
		 (yyval.dtype) = (yyvsp[(2) - (2)].dtype);
                 (yyval.dtype).val = NewStringf("*%s",(yyvsp[(2) - (2)].dtype).val);
	       }
    break;

  case 434:

/* Line 1806 of yacc.c  */
#line 6369 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 435:

/* Line 1806 of yacc.c  */
#line 6370 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 436:

/* Line 1806 of yacc.c  */
#line 6371 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 437:

/* Line 1806 of yacc.c  */
#line 6372 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 438:

/* Line 1806 of yacc.c  */
#line 6373 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 439:

/* Line 1806 of yacc.c  */
#line 6374 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 440:

/* Line 1806 of yacc.c  */
#line 6375 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 441:

/* Line 1806 of yacc.c  */
#line 6376 "parser.y"
    { (yyval.dtype) = (yyvsp[(1) - (1)].dtype); }
    break;

  case 442:

/* Line 1806 of yacc.c  */
#line 6379 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s+%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 443:

/* Line 1806 of yacc.c  */
#line 6383 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s-%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 444:

/* Line 1806 of yacc.c  */
#line 6387 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s*%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 445:

/* Line 1806 of yacc.c  */
#line 6391 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s/%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 446:

/* Line 1806 of yacc.c  */
#line 6395 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s%%%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 447:

/* Line 1806 of yacc.c  */
#line 6399 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s&%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 448:

/* Line 1806 of yacc.c  */
#line 6403 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s|%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 449:

/* Line 1806 of yacc.c  */
#line 6407 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s^%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote((yyvsp[(1) - (3)].dtype).type,(yyvsp[(3) - (3)].dtype).type);
	       }
    break;

  case 450:

/* Line 1806 of yacc.c  */
#line 6411 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s << %s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote_type((yyvsp[(1) - (3)].dtype).type);
	       }
    break;

  case 451:

/* Line 1806 of yacc.c  */
#line 6415 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s >> %s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = promote_type((yyvsp[(1) - (3)].dtype).type);
	       }
    break;

  case 452:

/* Line 1806 of yacc.c  */
#line 6419 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s&&%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 453:

/* Line 1806 of yacc.c  */
#line 6423 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s||%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 454:

/* Line 1806 of yacc.c  */
#line 6427 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s==%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 455:

/* Line 1806 of yacc.c  */
#line 6431 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s!=%s",(yyvsp[(1) - (3)].dtype).val,(yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 456:

/* Line 1806 of yacc.c  */
#line 6445 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s >= %s", (yyvsp[(1) - (3)].dtype).val, (yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 457:

/* Line 1806 of yacc.c  */
#line 6449 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s <= %s", (yyvsp[(1) - (3)].dtype).val, (yyvsp[(3) - (3)].dtype).val);
		 (yyval.dtype).type = cparse_cplusplus ? T_BOOL : T_INT;
	       }
    break;

  case 458:

/* Line 1806 of yacc.c  */
#line 6453 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("%s?%s:%s", (yyvsp[(1) - (5)].dtype).val, (yyvsp[(3) - (5)].dtype).val, (yyvsp[(5) - (5)].dtype).val);
		 /* This may not be exactly right, but is probably good enough
		  * for the purposes of parsing constant expressions. */
		 (yyval.dtype).type = promote((yyvsp[(3) - (5)].dtype).type, (yyvsp[(5) - (5)].dtype).type);
	       }
    break;

  case 459:

/* Line 1806 of yacc.c  */
#line 6459 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("-%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 460:

/* Line 1806 of yacc.c  */
#line 6463 "parser.y"
    {
                 (yyval.dtype).val = NewStringf("+%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 461:

/* Line 1806 of yacc.c  */
#line 6467 "parser.y"
    {
		 (yyval.dtype).val = NewStringf("~%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = (yyvsp[(2) - (2)].dtype).type;
	       }
    break;

  case 462:

/* Line 1806 of yacc.c  */
#line 6471 "parser.y"
    {
                 (yyval.dtype).val = NewStringf("!%s",(yyvsp[(2) - (2)].dtype).val);
		 (yyval.dtype).type = T_INT;
	       }
    break;

  case 463:

/* Line 1806 of yacc.c  */
#line 6475 "parser.y"
    {
		 String *qty;
                 skip_balanced('(',')');
		 qty = Swig_symbol_type_qualify((yyvsp[(1) - (2)].type),0);
		 if (SwigType_istemplate(qty)) {
		   String *nstr = SwigType_namestr(qty);
		   Delete(qty);
		   qty = nstr;
		 }
		 (yyval.dtype).val = NewStringf("%s%s",qty,scanner_ccode);
		 Clear(scanner_ccode);
		 (yyval.dtype).type = T_INT;
		 Delete(qty);
               }
    break;

  case 464:

/* Line 1806 of yacc.c  */
#line 6491 "parser.y"
    {
	        (yyval.str) = NewString("...");
	      }
    break;

  case 465:

/* Line 1806 of yacc.c  */
#line 6496 "parser.y"
    {
	        (yyval.str) = (yyvsp[(1) - (1)].str);
	      }
    break;

  case 466:

/* Line 1806 of yacc.c  */
#line 6499 "parser.y"
    {
	        (yyval.str) = 0;
	      }
    break;

  case 467:

/* Line 1806 of yacc.c  */
#line 6504 "parser.y"
    {
		 (yyval.bases) = (yyvsp[(1) - (1)].bases);
               }
    break;

  case 468:

/* Line 1806 of yacc.c  */
#line 6509 "parser.y"
    { inherit_list = 1; }
    break;

  case 469:

/* Line 1806 of yacc.c  */
#line 6509 "parser.y"
    { (yyval.bases) = (yyvsp[(3) - (3)].bases); inherit_list = 0; }
    break;

  case 470:

/* Line 1806 of yacc.c  */
#line 6510 "parser.y"
    { (yyval.bases) = 0; }
    break;

  case 471:

/* Line 1806 of yacc.c  */
#line 6513 "parser.y"
    {
		   Hash *list = NewHash();
		   Node *base = (yyvsp[(1) - (1)].node);
		   Node *name = Getattr(base,"name");
		   List *lpublic = NewList();
		   List *lprotected = NewList();
		   List *lprivate = NewList();
		   Setattr(list,"public",lpublic);
		   Setattr(list,"protected",lprotected);
		   Setattr(list,"private",lprivate);
		   Delete(lpublic);
		   Delete(lprotected);
		   Delete(lprivate);
		   Append(Getattr(list,Getattr(base,"access")),name);
	           (yyval.bases) = list;
               }
    break;

  case 472:

/* Line 1806 of yacc.c  */
#line 6530 "parser.y"
    {
		   Hash *list = (yyvsp[(1) - (3)].bases);
		   Node *base = (yyvsp[(3) - (3)].node);
		   Node *name = Getattr(base,"name");
		   Append(Getattr(list,Getattr(base,"access")),name);
                   (yyval.bases) = list;
               }
    break;

  case 473:

/* Line 1806 of yacc.c  */
#line 6539 "parser.y"
    {
		 (yyval.intvalue) = cparse_line;
	       }
    break;

  case 474:

/* Line 1806 of yacc.c  */
#line 6541 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setfile((yyval.node),cparse_file);
		 Setline((yyval.node),(yyvsp[(2) - (4)].intvalue));
		 Setattr((yyval.node),"name",(yyvsp[(3) - (4)].str));
		 Setfile((yyvsp[(3) - (4)].str),cparse_file);
		 Setline((yyvsp[(3) - (4)].str),(yyvsp[(2) - (4)].intvalue));
                 if (last_cpptype && (Strcmp(last_cpptype,"struct") != 0)) {
		   Setattr((yyval.node),"access","private");
		   Swig_warning(WARN_PARSE_NO_ACCESS, Getfile((yyval.node)), Getline((yyval.node)), "No access specifier given for base class '%s' (ignored).\n", SwigType_namestr((yyvsp[(3) - (4)].str)));
                 } else {
		   Setattr((yyval.node),"access","public");
		 }
		 if ((yyvsp[(4) - (4)].str))
		   SetFlag((yyval.node), "variadic");
               }
    break;

  case 475:

/* Line 1806 of yacc.c  */
#line 6557 "parser.y"
    {
		 (yyval.intvalue) = cparse_line;
	       }
    break;

  case 476:

/* Line 1806 of yacc.c  */
#line 6559 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setfile((yyval.node),cparse_file);
		 Setline((yyval.node),(yyvsp[(3) - (6)].intvalue));
		 Setattr((yyval.node),"name",(yyvsp[(5) - (6)].str));
		 Setfile((yyvsp[(5) - (6)].str),cparse_file);
		 Setline((yyvsp[(5) - (6)].str),(yyvsp[(3) - (6)].intvalue));
		 Setattr((yyval.node),"access",(yyvsp[(2) - (6)].id));
	         if (Strcmp((yyvsp[(2) - (6)].id),"public") != 0) {
		   Swig_warning(WARN_PARSE_PRIVATE_INHERIT, Getfile((yyval.node)), Getline((yyval.node)), "%s inheritance from base '%s' (ignored).\n", (yyvsp[(2) - (6)].id), SwigType_namestr((yyvsp[(5) - (6)].str)));
		 }
		 if ((yyvsp[(6) - (6)].str))
		   SetFlag((yyval.node), "variadic");
               }
    break;

  case 477:

/* Line 1806 of yacc.c  */
#line 6575 "parser.y"
    { (yyval.id) = (char*)"public"; }
    break;

  case 478:

/* Line 1806 of yacc.c  */
#line 6576 "parser.y"
    { (yyval.id) = (char*)"private"; }
    break;

  case 479:

/* Line 1806 of yacc.c  */
#line 6577 "parser.y"
    { (yyval.id) = (char*)"protected"; }
    break;

  case 480:

/* Line 1806 of yacc.c  */
#line 6581 "parser.y"
    { 
                   (yyval.id) = (char*)"class"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 481:

/* Line 1806 of yacc.c  */
#line 6585 "parser.y"
    { 
                   (yyval.id) = (char *)"typename"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 482:

/* Line 1806 of yacc.c  */
#line 6589 "parser.y"
    { 
                   (yyval.id) = (char *)"class..."; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 483:

/* Line 1806 of yacc.c  */
#line 6593 "parser.y"
    { 
                   (yyval.id) = (char *)"typename..."; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 484:

/* Line 1806 of yacc.c  */
#line 6599 "parser.y"
    {
                 (yyval.id) = (yyvsp[(1) - (1)].id);
               }
    break;

  case 485:

/* Line 1806 of yacc.c  */
#line 6602 "parser.y"
    { 
                   (yyval.id) = (char*)"struct"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 486:

/* Line 1806 of yacc.c  */
#line 6606 "parser.y"
    {
                   (yyval.id) = (char*)"union"; 
		   if (!inherit_list) last_cpptype = (yyval.id);
               }
    break;

  case 489:

/* Line 1806 of yacc.c  */
#line 6616 "parser.y"
    {
                    (yyval.dtype).qualifier = (yyvsp[(1) - (1)].str);
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
               }
    break;

  case 490:

/* Line 1806 of yacc.c  */
#line 6621 "parser.y"
    {
                    (yyval.dtype).qualifier = 0;
                    (yyval.dtype).throws = (yyvsp[(3) - (4)].pl);
                    (yyval.dtype).throwf = NewString("1");
               }
    break;

  case 491:

/* Line 1806 of yacc.c  */
#line 6626 "parser.y"
    {
                    (yyval.dtype).qualifier = (yyvsp[(1) - (5)].str);
                    (yyval.dtype).throws = (yyvsp[(4) - (5)].pl);
                    (yyval.dtype).throwf = NewString("1");
               }
    break;

  case 492:

/* Line 1806 of yacc.c  */
#line 6631 "parser.y"
    { 
                    (yyval.dtype).qualifier = 0; 
                    (yyval.dtype).throws = 0;
                    (yyval.dtype).throwf = 0;
               }
    break;

  case 493:

/* Line 1806 of yacc.c  */
#line 6638 "parser.y"
    { 
                    Clear(scanner_ccode); 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = 0; 
		    (yyval.decl).throws = (yyvsp[(1) - (3)].dtype).throws;
		    (yyval.decl).throwf = (yyvsp[(1) - (3)].dtype).throwf;
               }
    break;

  case 494:

/* Line 1806 of yacc.c  */
#line 6645 "parser.y"
    { 
                    skip_balanced('{','}'); 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = 0; 
                    (yyval.decl).throws = (yyvsp[(1) - (3)].dtype).throws;
                    (yyval.decl).throwf = (yyvsp[(1) - (3)].dtype).throwf;
               }
    break;

  case 495:

/* Line 1806 of yacc.c  */
#line 6652 "parser.y"
    { 
                    Clear(scanner_ccode); 
                    (yyval.decl).parms = (yyvsp[(2) - (4)].pl); 
                    (yyval.decl).have_parms = 1; 
                    (yyval.decl).defarg = 0; 
		    (yyval.decl).throws = 0;
		    (yyval.decl).throwf = 0;
               }
    break;

  case 496:

/* Line 1806 of yacc.c  */
#line 6660 "parser.y"
    {
                    skip_balanced('{','}'); 
                    (yyval.decl).parms = (yyvsp[(2) - (4)].pl); 
                    (yyval.decl).have_parms = 1; 
                    (yyval.decl).defarg = 0; 
                    (yyval.decl).throws = 0;
                    (yyval.decl).throwf = 0;
               }
    break;

  case 497:

/* Line 1806 of yacc.c  */
#line 6668 "parser.y"
    { 
                    (yyval.decl).have_parms = 0; 
                    (yyval.decl).defarg = (yyvsp[(2) - (3)].dtype).val; 
                    (yyval.decl).throws = 0;
                    (yyval.decl).throwf = 0;
               }
    break;

  case 504:

/* Line 1806 of yacc.c  */
#line 6686 "parser.y"
    {
		  skip_balanced('(',')');
		  Clear(scanner_ccode);
		}
    break;

  case 505:

/* Line 1806 of yacc.c  */
#line 6698 "parser.y"
    {
		  skip_balanced('{','}');
		  Clear(scanner_ccode);
		}
    break;

  case 506:

/* Line 1806 of yacc.c  */
#line 6704 "parser.y"
    { 
                     String *s = NewStringEmpty();
                     SwigType_add_template(s,(yyvsp[(2) - (3)].p));
                     (yyval.id) = Char(s);
		     scanner_last_id(1);
                 }
    break;

  case 507:

/* Line 1806 of yacc.c  */
#line 6710 "parser.y"
    { (yyval.id) = (char*)"";  }
    break;

  case 508:

/* Line 1806 of yacc.c  */
#line 6713 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 509:

/* Line 1806 of yacc.c  */
#line 6714 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 510:

/* Line 1806 of yacc.c  */
#line 6717 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id); }
    break;

  case 511:

/* Line 1806 of yacc.c  */
#line 6718 "parser.y"
    { (yyval.id) = 0; }
    break;

  case 512:

/* Line 1806 of yacc.c  */
#line 6721 "parser.y"
    { 
                  (yyval.str) = 0;
		  if (!(yyval.str)) (yyval.str) = NewStringf("%s%s", (yyvsp[(1) - (2)].str),(yyvsp[(2) - (2)].str));
      	          Delete((yyvsp[(2) - (2)].str));
               }
    break;

  case 513:

/* Line 1806 of yacc.c  */
#line 6726 "parser.y"
    { 
		 (yyval.str) = NewStringf("::%s%s",(yyvsp[(3) - (4)].str),(yyvsp[(4) - (4)].str));
                 Delete((yyvsp[(4) - (4)].str));
               }
    break;

  case 514:

/* Line 1806 of yacc.c  */
#line 6730 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
   	       }
    break;

  case 515:

/* Line 1806 of yacc.c  */
#line 6733 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 516:

/* Line 1806 of yacc.c  */
#line 6736 "parser.y"
    {
                 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
	       }
    break;

  case 517:

/* Line 1806 of yacc.c  */
#line 6739 "parser.y"
    {
                 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 518:

/* Line 1806 of yacc.c  */
#line 6744 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s%s",(yyvsp[(2) - (3)].str),(yyvsp[(3) - (3)].str));
		   Delete((yyvsp[(3) - (3)].str));
               }
    break;

  case 519:

/* Line 1806 of yacc.c  */
#line 6748 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 520:

/* Line 1806 of yacc.c  */
#line 6751 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 521:

/* Line 1806 of yacc.c  */
#line 6758 "parser.y"
    {
		 (yyval.str) = NewStringf("::~%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 522:

/* Line 1806 of yacc.c  */
#line 6764 "parser.y"
    {
                  (yyval.str) = NewStringf("%s%s",(yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].id));
		  /*		  if (Len($2)) {
		    scanner_last_id(1);
		    } */
              }
    break;

  case 523:

/* Line 1806 of yacc.c  */
#line 6773 "parser.y"
    { 
                  (yyval.str) = 0;
		  if (!(yyval.str)) (yyval.str) = NewStringf("%s%s", (yyvsp[(1) - (2)].id),(yyvsp[(2) - (2)].str));
      	          Delete((yyvsp[(2) - (2)].str));
               }
    break;

  case 524:

/* Line 1806 of yacc.c  */
#line 6778 "parser.y"
    { 
		 (yyval.str) = NewStringf("::%s%s",(yyvsp[(3) - (4)].id),(yyvsp[(4) - (4)].str));
                 Delete((yyvsp[(4) - (4)].str));
               }
    break;

  case 525:

/* Line 1806 of yacc.c  */
#line 6782 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].id));
   	       }
    break;

  case 526:

/* Line 1806 of yacc.c  */
#line 6785 "parser.y"
    {
		 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].id));
               }
    break;

  case 527:

/* Line 1806 of yacc.c  */
#line 6788 "parser.y"
    {
                 (yyval.str) = NewString((yyvsp[(1) - (1)].str));
	       }
    break;

  case 528:

/* Line 1806 of yacc.c  */
#line 6791 "parser.y"
    {
                 (yyval.str) = NewStringf("::%s",(yyvsp[(3) - (3)].str));
               }
    break;

  case 529:

/* Line 1806 of yacc.c  */
#line 6796 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s%s",(yyvsp[(2) - (3)].id),(yyvsp[(3) - (3)].str));
		   Delete((yyvsp[(3) - (3)].str));
               }
    break;

  case 530:

/* Line 1806 of yacc.c  */
#line 6800 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].id));
               }
    break;

  case 531:

/* Line 1806 of yacc.c  */
#line 6803 "parser.y"
    {
                   (yyval.str) = NewStringf("::%s",(yyvsp[(2) - (2)].str));
               }
    break;

  case 532:

/* Line 1806 of yacc.c  */
#line 6806 "parser.y"
    {
		 (yyval.str) = NewStringf("::~%s",(yyvsp[(2) - (2)].id));
               }
    break;

  case 533:

/* Line 1806 of yacc.c  */
#line 6812 "parser.y"
    { 
                   (yyval.id) = (char *) malloc(strlen((yyvsp[(1) - (2)].id))+strlen((yyvsp[(2) - (2)].id))+1);
                   strcpy((yyval.id),(yyvsp[(1) - (2)].id));
                   strcat((yyval.id),(yyvsp[(2) - (2)].id));
               }
    break;

  case 534:

/* Line 1806 of yacc.c  */
#line 6817 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id);}
    break;

  case 535:

/* Line 1806 of yacc.c  */
#line 6820 "parser.y"
    {
                   (yyval.id) = (char *) malloc(strlen((yyvsp[(1) - (2)].id))+strlen((yyvsp[(2) - (2)].id))+1);
                   strcpy((yyval.id),(yyvsp[(1) - (2)].id));
                   strcat((yyval.id),(yyvsp[(2) - (2)].id));
               }
    break;

  case 536:

/* Line 1806 of yacc.c  */
#line 6832 "parser.y"
    { (yyval.id) = (yyvsp[(1) - (1)].id);}
    break;

  case 537:

/* Line 1806 of yacc.c  */
#line 6835 "parser.y"
    {
		 (yyval.str) = NewString((yyvsp[(1) - (1)].id));
               }
    break;

  case 538:

/* Line 1806 of yacc.c  */
#line 6838 "parser.y"
    {
                  skip_balanced('{','}');
		  (yyval.str) = NewString(scanner_ccode);
               }
    break;

  case 539:

/* Line 1806 of yacc.c  */
#line 6842 "parser.y"
    {
		 (yyval.str) = (yyvsp[(1) - (1)].str);
              }
    break;

  case 540:

/* Line 1806 of yacc.c  */
#line 6847 "parser.y"
    {
                  Hash *n;
                  (yyval.node) = NewHash();
                  n = (yyvsp[(2) - (3)].node);
                  while(n) {
                     String *name, *value;
                     name = Getattr(n,"name");
                     value = Getattr(n,"value");
		     if (!value) value = (String *) "1";
                     Setattr((yyval.node),name, value);
		     n = nextSibling(n);
		  }
               }
    break;

  case 541:

/* Line 1806 of yacc.c  */
#line 6860 "parser.y"
    { (yyval.node) = 0; }
    break;

  case 542:

/* Line 1806 of yacc.c  */
#line 6864 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
		 Setattr((yyval.node),"value",(yyvsp[(3) - (3)].id));
               }
    break;

  case 543:

/* Line 1806 of yacc.c  */
#line 6869 "parser.y"
    {
		 (yyval.node) = NewHash();
		 Setattr((yyval.node),"name",(yyvsp[(1) - (5)].id));
		 Setattr((yyval.node),"value",(yyvsp[(3) - (5)].id));
		 set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
               }
    break;

  case 544:

/* Line 1806 of yacc.c  */
#line 6875 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"name",(yyvsp[(1) - (1)].id));
	       }
    break;

  case 545:

/* Line 1806 of yacc.c  */
#line 6879 "parser.y"
    {
                 (yyval.node) = NewHash();
                 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
                 set_nextSibling((yyval.node),(yyvsp[(3) - (3)].node));
               }
    break;

  case 546:

/* Line 1806 of yacc.c  */
#line 6884 "parser.y"
    {
                 (yyval.node) = (yyvsp[(3) - (3)].node);
		 Setattr((yyval.node),"name",(yyvsp[(1) - (3)].id));
               }
    break;

  case 547:

/* Line 1806 of yacc.c  */
#line 6888 "parser.y"
    {
                 (yyval.node) = (yyvsp[(3) - (5)].node);
		 Setattr((yyval.node),"name",(yyvsp[(1) - (5)].id));
		 set_nextSibling((yyval.node),(yyvsp[(5) - (5)].node));
               }
    break;

  case 548:

/* Line 1806 of yacc.c  */
#line 6895 "parser.y"
    {
		 (yyval.id) = (yyvsp[(1) - (1)].id);
               }
    break;

  case 549:

/* Line 1806 of yacc.c  */
#line 6898 "parser.y"
    {
                 (yyval.id) = Char((yyvsp[(1) - (1)].dtype).val);
               }
    break;



/* Line 1806 of yacc.c  */
#line 12782 "CParse/parser.c"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 6905 "parser.y"


SwigType *Swig_cparse_type(String *s) {
   String *ns;
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSETYPE);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


Parm *Swig_cparse_parm(String *s) {
   String *ns;
   ns = NewStringf("%s;",s);
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARM);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   Delete(ns);
   return top;
}


ParmList *Swig_cparse_parms(String *s, Node *file_line_node) {
   String *ns;
   char *cs = Char(s);
   if (cs && cs[0] != '(') {
     ns = NewStringf("(%s);",s);
   } else {
     ns = NewStringf("%s;",s);
   }
   Setfile(ns, Getfile(file_line_node));
   Setline(ns, Getline(file_line_node));
   Seek(ns,0,SEEK_SET);
   scanner_file(ns);
   top = 0;
   scanner_next_token(PARSEPARMS);
   yyparse();
   /*   Printf(stdout,"typeparse: '%s' ---> '%s'\n", s, top); */
   return top;
}


