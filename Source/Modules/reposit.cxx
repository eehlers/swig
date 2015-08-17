
#include "swigmod.h"
#include <string>
#include <vector>
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
String *parent = 0;

// Default names for directories for source code and headers.
// FIXME store these defaults in reposit.swg and retrieve them here.
String *objDir = NewString("AddinObjects");
String *addDir = NewString("AddinCpp");
String *cfyDir = NewString("AddinCfy");
String *xllDir = NewString("AddinXl");
String *objInc = NewString("AddinObjects");
String *addInc = NewString("AddinCpp");
String *xllInc = NewString("AddinXl");
String *cfyInc = NewString("AddinCfy");

// Features
String *obj_include = 0;
String *add_include = 0;
String *cfy_include = 0;
String *group_name = 0;
bool automatic = false;

struct Buffer;

// global buffers

Buffer *b_obj_all_hpp=0;
Buffer *b_cre_reg_cpp=0;
Buffer *b_cre_all_hpp=0;
Buffer *b_reg_ser_hpp=0;
Buffer *b_reg_all_hpp=0;
//Buffer *b_add_all_hpp=0;
//Buffer *b_cfy_mng_txt=0;
//Buffer *b_xll_reg_cpp=0;

List *errorList = NewList();

//bool generateXllAddin = false;
//bool generateCppAddin = false;
//bool generateCfyAddin = false;

// BEGIN *************************

Node *getNode(Node *n, const char *c) {
    Node *ret = 0;
    if (!(ret = Getattr(n, c))) {
        printf ("can't find node %s.\n", c);
        SWIG_exit(EXIT_FAILURE);
    }
    return ret;
}

void printNode(Node *n, File *f) {
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        Printf(f,"/* %d %s %s */\n", i, key, Getattr(n, key));
    }
}

void printList(Node *n, File *f) {
    while (n) {
        printNode(n, f);
        n = Getattr(n,"nextSibling");
    }
}

String *getTypeMap(const char *m, Node *n, SwigType *t, bool fatal = true) {
    if (String *tm = Swig_typemap_lookup(m, n, "", 0)) {
        Replaceall(tm, "$rp_typedef_resolved", Getattr(n, "rp_typedef_resolved"));
        Replaceall(tm, "$rp_typedef_raw", Getattr(n, "rp_typedef_raw"));
        Replaceall(tm, "$rp_typedef_obj_add", Getattr(n, "rp_typedef_obj_add"));
        Replaceall(tm, "$rp_typedef_obj_lib", Getattr(n, "rp_typedef_obj_lib"));
        return tm;
    }
    if (fatal) {
        Append(errorList, NewStringf("*** ERROR : typemap '%s' does not match type '%s'.\n", m, Char(SwigType_str(t, 0))));
        // Do not exit, instead keep running so that the user can see any other error messages.
        //SWIG_exit(EXIT_FAILURE);
        // Return an error string, this will be inserted into the source code.
        return NewStringf("#error NEED THIS TYPEMAP: >>> %%typemap(%s) %s \"XXX\"; <<<", m, SwigType_str(t, 0));
    }
    return 0;
}

String *getType(Parm *p, const char *m, bool fatal) {
    SwigType *t  = Getattr(p, "type");
    if (0==strcmp(m, "rp_tm_default"))
        return t;
    else {
        String *s = getTypeMap(m, p, t, fatal);
        return s;
    }
}

void printIndent(File *buf, int indent) {
    for (int i=0;i<indent;++i)
        Printf(buf, "    ");
}

void emitTypeMap(File *buf, const char *m, Node *n, SwigType *t, int indent=0, bool fatal = true) {
    printIndent(buf, indent);
    Printf(buf, "// BEGIN typemap %s %s\n", m, t);
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

void voGetProp(File *f, ParmList *parms) {
    Printf(f, "            // BEGIN func voGetProp\n");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *name = Getattr(p,"name");
        String *nameUpper = Getattr(p,"rp_name_upper");
        Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
        Printf(f, "                return %s_;\n", name);
    }
    Printf(f, "            // END   func voGetProp\n");
}

void voSetProp(File *f, ParmList *parms) {
    Printf(f, "            // BEGIN func voSetProp (using typemap rp_tm_val_cnv)\n");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *name = Getattr(p,"name");
        String *nameUpper = Getattr(p,"rp_name_upper");
        SwigType *type = Getattr(p,"type");
        String *cnv = getTypeMap("rp_tm_val_cnv", p, type);
        Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
        Printf(f, "                %s_ = %s;\n", name, cnv);
    }
    Printf(f, "            // END   func voSetProp (using typemap rp_tm_val_cnv)\n");
}

// END ***************************

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
    File *b0;
    File *b1;
    File *b2;
    File *b3;
    String *outputBuffer_;
    Buffer(String *name) : name_(name) {
        b0 = NewString("");
        b1 = NewString("");
        b2 = NewString("");
        b3 = NewString("");
    }
    bool fileChanged() {
        FILE *f = Swig_open(name_);
        if (!f)
            return true;
        String *s = Swig_read_file(f);
        return (0!=Strcmp(s, outputBuffer_));
    }
    ~Buffer() {
        printf("Generating file '%s'...", Char(name_));
        outputBuffer_ = NewString("");
        Dump(b0, outputBuffer_);
        Dump(b1, outputBuffer_);
        Dump(b2, outputBuffer_);
        Dump(b3, outputBuffer_);
        Delete(b0);
        Delete(b1);
        Delete(b2);
        Delete(b3);
        if (fileChanged()) {
            File *f = initFile(name_);
            Dump(outputBuffer_, f);
            Delete(f);
            printf("Done.\n");
        } else {
            printf("Unchanged.\n");
        }
        Delete(outputBuffer_);
        Delete(name_);
    }
};

struct GroupObjects {

    std::string name_;
    String *obj_include_;

    Buffer *b_val_hpp;
    Buffer *b_val_cpp;
    Buffer *b_cre_hpp;
    Buffer *b_cre_cpp;
    Buffer *b_reg_hpp;
    Buffer *b_reg_cpp;
    Buffer *b_obj_hpp;
    Buffer *b_obj_cpp;

    GroupObjects() {

        obj_include_ = Copy(obj_include);

        b_val_hpp = new Buffer(NewStringf("%s/valueobjects/vo_%s.hpp", objDir, group_name));
        b_val_cpp = new Buffer(NewStringf("%s/valueobjects/vo_%s.cpp", objDir, group_name));
        b_cre_hpp = new Buffer(NewStringf("%s/serialization/create/create_%s.hpp", objDir, group_name));
        b_cre_cpp = new Buffer(NewStringf("%s/serialization/create/create_%s.cpp", objDir, group_name));
        b_reg_hpp = new Buffer(NewStringf("%s/serialization/register/serialization_%s.hpp", objDir, group_name));
        b_reg_cpp = new Buffer(NewStringf("%s/serialization/register/serialization_%s.cpp", objDir, group_name));

        if (automatic) {
            b_obj_hpp = new Buffer(NewStringf("%s/obj_%s.hpp", objDir, group_name));
            b_obj_cpp = new Buffer(NewStringf("%s/obj_%s.cpp", objDir, group_name));
        } else {
            b_obj_hpp = new Buffer(NewStringf("%s/objmanual_%s.hpp.template", objDir, group_name));
            b_obj_cpp = new Buffer(NewStringf("%s/objmanual_%s.cpp.template", objDir, group_name));
        }

        Printf(b_val_hpp->b0, "\n");
        Printf(b_val_hpp->b0, "#ifndef vo_%s_hpp\n", group_name);
        Printf(b_val_hpp->b0, "#define vo_%s_hpp\n", group_name);
        Printf(b_val_hpp->b0, "\n");
        Printf(b_val_hpp->b0, "#include <oh/valueobject.hpp>\n");
        Printf(b_val_hpp->b0, "#include <string>\n");
        Printf(b_val_hpp->b0, "#include <vector>\n");
        Printf(b_val_hpp->b0, "#include <set>\n");
        Printf(b_val_hpp->b0, "#include <boost/serialization/map.hpp>\n");
        Printf(b_val_hpp->b0, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_val_hpp->b0, "\n");
        Printf(b_val_hpp->b0,"namespace %s {\n", module);
        Printf(b_val_hpp->b0, "\n");
        Printf(b_val_hpp->b0, "namespace ValueObjects {\n");
        Printf(b_val_hpp->b0, "\n");

        Printf(b_val_cpp->b0, "\n");
        Printf(b_val_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, group_name);
        Printf(b_val_cpp->b0, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_val_cpp->b0, "\n");
        Printf(b_val_cpp->b0,"namespace %s {\n", module);
        Printf(b_val_cpp->b0, "\n");
        Printf(b_val_cpp->b0, "namespace ValueObjects {\n");
        Printf(b_val_cpp->b0, "\n");

        Printf(b_cre_hpp->b0, "\n");
        Printf(b_cre_hpp->b0, "#ifndef create_%s_hpp\n", group_name);
        Printf(b_cre_hpp->b0, "#define create_%s_hpp\n", group_name);
        Printf(b_cre_hpp->b0, "\n");
        Printf(b_cre_hpp->b0, "#include <oh/ohdefines.hpp>\n");
        Printf(b_cre_hpp->b0, "#include <oh/object.hpp>\n");
        Printf(b_cre_hpp->b0, "#include <oh/valueobject.hpp>\n");
        Printf(b_cre_hpp->b0, "\n");
        Printf(b_cre_hpp->b0, "namespace %s {\n", module);
        Printf(b_cre_hpp->b0, "\n");

        Printf(b_cre_cpp->b0, "\n");
        Printf(b_cre_cpp->b0, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, group_name);
        Printf(b_cre_cpp->b0, "//#include <%s/qladdindefines.hpp>\n", objInc);
        Printf(b_cre_cpp->b0, "#include <%s/conversions/convert2.hpp>\n", objInc);
        Printf(b_cre_cpp->b0, "//#include <%s/handle.hpp>\n", objInc);
        Printf(b_cre_cpp->b0, "\n");
        if (automatic) {
            Printf(b_cre_cpp->b0, "#include <%s/obj_%s.hpp>\n", objInc, group_name);
        } else {
            Printf(b_cre_cpp->b0, "#include <%s/objmanual_%s.hpp>\n", objInc, group_name);
        }
        Printf(b_cre_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, group_name);
        Printf(b_cre_cpp->b0, "\n");
        Printf(b_cre_cpp->b0, "//#include <%s/conversions/all.hpp>\n", objInc);
        Printf(b_cre_cpp->b0, "#include <oh/property.hpp>\n");
        Printf(b_cre_cpp->b0, "\n");

        Printf(b_reg_hpp->b0, "\n");
        Printf(b_reg_hpp->b0, "#ifndef serialization_%s_hpp\n", group_name);
        Printf(b_reg_hpp->b0, "#define serialization_%s_hpp\n", group_name);
        Printf(b_reg_hpp->b0, "\n");
        Printf(b_reg_hpp->b0, "#include <boost/archive/xml_iarchive.hpp>\n");
        Printf(b_reg_hpp->b0, "#include <boost/archive/xml_oarchive.hpp>\n");
        Printf(b_reg_hpp->b0, "\n");
        Printf(b_reg_hpp->b0, "namespace %s {\n", module);
        Printf(b_reg_hpp->b0, "\n");
        Printf(b_reg_hpp->b0, "    void register_%s(boost::archive::xml_oarchive &ar);\n", group_name);
        Printf(b_reg_hpp->b0, "    void register_%s(boost::archive::xml_iarchive &ar);\n", group_name);
        Printf(b_reg_hpp->b0, "\n");
        Printf(b_reg_hpp->b0, "} // namespace %s\n", module);
        Printf(b_reg_hpp->b0, "\n");
        Printf(b_reg_hpp->b0, "#endif\n");
        Printf(b_reg_hpp->b0, "\n");

        Printf(b_reg_cpp->b0, "\n");
        Printf(b_reg_cpp->b0, "#include <oh/ohdefines.hpp>\n");
        Printf(b_reg_cpp->b0, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, group_name);
        Printf(b_reg_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, group_name);
        Printf(b_reg_cpp->b0, "#include <boost/serialization/shared_ptr.hpp>\n");
        Printf(b_reg_cpp->b0, "#include <boost/serialization/variant.hpp>\n");
        Printf(b_reg_cpp->b0, "#include <boost/serialization/vector.hpp>\n");
        Printf(b_reg_cpp->b0, "\n");
        Printf(b_reg_cpp->b0, "void %s::register_%s(boost::archive::xml_oarchive &ar) {\n", module, group_name);
        Printf(b_reg_cpp->b0, "\n");

        Printf(b_reg_cpp->b1, "\n");
        Printf(b_reg_cpp->b1, "void %s::register_%s(boost::archive::xml_iarchive &ar) {\n", module, group_name);
        Printf(b_reg_cpp->b1, "\n");

        Printf(b_obj_hpp->b0, "\n");
        Printf(b_obj_hpp->b0, "#ifndef obj_%s_hpp\n", group_name);
        Printf(b_obj_hpp->b0, "#define obj_%s_hpp\n", group_name);
        Printf(b_obj_hpp->b0, "\n");
        Printf(b_obj_hpp->b0, "#include <string>\n");
        Printf(b_obj_hpp->b0, "#include <oh/libraryobject.hpp>\n");
        Printf(b_obj_hpp->b0, "#include <oh/valueobject.hpp>\n");
        Printf(b_obj_hpp->b0, "#include <boost/shared_ptr.hpp>");
        Printf(b_obj_hpp->b0, "%s\n", obj_include_);
        Printf(b_obj_hpp->b0, "using namespace %s;\n", nmspace);
        Printf(b_obj_hpp->b0, "\n");
        Printf(b_obj_hpp->b0,"namespace %s {\n", module);

        Printf(b_obj_cpp->b0, "\n");
        if (automatic) {
            Printf(b_obj_cpp->b0, "#include <%s/obj_%s.hpp>\n", objInc, group_name);
        } else {
            Printf(b_obj_cpp->b0, "#include <%s/objmanual_%s.hpp>\n", objInc, group_name);
        }
        Printf(b_obj_cpp->b0, "\n");

        // write to global buffers

        if (generateCtor) {
            Printf(b_reg_ser_hpp->b0, "        register_%s(ar);\n", group_name);
        }

        if (automatic) {
            Printf(b_obj_all_hpp->b0, "#include <%s/obj_%s.hpp>\n", objInc, group_name);
        } else {
            Printf(b_obj_all_hpp->b0, "#include <%s/objmanual_%s.hpp>\n", objInc, group_name);
        }

        Printf(b_cre_all_hpp->b0, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, group_name);

        Printf(b_reg_all_hpp->b0, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, group_name);
    }

