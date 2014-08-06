
#include "swigmod.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>

String *prefix = 0;
String *module = 0;
String *addinCppNameSpace = 0;
String *nmspace = 0;
String *libraryClass = 0;
long idNum = 4;
bool generateCtor = false;

// FIXME store some defaults in reposit.swg and retrieve them here.
String *objDir = NewString("AddinObjects");
String *addDir = NewString("AddinCpp");
String *xllDir = NewString("AddinXl");
String *objInc = NewString("AddinObjects");
String *addInc = NewString("AddinCpp");
String *xllInc = NewString("AddinXl");


struct Buffer;

// global buffers

//Buffer *b_obj_all_hpp=0;//FIXME should generate this file too
Buffer *b_cre_reg_cpp=0;
Buffer *b_cre_all_hpp=0;
Buffer *b_reg_ser_hpp=0;
Buffer *b_reg_all_hpp=0;
Buffer *b_add_all_hpp=0;
Buffer *b_xll_reg_cpp=0;

List *errorList = NewList();

bool generateXllAddin = false;
bool generateCppAddin = false;

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
    File *b2;
    File *b3;
    File *b4;
    Buffer(String *name) : name_(name) {
        b = NewString("");
        b2 = NewString("");
        b3 = NewString("");
        b4 = NewString("");
    }
    ~Buffer() {
        printf("Generating file '%s'\n", Char(name_));
        File *f = initFile(name_);
        Delete(name_);
        Dump(b, f);
        Dump(b2, f);
        Dump(b3, f);
        Dump(b4, f);
        Delete(b);
        Delete(b2);
        Delete(b3);
        Delete(b4);
        Delete(f);
    }
};

struct BufferGroup {

    // buffers
    Buffer *b_val_hpp;
    Buffer *b_val_cpp;
    Buffer *b_cre_hpp;
    Buffer *b_cre_cpp;
    Buffer *b_reg_hpp;
    Buffer *b_reg_cpp;
    Buffer *b_obj_hpp;
    Buffer *b_obj_cpp;
    Buffer *b_add_hpp;
    Buffer *b_add_cpp;
    Buffer *b_xll_cpp;
    Buffer *b_xll_reg;

    String *name_;
    bool automatic_;

