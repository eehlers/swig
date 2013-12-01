
#include "swigmod.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

String *prefix = 0;
String *module = 0;

//SwigType *reduceType(SwigType *type) {
//    while (SwigType *t = SwigType_typedef_resolve(type))
//        type = t;
//    return type;
//}

File *initFile(const char *filename) {
   String *outfile = NewStringf(filename);
   File *f = NewFile(outfile, "w", SWIG_output_files());
   if (!f) {
      FileErrorDisplay(outfile);
      SWIG_exit(EXIT_FAILURE);
   }
    return f;
}

struct Buffer {

    // buffers
   File *b_val_cpp;
   File *b_val_hpp;
   File *b_obj_cpp;
   File *b_obj_hpp0;
   File *b_obj_hpp1;
   File *b_cpp_cpp;
   File *b_cpp_hpp;
   File *b_xll_cpp0;
   File *b_xll_cpp1;
   File *b_xll_cpp2;
   File *b_xll_cpp3;

    //String *name_;

    Buffer(/*String *name*/) {

        //name_ = Copy(name);

        b_val_cpp = NewString("");
        b_val_hpp = NewString("");
        b_obj_cpp = NewString("");
        b_obj_hpp0 = NewString("");
        b_obj_hpp1 = NewString("");
        b_cpp_cpp = NewString("");
        b_cpp_hpp = NewString("");
        b_xll_cpp0 = NewString("");
        b_xll_cpp1 = NewString("");
        b_xll_cpp2 = NewString("");
        b_xll_cpp3 = NewString("");

        Printf(b_val_hpp, "\n");
        Printf(b_val_hpp, "#ifndef vo_hpp\n");
        Printf(b_val_hpp, "#define vo_hpp\n");
        Printf(b_val_hpp, "\n");
        Printf(b_val_hpp, "#include <string>\n");
        Printf(b_val_hpp, "#include <set>\n");
        Printf(b_val_hpp, "#include <oh/valueobject.hpp>\n");
        Printf(b_val_hpp, "\n");
        Printf(b_val_hpp,"namespace %s {\n", module);
        Printf(b_val_hpp, "\n");
        Printf(b_val_hpp, "namespace ValueObjects {\n");
        Printf(b_val_hpp, "\n");

        Printf(b_val_cpp, "\n");
        Printf(b_val_cpp, "#include \"vo.hpp\"\n");
        Printf(b_val_cpp, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_val_cpp, "\n");
        Printf(b_val_cpp,"namespace %s {\n", module);
        Printf(b_val_cpp, "\n");
        Printf(b_val_cpp, "namespace ValueObjects {\n");
        Printf(b_val_cpp, "\n");

        Printf(b_obj_hpp0, "\n");
        Printf(b_obj_hpp0, "#ifndef obj_hpp\n");
        Printf(b_obj_hpp0, "#define obj_hpp\n");
        Printf(b_obj_hpp0, "\n");
        Printf(b_obj_hpp0, "#include <string>\n");
        Printf(b_obj_hpp0, "#include <oh/libraryobject.hpp>\n");
        Printf(b_obj_hpp0, "#include <oh/valueobject.hpp>\n");
        Printf(b_obj_hpp0, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_obj_hpp0, "#include \"Library/simplelib.hpp\"\n");

        Printf(b_obj_hpp1,"namespace %s {\n", module);

        Printf(b_obj_cpp, "\n");
        Printf(b_obj_cpp, "#include \"obj.hpp\"\n");
        Printf(b_obj_cpp, "\n");

        Printf(b_cpp_hpp, "#ifndef sla_hpp\n");
        Printf(b_cpp_hpp, "#define sla_hpp\n");
        Printf(b_cpp_hpp, "\n");
        Printf(b_cpp_hpp, "#include <string>\n");
        Printf(b_cpp_hpp, "\n");
        Printf(b_cpp_hpp, "namespace %s {\n", module);
        Printf(b_cpp_hpp, "\n");

        Printf(b_cpp_cpp, "#include \"sla.hpp\"\n");
        Printf(b_cpp_cpp, "#include \"ValueObjects/vo.hpp\"\n");
        Printf(b_cpp_cpp, "#include \"AddinObjects/obj.hpp\"\n");
        Printf(b_cpp_cpp, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_cpp_cpp, "#include <oh/repository.hpp>\n");
        Printf(b_cpp_cpp, "\n");
        Printf(b_cpp_cpp, "static ObjectHandler::Repository repository;\n");

        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "#include <ohxl/objecthandlerxl.hpp>\n");
        Printf(b_xll_cpp0, "#include <ohxl/register/register_all.hpp>\n");
        Printf(b_xll_cpp0, "#include <ohxl/functions/export.hpp>\n");
        Printf(b_xll_cpp0, "#include <ohxl/utilities/xlutilities.hpp>\n");
        Printf(b_xll_cpp0, "#include <ohxl/objectwrapperxl.hpp>\n");
        Printf(b_xll_cpp0, "#include \"ValueObjects/vo.hpp\"\n");
        Printf(b_xll_cpp0, "#include \"AddinObjects/obj.hpp\"\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,\n");
        Printf(b_xll_cpp0, "   for example) also #define _MSC_VER\n");
        Printf(b_xll_cpp0, "*/\n");
        Printf(b_xll_cpp0, "#ifdef BOOST_MSVC\n");
        Printf(b_xll_cpp0, "#  define BOOST_LIB_DIAGNOSTIC\n");
        Printf(b_xll_cpp0, "#  include <oh/auto_link.hpp>\n");
        Printf(b_xll_cpp0, "#  undef BOOST_LIB_DIAGNOSTIC\n");
        Printf(b_xll_cpp0, "#endif\n");
        Printf(b_xll_cpp0, "#include <sstream>\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "DLLEXPORT int xlAutoOpen() {\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "    // Instantiate the ObjectHandler Repository\n");
        Printf(b_xll_cpp0, "    static ObjectHandler::RepositoryXL repositoryXL;\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "    static XLOPER xDll;\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "    try {\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "        Excel(xlGetName, &xDll, 0);\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "        ObjectHandler::Configuration::instance().init();\n");
        Printf(b_xll_cpp0, "\n");
        Printf(b_xll_cpp0, "        registerOhFunctions(xDll);\n");

        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp2, "        return 1;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    } catch (const std::exception &e) {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        std::ostringstream err;\n");
        Printf(b_xll_cpp2, "        err << \"Error loading AddinXlHw: \" << e.what();\n");
        Printf(b_xll_cpp2, "        Excel(xlcAlert, 0, 1, TempStrStl(err.str()));\n");
        Printf(b_xll_cpp2, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp2, "        return 0;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    } catch (...) {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp2, "        return 0;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    }\n");
        Printf(b_xll_cpp2, "}\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "DLLEXPORT int xlAutoClose() {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    static XLOPER xDll;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    try {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        Excel(xlGetName, &xDll, 0);\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        unregisterOhFunctions(xDll);\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp2, "        return 1;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    } catch (const std::exception &e) {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        std::ostringstream err;\n");
        Printf(b_xll_cpp2, "        err << \"Error unloading AddinXlHw: \" << e.what();\n");
        Printf(b_xll_cpp2, "        Excel(xlcAlert, 0, 1, TempStrStl(err.str()));\n");
        Printf(b_xll_cpp2, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp2, "        return 0;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    } catch (...) {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp2, "        return 0;\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    }\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "}\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "DLLEXPORT void xlAutoFree(XLOPER *px) {\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "    freeOper(px);\n");
        Printf(b_xll_cpp2, "\n");
        Printf(b_xll_cpp2, "}\n");

    }

    ~Buffer() {

        Printf(b_val_hpp, "} // namespace %s\n", module);
        Printf(b_val_hpp, "\n");
        Printf(b_val_hpp, "} // namespace ValueObjects\n");
        Printf(b_val_hpp, "\n");
        Printf(b_val_hpp, "#endif\n");
        Printf(b_val_hpp, "\n");

        Printf(b_val_cpp, "\n");
        Printf(b_val_cpp, "} // namespace %s\n", module);
        Printf(b_val_cpp, "\n");
        Printf(b_val_cpp, "} // namespace ValueObjects\n");
        Printf(b_val_cpp, "\n");

        Printf(b_obj_hpp1, "} // namespace %s\n", module);
        Printf(b_obj_hpp1, "\n");
        Printf(b_obj_hpp1, "#endif\n");
        Printf(b_obj_hpp1, "\n");

        Printf(b_cpp_hpp, "\n");
        Printf(b_cpp_hpp, "} // namespace %s\n", module);
        Printf(b_cpp_hpp, "\n");
        Printf(b_cpp_hpp, "#endif\n");
        Printf(b_cpp_hpp, "\n");

        // OH output files
        File *f_val_cpp = initFile("ValueObjects/vo.cpp");
        File *f_val_hpp = initFile("ValueObjects/vo.hpp");
        File *f_obj_cpp = initFile("AddinObjects/obj.cpp");
        File *f_obj_hpp = initFile("AddinObjects/obj.hpp");
        File *f_cpp_cpp = initFile("AddinCpp/sla.cpp");
        File *f_cpp_hpp = initFile("AddinCpp/sla.hpp");
        File *f_xll_cpp = initFile("AddinXl/AddinXl.cpp");

       /* Write all to the file */
        Dump(b_val_cpp, f_val_cpp);
        Dump(b_val_hpp, f_val_hpp);
        Dump(b_obj_cpp, f_obj_cpp);
        Dump(b_obj_hpp0, f_obj_hpp);    // OH #includes
        //Dump(b_header, f_obj_hpp);      // SWIG #includes from the .i file
        Dump(b_obj_hpp1, f_obj_hpp);    // The rest of the file
        Dump(b_cpp_cpp, f_cpp_cpp);
        Dump(b_cpp_hpp, f_cpp_hpp);
        Dump(b_xll_cpp0, f_xll_cpp);
        Dump(b_xll_cpp1, f_xll_cpp);
        Dump(b_xll_cpp2, f_xll_cpp);
        Dump(b_xll_cpp3, f_xll_cpp);
       //Wrapper_pretty_print(b_init, b_begin);

        Delete(b_val_cpp);
        Delete(b_val_hpp);
        Delete(b_obj_cpp);
        Delete(b_obj_hpp0);
        Delete(b_obj_hpp1);
        Delete(b_cpp_cpp);
        Delete(b_cpp_hpp);
        Delete(b_xll_cpp0);
        Delete(b_xll_cpp1);
        Delete(b_xll_cpp2);
        Delete(b_xll_cpp3);

        Delete(f_val_cpp);
        Delete(f_val_hpp);
        Delete(f_obj_cpp);
        Delete(f_obj_hpp);
        Delete(f_cpp_cpp);
        Delete(f_cpp_hpp);
        Delete(f_xll_cpp);
    }
};

// placeholder for upcoming change
struct BufferHash {
    Buffer b;
    Buffer *f(String*) {
        return &b;
    }
};

class OBJECTHANDLER : public Language {

