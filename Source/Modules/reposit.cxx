
#include "swigmod.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

String *prefix = 0;
String *module = 0;

//SwigType *reduceType(SwigType *type) {
//    while (SwigType *t = SwigType_typedef_resolve(type))
//        type = t;
//    return type;
//}

File *initFile(String *outfile) {
   File *f = NewFile(outfile, "w", SWIG_output_files());
   if (!f) {
      FileErrorDisplay(outfile);
      SWIG_exit(EXIT_FAILURE);
   }
    return f;
}

struct Buffer {
    String *name_;
    File *b;
    Buffer(String *name) {
        name_ = Copy(name);
        b = NewString("");
    }
    ~Buffer() {
        File *f = initFile(name_);
        Delete(name_);
        Dump(b, f);
        Delete(b);
        Delete(f);
    }
};

struct BufferGroup {

    // buffers
    Buffer *b_val_cpp;
    Buffer *b_val_hpp;
    Buffer *b_obj_cpp;
    Buffer *b_obj_hpp;
    Buffer *b_cpp_cpp;
    Buffer *b_cpp_hpp;
    Buffer *b_xll_cpp;

    String *name_;
    bool manual_;

    BufferGroup(String *name, String *include, bool manual) {

        name_ = Copy(name);
        manual_ = manual;

        String *s_val_cpp = NewStringf("ValueObjects/vo_%s.cpp", name_);
        String *s_val_hpp = NewStringf("ValueObjects/vo_%s.hpp", name_);
        String *s_cpp_cpp = NewStringf("AddinCpp/cpp_%s.cpp", name_);
        String *s_cpp_hpp = NewStringf("AddinCpp/cpp_%s.hpp", name_);
        String *s_xll_cpp = NewStringf("AddinXl/xl_%s.cpp", name_);

        b_val_cpp = new Buffer(s_val_cpp);
        b_val_hpp = new Buffer(s_val_hpp);
        b_cpp_cpp = new Buffer(s_cpp_cpp);
        b_cpp_hpp = new Buffer(s_cpp_hpp);
        b_xll_cpp = new Buffer(s_xll_cpp);

        Delete(s_val_cpp);
        Delete(s_val_hpp);
        Delete(s_cpp_cpp);
        Delete(s_cpp_hpp);
        Delete(s_xll_cpp);

        if (!manual_) {
            String *s_obj_cpp = NewStringf("AddinObjects/obj_%s.cpp", name_);
            String *s_obj_hpp = NewStringf("AddinObjects/obj_%s.hpp", name_);
            b_obj_cpp = new Buffer(s_obj_cpp);
            b_obj_hpp = new Buffer(s_obj_hpp);
            Delete(s_obj_cpp);
            Delete(s_obj_hpp);
        }

        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "#ifndef vo_%s_hpp\n", name);
        Printf(b_val_hpp->b, "#define vo_%s_hpp\n", name);
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "#include <string>\n");
        Printf(b_val_hpp->b, "#include <set>\n");
        Printf(b_val_hpp->b, "#include <oh/valueobject.hpp>\n");
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b,"namespace %s {\n", module);
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "namespace ValueObjects {\n");
        Printf(b_val_hpp->b, "\n");

        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b, "#include \"vo_%s.hpp\"\n", name);
        Printf(b_val_cpp->b, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b,"namespace %s {\n", module);
        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b, "namespace ValueObjects {\n");
        Printf(b_val_cpp->b, "\n");

        if (!manual_) {
        Printf(b_obj_hpp->b, "\n");
        Printf(b_obj_hpp->b, "#ifndef obj_%s_hpp\n", name);
        Printf(b_obj_hpp->b, "#define obj_%s_hpp\n", name);
        Printf(b_obj_hpp->b, "\n");
        Printf(b_obj_hpp->b, "#include <string>\n");
        Printf(b_obj_hpp->b, "#include <oh/libraryobject.hpp>\n");
        Printf(b_obj_hpp->b, "#include <oh/valueobject.hpp>\n");
        Printf(b_obj_hpp->b, "#include <boost/shared_ptr.hpp>");
        Printf(b_obj_hpp->b, "%s\n", include);
        Printf(b_obj_hpp->b,"namespace %s {\n", module);

        Printf(b_obj_cpp->b, "\n");
        Printf(b_obj_cpp->b, "#include \"obj_%s.hpp\"\n", name);
        Printf(b_obj_cpp->b, "\n");
        }

        Printf(b_cpp_hpp->b, "\n");
        Printf(b_cpp_hpp->b, "#ifndef cpp_%s_hpp\n", name);
        Printf(b_cpp_hpp->b, "#define cpp_%s_hpp\n", name);
        Printf(b_cpp_hpp->b, "\n");
        Printf(b_cpp_hpp->b, "#include <string>\n");
        // FIXME this #include is only needed if a datatype conversion is taking place.
        Printf(b_cpp_hpp->b, "#include <oh/property.hpp>\n");
        Printf(b_cpp_hpp->b, "\n");
        Printf(b_cpp_hpp->b, "namespace %s {\n", module);
        Printf(b_cpp_hpp->b, "\n");

        Printf(b_cpp_cpp->b, "\n");
        Printf(b_cpp_cpp->b, "#include \"cpp_%s.hpp\"\n", name);
        // FIXME this #include is only required if the file contains conversions.
        Printf(b_cpp_cpp->b, "#include \"convert2.hpp\"\n");
        // FIXME this #include is only required if the file contains enumerations.
        Printf(b_cpp_cpp->b, "#include <oh/enumerations/typefactory.hpp>\n");
        // FIXME this #include is only required if the file contains constructors.
        Printf(b_cpp_cpp->b, "#include \"ValueObjects/vo_%s.hpp\"\n", name);
        Printf(b_cpp_cpp->b, "#include \"AddinObjects/obj_%s.hpp\"\n", name);
        Printf(b_cpp_cpp->b, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_cpp_cpp->b, "#include <oh/repository.hpp>\n");
        Printf(b_cpp_cpp->b, "\n");

        Printf(b_xll_cpp->b, "\n");
        Printf(b_xll_cpp->b, "#include <ohxl/objecthandlerxl.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/register/register_all.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/functions/export.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/utilities/xlutilities.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/objectwrapperxl.hpp>\n");
        Printf(b_xll_cpp->b, "#include \"ValueObjects/vo_%s.hpp\"\n", name);
        Printf(b_xll_cpp->b, "#include \"AddinObjects/obj_%s.hpp\"\n", name);
        Printf(b_xll_cpp->b, "\n");
        Printf(b_xll_cpp->b, "/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,\n");
        Printf(b_xll_cpp->b, "   for example) also #define _MSC_VER\n");
        Printf(b_xll_cpp->b, "*/\n");
        Printf(b_xll_cpp->b, "#ifdef BOOST_MSVC\n");
        Printf(b_xll_cpp->b, "#  define BOOST_LIB_DIAGNOSTIC\n");
        Printf(b_xll_cpp->b, "#  include <oh/auto_link.hpp>\n");
        Printf(b_xll_cpp->b, "#  undef BOOST_LIB_DIAGNOSTIC\n");
        Printf(b_xll_cpp->b, "#endif\n");
        Printf(b_xll_cpp->b, "#include <sstream>\n");
        Printf(b_xll_cpp->b, "\n");
    }

    ~BufferGroup() {
        Printf(b_val_hpp->b, "} // namespace %s\n", module);
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "} // namespace ValueObjects\n");
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "#endif\n");
        Printf(b_val_hpp->b, "\n");

        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b, "} // namespace %s\n", module);
        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b, "} // namespace ValueObjects\n");
        Printf(b_val_cpp->b, "\n");

        if (!manual_) {
        Printf(b_obj_hpp->b, "} // namespace %s\n", module);
        Printf(b_obj_hpp->b, "\n");
        Printf(b_obj_hpp->b, "#endif\n");
        Printf(b_obj_hpp->b, "\n");
        }

        Printf(b_cpp_hpp->b, "\n");
        Printf(b_cpp_hpp->b, "} // namespace %s\n", module);
        Printf(b_cpp_hpp->b, "\n");
        Printf(b_cpp_hpp->b, "#endif\n");
        Printf(b_cpp_hpp->b, "\n");

        Printf(b_cpp_cpp->b, "\n");

        delete b_val_cpp;
        delete b_val_hpp;
        if (!manual_) {
        delete b_obj_cpp;
        delete b_obj_hpp;
        }
        delete b_cpp_cpp;
        delete b_cpp_hpp;
        delete b_xll_cpp;
    }
};