    BufferGroup(String *name, String *include, bool automatic) {

        name_ = Copy(name);
        automatic_ = automatic;

        b_val_hpp = new Buffer(NewStringf("%s/valueobjects/vo_%s.hpp", objDir, name_));
        b_val_cpp = new Buffer(NewStringf("%s/valueobjects/vo_%s.cpp", objDir, name_));
        b_cre_hpp = new Buffer(NewStringf("%s/serialization/create/create_%s.hpp", objDir, name_));
        b_cre_cpp = new Buffer(NewStringf("%s/serialization/create/create_%s.cpp", objDir, name_));
        b_reg_hpp = new Buffer(NewStringf("%s/serialization/register/serialization_%s.hpp", objDir, name_));
        b_reg_cpp = new Buffer(NewStringf("%s/serialization/register/serialization_%s.cpp", objDir, name_));
        b_obj_hpp = new Buffer(NewStringf("%s/obj_%s.hpp", objDir, name_));
        if (automatic_) {
        b_obj_cpp = new Buffer(NewStringf("%s/obj_%s.cpp", objDir, name_));
        }
        if (generateCppAddin) {
        b_add_hpp = new Buffer(NewStringf("%s/add_%s.hpp", addDir, name_));
        b_add_cpp = new Buffer(NewStringf("%s/add_%s.cpp", addDir, name_));
        }
        if (generateXllAddin) {
        b_xll_cpp = new Buffer(NewStringf("%s/functions/function_%s.cpp", xllDir, name_));
        b_xll_reg = new Buffer(NewStringf("%s/register/register_%s.cpp", xllDir, name_));
        }

        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "#ifndef vo_%s_hpp\n", name);
        Printf(b_val_hpp->b, "#define vo_%s_hpp\n", name);
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "#include <oh/valueobject.hpp>\n");
        Printf(b_val_hpp->b, "#include <string>\n");
        Printf(b_val_hpp->b, "#include <vector>\n");
        Printf(b_val_hpp->b, "#include <set>\n");
        Printf(b_val_hpp->b, "#include <boost/serialization/map.hpp>\n");
        Printf(b_val_hpp->b, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b,"namespace %s {\n", module);
        Printf(b_val_hpp->b, "\n");
        Printf(b_val_hpp->b, "namespace ValueObjects {\n");
        Printf(b_val_hpp->b, "\n");

        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, name);
        Printf(b_val_cpp->b, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b,"namespace %s {\n", module);
        Printf(b_val_cpp->b, "\n");
        Printf(b_val_cpp->b, "namespace ValueObjects {\n");
        Printf(b_val_cpp->b, "\n");

        Printf(b_cre_hpp->b, "\n");
        Printf(b_cre_hpp->b, "#ifndef create_%s_hpp\n", name);
        Printf(b_cre_hpp->b, "#define create_%s_hpp\n", name);
        Printf(b_cre_hpp->b, "\n");
        Printf(b_cre_hpp->b, "#include <oh/ohdefines.hpp>\n");
        Printf(b_cre_hpp->b, "#include <oh/object.hpp>\n");
        Printf(b_cre_hpp->b, "#include <oh/valueobject.hpp>\n");
        Printf(b_cre_hpp->b, "\n");
        Printf(b_cre_hpp->b, "namespace %s {\n", module);
        Printf(b_cre_hpp->b, "\n");

        Printf(b_cre_cpp->b, "\n");
        Printf(b_cre_cpp->b, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, name);
        Printf(b_cre_cpp->b, "//#include <%s/qladdindefines.hpp>\n", objInc);
        Printf(b_cre_cpp->b, "#include <%s/conversions/convert2.hpp>\n", objInc);
        Printf(b_cre_cpp->b, "//#include <%s/handle.hpp>\n", objInc);
        Printf(b_cre_cpp->b, "\n");
        Printf(b_cre_cpp->b, "#include <%s/obj_%s.hpp>\n", objInc, name);
        Printf(b_cre_cpp->b, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, name);
        Printf(b_cre_cpp->b, "\n");
        Printf(b_cre_cpp->b, "//#include <%s/conversions/all.hpp>\n", objInc);
        Printf(b_cre_cpp->b, "#include <oh/property.hpp>\n");
        Printf(b_cre_cpp->b, "\n");

        Printf(b_reg_hpp->b, "\n");
        Printf(b_reg_hpp->b, "#ifndef serialization_%s_hpp\n", name);
        Printf(b_reg_hpp->b, "#define serialization_%s_hpp\n", name);
        Printf(b_reg_hpp->b, "\n");
        Printf(b_reg_hpp->b, "#include <boost/archive/xml_iarchive.hpp>\n");
        Printf(b_reg_hpp->b, "#include <boost/archive/xml_oarchive.hpp>\n");
        Printf(b_reg_hpp->b, "\n");
        Printf(b_reg_hpp->b, "namespace %s {\n", module);
        Printf(b_reg_hpp->b, "\n");
        Printf(b_reg_hpp->b, "    void register_%s(boost::archive::xml_oarchive &ar);\n", name);
        Printf(b_reg_hpp->b, "    void register_%s(boost::archive::xml_iarchive &ar);\n", name);
        Printf(b_reg_hpp->b, "\n");
        Printf(b_reg_hpp->b, "} // namespace %s\n", module);
        Printf(b_reg_hpp->b, "\n");
        Printf(b_reg_hpp->b, "#endif\n");
        Printf(b_reg_hpp->b, "\n");

        Printf(b_reg_cpp->b, "\n");
        Printf(b_reg_cpp->b, "#include <oh/ohdefines.hpp>\n");
        Printf(b_reg_cpp->b, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, name);
        Printf(b_reg_cpp->b, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, name);
        Printf(b_reg_cpp->b, "#include <boost/serialization/shared_ptr.hpp>\n");
        Printf(b_reg_cpp->b, "#include <boost/serialization/variant.hpp>\n");
        Printf(b_reg_cpp->b, "#include <boost/serialization/vector.hpp>\n");
        Printf(b_reg_cpp->b, "\n");
        Printf(b_reg_cpp->b, "void %s::register_%s(boost::archive::xml_oarchive &ar) {\n", module, name);
        Printf(b_reg_cpp->b, "\n");

        Printf(b_reg_cpp->b2, "\n");
        Printf(b_reg_cpp->b2, "void %s::register_%s(boost::archive::xml_iarchive &ar) {\n", module, name);
        Printf(b_reg_cpp->b2, "\n");

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

        if (automatic_) {
            Printf(b_obj_cpp->b, "\n");
            Printf(b_obj_cpp->b, "#include <%s/obj_%s.hpp>\n", objInc, name);
            Printf(b_obj_cpp->b, "\n");
        }

        if (generateCppAddin) {
        Printf(b_add_hpp->b, "\n");
        Printf(b_add_hpp->b, "#ifndef add_%s_hpp\n", name);
        Printf(b_add_hpp->b, "#define add_%s_hpp\n", name);
        Printf(b_add_hpp->b, "\n");
        Printf(b_add_hpp->b, "#include <string>\n");
        // FIXME this #include is only needed if a datatype conversion is taking place.
        Printf(b_add_hpp->b, "#include <oh/property.hpp>\n");
        Printf(b_add_hpp->b, "\n");
        Printf(b_add_hpp->b, "namespace %s {\n", addinCppNameSpace);
        Printf(b_add_hpp->b, "\n");

        Printf(b_add_cpp->b, "\n");
        Printf(b_add_cpp->b, "#include <AddinCpp/add_%s.hpp>\n", name);
        // FIXME this #include is only required if the file contains conversions.
        Printf(b_add_cpp->b, "#include <%s/conversions/convert2.hpp>\n", objInc);
        // FIXME this #include is only required if the file contains enumerations.
        //Printf(b_add_cpp->b, "#include <oh/enumerations/typefactory.hpp>\n");
        // FIXME this #include is only required if the file contains constructors.
        Printf(b_add_cpp->b, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, name);
        Printf(b_add_cpp->b, "#include \"%s/obj_%s.hpp\"\n", objInc, name);
        // FIXME include only factories for types used in the current file.
        Printf(b_add_cpp->b, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        Printf(b_add_cpp->b, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_add_cpp->b, "#include <oh/repository.hpp>\n");
        Printf(b_add_cpp->b, "#include <AddinCpp/add_all.hpp>\n");
        Printf(b_add_cpp->b, "\n");
        }

        if (generateXllAddin) {
        Printf(b_xll_cpp->b, "\n");
        Printf(b_xll_cpp->b, "#include <ohxl/objecthandlerxl.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/register/register_all.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/functions/export.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/utilities/xlutilities.hpp>\n");
        Printf(b_xll_cpp->b, "#include <ohxl/objectwrapperxl.hpp>\n");
        Printf(b_xll_cpp->b, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, name);
        Printf(b_xll_cpp->b, "//#include \"%s/obj_%s.hpp\"\n", objInc, name);
        Printf(b_xll_cpp->b, "#include \"%s/obj_all.hpp\"\n", objInc);
        Printf(b_xll_cpp->b, "#include \"conversions/convert2.hpp\"\n");
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

        Printf(b_xll_reg->b, "\n");
        Printf(b_xll_reg->b, "#include <xlsdk/xlsdkdefines.hpp>\n");
        Printf(b_xll_reg->b, "\n");
        Printf(b_xll_reg->b, "void register_%s(const XLOPER &xDll) {\n", name);
        Printf(b_xll_reg->b, "\n");
        Printf(b_xll_reg->b2, "\n");
        Printf(b_xll_reg->b2, "void unregister_%s(const XLOPER &xDll) {\n", name);
        Printf(b_xll_reg->b2, "\n");
        Printf(b_xll_reg->b2, "    XLOPER xlRegID;\n");
        Printf(b_xll_reg->b2, "\n");
        }

        // write to global buffers

        Printf(b_cre_all_hpp->b, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, name);

        Printf(b_reg_all_hpp->b, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, name);

        if (generateCtor) {
        Printf(b_reg_ser_hpp->b, "        register_%s(ar);\n", name);
        }

        if (generateCppAddin) {
        Printf(b_add_all_hpp->b, "#include <%s/add_%s.hpp>\n", addInc, name);
        }

        if (generateXllAddin) {
        Printf(b_xll_reg_cpp->b, "extern void register_%s(const XLOPER&);\n", name);
        Printf(b_xll_reg_cpp->b2, "extern void unregister_%s(const XLOPER&);\n", name);
        Printf(b_xll_reg_cpp->b3, "    register_%s(xDll);\n", name);
        Printf(b_xll_reg_cpp->b4, "    unregister_%s(xDll);\n", name);
        }
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

        Printf(b_cre_hpp->b, "\n");
        Printf(b_cre_hpp->b, "} // namespace %s\n", module);
        Printf(b_cre_hpp->b, "\n");
        Printf(b_cre_hpp->b, "#endif\n");
        Printf(b_cre_hpp->b, "\n");

        Printf(b_cre_cpp->b, "\n");

        Printf(b_reg_hpp->b, "\n");

        Printf(b_reg_cpp->b, "}\n");
        Printf(b_reg_cpp->b, "\n");

        Printf(b_reg_cpp->b2, "}\n");
        Printf(b_reg_cpp->b2, "\n");

        Printf(b_obj_hpp->b, "} // namespace %s\n", module);
        Printf(b_obj_hpp->b, "\n");
        Printf(b_obj_hpp->b, "#endif\n");
        Printf(b_obj_hpp->b, "\n");

        if (automatic_) {
            Printf(b_obj_cpp->b, "\n");
        }

        if (generateCppAddin) {
        Printf(b_add_hpp->b, "\n");
        Printf(b_add_hpp->b, "} // namespace %s\n", addinCppNameSpace);
        Printf(b_add_hpp->b, "\n");
        Printf(b_add_hpp->b, "#endif\n");
        Printf(b_add_hpp->b, "\n");

        Printf(b_add_cpp->b, "\n");
        }

        if (generateXllAddin) {
        Printf(b_xll_reg->b, "}\n");
        Printf(b_xll_reg->b2, "}\n");
        }

        delete b_val_hpp;
        delete b_val_cpp;
        delete b_cre_hpp;
        delete b_cre_cpp;
        delete b_reg_hpp;
        delete b_reg_cpp;
        delete b_obj_hpp;
        if (automatic_) {
            delete b_obj_cpp;
        }
        if (generateCppAddin) {
        delete b_add_hpp;
        delete b_add_cpp;
        }
        if (generateXllAddin) {
        delete b_xll_cpp;
        delete b_xll_reg;
        }
    }
};

class BufferMap {

    typedef std::map<std::string, BufferGroup*> BM;
    BM bm_;
    std::string name_;

public:

    BufferGroup *getBufferGroup(String *name, String *include, bool automatic) {
        name_ = Char(name);
        if (bm_.end() == bm_.find(name_))
            bm_[name_] = new BufferGroup(name, include, automatic);
        return bm_[name_];
    }

    void clear() {
        for (BM::const_iterator i=bm_.begin(); i!=bm_.end(); ++i)
            delete i->second;
    }
};

class REPOSIT : public Language {

    BufferMap bm_;
    BufferGroup *bg;
    bool automatic;
    int functionType;//0=function, 1=constructor, 2=member

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
        if ( (strcmp(argv[i],"-gencpp") == 0)) {
            generateCppAddin = true;
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-genxll") == 0)) {
            generateXllAddin = true;
            Swig_mark_arg(i);
        } else if (strcmp(argv[i], "-prefix") == 0) {
            if (argv[i + 1]) {
                prefix = NewString(argv[i + 1]);
                Swig_mark_arg(i);
                Swig_mark_arg(i + 1);
                i++;
            } else {
                Swig_arg_error();
            }
        }
    }
  }

