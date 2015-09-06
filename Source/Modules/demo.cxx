
#include "swigmod.h"

void printNode(Node *n) {
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        printf("%d %s %s\n", i, Char(key), Char(Getattr(n, key)));
    }
}

class DEMO : public Language {

public:

  virtual void main(int /*argc*/, char* /*argv[]*/) {

    printf("I'm the demo module.\n");

    /* Set language-specific subdirectory in SWIG library */
   SWIG_library_directory("demo");

   /* Set language-specific preprocessing symbol */
   Preprocessor_define("SWIGDEMO 1", 0);

   /* Set language-specific configuration file */
   SWIG_config_file("demo.swg");

   /* Set typemap language (historical) */
   SWIG_typemap_lang("demo");
  }

virtual int top(Node *n) {
    printf("Generating code.\n");

    /* Get the module name */
    String *module = Getattr(n, "name");

    printf("module=%s\n", Char(module));

    File *b_runtime = NewString("");
    Swig_register_filebyname("runtime", b_runtime);

    Language::top(n);

    Delete(b_runtime);

   return SWIG_OK;
}

int moduleDirective(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN moduleDirective - node name='%s'.\n", Char(name));
    int ret=Language::moduleDirective(n);
    printf("END   moduleDirective - node name='%s'.\n", Char(name));
    return ret;
}

int classDeclaration(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN classDeclaration - node name='%s'.\n", Char(name));
    int ret=Language::classDeclaration(n);
    printf("END   classDeclaration - node name='%s'.\n", Char(name));
    return ret;
}

int constructorDeclaration(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN constructorDeclaration - node name='%s'.\n", Char(name));
    int ret=Language::constructorDeclaration(n);
    printf("END   constructorDeclaration - node name='%s'.\n", Char(name));
    return ret;
}

int namespaceDeclaration(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN namespaceDeclaration - node name='%s'.\n", Char(name));
    int ret=Language::namespaceDeclaration(n);
    printf("END   namespaceDeclaration - node name='%s'.\n", Char(name));
    return ret;
}

//void processParm(Parm *p) {
//    String *name = Getattr(p, "name");
//    printf("BEGIN processParm - node name='%s'.\n", Char(name));
//    printNode(p);
//
//    SwigType *t1 = Getattr(p, "type");
//    SwigType *t2 = SwigType_base(t1);
//    printf("base = %s\n", Char(SwigType_str(t2, 0)));
//
//    SwigType *t3 = SwigType_str(SwigType_typedef_resolve_all(t1), 0);
//    printf("resolved = %s\n", Char(SwigType_str(t3, 0)));
//
//    printf("is_template=%d\n", SwigType_istemplate(t1));
//
//    String *s1 = SwigType_str(t1, 0);
//    char *c1 = Char(s1);
//    char *c2 = strchr(c1, '<');
//    char *c3 = strchr(c1, '>');
//    char x[100];
//    strncpy(x, c2+1, c3-c2-1);
//    x[c3-c2-1]=0;
//    printf("T=%s\n", x);
//
//    printf("END   processParm - node name='%s'.\n", Char(name));
//}

int functionHandler(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN functionHandler - node name='%s'.\n", Char(name));

    printNode(n);

    printf("**************\n");
    ParmList *parms  = Getattr(n, "parms");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        SwigType *t1 = Getattr(p, "type");
        // prints "t1 = std::vector< int >"
        printf("t1 = %s\n", Char(SwigType_str(t1, 0)));
        // prints "is_template=1"
        printf("is_template=%d\n", SwigType_istemplate(t1));
        // This doesn't compile :(
        //SwigType *t2 = SwigType_pop_template(t1);

        //SwigType *t2 = SwigType_prefix(t1);
        //printf("t2 = %s\n", Char(SwigType_str(t2, 0)));
        //SwigType *t2 = SwigType_base(t1);
        //printf("t2 = %s\n", Char(SwigType_str(t2, 0)));
        //SwigType *t3 = SwigType_str(SwigType_typedef_resolve_all(t2), 0);
        //printf("t3 = %s\n", Char(SwigType_str(t3, 0)));
    }
    printf("**************\n");

    int ret=Language::functionHandler(n);
    printf("END   functionHandler - node name='%s'.\n", Char(name));
    return ret;
}

int memberfunctionHandler(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN memberfunctionHandler - node name='%s'.\n", Char(name));
    int ret=Language::memberfunctionHandler(n);
    printf("END   memberfunctionHandler - node name='%s'.\n", Char(name));
    return ret;
}

int constructorHandler(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN constructorHandler - node name='%s'.\n", Char(name));
    int ret=Language::constructorHandler(n);
    printf("END   constructorHandler - node name='%s'.\n", Char(name));
    return ret;
}

int functionWrapper(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN functionWrapper - node name='%s'.\n", Char(name));
    int ret=Language::functionWrapper(n);
    printf("END   functionWrapper - node name='%s'.\n", Char(name));
    return ret;
}

int classHandler(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN classHandler - node name='%s'.\n", Char(name));
    int ret=Language::classHandler(n);
    printf("END   classHandler - node name='%s'.\n", Char(name));
    return ret;
}

int includeDirective(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN includeDirective - node name='%s'.\n", Char(name));
    int ret=Language::includeDirective(n);
    printf("END   includeDirective - node name='%s'.\n", Char(name));
    return ret;
}

}; // class DEMO

extern "C" Language *
swig_demo(void) {
  return new DEMO();
}