    String *group_;
    BufferHash *bh_;

protected:

    // SWIG buffers
    File *b_begin;
    File *b_runtime;
    File *b_header;
    File *b_wrappers;
    File *b_director;
    File *b_director_h;
    File *b_init;

    // SWIG output files
    File *f_test;

public:

  virtual void main(int argc, char *argv[]) {
    printf("I'm the ObjectHandler module.\n");

    /* Set language-specific subdirectory in SWIG library */
   SWIG_library_directory("objecthandler");

   /* Set language-specific preprocessing symbol */
   Preprocessor_define("SWIGOBJECTHANDLER 1", 0);

   /* Set language-specific configuration file */
   SWIG_config_file("objecthandler.swg");

   /* Set typemap language (historical) */
   SWIG_typemap_lang("objecthandler");

    for (int i = 1; i < argc; i++) {
	if (strcmp(argv[i], "-prefix") == 0) {
	  if (argv[i + 1]) {
	    prefix = NewString(argv[i + 1]);
	    Swig_mark_arg(i);
	    Swig_mark_arg(i + 1);
	    i++;
	  } else {
	    Swig_arg_error();
	  }
	  /* end added */
	}
    }
  }

virtual int top(Node *n) {
    printf("Generating code.\n");

   /* Get the module name */
   module = Getattr(n,"name");

    bh_ = new BufferHash;

   /* Initialize I/O */
    b_runtime = NewString("");
    b_init = NewString("");
    b_header = NewString("");
    b_wrappers = NewString("");
    b_director_h = NewString("");
    b_director = NewString("");
    b_begin = NewString("");

   /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("header", b_header);
    Swig_register_filebyname("wrapper", b_wrappers);
    Swig_register_filebyname("begin", b_begin);
    Swig_register_filebyname("runtime", b_runtime);
    Swig_register_filebyname("init", b_init);
    Swig_register_filebyname("director", b_director);
    Swig_register_filebyname("director_h", b_director_h);

   /* Output module initialization code */
   Swig_banner(b_begin);

   /* Emit code for children */
   Language::top(n);

    // To help with troubleshooting, create an output file to which all of the
    // SWIG buffers will be written.  We are not going to compile this file but
    // we give it a cpp extension so that the editor will apply syntax
    // highlighting.
    f_test = initFile("test.cpp");

    // Write all of the SWIG buffers to the dummy output file.
    Printf(f_test, "//**********begin b_begin\n");
    Dump(b_begin, f_test);
    Printf(f_test, "//**********end b_begin\n");
    Printf(f_test, "//**********begin b_runtime\n");
    Dump(b_runtime, f_test);
    Printf(f_test, "//**********end b_runtime\n");
    Printf(f_test, "//**********begin b_header\n");
    Dump(b_header, f_test);
    Printf(f_test, "//**********end b_header\n");
    Printf(f_test, "//**********begin b_wrappers\n");
    Dump(b_wrappers, f_test);
    Printf(f_test, "//**********end b_wrappers\n");
    Printf(f_test, "//**********begin b_director\n");
    Dump(b_director, f_test);
    Printf(f_test, "//**********end b_director\n");
    Printf(f_test, "//**********begin b_director_h\n");
    Dump(b_director_h, f_test);
    Printf(f_test, "//**********end b_director_h\n");
    Printf(f_test, "//**********begin b_init\n");
    Dump(b_init, f_test);
    Printf(f_test, "//**********end b_init\n");

   /* Cleanup files */
    Delete(b_header);
    Delete(b_wrappers);
    Delete(b_init);
    Delete(b_director);
    Delete(b_director_h);
    Delete(b_runtime);
    Delete(b_begin);
    //Close(f_test);
    Delete(f_test);

    delete bh_;

   return SWIG_OK;
  }

void printNode(Node *n) {
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        Printf(b_wrappers,"/* %d %s %s */\n", i, key, Getattr(n, key));
    }
}

void printList(Node *n) {
    while (n) {
        printNode(n);
        n = Getattr(n,"nextSibling");
    }
}

// "double d, string s"
void emitParmList(ParmList *parms, File *buf, bool deref = false) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        //if (Getattr(p, "hidden")) continue;
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        SwigType *type  = Getattr(p,"type");
        String   *name  = Getattr(p,"name");
        //type = reduceType(type);
        if (deref)
            Printf(buf, "%s *%s", type, name);
        else
            Printf(buf, "%s %s", type, name);
    }
}