Node *getNode(Node *n, const char *c) {
    Node *ret = 0;
    if (!(ret = Getattr(n, c))) {
        printf ("can't find node %s.\n", c);
        SWIG_exit(EXIT_FAILURE);
    }
    return ret;
}

void printNode(Node *n, File *f=0) {
    if (0==f)
        f=b_wrappers;
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        Printf(f,"/* %d %s %s */\n", i, key, Getattr(n, key));
    }
}

virtual int top(Node *n) {
    printf("Generating code.\n");

   /* Get the module name */
   module = Getattr(n, "name");
    addinCppNameSpace = NewStringf("%sCpp", module);

    // Extract some config info.
    Node *n2 = getNode(n, "module");
    if (Node *n3 = Getattr(n2, "options")) {
        if (String *n4 = getNode(n3, "rp_obj_dir"))
            objDir = n4;
        if (String *n5 = getNode(n3, "rp_add_dir"))
            addDir = n5;
        if (String *n6 = getNode(n3, "rp_xll_dir"))
            xllDir = n6;
        if (String *n7 = getNode(n3, "rp_obj_inc"))
            objInc = n7;
        if (String *n8 = getNode(n3, "rp_add_inc"))
            addInc = n8;
        if (String *n9 = getNode(n3, "rp_xll_inc"))
            xllInc = n9;
    }

    printf("module=%s\n", Char(module));
    printf("addinCppNameSpace=%s\n", Char(addinCppNameSpace));
    printf("rp_obj_dir=%s\n", Char(objDir));
    printf("rp_add_dir=%s\n", Char(addDir));
    printf("rp_xll_dir=%s\n", Char(xllDir));
    printf("rp_obj_inc=%s\n", Char(objInc));
    printf("rp_add_inc=%s\n", Char(addInc));
    printf("rp_xll_inc=%s\n", Char(xllInc));

   /* Initialize I/O */
    b_runtime = NewString("");
    b_init = NewString("");
    b_header = NewString("");
    b_wrappers = NewString("");
    b_director_h = NewString("");
    b_director = NewString("");
    b_begin = NewString("");

        printNode(n);

   /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("header", b_header);
    Swig_register_filebyname("wrapper", b_wrappers);
    Swig_register_filebyname("begin", b_begin);
    Swig_register_filebyname("runtime", b_runtime);
    Swig_register_filebyname("init", b_init);
    Swig_register_filebyname("director", b_director);
    Swig_register_filebyname("director_h", b_director_h);

    b_cre_reg_cpp = new Buffer(NewStringf("%s/serialization/register_creators.cpp", objDir));
    b_cre_all_hpp = new Buffer(NewStringf("%s/serialization/create/create_all.hpp", objDir));
    b_reg_ser_hpp = new Buffer(NewStringf("%s/serialization/register/serialization_register.hpp", objDir));
    b_reg_all_hpp = new Buffer(NewStringf("%s/serialization/register/serialization_all.hpp", objDir));
    if (generateCppAddin) {
    b_add_all_hpp = new Buffer(NewStringf("%s/add_all.hpp", addDir));
    }
    if (generateXllAddin) {
    b_xll_reg_cpp = new Buffer(NewStringf("%s/register/register_all.cpp", xllDir));
    }

        Printf(b_cre_reg_cpp->b, "\n");
        Printf(b_cre_reg_cpp->b, "#include <%s/serialization/serializationfactory.hpp>\n", objInc);
        Printf(b_cre_reg_cpp->b, "#include <%s/serialization/create/create_all.hpp>\n", objInc);
        Printf(b_cre_reg_cpp->b, "\n");
        Printf(b_cre_reg_cpp->b, "void %s::SerializationFactory::registerCreators() {\n", module);
        Printf(b_cre_reg_cpp->b, "\n");

        Printf(b_cre_all_hpp->b, "\n");
        Printf(b_cre_all_hpp->b, "#ifndef create_all_hpp\n");
        Printf(b_cre_all_hpp->b, "#define create_all_hpp\n");
        Printf(b_cre_all_hpp->b, "\n");

        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "#ifndef serialization_register_hpp\n");
        Printf(b_reg_ser_hpp->b, "#define serialization_register_hpp\n");
        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "#include <%s/serialization/register/serialization_all.hpp>\n", objInc);
        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "namespace %s {\n", module);
        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "    template<class Archive>\n");
        Printf(b_reg_ser_hpp->b, "    void tpl_register_classes(Archive& ar) {\n");
        Printf(b_reg_ser_hpp->b, "\n");

        Printf(b_reg_all_hpp->b, "\n");
        Printf(b_reg_all_hpp->b, "#ifndef serialization_all_hpp\n");
        Printf(b_reg_all_hpp->b, "#define serialization_all_hpp\n");
        Printf(b_reg_all_hpp->b, "\n");

        if (generateCppAddin) {
        Printf(b_add_all_hpp->b, "\n");
        Printf(b_add_all_hpp->b, "#ifndef add_all_hpp\n");
        Printf(b_add_all_hpp->b, "#define add_all_hpp\n");
        Printf(b_add_all_hpp->b, "\n");
        Printf(b_add_all_hpp->b, "#include <%s/init.hpp>\n", addInc);
        }

        if (generateXllAddin) {
        Printf(b_xll_reg_cpp->b, "\n");
        Printf(b_xll_reg_cpp->b, "#include <register/register_all.hpp>\n");
        Printf(b_xll_reg_cpp->b, "\n");

        Printf(b_xll_reg_cpp->b3, "\n");
        Printf(b_xll_reg_cpp->b3, "void registerFunctions(const XLOPER& xDll) {\n");
        Printf(b_xll_reg_cpp->b3, "\n");

        Printf(b_xll_reg_cpp->b4, "\n");
        Printf(b_xll_reg_cpp->b4, "void unregisterFunctions(const XLOPER& xDll) {\n");
        Printf(b_xll_reg_cpp->b4, "\n");
        }

   /* Output module initialization code */
   Swig_banner(b_begin);

   /* Emit code for children */
   Language::top(n);

        Printf(b_cre_reg_cpp->b, "\n");
        Printf(b_cre_reg_cpp->b, "}\n");
        Printf(b_cre_reg_cpp->b, "\n");

        Printf(b_cre_all_hpp->b, "\n");
        Printf(b_cre_all_hpp->b, "#endif\n");
        Printf(b_cre_all_hpp->b, "\n");

        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "    }\n");
        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "}\n");
        Printf(b_reg_ser_hpp->b, "\n");
        Printf(b_reg_ser_hpp->b, "#endif\n");
        Printf(b_reg_ser_hpp->b, "\n");

        Printf(b_reg_all_hpp->b, "\n");
        Printf(b_reg_all_hpp->b, "#endif\n");
        Printf(b_reg_all_hpp->b, "\n");

        if (generateCppAddin) {
        Printf(b_add_all_hpp->b, "\n");
        Printf(b_add_all_hpp->b, "#endif\n");
        Printf(b_add_all_hpp->b, "\n");
        }

        if (generateXllAddin) {
        Printf(b_xll_reg_cpp->b3, "\n");
        Printf(b_xll_reg_cpp->b3, "}\n");
        Printf(b_xll_reg_cpp->b3, "\n");

        Printf(b_xll_reg_cpp->b4, "\n");
        Printf(b_xll_reg_cpp->b4, "}\n");
        Printf(b_xll_reg_cpp->b4, "\n");
        }

    delete b_cre_reg_cpp;
    delete b_cre_all_hpp;
    delete b_reg_ser_hpp;
    delete b_reg_all_hpp;
    if (generateCppAddin) {
    delete b_add_all_hpp;
    }
    if (generateXllAddin) {
    delete b_xll_reg_cpp;
    }

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

    bm_.clear();

    for (int i=0; i<Len(errorList); ++i) {
        String *errorMessage = Getitem(errorList, i);
        printf("%s", Char(errorMessage));
    }
    Delete(errorList);//FIXME also delete each item individually

   return SWIG_OK;
}

