
#include "swigmod.h"

class DEMO : public Language {

public:

  virtual void main(int argc, char *argv[]) {

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

int functionHandler(Node *n) {
    String *name = Getattr(n, "name");
    printf("BEGIN functionHandler - node name='%s'.\n", Char(name));
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