class BufferMap {

    typedef std::map<std::string, BufferGroup*> BM;
    BM bm_;
    std::string name_;

public:

    void init(String *name, String *include, bool manual) {
        name_ = Char(name);
        if (bm_.end() == bm_.find(name_))
            bm_[name_] = new BufferGroup(name, include, manual);
    }

    BufferGroup *f() {
        return bm_[name_];
    }

    ~BufferMap() {
        for (BM::const_iterator i=bm_.begin(); i!=bm_.end(); ++i)
            delete i->second;
    }
};

class REPOSIT : public Language {

    BufferMap bm_;

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

    Buffer *b_xll_cpp4;

public:

  virtual void main(int argc, char *argv[]) {
    printf("I'm the reposit module.\n");

    /* Set language-specific subdirectory in SWIG library */
   SWIG_library_directory("reposit");

   /* Set language-specific preprocessing symbol */
   Preprocessor_define("SWIGREPOSIT 1", 0);

   /* Set language-specific configuration file */
   SWIG_config_file("reposit.swg");

   /* Set typemap language (historical) */
   SWIG_typemap_lang("reposit");

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

    String *s_xll_cpp4 = NewString("AddinXl/xl_addin.cpp");
    b_xll_cpp4 = new Buffer(s_xll_cpp4);
    Delete(s_xll_cpp4);

        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "#include <ohxl/objecthandlerxl.hpp>\n");
        Printf(b_xll_cpp4->b, "#include <ohxl/register/register_all.hpp>\n");
        Printf(b_xll_cpp4->b, "#include <ohxl/functions/export.hpp>\n");
        Printf(b_xll_cpp4->b, "#include <ohxl/utilities/xlutilities.hpp>\n");
        Printf(b_xll_cpp4->b, "#include <ohxl/objectwrapperxl.hpp>\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,\n");
        Printf(b_xll_cpp4->b, "   for example) also #define _MSC_VER\n");
        Printf(b_xll_cpp4->b, "*/\n");
        Printf(b_xll_cpp4->b, "#ifdef BOOST_MSVC\n");
        Printf(b_xll_cpp4->b, "#  define BOOST_LIB_DIAGNOSTIC\n");
        Printf(b_xll_cpp4->b, "#  include <oh/auto_link.hpp>\n");
        Printf(b_xll_cpp4->b, "#  undef BOOST_LIB_DIAGNOSTIC\n");
        Printf(b_xll_cpp4->b, "#endif\n");
        Printf(b_xll_cpp4->b, "#include <sstream>\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "DLLEXPORT int xlAutoOpen() {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    // Instantiate the ObjectHandler Repository\n");
        Printf(b_xll_cpp4->b, "    static ObjectHandler::RepositoryXL repositoryXL;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    static XLOPER xDll;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    try {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        Excel(xlGetName, &xDll, 0);\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        ObjectHandler::Configuration::instance().init();\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        registerOhFunctions(xDll);\n");

   /* Output module initialization code */
   Swig_banner(b_begin);

   /* Emit code for children */
   Language::top(n);

        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp4->b, "        return 1;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    } catch (const std::exception &e) {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        std::ostringstream err;\n");
        Printf(b_xll_cpp4->b, "        err << \"Error loading AddinXl: \" << e.what();\n");
        Printf(b_xll_cpp4->b, "        Excel(xlcAlert, 0, 1, TempStrStl(err.str()));\n");
        Printf(b_xll_cpp4->b, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp4->b, "        return 0;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    } catch (...) {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp4->b, "        return 0;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    }\n");
        Printf(b_xll_cpp4->b, "}\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "DLLEXPORT int xlAutoClose() {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    static XLOPER xDll;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    try {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        Excel(xlGetName, &xDll, 0);\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        unregisterOhFunctions(xDll);\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp4->b, "        return 1;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    } catch (const std::exception &e) {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        std::ostringstream err;\n");
        Printf(b_xll_cpp4->b, "        err << \"Error unloading AddinXl: \" << e.what();\n");
        Printf(b_xll_cpp4->b, "        Excel(xlcAlert, 0, 1, TempStrStl(err.str()));\n");
        Printf(b_xll_cpp4->b, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp4->b, "        return 0;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    } catch (...) {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "        Excel(xlFree, 0, 1, &xDll);\n");
        Printf(b_xll_cpp4->b, "        return 0;\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    }\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "}\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "DLLEXPORT void xlAutoFree(XLOPER *px) {\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "    freeOper(px);\n");
        Printf(b_xll_cpp4->b, "\n");
        Printf(b_xll_cpp4->b, "}\n");

    delete b_xll_cpp4;

    // To help with troubleshooting, create an output file to which all of the
    // SWIG buffers will be written.  We are not going to compile this file but
    // we give it a cpp extension so that the editor will apply syntax
    // highlighting.
    String *s_test = NewString("test.cpp");
    f_test = initFile(s_test);
    Delete(s_test);

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

String *getType(const char *typemapname, Node *n, SwigType *type) {
    //type = reduceType(type);
    String *tm = Swig_typemap_lookup(typemapname, n, type, 0);
    if (!tm) {
        printf("No \"%s\" typemap for type \"%s\".\n", typemapname, Char(SwigType_str(type, 0)));
        SWIG_exit(EXIT_FAILURE);
    }
    return tm;
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
        //if (deref)
        //    Printf(buf, "%s *%s", type, name);
        //else
        //    Printf(buf, "%s %s", type, name);
        Printf(buf, "%s", Char(SwigType_str(type, name)));
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

// "double d, string s"
void emitParmList3(ParmList *parms, File *buf) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (first) {
            first = false;
        } else {
            Append(buf, ", ");
        }
        SwigType *type  = Getattr(p, "type");
        String *tm = getType("excel_in", p, type);
        String *name  = Getattr(p,"name");
        Printf(buf, "%s %s", tm, name);
    }
}

void emitParmList4(ParmList *parms, File *buf) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        SwigType *type  = Getattr(p,"type");
        String *tm = Swig_typemap_lookup("cpp_in", p, type, 0);
        if (!tm)
            tm = SwigType_str(type, 0);
        String   *name  = Getattr(p,"name");
        Printf(buf, "%s %s", tm, name);
    }
}

void emitParmList5(ParmList *parms, File *buf) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        SwigType *type  = Getattr(p,"type");
        String *tm = Swig_typemap_lookup("cpp_cnv", p, type, 0);
        if (!tm)
            continue;
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        Printf(buf, "%s", tm);
    }
}