    ~GroupObjects() {

        Printf(b_val_hpp->b0, "} // namespace %s\n", module);
        Printf(b_val_hpp->b0, "\n");
        Printf(b_val_hpp->b0, "} // namespace ValueObjects\n");
        Printf(b_val_hpp->b0, "\n");
        Printf(b_val_hpp->b0, "#endif\n");
        Printf(b_val_hpp->b0, "\n");

        Printf(b_val_cpp->b0, "\n");
        Printf(b_val_cpp->b0, "} // namespace %s\n", module);
        Printf(b_val_cpp->b0, "\n");
        Printf(b_val_cpp->b0, "} // namespace ValueObjects\n");
        Printf(b_val_cpp->b0, "\n");

        Printf(b_cre_hpp->b0, "\n");
        Printf(b_cre_hpp->b0, "} // namespace %s\n", module);
        Printf(b_cre_hpp->b0, "\n");
        Printf(b_cre_hpp->b0, "#endif\n");
        Printf(b_cre_hpp->b0, "\n");

        Printf(b_cre_cpp->b0, "\n");

        Printf(b_reg_hpp->b0, "\n");

        Printf(b_reg_cpp->b0, "}\n");
        Printf(b_reg_cpp->b0, "\n");

        Printf(b_reg_cpp->b1, "}\n");
        Printf(b_reg_cpp->b1, "\n");

        Printf(b_obj_hpp->b0, "} // namespace %s\n", module);
        Printf(b_obj_hpp->b0, "\n");
        Printf(b_obj_hpp->b0, "#endif\n");
        Printf(b_obj_hpp->b0, "\n");

        Printf(b_obj_cpp->b0, "\n");

        delete b_val_hpp;
        delete b_val_cpp;
        delete b_cre_hpp;
        delete b_cre_cpp;
        delete b_reg_hpp;
        delete b_reg_cpp;
        delete b_obj_hpp;
        delete b_obj_cpp;
    }

    virtual void functionWrapperImplFunc(Node *n, ParmList *parms, String *name, SwigType *type, String *symname) {

        Printf(b_obj_hpp->b0,"\n");
        emitTypeMap(b_obj_hpp->b0, "rp_tm_obj_ret", n, type, 1);
        Printf(b_obj_hpp->b0,"    %s(\n", symname);
        emitParmList(parms, b_obj_hpp->b0, 2, "rp_tm_default", 2);
        Printf(b_obj_hpp->b0,"    );\n");

        emitTypeMap(b_obj_cpp->b0, "rp_tm_obj_ret", n, type);
        Printf(b_obj_cpp->b0,"%s::%s(\n", module, symname);
        emitParmList(parms, b_obj_cpp->b0, 2, "rp_tm_default", 2);
        Printf(b_obj_cpp->b0,"    ) {\n");
        emitTypeMap(b_obj_cpp->b0, "rp_tm_obj_rdc", n, type, 2);
        Printf(b_obj_cpp->b0,"        %s(\n", name);
        emitParmList(parms, b_obj_cpp->b0, 0, "rp_tm_default", 3, ',', true, true);
        Printf(b_obj_cpp->b0,"        );\n");
        Printf(b_obj_cpp->b0,"}\n");
    }