// "d, s"
void emitParmList2(ParmList *parms, File *buf, bool deref = false) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (Getattr(p, "hidden")) continue;
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        String *name  = Getattr(p,"name");
        if (deref)
            Printf(buf, "*%s", name);
        else
            Printf(buf, "%s", name);
    }
}

std::string f(String *c) {
    std::stringstream s;
    s << std::hex << std::setw(2) << std::setfill('0') << Len(c);
    return s.str();
}

String *getType(const char *typemapname, Node *n, SwigType *type) {
    //type = reduceType(type);
    String *tm = Swig_typemap_lookup(typemapname, n, type, 0);
    if (!tm) {
        printf("No \"%s\" typemap for type \"%s\".\n", typemapname, Char(SwigType_str(type, 0)));
        SWIG_exit(EXIT_FAILURE);
    }
    return tm;
}

String *f2(Node *n, SwigType *type, ParmList *parms) {
    String *s = NewString("");
    if (type) {
        String *tm = getType("excel", n, type);
        Append(s, tm);
    }
    for (Parm *p = parms; p; p = nextSibling(p)) {
        //if (Getattr(p, "hidden")) continue;
        SwigType *t  = Getattr(p, "type");
        String *tm = getType("excel", p, t);
        Append(s, tm);
    }
    Append(s, "#");
    return s;
}