void emitParmList6(ParmList *parms, File *buf) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (Getattr(p, "hidden")) continue;
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        SwigType *type  = Getattr(p,"type");
        String *tm = Swig_typemap_lookup("cpp_call", p, type, 0);
        if (!tm)
            tm  = Getattr(p,"name");
        Printf(buf, "%s", tm);
    }
}

std::string f(String *c) {
    std::stringstream s;
    s << std::hex << std::setw(2) << std::setfill('0') << Len(c);
    return s.str();
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
        if (Getattr(p, "hidden")) continue;
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
    String *funcName   = Getattr(n, "rp:funcName");
    Printf(b_xll_cpp4->b, "\n");
    Printf(b_xll_cpp4->b, "        Excel(xlfRegister, 0, 7, &xDll,\n");
    Printf(b_xll_cpp4->b, "            // function code name\n");
    Printf(b_xll_cpp4->b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(funcName).c_str(), funcName);
    Printf(b_xll_cpp4->b, "            // parameter codes\n");
    String *x = f2(n, type, parms);
    Printf(b_xll_cpp4->b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(x).c_str(), x);
    Printf(b_xll_cpp4->b, "            // function display name\n");
    Printf(b_xll_cpp4->b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(funcName).c_str(), funcName);
    Printf(b_xll_cpp4->b, "            // comma-delimited list of parameters\n");
    String *x2 = f3(parms);
    Printf(b_xll_cpp4->b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", f(x2).c_str(), x2);
    Printf(b_xll_cpp4->b, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
    Printf(b_xll_cpp4->b, "            TempStrNoSize(\"\\x01\"\"1\"),\n");
    Printf(b_xll_cpp4->b, "            // function category\n");
    Printf(b_xll_cpp4->b, "            TempStrNoSize(\"\\x07\"\"Example\"));\n");
}

String *copyUpper(String *s) {
    String *ret = Copy(s);
    char *c = Char(ret);
    c[0] = toupper(c[0]);
    return ret;
}

void printFunc(Node *n, bool manual) {
    Printf(bm_.f()->b_cpp_cpp->b,"//****FUNC*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    String   *symname   = Getattr(n,"sym:name");
    //String   *action = Getattr(n,"wrap:action");

    String *temp = copyUpper(symname);
    String *funcName = NewStringf("%s%s", prefix, temp);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    if (!manual) {
    Printf(bm_.f()->b_obj_hpp->b,"\n");
    Printf(bm_.f()->b_obj_hpp->b,"    %s %s(", type, symname);
    emitParmList(parms, bm_.f()->b_obj_hpp->b);
    Printf(bm_.f()->b_obj_hpp->b,");\n");

    Printf(bm_.f()->b_obj_cpp->b,"%s %s::%s(", type, module, symname);
    emitParmList(parms, bm_.f()->b_obj_cpp->b);
    Printf(bm_.f()->b_obj_cpp->b,") {\n");
    Printf(bm_.f()->b_obj_cpp->b,"    return %s(", name);
    emitParmList2(parms, bm_.f()->b_obj_cpp->b);
    Printf(bm_.f()->b_obj_cpp->b,");\n");
    Printf(bm_.f()->b_obj_cpp->b,"}\n");
    }

    Printf(bm_.f()->b_cpp_hpp->b,"    %s %s(", type, funcName);
    emitParmList4(parms, bm_.f()->b_cpp_hpp->b);
    Printf(bm_.f()->b_cpp_hpp->b,");\n");

    Printf(bm_.f()->b_cpp_cpp->b,"%s %s::%s(", type, module, funcName);
    emitParmList4(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,") {\n");
    emitParmList5(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,"    return %s::%s(", module, symname);
    emitParmList6(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,");\n");
    Printf(bm_.f()->b_cpp_cpp->b,"}\n");

    f4(n, type, parms);

    String *ret_type = getType("excel_out", n, type);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "DLLEXPORT %s %s(", ret_type, funcName);
    emitParmList(parms, bm_.f()->b_xll_cpp->b, true);
    Printf(bm_.f()->b_xll_cpp->b, ") {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    try {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bm_.f()->b_xll_cpp->b, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        %s returnValue =\n", type);
    Printf(bm_.f()->b_xll_cpp->b, "            %s::%s(", module, symname);
    emitParmList2(parms, bm_.f()->b_xll_cpp->b, true);
    Printf(bm_.f()->b_xll_cpp->b, ");\n");

    String *tm = getType("ohxl_ret", n, type);
    Printf(bm_.f()->b_xll_cpp->b, Char(tm));

    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    } catch (const std::exception &e) {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bm_.f()->b_xll_cpp->b, "        return 0;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    }\n");
    Printf(bm_.f()->b_xll_cpp->b, "}\n");
}

void printMemb(Node *n) {
    Printf(bm_.f()->b_cpp_cpp->b,"//****MEMB*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    Node *p = Getattr(n,"parentNode");
    String   *cls   = Getattr(p,"sym:name");
    String   *pname   = Getattr(p,"name");
    ParmList *parms  = Getattr(n,"parms");

    String *temp0 = copyUpper(cls);
    String *temp1 = copyUpper(name);
    String *funcName = NewStringf("%s%s%s", prefix, temp0, temp1);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every member.
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
    Printf(bm_.f()->b_cpp_hpp->b,"    %s %s(", type, funcName);
    emitParmList4(parms2, bm_.f()->b_cpp_hpp->b);
    Printf(bm_.f()->b_cpp_hpp->b,");\n");

    Printf(bm_.f()->b_cpp_cpp->b,"%s %s::%s(\n", type, module, funcName);
    emitParmList4(parms2, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,") {\n");
    emitParmList5(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,"    OH_GET_REFERENCE(x, objectID, %s::%s, %s);\n", module, cls, pname);
    Printf(bm_.f()->b_cpp_cpp->b,"    return x->%s(", name);
    emitParmList6(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,");\n", name);
    Printf(bm_.f()->b_cpp_cpp->b,"}\n");

    f4(n, type, parms2);

    String *ret_type = getType("excel_out", n, type);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "DLLEXPORT %s %s(", ret_type, funcName);
    emitParmList3(parms2, bm_.f()->b_xll_cpp->b);
    Printf(bm_.f()->b_xll_cpp->b, ") {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    try {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bm_.f()->b_xll_cpp->b, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        OH_GET_REFERENCE(x, objectID, %s::%s, %s);\n", module, cls, pname);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        static %s ret;\n", type);
    Printf(bm_.f()->b_xll_cpp->b, "        ret = x->%s(", name);
    emitParmList2(parms, bm_.f()->b_xll_cpp->b, true);
    Printf(bm_.f()->b_xll_cpp->b, ");\n");
    Printf(bm_.f()->b_xll_cpp->b, "        return &ret;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    } catch (const std::exception &e) {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bm_.f()->b_xll_cpp->b, "        return 0;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    }\n");
    Printf(bm_.f()->b_xll_cpp->b, "}\n");
}

void printCtor(Node *n, bool manual) {
    Printf(bm_.f()->b_cpp_cpp->b,"//****CTOR*****\n");
    String   *name   = Getattr(n,"name");
    //SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    Node *p = Getattr(n,"parentNode");
    String *pname   = Getattr(p,"name");

    String *temp = copyUpper(name);
    String *funcName = NewStringf("%s%s", prefix, temp);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every ctor.
    Parm *parms2 = NewHash();
    Setattr(parms2, "name", "objectID");
    String *nt  = NewStringf("std::string");
    SwigType_add_qualifier(nt, "const");
    SwigType_add_reference(nt);
    Setattr(parms2, "type", nt);
    Setattr(parms2, "nextSibling", parms);

    // Create from parms2 another list parms3 - prepend an argument to represent
    // the object ID which is the return value of addin func that wraps ctor.
    Parm *parms3 = NewHash();
    Setattr(parms3, "name", "");
    String *nt2  = NewStringf("std::string");
    SwigType_add_qualifier(nt2, "const");
    SwigType_add_reference(nt2);
    Setattr(parms3, "type", nt2);
    Setattr(parms3, "hidden", "1");
    Setattr(parms3, "nextSibling", parms2);

    Printf(b_wrappers, "//***DEF\n");
    printList(parms2);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(parms2)));
    Printf(b_wrappers, "//***DEF\n");

    //type = reduceType(type);

    Printf(bm_.f()->b_val_hpp->b,"        class %s : public ObjectHandler::ValueObject {\n", funcName);
    Printf(bm_.f()->b_val_hpp->b,"            friend class boost::serialization::access;\n");
    Printf(bm_.f()->b_val_hpp->b,"        public:\n");
    Printf(bm_.f()->b_val_hpp->b,"            %s() {}\n", funcName);
    Printf(bm_.f()->b_val_hpp->b,"            %s(\n", funcName);
    Printf(bm_.f()->b_val_hpp->b,"                const std::string& ObjectId,\n");
    Printf(bm_.f()->b_val_hpp->b,"                bool Permanent);\n");
    Printf(bm_.f()->b_val_hpp->b,"\n");
    Printf(bm_.f()->b_val_hpp->b,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
    Printf(bm_.f()->b_val_hpp->b,"            std::vector<std::string> getPropertyNamesVector() const;\n");
    Printf(bm_.f()->b_val_hpp->b,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
    Printf(bm_.f()->b_val_hpp->b,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
    Printf(bm_.f()->b_val_hpp->b,"\n");
    Printf(bm_.f()->b_val_hpp->b,"        protected:\n");
    Printf(bm_.f()->b_val_hpp->b,"            static const char* mPropertyNames[];\n");
    Printf(bm_.f()->b_val_hpp->b,"            static std::set<std::string> mSystemPropertyNames;\n");
    Printf(bm_.f()->b_val_hpp->b,"            bool Permanent_;\n");
    Printf(bm_.f()->b_val_hpp->b,"\n");
    Printf(bm_.f()->b_val_hpp->b,"            template<class Archive>\n");
    Printf(bm_.f()->b_val_hpp->b,"            void serialize(Archive& ar, const unsigned int) {\n");
    Printf(bm_.f()->b_val_hpp->b,"            boost::serialization::void_cast_register<%s, ObjectHandler::ValueObject>(this, this);\n", funcName);
    Printf(bm_.f()->b_val_hpp->b,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
    Printf(bm_.f()->b_val_hpp->b,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
    Printf(bm_.f()->b_val_hpp->b,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
    Printf(bm_.f()->b_val_hpp->b,"            }\n");
    Printf(bm_.f()->b_val_hpp->b,"        };\n");
    Printf(bm_.f()->b_val_hpp->b,"\n");

    Printf(bm_.f()->b_val_cpp->b,"        const char* %s::mPropertyNames[] = {\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            \"Permanent\"\n");
    Printf(bm_.f()->b_val_cpp->b,"        };\n");
    Printf(bm_.f()->b_val_cpp->b,"\n");
    Printf(bm_.f()->b_val_cpp->b,"        std::set<std::string> %s::mSystemPropertyNames(\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(bm_.f()->b_val_cpp->b,"\n");
    Printf(bm_.f()->b_val_cpp->b,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            return mSystemPropertyNames;\n");
    Printf(bm_.f()->b_val_cpp->b,"        }\n");
    Printf(bm_.f()->b_val_cpp->b,"\n");
    Printf(bm_.f()->b_val_cpp->b,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            std::vector<std::string> ret(\n");
    Printf(bm_.f()->b_val_cpp->b,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(bm_.f()->b_val_cpp->b,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
    Printf(bm_.f()->b_val_cpp->b,"                i != userProperties.end(); ++i)\n");
    Printf(bm_.f()->b_val_cpp->b,"                ret.push_back(i->first);\n");
    Printf(bm_.f()->b_val_cpp->b,"            return ret;\n");
    Printf(bm_.f()->b_val_cpp->b,"        }\n");
    Printf(bm_.f()->b_val_cpp->b,"\n");
    Printf(bm_.f()->b_val_cpp->b,"        ObjectHandler::property_t %s::getSystemProperty(const std::string& name) const {\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(bm_.f()->b_val_cpp->b,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(bm_.f()->b_val_cpp->b,"                return objectId_;\n");
    Printf(bm_.f()->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(bm_.f()->b_val_cpp->b,"                return className_;\n");
    Printf(bm_.f()->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(bm_.f()->b_val_cpp->b,"                return Permanent_;\n");
    Printf(bm_.f()->b_val_cpp->b,"            else\n");
    Printf(bm_.f()->b_val_cpp->b,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
    Printf(bm_.f()->b_val_cpp->b,"        }\n");
    Printf(bm_.f()->b_val_cpp->b,"\n");
    Printf(bm_.f()->b_val_cpp->b,"        void %s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(bm_.f()->b_val_cpp->b,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(bm_.f()->b_val_cpp->b,"                objectId_ = boost::get<std::string>(value);\n");
    Printf(bm_.f()->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(bm_.f()->b_val_cpp->b,"                className_ = boost::get<std::string>(value);\n");
    Printf(bm_.f()->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(bm_.f()->b_val_cpp->b,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
    Printf(bm_.f()->b_val_cpp->b,"            else\n");
    Printf(bm_.f()->b_val_cpp->b,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
    Printf(bm_.f()->b_val_cpp->b,"        }\n");
    Printf(bm_.f()->b_val_cpp->b,"\n");
    Printf(bm_.f()->b_val_cpp->b,"        %s::%s(\n", funcName, funcName);
    Printf(bm_.f()->b_val_cpp->b,"                const std::string& ObjectId,\n");
    Printf(bm_.f()->b_val_cpp->b,"                bool Permanent) :\n");
    Printf(bm_.f()->b_val_cpp->b,"            ObjectHandler::ValueObject(ObjectId, \"%s\", Permanent),\n", funcName);
    Printf(bm_.f()->b_val_cpp->b,"            Permanent_(Permanent) {\n");
    Printf(bm_.f()->b_val_cpp->b,"        }\n");

    if (!manual) {
    Printf(bm_.f()->b_obj_hpp->b,"\n");
    Printf(bm_.f()->b_obj_hpp->b,"    class %s : \n", name);
    Printf(bm_.f()->b_obj_hpp->b,"        public ObjectHandler::LibraryObject<%s> {\n", pname);
    Printf(bm_.f()->b_obj_hpp->b,"    public:\n");
    Printf(bm_.f()->b_obj_hpp->b,"        %s(\n", name);
    Printf(bm_.f()->b_obj_hpp->b,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,\n");
    Printf(bm_.f()->b_obj_hpp->b,"            ");
    emitParmList(parms, bm_.f()->b_obj_hpp->b);
    Printf(bm_.f()->b_obj_hpp->b,", bool permanent)\n");
    Printf(bm_.f()->b_obj_hpp->b,"        : ObjectHandler::LibraryObject<%s>(properties, permanent) {\n", pname);
    Printf(bm_.f()->b_obj_hpp->b,"            libraryObject_ = boost::shared_ptr<%s>(new %s(", pname, pname);
    emitParmList2(parms, bm_.f()->b_obj_hpp->b);
    Printf(bm_.f()->b_obj_hpp->b,"));\n");
    Printf(bm_.f()->b_obj_hpp->b,"        }\n");
    Printf(bm_.f()->b_obj_hpp->b,"    };\n");
    Printf(bm_.f()->b_obj_hpp->b,"\n");
    }

    Printf(bm_.f()->b_cpp_hpp->b,"    std::string %s(", funcName);
    emitParmList4(parms2, bm_.f()->b_cpp_hpp->b);
    Printf(bm_.f()->b_cpp_hpp->b,");\n");

    Printf(bm_.f()->b_cpp_cpp->b,"std::string %s::%s(", module, funcName);
    emitParmList4(parms2, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,") {\n");
    emitParmList5(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,"    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(bm_.f()->b_cpp_cpp->b,"        new %s::ValueObjects::%s(\n", module, funcName);
    Printf(bm_.f()->b_cpp_cpp->b,"            objectID, false));\n");
    Printf(bm_.f()->b_cpp_cpp->b,"    boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(bm_.f()->b_cpp_cpp->b,"        new %s::%s(\n", module, name);
    Printf(bm_.f()->b_cpp_cpp->b,"            valueObject, ");
    emitParmList6(parms, bm_.f()->b_cpp_cpp->b);
    Printf(bm_.f()->b_cpp_cpp->b,", false));\n");
    Printf(bm_.f()->b_cpp_cpp->b,"    std::string returnValue =\n");
    Printf(bm_.f()->b_cpp_cpp->b,"        ObjectHandler::Repository::instance().storeObject(\n");
    Printf(bm_.f()->b_cpp_cpp->b,"            objectID, object, false, valueObject);\n");
    Printf(bm_.f()->b_cpp_cpp->b,"    return returnValue;\n");
    Printf(bm_.f()->b_cpp_cpp->b,"}\n");

    f4(n, 0, parms3);

    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "DLLEXPORT char *%s(", funcName);
    emitParmList3(parms2, bm_.f()->b_xll_cpp->b);
    Printf(bm_.f()->b_xll_cpp->b, ") {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    try {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bm_.f()->b_xll_cpp->b, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(bm_.f()->b_xll_cpp->b, "            new %s::ValueObjects::%s(objectID, false));\n", module, funcName);
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(bm_.f()->b_xll_cpp->b, "            new %s::%s(valueObject,", module, name);
    emitParmList2(parms, bm_.f()->b_xll_cpp->b, true);
    Printf(bm_.f()->b_xll_cpp->b, ", false));\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        std::string returnValue =\n");
    Printf(bm_.f()->b_xll_cpp->b, "            ObjectHandler::RepositoryXL::instance().storeObject(objectID, object, true);\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        static char ret[XL_MAX_STR_LEN];\n");
    Printf(bm_.f()->b_xll_cpp->b, "        ObjectHandler::stringToChar(returnValue, ret);\n");
    Printf(bm_.f()->b_xll_cpp->b, "        return ret;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    } catch (const std::exception &e) {\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bm_.f()->b_xll_cpp->b, "        return 0;\n");
    Printf(bm_.f()->b_xll_cpp->b, "\n");
    Printf(bm_.f()->b_xll_cpp->b, "    }\n");
    Printf(bm_.f()->b_xll_cpp->b, "}\n");
}

int functionWrapper(Node *n) {
    String *group = Getattr(n,"feature:rp:group");
    String *include = Getattr(n,"feature:rp:include");
    bool manual = checkAttribute(n,"feature:rp:generation","manual");
    bm_.init(group, include, manual);

    Printf(b_wrappers,"//XXX***functionWrapper*******\n");
    Printf(b_wrappers,"//module=%s\n", module);
    Printf(b_wrappers,"//group=%s\n", group);
    printNode(n);
    printList(Getattr(n, "parms"));
    Printf(b_wrappers,"//*************\n");

    String *nodeType = Getattr(n,"nodeType");
    if (0 == Strcmp("cdecl", nodeType)) {
        if (NULL == Getattr(n, "ismember")) {
            printFunc(n, manual);
        } else {
            printMemb(n);
        }
    } else if (0 == Strcmp("constructor", nodeType)) {
            printCtor(n, manual);
    }

  return SWIG_OK;
}

}; // class REPOSIT

extern "C" Language *
swig_reposit(void) {
  return new REPOSIT();
}