    virtual void functionWrapperImplCtor(Node *n, String *name, bool generateCtor, String *funcName, ParmList *parms, String *pname, String *base) {

        if (generateCtor) {

            Printf(b_val_hpp->b0,"        class %s : public ObjectHandler::ValueObject {\n", funcName);
            Printf(b_val_hpp->b0,"            friend class boost::serialization::access;\n");
            Printf(b_val_hpp->b0,"        public:\n");
            Printf(b_val_hpp->b0,"            %s() {}\n", funcName);
            Printf(b_val_hpp->b0,"            %s(\n", funcName);
            Printf(b_val_hpp->b0,"                const std::string& ObjectId,\n");
            emitParmList(parms, b_val_hpp->b0, 2, "rp_tm_val_prm", 4, ',', true, false, true);
            Printf(b_val_hpp->b0,"                bool Permanent);\n");
            Printf(b_val_hpp->b0,"\n");
            Printf(b_val_hpp->b0,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
            Printf(b_val_hpp->b0,"            std::vector<std::string> getPropertyNamesVector() const;\n");
            Printf(b_val_hpp->b0,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
            Printf(b_val_hpp->b0,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
            Printf(b_val_hpp->b0,"\n");
            Printf(b_val_hpp->b0,"        protected:\n");
            Printf(b_val_hpp->b0,"            static const char* mPropertyNames[];\n");
            Printf(b_val_hpp->b0,"            static std::set<std::string> mSystemPropertyNames;\n");
            emitParmList(parms, b_val_hpp->b0, 1, "rp_tm_val_dcl", 3, ';', true, false, true);
            Printf(b_val_hpp->b0,"            bool Permanent_;\n");
            Printf(b_val_hpp->b0,"\n");
            Printf(b_val_hpp->b0,"            template<class Archive>\n");
            Printf(b_val_hpp->b0,"            void serialize(Archive& ar, const unsigned int) {\n");
            Printf(b_val_hpp->b0,"            boost::serialization::void_cast_register<%s, ObjectHandler::ValueObject>(this, this);\n", funcName);
            Printf(b_val_hpp->b0,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
            Printf(b_val_hpp->b0,"                    & boost::serialization::make_nvp(\"ClassName\", className_)\n");
            emitParmList(parms, b_val_hpp->b0, 1, "rp_tm_val_ser", 5, 0);
            Printf(b_val_hpp->b0,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
            Printf(b_val_hpp->b0,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
            Printf(b_val_hpp->b0,"            }\n");
            Printf(b_val_hpp->b0,"        };\n");
            Printf(b_val_hpp->b0,"\n");

            Printf(b_val_cpp->b0,"        const char* %s::mPropertyNames[] = {\n", funcName);
            emitParmList(parms, b_val_cpp->b0, 1, "rp_tm_val_nam", 3, ',', true, false, true);
            Printf(b_val_cpp->b0,"            \"Permanent\"\n");
            Printf(b_val_cpp->b0,"        };\n");
            Printf(b_val_cpp->b0,"\n");
            Printf(b_val_cpp->b0,"        std::set<std::string> %s::mSystemPropertyNames(\n", funcName);
            Printf(b_val_cpp->b0,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
            Printf(b_val_cpp->b0,"\n");
            Printf(b_val_cpp->b0,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", funcName);
            Printf(b_val_cpp->b0,"            return mSystemPropertyNames;\n");
            Printf(b_val_cpp->b0,"        }\n");
            Printf(b_val_cpp->b0,"\n");
            Printf(b_val_cpp->b0,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", funcName);
            Printf(b_val_cpp->b0,"            std::vector<std::string> ret(\n");
            Printf(b_val_cpp->b0,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
            Printf(b_val_cpp->b0,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
            Printf(b_val_cpp->b0,"                i != userProperties.end(); ++i)\n");
            Printf(b_val_cpp->b0,"                ret.push_back(i->first);\n");
            Printf(b_val_cpp->b0,"            return ret;\n");
            Printf(b_val_cpp->b0,"        }\n");
            Printf(b_val_cpp->b0,"\n");
            Printf(b_val_cpp->b0,"        ObjectHandler::property_t %s::getSystemProperty(const std::string& name) const {\n", funcName);
            Printf(b_val_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
            Printf(b_val_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
            Printf(b_val_cpp->b0,"                return objectId_;\n");
            Printf(b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
            Printf(b_val_cpp->b0,"                return className_;\n");
            voGetProp(b_val_cpp->b0, parms);
            Printf(b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
            Printf(b_val_cpp->b0,"                return Permanent_;\n");
            Printf(b_val_cpp->b0,"            else\n");
            Printf(b_val_cpp->b0,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
            Printf(b_val_cpp->b0,"        }\n");
            Printf(b_val_cpp->b0,"\n");
            Printf(b_val_cpp->b0,"        void %s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", funcName);
            Printf(b_val_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
            Printf(b_val_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
            Printf(b_val_cpp->b0,"                objectId_ = boost::get<std::string>(value);\n");
            Printf(b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
            Printf(b_val_cpp->b0,"                className_ = boost::get<std::string>(value);\n");
            voSetProp(b_val_cpp->b0, parms);
            Printf(b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
            Printf(b_val_cpp->b0,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
            Printf(b_val_cpp->b0,"            else\n");
            Printf(b_val_cpp->b0,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
            Printf(b_val_cpp->b0,"        }\n");
            Printf(b_val_cpp->b0,"\n");
            Printf(b_val_cpp->b0,"        %s::%s(\n", funcName, funcName);
            Printf(b_val_cpp->b0,"                const std::string& ObjectId,\n");
            emitParmList(parms, b_val_cpp->b0, 2, "rp_tm_val_prm", 4, ',', true, false, true);
            Printf(b_val_cpp->b0,"                bool Permanent) :\n");
            Printf(b_val_cpp->b0,"            ObjectHandler::ValueObject(ObjectId, \"%s\", Permanent),\n", funcName);
            emitParmList(parms, b_val_cpp->b0, 1, "rp_tm_val_ini", 3, ',', true, false, true);
            Printf(b_val_cpp->b0,"            Permanent_(Permanent) {\n");
            Printf(b_val_cpp->b0,"        }\n");

            Printf(b_cre_hpp->b0, "\n");
            Printf(b_cre_hpp->b0, "boost::shared_ptr<ObjectHandler::Object> create_%s(\n", funcName);
            Printf(b_cre_hpp->b0, "    const boost::shared_ptr<ObjectHandler::ValueObject>&);\n");

            Printf(b_cre_cpp->b0, "\n");
            Printf(b_cre_cpp->b0, "boost::shared_ptr<ObjectHandler::Object> %s::create_%s(\n", module, funcName);
            Printf(b_cre_cpp->b0, "    const boost::shared_ptr<ObjectHandler::ValueObject> &valueObject) {\n");
            Printf(b_cre_cpp->b0, "\n");
            Printf(b_cre_cpp->b0, "    // conversions\n\n");
            emitParmList(parms, b_cre_cpp->b0, 1, "rp_tm_cre_cnv", 1, 0);
            Printf(b_cre_cpp->b0, "\n");
            Printf(b_cre_cpp->b0, "    bool Permanent =\n");
            Printf(b_cre_cpp->b0, "        ObjectHandler::convert2<bool>(valueObject->getProperty(\"Permanent\"));\n");
            Printf(b_cre_cpp->b0, "\n");
            Printf(b_cre_cpp->b0, "    // construct and return the object\n");
            Printf(b_cre_cpp->b0, "\n");
            Printf(b_cre_cpp->b0, "    boost::shared_ptr<ObjectHandler::Object> object(\n");
            Printf(b_cre_cpp->b0, "        new %s::%s(\n", module, name);
            Printf(b_cre_cpp->b0, "            valueObject,\n");
            emitParmList(parms, b_cre_cpp->b0, 0, "rp_tm_default", 3, ',', true, false, true);
            Printf(b_cre_cpp->b0, "            Permanent));\n");
            Printf(b_cre_cpp->b0, "    return object;\n");
            Printf(b_cre_cpp->b0, "}\n");

            Printf(b_reg_cpp->b0, "    // class ID %d in the boost serialization framework\n", idNum);
            Printf(b_reg_cpp->b0, "    ar.register_type<%s::ValueObjects::%s>();\n", module, funcName);

            Printf(b_reg_cpp->b1, "    // class ID %d in the boost serialization framework\n", idNum);
            Printf(b_reg_cpp->b1, "    ar.register_type<%s::ValueObjects::%s>();\n", module, funcName);

            idNum++;

            String *s0 = NewString("");
            String *s1 = NewString("");
            if (base) {
                s0 = base;
                s1 = NewStringf("%s::%s", nmspace, base);
            } else {
                s0 = NewStringf("ObjectHandler::LibraryObject<%s>", pname);
                s1 = pname;
            }
            Printf(b_obj_hpp->b0,"\n");
            Printf(b_obj_hpp->b0,"    class %s : \n", name);
            Printf(b_obj_hpp->b0,"        public %s {\n", s0);
            Printf(b_obj_hpp->b0,"    public:\n");
            Printf(b_obj_hpp->b0,"        %s(\n", name);
            Printf(b_obj_hpp->b0,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,\n");
            emitParmList(parms, b_obj_hpp->b0, 2, "rp_tm_default", 3, ',', true, false, true);
            Printf(b_obj_hpp->b0,"            bool permanent)\n");
            Printf(b_obj_hpp->b0,"        : %s(properties, permanent) {\n", s0);
            Printf(b_obj_hpp->b0,"            libraryObject_ = boost::shared_ptr<%s>(new %s(\n", s1, pname);
            emitParmList(parms, b_obj_hpp->b0, 0, "rp_tm_default", 4);
            Printf(b_obj_hpp->b0,"            ));\n");
            Printf(b_obj_hpp->b0,"        }\n");
            Printf(b_obj_hpp->b0,"    };\n");
            Printf(b_obj_hpp->b0,"\n");

            Printf(b_cre_reg_cpp->b0, "    registerCreator(\"%s\", create_%s);\n", funcName, funcName);

        } else { //!generateCtor

            Printf(b_obj_hpp->b0, "    // BEGIN typemap rp_tm_obj_cls\n");
            if (parent) {
                Printf(b_obj_hpp->b0, "    OH_OBJ_CLASS(%s, %s);\n", name, parent);
            } else {
                Printf(b_obj_hpp->b0, "    OH_LIB_CLASS(%s, %s);\n", name, libraryClass);
            }
            Printf(b_obj_hpp->b0, "    // END   typemap rp_tm_obj_cls\n");
        }
    }
};

struct Addin {

    virtual void functionWrapperImplFunc(Node *n, ParmList *parms, String *name, SwigType *type, String *symname) = 0;
    virtual void functionWrapperImplCtor(Node *n, String *name, bool generateCtor, String *funcName, ParmList *parms, String *pname, String *base) = 0;
    virtual void clear() = 0;
};

template <class Group>
struct AddinImpl : public Addin {

    typedef std::map<std::string, Group*> GroupMap;
    GroupMap groupMap_;

    Group *getGroup() {
        std::string name_ = Char(group_name);
        if (groupMap_.end() == groupMap_.find(name_))
            groupMap_[name_] = new Group;
        return groupMap_[name_];
    }

    virtual void clear() {
        for (GroupMap::const_iterator i=groupMap_.begin(); i!=groupMap_.end(); ++i)
            delete i->second;
    }

    virtual void functionWrapperImplFunc(Node *n, ParmList *parms, String *name, SwigType *type, String *symname) {
        Group *group = getGroup();
        group->functionWrapperImplFunc(n, parms, name, type, symname);
    }

    virtual void functionWrapperImplCtor(Node *n, String *name, bool generateCtor, String *funcName, ParmList *parms, String *pname, String *base) {
        Group *group = getGroup();
        group->functionWrapperImplCtor(n, name, generateCtor, funcName, parms, pname, base);
    }
};

struct AddinList {

    std::vector<Addin*> addinList_;
    typedef std::vector<Addin*>::const_iterator iter;

    void appendAddin(Addin *addin) {
        addinList_.push_back(addin);
    }

    void clear() {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->clear();
        }
    }

    void functionWrapperImplFunc(Node *n, ParmList *parms, String *name, SwigType *type, String *symname) {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->functionWrapperImplFunc(n, parms, name, type, symname);
        }
    }

    void functionWrapperImplCtor(Node *n, String *name, bool generateCtor, String *funcName, ParmList *parms, String *pname, String *base) {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->functionWrapperImplCtor(n, name, generateCtor, funcName, parms, pname, base);
        }
    }
};

//struct BufferGroup {
//
//    // buffers
//    Buffer *b_add_hpp;
//    Buffer *b_add_cpp;
//    Buffer *b_cfy_hpp;
//    Buffer *b_cfy_cpp;
//    Buffer *b_xll_cpp;
//    Buffer *b_xll_reg;
//
//    String *name_;
//    bool automatic_;
//
//    BufferGroup(String *name, String *obj_include, String *add_include, String *cfy_include, bool automatic) {
//
//        name_ = Copy(name);
//        automatic_ = automatic;
//
//        if (generateCppAddin) {
//        b_add_hpp = new Buffer(NewStringf("%s/add_%s.hpp", addDir, name_));
//        b_add_cpp = new Buffer(NewStringf("%s/add_%s.cpp", addDir, name_));
//        }
//        if (generateXllAddin) {
//        b_xll_cpp = new Buffer(NewStringf("%s/functions/function_%s.cpp", xllDir, name_));
//        b_xll_reg = new Buffer(NewStringf("%s/register/register_%s.cpp", xllDir, name_));
//        }
//        if (generateCfyAddin) {
//        b_cfy_hpp = new Buffer(NewStringf("%s/cfy_%s.hpp", cfyDir, name_));
//        b_cfy_cpp = new Buffer(NewStringf("%s/cfy_%s.cpp", cfyDir, name_));
//        }

//        if (generateCppAddin) {
//        Printf(b_add_hpp->b0, "\n");
//        Printf(b_add_hpp->b0, "#ifndef add_%s_hpp\n", name);
//        Printf(b_add_hpp->b0, "#define add_%s_hpp\n", name);
//        Printf(b_add_hpp->b0, "\n");
//        Printf(b_add_hpp->b0, "#include <string>\n");
//        // FIXME this #include is only needed if a datatype conversion is taking place.
//        Printf(b_add_hpp->b0, "#include <oh/property.hpp>\n");
//        Printf(b_add_hpp->b0, "\n");
//        Printf(b_add_hpp->b0, "namespace %s {\n", addinCppNameSpace);
//        Printf(b_add_hpp->b0, "\n");
//
//        Printf(b_add_cpp->b0, "\n");
//        Printf(b_add_cpp->b0, "#include <AddinCpp/add_%s.hpp>\n", name);
//        // FIXME this #include is only required if the file contains conversions.
//        Printf(b_add_cpp->b0, "#include <%s/conversions/convert2.hpp>\n", objInc);
//        Printf(b_add_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
//        // FIXME this #include is only required if the file contains enumerations.
//        //Printf(b_add_cpp->b0, "#include <oh/enumerations/typefactory.hpp>\n");
//        // FIXME this #include is only required if the file contains constructors.
//        Printf(b_add_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, name);
//        if (automatic_) {
//        Printf(b_add_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, name);
//        } else {
//        Printf(b_add_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, name);
//        }
//        Printf(b_add_cpp->b0, "%s\n", add_include);
//        // FIXME include only factories for types used in the current file.
//        Printf(b_add_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
//        Printf(b_add_cpp->b0, "#include <boost/shared_ptr.hpp>\n");
//        Printf(b_add_cpp->b0, "#include <oh/repository.hpp>\n");
//        //Printf(b_add_cpp->b0, "#include <AddinCpp/add_all.hpp>\n");
//        Printf(b_add_cpp->b0, "\n");
//        }
//
//        if (generateXllAddin) {
//        Printf(b_xll_cpp->b0, "\n");
//        Printf(b_xll_cpp->b0, "#include <ohxl/objecthandlerxl.hpp>\n");
//        Printf(b_xll_cpp->b0, "#include <ohxl/register/register_all.hpp>\n");
//        Printf(b_xll_cpp->b0, "#include <ohxl/functions/export.hpp>\n");
//        Printf(b_xll_cpp->b0, "#include <ohxl/utilities/xlutilities.hpp>\n");
//        Printf(b_xll_cpp->b0, "#include <ohxl/objectwrapperxl.hpp>\n");
//        Printf(b_xll_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
//        Printf(b_xll_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
//        Printf(b_xll_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, name);
//        Printf(b_xll_cpp->b0, "//#include \"%s/obj_%s.hpp\"\n", objInc, name);
//        Printf(b_xll_cpp->b0, "#include \"%s/obj_all.hpp\"\n", objInc);
//        Printf(b_xll_cpp->b0, "#include \"%s/conversions/convert2.hpp\"\n", objInc);
//        Printf(b_xll_cpp->b0, "#include \"%s/conversions/convert2.hpp\"\n", xllInc);
//        Printf(b_xll_cpp->b0, "\n");
//        Printf(b_xll_cpp->b0, "/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,\n");
//        Printf(b_xll_cpp->b0, "   for example) also #define _MSC_VER\n");
//        Printf(b_xll_cpp->b0, "*/\n");
//        Printf(b_xll_cpp->b0, "#ifdef BOOST_MSVC\n");
//        Printf(b_xll_cpp->b0, "#  define BOOST_LIB_DIAGNOSTIC\n");
//        Printf(b_xll_cpp->b0, "#  include <oh/auto_link.hpp>\n");
//        Printf(b_xll_cpp->b0, "#  undef BOOST_LIB_DIAGNOSTIC\n");
//        Printf(b_xll_cpp->b0, "#endif\n");
//        Printf(b_xll_cpp->b0, "#include <sstream>\n");
//        Printf(b_xll_cpp->b0, "\n");
//
//        Printf(b_xll_reg->b0, "\n");
//        Printf(b_xll_reg->b0, "#include <xlsdk/xlsdkdefines.hpp>\n");
//        Printf(b_xll_reg->b0, "\n");
//        Printf(b_xll_reg->b0, "void register_%s(const XLOPER &xDll) {\n", name);
//        Printf(b_xll_reg->b0, "\n");
//        Printf(b_xll_reg->b1, "\n");
//        Printf(b_xll_reg->b1, "void unregister_%s(const XLOPER &xDll) {\n", name);
//        Printf(b_xll_reg->b1, "\n");
//        Printf(b_xll_reg->b1, "    XLOPER xlRegID;\n");
//        Printf(b_xll_reg->b1, "\n");
//        }
//
//        if (generateCfyAddin) {
////        Printf(b_cfy_hpp->b0, "\n");
////        Printf(b_cfy_hpp->b0, "#ifndef add_%s_hpp\n", name);
////        Printf(b_cfy_hpp->b0, "#define add_%s_hpp\n", name);
////        Printf(b_cfy_hpp->b0, "\n");
////        Printf(b_cfy_hpp->b0, "#include <string>\n");
////        // FIXME this #include is only needed if a datatype conversion is taking place.
////        Printf(b_cfy_hpp->b0, "#include <oh/property.hpp>\n");
////        Printf(b_cfy_hpp->b0, "\n");
////        Printf(b_cfy_hpp->b0, "namespace %s {\n", addinCppNameSpace);
////        Printf(b_cfy_hpp->b0, "\n");
////
//        Printf(b_cfy_cpp->b0, "\n");
//        Printf(b_cfy_cpp->b0, "#include <string>\n");
//        Printf(b_cfy_cpp->b0, "#include \"init.hpp\"\n");
//        Printf(b_cfy_cpp->b0, "#include <oh/property.hpp>\n");
//
//        Printf(b_cfy_cpp->b0, "#include <oh/repository.hpp>\n");
//        Printf(b_cfy_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, name);
//        Printf(b_cfy_cpp->b0, "\n");
//        // FIXME this #include is only required if the file contains conversions.
//        Printf(b_cfy_cpp->b0, "#include <%s/conversions/convert2.hpp>\n", objInc);
//        Printf(b_cfy_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
//        // FIXME this #include is only required if the file contains enumerations.
//        //Printf(b_cfy_cpp->b0, "#include <oh/enumerations/typefactory.hpp>\n");
//        // FIXME this #include is only required if the file contains constructors.
//        Printf(b_cfy_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, name);
//        if (automatic_) {
//        Printf(b_cfy_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, name);
//        } else {
//        Printf(b_cfy_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, name);
//        }
//        Printf(b_cfy_cpp->b0, "%s\n", add_include);
//        // FIXME include only factories for types used in the current file.
//        Printf(b_cfy_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
//        Printf(b_cfy_cpp->b0, "#include <boost/shared_ptr.hpp>\n");
//        Printf(b_cfy_cpp->b0, "#include <oh/repository.hpp>\n");
//        //Printf(b_cfy_cpp->b0, "#include <AddinCpp/add_all.hpp>\n");
//        Printf(b_cfy_cpp->b0, "\n");
//        }
//
//        if (generateCppAddin) {
//        Printf(b_add_all_hpp->b0, "#include <%s/add_%s.hpp>\n", addInc, name);
//        }
//
//        if (generateXllAddin) {
//        Printf(b_xll_reg_cpp->b0, "extern void register_%s(const XLOPER&);\n", name);
//        Printf(b_xll_reg_cpp->b1, "extern void unregister_%s(const XLOPER&);\n", name);
//        Printf(b_xll_reg_cpp->b2, "    register_%s(xDll);\n", name);
//        Printf(b_xll_reg_cpp->b3, "    unregister_%s(xDll);\n", name);
//        }
//    }
//
//    ~BufferGroup() {
//
//        if (generateCppAddin) {
//        Printf(b_add_hpp->b0, "\n");
//        Printf(b_add_hpp->b0, "} // namespace %s\n", addinCppNameSpace);
//        Printf(b_add_hpp->b0, "\n");
//        Printf(b_add_hpp->b0, "#endif\n");
//        Printf(b_add_hpp->b0, "\n");
//
//        Printf(b_add_cpp->b0, "\n");
//        }
//
//        if (generateXllAddin) {
//        Printf(b_xll_reg->b0, "}\n");
//        Printf(b_xll_reg->b1, "}\n");
//        }
//
//        //if (generateCfyAddin) {
//        //Printf(b_cfy_hpp->b0, "} // namespace %s\n", addinCppNameSpace);
//        //Printf(b_cfy_hpp->b0, "\n");
//        //Printf(b_cfy_hpp->b0, "#endif\n");
//        //Printf(b_cfy_hpp->b0, "\n");
//        //}
//
//        if (generateCppAddin) {
//        delete b_add_hpp;
//        delete b_add_cpp;
//        }
//        if (generateXllAddin) {
//        delete b_xll_cpp;
//        delete b_xll_reg;
//        }
//        if (generateCfyAddin) {
//        delete b_cfy_hpp;
//        delete b_cfy_cpp;
//        }
//    }
//};

//class BufferMap {
//
//    typedef std::map<std::string, BufferGroup*> BM;
//    BM bm_;
//    std::string name_;
//
//public:
//
//    BufferGroup *getBufferGroup(String *name, String *obj_include, String *add_include, String *cfy_include, bool automatic) {
//        name_ = Char(name);
//        if (bm_.end() == bm_.find(name_))
//            bm_[name_] = new BufferGroup(name, obj_include, add_include, cfy_include, automatic);
//        return bm_[name_];
//    }
//
//    void clear() {
//        for (BM::const_iterator i=bm_.begin(); i!=bm_.end(); ++i)
//            delete i->second;
//    }
//};

class REPOSIT : public Language {

    //BufferMap bm_;
    //BufferGroup *bg;
    int functionType;//0=function, 1=constructor, 2=member
    AddinList addinList_;

protected:

    // SWIG buffers
    File *b_begin;
    File *b_runtime;
    File *b_header;
    File *b_wrappers;
    File *b_init;
    //File *b_director;
    //File *b_director_h;

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

   addinList_.appendAddin(new AddinImpl<GroupObjects>);

    for (int i = 1; i < argc; i++) {
        if ( (strcmp(argv[i],"-gencpp") == 0)) {
            //generateCppAddin = true;
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-genxll") == 0)) {
            //generateXllAddin = true;
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-gencfy") == 0)) {
            //generateCfyAddin = true;
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
        if (String *n6a = getNode(n3, "rp_cfy_dir"))
            cfyDir = n6a;
        if (String *n7 = getNode(n3, "rp_obj_inc"))
            objInc = n7;
        if (String *n8 = getNode(n3, "rp_add_inc"))
            addInc = n8;
        if (String *n9 = getNode(n3, "rp_xll_inc"))
            xllInc = n9;
        if (String *n10 = getNode(n3, "rp_cfy_inc"))
            cfyInc = n10;
    }

    printf("module=%s\n", Char(module));
    printf("addinCppNameSpace=%s\n", Char(addinCppNameSpace));
    printf("rp_obj_dir=%s\n", Char(objDir));
    printf("rp_add_dir=%s\n", Char(addDir));
    printf("rp_xll_dir=%s\n", Char(xllDir));
    printf("rp_cfy_dir=%s\n", Char(cfyDir));
    printf("rp_obj_inc=%s\n", Char(objInc));
    printf("rp_add_inc=%s\n", Char(addInc));
    printf("rp_xll_inc=%s\n", Char(xllInc));
    printf("rp_cfy_inc=%s\n", Char(cfyInc));

   /* Initialize I/O */
    b_begin = NewString("");
    b_runtime = NewString("");
    b_header = NewString("");
    b_wrappers = NewString("");
    b_init = NewString("");
    //b_director_h = NewString("");
    //b_director = NewString("");

    printNode(n, b_wrappers);

   /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("begin", b_begin);
    Swig_register_filebyname("runtime", b_runtime);
    Swig_register_filebyname("header", b_header);
    Swig_register_filebyname("wrapper", b_wrappers);
    Swig_register_filebyname("init", b_init);
    //Swig_register_filebyname("director", b_director);
    //Swig_register_filebyname("director_h", b_director_h);

    b_obj_all_hpp = new Buffer(NewStringf("%s/obj_all.hpp", objDir));
    b_cre_reg_cpp = new Buffer(NewStringf("%s/serialization/register_creators.cpp", objDir));
    b_cre_all_hpp = new Buffer(NewStringf("%s/serialization/create/create_all.hpp", objDir));
    b_reg_ser_hpp = new Buffer(NewStringf("%s/serialization/register/serialization_register.hpp", objDir));
    b_reg_all_hpp = new Buffer(NewStringf("%s/serialization/register/serialization_all.hpp", objDir));
    //if (generateCppAddin) {
    //b_add_all_hpp = new Buffer(NewStringf("%s/add_all.hpp", addDir));
    //}
    //if (generateXllAddin) {
    //b_xll_reg_cpp = new Buffer(NewStringf("%s/register/register_all.cpp", xllDir));
    //}
    //if (generateCfyAddin) {
    //b_cfy_mng_txt = new Buffer(NewStringf("%s/cfy_mongo.txt", cfyDir));
    //}

        Printf(b_cre_reg_cpp->b0, "\n");
        Printf(b_cre_reg_cpp->b0, "#include <%s/serialization/serializationfactory.hpp>\n", objInc);
        Printf(b_cre_reg_cpp->b0, "#include <%s/serialization/create/create_all.hpp>\n", objInc);
        Printf(b_cre_reg_cpp->b0, "\n");
        Printf(b_cre_reg_cpp->b0, "void %s::SerializationFactory::registerCreators() {\n", module);
        Printf(b_cre_reg_cpp->b0, "\n");

        Printf(b_obj_all_hpp->b0, "\n");
        Printf(b_obj_all_hpp->b0, "#ifndef obj_all_hpp\n");
        Printf(b_obj_all_hpp->b0, "#define obj_all_hpp\n");
        Printf(b_obj_all_hpp->b0, "\n");

        Printf(b_cre_all_hpp->b0, "\n");
        Printf(b_cre_all_hpp->b0, "#ifndef create_all_hpp\n");
        Printf(b_cre_all_hpp->b0, "#define create_all_hpp\n");
        Printf(b_cre_all_hpp->b0, "\n");

        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "#ifndef serialization_register_hpp\n");
        Printf(b_reg_ser_hpp->b0, "#define serialization_register_hpp\n");
        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "#include <%s/serialization/register/serialization_all.hpp>\n", objInc);
        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "namespace %s {\n", module);
        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "    template<class Archive>\n");
        Printf(b_reg_ser_hpp->b0, "    void tpl_register_classes(Archive& ar) {\n");
        Printf(b_reg_ser_hpp->b0, "\n");

        Printf(b_reg_all_hpp->b0, "\n");
        Printf(b_reg_all_hpp->b0, "#ifndef serialization_all_hpp\n");
        Printf(b_reg_all_hpp->b0, "#define serialization_all_hpp\n");
        Printf(b_reg_all_hpp->b0, "\n");

        //if (generateCppAddin) {
        //Printf(b_add_all_hpp->b0, "\n");
        //Printf(b_add_all_hpp->b0, "#ifndef add_all_hpp\n");
        //Printf(b_add_all_hpp->b0, "#define add_all_hpp\n");
        //Printf(b_add_all_hpp->b0, "\n");
        //Printf(b_add_all_hpp->b0, "#include <%s/init.hpp>\n", addInc);
        //}

        //if (generateXllAddin) {
        //Printf(b_xll_reg_cpp->b0, "\n");
        //Printf(b_xll_reg_cpp->b0, "#include <%s/register/register_all.hpp>\n", xllInc);
        //Printf(b_xll_reg_cpp->b0, "\n");

        //Printf(b_xll_reg_cpp->b2, "\n");
        //Printf(b_xll_reg_cpp->b2, "void registerFunctions(const XLOPER& xDll) {\n");
        //Printf(b_xll_reg_cpp->b2, "\n");

        //Printf(b_xll_reg_cpp->b3, "\n");
        //Printf(b_xll_reg_cpp->b3, "void unregisterFunctions(const XLOPER& xDll) {\n");
        //Printf(b_xll_reg_cpp->b3, "\n");
        //}

        //if (generateCfyAddin) {
        //Printf(b_add_all_hpp->b0, "#ifndef add_all_hpp\n");
        //Printf(b_add_all_hpp->b0, "#define add_all_hpp\n");
        //Printf(b_add_all_hpp->b0, "\n");
        //Printf(b_add_all_hpp->b0, "#include <%s/init.hpp>\n", addInc);
        //}

   /* Output module initialization code */
   Swig_banner(b_begin);

   /* Emit code for children */
   Language::top(n);

        Printf(b_cre_reg_cpp->b0, "\n");
        Printf(b_cre_reg_cpp->b0, "}\n");
        Printf(b_cre_reg_cpp->b0, "\n");

        Printf(b_obj_all_hpp->b0, "\n");
        Printf(b_obj_all_hpp->b0, "#endif\n");
        Printf(b_obj_all_hpp->b0, "\n");

        Printf(b_cre_all_hpp->b0, "\n");
        Printf(b_cre_all_hpp->b0, "#endif\n");
        Printf(b_cre_all_hpp->b0, "\n");

        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "    }\n");
        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "}\n");
        Printf(b_reg_ser_hpp->b0, "\n");
        Printf(b_reg_ser_hpp->b0, "#endif\n");
        Printf(b_reg_ser_hpp->b0, "\n");

        Printf(b_reg_all_hpp->b0, "\n");
        Printf(b_reg_all_hpp->b0, "#endif\n");
        Printf(b_reg_all_hpp->b0, "\n");

        //if (generateCppAddin) {
        //Printf(b_add_all_hpp->b0, "\n");
        //Printf(b_add_all_hpp->b0, "#endif\n");
        //Printf(b_add_all_hpp->b0, "\n");
        //}

        //if (generateXllAddin) {
        //Printf(b_xll_reg_cpp->b2, "\n");
        //Printf(b_xll_reg_cpp->b2, "}\n");
        //Printf(b_xll_reg_cpp->b2, "\n");

        //Printf(b_xll_reg_cpp->b3, "\n");
        //Printf(b_xll_reg_cpp->b3, "}\n");
        //Printf(b_xll_reg_cpp->b3, "\n");
        //}

        //if (generateCfyAddin) {
        //Printf(b_add_all_hpp->b0, "#endif\n");
        //Printf(b_add_all_hpp->b0, "\n");
        //}

    delete b_obj_all_hpp;
    delete b_cre_reg_cpp;
    delete b_cre_all_hpp;
    delete b_reg_ser_hpp;
    delete b_reg_all_hpp;
    //if (generateCppAddin) {
    //delete b_add_all_hpp;
    //}
    //if (generateXllAddin) {
    //delete b_xll_reg_cpp;
    //}
    //if (generateCfyAddin) {
    //delete b_cfy_mng_txt;
    //}

    // To help with troubleshooting, create an output file to which all of the
    // SWIG buffers will be written.  We are not going to compile this file but
    // we give it a cpp extension so that the editor will apply syntax
    // highlighting.
    String *s_test = NewString("test.cpp");
    File *f_test = initFile(s_test);
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
    Printf(f_test, "//**********begin b_init\n");
    Dump(b_init, f_test);
    Printf(f_test, "//**********end b_init\n");
//    Printf(f_test, "//**********begin b_director\n");
//    Dump(b_director, f_test);
//    Printf(f_test, "//**********end b_director\n");
//    Printf(f_test, "//**********begin b_director_h\n");
//    Dump(b_director_h, f_test);
//    Printf(f_test, "//**********end b_director_h\n");

   /* Cleanup files */
    Delete(b_begin);
    Delete(b_runtime);
    Delete(b_header);
    Delete(b_wrappers);
    Delete(b_init);
    //Delete(b_director);
    //Delete(b_director_h);
    // The line below is from the SWIG example but it does not compile?
    //Close(f_test);
    Delete(f_test);

    //bm_.clear();
    addinList_.clear();

    for (int i=0; i<Len(errorList); ++i) {
        String *errorMessage = Getitem(errorList, i);
        printf("%s", Char(errorMessage));
    }
    Delete(errorList);//FIXME also delete each item individually

   return SWIG_OK;
}

void getFeatures(Node *n) {

    obj_include = Getattr(n, "feature:rp:obj_include");
    add_include = Getattr(n, "feature:rp:add_include");
    cfy_include = Getattr(n, "feature:rp:cfy_include");

    // Check whether to generate all source code, or to omit some code to be handwritten by the user.
    // For the user writing the config file, it is easier to assume automatic (default)
    // unless overridden with '%feature("rp:override_obj");' :
    bool manual = 0 != checkAttribute(n, "feature:rp:override_obj", "1");
    // The source code for this SWIG module is cleaner if we think of it the opposite way:
    automatic = !manual;

    group_name = Getattr(n, "feature:rp:group");
    printf("Group='%s'.\n", Char(group_name));
    //if (group)
    //    bg = bm_.getBufferGroup(group, obj_include, add_include, cfy_include, automatic);
}

// overrride base class members, write debug info to b_init,
// and possibly pass control to a handler.

int moduleDirective(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN moduleDirective - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    int ret=Language::moduleDirective(n);
    Printf(b_init, "END   moduleDirective - node name='%s'.\n", Char(nodename));
    return ret;
}

int classDeclaration(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN classDeclaration - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    getFeatures(n);
    int ret=Language::classDeclaration(n);
    Printf(b_init, "END   classDeclaration - node name='%s'.\n", Char(nodename));
    return ret;
}

int constructorDeclaration(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN constructorDeclaration - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    int ret=Language::constructorDeclaration(n);
    Printf(b_init, "END   constructorDeclaration - node name='%s'.\n", Char(nodename));
    return ret;
}

int namespaceDeclaration(Node *n) {
    nmspace = Getattr(n, "name");
    Printf(b_init, "BEGIN namespaceDeclaration - node name='%s'.\n", Char(nmspace));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    getFeatures(n);
    int ret=Language::namespaceDeclaration(n);
    Printf(b_init, "END   namespaceDeclaration - node name='%s'.\n", Char(nmspace));
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
    Printf(b_init, "BEGIN functionHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    //int ret=Language::functionHandler(n);
    int ret=functionHandlerImpl(n);
    Printf(b_init, "END   functionHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int memberfunctionHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN memberfunctionHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    //int ret=Language::memberfunctionHandler(n);
    int ret=memberfunctionHandlerImpl(n);
    Printf(b_init, "END   memberfunctionHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int constructorHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN constructorHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    //int ret=Language::constructorHandler(n);
    int ret=constructorHandlerImpl(n);
    Printf(b_init, "END   constructorHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int functionWrapper(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN functionWrapper - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    //int ret=Language::functionWrapper(n);
    int ret=functionWrapperImpl(n);
    Printf(b_init, "END   functionWrapper - node name='%s'.\n", Char(nodename));
    return ret;
}

int classHandler(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN classHandler - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    int ret=Language::classHandler(n);
    Printf(b_init, "END   classHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int includeDirective(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN includeDirective - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    int ret=Language::includeDirective(n);
    Printf(b_init, "END   includeDirective - node name='%s'.\n", Char(nodename));
    return ret;
}


int functionWrapperImpl(Node *n) {
    // Perform some processing common to all function types
    //functionWrapperImplAll(n);

    // Generate outpu appropriate to the given function type
    int ret;
    if (0==functionType) {
        functionWrapperImplAll(n);
        ret = functionWrapperImplFunc(n);
    } else if (1==functionType) {
        functionWrapperImplAll(n);
        ret = functionWrapperImplCtor(n);
    } else if (2==functionType) {
        functionWrapperImplAll(n);
        ret = functionWrapperImplMemb(n);
    } else {
        ret = SWIG_OK;
    }
    functionType=-1;
    return ret;
}

int functionHandlerImpl(Node *n) {
    functionType=0;
    return Language::functionHandler(n);
}

Parm *prependParm(ParmList *parms, const char *name, const char *type, bool constRef = true, bool hidden = false) {
    Parm *ret = NewHash();
    Setattr(ret, "name", name);
    String *nt  = NewString(type);
    if (constRef) {
        SwigType_add_qualifier(nt, "const");
        SwigType_add_reference(nt);
    }
    Setattr(ret, "type", nt);
    if (hidden)
        Setattr(ret, "hidden", "1");
    processParm(ret);
    Setattr(ret, "nextSibling", parms);
    return ret;
}

int functionWrapperImplFunc(Node *n) {
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    String   *symname   = Getattr(n,"sym:name");
    //String   *action = Getattr(n,"wrap:action");

    String *temp = copyUpper(symname);
    String *funcName = NewStringf("%s%s", prefix, temp);
    Setattr(n, "rp:funcName", funcName);
    printf("funcName=%s\n", Char(funcName));

    // Create from parms another list parms2 - prepend an argument to represent
    // the dependency trigger which is the first argument of every addin function.
    Parm *parms2 = prependParm(parms, "Trigger", "ObjectHandler::property_t");

    addinList_.functionWrapperImplFunc(n, parms, name, type, symname);

    //if (generateCppAddin) {
    //emitTypeMap(bg->b_add_hpp->b0, "rp_tm_add_ret", n, type, 1);
    //Printf(bg->b_add_hpp->b0,"    %s(\n", funcName);
    //emitParmList(parms, bg->b_add_hpp->b0, 2, "rp_tm_add_prm", 2);
    //Printf(bg->b_add_hpp->b0,"    );\n");

    //Printf(bg->b_add_cpp->b0,"//****FUNC*****\n");
    //emitTypeMap(bg->b_add_cpp->b0, "rp_tm_add_ret", n, type);
    //Printf(bg->b_add_cpp->b0,"%s::%s(\n", addinCppNameSpace, funcName);
    //emitParmList(parms, bg->b_add_cpp->b0, 2, "rp_tm_add_prm");
    //Printf(bg->b_add_cpp->b0,") {\n");
    //emitParmList(parms, bg->b_add_cpp->b0, 1, "rp_tm_add_cnv", 1, 0, false);
    //Printf(bg->b_add_cpp->b0,"\n");
    //Printf(bg->b_add_cpp->b0,"    return %s::%s(\n", module, symname);
    //emitParmList(parms, bg->b_add_cpp->b0, 1, "rp_tm_add_cll", 2, ',', true, true);
    //Printf(bg->b_add_cpp->b0,"    );\n");
    //Printf(bg->b_add_cpp->b0,"}\n");
    //}

    //if (generateXllAddin) {
    //excelRegister(bg->b_xll_reg->b0, n, type, parms2);
    //excelUnregister(bg->b_xll_reg->b1, n, type, parms2);

    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0,"//****FUNC*****\n");
    //Printf(bg->b_xll_cpp->b0, "DLLEXPORT\n");
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_tm_xll_ret", n, type);
    //Printf(bg->b_xll_cpp->b0, "%s(\n", funcName);
    //emitParmList(parms2, bg->b_xll_cpp->b0, 2, "rp_tm_xll_prm", 1);
    //Printf(bg->b_xll_cpp->b0, ") {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    try {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    //Printf(bg->b_xll_cpp->b0, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        ObjectHandler::validateRange(Trigger, \"Trigger\");\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cnv", 2, 0, false);
    //Printf(bg->b_xll_cpp->b0, "\n");
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_xll_get", n, type, 2);
    //Printf(bg->b_xll_cpp->b0, "        %s::%s(\n", module, symname);
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cll_obj", 3, ',', true, true);
    //Printf(bg->b_xll_cpp->b0, "        );\n\n");
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_tm_xll_rdc", n, type, 2);

    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    } catch (const std::exception &e) {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    //Printf(bg->b_xll_cpp->b0, "        return 0;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    }\n");
    //Printf(bg->b_xll_cpp->b0, "}\n");
    //}

    //if (generateCfyAddin) {
    //    Printf(bg->b_cfy_cpp->b0,"//****FUNC*****\n");
    //    Printf(bg->b_cfy_cpp->b0,"extern \"C\" {\n");
    //    Printf(bg->b_cfy_cpp->b0,"COUNTIFY_API\n");
    //    emitTypeMap(bg->b_cfy_cpp->b0, "rp_tm_cfy_ret", n, type);
    //    Printf(bg->b_cfy_cpp->b0,"%s(\n", funcName);
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_prm");
    //    Printf(bg->b_cfy_cpp->b0,") {\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"    try {\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"        initializeAddin();\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"        // Convert Countify types into native types\n\n");
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_xx0", 2, 0, false);
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_cnv", 2, 0, false);
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    emitTypeMap(bg->b_cfy_cpp->b0, "rp_tm_cfy_cl1", n, type, 2, false);
    //    Printf(bg->b_cfy_cpp->b0,"        %s::%s(\n", module, symname);
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_cll", 3, ',', true, true);
    //    Printf(bg->b_cfy_cpp->b0,"        );\n");
    //    emitTypeMap(bg->b_cfy_cpp->b0, "rp_tm_cfy_cl2", n, type, 2, false);
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"    } catch (...) {\n");
    //    emitTypeMap(bg->b_cfy_cpp->b0, "rp_tm_cfy_rt2", n, type, 2, false);
    //    Printf(bg->b_cfy_cpp->b0,"    }\n");
    //    Printf(bg->b_cfy_cpp->b0,"}\n");
    //    Printf(bg->b_cfy_cpp->b0,"} // extern \"C\"\n");
    //    mongoFunc(temp, funcName, n, type, parms);
    //}
    return SWIG_OK;
}

//void mongoFunc(String *funcName1, String *funcName2, Node *n, SwigType *t, ParmList *parms) {
//    Printf(b_cfy_mng_txt->b0, "        {\n");
//    Printf(b_cfy_mng_txt->b0, "            \"name\": \"%s\",\n", funcName1);
//    Printf(b_cfy_mng_txt->b0, "            \"codeName\": \"%s\",\n", funcName2);
//    Printf(b_cfy_mng_txt->b0, "            \"description\": \"\",\n");
//    Printf(b_cfy_mng_txt->b0, "            \"returnValue\": {\n");
//    String *s = getTypeMap("rp_tm_cfy_mng", n, t);
//    Printf(b_cfy_mng_txt->b0, "                \"dataType\": \"%s\"\n", s);
//    Printf(b_cfy_mng_txt->b0, "            }");
//    mongoParms(b_cfy_mng_txt->b0, parms);
//    Printf(b_cfy_mng_txt->b0, "        },\n");
//}
//
//void mongoParms(File *f, ParmList *parms) {
//    if (parms) {
//    Printf(f, ",\n");
//    Printf(f, "            \"parameters\": [\n");
//    bool first = true;
//    for (Parm *p = parms; p; p = nextSibling(p)) {
//    String *name = Getattr(p,"name");
//    if (first) {
//        first = false;
//    } else {
//        Printf(f, ",\n");
//    }
//    Printf(f, "                {\n");
//    Printf(f, "                    \"name\": \"%s\",\n", name);
//    SwigType *t  = Getattr(p, "type");
//    String *s = getTypeMap("rp_tm_cfy_mng", p, t);
//    Printf(f, "                    \"dataType\": \"%s\",\n", s);
//    Printf(f, "                    \"description\": \"\",\n");
//    Printf(f, "                    \"optional\": false\n");
//    Printf(f, "                }");
//    }
//    Printf(f, "\n");
//    Printf(f, "            ]\n");
//    } else {
//    Printf(f, "\n");
//    }
//}

int constructorHandlerImpl(Node *n) {

    // If no ctor was defined in the *.i file then SWIG sets the following flag:
    bool defaultCtor = Getattr(n, "default_constructor");
    // For our purposes if no ctor was configured then we don't generate one:
    generateCtor = !defaultCtor;


    Node *p=Getattr(n, "parentNode");
    libraryClass=Getattr(p, "name");
    if (Node *l=Getattr(p, "baselist")) {
        parent=Getitem(l, 0);
    } else {
        parent=0;
    }
    functionType=1;
    return Language::constructorHandler(n);
}

void processParm(Parm *p) {

    String *name = Getattr(p,"name");
    if (!name) {
        printf("parameter has no name\n");
        SWIG_exit(EXIT_FAILURE);
    }

    SwigType *t = Getattr(p, "type");
    if (!t) {
        printf("parameter has no type\n");
        SWIG_exit(EXIT_FAILURE);
    }

    String *nameUpper = copyUpper2(name);
    Setattr(p, "rp_name_upper", nameUpper);

    SwigType *t2 = SwigType_str(SwigType_typedef_resolve_all(t), 0);
    Setattr(p, "rp_typedef_resolved", t2);

////  From "const T&" extract "T"
//    Parm *p2 = CopyParm(p);
//    SwigType *t3 = Getattr(p2, "type");
//    SwigType_del_reference(t3);
//    SwigType_del_qualifier(t3);
//    Setattr(p, "rp_typedef_raw", SwigType_str(t3, 0));
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
    Parm *temp1 = prependParm(parms, "Permanent", "bool", false);
    Parm *temp2 = prependParm(temp1, "Overwrite", "bool", false);
    Parm *temp3 = prependParm(temp2, "objectID", "std::string");
    Parm *parms2 = prependParm(temp3, "Trigger", "ObjectHandler::property_t");

    // Create from parms2 another list parms3 - prepend an argument to represent
    // the object ID which is the return value of addin func that wraps ctor.
    Parm *parms3 = prependParm(parms2, "objectID", "std::string", true, true);

    Printf(b_wrappers, "//***DEF\n");
    printList(parms2, b_wrappers);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(parms2)));
    Printf(b_wrappers, "//***DEF\n");

    addinList_.functionWrapperImplCtor(n, name, generateCtor, funcName, parms, pname, base);

    //if (generateCtor) {
    //Printf(bg->b_val_hpp->b0,"        class %s : public ObjectHandler::ValueObject {\n", funcName);
    //Printf(bg->b_val_hpp->b0,"            friend class boost::serialization::access;\n");
    //Printf(bg->b_val_hpp->b0,"        public:\n");
    //Printf(bg->b_val_hpp->b0,"            %s() {}\n", funcName);
    //Printf(bg->b_val_hpp->b0,"            %s(\n", funcName);
    //Printf(bg->b_val_hpp->b0,"                const std::string& ObjectId,\n");
    //emitParmList(parms, bg->b_val_hpp->b0, 2, "rp_tm_val_prm", 4, ',', true, false, true);
    //Printf(bg->b_val_hpp->b0,"                bool Permanent);\n");
    //Printf(bg->b_val_hpp->b0,"\n");
    //Printf(bg->b_val_hpp->b0,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
    //Printf(bg->b_val_hpp->b0,"            std::vector<std::string> getPropertyNamesVector() const;\n");
    //Printf(bg->b_val_hpp->b0,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
    //Printf(bg->b_val_hpp->b0,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
    //Printf(bg->b_val_hpp->b0,"\n");
    //Printf(bg->b_val_hpp->b0,"        protected:\n");
    //Printf(bg->b_val_hpp->b0,"            static const char* mPropertyNames[];\n");
    //Printf(bg->b_val_hpp->b0,"            static std::set<std::string> mSystemPropertyNames;\n");
    //emitParmList(parms, bg->b_val_hpp->b0, 1, "rp_tm_val_dcl", 3, ';', true, false, true);
    //Printf(bg->b_val_hpp->b0,"            bool Permanent_;\n");
    //Printf(bg->b_val_hpp->b0,"\n");
    //Printf(bg->b_val_hpp->b0,"            template<class Archive>\n");
    //Printf(bg->b_val_hpp->b0,"            void serialize(Archive& ar, const unsigned int) {\n");
    //Printf(bg->b_val_hpp->b0,"            boost::serialization::void_cast_register<%s, ObjectHandler::ValueObject>(this, this);\n", funcName);
    //Printf(bg->b_val_hpp->b0,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
    //Printf(bg->b_val_hpp->b0,"                    & boost::serialization::make_nvp(\"ClassName\", className_)\n");
    //emitParmList(parms, bg->b_val_hpp->b0, 1, "rp_tm_val_ser", 5, 0);
    //Printf(bg->b_val_hpp->b0,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
    //Printf(bg->b_val_hpp->b0,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
    //Printf(bg->b_val_hpp->b0,"            }\n");
    //Printf(bg->b_val_hpp->b0,"        };\n");
    //Printf(bg->b_val_hpp->b0,"\n");

    //Printf(bg->b_val_cpp->b0,"        const char* %s::mPropertyNames[] = {\n", funcName);
    //emitParmList(parms, bg->b_val_cpp->b0, 1, "rp_tm_val_nam", 3, ',', true, false, true);
    //Printf(bg->b_val_cpp->b0,"            \"Permanent\"\n");
    //Printf(bg->b_val_cpp->b0,"        };\n");
    //Printf(bg->b_val_cpp->b0,"\n");
    //Printf(bg->b_val_cpp->b0,"        std::set<std::string> %s::mSystemPropertyNames(\n", funcName);
    //Printf(bg->b_val_cpp->b0,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    //Printf(bg->b_val_cpp->b0,"\n");
    //Printf(bg->b_val_cpp->b0,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", funcName);
    //Printf(bg->b_val_cpp->b0,"            return mSystemPropertyNames;\n");
    //Printf(bg->b_val_cpp->b0,"        }\n");
    //Printf(bg->b_val_cpp->b0,"\n");
    //Printf(bg->b_val_cpp->b0,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", funcName);
    //Printf(bg->b_val_cpp->b0,"            std::vector<std::string> ret(\n");
    //Printf(bg->b_val_cpp->b0,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    //Printf(bg->b_val_cpp->b0,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
    //Printf(bg->b_val_cpp->b0,"                i != userProperties.end(); ++i)\n");
    //Printf(bg->b_val_cpp->b0,"                ret.push_back(i->first);\n");
    //Printf(bg->b_val_cpp->b0,"            return ret;\n");
    //Printf(bg->b_val_cpp->b0,"        }\n");
    //Printf(bg->b_val_cpp->b0,"\n");
    //Printf(bg->b_val_cpp->b0,"        ObjectHandler::property_t %s::getSystemProperty(const std::string& name) const {\n", funcName);
    //Printf(bg->b_val_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    //Printf(bg->b_val_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    //Printf(bg->b_val_cpp->b0,"                return objectId_;\n");
    //Printf(bg->b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    //Printf(bg->b_val_cpp->b0,"                return className_;\n");
    //voGetProp(bg->b_val_cpp->b0, parms);
    //Printf(bg->b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    //Printf(bg->b_val_cpp->b0,"                return Permanent_;\n");
    //Printf(bg->b_val_cpp->b0,"            else\n");
    //Printf(bg->b_val_cpp->b0,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
    //Printf(bg->b_val_cpp->b0,"        }\n");
    //Printf(bg->b_val_cpp->b0,"\n");
    //Printf(bg->b_val_cpp->b0,"        void %s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", funcName);
    //Printf(bg->b_val_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    //Printf(bg->b_val_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    //Printf(bg->b_val_cpp->b0,"                objectId_ = boost::get<std::string>(value);\n");
    //Printf(bg->b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    //Printf(bg->b_val_cpp->b0,"                className_ = boost::get<std::string>(value);\n");
    //voSetProp(bg->b_val_cpp->b0, parms);
    //Printf(bg->b_val_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    //Printf(bg->b_val_cpp->b0,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
    //Printf(bg->b_val_cpp->b0,"            else\n");
    //Printf(bg->b_val_cpp->b0,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
    //Printf(bg->b_val_cpp->b0,"        }\n");
    //Printf(bg->b_val_cpp->b0,"\n");
    //Printf(bg->b_val_cpp->b0,"        %s::%s(\n", funcName, funcName);
    //Printf(bg->b_val_cpp->b0,"                const std::string& ObjectId,\n");
    //emitParmList(parms, bg->b_val_cpp->b0, 2, "rp_tm_val_prm", 4, ',', true, false, true);
    //Printf(bg->b_val_cpp->b0,"                bool Permanent) :\n");
    //Printf(bg->b_val_cpp->b0,"            ObjectHandler::ValueObject(ObjectId, \"%s\", Permanent),\n", funcName);
    //emitParmList(parms, bg->b_val_cpp->b0, 1, "rp_tm_val_ini", 3, ',', true, false, true);
    //Printf(bg->b_val_cpp->b0,"            Permanent_(Permanent) {\n");
    //Printf(bg->b_val_cpp->b0,"        }\n");
    //}

    //if (generateCtor) {
    //Printf(bg->b_cre_hpp->b0, "\n");
    //Printf(bg->b_cre_hpp->b0, "boost::shared_ptr<ObjectHandler::Object> create_%s(\n", funcName);
    //Printf(bg->b_cre_hpp->b0, "    const boost::shared_ptr<ObjectHandler::ValueObject>&);\n");

    //Printf(bg->b_cre_cpp->b0, "\n");
    //Printf(bg->b_cre_cpp->b0, "boost::shared_ptr<ObjectHandler::Object> %s::create_%s(\n", module, funcName);
    //Printf(bg->b_cre_cpp->b0, "    const boost::shared_ptr<ObjectHandler::ValueObject> &valueObject) {\n");
    //Printf(bg->b_cre_cpp->b0, "\n");
    //Printf(bg->b_cre_cpp->b0, "    // conversions\n\n");
    //emitParmList(parms, bg->b_cre_cpp->b0, 1, "rp_tm_cre_cnv", 1, 0);
    //Printf(bg->b_cre_cpp->b0, "\n");
    //Printf(bg->b_cre_cpp->b0, "    bool Permanent =\n");
    //Printf(bg->b_cre_cpp->b0, "        ObjectHandler::convert2<bool>(valueObject->getProperty(\"Permanent\"));\n");
    //Printf(bg->b_cre_cpp->b0, "\n");
    //Printf(bg->b_cre_cpp->b0, "    // construct and return the object\n");
    //Printf(bg->b_cre_cpp->b0, "\n");
    //Printf(bg->b_cre_cpp->b0, "    boost::shared_ptr<ObjectHandler::Object> object(\n");
    //Printf(bg->b_cre_cpp->b0, "        new %s::%s(\n", module, name);
    //Printf(bg->b_cre_cpp->b0, "            valueObject,\n");
    //emitParmList(parms, bg->b_cre_cpp->b0, 0, "rp_tm_default", 3, ',', true, false, true);
    //Printf(bg->b_cre_cpp->b0, "            Permanent));\n");
    //Printf(bg->b_cre_cpp->b0, "    return object;\n");
    //Printf(bg->b_cre_cpp->b0, "}\n");
    //}

    //if (generateCtor) {
    //Printf(bg->b_reg_cpp->b0, "    // class ID %d in the boost serialization framework\n", idNum);
    //Printf(bg->b_reg_cpp->b0, "    ar.register_type<%s::ValueObjects::%s>();\n", module, funcName);

    //Printf(bg->b_reg_cpp->b1, "    // class ID %d in the boost serialization framework\n", idNum);
    //Printf(bg->b_reg_cpp->b1, "    ar.register_type<%s::ValueObjects::%s>();\n", module, funcName);
    //}

    //idNum++;

    //if (generateCtor) {
    //    String *s0 = NewString("");
    //    String *s1 = NewString("");
    //    if (base) {
    //        s0 = base;
    //        s1 = NewStringf("%s::%s", nmspace, base);
    //    } else {
    //        s0 = NewStringf("ObjectHandler::LibraryObject<%s>", pname);
    //        s1 = pname;
    //    }
    //    Printf(bg->b_obj_hpp->b0,"\n");
    //    Printf(bg->b_obj_hpp->b0,"    class %s : \n", name);
    //    Printf(bg->b_obj_hpp->b0,"        public %s {\n", s0);
    //    Printf(bg->b_obj_hpp->b0,"    public:\n");
    //    Printf(bg->b_obj_hpp->b0,"        %s(\n", name);
    //    Printf(bg->b_obj_hpp->b0,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,\n");
    //    emitParmList(parms, bg->b_obj_hpp->b0, 2, "rp_tm_default", 3, ',', true, false, true);
    //    Printf(bg->b_obj_hpp->b0,"            bool permanent)\n");
    //    Printf(bg->b_obj_hpp->b0,"        : %s(properties, permanent) {\n", s0);
    //    Printf(bg->b_obj_hpp->b0,"            libraryObject_ = boost::shared_ptr<%s>(new %s(\n", s1, pname);
    //    emitParmList(parms, bg->b_obj_hpp->b0, 0, "rp_tm_default", 4);
    //    Printf(bg->b_obj_hpp->b0,"            ));\n");
    //    Printf(bg->b_obj_hpp->b0,"        }\n");
    //    Printf(bg->b_obj_hpp->b0,"    };\n");
    //    Printf(bg->b_obj_hpp->b0,"\n");

    //    if (generateCppAddin) {
    //    Printf(bg->b_add_hpp->b0,"\n");
    //    Printf(bg->b_add_hpp->b0,"    std::string %s(\n", funcName);
    //    emitParmList(parms2, bg->b_add_hpp->b0, 2, "rp_tm_add_prm", 2);
    //    Printf(bg->b_add_hpp->b0,"    );\n\n");

    //    Printf(bg->b_add_cpp->b0,"//****CTOR*****\n");
    //    Printf(bg->b_add_cpp->b0,"std::string %s::%s(\n", addinCppNameSpace, funcName);
    //    emitParmList(parms2, bg->b_add_cpp->b0, 2, "rp_tm_add_prm", 2);
    //    Printf(bg->b_add_cpp->b0,"    ) {\n");
    //    Printf(bg->b_add_cpp->b0,"\n");
    //    Printf(bg->b_add_cpp->b0,"    // Convert input types into Library types\n\n");
    //    emitParmList(parms, bg->b_add_cpp->b0, 1, "rp_tm_add_cnv", 1, 0, false);
    //    Printf(bg->b_add_cpp->b0,"\n");
    //    Printf(bg->b_add_cpp->b0,"    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    //    Printf(bg->b_add_cpp->b0,"        new %s::ValueObjects::%s(\n", module, funcName);
    //    Printf(bg->b_add_cpp->b0,"            objectID,\n");
    //    emitParmList(parms, bg->b_add_cpp->b0, 0, "rp_tm_default", 3, ',', true, false, true);
    //    Printf(bg->b_add_cpp->b0,"            false));\n");
    //    Printf(bg->b_add_cpp->b0,"    boost::shared_ptr<ObjectHandler::Object> object(\n");
    //    Printf(bg->b_add_cpp->b0,"        new %s::%s(\n", module, name);
    //    Printf(bg->b_add_cpp->b0,"            valueObject,\n");
    //    emitParmList(parms, bg->b_add_cpp->b0, 1, "rp_tm_add_cll", 3, ',', true, true, true);
    //    Printf(bg->b_add_cpp->b0,"            false));\n");
    //    Printf(bg->b_add_cpp->b0,"    std::string returnValue =\n");
    //    Printf(bg->b_add_cpp->b0,"        ObjectHandler::Repository::instance().storeObject(\n");
    //    Printf(bg->b_add_cpp->b0,"            objectID, object, false, valueObject);\n");
    //    Printf(bg->b_add_cpp->b0,"    return returnValue;\n");
    //    Printf(bg->b_add_cpp->b0,"}\n\n");
    //    }

    //    if (generateCfyAddin) {
    //    Printf(bg->b_cfy_hpp->b0,"\n");

    //    Printf(bg->b_cfy_cpp->b0,"//****CTOR*****\n");
    //    Printf(bg->b_cfy_cpp->b0,"extern \"C\" {\n");
    //    Printf(bg->b_cfy_cpp->b0,"COUNTIFY_API\n");
    //    Printf(bg->b_cfy_cpp->b0,"const char *%s(\n", funcName);
    //    emitParmList(parms2, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_prm", 2);
    //    Printf(bg->b_cfy_cpp->b0,"    ) {\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"    try {\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"        initializeAddin();\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"        // Convert Countify types into native types\n\n");
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_xx0", 2, 0, false);
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"        // Convert input types into Library types\n\n");
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_cnv", 2, 0, false);
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    //    Printf(bg->b_cfy_cpp->b0,"            new %s::ValueObjects::%s(\n", module, funcName);
    //    Printf(bg->b_cfy_cpp->b0,"                objectID,\n");
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 0, "rp_tm_default", 4, ',', true, false, true);
    //    Printf(bg->b_cfy_cpp->b0,"                false));\n");
    //    Printf(bg->b_cfy_cpp->b0,"        boost::shared_ptr<ObjectHandler::Object> object(\n");
    //    Printf(bg->b_cfy_cpp->b0,"            new %s::%s(\n", module, name);
    //    Printf(bg->b_cfy_cpp->b0,"                valueObject,\n");
    //    emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_cll", 4, ',', true, true, true);
    //    Printf(bg->b_cfy_cpp->b0,"                false));\n");
    //    Printf(bg->b_cfy_cpp->b0,"        static std::string returnValue;\n");
    //    Printf(bg->b_cfy_cpp->b0,"        returnValue =\n");
    //    Printf(bg->b_cfy_cpp->b0,"            ObjectHandler::Repository::instance().storeObject(\n");
    //    Printf(bg->b_cfy_cpp->b0,"                objectID, object, true, valueObject);\n");
    //    Printf(bg->b_cfy_cpp->b0,"        return returnValue.c_str();\n");
    //    Printf(bg->b_cfy_cpp->b0,"\n");
    //    Printf(bg->b_cfy_cpp->b0,"    } catch (const std::exception &e) {\n");
    //    Printf(bg->b_cfy_cpp->b0,"        return e.what();\n");
    //    Printf(bg->b_cfy_cpp->b0,"    } catch (...) {\n");
    //    Printf(bg->b_cfy_cpp->b0,"        return 0;\n");
    //    Printf(bg->b_cfy_cpp->b0,"    }\n");
    //    Printf(bg->b_cfy_cpp->b0,"}\n\n");
    //    Printf(bg->b_cfy_cpp->b0,"} // extern \"C\"\n");

    //    mongoFunc(name, funcName, n, type, parms2);
    //    }

    //    Printf(b_cre_reg_cpp->b0, "    registerCreator(\"%s\", create_%s);\n", funcName, funcName);
    //} else { //!generateCtor
    //    Printf(bg->b_obj_hpp->b0, "    // BEGIN typemap rp_tm_obj_cls\n");
    //    if (parent) {
    //        Printf(bg->b_obj_hpp->b0, "    OH_OBJ_CLASS(%s, %s);\n", name, parent);
    //    } else {
    //        Printf(bg->b_obj_hpp->b0, "    OH_LIB_CLASS(%s, %s);\n", name, libraryClass);
    //    }
    //    Printf(bg->b_obj_hpp->b0, "    // END   typemap rp_tm_obj_cls\n");
    //}

    //if (generateXllAddin) {
    //excelRegister(bg->b_xll_reg->b0, n, 0, parms3);
    //excelUnregister(bg->b_xll_reg->b1, n, type, parms3);

    //if (generateCtor) {
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0,"//****CTOR*****\n");
    //Printf(bg->b_xll_cpp->b0, "DLLEXPORT char *%s(\n", funcName);
    //emitParmList(parms2, bg->b_xll_cpp->b0, 2, "rp_tm_xll_prm");
    //Printf(bg->b_xll_cpp->b0, ") {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    try {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    //Printf(bg->b_xll_cpp->b0, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    //Printf(bg->b_xll_cpp->b0, "\n");
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cnv", 2, 0, false);
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    //Printf(bg->b_xll_cpp->b0, "            new %s::ValueObjects::%s(\n", module, funcName);
    //Printf(bg->b_xll_cpp->b0, "                objectID,\n");
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cll_val", 4, ',', true, true, true);
    //Printf(bg->b_xll_cpp->b0, "                false));\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        boost::shared_ptr<ObjectHandler::Object> object(\n");
    //Printf(bg->b_xll_cpp->b0, "            new %s::%s(\n", module, name);
    //Printf(bg->b_xll_cpp->b0, "                valueObject,\n");
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cll_obj", 4, ',', true, true, true);
    //Printf(bg->b_xll_cpp->b0, "                false));\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        std::string returnValue =\n");
    //Printf(bg->b_xll_cpp->b0, "            ObjectHandler::RepositoryXL::instance().storeObject(objectID, object, true);\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        static char ret[XL_MAX_STR_LEN];\n");
    //Printf(bg->b_xll_cpp->b0, "        ObjectHandler::stringToChar(returnValue, ret);\n");
    //Printf(bg->b_xll_cpp->b0, "        return ret;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    } catch (const std::exception &e) {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    //Printf(bg->b_xll_cpp->b0, "        return 0;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    }\n");
    //Printf(bg->b_xll_cpp->b0, "}\n");
    //}
    //}
    return SWIG_OK;
}

int memberfunctionHandlerImpl(Node *n) {
    functionType=2;
    return Language::memberfunctionHandler(n);
}

int functionWrapperImplMemb(Node *n) {
    Printf(b_wrappers, "//***XYZ\n");
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
    String *funcName2 = NewStringf("%s%s", temp0, temp1);

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every ctor.
    ParmList *parmsTemp = Getattr(parms, "nextSibling");
    Parm *parmsTemp2 = prependParm(parmsTemp, "objectID", "std::string");
    Parm *parms2 = prependParm(parmsTemp2, "Trigger", "ObjectHandler::property_t");

    // We are invoking the member function of a class.
    // Create a dummy node and attach to it the type of the class.
    // This allows us to apply a typemap to the node.
    Node *node = NewHash();
    Setfile(node, Getfile(n));
    Setline(node, Getline(n));
    Setattr(node, "type", pname);
    // Attach to the node some values that might be referenced by the typemap:
    Setattr(node, "rp_typedef_obj_add", addinClass);    // The type of the addin wrapper object
    Setattr(node, "rp_typedef_obj_lib", pname);         // The type of the library object

    Printf(b_wrappers, "//***ABC\n");
    printList(parms2, b_wrappers);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(parms2)));
    Printf(b_wrappers, "//***ABC\n");

    //if (generateCppAddin) {
    //emitTypeMap(bg->b_add_hpp->b0, "rp_tm_add_ret", n, type, 1);
    //Printf(bg->b_add_hpp->b0,"    %s(\n", funcName);
    //emitParmList(parms2, bg->b_add_hpp->b0, 2, "rp_tm_add_prm", 2);
    //Printf(bg->b_add_hpp->b0,"    );\n\n");

    //Printf(bg->b_add_cpp->b0,"//****MEMB*****\n");
    //emitTypeMap(bg->b_add_cpp->b0, "rp_tm_add_ret", n, type);
    //Printf(bg->b_add_cpp->b0,"%s::%s(\n", addinCppNameSpace, funcName);
    //emitParmList(parms2, bg->b_add_cpp->b0, 2, "rp_tm_add_prm", 2);
    //Printf(bg->b_add_cpp->b0,"    ) {\n\n");
    //emitParmList(parms, bg->b_add_cpp->b0, 1, "rp_tm_add_cnv", 1, 0, false);
    //Printf(bg->b_add_cpp->b0,"\n");

    ////// We are invoking the member function of a class.
    ////// Create a dummy node and attach to it the type of the class.
    ////// This allows us to apply a typemap to the node.
    ////Node *node = NewHash();
    ////Setfile(node, Getfile(n));
    ////Setline(node, Getline(n));
    ////Setattr(node, "type", pname);
    ////// Attach to the node some values that might be referenced by the typemap:
    ////Setattr(node, "rp_typedef_obj_add", addinClass);    // The type of the addin wrapper object
    ////Setattr(node, "rp_typedef_obj_lib", pname);         // The type of the library object
    //// Apply the typemap to the dummy node.
    //emitTypeMap(bg->b_add_cpp->b0, "rp_tm_add_oh_get", node, type);// FIXME last parm "type" does not matter + can be omitted
    ////// Delete the dummy node.
    ////Delete(node);

    //Printf(bg->b_add_cpp->b0,"    return x->%s(\n", name);
    //emitParmList(parms, bg->b_add_cpp->b0, 1, "rp_tm_add_cll", 3, ',', true, true);
    //Printf(bg->b_add_cpp->b0,"        );\n", name);
    //Printf(bg->b_add_cpp->b0,"}\n");
    //}

    //if (generateXllAddin) {
    //excelRegister(bg->b_xll_reg->b0, n, type, parms2);
    //excelUnregister(bg->b_xll_reg->b1, n, type, parms2);

    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0,"//****MEMB*****\n");
    //Printf(bg->b_xll_cpp->b0, "DLLEXPORT\n");
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_tm_xll_ret", n, type);
    //Printf(bg->b_xll_cpp->b0, "%s(\n", funcName);
    //emitParmList(parms2, bg->b_xll_cpp->b0, 2, "rp_tm_xll_prm");
    //Printf(bg->b_xll_cpp->b0, ") {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    try {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>\n");
    //Printf(bg->b_xll_cpp->b0, "            (new ObjectHandler::FunctionCall(\"%s\"));\n", funcName);
    //Printf(bg->b_xll_cpp->b0, "\n");
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cnv", 2, 0, false);
    //Printf(bg->b_xll_cpp->b0, "\n");
    ////Printf(bg->b_xll_cpp->b0, "        OH_GET_REFERENCE(x, objectID, %s, %s);\n", addinClass, pname);
    //// Apply the typemap to the dummy node.
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_tm_add_oh_get", node, type, 2);// FIXME last parm "type" does not matter + can be omitted
    //Printf(bg->b_xll_cpp->b0, "\n");
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_xll_get", n, type, 2);
    //Printf(bg->b_xll_cpp->b0, "        x->%s(\n", name);
    //emitParmList(parms, bg->b_xll_cpp->b0, 1, "rp_tm_xll_cll_obj", 3, ',', true, true);
    //Printf(bg->b_xll_cpp->b0, "        );\n\n");
    //emitTypeMap(bg->b_xll_cpp->b0, "rp_tm_xll_rdc", n, type, 2);
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    } catch (const std::exception &e) {\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);\n");
    //Printf(bg->b_xll_cpp->b0, "        return 0;\n");
    //Printf(bg->b_xll_cpp->b0, "\n");
    //Printf(bg->b_xll_cpp->b0, "    }\n");
    //Printf(bg->b_xll_cpp->b0, "}\n");
    //}

    //if (generateCfyAddin) {

    //Printf(bg->b_cfy_cpp->b0,"//****MEMB*****\n");
    //Printf(bg->b_cfy_cpp->b0,"extern \"C\" {\n");
    //Printf(bg->b_cfy_cpp->b0,"COUNTIFY_API\n");
    //emitTypeMap(bg->b_cfy_cpp->b0, "rp_tm_add_ret", n, type);
    //Printf(bg->b_cfy_cpp->b0,"%s(\n", funcName);
    //emitParmList(parms2, bg->b_cfy_cpp->b0, 1, "rp_tm_cfy_prm", 2);
    //Printf(bg->b_cfy_cpp->b0,"    ) {\n\n");
    //emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_add_cnv", 1, 0, false);
    //Printf(bg->b_cfy_cpp->b0,"\n");

    //// We are invoking the member function of a class.
    //// Create a dummy node and attach to it the type of the class.
    //// This allows us to apply a typemap to the node.
    //Node *node = NewHash();
    //Setfile(node, Getfile(n));
    //Setline(node, Getline(n));
    //Setattr(node, "type", pname);
    //// Attach to the node some values that might be referenced by the typemap:
    //Setattr(node, "rp_typedef_obj_add", addinClass);    // The type of the addin wrapper object
    //Setattr(node, "rp_typedef_obj_lib", pname);         // The type of the library object
    //// Apply the typemap to the dummy node.
    //emitTypeMap(bg->b_cfy_cpp->b0, "rp_tm_add_oh_get", node, type, 2);// FIXME last parm "type" does not matter + can be omitted
    //// Delete the dummy node.
    //Delete(node);

    //Printf(bg->b_cfy_cpp->b0,"    return x->%s(\n", name);
    //emitParmList(parms, bg->b_cfy_cpp->b0, 1, "rp_tm_add_cll", 3, ',', true, true);
    //Printf(bg->b_cfy_cpp->b0,"        );\n", name);
    //Printf(bg->b_cfy_cpp->b0,"}\n");
    //Printf(bg->b_cfy_cpp->b0,"} // extern \"C\"\n");

    //mongoFunc(funcName2, funcName, n, type, parms);
    //}

    // Delete the dummy node.
    Delete(node);

    return SWIG_OK;
}

void functionWrapperImplAll(Node *n) {
    String *nodeName = Getattr(n, "name");
    printf("Processing node name '%s'.\n", Char(nodeName));

    getFeatures(n);

    // Process the parameter list.
    ParmList *parms  = Getattr(n,"parms");
    for (Parm *p = parms; p; p = nextSibling(p))
        processParm(p);

    // Write some debug info to the b_wrappers buffer (test.cpp).
    Printf(b_wrappers,"//XXX***functionWrapper*******\n");
    Printf(b_wrappers,"//module=%s\n", module);
    //Printf(b_wrappers,"//group_name=%s\n", group_name);
    printNode(n, b_wrappers);
    printList(Getattr(n, "parms"), b_wrappers);
    Printf(b_wrappers,"//*************\n");
}
}; // class REPOSIT

extern "C" Language *
swig_reposit(void) {
  return new REPOSIT();
}