// overrride base class members, write debug info to b_director,
// and possibly pass control to a handler.

int moduleDirective(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN moduleDirective - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    int ret=Language::moduleDirective(n);
    Printf(b_director, "END   moduleDirective - node name='%s'.\n", Char(nodename));
    return ret;
}

int classDeclaration(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN classDeclaration - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    int ret=Language::classDeclaration(n);
    Printf(b_director, "END   classDeclaration - node name='%s'.\n", Char(nodename));
    return ret;
}

int constructorDeclaration(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN constructorDeclaration - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    int ret=Language::constructorDeclaration(n);
    Printf(b_director, "END   constructorDeclaration - node name='%s'.\n", Char(nodename));
    return ret;
}

int namespaceDeclaration(Node *n) {
    nmspace = Getattr(n, "name");
    Printf(b_director, "BEGIN namespaceDeclaration - node name='%s'.\n", Char(nmspace));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    int ret=Language::namespaceDeclaration(n);
    Printf(b_director, "END   namespaceDeclaration - node name='%s'.\n", Char(nmspace));
    return ret;
}

//BEGIN functionHandler - node name='SimpleLib::func'.  functionHandlerImpl - set functionType=0
//.BEGIN functionWrapper - node name='SimpleLib::func'. functionWrapperImpl - call functionWrapperImplAll then functionWrapperImplFunc
//.END   functionWrapper - node name='SimpleLib::func'.
//END   functionHandler - node name='SimpleLib::func'.
//BEGIN classDeclaration - node name='SimpleLib::Adder'.
//.BEGIN classHandler - node name='SimpleLib::Adder'.
//..BEGIN constructorDeclaration - node name='Adder'.
//...BEGIN constructorHandler - node name='Adder'.      constructorHandlerImpl - set functionType=1
//....BEGIN functionWrapper - node name='Adder'.        functionWrapperImpl - call functionWrapperImplAll then functionWrapperImplCtor
//....END   functionWrapper - node name='Adder'.
//...END   constructorHandler - node name='Adder'.
//..END   constructorDeclaration - node name='Adder'.
//..BEGIN functionHandler - node name='add'.
//...BEGIN memberfunctionHandler - node name='add'.     memberfunctionHandlerImpl - set functionType=2
//....BEGIN functionWrapper - node name='add'.          functionWrapperImpl - call functionWrapperImplAll then functionWrapperImplMemb
//....END   functionWrapper - node name='add'.
//...END   memberfunctionHandler - node name='add'.
//..END   functionHandler - node name='add'.
//.END   classHandler - node name='SimpleLib::Adder'.
//END   classDeclaration - node name='Adder'.

int functionHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN functionHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    //int ret=Language::functionHandler(n);
    int ret=functionHandlerImpl(n);
    Printf(b_director, "END   functionHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int memberfunctionHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN memberfunctionHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    //int ret=Language::memberfunctionHandler(n);
    int ret=memberfunctionHandlerImpl(n);
    Printf(b_director, "END   memberfunctionHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int constructorHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN constructorHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    //int ret=Language::constructorHandler(n);
    int ret=constructorHandlerImpl(n);
    Printf(b_director, "END   constructorHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int functionWrapper(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN functionWrapper - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    //int ret=Language::functionWrapper(n);
    int ret=functionWrapperImpl(n);
    Printf(b_director, "END   functionWrapper - node name='%s'.\n", Char(nodename));
    return ret;
}

int classHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_director, "BEGIN classHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_director);
    Printf(b_director, "call parent\n");
    int ret=Language::classHandler(n);
    Printf(b_director, "END   classHandler - node name='%s'.\n", Char(nodename));
    return ret;
}


void printList(Node *n) {
    while (n) {
        printNode(n);
        n = Getattr(n,"nextSibling");
    }
}

String *getTypeMap(const char *m, Node *n, SwigType *t, bool fatal = true) {
    if (String *tm = Swig_typemap_lookup(m, n, t, 0))
        return tm;
    if (fatal) {
        Append(errorList, NewStringf("*** ERROR : typemap '%s' does not match type '%s'.\n", m, Char(SwigType_str(t, 0))));
        // Do not exit, instead keep running so that the user can see any other error messages.
        //SWIG_exit(EXIT_FAILURE);
        // Return an error string, this will be inserted into the source code.
        //return NewStringf("#error *** typemap '%s' does not match type '%s' ***", m, SwigType_str(t, 0));
        return NewStringf("#error NEED THIS TYPEMAP: >>> %%typemap(%s) %s \"XXX\"; <<<", m, SwigType_str(t, 0));
    }
    return 0;
}

String *getType(Parm *p, const char *m, bool fatal) {
    SwigType *t  = Getattr(p, "type");
    if (0==strcmp(m, "rp_tm_default"))
        return t;
    else
        return getTypeMap(m, p, t, fatal);
}

void printIndent(File *buf, int indent) {
    for (int i=0;i<indent;++i)
        Printf(buf, "    ");
}

void emitTypeMap(File *buf, const char *m, Node *n, SwigType *t, int indent=0, bool fatal = true) {
    printIndent(buf, indent);
    Printf(buf, "// BEGIN typemap %s\n", m);
    printIndent(buf, indent);
    String *s = getTypeMap(m, n, t, fatal);
    if (Len(s)) {
        Printf(buf, "%s\n", s);
        printIndent(buf, indent);
    }
    Printf(buf, "// END   typemap %s\n", m);
}

void emitParmList(
    ParmList *parms,
    File *buf,
    int mode=0,         // 0=name, 1=type, 2=both
    const char *map="rp_tm_default",
    int indent=1,
    char delim=',',
    bool fatal=true,
    bool skipHidden=false,
    bool append=false) {

    if (0==map) {
        printf ("Function emitParmList called with null type map.\n");
        SWIG_exit(EXIT_FAILURE);
    }

    bool first = true;

    printIndent(buf, indent);
    Printf(buf, "// BEGIN typemap %s\n", map);
    printIndent(buf, indent);

    for (Parm *p = parms; p; p = nextSibling(p)) {

        if (skipHidden && Getattr(p, "hidden"))
            continue;

        String *name = Getattr(p,"name");
        String *type  = getType(p, map, fatal);

        if (!type)
            continue;

        if (first) {
            first = false;
        } else {
            Printf(buf, "%c\n", delim);
            printIndent(buf, indent);
        }

        if (0==mode)
            Printf(buf, "%s", name);
        else if (1==mode)
            Printf(buf, "%s", type);
        else
            Printf(buf, "%s", Char(SwigType_str(type, name)));
    }

    if (!first) {
        if (append)
            Printf(buf, "%c", delim);
        Printf(buf, "\n");
        printIndent(buf, indent);
    }
    Printf(buf, "// END   typemap %s\n", map);
}

String *excelParamCodes(Node *n, SwigType *type, ParmList *parms) {
    String *s = NewString("");
    if (type) {
        String *tm = getTypeMap("rp_tm_xll_cod", n, type);
        Append(s, tm);
    }
    for (Parm *p = parms; p; p = nextSibling(p)) {
        //if (Getattr(p, "hidden")) continue;
        SwigType *t  = Getattr(p, "type");
        String *tm = getTypeMap("rp_tm_xll_cod", p, t);
        Append(s, tm);
    }
    Append(s, "#");
    return s;
}

String *excelParamList(ParmList *parms) {
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

std::string hexLen(String *c) {
    std::stringstream s;
    s << std::hex << std::setw(2) << std::setfill('0') << Len(c);
    return s.str();
}

void excelRegister(File *b, Node *n, SwigType *type, ParmList *parms) {
    String *funcName   = Getattr(n, "rp:funcName");
    Printf(b, "        // BEGIN function excelRegister\n");
    Printf(b, "        Excel(xlfRegister, 0, 7, &xDll,\n");
    Printf(b, "            // function code name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // parameter codes\n");
    String *xlParamCodes = excelParamCodes(n, type, parms);
    Printf(b, "            TempStrNoSize(\n");
    Printf(b, "            // BEGIN func excelParamCodes (using typemap rp_tm_xll_cod)\n");
    Printf(b, "            \"\\x%s\"\"%s\"\n", hexLen(xlParamCodes).c_str(), xlParamCodes);
    Printf(b, "            // END   func excelParamCodes (using typemap rp_tm_xll_cod)\n");
    Printf(b, "            ),\n");
    Printf(b, "            // function display name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // comma-delimited list of parameters\n");
    String *xlParamList = excelParamList(parms);
    Printf(b, "            TempStrNoSize(\n");
    Printf(b, "            // BEGIN func excelParamList\n");
    Printf(b, "            \"\\x%s\"\"%s\"\n", hexLen(xlParamList).c_str(), xlParamList);
    Printf(b, "            // END   func excelParamList\n");
    Printf(b, "            ),\n");
    Printf(b, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
    Printf(b, "            TempStrNoSize(\"\\x01\"\"1\"),\n");
    Printf(b, "            // function category\n");
    Printf(b, "            TempStrNoSize(\"\\x07\"\"Example\")\n");
    Printf(b, "        );\n");
    Printf(b, "        // END   function excelRegister\n\n");
}

void excelUnregister(File *b, Node *n, SwigType *type, ParmList *parms) {
    String *funcName   = Getattr(n, "rp:funcName");
    Printf(b, "        // BEGIN function excelUnregister\n");
    Printf(b, "        Excel(xlfRegister, 0, 7, &xDll,\n");
    Printf(b, "            // function code name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // parameter codes\n");
    String *xlParamCodes = excelParamCodes(n, type, parms);
    Printf(b, "            TempStrNoSize(\n");
    Printf(b, "            // BEGIN func excelParamCodes (using typemap rp_tm_xll_cod)\n");
    Printf(b, "            \"\\x%s\"\"%s\"\n", hexLen(xlParamCodes).c_str(), xlParamCodes);
    Printf(b, "            // END   func excelParamCodes (using typemap rp_tm_xll_cod)\n");
    Printf(b, "            ),\n");
    Printf(b, "            // function display name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // comma-delimited list of parameters\n");
    String *xlParamList = excelParamList(parms);
    Printf(b, "            TempStrNoSize(\n");
    Printf(b, "            // BEGIN func excelParamList\n");
    Printf(b, "            \"\\x%s\"\"%s\"\n", hexLen(xlParamList).c_str(), xlParamList);
    Printf(b, "            // END   func excelParamList\n");
    Printf(b, "            ),\n");
    Printf(b, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
    Printf(b, "            TempStrNoSize(\"\\x01\"\"0\"),\n");
    Printf(b, "            // function category\n");
    Printf(b, "            TempStrNoSize(\"\\x07\"\"Example\")\n");
    Printf(b, "        );\n");
    Printf(b, "\n");
    Printf(b, "        Excel4(xlfRegisterId, &xlRegID, 2, &xDll,\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"));\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "        Excel4(xlfUnregister, 0, 1, &xlRegID);\n");
    Printf(b, "        // END   function excelUnregister\n\n");
}

// return a copy with first character uppercase
String *copyUpper(String *s) {
    String *ret = Copy(s);
    char *c = Char(ret);
    c[0] = toupper(c[0]);
    return ret;
}

// return a copy with all characters uppercase
String *copyUpper2(String *s) {
    String *ret = Copy(s);
    char *c = Char(ret);
    for (unsigned int i=0; i<strlen(c); i++)
        c[i] = toupper(c[i]);
    return ret;
}

int functionWrapperImpl(Node *n) {
    // Perform some processing common to all function types
    functionWrapperImplAll(n);

    // Generate outpu appropriate to the given function type
    int ret;
    if (0==functionType)
        ret = functionWrapperImplFunc(n);
    else if (1==functionType)
        ret = functionWrapperImplCtor(n);
    else if (2==functionType)
        ret = functionWrapperImplMemb(n);
    else
        ret = SWIG_OK;
    functionType=-1;
    return ret;
}

int functionHandlerImpl(Node *n) {
    functionType=0;
    return Language::functionHandler(n);
}

int functionWrapperImplFunc(Node *n) {
    if (generateCppAddin) {
    Printf(bg->b_add_cpp->b,"//****FUNC*****\n");
    }
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    String   *symname   = Getattr(n,"sym:name");
    //String   *action = Getattr(n,"wrap:action");

    String *temp = copyUpper(symname);
    String *funcName = NewStringf("%s%s", prefix, temp);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    Printf(bg->b_obj_hpp->b,"\n");
    emitTypeMap(bg->b_obj_hpp->b, "rp_tm_obj_ret", n, type, 1);
    Printf(bg->b_obj_hpp->b,"    %s(\n", symname);
    emitParmList(parms, bg->b_obj_hpp->b, 2, "rp_tm_default", 2);
    Printf(bg->b_obj_hpp->b,"    );\n");

    if (automatic) {
        emitTypeMap(bg->b_obj_cpp->b, "rp_tm_obj_ret", n, type);
        Printf(bg->b_obj_cpp->b,"%s::%s(\n", module, symname);
        emitParmList(parms, bg->b_obj_cpp->b, 2, "rp_tm_default", 2);
        Printf(bg->b_obj_cpp->b,"    ) {\n");
        emitTypeMap(bg->b_obj_cpp->b, "rp_tm_obj_rdc", n, type, 2);
        Printf(bg->b_obj_cpp->b,"        %s(\n", name);
        emitParmList(parms, bg->b_obj_cpp->b, 0, "rp_tm_default", 3, ',', true, true);
        Printf(bg->b_obj_cpp->b,"        );\n");
        Printf(bg->b_obj_cpp->b,"}\n");
    }

    if (generateCppAddin) {
    emitTypeMap(bg->b_add_hpp->b, "rp_tm_add_ret", n, type, 1);
    Printf(bg->b_add_hpp->b,"    %s(\n", funcName);
    emitParmList(parms, bg->b_add_hpp->b, 2, "rp_tm_add_prm", 2);
    Printf(bg->b_add_hpp->b,"    );\n");

    emitTypeMap(bg->b_add_cpp->b, "rp_tm_add_ret", n, type);
    Printf(bg->b_add_cpp->b,"%s::%s(\n", addinCppNameSpace, funcName);
    emitParmList(parms, bg->b_add_cpp->b, 2, "rp_tm_add_prm");
    Printf(bg->b_add_cpp->b,") {\n");
    emitParmList(parms, bg->b_add_cpp->b, 1, "rp_tm_add_cnv", 1, 0, false);
    Printf(bg->b_add_cpp->b,"\n");
    Printf(bg->b_add_cpp->b,"    return %s::%s(\n", module, symname);
    emitParmList(parms, bg->b_add_cpp->b, 1, "rp_tm_add_cll", 2, ',', true, true);
    Printf(bg->b_add_cpp->b,"    );\n");
    Printf(bg->b_add_cpp->b,"}\n");
    }

    if (generateXllAddin) {
    excelRegister(bg->b_xll_reg->b, n, type, parms);
    excelUnregister(bg->b_xll_reg->b2, n, type, parms);

    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "DLLEXPORT\n");
    emitTypeMap(bg->b_xll_cpp->b, "rp_tm_xll_ret", n, type);
    Printf(bg->b_xll_cpp->b, "%s(\n", funcName);
    emitParmList(parms, bg->b_xll_cpp->b, 2, "rp_tm_xll_prm", 1);
    Printf(bg->b_xll_cpp->b, ") {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    try {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bg->b_xll_cpp->b, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bg->b_xll_cpp->b, "\n");
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cnv", 2, 0, false);
    Printf(bg->b_xll_cpp->b, "\n");
    emitTypeMap(bg->b_xll_cpp->b, "rp_xll_get", n, type, 2);
    Printf(bg->b_xll_cpp->b, "        %s::%s(\n", module, symname);
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cll_obj", 3, ',', true, true);
    Printf(bg->b_xll_cpp->b, "        );\n\n");
    emitTypeMap(bg->b_xll_cpp->b, "rp_tm_xll_rdc", n, type, 2);

    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    } catch (const std::exception &e) {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bg->b_xll_cpp->b, "        return 0;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    }\n");
    Printf(bg->b_xll_cpp->b, "}\n");
    }
    return SWIG_OK;
}

void voGetProp(File *f, ParmList *parms) {
    Printf(f, "            // BEGIN func voGetProp\n");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *name = Getattr(p,"name");
        String *nameUpper = Getattr(p,"nameUpper");
        Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
        Printf(f, "                return %s_;\n", name);
    }
    Printf(f, "            // END   func voGetProp\n");
}