String *f3(ParmList *parms) {
    String *s = NewString("");
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        //if (Getattr(p, "hidden")) continue;
        if (first) {
            first = false;
        } else {
            Append(s, ",");
        }
        String *name = Getattr(p,"name");
        Append(s, name);
    }
    return s;
}

void f4(Node *n, SwigType *type, ParmList *parms) {
    String *funcName   = Getattr(n, "oh:funcName");
    Printf(bh_->f(group_)->b_xll_cpp1, "\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "        Excel(xlfRegister, 0, 7, &xDll,\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "            // function code name\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(funcName).c_str(), funcName);
    Printf(bh_->f(group_)->b_xll_cpp1, "            // parameter codes\n");
    String *x = f2(n, type, parms);
    Printf(bh_->f(group_)->b_xll_cpp1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(x).c_str(), x);
    Printf(bh_->f(group_)->b_xll_cpp1, "            // function display name\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(funcName).c_str(), funcName);
    Printf(bh_->f(group_)->b_xll_cpp1, "            // comma-delimited list of parameters\n");
    String *x2 = f3(parms);
    Printf(bh_->f(group_)->b_xll_cpp1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(x2).c_str(), x2);
    Printf(bh_->f(group_)->b_xll_cpp1, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "            TempStrNoSize(\"\\x01\"\"1\"),\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "            // function category\n");
    Printf(bh_->f(group_)->b_xll_cpp1, "            TempStrNoSize(\"\\x07\"\"Example\"));\n");
}

String *copyUpper(String *s) {
    String *ret = Copy(s);
    char *c = Char(ret);
    c[0] = toupper(c[0]);
    return ret;
}

void printFunc(Node *n) {
    Printf(bh_->f(group_)->b_cpp_cpp,"//****FUNC*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    String   *symname   = Getattr(n,"sym:name");
    //String   *action = Getattr(n,"wrap:action");

    String *temp = copyUpper(symname);
    String *funcName = NewStringf("%s%s", prefix, temp);
    //Setattr(n, "oh:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    Printf(bh_->f(group_)->b_obj_hpp1,"\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"    %s %s(", type, symname);
    emitParmList(parms, bh_->f(group_)->b_obj_hpp1);
    Printf(bh_->f(group_)->b_obj_hpp1,");\n");

    Printf(bh_->f(group_)->b_obj_cpp,"%s %s::%s(", type, module, symname);
    emitParmList(parms, bh_->f(group_)->b_obj_cpp);
    Printf(bh_->f(group_)->b_obj_cpp,") {\n");
    Printf(bh_->f(group_)->b_obj_cpp,"    return %s(", name);
    emitParmList2(parms, bh_->f(group_)->b_obj_cpp);
    Printf(bh_->f(group_)->b_obj_cpp,");\n");
    Printf(bh_->f(group_)->b_obj_cpp,"}\n");

    Printf(bh_->f(group_)->b_cpp_hpp,"    %s %s(", type, funcName);
    emitParmList(parms, bh_->f(group_)->b_cpp_hpp);
    Printf(bh_->f(group_)->b_cpp_hpp,");\n");

    Printf(bh_->f(group_)->b_cpp_cpp,"%s %s::%s(", type, module, funcName);
    emitParmList(parms, bh_->f(group_)->b_cpp_cpp);
    Printf(bh_->f(group_)->b_cpp_cpp,") {\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"    return %s(", name);
    emitParmList2(parms, bh_->f(group_)->b_cpp_cpp);
    Printf(bh_->f(group_)->b_cpp_cpp,");\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"}\n");

    f4(n, type, parms);

    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "DLLEXPORT %s *%s(", type, funcName);
    emitParmList(parms, bh_->f(group_)->b_xll_cpp3, true);
    Printf(bh_->f(group_)->b_xll_cpp3, ") {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    try {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        %s returnValue =\n", type);
    Printf(bh_->f(group_)->b_xll_cpp3, "            %s::%s(", module, symname);
    emitParmList2(parms, bh_->f(group_)->b_xll_cpp3, true);
    Printf(bh_->f(group_)->b_xll_cpp3, ");\n");

    String *tm = getType("ohxl_ret", n, type);
    Printf(bh_->f(group_)->b_xll_cpp3, Char(tm));

    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    } catch (const std::exception &e) {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        return 0;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    }\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "}\n");
}

void printMemb(Node *n) {
    Printf(bh_->f(group_)->b_cpp_cpp,"//****MEMB*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    Node *p = Getattr(n,"parentNode");
    String   *cls   = Getattr(p,"sym:name");
    String   *pname   = Getattr(p,"name");
    ParmList *parms  = Getattr(n,"parms");

    String *temp0 = copyUpper(cls);
    String *temp1 = copyUpper(name);
    String *funcName = NewStringf("%s%s%s", prefix, temp0, temp1);
    //Setattr(n, "oh:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    Parm *parms2 = NewHash();
    Setattr(parms2, "name", "objectID");
    String *nt  = NewStringf("std::string");
    SwigType_add_qualifier(nt, "const");
    SwigType_add_reference(nt);
    Setattr(parms2, "type", nt);
    Setattr(parms2, "nextSibling", Getattr(parms, "nextSibling"));

    Printf(b_wrappers, "//***ABC\n");
    printList(parms2);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(parms2)));
    Printf(b_wrappers, "//***ABC\n");

    //type = reduceType(type);
    Printf(bh_->f(group_)->b_cpp_hpp,"    %s %s(%s);\n", type, funcName, Char(ParmList_str(parms2)));

    Printf(bh_->f(group_)->b_cpp_cpp,"%s %s::%s(%s) {\n", type, module, funcName, Char(ParmList_str(parms2)));
    Printf(bh_->f(group_)->b_cpp_cpp,"    OH_GET_REFERENCE(x, objectID, %s::%s, %s);\n", module, cls, pname);
    Printf(bh_->f(group_)->b_cpp_cpp,"    return x->%s(", name);
    emitParmList2(parms, bh_->f(group_)->b_cpp_cpp);
    Printf(bh_->f(group_)->b_cpp_cpp,");\n", name);
    Printf(bh_->f(group_)->b_cpp_cpp,"}\n");

    f4(n, 0, parms2);

    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "DLLEXPORT %s *%s(", type, funcName);
    emitParmList(parms2, bh_->f(group_)->b_xll_cpp3);
    Printf(bh_->f(group_)->b_xll_cpp3, ") {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    try {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        OH_GET_REFERENCE(x, objectID, %s::%s, %s);\n", module, cls, pname);
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        static %s ret;\n", type);
    Printf(bh_->f(group_)->b_xll_cpp3, "        ret = x->%s(", name);
    emitParmList2(parms, bh_->f(group_)->b_xll_cpp3, true);
    Printf(bh_->f(group_)->b_xll_cpp3, ");\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        return &ret;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    } catch (const std::exception &e) {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        return 0;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    }\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "}\n");
}

void printCtor(Node *n) {
    Printf(bh_->f(group_)->b_cpp_cpp,"//****CTOR*****\n");
    String   *name   = Getattr(n,"name");
    //SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    Node *p = Getattr(n,"parentNode");
    String *pname   = Getattr(p,"name");

    String *temp = copyUpper(name);
    String *funcName = NewStringf("%s%s", prefix, temp);
    //Setattr(n, "oh:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    Parm *parms2 = NewHash();
    Setattr(parms2, "name", "objectID");
    String *nt  = NewStringf("std::string");
    SwigType_add_qualifier(nt, "const");
    SwigType_add_reference(nt);
    Setattr(parms2, "type", nt);
    Setattr(parms2, "nextSibling", parms);

    Printf(b_wrappers, "//***DEF\n");
    printList(parms2);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(parms2)));
    Printf(b_wrappers, "//***DEF\n");

    //type = reduceType(type);

    Printf(bh_->f(group_)->b_val_hpp,"        class %s : public ObjectHandler::ValueObject {\n", funcName);
    Printf(bh_->f(group_)->b_val_hpp,"            friend class boost::serialization::access;\n");
    Printf(bh_->f(group_)->b_val_hpp,"        public:\n");
    Printf(bh_->f(group_)->b_val_hpp,"            %s() {}\n", funcName);
    Printf(bh_->f(group_)->b_val_hpp,"            %s(\n", funcName);
    Printf(bh_->f(group_)->b_val_hpp,"                const std::string& ObjectId,\n");
    Printf(bh_->f(group_)->b_val_hpp,"                bool Permanent);\n");
    Printf(bh_->f(group_)->b_val_hpp,"\n");
    Printf(bh_->f(group_)->b_val_hpp,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
    Printf(bh_->f(group_)->b_val_hpp,"            std::vector<std::string> getPropertyNamesVector() const;\n");
    Printf(bh_->f(group_)->b_val_hpp,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
    Printf(bh_->f(group_)->b_val_hpp,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
    Printf(bh_->f(group_)->b_val_hpp,"\n");
    Printf(bh_->f(group_)->b_val_hpp,"        protected:\n");
    Printf(bh_->f(group_)->b_val_hpp,"            static const char* mPropertyNames[];\n");
    Printf(bh_->f(group_)->b_val_hpp,"            static std::set<std::string> mSystemPropertyNames;\n");
    Printf(bh_->f(group_)->b_val_hpp,"            bool Permanent_;\n");
    Printf(bh_->f(group_)->b_val_hpp,"\n");
    Printf(bh_->f(group_)->b_val_hpp,"            template<class Archive>\n");
    Printf(bh_->f(group_)->b_val_hpp,"            void serialize(Archive& ar, const unsigned int) {\n");
    Printf(bh_->f(group_)->b_val_hpp,"            boost::serialization::void_cast_register<%s, ObjectHandler::ValueObject>(this, this);\n", funcName);
    Printf(bh_->f(group_)->b_val_hpp,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
    Printf(bh_->f(group_)->b_val_hpp,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
    Printf(bh_->f(group_)->b_val_hpp,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
    Printf(bh_->f(group_)->b_val_hpp,"            }\n");
    Printf(bh_->f(group_)->b_val_hpp,"        };\n");
    Printf(bh_->f(group_)->b_val_hpp,"\n");

    Printf(bh_->f(group_)->b_val_cpp,"        const char* %s::mPropertyNames[] = {\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            \"Permanent\"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        };\n");
    Printf(bh_->f(group_)->b_val_cpp,"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        std::set<std::string> %s::mSystemPropertyNames(\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(bh_->f(group_)->b_val_cpp,"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            return mSystemPropertyNames;\n");
    Printf(bh_->f(group_)->b_val_cpp,"        }\n");
    Printf(bh_->f(group_)->b_val_cpp,"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            std::vector<std::string> ret(\n");
    Printf(bh_->f(group_)->b_val_cpp,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(bh_->f(group_)->b_val_cpp,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
    Printf(bh_->f(group_)->b_val_cpp,"                i != userProperties.end(); ++i)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                ret.push_back(i->first);\n");
    Printf(bh_->f(group_)->b_val_cpp,"            return ret;\n");
    Printf(bh_->f(group_)->b_val_cpp,"        }\n");
    Printf(bh_->f(group_)->b_val_cpp,"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        ObjectHandler::property_t %s::getSystemProperty(const std::string& name) const {\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(bh_->f(group_)->b_val_cpp,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                return objectId_;\n");
    Printf(bh_->f(group_)->b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                return className_;\n");
    Printf(bh_->f(group_)->b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                return Permanent_;\n");
    Printf(bh_->f(group_)->b_val_cpp,"            else\n");
    Printf(bh_->f(group_)->b_val_cpp,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
    Printf(bh_->f(group_)->b_val_cpp,"        }\n");
    Printf(bh_->f(group_)->b_val_cpp,"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        void %s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(bh_->f(group_)->b_val_cpp,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                objectId_ = boost::get<std::string>(value);\n");
    Printf(bh_->f(group_)->b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                className_ = boost::get<std::string>(value);\n");
    Printf(bh_->f(group_)->b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(bh_->f(group_)->b_val_cpp,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
    Printf(bh_->f(group_)->b_val_cpp,"            else\n");
    Printf(bh_->f(group_)->b_val_cpp,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
    Printf(bh_->f(group_)->b_val_cpp,"        }\n");
    Printf(bh_->f(group_)->b_val_cpp,"\n");
    Printf(bh_->f(group_)->b_val_cpp,"        %s::%s(\n", funcName, funcName);
    Printf(bh_->f(group_)->b_val_cpp,"                const std::string& ObjectId,\n");
    Printf(bh_->f(group_)->b_val_cpp,"                bool Permanent) :\n");
    Printf(bh_->f(group_)->b_val_cpp,"            ObjectHandler::ValueObject(ObjectId, \"%s\", Permanent),\n", funcName);
    Printf(bh_->f(group_)->b_val_cpp,"            Permanent_(Permanent) {\n");
    Printf(bh_->f(group_)->b_val_cpp,"        }\n");

    Printf(bh_->f(group_)->b_obj_hpp1,"\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"    class %s : \n", name);
    Printf(bh_->f(group_)->b_obj_hpp1,"        public ObjectHandler::LibraryObject<%s> {\n", pname);
    Printf(bh_->f(group_)->b_obj_hpp1,"    public:\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"        %s(\n", name);
    Printf(bh_->f(group_)->b_obj_hpp1,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,\n");
    emitParmList(parms, bh_->f(group_)->b_obj_hpp1);
    Printf(bh_->f(group_)->b_obj_hpp1,", bool permanent)\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"        : ObjectHandler::LibraryObject<%s>(properties, permanent) {\n", pname);
    Printf(bh_->f(group_)->b_obj_hpp1,"            libraryObject_ = boost::shared_ptr<%s>(new %s(", pname, pname);
    emitParmList2(parms, bh_->f(group_)->b_obj_hpp1);
    Printf(bh_->f(group_)->b_obj_hpp1,"));\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"        }\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"    };\n");
    Printf(bh_->f(group_)->b_obj_hpp1,"\n");

    Printf(bh_->f(group_)->b_cpp_hpp,"    std::string %s(%s);\n", funcName, Char(ParmList_str(parms2)));

    Printf(bh_->f(group_)->b_cpp_cpp,"std::string %s::%s(%s) {\n", module, funcName, Char(ParmList_str(parms2)));
    Printf(bh_->f(group_)->b_cpp_cpp,"    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"        new %s::ValueObjects::%s(\n", module, funcName);
    Printf(bh_->f(group_)->b_cpp_cpp,"            objectID, false));\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"    boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"        new %s::%s(\n", module, name);
    Printf(bh_->f(group_)->b_cpp_cpp,"            valueObject, ");
    emitParmList2(parms, bh_->f(group_)->b_cpp_cpp);
    Printf(bh_->f(group_)->b_cpp_cpp,", false));\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"    std::string returnValue =\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"        ObjectHandler::Repository::instance().storeObject(\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"            objectID, object, false, valueObject);\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"    return returnValue;\n");
    Printf(bh_->f(group_)->b_cpp_cpp,"}\n");

    f4(n, 0, parms);

    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "DLLEXPORT char *%s(", funcName);
    emitParmList(parms, bh_->f(group_)->b_xll_cpp3, true);
    Printf(bh_->f(group_)->b_xll_cpp3, ") {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    try {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "            new %s::ValueObjects::%s(objectID, false));\n", module, funcName);
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "            new %s::%s(valueObject,", module, name);
    emitParmList2(parms2, bh_->f(group_)->b_xll_cpp3, true);
    Printf(bh_->f(group_)->b_xll_cpp3, "));\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        std::string returnValue =\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "            ObjectHandler::RepositoryXL::instance().storeObject(objectID, object, true);\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        static char ret[XL_MAX_STR_LEN];\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        ObjectHandler::stringToChar(returnValue, ret);\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        return ret;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    } catch (const std::exception &e) {\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "        return 0;\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "    }\n");
    Printf(bh_->f(group_)->b_xll_cpp3, "}\n");
}

int functionWrapper(Node *n) {
    group_ = Getattr(n,"feature:oh:group");
    Printf(b_wrappers,"//XXX***functionWrapper*******\n");
    Printf(b_wrappers,"//module=%s\n", module);
    Printf(b_wrappers,"//group_=%s\n", Char(group_));
    printNode(n);
    printList(Getattr(n, "parms"));
    Printf(b_wrappers,"//*************\n");

  String   *nodeType   = Getattr(n,"nodeType");
    if (0 == Strcmp("cdecl", nodeType)) {
        if (NULL == Getattr(n, "ismember")) {
            printFunc(n);
        } else {
            printMemb(n);
        }
    } else if (0 == Strcmp("constructor", nodeType)) {
            printCtor(n);
    }

  return SWIG_OK;
}

}; // class OBJECTHANDLER

extern "C" Language *
swig_objecthandler(void) {
  return new OBJECTHANDLER();
}