void voSetProp(File *f, ParmList *parms) {
    Printf(f, "            // BEGIN func voSetProp (using typemap rp_tm_val_cnv)\n");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *name = Getattr(p,"name");
        String *nameUpper = Getattr(p,"nameUpper");
        SwigType *type = Getattr(p,"type");
        String *cnv = getTypeMap("rp_tm_val_cnv", p, type);
        Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
        Printf(f, "                %s_ = %s;\n", name, cnv);
    }
    Printf(f, "            // END   func voSetProp (using typemap rp_tm_val_cnv)\n");
}

int constructorHandlerImpl(Node *n) {

    // If no ctor was defined in the *.i file then SWIG sets the following flag:
    bool defaultCtor = Getattr(n, "default_constructor");
    // For our purposes if no ctor was configured then we don't generate one:
    generateCtor = !defaultCtor;

    Node *p = Getattr(n, "parentNode");
    libraryClass = Getattr(p, "name");
    functionType=1;
    return Language::constructorHandler(n);
}

int functionWrapperImplCtor(Node *n) {
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    Node *p = Getattr(n,"parentNode");
    String *pname   = Getattr(p,"name");

    String *base = 0;
    if (List *baseList  = Getattr(p,"baselist")) {
        if (1==Len(baseList)) {
            base = Getitem(baseList, 0);
            printf("base = %s\n", Char(base));
        } else {
            printf("ERROR : Classs %s has multiple base classes.\n", Char(name));
            SWIG_exit(EXIT_FAILURE);
        }
    } else {
        printf("no bases\n");
    }

    String *temp = copyUpper(name);
    String *funcName = NewStringf("%s%s", prefix, temp);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));
    printf("type=%s\n", Char(SwigType_str(type, 0)));

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every ctor.
    Parm *parms2 = NewHash();
    Setattr(parms2, "name", "objectID");
    String *nt  = NewString("std::string");
    SwigType_add_qualifier(nt, "const");
    SwigType_add_reference(nt);
    Setattr(parms2, "type", nt);
    Setattr(parms2, "nextSibling", parms);

    // Create from parms2 another list parms3 - prepend an argument to represent
    // the object ID which is the return value of addin func that wraps ctor.
    Parm *parms3 = NewHash();
    Setattr(parms3, "name", "");
    String *nt2  = NewString("std::string");
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

    if (generateCtor) {
    Printf(bg->b_val_hpp->b,"        class %s : public ObjectHandler::ValueObject {\n", funcName);
    Printf(bg->b_val_hpp->b,"            friend class boost::serialization::access;\n");
    Printf(bg->b_val_hpp->b,"        public:\n");
    Printf(bg->b_val_hpp->b,"            %s() {}\n", funcName);
    Printf(bg->b_val_hpp->b,"            %s(\n", funcName);
    Printf(bg->b_val_hpp->b,"                const std::string& ObjectId,\n");
    emitParmList(parms, bg->b_val_hpp->b, 2, "rp_tm_val_prm", 4, ',', true, false, true);
    Printf(bg->b_val_hpp->b,"                bool Permanent);\n");
    Printf(bg->b_val_hpp->b,"\n");
    Printf(bg->b_val_hpp->b,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
    Printf(bg->b_val_hpp->b,"            std::vector<std::string> getPropertyNamesVector() const;\n");
    Printf(bg->b_val_hpp->b,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
    Printf(bg->b_val_hpp->b,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
    Printf(bg->b_val_hpp->b,"\n");
    Printf(bg->b_val_hpp->b,"        protected:\n");
    Printf(bg->b_val_hpp->b,"            static const char* mPropertyNames[];\n");
    Printf(bg->b_val_hpp->b,"            static std::set<std::string> mSystemPropertyNames;\n");
    emitParmList(parms, bg->b_val_hpp->b, 1, "rp_tm_val_dcl", 3, ';', true, false, true);
    Printf(bg->b_val_hpp->b,"            bool Permanent_;\n");
    Printf(bg->b_val_hpp->b,"\n");
    Printf(bg->b_val_hpp->b,"            template<class Archive>\n");
    Printf(bg->b_val_hpp->b,"            void serialize(Archive& ar, const unsigned int) {\n");
    Printf(bg->b_val_hpp->b,"            boost::serialization::void_cast_register<%s, ObjectHandler::ValueObject>(this, this);\n", funcName);
    Printf(bg->b_val_hpp->b,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
    Printf(bg->b_val_hpp->b,"                    & boost::serialization::make_nvp(\"ClassName\", className_)\n");
    emitParmList(parms, bg->b_val_hpp->b, 1, "rp_tm_val_ser", 5, 0);
    Printf(bg->b_val_hpp->b,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
    Printf(bg->b_val_hpp->b,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
    Printf(bg->b_val_hpp->b,"            }\n");
    Printf(bg->b_val_hpp->b,"        };\n");
    Printf(bg->b_val_hpp->b,"\n");

    Printf(bg->b_val_cpp->b,"        const char* %s::mPropertyNames[] = {\n", funcName);
    emitParmList(parms, bg->b_val_cpp->b, 1, "rp_tm_val_nam", 3, ',', true, false, true);
    Printf(bg->b_val_cpp->b,"            \"Permanent\"\n");
    Printf(bg->b_val_cpp->b,"        };\n");
    Printf(bg->b_val_cpp->b,"\n");
    Printf(bg->b_val_cpp->b,"        std::set<std::string> %s::mSystemPropertyNames(\n", funcName);
    Printf(bg->b_val_cpp->b,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(bg->b_val_cpp->b,"\n");
    Printf(bg->b_val_cpp->b,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", funcName);
    Printf(bg->b_val_cpp->b,"            return mSystemPropertyNames;\n");
    Printf(bg->b_val_cpp->b,"        }\n");
    Printf(bg->b_val_cpp->b,"\n");
    Printf(bg->b_val_cpp->b,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", funcName);
    Printf(bg->b_val_cpp->b,"            std::vector<std::string> ret(\n");
    Printf(bg->b_val_cpp->b,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(bg->b_val_cpp->b,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
    Printf(bg->b_val_cpp->b,"                i != userProperties.end(); ++i)\n");
    Printf(bg->b_val_cpp->b,"                ret.push_back(i->first);\n");
    Printf(bg->b_val_cpp->b,"            return ret;\n");
    Printf(bg->b_val_cpp->b,"        }\n");
    Printf(bg->b_val_cpp->b,"\n");
    Printf(bg->b_val_cpp->b,"        ObjectHandler::property_t %s::getSystemProperty(const std::string& name) const {\n", funcName);
    Printf(bg->b_val_cpp->b,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(bg->b_val_cpp->b,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(bg->b_val_cpp->b,"                return objectId_;\n");
    Printf(bg->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(bg->b_val_cpp->b,"                return className_;\n");
    voGetProp(bg->b_val_cpp->b, parms);
    Printf(bg->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(bg->b_val_cpp->b,"                return Permanent_;\n");
    Printf(bg->b_val_cpp->b,"            else\n");
    Printf(bg->b_val_cpp->b,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
    Printf(bg->b_val_cpp->b,"        }\n");
    Printf(bg->b_val_cpp->b,"\n");
    Printf(bg->b_val_cpp->b,"        void %s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", funcName);
    Printf(bg->b_val_cpp->b,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(bg->b_val_cpp->b,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(bg->b_val_cpp->b,"                objectId_ = boost::get<std::string>(value);\n");
    Printf(bg->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(bg->b_val_cpp->b,"                className_ = boost::get<std::string>(value);\n");
    voSetProp(bg->b_val_cpp->b, parms);
    Printf(bg->b_val_cpp->b,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(bg->b_val_cpp->b,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
    Printf(bg->b_val_cpp->b,"            else\n");
    Printf(bg->b_val_cpp->b,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
    Printf(bg->b_val_cpp->b,"        }\n");
    Printf(bg->b_val_cpp->b,"\n");
    Printf(bg->b_val_cpp->b,"        %s::%s(\n", funcName, funcName);
    Printf(bg->b_val_cpp->b,"                const std::string& ObjectId,\n");
    emitParmList(parms, bg->b_val_cpp->b, 2, "rp_tm_val_prm", 4, ',', true, false, true);
    Printf(bg->b_val_cpp->b,"                bool Permanent) :\n");
    Printf(bg->b_val_cpp->b,"            ObjectHandler::ValueObject(ObjectId, \"%s\", Permanent),\n", funcName);
    emitParmList(parms, bg->b_val_cpp->b, 1, "rp_tm_val_ini", 3, ',', true, false, true);
    Printf(bg->b_val_cpp->b,"            Permanent_(Permanent) {\n");
    Printf(bg->b_val_cpp->b,"        }\n");
    }

    if (generateCtor) {
    Printf(bg->b_cre_hpp->b, "\n");
    Printf(bg->b_cre_hpp->b, "boost::shared_ptr<ObjectHandler::Object> create_%s(\n", funcName);
    Printf(bg->b_cre_hpp->b, "    const boost::shared_ptr<ObjectHandler::ValueObject>&);\n");

    Printf(bg->b_cre_cpp->b, "\n");
    Printf(bg->b_cre_cpp->b, "boost::shared_ptr<ObjectHandler::Object> %s::create_%s(\n", module, funcName);
    Printf(bg->b_cre_cpp->b, "    const boost::shared_ptr<ObjectHandler::ValueObject> &valueObject) {\n");
    Printf(bg->b_cre_cpp->b, "\n");
    Printf(bg->b_cre_cpp->b, "    // conversions\n\n");
    emitParmList(parms, bg->b_cre_cpp->b, 1, "rp_tm_cre_cnv", 1, 0);
    Printf(bg->b_cre_cpp->b, "\n");
    Printf(bg->b_cre_cpp->b, "    bool Permanent =\n");
    Printf(bg->b_cre_cpp->b, "        ObjectHandler::convert2<bool>(valueObject->getProperty(\"Permanent\"));\n");
    Printf(bg->b_cre_cpp->b, "\n");
    Printf(bg->b_cre_cpp->b, "    // construct and return the object\n");
    Printf(bg->b_cre_cpp->b, "\n");
    Printf(bg->b_cre_cpp->b, "    boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(bg->b_cre_cpp->b, "        new %s::%s(\n", module, name);
    Printf(bg->b_cre_cpp->b, "            valueObject,\n");
    emitParmList(parms, bg->b_cre_cpp->b, 0, "rp_tm_default", 3, ',', true, false, true);
    Printf(bg->b_cre_cpp->b, "            Permanent));\n");
    Printf(bg->b_cre_cpp->b, "    return object;\n");
    Printf(bg->b_cre_cpp->b, "}\n");
    }

    if (generateCtor) {
    Printf(bg->b_reg_cpp->b, "    // class ID %d in the boost serialization framework\n", idNum);
    Printf(bg->b_reg_cpp->b, "    ar.register_type<%s::ValueObjects::%s>();\n", module, funcName);

    Printf(bg->b_reg_cpp->b2, "    // class ID %d in the boost serialization framework\n", idNum);
    Printf(bg->b_reg_cpp->b2, "    ar.register_type<%s::ValueObjects::%s>();\n", module, funcName);
    }

    idNum++;

    if (generateCtor) {
        String *s0 = NewString("");
        String *s1 = NewString("");
        if (base) {
            s0 = base;
            s1 = NewStringf("%s::%s", nmspace, base);
        } else {
            s0 = NewStringf("ObjectHandler::LibraryObject<%s>", pname);
            s1 = pname;
        }
        Printf(bg->b_obj_hpp->b,"\n");
        Printf(bg->b_obj_hpp->b,"    class %s : \n", name);
        Printf(bg->b_obj_hpp->b,"        public %s {\n", s0);
        Printf(bg->b_obj_hpp->b,"    public:\n");
        Printf(bg->b_obj_hpp->b,"        %s(\n", name);
        Printf(bg->b_obj_hpp->b,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,\n");
        emitParmList(parms, bg->b_obj_hpp->b, 2, "rp_tm_default", 3, ',', true, false, true);
        Printf(bg->b_obj_hpp->b,"            bool permanent)\n");
        Printf(bg->b_obj_hpp->b,"        : %s(properties, permanent) {\n", s0);
        Printf(bg->b_obj_hpp->b,"            libraryObject_ = boost::shared_ptr<%s>(new %s(\n", s1, pname);
        emitParmList(parms, bg->b_obj_hpp->b, 0, "rp_tm_default", 4);
        Printf(bg->b_obj_hpp->b,"            ));\n");
        Printf(bg->b_obj_hpp->b,"        }\n");
        Printf(bg->b_obj_hpp->b,"    };\n");
        Printf(bg->b_obj_hpp->b,"\n");

        if (generateCppAddin) {
        Printf(bg->b_add_hpp->b,"\n");
        Printf(bg->b_add_hpp->b,"    std::string %s(\n", funcName);
        emitParmList(parms2, bg->b_add_hpp->b, 2, "rp_tm_add_prm", 2);
        Printf(bg->b_add_hpp->b,"    );\n\n");

        Printf(bg->b_add_cpp->b,"//****CTOR*****\n");
        Printf(bg->b_add_cpp->b,"std::string %s::%s(\n", addinCppNameSpace, funcName);
        emitParmList(parms2, bg->b_add_cpp->b, 2, "rp_tm_add_prm", 2);
        Printf(bg->b_add_cpp->b,"    ) {\n");
        Printf(bg->b_add_cpp->b,"\n");
        Printf(bg->b_add_cpp->b,"    // Convert input types into Library types\n\n");
        emitParmList(parms, bg->b_add_cpp->b, 1, "rp_tm_add_cnv", 1, 0, false);
        Printf(bg->b_add_cpp->b,"\n");
        Printf(bg->b_add_cpp->b,"    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
        Printf(bg->b_add_cpp->b,"        new %s::ValueObjects::%s(\n", module, funcName);
        Printf(bg->b_add_cpp->b,"            objectID,\n");
        emitParmList(parms, bg->b_add_cpp->b, 0, "rp_tm_default", 3, ',', true, false, true);
        Printf(bg->b_add_cpp->b,"            false));\n");
        Printf(bg->b_add_cpp->b,"    boost::shared_ptr<ObjectHandler::Object> object(\n");
        Printf(bg->b_add_cpp->b,"        new %s::%s(\n", module, name);
        Printf(bg->b_add_cpp->b,"            valueObject,\n");
        emitParmList(parms, bg->b_add_cpp->b, 1, "rp_tm_add_cll", 3, ',', true, true, true);
        Printf(bg->b_add_cpp->b,"            false));\n");
        Printf(bg->b_add_cpp->b,"    std::string returnValue =\n");
        Printf(bg->b_add_cpp->b,"        ObjectHandler::Repository::instance().storeObject(\n");
        Printf(bg->b_add_cpp->b,"            objectID, object, false, valueObject);\n");
        Printf(bg->b_add_cpp->b,"    return returnValue;\n");
        Printf(bg->b_add_cpp->b,"}\n\n");
        }

        Printf(b_cre_reg_cpp->b, "    registerCreator(\"%s\", create_%s);\n", funcName, funcName);
    } else {
        Printf(bg->b_obj_hpp->b, "    // BEGIN typemap rp_tm_obj_cls\n");
        Printf(bg->b_obj_hpp->b, "    OH_LIB_CLASS(%s, %s);\n", name, libraryClass);
        Printf(bg->b_obj_hpp->b, "    // END   typemap rp_tm_obj_cls\n");
    }

    if (generateXllAddin) {
    excelRegister(bg->b_xll_reg->b, n, 0, parms3);
    excelUnregister(bg->b_xll_reg->b2, n, type, parms3);

    if (generateCtor) {
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "DLLEXPORT char *%s(\n", funcName);
    emitParmList(parms2, bg->b_xll_cpp->b, 2, "rp_tm_xll_prm");
    Printf(bg->b_xll_cpp->b, ") {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    try {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bg->b_xll_cpp->b, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bg->b_xll_cpp->b, "\n");
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cnv", 2, 0, false);
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(bg->b_xll_cpp->b, "            new %s::ValueObjects::%s(\n", module, funcName);
    Printf(bg->b_xll_cpp->b, "                objectID,\n");
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cll_val", 4, ',', true, true, true);
    Printf(bg->b_xll_cpp->b, "                false));\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(bg->b_xll_cpp->b, "            new %s::%s(\n", module, name);
    Printf(bg->b_xll_cpp->b, "                valueObject,\n");
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cll_obj", 4, ',', true, true, true);
    Printf(bg->b_xll_cpp->b, "                false));\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        std::string returnValue =\n");
    Printf(bg->b_xll_cpp->b, "            ObjectHandler::RepositoryXL::instance().storeObject(objectID, object, true);\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        static char ret[XL_MAX_STR_LEN];\n");
    Printf(bg->b_xll_cpp->b, "        ObjectHandler::stringToChar(returnValue, ret);\n");
    Printf(bg->b_xll_cpp->b, "        return ret;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    } catch (const std::exception &e) {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bg->b_xll_cpp->b, "        return 0;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    }\n");
    Printf(bg->b_xll_cpp->b, "}\n");
    }
    }
    return SWIG_OK;
}

int memberfunctionHandlerImpl(Node *n) {
    functionType=2;
    return Language::memberfunctionHandler(n);
}

int functionWrapperImplMemb(Node *n) {
    if (generateCppAddin) {
    Printf(bg->b_add_cpp->b,"//****MEMB*****\n");
    }
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    Node *p = Getattr(n,"parentNode");
    String   *cls   = Getattr(p,"sym:name");
    String   *pname   = Getattr(p,"name");
    ParmList *parms  = Getattr(n,"parms");
    String *addinClass = NewStringf("%s::%s", module, cls);

    String *temp0 = copyUpper(cls);
    String *temp1 = copyUpper(name);
    String *funcName = NewStringf("%s%s%s", prefix, temp0, temp1);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every member.
    Parm *parms2 = NewHash();
    Setattr(parms2, "name", "objectID");
    String *nt  = NewString("std::string");
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

    if (generateCppAddin) {
    emitTypeMap(bg->b_add_hpp->b, "rp_tm_add_ret", n, type, 1);
    Printf(bg->b_add_hpp->b,"    %s(\n", funcName);
    emitParmList(parms2, bg->b_add_hpp->b, 2, "rp_tm_add_prm", 2);
    Printf(bg->b_add_hpp->b,"    );\n\n");

    emitTypeMap(bg->b_add_cpp->b, "rp_tm_add_ret", n, type);
    Printf(bg->b_add_cpp->b,"%s::%s(\n", addinCppNameSpace, funcName);
    emitParmList(parms2, bg->b_add_cpp->b, 2, "rp_tm_add_prm", 2);
    Printf(bg->b_add_cpp->b,"    ) {\n\n");
    emitParmList(parms, bg->b_add_cpp->b, 1, "rp_tm_add_cnv", 1, 0, false);
    Printf(bg->b_add_cpp->b,"\n");
    Printf(bg->b_add_cpp->b,"    OH_GET_REFERENCE(x, objectID, %s, %s);\n", addinClass, pname);
    Printf(bg->b_add_cpp->b,"    return x->%s(\n", name);
    emitParmList(parms, bg->b_add_cpp->b, 1, "rp_tm_add_cll", 3, ',', true, true);
    Printf(bg->b_add_cpp->b,"        );\n", name);
    Printf(bg->b_add_cpp->b,"}\n");
    }

    if (generateXllAddin) {
    excelRegister(bg->b_xll_reg->b, n, type, parms2);
    excelUnregister(bg->b_xll_reg->b2, n, type, parms2);

    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "DLLEXPORT\n");
    emitTypeMap(bg->b_xll_cpp->b, "rp_tm_xll_ret", n, type);
    Printf(bg->b_xll_cpp->b, "%s(\n", funcName);
    emitParmList(parms2, bg->b_xll_cpp->b, 2, "rp_tm_xll_prm");
    Printf(bg->b_xll_cpp->b, ") {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    try {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    Printf(bg->b_xll_cpp->b, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    Printf(bg->b_xll_cpp->b, "\n");
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cnv", 2, 0, false);
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        OH_GET_REFERENCE(x, objectID, %s, %s);\n", addinClass, pname);
    Printf(bg->b_xll_cpp->b, "\n");
    emitTypeMap(bg->b_xll_cpp->b, "rp_xll_get", n, type, 2);
    Printf(bg->b_xll_cpp->b, "        x->%s(\n", name);
    emitParmList(parms, bg->b_xll_cpp->b, 1, "rp_tm_xll_cll_obj", 3, ',', true, true);
    Printf(bg->b_xll_cpp->b, "        );\n\n");
    emitTypeMap(bg->b_xll_cpp->b, "rp_tm_xll_rdc", n, type, 2);
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    } catch (const std::exception &e) {\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    Printf(bg->b_xll_cpp->b, "        return 0;\n");
    Printf(bg->b_xll_cpp->b, "\n");
    Printf(bg->b_xll_cpp->b, "    }\n");
    Printf(bg->b_xll_cpp->b, "}\n");
    }
    return SWIG_OK;
}

void functionWrapperImplAll(Node *n) {
    String *group = Getattr(n,"feature:rp:group");
    String *include = Getattr(n,"feature:rp:include");

    // Check whether to generate all source code, or to omit some code to be handwritten by the user.
    // For the user writing the config file, it is easier to assume automatic (default)
    // unless overridden with '%feature("rp:generation", "manual");' :
    bool manual = 0 != checkAttribute(n, "feature:rp:generation", "manual");
    // The source code for this SWIG module is cleaner if we think of it the opposite way:
    automatic = !manual;

    //SwigType *type   = Getattr(n,"type");
    //xxx = getTypeMap("rp_tm_obj_cls", n, type, false);
    bg = bm_.getBufferGroup (group, include, automatic);

    // In the *.i files, if there is a parameter with no name,
    // that would cause a segfault later, so trap it here.
    String *nodeName = Getattr(n, "name");
    printf("Processing node name '%s'.\n", Char(nodeName));
    printf("Group='%s'.\n", Char(group));
    ParmList *parms  = Getattr(n,"parms");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *name = Getattr(p,"name");
        if (!name) {
            printf("parameter has no name\n");
            SWIG_exit(EXIT_FAILURE);
        }
        if (!Getattr(p, "type")) {
            printf ("parameter has no type\n");
            SWIG_exit(EXIT_FAILURE);
        }
        String *nameUpper = copyUpper2(name);
        Setattr(p, "nameUpper", nameUpper);
    }

    Printf(b_wrappers,"//XXX***functionWrapper*******\n");
    Printf(b_wrappers,"//module=%s\n", module);
    Printf(b_wrappers,"//group=%s\n", group);
    printNode(n);
    printList(Getattr(n, "parms"));
    Printf(b_wrappers,"//*************\n");
}
}; // class REPOSIT

extern "C" Language *
swig_reposit(void) {
  return new REPOSIT();
}

