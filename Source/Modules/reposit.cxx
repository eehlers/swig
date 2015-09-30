
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wempty-body"
#endif

#include "swigmod.h"
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <algorithm>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

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
String *cshDir = NewString("AddinCSharp");
String *cfyDir = NewString("AddinCfy");
String *xllDir = NewString("AddinXl");
String *objInc = NewString("AddinObjects");
String *addInc = NewString("AddinCpp");
String *xllInc = NewString("AddinXl");

//*****************************************************************************
// ERROR HANDLING
//*****************************************************************************

List *errorList = NewList();

// This macro is shared by the two below it and should not be called directly.
#define REPOSIT_SWIG_ERROR(message) \
std::ostringstream _rp_msg_stream; \
_rp_msg_stream << message << std::endl; \
printf("%s\n", std::string(80, '=').c_str()); \
printf("FATAL ERROR - REPOSIT SWIG MODULE\n"); \
printf("%s\n", std::string(80, '=').c_str()); \
printf("%s\n", _rp_msg_stream.str().c_str()); \
printf("%s\n", std::string(80, '=').c_str()); \
SWIG_exit(EXIT_FAILURE);

// Instead of calling SWIG_exit() directly, use one of the two macros below.
#define REPOSIT_SWIG_FAIL(message) \
do { \
    REPOSIT_SWIG_ERROR(message) \
} while (false)

#define REPOSIT_SWIG_REQUIRE(condition,message) \
if (!(condition)) { \
    REPOSIT_SWIG_ERROR(message) \
} else

//*****************************************************************************
// UTILITY FUNCTIONS
//*****************************************************************************

Node *getNode(Node *n, const char *c) {
    Node *ret = 0;
    REPOSIT_SWIG_REQUIRE((ret = Getattr(n, c)),
        "Can't find node '" << c << "'");
    return ret;
}

void printNode(Node *n, File *f) {
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        Printf(f,"/* %d %s %s */\n", i, key, Getattr(n, key));
    }
}

void printNode2(Node *n) {
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        printf("/* %d %s %s */\n", i, Char(key), Char(Getattr(n, key)));
    }
}

void printList(Node *n, File *f) {
    while (n) {
        printNode(n, f);
        n = Getattr(n,"nextSibling");
    }
}

String *getTypeMap(Node *n, const char *m, bool fatal = true) {
    if (String *tm = Swig_typemap_lookup(m, n, "", 0)) {
        Replaceall(tm, "$rp_typedef_resolved", Getattr(n, "rp_typedef_resolved"));
        Replaceall(tm, "$rp_typedef_base", Getattr(n, "rp_typedef_base"));
        Replaceall(tm, "$rp_typedef_no_template", Getattr(n, "rp_typedef_no_template"));
        Replaceall(tm, "$rp_typedef_no_namespace", Getattr(n, "rp_typedef_no_namespace"));
        Replaceall(tm, "$rp_addin_namespace", module);

        Replaceall(tm, "$rp_typedef_obj_add", Getattr(n, "rp_typedef_obj_add"));
        Replaceall(tm, "$rp_typedef_obj_lib", Getattr(n, "rp_typedef_obj_lib"));
        return tm;
    }
    if (fatal) {
        SwigType *t  = Getattr(n, "type");
        Append(errorList, NewStringf("*** ERROR : typemap '%s' does not match type '%s'.\n", m, SwigType_str(t, 0)));
        // Return an error string, this will be inserted into the source code.
        return NewStringf("#error NEED THIS TYPEMAP: >>> %%typemap(%s) %s \"XXX\"; <<<", m, SwigType_str(t, 0));
    }
    return 0;
}

String *getType(Parm *p, const char *m, bool fatal) {
    SwigType *t = Getattr(p, "type");
    if (0==strcmp(m, "rp_tm_default"))
        return t;
    else {
        String *s = getTypeMap(p, m, fatal);
        return s;
    }
}

void printIndent(File *buf, int indent) {
    for (int i=0;i<indent;++i)
        Printf(buf, "    ");
}

void emitTypeMap(File *buf, Node *n, const char *m, int indent=0, bool fatal = true) {
    SwigType *t  = Getattr(n, "type");
    printIndent(buf, indent);
    Printf(buf, "// BEGIN typemap %s %s\n", m, t);
    printIndent(buf, indent);
    String *s = getType(n, m, fatal);
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

    REPOSIT_SWIG_REQUIRE(map, "Function emitParmList called with null type map.");

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

String *excelParamCodes(Node *n, ParmList *parms) {
    String *s = NewString("");
    String *tm = getTypeMap(n, "rp_tm_xll_cdrt");
    Append(s, tm);
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *tm = getTypeMap(p, "rp_tm_xll_code");
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

void excelRegister(File *b, Node *n, ParmList *parms) {
    String *funcName   = Getattr(n, "rp:funcRename");
    Printf(b, "        // BEGIN function excelRegister\n");
    Printf(b, "        Excel(xlfRegister, 0, 7, &xDll,\n");
    Printf(b, "            // function code name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // parameter codes\n");
    String *xlParamCodes = excelParamCodes(n, parms);
    Printf(b, "            TempStrNoSize(\n");
    Printf(b, "            // BEGIN func excelParamCodes (using typemap rp_tm_xll_code)\n");
    Printf(b, "            \"\\x%s\"\"%s\"\n", hexLen(xlParamCodes).c_str(), xlParamCodes);
    Printf(b, "            // END   func excelParamCodes (using typemap rp_tm_xll_code)\n");
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

void excelUnregister(File *b, Node *n, ParmList *parms) {
    String *funcName   = Getattr(n, "rp:funcRename");
    Printf(b, "        // BEGIN function excelUnregister\n");
    Printf(b, "        Excel(xlfRegister, 0, 7, &xDll,\n");
    Printf(b, "            // function code name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // parameter codes\n");
    String *xlParamCodes = excelParamCodes(n, parms);
    Printf(b, "            TempStrNoSize(\n");
    Printf(b, "            // BEGIN func excelParamCodes (using typemap rp_tm_xll_code)\n");
    Printf(b, "            \"\\x%s\"\"%s\"\n", hexLen(xlParamCodes).c_str(), xlParamCodes);
    Printf(b, "            // END   func excelParamCodes (using typemap rp_tm_xll_code)\n");
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
    Printf(f, "            // BEGIN func voSetProp (using typemap rp_tm_vob_cnvt)\n");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        String *name = Getattr(p,"name");
        String *nameUpper = Getattr(p,"rp_name_upper");
        String *cnv = getTypeMap(p, "rp_tm_vob_cnvt");
        Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
        Printf(f, "                %s_ = %s;\n", name, cnv);
    }
    Printf(f, "            // END   func voSetProp (using typemap rp_tm_vob_cnvt)\n");
}

File *initFile(String *outfile) {
   File *f = NewFile(outfile, "w", SWIG_output_files());
   if (!f) {
      FileErrorDisplay(outfile);
      REPOSIT_SWIG_FAIL("Error initializing file '" << Char(outfile) << "'");
   }
    return f;
}

//*****************************************************************************
// COUNT
//*****************************************************************************

struct Count {
    int created;
    int updated;
    int unchanged;
    int total;
    int functions;
    int constructors;
    int members;
    int total2;
    Count() : created(0), updated(0), unchanged(0), total(0),
        functions(0), constructors(0), members(0) {}
    void add(const Count &c) {
        created += c.created;
        updated += c.updated;
        unchanged += c.unchanged;
        total += c.total;/*
        functions += c.functions;
        constructors += c.constructors;
        members += c.members;
        total2 += c.total2;*/
    }
};

//*****************************************************************************
// PRAGMAS
//*****************************************************************************

struct Pragmas {
    const String *groupName_;
    String *lib_inc;
    String *add_inc;
    bool automatic_;
    Pragmas() : groupName_(0), lib_inc(0), add_inc(0), automatic_(true) {}
    Pragmas & operator= (const Pragmas & other) {
        groupName_ = other.groupName_;
        lib_inc = other.lib_inc;
        add_inc = other.add_inc;
        automatic_ = other.automatic_;
        return *this;
    }
    void setGroupName(const String *groupName) {
        groupName_ = groupName;
        lib_inc = NewString("");
        add_inc = NewString("");
        Swig_register_filebyname(NewStringf("%s_library_hpp", groupName), lib_inc);
        Swig_register_filebyname(NewStringf("%s_addin_cpp", groupName), add_inc);
    }
};

//*****************************************************************************
// BUFFER
//*****************************************************************************

struct Buffer {
    String *name_;
    String *path_;
    File *b0;
    File *b1;
    File *b2;
    File *b3;
    String *outputBuffer_;
    Buffer(const String *name, String *path) :
    name_(NewString(name)),
    path_(Copy(path)) {
        b0 = NewString("");
        b1 = NewString("");
        b2 = NewString("");
        b3 = NewString("");
        Printf(b0, "\n");
        Printf(b0, "// BEGIN buffer %s\n", name_);
        Printf(b0, "\n");
    }
    bool fileExists() {
#ifdef WIN32
        return (-1 != _access(Char(path_), 0));
#else
        return (-1 != access(Char(path_), F_OK));
#endif
    }
    bool fileChanged() {
        FILE *f = Swig_open(path_);
        if (!f)
            return true;
        String *s = Swig_read_file(f);
        return (0!=Strcmp(s, outputBuffer_));
    }
    void writeFile() {
        File *f = initFile(path_);
        Dump(outputBuffer_, f);
        Delete(f);
    }
    void clearImpl(Count &count) {
        if (Len(path_)<68) {
            printf("%s", Char(path_));
            printf("%s", std::string(68-Len(path_), '.').c_str());
        } else {
            printf("%s", Char(path_) + (Len(path_) - 68));
        }
        Printf(b3, "\n");
        Printf(b3, "// END buffer %s\n", name_);
        Printf(b3, "\n");
        outputBuffer_ = NewString("");
        Dump(b0, outputBuffer_);
        Dump(b1, outputBuffer_);
        Dump(b2, outputBuffer_);
        Dump(b3, outputBuffer_);
        if (fileExists()) {
            if (fileChanged()) {
                writeFile();
                count.updated++;
                printf(".....Updated\n");
            } else {
                count.unchanged++;
                printf("...Unchanged\n");
            }
        } else {
            writeFile();
            count.created++;
            printf(".....Created\n");
        }
        count.total++;
        Delete(outputBuffer_);
    }
    void clear(Count &count, bool generateFile = true) {
        if (generateFile)
            clearImpl(count);
        Delete(b0);
        Delete(b1);
        Delete(b2);
        Delete(b3);
        Delete(name_);
        Delete(path_);
    }
};

//*****************************************************************************
// PARAMETERS
//*****************************************************************************

struct ParmsFunc {
    Node *n;
    ParmList *parms;
    Parm *parms2;
    String *name;
    String *symname;
    String *symnameUpper;
    String* funcName;
};

struct ParmsCtor {
    Node *n;
    String *name;
    String *rename;
    String *funcName;
    String *funcRename;
    ParmList *parms;
    ParmList *parms2;
    String *pname;
    String *base;
};

struct ParmsMemb {
    Node *n;
    String *nameUpper;
    String *funcName;
    ParmList *parms;
    ParmList *parms2;
    String *pname;
    String *addinClass;
    String *name;
    Node *node;
};

//*****************************************************************************
// GROUPS
//*****************************************************************************

struct GroupBase {
    Pragmas pragmas_;
    Count &count_;
    GroupBase(const Pragmas &pragmas, Count &count) : pragmas_(pragmas), count_(count) {}
    virtual ~GroupBase() {}
    virtual void functionWrapperImplFunc(ParmsFunc &) {}
    virtual void functionWrapperImplMemb(ParmsMemb &) {}
    virtual void functionWrapperImplCtor(ParmsCtor &) {}
};

struct GroupLibraryObjects : public GroupBase {

    Buffer *b_lib_grp_hpp;
    bool generateHppFile;

    GroupLibraryObjects(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), generateHppFile(false) {

        if (pragmas_.automatic_) {
            b_lib_grp_hpp = new Buffer("b_lib_grp_hpp", NewStringf("%s/obj_%s.hpp", objDir, pragmas_.groupName_));
        } else {
            b_lib_grp_hpp = new Buffer("b_lib_grp_hpp", NewStringf("%s/objmanual_%s.hpp.template", objDir, pragmas_.groupName_));
        }

        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#ifndef obj_%s_hpp\n", pragmas_.groupName_);
        Printf(b_lib_grp_hpp->b0, "#define obj_%s_hpp\n", pragmas_.groupName_);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#include <string>\n");
        Printf(b_lib_grp_hpp->b0, "#include <rp/libraryobject.hpp>\n");
        Printf(b_lib_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_lib_grp_hpp->b0, "#include <boost/shared_ptr.hpp>");

        Append(b_lib_grp_hpp->b0, pragmas_.lib_inc);

        Printf(b_lib_grp_hpp->b0, "// FIXME get rid of this:\n", nmspace);
        Printf(b_lib_grp_hpp->b0, "using namespace %s;\n", nmspace);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0,"namespace %s {\n", module);
        Printf(b_lib_grp_hpp->b0, "\n");
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        if (generateCtor) {

            String *s0 = NewString("");
            String *s1 = NewString("");
            if (p.base) {
                s0 = p.base;
                s1 = NewStringf("%s::%s", nmspace, p.base);
            } else {
                s0 = NewStringf("reposit::LibraryObject<%s>", p.pname);
                s1 = p.pname;
            }
            Printf(b_lib_grp_hpp->b0,"\n");
            Printf(b_lib_grp_hpp->b0,"    class %s : \n", p.name);
            Printf(b_lib_grp_hpp->b0,"        public %s {\n", s0);
            Printf(b_lib_grp_hpp->b0,"    public:\n");
            Printf(b_lib_grp_hpp->b0,"        %s(\n", p.name);
            Printf(b_lib_grp_hpp->b0,"            const boost::shared_ptr<reposit::ValueObject>& properties,\n");
            emitParmList(p.parms, b_lib_grp_hpp->b0, 2, "rp_tm_default", 3, ',', true, false, true);
            Printf(b_lib_grp_hpp->b0,"            bool permanent)\n");
            Printf(b_lib_grp_hpp->b0,"        : %s(properties, permanent) {\n", s0);
            Printf(b_lib_grp_hpp->b0,"            libraryObject_ = boost::shared_ptr<%s>(new %s(\n", s1, p.pname);
            emitParmList(p.parms, b_lib_grp_hpp->b0, 0, "rp_tm_default", 4);
            Printf(b_lib_grp_hpp->b0,"            ));\n");
            Printf(b_lib_grp_hpp->b0,"        }\n");
            Printf(b_lib_grp_hpp->b0,"    };\n");
            Printf(b_lib_grp_hpp->b0,"\n");

        } else { //!generateCtor

            Printf(b_lib_grp_hpp->b0, "    // BEGIN typemap rp_tm_lib_cls\n");
            if (parent) {
                Printf(b_lib_grp_hpp->b0, "    RP_OBJ_CLASS(%s, %s);\n", p.name, parent);
            } else {
                Printf(b_lib_grp_hpp->b0, "    RP_LIB_CLASS(%s, %s);\n", p.name, libraryClass);
            }
            Printf(b_lib_grp_hpp->b0, "    // END   typemap rp_tm_lib_cls\n");
        }

        count_.constructors++;
        count_.total2++;
        generateHppFile = true;
    }

    void clear() {

        Printf(b_lib_grp_hpp->b0, "} // namespace %s\n", module);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#endif\n");
        Printf(b_lib_grp_hpp->b0, "\n");

        b_lib_grp_hpp->clear(count_, generateHppFile);
    }
};

struct GroupValueObjects : public GroupBase {

    Buffer *b_vob_grp_hpp;
    Buffer *b_vob_grp_cpp;
    bool generateOutput;

    GroupValueObjects(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), generateOutput(false) {

        b_vob_grp_hpp = new Buffer("b_vob_grp_hpp", NewStringf("%s/valueobjects/vo_%s.hpp", objDir, pragmas_.groupName_));
        b_vob_grp_cpp = new Buffer("b_vob_grp_cpp", NewStringf("%s/valueobjects/vo_%s.cpp", objDir, pragmas_.groupName_));

        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "#ifndef vo_%s_hpp\n", pragmas_.groupName_);
        Printf(b_vob_grp_hpp->b0, "#define vo_%s_hpp\n", pragmas_.groupName_);
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_vob_grp_hpp->b0, "#include <string>\n");
        Printf(b_vob_grp_hpp->b0, "#include <vector>\n");
        Printf(b_vob_grp_hpp->b0, "#include <set>\n");
        Printf(b_vob_grp_hpp->b0, "#include <boost/serialization/map.hpp>\n");
        Printf(b_vob_grp_hpp->b0, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0,"namespace %s {\n", module);
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "namespace ValueObjects {\n");
        Printf(b_vob_grp_hpp->b0, "\n");

        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, pragmas_.groupName_);
        Printf(b_vob_grp_cpp->b0, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0,"namespace %s {\n", module);
        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "namespace ValueObjects {\n");
        Printf(b_vob_grp_cpp->b0, "\n");
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        Printf(b_vob_grp_hpp->b0,"        class %s : public reposit::ValueObject {\n", p.funcRename);
        Printf(b_vob_grp_hpp->b0,"            friend class boost::serialization::access;\n");
        Printf(b_vob_grp_hpp->b0,"        public:\n");
        Printf(b_vob_grp_hpp->b0,"            %s() {}\n", p.funcRename);
        Printf(b_vob_grp_hpp->b0,"            %s(\n", p.funcRename);
        Printf(b_vob_grp_hpp->b0,"                const std::string& ObjectId,\n");
        emitParmList(p.parms, b_vob_grp_hpp->b0, 2, "rp_tm_vob_parm", 4, ',', true, false, true);
        Printf(b_vob_grp_hpp->b0,"                bool Permanent);\n");
        Printf(b_vob_grp_hpp->b0,"\n");
        Printf(b_vob_grp_hpp->b0,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
        Printf(b_vob_grp_hpp->b0,"            std::vector<std::string> getPropertyNamesVector() const;\n");
        Printf(b_vob_grp_hpp->b0,"            reposit::property_t getSystemProperty(const std::string&) const;\n");
        Printf(b_vob_grp_hpp->b0,"            void setSystemProperty(const std::string& name, const reposit::property_t& value);\n");
        Printf(b_vob_grp_hpp->b0,"\n");
        Printf(b_vob_grp_hpp->b0,"        protected:\n");
        Printf(b_vob_grp_hpp->b0,"            static const char* mPropertyNames[];\n");
        Printf(b_vob_grp_hpp->b0,"            static std::set<std::string> mSystemPropertyNames;\n");
        emitParmList(p.parms, b_vob_grp_hpp->b0, 1, "rp_tm_vob_mbvr", 3, ';', true, false, true);
        Printf(b_vob_grp_hpp->b0,"            bool Permanent_;\n");
        if (String *processorName = Getattr(p.n, "feature:rp:processorName"))
            Printf(b_vob_grp_hpp->b0,"            virtual std::string processorName() { return \"%s\"; }\n", processorName);
        Printf(b_vob_grp_hpp->b0,"\n");
        Printf(b_vob_grp_hpp->b0,"            template<class Archive>\n");
        Printf(b_vob_grp_hpp->b0,"            void serialize(Archive& ar, const unsigned int) {\n");
        Printf(b_vob_grp_hpp->b0,"            boost::serialization::void_cast_register<%s, reposit::ValueObject>(this, this);\n", p.funcRename);
        Printf(b_vob_grp_hpp->b0,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"ClassName\", className_)\n");
        emitParmList(p.parms, b_vob_grp_hpp->b0, 1, "rp_tm_vob_srmv", 5, 0);
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
        Printf(b_vob_grp_hpp->b0,"            }\n");
        Printf(b_vob_grp_hpp->b0,"        };\n");
        Printf(b_vob_grp_hpp->b0,"\n");

        Printf(b_vob_grp_cpp->b0,"        const char* %s::mPropertyNames[] = {\n", p.funcRename);
        emitParmList(p.parms, b_vob_grp_cpp->b0, 1, "rp_tm_vob_name", 3, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"            \"Permanent\"\n");
        Printf(b_vob_grp_cpp->b0,"        };\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        std::set<std::string> %s::mSystemPropertyNames(\n", p.funcRename);
        Printf(b_vob_grp_cpp->b0,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", p.funcRename);
        Printf(b_vob_grp_cpp->b0,"            return mSystemPropertyNames;\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", p.funcRename);
        Printf(b_vob_grp_cpp->b0,"            std::vector<std::string> ret(\n");
        Printf(b_vob_grp_cpp->b0,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
        Printf(b_vob_grp_cpp->b0,"            for (std::map<std::string, reposit::property_t>::const_iterator i = userProperties.begin();\n");
        Printf(b_vob_grp_cpp->b0,"                i != userProperties.end(); ++i)\n");
        Printf(b_vob_grp_cpp->b0,"                ret.push_back(i->first);\n");
        Printf(b_vob_grp_cpp->b0,"            return ret;\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        reposit::property_t %s::getSystemProperty(const std::string& name) const {\n", p.funcRename);
        Printf(b_vob_grp_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
        Printf(b_vob_grp_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                return objectId_;\n");
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                return className_;\n");
        voGetProp(b_vob_grp_cpp->b0, p.parms);
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                return Permanent_;\n");
        Printf(b_vob_grp_cpp->b0,"            else\n");
        Printf(b_vob_grp_cpp->b0,"                RP_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        void %s::setSystemProperty(const std::string& name, const reposit::property_t& value) {\n", p.funcRename);
        Printf(b_vob_grp_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
        Printf(b_vob_grp_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                objectId_ = boost::get<std::string>(value);\n");
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                className_ = boost::get<std::string>(value);\n");
        voSetProp(b_vob_grp_cpp->b0, p.parms);
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                Permanent_ = reposit::convert2<bool>(value);\n");
        Printf(b_vob_grp_cpp->b0,"            else\n");
        Printf(b_vob_grp_cpp->b0,"                RP_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        %s::%s(\n", p.funcRename, p.funcRename);
        Printf(b_vob_grp_cpp->b0,"                const std::string& ObjectId,\n");
        emitParmList(p.parms, b_vob_grp_cpp->b0, 2, "rp_tm_vob_parm", 4, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"                bool Permanent) :\n");
        Printf(b_vob_grp_cpp->b0,"            reposit::ValueObject(ObjectId, \"%s\", Permanent),\n", p.funcRename);
        emitParmList(p.parms, b_vob_grp_cpp->b0, 1, "rp_tm_vob_init", 3, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"            Permanent_(Permanent) {\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");

        count_.constructors++;
        count_.total2++;

        generateOutput = true;
    }

    void functionWrapperImplMemb(ParmsMemb & /*p*/) {
    }

    void clear() {

        Printf(b_vob_grp_hpp->b0, "} // namespace %s\n", module);
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "} // namespace ValueObjects\n");
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "#endif\n");
        Printf(b_vob_grp_hpp->b0, "\n");

        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "} // namespace %s\n", module);
        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "} // namespace ValueObjects\n");
        Printf(b_vob_grp_cpp->b0, "\n");

        b_vob_grp_hpp->clear(count_, generateOutput);
        b_vob_grp_cpp->clear(count_, generateOutput);
    }
};

struct GroupSerializationCreate : public GroupBase {

    Buffer *b_scr_grp_hpp;
    Buffer *b_scr_grp_cpp;
    bool generateOutput;

    GroupSerializationCreate(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), generateOutput(false) {

        b_scr_grp_hpp = new Buffer("b_scr_grp_hpp", NewStringf("%s/serialization/create/create_%s.hpp", objDir, pragmas_.groupName_));
        b_scr_grp_cpp = new Buffer("b_scr_grp_cpp", NewStringf("%s/serialization/create/create_%s.cpp", objDir, pragmas_.groupName_));

        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "#ifndef create_%s_hpp\n", pragmas_.groupName_);
        Printf(b_scr_grp_hpp->b0, "#define create_%s_hpp\n", pragmas_.groupName_);
        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "#include <rp/rpdefines.hpp>\n");
        Printf(b_scr_grp_hpp->b0, "#include <rp/object.hpp>\n");
        Printf(b_scr_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "namespace %s {\n", module);
        Printf(b_scr_grp_hpp->b0, "\n");

        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, pragmas_.groupName_);
        Printf(b_scr_grp_cpp->b0, "//#include <%s/qladdindefines.hpp>\n", objInc);
        //Printf(b_scr_grp_cpp->b0, "#include <%s/conversions/convert2.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b0, "//#include <%s/handle.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b0, "\n");
        if (pragmas_.automatic_) {
            Printf(b_scr_grp_cpp->b0, "#include <%s/obj_%s.hpp>\n", objInc, pragmas_.groupName_);
        } else {
            Printf(b_scr_grp_cpp->b0, "#include <%s/objmanual_%s.hpp>\n", objInc, pragmas_.groupName_);
        }
        Append(b_scr_grp_cpp->b0, pragmas_.add_inc);
        Printf(b_scr_grp_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, pragmas_.groupName_);
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "#include <%s/conversions/all.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b0, "#include <%s/enumerations/factories/all.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b0, "#include <rp/property.hpp>\n");
        Printf(b_scr_grp_cpp->b0, "\n");
    }

    void functionWrapperImplFunc(ParmsFunc & /*p*/) {
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "boost::shared_ptr<reposit::Object> create_%s(\n", p.funcRename);
        Printf(b_scr_grp_hpp->b0, "    const boost::shared_ptr<reposit::ValueObject>&);\n");

        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "boost::shared_ptr<reposit::Object> %s::create_%s(\n", module, p.funcRename);
        Printf(b_scr_grp_cpp->b0, "    const boost::shared_ptr<reposit::ValueObject> &valueObject) {\n");
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    // conversions\n\n");
        emitParmList(p.parms, b_scr_grp_cpp->b0, 1, "rp_tm_scr_cnvt", 1, 0);
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    bool Permanent =\n");
        Printf(b_scr_grp_cpp->b0, "        reposit::convert2<bool>(valueObject->getProperty(\"Permanent\"));\n");
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    // construct and return the object\n");
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    boost::shared_ptr<reposit::Object> object(\n");
        Printf(b_scr_grp_cpp->b0, "        new %s::%s(\n", module, p.name);
        Printf(b_scr_grp_cpp->b0, "            valueObject,\n");
        emitParmList(p.parms, b_scr_grp_cpp->b0, 0, "rp_tm_default", 3, ',', true, false, true);
        Printf(b_scr_grp_cpp->b0, "            Permanent));\n");
        Printf(b_scr_grp_cpp->b0, "    return object;\n");
        Printf(b_scr_grp_cpp->b0, "}\n");

        count_.constructors++;
        count_.total2++;

        generateOutput = true;
    }

    void clear() {

        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "} // namespace %s\n", module);
        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "#endif\n");
        Printf(b_scr_grp_hpp->b0, "\n");

        Printf(b_scr_grp_cpp->b0, "\n");

        b_scr_grp_hpp->clear(count_, generateOutput);
        b_scr_grp_cpp->clear(count_, generateOutput);
    }
};

struct GroupSerializationRegister : public GroupBase {

    Buffer *b_srg_grp_hpp;
    Buffer *b_srg_grp_cpp;
    bool generateOutput;

    GroupSerializationRegister(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), generateOutput(false) {

        b_srg_grp_hpp = new Buffer("b_srg_grp_hpp", NewStringf("%s/serialization/register/serialization_%s.hpp", objDir, pragmas_.groupName_));
        b_srg_grp_cpp = new Buffer("b_srg_grp_cpp", NewStringf("%s/serialization/register/serialization_%s.cpp", objDir, pragmas_.groupName_));

        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "#ifndef serialization_%s_hpp\n", pragmas_.groupName_);
        Printf(b_srg_grp_hpp->b0, "#define serialization_%s_hpp\n", pragmas_.groupName_);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "#include <boost/archive/xml_iarchive.hpp>\n");
        Printf(b_srg_grp_hpp->b0, "#include <boost/archive/xml_oarchive.hpp>\n");
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "namespace %s {\n", module);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "    void register_%s(boost::archive::xml_oarchive &ar);\n", pragmas_.groupName_);
        Printf(b_srg_grp_hpp->b0, "    void register_%s(boost::archive::xml_iarchive &ar);\n", pragmas_.groupName_);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "} // namespace %s\n", module);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "#endif\n");
        Printf(b_srg_grp_hpp->b0, "\n");

        Printf(b_srg_grp_cpp->b0, "\n");
        Printf(b_srg_grp_cpp->b0, "#include <rp/rpdefines.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, pragmas_.groupName_);
        Printf(b_srg_grp_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, pragmas_.groupName_);
        Printf(b_srg_grp_cpp->b0, "#include <boost/serialization/shared_ptr.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "#include <boost/serialization/variant.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "#include <boost/serialization/vector.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "\n");
        Printf(b_srg_grp_cpp->b0, "void %s::register_%s(boost::archive::xml_oarchive &ar) {\n", module, pragmas_.groupName_);
        Printf(b_srg_grp_cpp->b0, "\n");

        Printf(b_srg_grp_cpp->b1, "\n");
        Printf(b_srg_grp_cpp->b1, "void %s::register_%s(boost::archive::xml_iarchive &ar) {\n", module, pragmas_.groupName_);
        Printf(b_srg_grp_cpp->b1, "\n");
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        Printf(b_srg_grp_cpp->b0, "    // class ID %d in the boost serialization framework\n", idNum);
        Printf(b_srg_grp_cpp->b0, "    ar.register_type<%s::ValueObjects::%s>();\n", module, p.funcRename);

        Printf(b_srg_grp_cpp->b1, "    // class ID %d in the boost serialization framework\n", idNum);
        Printf(b_srg_grp_cpp->b1, "    ar.register_type<%s::ValueObjects::%s>();\n", module, p.funcRename);

        idNum++;

        count_.constructors++;
        count_.total2++;

        generateOutput = true;
    }

    void clear() {

        Printf(b_srg_grp_hpp->b0, "\n");

        Printf(b_srg_grp_cpp->b0, "}\n");
        Printf(b_srg_grp_cpp->b0, "\n");

        Printf(b_srg_grp_cpp->b1, "}\n");
        Printf(b_srg_grp_cpp->b1, "\n");

        b_srg_grp_hpp->clear(count_, generateOutput);
        b_srg_grp_cpp->clear(count_, generateOutput);
    }
};

struct GroupCpp : public GroupBase {

    Buffer *b_cpp_grp_hpp;
    Buffer *b_cpp_grp_cpp;
    bool groupContainsClass;
    bool groupContainsConstructor;

    GroupCpp(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), groupContainsClass(false), groupContainsConstructor(false) {

        b_cpp_grp_hpp = new Buffer("b_cpp_grp_hpp", NewStringf("%s/add_%s.hpp", addDir, pragmas_.groupName_));
        b_cpp_grp_cpp = new Buffer("b_cpp_grp_cpp", NewStringf("%s/add_%s.cpp", addDir, pragmas_.groupName_));

        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "#ifndef add_%s_hpp\n", pragmas_.groupName_);
        Printf(b_cpp_grp_hpp->b0, "#define add_%s_hpp\n", pragmas_.groupName_);
        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "#include <string>\n");        
        Printf(b_cpp_grp_hpp->b0, "// FIXME this #include is only needed if a datatype conversion is taking place.\n");
        Printf(b_cpp_grp_hpp->b0, "#include <rp/property.hpp>\n");
        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "namespace %s {\n", addinCppNameSpace);
        Printf(b_cpp_grp_hpp->b0, "\n");

        Printf(b_cpp_grp_cpp->b0, "\n");
        Printf(b_cpp_grp_cpp->b0, "#include <AddinCpp/add_%s.hpp>\n", pragmas_.groupName_);
        Printf(b_cpp_grp_cpp->b0, "//FIXME this #include is only required if the file contains conversions\n", objInc);
        Printf(b_cpp_grp_cpp->b0, "#include <%s/conversions/all.hpp>\n", objInc);
        Printf(b_cpp_grp_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
        Printf(b_cpp_grp_cpp->b0, "#include <%s/conversions/coercetermstructure.hpp>\n", objInc);
        // FIXME this #include is only required if the file contains enumerations.
        //Printf(b_cpp_grp_cpp->b0, "#include <rp/enumerations/typefactory.hpp>\n");

        // From this point on we stop writing to b0 and write to b1 instead.
        // After all processing finishes we will append some more #includes to b0 depending on what code this group requires.

        Printf(b_cpp_grp_cpp->b1, "//FIXME include only factories for types used in the current file\n", objInc);
        Printf(b_cpp_grp_cpp->b1, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        Printf(b_cpp_grp_cpp->b1, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_cpp_grp_cpp->b1, "#include <rp/repository.hpp>\n");
        //Printf(b_cpp_grp_cpp->b1, "#include <AddinCpp/add_all.hpp>\n");
        Printf(b_cpp_grp_cpp->b1, "\n");
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        emitTypeMap(b_cpp_grp_hpp->b0, p.n, "rp_tm_cpp_rttp", 1);
        Printf(b_cpp_grp_hpp->b0,"    %s(\n", p.funcName);
        emitParmList(p.parms, b_cpp_grp_hpp->b0, 2, "rp_tm_cpp_parm", 2);
        Printf(b_cpp_grp_hpp->b0,"    );\n");

        Printf(b_cpp_grp_cpp->b1,"//****FUNC*****\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rttp");
        Printf(b_cpp_grp_cpp->b1,"%s::%s(\n", addinCppNameSpace, p.funcName);
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 2, "rp_tm_cpp_parm");
        Printf(b_cpp_grp_cpp->b1,") {\n");
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_cnvt", 1, 0, false);
        Printf(b_cpp_grp_cpp->b1,"\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtdc", 2);
        Printf(b_cpp_grp_cpp->b1,"    %s::%s(\n", nmspace, p.symname);
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_args", 2, ',', true, true);
        Printf(b_cpp_grp_cpp->b1,"    );\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtst", 2);
        Printf(b_cpp_grp_cpp->b1,"}\n");

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        if (generateCtor) {
            Printf(b_cpp_grp_hpp->b0,"\n");
            Printf(b_cpp_grp_hpp->b0,"    std::string %s(\n", p.funcName);
            emitParmList(p.parms2, b_cpp_grp_hpp->b0, 2, "rp_tm_cpp_parm", 2);
            Printf(b_cpp_grp_hpp->b0,"    );\n\n");

            Printf(b_cpp_grp_cpp->b1,"//****CTOR*****\n");
            Printf(b_cpp_grp_cpp->b1,"std::string %s::%s(\n", addinCppNameSpace, p.funcName);
            emitParmList(p.parms2, b_cpp_grp_cpp->b1, 2, "rp_tm_cpp_parm", 2);
            Printf(b_cpp_grp_cpp->b1,"    ) {\n");
            Printf(b_cpp_grp_cpp->b1,"\n");
            Printf(b_cpp_grp_cpp->b1,"    // Convert input types into Library types\n\n");
            emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_cnvt", 1, 0, false);
            Printf(b_cpp_grp_cpp->b1,"\n");
            Printf(b_cpp_grp_cpp->b1,"    boost::shared_ptr<reposit::ValueObject> valueObject(\n");
            Printf(b_cpp_grp_cpp->b1,"        new %s::ValueObjects::%s(\n", module, p.funcName);
            Printf(b_cpp_grp_cpp->b1,"            objectID,\n");
            emitParmList(p.parms, b_cpp_grp_cpp->b1, 0, "rp_tm_default", 3, ',', true, false, true);
            Printf(b_cpp_grp_cpp->b1,"            false));\n");
            Printf(b_cpp_grp_cpp->b1,"    boost::shared_ptr<reposit::Object> object(\n");
            Printf(b_cpp_grp_cpp->b1,"        new %s::%s(\n", module, p.name);
            Printf(b_cpp_grp_cpp->b1,"            valueObject,\n");
            emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_args", 3, ',', true, true, true);
            Printf(b_cpp_grp_cpp->b1,"            false));\n");
            Printf(b_cpp_grp_cpp->b1,"    std::string returnValue =\n");
            Printf(b_cpp_grp_cpp->b1,"        reposit::Repository::instance().storeObject(\n");
            Printf(b_cpp_grp_cpp->b1,"            objectID, object, false, valueObject);\n");
            Printf(b_cpp_grp_cpp->b1,"    return returnValue;\n");
            Printf(b_cpp_grp_cpp->b1,"}\n\n");

            count_.constructors++;
            count_.total2++;
            groupContainsConstructor = true;
        }
        groupContainsClass = true;
    }

    void functionWrapperImplMemb(ParmsMemb &p) {
        emitTypeMap(b_cpp_grp_hpp->b0, p.n, "rp_tm_cpp_rtmb", 1);
        Printf(b_cpp_grp_hpp->b0,"    %s(\n", p.funcName);
        emitParmList(p.parms2, b_cpp_grp_hpp->b0, 2, "rp_tm_cpp_parm", 2);
        Printf(b_cpp_grp_hpp->b0,"    );\n\n");

        Printf(b_cpp_grp_cpp->b1,"//****MEMB*****\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtmb");
        Printf(b_cpp_grp_cpp->b1,"%s::%s(\n", addinCppNameSpace, p.funcName);
        emitParmList(p.parms2, b_cpp_grp_cpp->b1, 2, "rp_tm_cpp_parm", 2);
        Printf(b_cpp_grp_cpp->b1,"    ) {\n\n");
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_cnvt", 1, 0, false);
        Printf(b_cpp_grp_cpp->b1,"\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.node, "rp_tm_xxx_rp_get");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtdc", 2);
        Printf(b_cpp_grp_cpp->b1,"    xxx->%s(\n", p.name);
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_args", 3, ',', true, true);
        Printf(b_cpp_grp_cpp->b1,"        );\n", p.name);
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtst", 2);
        Printf(b_cpp_grp_cpp->b1,"}\n");

        count_.members++;
        count_.total2++;
    }

    void clear() {

        if (groupContainsConstructor)
            Printf(b_cpp_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, pragmas_.groupName_);
        if (groupContainsClass) {
            if (pragmas_.automatic_) {
                Printf(b_cpp_grp_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
            } else {
                Printf(b_cpp_grp_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
            }
        }
        Append(b_cpp_grp_cpp->b0, pragmas_.add_inc);

        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "} // namespace %s\n", addinCppNameSpace);
        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "#endif\n");
        Printf(b_cpp_grp_hpp->b0, "\n");

        Printf(b_cpp_grp_cpp->b1, "\n");

        b_cpp_grp_hpp->clear(count_);
        b_cpp_grp_cpp->clear(count_);
    }
};

struct GroupCSharp : public GroupBase {

    Buffer *b_csh_grp_cpp;
    bool groupContainsClass;
    bool groupContainsConstructor;

    GroupCSharp(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), groupContainsClass(false), groupContainsConstructor(false) {

        b_csh_grp_cpp = new Buffer("b_csh_grp_cpp", NewStringf("%s/csh_%s.cpp", cshDir, pragmas_.groupName_));

        if (pragmas_.automatic_) {
            Printf(b_csh_grp_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
        } else {
            Printf(b_csh_grp_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
        }

        Printf(b_csh_grp_cpp->b0, "#include <rp/repository.hpp>\n");
        Printf(b_csh_grp_cpp->b0, "//FIXME include only factories for types used in the current file\n", objInc);
        Printf(b_csh_grp_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        Printf(b_csh_grp_cpp->b0, "#include <windows.h>\n");
        // From this point on we stop writing to b0 and write to b1 instead.
        // After all processing finishes we will append some more #includes to b0 depending on what code this group requires.
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        Printf(b_csh_grp_cpp->b1, "extern \"C\" __declspec(dllexport)\n");
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rttp");
        Printf(b_csh_grp_cpp->b1, "__stdcall %s(\n", p.funcName);
        emitParmList(p.parms, b_csh_grp_cpp->b1, 2, "rp_tm_csh_parm", 2);
        Printf(b_csh_grp_cpp->b1, ") {\n");
        Printf(b_csh_grp_cpp->b1, "    try {\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"BEGIN - FUNCTION '%s'\" << std::endl;\n", p.funcName);
        Printf(b_csh_grp_cpp->b1, "\n");
        emitParmList(p.parms, b_csh_grp_cpp->b1, 1, "rp_tm_csh_cnvt", 2, 0, false);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtdc", 2);
        Printf(b_csh_grp_cpp->b1, "        %s(\n", p.name);
        emitParmList(p.parms, b_csh_grp_cpp->b1, 1, "rp_tm_csh_args", 3, ',', true, true);
        Printf(b_csh_grp_cpp->b1, "        );\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"END   - FUNCTION '%s'\" << std::endl;\n", p.funcName);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtst", 2);
        Printf(b_csh_grp_cpp->b1,"    } catch (const std::exception &e) {\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"ERROR - FUNCTION '%s' - \" << e.what() << std::endl;\n", p.funcName);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtex", 2, false);
        Printf(b_csh_grp_cpp->b1,"    } catch (...) {\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"ERROR - FUNCTION '%s' - UNKNOWN EXCEPTION\" << std::endl;\n", p.funcName);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtex", 2, false);
        Printf(b_csh_grp_cpp->b1,"    }\n");
        Printf(b_csh_grp_cpp->b1, "}\n");

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            Printf(b_csh_grp_cpp->b1, "extern \"C\" __declspec(dllexport) char * __stdcall %s(\n", p.funcName);
            emitParmList(p.parms2, b_csh_grp_cpp->b1, 2, "rp_tm_csh_parm", 1);
            Printf(b_csh_grp_cpp->b1, ") {\n");
            Printf(b_csh_grp_cpp->b1, "    try {\n");
            Printf(b_csh_grp_cpp->b1,"        std::cout << \"BEGIN - FUNCTION '%s'\" << std::endl;\n", p.funcName);
            Printf(b_csh_grp_cpp->b1, "\n");
            emitParmList(p.parms, b_csh_grp_cpp->b1, 1, "rp_tm_csh_cnvt", 2, 0, false);
            Printf(b_csh_grp_cpp->b1,"\n");
            Printf(b_csh_grp_cpp->b1,"        boost::shared_ptr<reposit::ValueObject> valueObject(\n");
            Printf(b_csh_grp_cpp->b1,"            new %s::ValueObjects::%s(\n", module, p.funcName);
            Printf(b_csh_grp_cpp->b1,"                objectID,\n");
            emitParmList(p.parms, b_csh_grp_cpp->b1, 0, "rp_tm_default", 4, ',', true, false, true);
            Printf(b_csh_grp_cpp->b1,"                false));\n");
            Printf(b_csh_grp_cpp->b1,"        boost::shared_ptr<reposit::Object> object(\n");
            Printf(b_csh_grp_cpp->b1,"            new %s::%s(\n", module, p.name);
            Printf(b_csh_grp_cpp->b1,"                valueObject,\n");
            emitParmList(p.parms, b_csh_grp_cpp->b1, 1, "rp_tm_csh_args", 4, ',', true, true, true);
            Printf(b_csh_grp_cpp->b1,"                false));\n");
            Printf(b_csh_grp_cpp->b1,"        std::string returnValue =\n");
            Printf(b_csh_grp_cpp->b1,"            reposit::Repository::instance().storeObject(\n");
            Printf(b_csh_grp_cpp->b1,"                objectID, object, false, valueObject);\n");
            Printf(b_csh_grp_cpp->b1,"\n");
            Printf(b_csh_grp_cpp->b1,"        ULONG size = returnValue.length() + sizeof(char);\n");
            Printf(b_csh_grp_cpp->b1,"        char *ret = (char*)::CoTaskMemAlloc(size);\n");
            Printf(b_csh_grp_cpp->b1,"        strcpy(ret, returnValue.c_str());\n");
            Printf(b_csh_grp_cpp->b1,"        std::cout << \"END   - FUNCTION '%s'\" << std::endl;\n", p.funcName);
            Printf(b_csh_grp_cpp->b1,"        return ret;\n");
            Printf(b_csh_grp_cpp->b1,"    } catch (const std::exception &e) {\n");
            Printf(b_csh_grp_cpp->b1,"        std::cout << \"ERROR - FUNCTION '%s' - \" << e.what() << std::endl;\n", p.funcName);
            Printf(b_csh_grp_cpp->b1,"        return 0;\n");
            Printf(b_csh_grp_cpp->b1,"    } catch (...) {\n");
            Printf(b_csh_grp_cpp->b1,"        std::cout << \"ERROR - FUNCTION '%s' - UNKNOWN EXCEPTION\" << std::endl;\n", p.funcName);
            Printf(b_csh_grp_cpp->b1,"        return 0;\n");
            Printf(b_csh_grp_cpp->b1,"    }\n");
            Printf(b_csh_grp_cpp->b1,"}\n\n");

            count_.constructors++;
            count_.total2++;
            groupContainsConstructor = true;
        }
        groupContainsClass = true;
    }

    void functionWrapperImplMemb(ParmsMemb &p) {
        Printf(b_csh_grp_cpp->b1, "extern \"C\" __declspec(dllexport)\n");
        //emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtmb");
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rttp");
        Printf(b_csh_grp_cpp->b1, "__stdcall %s(\n", p.funcName);
        emitParmList(p.parms2, b_csh_grp_cpp->b1, 2, "rp_tm_csh_parm", 2);
        Printf(b_csh_grp_cpp->b1,"    ) {\n");
        Printf(b_csh_grp_cpp->b1, "    try {\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"BEGIN - FUNCTION '%s'\" << std::endl;\n", p.funcName);
        Printf(b_csh_grp_cpp->b1, "\n");
        emitParmList(p.parms, b_csh_grp_cpp->b1, 1, "rp_tm_csh_cnvt", 1, 0, false);
        Printf(b_csh_grp_cpp->b1,"\n");
        emitTypeMap(b_csh_grp_cpp->b1, p.node, "rp_tm_xxx_rp_get");
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtdc", 2);
        Printf(b_csh_grp_cpp->b1,"        xxx->%s(\n", p.name);
        emitParmList(p.parms, b_csh_grp_cpp->b1, 1, "rp_tm_csh_args", 3, ',', true, true);
        Printf(b_csh_grp_cpp->b1,"        );\n", p.name);
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"END   - FUNCTION '%s'\" << std::endl;\n", p.funcName);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtst", 2);
        Printf(b_csh_grp_cpp->b1,"    } catch (const std::exception &e) {\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"ERROR - FUNCTION '%s' - \" << e.what() << std::endl;\n", p.funcName);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtex", 2, false);
        Printf(b_csh_grp_cpp->b1,"    } catch (...) {\n");
        Printf(b_csh_grp_cpp->b1,"        std::cout << \"ERROR - FUNCTION '%s' - UNKNOWN EXCEPTION\" << std::endl;\n", p.funcName);
        emitTypeMap(b_csh_grp_cpp->b1, p.n, "rp_tm_csh_rtex", 2, false);
        Printf(b_csh_grp_cpp->b1,"    }\n");
        Printf(b_csh_grp_cpp->b1,"}\n");

        count_.members++;
        count_.total2++;
    }

    void clear() {

        if (groupContainsConstructor)
            Printf(b_csh_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, pragmas_.groupName_);
        if (groupContainsClass) {
            if (pragmas_.automatic_) {
                Printf(b_csh_grp_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
            } else {
                Printf(b_csh_grp_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
            }
        }
        Append(b_csh_grp_cpp->b0, pragmas_.add_inc);

        Printf(b_csh_grp_cpp->b1, "\n");
        b_csh_grp_cpp->clear(count_);
    }
};

struct GroupExcelFunctions : public GroupBase {

    Buffer *b_xlf_grp_cpp;
    bool groupContainsClass;
    bool groupContainsConstructor;

    GroupExcelFunctions(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), groupContainsClass(false), groupContainsConstructor(false) {

        b_xlf_grp_cpp = new Buffer("b_xlf_grp_cpp", NewStringf("%s/functions/function_%s.cpp", xllDir, pragmas_.groupName_));

        Printf(b_xlf_grp_cpp->b0, "\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/repositxl.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/register/register_all.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/functions/export.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/utilities/xlutilities.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/objectwrapperxl.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "// FIXME only required if the file contains a looping function\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/loop.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
        Printf(b_xlf_grp_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        // From this point on we stop writing to b0 and write to b1 instead.
        // After all processing finishes we will append some more #includes to b0 depending on what code this group requires.

        Printf(b_xlf_grp_cpp->b1, "//#include \"%s/conversions/convert2.hpp\"\n", objInc);
        Printf(b_xlf_grp_cpp->b1, "#include \"%s/conversions/conversions.hpp\"\n", objInc);
        Printf(b_xlf_grp_cpp->b1, "#include \"%s/conversions/all.hpp\"\n", xllInc);
        Printf(b_xlf_grp_cpp->b1, "// FIXME only required if the file contains a looping function\n");
        Printf(b_xlf_grp_cpp->b1, "#include \"%s/loop.hpp\"\n", objInc);
        Printf(b_xlf_grp_cpp->b1, "\n");
    }

    void emitLoopFunc(ParmsFunc &p, String *loopParameter) {
        String *loopParameterType = Getattr(p.n, "rp:loopParameterType");
        String *loopFunctionType = Getattr(p.n, "rp:loopFunctionType");
        Printf(b_xlf_grp_cpp->b1, "        // BEGIN function emitLoopFunc\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        static XLOPER returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        %s::%sBind bindObject =\n", module, p.funcName);
        Printf(b_xlf_grp_cpp->b1, "            boost::bind(\n");
        Printf(b_xlf_grp_cpp->b1, "                %s,\n", p.name);
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xxx_loop", 4, ',', true, true);
        Printf(b_xlf_grp_cpp->b1, "            );\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::loop\n");
        Printf(b_xlf_grp_cpp->b1, "            <%s::%sBind, %s, %s>\n", module, p.funcName, loopParameterType, loopFunctionType);
        Printf(b_xlf_grp_cpp->b1, "            (functionCall, bindObject, %s, returnValue);\n", loopParameter);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        return &returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        // END   function emitLoopFunc\n");
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****FUNC*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT\n");
        emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtft");
        Printf(b_xlf_grp_cpp->b1, "%s(\n", p.funcName);
        emitParmList(p.parms2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", 1);
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", p.funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::validateRange(Trigger, \"Trigger\");\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (String *loopParameter = Getattr(p.n, "feature:rp:loopParameter")) {
            emitLoopFunc(p, loopParameter);
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtdc", 2);
            //Printf(b_xlf_grp_cpp->b1, "        %s::%s(\n", nmspace, p.symname);
            Printf(b_xlf_grp_cpp->b1, "        %s(\n", p.name);
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", 3, ',', true, true);
            Printf(b_xlf_grp_cpp->b1, "        );\n\n");
            emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtst", 2);
        }

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    } catch (const std::exception &e) {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::RepositoryXL::instance().logError(e.what(), functionCall);\n");
        Printf(b_xlf_grp_cpp->b1, "        return 0;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    }\n");
        Printf(b_xlf_grp_cpp->b1, "}\n");

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        if (generateCtor) {
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1,"//****CTOR*****\n");
            Printf(b_xlf_grp_cpp->b1, "DLLEXPORT char *%s(\n", p.funcRename);
            emitParmList(p.parms2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm");
            Printf(b_xlf_grp_cpp->b1, ") {\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "    try {\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
            Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", p.funcRename);
            Printf(b_xlf_grp_cpp->b1, "\n");
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", 2, 0, false);
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "        boost::shared_ptr<reposit::ValueObject> valueObject(\n");
            Printf(b_xlf_grp_cpp->b1, "            new %s::ValueObjects::%s(\n", module, p.funcRename);
            Printf(b_xlf_grp_cpp->b1, "                objectID,\n");
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argfv", 4, ',', true, true, true);
            Printf(b_xlf_grp_cpp->b1, "                false));\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "        boost::shared_ptr<reposit::Object> object(\n");
            Printf(b_xlf_grp_cpp->b1, "            new %s::%s(\n", module, p.name);
            Printf(b_xlf_grp_cpp->b1, "                valueObject,\n");
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", 4, ',', true, true, true);
            Printf(b_xlf_grp_cpp->b1, "                false));\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "        std::string returnValue =\n");
            Printf(b_xlf_grp_cpp->b1, "            reposit::RepositoryXL::instance().storeObject(objectID, object, true);\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "        static char ret[XL_MAX_STR_LEN];\n");
            Printf(b_xlf_grp_cpp->b1, "        reposit::stringToChar(returnValue, ret);\n");
            Printf(b_xlf_grp_cpp->b1, "        return ret;\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "    } catch (const std::exception &e) {\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "        reposit::RepositoryXL::instance().logError(e.what(), functionCall);\n");
            Printf(b_xlf_grp_cpp->b1, "        return 0;\n");
            Printf(b_xlf_grp_cpp->b1, "\n");
            Printf(b_xlf_grp_cpp->b1, "    }\n");
            Printf(b_xlf_grp_cpp->b1, "}\n");

            count_.constructors++;
            count_.total2++;
            groupContainsConstructor = true;
        }
        groupContainsClass = true;
    }

    void emitLoopFunc(ParmsMemb &p, String *loopParameter) {
        String *loopParameterType = Getattr(p.n, "rp:loopParameterType");
        String *loopFunctionType = Getattr(p.n, "rp:loopFunctionType");
        Printf(b_xlf_grp_cpp->b1, "        // BEGIN function emitLoopFunc\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        static XLOPER returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        %s::%sBind bindObject =\n", module, p.funcName);
        Printf(b_xlf_grp_cpp->b1, "            boost::bind(\n");
        Printf(b_xlf_grp_cpp->b1, "                &%s::%s,\n", p.pname, p.name);
        Printf(b_xlf_grp_cpp->b1, "                xxx,\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xxx_loop", 4, ',', true, true);
        Printf(b_xlf_grp_cpp->b1, "            );\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::loop\n");
        Printf(b_xlf_grp_cpp->b1, "            <%s::%sBind, %s, %s>\n", module, p.funcName, loopParameterType, loopFunctionType);
        Printf(b_xlf_grp_cpp->b1, "            (functionCall, bindObject, %s, returnValue);\n", loopParameter);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        return &returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        // END   function emitLoopFunc\n");
    }

    void functionWrapperImplMemb(ParmsMemb &p) {

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****MEMB*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT\n");
        emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtft");
        Printf(b_xlf_grp_cpp->b1, "%s(\n", p.funcName);
        emitParmList(p.parms2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm");
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", p.funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitTypeMap(b_xlf_grp_cpp->b1, p.node, "rp_tm_xxx_rp_get", 2);
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (String *loopParameter = Getattr(p.n, "feature:rp:loopParameter")) {
            emitLoopFunc(p, loopParameter);
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtdc", 2);
            Printf(b_xlf_grp_cpp->b1, "        xxx->%s(\n", p.name);
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", 3, ',', true, true);
            Printf(b_xlf_grp_cpp->b1, "        );\n\n");
            emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtst", 2);
        }
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    } catch (const std::exception &e) {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::RepositoryXL::instance().logError(e.what(), functionCall);\n");
        Printf(b_xlf_grp_cpp->b1, "        return 0;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    }\n");
        Printf(b_xlf_grp_cpp->b1, "}\n");

        count_.members++;
        count_.total2++;
    }

    void clear() {

        if (groupContainsConstructor)
            Printf(b_xlf_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, pragmas_.groupName_);
        if (groupContainsClass) {
            if (pragmas_.automatic_) {
                Printf(b_xlf_grp_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
            } else {
                Printf(b_xlf_grp_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
            }
        }
        Append(b_xlf_grp_cpp->b0, pragmas_.add_inc);

        b_xlf_grp_cpp->clear(count_);
    }
};

struct GroupExcelRegister : public GroupBase {

    Buffer *b_xlr_grp_cpp;

    GroupExcelRegister(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count) {

        b_xlr_grp_cpp = new Buffer("b_xlr_grp_cpp", NewStringf("%s/register/register_%s.cpp", xllDir, pragmas_.groupName_));

        Printf(b_xlr_grp_cpp->b0, "\n");
        Printf(b_xlr_grp_cpp->b0, "#include <xlsdk/xlsdkdefines.hpp>\n");
        Printf(b_xlr_grp_cpp->b0, "\n");
        Printf(b_xlr_grp_cpp->b0, "void register_%s(const XLOPER &xDll) {\n", pragmas_.groupName_);
        Printf(b_xlr_grp_cpp->b0, "\n");
        Printf(b_xlr_grp_cpp->b1, "\n");
        Printf(b_xlr_grp_cpp->b1, "void unregister_%s(const XLOPER &xDll) {\n", pragmas_.groupName_);
        Printf(b_xlr_grp_cpp->b1, "\n");
        Printf(b_xlr_grp_cpp->b1, "    XLOPER xlRegID;\n");
        Printf(b_xlr_grp_cpp->b1, "\n");
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        excelRegister(b_xlr_grp_cpp->b0, p.n, p.parms2);
        excelUnregister(b_xlr_grp_cpp->b1, p.n, p.parms2);

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        excelRegister(b_xlr_grp_cpp->b0, p.n, p.parms2);
        excelUnregister(b_xlr_grp_cpp->b1, p.n, p.parms2);

        count_.constructors++;
        count_.total2++;
    }

    void functionWrapperImplMemb(ParmsMemb &p) {
        excelRegister(b_xlr_grp_cpp->b0, p.n, p.parms2);
        excelUnregister(b_xlr_grp_cpp->b1, p.n, p.parms2);

        count_.members++;
        count_.total2++;
    }

    void clear() {

        Printf(b_xlr_grp_cpp->b0, "}\n");
        Printf(b_xlr_grp_cpp->b1, "}\n");

        b_xlr_grp_cpp->clear(count_);
    }
};

void mongoParms(File *f, ParmList *parms) {
    if (parms) {
    Printf(f, ",\n");
    Printf(f, "            \"parameters\": [\n");
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
    String *name = Getattr(p,"name");
    if (first) {
        first = false;
    } else {
        Printf(f, ",\n");
    }
    Printf(f, "                {\n");
    Printf(f, "                    \"name\": \"%s\",\n", name);
    String *s = getTypeMap(p, "rp_tm_cfy_mngo");
    Printf(f, "                    \"dataType\": \"%s\",\n", s);
    Printf(f, "                    \"description\": \"\",\n");
    Printf(f, "                    \"optional\": false\n");
    Printf(f, "                }");
    }
    Printf(f, "\n");
    Printf(f, "            ]\n");
    } else {
    Printf(f, "\n");
    }
}

void mongoFunc(File *f, String *funcName1, String *funcName2, Node *n, ParmList *parms) {
    Printf(f, "        {\n");
    Printf(f, "            \"name\": \"%s\",\n", funcName1);
    Printf(f, "            \"codeName\": \"%s\",\n", funcName2);
    Printf(f, "            \"description\": \"\",\n");
    Printf(f, "            \"returnValue\": {\n");
    String *s = getTypeMap(n, "rp_tm_cfy_mngo");
    Printf(f, "                \"dataType\": \"%s\"\n", s);
    Printf(f, "            }");
    mongoParms(f, parms);
    Printf(f, "        },\n");
}

struct GroupCountify : public GroupBase {

    Buffer *b_cfy_grp_cpp;

    GroupCountify(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count) {

        b_cfy_grp_cpp = new Buffer("b_cfy_grp_cpp", NewStringf("%s/cfy_%s.cpp", cfyDir, pragmas_.groupName_));

        Printf(b_cfy_grp_cpp->b0, "\n");
        Printf(b_cfy_grp_cpp->b0, "#include \"init.hpp\"\n");
        Printf(b_cfy_grp_cpp->b0, "#include <rp/repository.hpp>\n");
        Printf(b_cfy_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, pragmas_.groupName_);
        Printf(b_cfy_grp_cpp->b0, "\n");
        Printf(b_cfy_grp_cpp->b0, "//FIXME this #include is only required if the file contains conversions\n", objInc);
        Printf(b_cfy_grp_cpp->b0, "#include <%s/conversions/all.hpp>\n", objInc);
        Printf(b_cfy_grp_cpp->b0, "#include <%s/conversions/coercetermstructure.hpp>\n", objInc);
        //Printf(b_cfy_grp_cpp->b0, "//FIXME this #include is only required if the file contains enumerations\n", objInc);
        //Printf(b_cfy_grp_cpp->b0, "#include <rp/enumerations/typefactory.hpp>\n");
        Printf(b_cfy_grp_cpp->b0, "//FIXME this #include is only required if the file contains constructors\n", objInc);
        Printf(b_cfy_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, pragmas_.groupName_);
        if (pragmas_.automatic_) {
            Printf(b_cfy_grp_cpp->b0, "#include \"%s/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
        } else {
            Printf(b_cfy_grp_cpp->b0, "#include \"%s/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
        }
        //Append(b_cfy_grp_cpp->b0, pragmas_.add_inc);
        Printf(b_cfy_grp_cpp->b0, "//FIXME include only factories for types used in the current file\n", objInc);
        Printf(b_cfy_grp_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        Printf(b_cfy_grp_cpp->b0, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_cfy_grp_cpp->b0, "#include <rp/repository.hpp>\n");
        //Printf(b_cfy_grp_cpp->b0, "#include <AddinCpp/add_all.hpp>\n");
        Printf(b_cfy_grp_cpp->b0, "\n");
    }

    void functionWrapperImplFunc(ParmsFunc &p) {
        Printf(b_cfy_grp_cpp->b0,"//****FUNC*****\n");
        Printf(b_cfy_grp_cpp->b0,"extern \"C\" {\n");
        Printf(b_cfy_grp_cpp->b0,"COUNTIFY_API\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtfn");
        Printf(b_cfy_grp_cpp->b0,"%s(\n", p.funcName);
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_parm");
        Printf(b_cfy_grp_cpp->b0,") {\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"    try {\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"Begin function\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        initializeAddin();\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        // Convert input types into Library types\n\n");
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_cnvt", 2, 0, false);
        Printf(b_cfy_grp_cpp->b0,"\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtdf", 2, false);
        Printf(b_cfy_grp_cpp->b0,"        %s::%s(\n", module, p.symname);
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_args", 2, ',', true, true);
        Printf(b_cfy_grp_cpp->b0,"        );\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"End function\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtsf", 2, false);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"    } catch (const std::exception &e) {\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"ERROR - \" << e.what());\n", p.funcName);
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtex", 2, false);
        Printf(b_cfy_grp_cpp->b0,"    } catch (...) {\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"ERROR - UNKNOWN EXCEPTION\");\n", p.funcName);
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtex", 2, false);
        Printf(b_cfy_grp_cpp->b0,"    }\n");
        Printf(b_cfy_grp_cpp->b0,"}\n");
        Printf(b_cfy_grp_cpp->b0,"} // extern \"C\"\n");

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        Printf(b_cfy_grp_cpp->b0,"//****CTOR*****\n");
        Printf(b_cfy_grp_cpp->b0,"extern \"C\" {\n");
        Printf(b_cfy_grp_cpp->b0,"COUNTIFY_API\n");
        Printf(b_cfy_grp_cpp->b0,"const char *%s(\n", p.funcName);
        emitParmList(p.parms2, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_parm", 2);
        Printf(b_cfy_grp_cpp->b0,"    ) {\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"    try {\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"Begin function\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        initializeAddin();\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        // Convert input types into Library types\n\n");
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_cnvt", 2, 0, false);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        boost::shared_ptr<reposit::ValueObject> valueObject(\n");
        Printf(b_cfy_grp_cpp->b0,"            new %s::ValueObjects::%s(\n", module, p.funcName);
        Printf(b_cfy_grp_cpp->b0,"                objectID,\n");
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 0, "rp_tm_default", 4, ',', true, false, true);
        Printf(b_cfy_grp_cpp->b0,"                false));\n");
        Printf(b_cfy_grp_cpp->b0,"        boost::shared_ptr<reposit::Object> object(\n");
        Printf(b_cfy_grp_cpp->b0,"            new %s::%s(\n", module, p.name);
        Printf(b_cfy_grp_cpp->b0,"                valueObject,\n");
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_args", 4, ',', true, true, true);
        Printf(b_cfy_grp_cpp->b0,"                false));\n");
        Printf(b_cfy_grp_cpp->b0,"        static std::string returnValue;\n");
        Printf(b_cfy_grp_cpp->b0,"        returnValue =\n");
        Printf(b_cfy_grp_cpp->b0,"            reposit::Repository::instance().storeObject(\n");
        Printf(b_cfy_grp_cpp->b0,"                objectID, object, true, valueObject);\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"End function\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        return returnValue.c_str();\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"    } catch (const std::exception &e) {\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"ERROR - \" << e.what());\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"        static std::string errorMessage;\n");
        Printf(b_cfy_grp_cpp->b0,"        errorMessage = e.what();\n");
        Printf(b_cfy_grp_cpp->b0,"        return errorMessage.c_str();\n");
        Printf(b_cfy_grp_cpp->b0,"    } catch (...) {\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"ERROR - UNKNOWN EXCEPTION\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"        static std::string errorMessage = \"UNKNOWN EXCEPTION\";\n");
        Printf(b_cfy_grp_cpp->b0,"        return errorMessage.c_str();\n");
        Printf(b_cfy_grp_cpp->b0,"    }\n");
        Printf(b_cfy_grp_cpp->b0,"}\n\n");
        Printf(b_cfy_grp_cpp->b0,"} // extern \"C\"\n");

        count_.constructors++;
        count_.total2++;
    }

    void functionWrapperImplMemb(ParmsMemb &p) {

        Printf(b_cfy_grp_cpp->b0,"//****MEMB*****\n");
        Printf(b_cfy_grp_cpp->b0,"extern \"C\" {\n");
        Printf(b_cfy_grp_cpp->b0,"COUNTIFY_API\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtmb");
        Printf(b_cfy_grp_cpp->b0,"%s(\n", p.funcName);
        emitParmList(p.parms2, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_parm", 2);
        Printf(b_cfy_grp_cpp->b0,"    ) {\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"    try {\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"Begin function\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        initializeAddin();\n");
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        // Convert input types into Library types\n\n");
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cfy_cnvt", 2, 0, false);
        Printf(b_cfy_grp_cpp->b0,"\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.node, "rp_tm_xxx_rp_get", 2);
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtdm", 2);
        Printf(b_cfy_grp_cpp->b0,"        xxx->%s(\n", p.name);
        emitParmList(p.parms, b_cfy_grp_cpp->b0, 1, "rp_tm_cpp_args", 3, ',', true, true);
        Printf(b_cfy_grp_cpp->b0,"        );\n", p.name);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"End function\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtsm", 2);
        Printf(b_cfy_grp_cpp->b0,"\n");
        Printf(b_cfy_grp_cpp->b0,"    } catch (const std::exception &e) {\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"ERROR - \" << e.what());\n", p.funcName);
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtex", 2, false);
        Printf(b_cfy_grp_cpp->b0,"    } catch (...) {\n");
        Printf(b_cfy_grp_cpp->b0,"        RP_LOG_MESSAGE(\"%s\", \"ERROR - UNKNOWN EXCEPTION\");\n", p.funcName);
        Printf(b_cfy_grp_cpp->b0,"        static std::string errorMessage = \"UNKNOWN EXCEPTION\";\n");
        emitTypeMap(b_cfy_grp_cpp->b0, p.n, "rp_tm_cfy_rtex", 2, false);
        Printf(b_cfy_grp_cpp->b0,"    }\n");
        Printf(b_cfy_grp_cpp->b0,"}\n");
        Printf(b_cfy_grp_cpp->b0,"} // extern \"C\"\n");

        count_.members++;
        count_.total2++;
    }

    void clear() {
        b_cfy_grp_cpp->clear(count_);
    }
};

//*****************************************************************************
// ADDINS
//*****************************************************************************

struct Addin {

    std::string name_;
    Pragmas pragmas_;
    Count count;

    Addin(const std::string &name) : name_(name) {}
    virtual ~Addin() {}
    void setPragmas(const Pragmas &pragmas = Pragmas()) {
        pragmas_ = pragmas;
    }
    virtual void top() {}
    virtual void functionWrapperImplFunc(ParmsFunc &p) = 0;
    virtual void functionWrapperImplCtor(ParmsCtor &p) = 0;
    virtual void functionWrapperImplMemb(ParmsMemb &p) = 0;
    virtual void clear() = 0;
};

template <class Group>
struct AddinImpl : public Addin {

    typedef std::map<std::string, Group*> GroupMap;
    typedef typename GroupMap::const_iterator GroupMapIter;
    GroupMap groupMap_;

    AddinImpl(const std::string &name) : Addin(name) {}

    virtual Group *getGroup() {
        std::string name_ = Char(pragmas_.groupName_);
        if (groupMap_.end() == groupMap_.find(name_)) {
            groupMap_[name_] = new Group(pragmas_, count);
        }
        return groupMap_[name_];
    }

    virtual void functionWrapperImplFunc(ParmsFunc &p) {
        Group *group = getGroup();
        group->functionWrapperImplFunc(p);
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        Group *group = getGroup();
        group->functionWrapperImplCtor(p);
    }

    virtual void functionWrapperImplMemb(ParmsMemb &p) {
        Group *group = getGroup();
        group->functionWrapperImplMemb(p);
    }

    virtual void clear() {
        printf("%s - Group Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        for (GroupMapIter i=groupMap_.begin(); i!=groupMap_.end(); ++i)
            i->second->clear();
    }
};

struct AddinLibraryObjects : public AddinImpl<GroupLibraryObjects> {

    Buffer *b_lib_add_hpp;

    AddinLibraryObjects() : AddinImpl("Library Objects") {}

    virtual void top() {

        b_lib_add_hpp = new Buffer("b_lib_add_hpp", NewStringf("%s/obj_all.hpp", objDir));

        Printf(b_lib_add_hpp->b0, "\n");
        Printf(b_lib_add_hpp->b0, "#ifndef obj_all_hpp\n");
        Printf(b_lib_add_hpp->b0, "#define obj_all_hpp\n");
        Printf(b_lib_add_hpp->b0, "\n");
    }

    virtual void clear() {

        AddinImpl::clear();

        for (GroupMapIter i=groupMap_.begin(); i!=groupMap_.end(); ++i) {
            GroupLibraryObjects *group = i->second;
            if (group->pragmas_.automatic_) {
                Printf(b_lib_add_hpp->b0, "#include <%s/obj_%s.hpp>\n", objInc, group->pragmas_.groupName_);
            } else {
                Printf(b_lib_add_hpp->b0, "#include <%s/objmanual_%s.hpp>\n", objInc, group->pragmas_.groupName_);
            }
        }

        Printf(b_lib_add_hpp->b0, "\n");
        Printf(b_lib_add_hpp->b0, "#endif\n");
        Printf(b_lib_add_hpp->b0, "\n");

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_lib_add_hpp->clear(count);
    }
};

struct AddinValueObjects : public AddinImpl<GroupValueObjects> {

    AddinValueObjects() : AddinImpl("Value Objects") {}

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            AddinImpl::functionWrapperImplCtor(p);
        }
    }
};

struct AddinSerializationCreate : public AddinImpl<GroupSerializationCreate> {

    Buffer *b_scr_add_hpp;

    AddinSerializationCreate() : AddinImpl("Serialization - Create") {}

    virtual void top() {
        b_scr_add_hpp = new Buffer("b_scr_add_hpp", NewStringf("%s/serialization/create/create_all.hpp", objDir));

        Printf(b_scr_add_hpp->b0, "\n");
        Printf(b_scr_add_hpp->b0, "#ifndef create_all_hpp\n");
        Printf(b_scr_add_hpp->b0, "#define create_all_hpp\n");
        Printf(b_scr_add_hpp->b0, "\n");
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            AddinImpl::functionWrapperImplCtor(p);
        }
    }

    virtual void clear() {

        AddinImpl::clear();

        for (GroupMapIter i=groupMap_.begin(); i!=groupMap_.end(); ++i) {
            GroupSerializationCreate *group = i->second;
            if (group->generateOutput)
                Printf(b_scr_add_hpp->b0, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, group->pragmas_.groupName_);
        }

        Printf(b_scr_add_hpp->b0, "\n");
        Printf(b_scr_add_hpp->b0, "#endif\n");
        Printf(b_scr_add_hpp->b0, "\n");

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_scr_add_hpp->clear(count);
    }
};

struct AddinSerializationRegister : public AddinImpl<GroupSerializationRegister> {

    Buffer *b_srg_add_cpp;
    Buffer *b_srg_add_hpp;
    Buffer *b_sra_add_hpp;

    AddinSerializationRegister() : AddinImpl("Serialization - Register") {}

    virtual void top() {
        b_srg_add_cpp = new Buffer("b_srg_add_cpp", NewStringf("%s/serialization/register_creators.cpp", objDir));
        b_srg_add_hpp = new Buffer("b_srg_add_hpp", NewStringf("%s/serialization/register/serialization_register.hpp", objDir));
        b_sra_add_hpp = new Buffer("b_sra_add_hpp", NewStringf("%s/serialization/register/serialization_all.hpp", objDir));

        Printf(b_srg_add_cpp->b0, "\n");
        Printf(b_srg_add_cpp->b0, "#include <%s/serialization/serializationfactory.hpp>\n", objInc);
        Printf(b_srg_add_cpp->b0, "#include <%s/serialization/create/create_all.hpp>\n", objInc);
        Printf(b_srg_add_cpp->b0, "\n");
        Printf(b_srg_add_cpp->b0, "void %s::SerializationFactory::registerCreators() {\n", module);
        Printf(b_srg_add_cpp->b0, "\n");

        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "#ifndef serialization_register_hpp\n");
        Printf(b_srg_add_hpp->b0, "#define serialization_register_hpp\n");
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "#include <%s/serialization/register/serialization_all.hpp>\n", objInc);
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "namespace %s {\n", module);
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "    template<class Archive>\n");
        Printf(b_srg_add_hpp->b0, "    void tpl_register_classes(Archive& ar) {\n");
        Printf(b_srg_add_hpp->b0, "\n");

        Printf(b_sra_add_hpp->b0, "\n");
        Printf(b_sra_add_hpp->b0, "#ifndef serialization_all_hpp\n");
        Printf(b_sra_add_hpp->b0, "#define serialization_all_hpp\n");
        Printf(b_sra_add_hpp->b0, "\n");
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            AddinImpl::functionWrapperImplCtor(p);
            Printf(b_srg_add_cpp->b0, "    registerCreator(\"%s\", create_%s);\n", p.funcRename, p.funcRename);
        }
    }

    virtual void clear() {

        AddinImpl::clear();

        for (GroupMapIter i=groupMap_.begin(); i!=groupMap_.end(); ++i) {
            GroupSerializationRegister *group = i->second;
            if (group->generateOutput) {
                Printf(b_sra_add_hpp->b0, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, group->pragmas_.groupName_);
                Printf(b_srg_add_hpp->b0, "        register_%s(ar);\n", group->pragmas_.groupName_);
            }
        }

        Printf(b_srg_add_cpp->b0, "\n");
        Printf(b_srg_add_cpp->b0, "}\n");
        Printf(b_srg_add_cpp->b0, "\n");

        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "    }\n");
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "}\n");
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "#endif\n");
        Printf(b_srg_add_hpp->b0, "\n");

        Printf(b_sra_add_hpp->b0, "\n");
        Printf(b_sra_add_hpp->b0, "#endif\n");
        Printf(b_sra_add_hpp->b0, "\n");

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_srg_add_cpp->clear(count);
        b_srg_add_hpp->clear(count);
        b_sra_add_hpp->clear(count);
    }
};

struct AddinCpp : public AddinImpl<GroupCpp> {

    Buffer *b_cpp_add_all_hpp;

    AddinCpp() : AddinImpl("C++ Addin") {}

    virtual void top() {
        b_cpp_add_all_hpp = new Buffer("b_cpp_add_all_hpp", NewStringf("%s/add_all.hpp", addDir));

        Printf(b_cpp_add_all_hpp->b0, "\n");
        Printf(b_cpp_add_all_hpp->b0, "#ifndef add_all_hpp\n");
        Printf(b_cpp_add_all_hpp->b0, "#define add_all_hpp\n");
        Printf(b_cpp_add_all_hpp->b0, "\n");
        Printf(b_cpp_add_all_hpp->b0, "#include <%s/init.hpp>\n", addInc);
    }

    virtual void functionWrapperImplFunc(ParmsFunc &p) {
        if (checkAttribute(p.n, "feature:rp:generate:c++", "1")) {
            AddinImpl::functionWrapperImplFunc(p);
        }
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (checkAttribute(p.n, "feature:rp:generate:c++", "1")) {
            AddinImpl::functionWrapperImplCtor(p);
        }
    }

    virtual void functionWrapperImplMemb(ParmsMemb &p) {
        if (checkAttribute(p.n, "feature:rp:generate:c++", "1")) {
            AddinImpl::functionWrapperImplMemb(p);
        }
    }

    virtual void clear() {

        AddinImpl::clear();

        for (GroupMapIter i=groupMap_.begin(); i!=groupMap_.end(); ++i) {
            GroupCpp *group = i->second;
            Printf(b_cpp_add_all_hpp->b0, "#include <%s/add_%s.hpp>\n", addInc, group->pragmas_.groupName_);
        }

        Printf(b_cpp_add_all_hpp->b0, "\n");
        Printf(b_cpp_add_all_hpp->b0, "#endif\n");
        Printf(b_cpp_add_all_hpp->b0, "\n");

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_cpp_add_all_hpp->clear(count);
    }
};

struct AddinCSharp : public AddinImpl<GroupCSharp> {

    Buffer *b_csh_exp_all_cs;

    AddinCSharp() : AddinImpl("C# Addin") {}

    virtual void top() {
        b_csh_exp_all_cs = new Buffer("b_csh_exp_all_cs", NewStringf("%s/Export.cs", cshDir));

        Printf(b_csh_exp_all_cs->b0, "\n");
        Printf(b_csh_exp_all_cs->b0, "using System;\n");
        Printf(b_csh_exp_all_cs->b0, "using System.Collections.Generic;\n");
        Printf(b_csh_exp_all_cs->b0, "using System.Linq;\n");
        Printf(b_csh_exp_all_cs->b0, "using System.Text;\n");
        Printf(b_csh_exp_all_cs->b0, "using System.Runtime.InteropServices;\n");
        Printf(b_csh_exp_all_cs->b0, "\n");
        Printf(b_csh_exp_all_cs->b0, "namespace QuantLibAddin\n");
        Printf(b_csh_exp_all_cs->b0, "{\n");
        Printf(b_csh_exp_all_cs->b0, "    class Export\n");
        Printf(b_csh_exp_all_cs->b0, "    {\n");
        Printf(b_csh_exp_all_cs->b0, "        // qlInitializeAddin\n");
        Printf(b_csh_exp_all_cs->b0, "        [DllImport(\"..\\\\..\\\\..\\\\..\\\\..\\\\bin\\\\AddinCSharp-vc90-mt-s-1_7_0.dll\", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]\n");
        Printf(b_csh_exp_all_cs->b0, "        public static extern void qlInitializeAddin();\n");
    }

    virtual void functionWrapperImplFunc(ParmsFunc &p) {
        if (checkAttribute(p.n, "feature:rp:generate:c#", "1")) {
            AddinImpl::functionWrapperImplFunc(p);
            Printf(b_csh_exp_all_cs->b0, "\n");
            Printf(b_csh_exp_all_cs->b0, "        // %s\n", p.funcName);
            Printf(b_csh_exp_all_cs->b0, "        [DllImport(\"..\\\\..\\\\..\\\\..\\\\..\\\\bin\\\\AddinCSharp-vc90-mt-s-1_7_0.dll\", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]\n");
            emitTypeMap(b_csh_exp_all_cs->b0, p.n, "rp_tm_csh_rtcp", 2);
            Printf(b_csh_exp_all_cs->b0, "        public static extern\n");
            emitTypeMap(b_csh_exp_all_cs->b0, p.n, "rp_tm_csh_rscp", 2);
            Printf(b_csh_exp_all_cs->b0, "        %s(\n", p.funcName);
            emitParmList(p.parms, b_csh_exp_all_cs->b0, 2, "rp_tm_csh_clcp", 3);
            Printf(b_csh_exp_all_cs->b0, "        );\n");
        }
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (checkAttribute(p.n, "feature:rp:generate:c#", "1")) {
            AddinImpl::functionWrapperImplCtor(p);
            Printf(b_csh_exp_all_cs->b0, "\n");
            Printf(b_csh_exp_all_cs->b0, "        // %s\n", p.funcName);
            Printf(b_csh_exp_all_cs->b0, "        [DllImport(\"..\\\\..\\\\..\\\\..\\\\..\\\\bin\\\\AddinCSharp-vc90-mt-s-1_7_0.dll\", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]\n");
            Printf(b_csh_exp_all_cs->b0, "        [return: MarshalAs(UnmanagedType.LPStr)]\n");
            Printf(b_csh_exp_all_cs->b0, "        public static extern string %s(\n", p.funcName);
            emitParmList(p.parms2, b_csh_exp_all_cs->b0, 2, "rp_tm_csh_clcp", 3);
            Printf(b_csh_exp_all_cs->b0, "        );\n");
        }
    }

    virtual void functionWrapperImplMemb(ParmsMemb &p) {
        if (checkAttribute(p.n, "feature:rp:generate:c#", "1")) {
            AddinImpl::functionWrapperImplMemb(p);
            Printf(b_csh_exp_all_cs->b0, "\n");
            Printf(b_csh_exp_all_cs->b0, "        // %s\n", p.funcName);
            Printf(b_csh_exp_all_cs->b0, "        [DllImport(\"..\\\\..\\\\..\\\\..\\\\..\\\\bin\\\\AddinCSharp-vc90-mt-s-1_7_0.dll\", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]\n");
            emitTypeMap(b_csh_exp_all_cs->b0, p.n, "rp_tm_csh_rtcp", 2);
            Printf(b_csh_exp_all_cs->b0, "        public static extern\n");
            emitTypeMap(b_csh_exp_all_cs->b0, p.n, "rp_tm_csh_rscp", 2);
            Printf(b_csh_exp_all_cs->b0, "        %s(\n", p.funcName);
            emitParmList(p.parms2, b_csh_exp_all_cs->b0, 2, "rp_tm_csh_clcp", 3);
            Printf(b_csh_exp_all_cs->b0, "        );\n");
        }
    }

    virtual void clear() {

        AddinImpl::clear();

        Printf(b_csh_exp_all_cs->b0, "    }\n");
        Printf(b_csh_exp_all_cs->b0, "}\n");
        Printf(b_csh_exp_all_cs->b0, "\n");

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_csh_exp_all_cs->clear(count);
    }
};

struct AddinExcelFunctions : public AddinImpl<GroupExcelFunctions> {

    AddinExcelFunctions() : AddinImpl("Excel Addin - Functions") {}

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        AddinImpl::functionWrapperImplCtor(p);
    }
};

struct AddinExcelRegister : public AddinImpl<GroupExcelRegister> {

    Buffer *b_xlr_add_all_cpp;

    AddinExcelRegister() : AddinImpl("Excel Addin - Register") {}

    virtual void top() {
        b_xlr_add_all_cpp = new Buffer("b_xlr_add_all_cpp", NewStringf("%s/register/register_all.cpp", xllDir));

        Printf(b_xlr_add_all_cpp->b0, "\n");
        Printf(b_xlr_add_all_cpp->b0, "#include <%s/register/register_all.hpp>\n", xllInc);
        Printf(b_xlr_add_all_cpp->b0, "\n");

        Printf(b_xlr_add_all_cpp->b2, "\n");
        Printf(b_xlr_add_all_cpp->b2, "void registerFunctions(const XLOPER& xDll) {\n");
        Printf(b_xlr_add_all_cpp->b2, "\n");

        Printf(b_xlr_add_all_cpp->b3, "\n");
        Printf(b_xlr_add_all_cpp->b3, "void unregisterFunctions(const XLOPER& xDll) {\n");
        Printf(b_xlr_add_all_cpp->b3, "\n");
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            AddinImpl::functionWrapperImplCtor(p);
        }
    }

    virtual void clear() {

        AddinImpl::clear();

        for (GroupMapIter i=groupMap_.begin(); i!=groupMap_.end(); ++i) {
            GroupExcelRegister *group = i->second;
            Printf(b_xlr_add_all_cpp->b0, "extern void register_%s(const XLOPER&);\n", group->pragmas_.groupName_);
            Printf(b_xlr_add_all_cpp->b1, "extern void unregister_%s(const XLOPER&);\n", group->pragmas_.groupName_);
            Printf(b_xlr_add_all_cpp->b2, "    register_%s(xDll);\n", group->pragmas_.groupName_);
            Printf(b_xlr_add_all_cpp->b3, "    unregister_%s(xDll);\n", group->pragmas_.groupName_);
        }

        Printf(b_xlr_add_all_cpp->b2, "\n");
        Printf(b_xlr_add_all_cpp->b2, "}\n");
        Printf(b_xlr_add_all_cpp->b2, "\n");
        Printf(b_xlr_add_all_cpp->b3, "\n");
        Printf(b_xlr_add_all_cpp->b3, "}\n");
        Printf(b_xlr_add_all_cpp->b3, "\n");

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_xlr_add_all_cpp->clear(count);
    }
};

struct AddinCountify : public AddinImpl<GroupCountify> {

    Buffer *b_cfy_add_mng_txt;

    AddinCountify() : AddinImpl("Countify Addin") {}

    virtual void top() {
        b_cfy_add_mng_txt = new Buffer("b_cfy_add_mng_txt", NewStringf("%s/cfy_mongo.txt", cfyDir));
    }

    virtual void functionWrapperImplFunc(ParmsFunc &p) {
        if (checkAttribute(p.n, "feature:rp:generate:countify", "1")) {
            AddinImpl::functionWrapperImplFunc(p);
            mongoFunc(b_cfy_add_mng_txt->b0, p.symnameUpper, p.funcName, p.n, p.parms);
        }
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (checkAttribute(p.n, "feature:rp:generate:countify", "1") && generateCtor) {
            AddinImpl::functionWrapperImplCtor(p);
            mongoFunc(b_cfy_add_mng_txt->b0, p.name, p.funcName, p.n, p.parms2);
        }
    }

    virtual void functionWrapperImplMemb(ParmsMemb &p) {
        if (checkAttribute(p.n, "feature:rp:generate:countify", "1")) {
            AddinImpl::functionWrapperImplMemb(p);
            mongoFunc(b_cfy_add_mng_txt->b0, p.nameUpper, p.funcName, p.n, p.parms2);
        }
    }

    virtual void clear() {

        AddinImpl::clear();

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
        b_cfy_add_mng_txt->clear(count);
    }
};

struct AddinList {

    std::vector<Addin*> addinList_;
    typedef std::vector<Addin*>::const_iterator iter;

    void appendAddin(Addin *addin) {
        addinList_.push_back(addin);
    }

    void top() {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->top();
        }
    }

    void setPragmas(const Pragmas &pragmas = Pragmas()) {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->setPragmas(pragmas);
        }
    }

    void functionWrapperImplFunc(ParmsFunc &p) {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->functionWrapperImplFunc(p);
        }
    }

    void functionWrapperImplCtor(ParmsCtor &p) {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->functionWrapperImplCtor(p);
        }
    }

    void functionWrapperImplMemb(ParmsMemb &p) {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->functionWrapperImplMemb(p);
        }
    }

    void clear() {
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            addin->clear();
        }
        printf("%s\n", std::string(80, '=').c_str());
        printf("Function Count                      Function Constructor      Member       Total\n");
        printf("%s\n", std::string(80, '=').c_str());
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            printf("%s", addin->name_.c_str());
            printf("%s", std::string(32-addin->name_.length(), ' ').c_str());
            printf("%12d%12d%12d%12d\n",
                addin->count.functions, addin->count.constructors, addin->count.members, addin->count.total2);
        }
        printf("%s\n", std::string(80, '=').c_str());
        printf("File Count                           Created     Updated   Unchanged       Total\n");
        printf("%s\n", std::string(80, '=').c_str());
        Count c;
        for (iter i=addinList_.begin(); i!=addinList_.end(); ++i) {
            Addin *addin = *i;
            printf("%s", addin->name_.c_str());
            printf("%s", std::string(32-addin->name_.length(), ' ').c_str());
            printf("%12d%12d%12d%12d\n",
                addin->count.created, addin->count.updated, addin->count.unchanged, addin->count.total);
            c.add(addin->count);
        }
        printf("%s\n", std::string(80, '=').c_str());
        printf("Total");
        printf("%s", std::string(27, ' ').c_str());
        printf("%12d%12d%12d%12d\n",
            c.created, c.updated, c.unchanged, c.total);
        printf("%s\n", std::string(80, '=').c_str());
    }
};

//*****************************************************************************
// LANGUAGE
//*****************************************************************************

class REPOSIT : public Language {

    int functionType;//0=function, 1=constructor, 2=member
    Pragmas pragmas_;
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

   addinList_.appendAddin(new AddinLibraryObjects);
   addinList_.appendAddin(new AddinValueObjects);
   addinList_.appendAddin(new AddinSerializationCreate);
   addinList_.appendAddin(new AddinSerializationRegister);

    for (int i = 1; i < argc; i++) {
        if ( (strcmp(argv[i],"-genc++") == 0)) {
            addinList_.appendAddin(new AddinCpp);
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-genc#") == 0)) {
            addinList_.appendAddin(new AddinCSharp);
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-genxll") == 0)) {
            addinList_.appendAddin(new AddinExcelFunctions);
            addinList_.appendAddin(new AddinExcelRegister);
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-gencfy") == 0)) {
            addinList_.appendAddin(new AddinCountify);
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
        if (String *s = getNode(n3, "rp_add_dir"))
            addDir = s;
        if (String *s = getNode(n3, "rp_csh_dir"))
            cshDir = s;
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
    }

    printf("module=%s\n", Char(module));
    printf("addinCppNameSpace=%s\n", Char(addinCppNameSpace));
    printf("rp_obj_dir=%s\n", Char(objDir));
    printf("rp_add_dir=%s\n", Char(addDir));
    printf("rp_csh_dir=%s\n", Char(cshDir));
    printf("rp_xll_dir=%s\n", Char(xllDir));
    printf("rp_cfy_dir=%s\n", Char(cfyDir));
    printf("rp_obj_inc=%s\n", Char(objInc));
    printf("rp_add_inc=%s\n", Char(addInc));
    printf("rp_xll_inc=%s\n", Char(xllInc));

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

    addinList_.top();

   /* Output module initialization code */
   Swig_banner(b_begin);

   /* Emit code for children */
   Language::top(n);

    addinList_.clear();

    // To help with troubleshooting, create an output file to which all of the
    // SWIG buffers will be written.  We are not going to compile this file but
    // we give it a cpp extension so that the editor will apply syntax highlighting.
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
    //Printf(f_test, "//**********begin b_director\n");
    //Dump(b_director, f_test);
    //Printf(f_test, "//**********end b_director\n");
    //Printf(f_test, "//**********begin b_director_h\n");
    //Dump(b_director_h, f_test);
    //Printf(f_test, "//**********end b_director_h\n");

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

    for (int i=0; i<Len(errorList); ++i) {
        String *errorMessage = Getitem(errorList, i);
        printf("%s", Char(errorMessage));
    }
    Delete(errorList);//FIXME also delete each item individually

   return SWIG_OK;
}

// overrride base class members, write debug info to b_init,
// and possibly pass control to a handler.

int moduleDirective(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN moduleDirective - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    printf("moduleDirective\n");
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
    printf("classDeclaration\n");
    int ret=Language::classDeclaration(n);
    Printf(b_init, "END   classDeclaration - node name='%s'.\n", Char(nodename));
    return ret;
}

int constructorDeclaration(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN constructorDeclaration - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    printf("constructorDeclaration\n");
    int ret=Language::constructorDeclaration(n);
    Printf(b_init, "END   constructorDeclaration - node name='%s'.\n", Char(nodename));
    return ret;
}

int namespaceDeclaration(Node *n) {
    nmspace = Getattr(n, "name");
    Printf(b_init, "BEGIN namespaceDeclaration - node name='%s'.\n", Char(nmspace));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    printf("namespaceDeclaration\n");
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
    printf("functionHandler\n");
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
    printf("memberfunctionHandler\n");
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
    printf("constructorHandler\n");
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
    printf("functionWrapper\n");
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
    printf("classHandler\n");
    int ret=Language::classHandler(n);
    Printf(b_init, "END   classHandler - node name='%s'.\n", Char(nodename));
    return ret;
}

int includeDirective(Node *n) {
    String *nodename = Getattr(n, "name");
    Printf(b_init, "BEGIN includeDirective - node name='%s'.\n", Char(nodename));
    printNode(n, b_init);
    Printf(b_init, "call parent\n");
    printf("includeDirective\n");

    pragmas_ = Pragmas();
    addinList_.setPragmas();

    int ret=Language::includeDirective(n);
    Printf(b_init, "END   includeDirective - node name='%s'.\n", Char(nodename));
    return ret;
}

int pragmaDirective(Node *n) {
    if (!ImportMode) {
        String *lang = Getattr(n, "lang");

        if (0 == Strcmp(lang, "reposit")) {
            String *name = Getattr(n, "name");
            String *value = Getattr(n, "value");
            printf ("ABCDEF name=%s value=%s.\n", Char(name), Char(value));

            if (0 == Strcmp(name, "group")) {
                pragmas_.setGroupName(NewString(value));
            } else if (0 == Strcmp(name, "override_obj")) {
                // For the user writing the config file, it is easier to assume automatic (default)
                // unless overridden with '%feature("rp:override_obj");' :
                bool manual = 0 == Strcmp(value, "true");
                // The source code for this SWIG module is cleaner if we think of it the opposite way:
                pragmas_.automatic_ = !manual;
            } else {
                Swig_error(input_file, line_number, "Unrecognized pragma.\n");
            }
        }
    }
    return Language::pragmaDirective(n);
}

int functionWrapperImpl(Node *n) {

    // If no group name was set at the top of the current interface file,
    // then do not generate any output for this file.
    if (!pragmas_.groupName_)
        return SWIG_OK;

    addinList_.setPragmas(pragmas_);

    // Generate output appropriate to the given function type
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

// Validate the function name.
// For the moment, the only test that we perform is to check whether the function name
// clashes with Excel range names.  It is possible that a function name could pass this
// test but still be invalid for other reasons.
// Excel cell names lie in the range A1 - XFD1048576.
// For performance reason this function tests the range A1 - ZZZ9999999.
// So this function could return a false positive, i.e. fail even though the name is OK.
void validateFunctionName(const String *functionName) {
    unsigned int len = Len(functionName);
    if (len > 10)
        return;
    const char *c = Char(functionName);
    unsigned int a=0; // the number of characters at the start of the string
    unsigned int n=0; // the number of digits at the end of the string
    // Step through 0, 1, 2, or 3 letters at the start of the string.
    for (; a<3 && a<len && isalpha(c[a]); a++)
        ;
    if (a==0)
        // If control arrives here it means that the first character of the function name is not a letter.
        // This name does not clash with Excel, so return success, but the name is probably invalid.
        return;
    if (a==len)
        // If control arrives here it means that the entire string comprises 1-3 letters.
        // This name does not clash with Excel, so return success, but the name is probably invalid.
        return;
    // Step through any numbers up to the end of the string.
    for (; n<len-a && isdigit(c[n+a]); n++)
        ;
    if (len > a+n)
        // If control arrives here it means that the string contains additional characters after
        // any letters and numbers.  This name does not clash with Excel, so return success.
        return;
    if (n>7)
        // If control arrives here it means that the string ends in more than 7 digits.
        // This name does not clash with Excel, so return success.
        return;
    // If control arrives here it means that the string comprises 1, 2, or 3 letters, followed by
    // numbers.  At this point the only way the string could not clash with Excel is if it
    // comprises exactly 3 letters and 7 numbers and is greater than the Excel max (XFD1048576).
    // So test for that case:
    if (3==a && 7==n) {
        std::string s(c);
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        if (s>"XFD1048576")
            return;
    }
    // OK, this function name is also the name of a cell in Excel.  Fail.
    REPOSIT_SWIG_FAIL(
        "Error : Invalid function name: '" << c << "'.\n" <<
        "This string is in the range of Excel cell names (A1...XFD1048576)\n" <<
        "Rename this function with %%rename() in the SWIG interface file.");
}

int functionWrapperImplFunc(Node *n) {

    ParmsFunc p;

    p.n = n;
    p.name   = Getattr(n,"name");
    p.parms  = Getattr(n,"parms");
    p.symname   = Getattr(n,"sym:name");
    bool staticMember = 1 == checkAttribute(p.n, "globalfunctionHandler:view", "staticmemberfunctionHandler");
    if (staticMember)
        Replaceall(p.symname, "_", "");
    p.symnameUpper = copyUpper(p.symname);

    p.funcName = NewStringf("%s%s", prefix, p.symnameUpper);
    validateFunctionName(p.funcName);

    Setattr(n, "rp:funcName", p.funcName);
    Setattr(n, "rp:funcRename", p.funcName);
    printf("p.name=%s\n", Char(p.name));
    printf("p.symname=%s\n", Char(p.symname));
    printf("p.symnameUpper=%s\n", Char(p.symnameUpper));
    printf("p.funcName=%s\n", Char(p.funcName));
    printf("staticMember=%d\n", staticMember);
    Printf(b_init, "@@@ FUNC Name=%s\n", p.funcName);
    Printf(b_init, "&&& p.symname=%s\n", p.symname);
    Printf(b_init, "&&& prefix=%s\n", prefix);
    Printf(b_init, "&&& p.funcName=%s\n", p.funcName);

    // Create from parms another list parms2 - prepend an argument to represent
    // the dependency trigger which is the first argument of every addin function.
    p.parms2 = prependParm(p.parms, "Trigger", "reposit::property_t");

    addinList_.functionWrapperImplFunc(p);

    return SWIG_OK;
}

int constructorHandlerImpl(Node *n) {

    // If no ctor was defined in the *.i file then SWIG sets the following flag:
    bool defaultCtor = 0 != Getattr(n, "default_constructor");
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
    REPOSIT_SWIG_REQUIRE(name, "parameter has no name");

    SwigType *t = Getattr(p, "type");
    REPOSIT_SWIG_REQUIRE(t, "parameter '" << Char(name) << "' has no type");

    String *nameUpper = copyUpper2(name);
    Setattr(p, "rp_name_upper", nameUpper);

    SwigType *t2 = SwigType_typedef_resolve_all(t);
    Setattr(p, "rp_typedef_resolved", SwigType_str(t2, 0));

    // From "const T&" extract "T"
    SwigType *t3 = SwigType_base(t);
    Setattr(p, "rp_typedef_base", SwigType_str(t3, 0));

    // From A<B> extract B.  We do not yet support nested templates.
    char no_template[100];
    if (SwigType_istemplate(t3)) {
        String *s1 = SwigType_str(t3, 0);
        char *c1 = Char(s1);
        char *c2 = strchr(c1, '<');
        char *c3 = strchr(c1, '>');
        strncpy(no_template, c2+1, c3-c2-1);
        no_template[c3-c2-1]=0;
    } else {
        strcpy(no_template, Char(SwigType_str(t3, 0)));
    }
    String *s2 = NewString(no_template);
    Setattr(p, "rp_typedef_no_template", s2);

    // From namespace::type extract type.  We do not yet support nested namespaces.
    char no_namespace[100];
    char *c1 = strchr(no_template, ':');
    if (c1) {
        strcpy(no_namespace, c1+2);
    } else {
        strcpy(no_namespace, no_template);
    }
    String *s3 = NewString(no_namespace);
    Setattr(p, "rp_typedef_no_namespace", s3);
}

int functionWrapperImplCtor(Node *n) {

    ParmsCtor p;

    p.n = n;
    p.name = Getattr(n, "name");
    p.rename = Getattr(n, "constructorDeclaration:sym:name");
    p.parms = Getattr(n, "parms");
    Node *n1 = Getattr(n, "parentNode");
    p.pname = Getattr(n1, "name");

    p.base = 0;
    if (List *baseList = Getattr(n1, "baselist")) {
        REPOSIT_SWIG_REQUIRE(1==Len(baseList), "Class '" << Char(p.name) << "' has multiple base classes.");
        p.base = Getitem(baseList, 0);
        printf("base = %s\n", Char(p.base));
    } else {
        printf("no bases\n");
    }

    String *temp = copyUpper(p.name);
    p.funcName = NewStringf("%s%s", prefix, temp);
    String *tempx = copyUpper(p.rename);
    p.funcRename = NewStringf("%s%s", prefix, tempx);
    Setattr(n, "rp:funcName", p.funcName);
    Setattr(n, "rp:funcRename", p.funcRename);
    validateFunctionName(p.funcRename);
    printf("funcName=%s\n", Char(p.funcName));
    Printf(b_init, "@@@ CTOR Name=%s\n", Char(p.funcName));

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every ctor.
    Parm *temp1 = prependParm(p.parms, "Permanent", "bool", false);
    Parm *temp2 = prependParm(temp1, "Overwrite", "bool", false);
    Parm *temp3 = prependParm(temp2, "objectID", "std::string");
    p.parms2 = prependParm(temp3, "Trigger", "reposit::property_t");

    Printf(b_wrappers, "//***DEF\n");
    printList(p.parms2, b_wrappers);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(p.parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(p.parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(p.parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(p.parms2)));
    Printf(b_wrappers, "//***DEF\n");

    addinList_.functionWrapperImplCtor(p);

    return SWIG_OK;
}

int memberfunctionHandlerImpl(Node *n) {
    functionType=2;
    return Language::memberfunctionHandler(n);
}

int functionWrapperImplMemb(Node *n) {

    ParmsMemb p;

    p.n = n;
    p.name   = Getattr(n,"name");
    Node *n1 = Getattr(n,"parentNode");
    String   *cls   = Getattr(n1,"sym:name");
    p.pname   = Getattr(n1,"name");
    p.parms  = Getattr(n,"parms");
    p.addinClass = NewStringf("%s::%s", module, cls);

    String *temp0 = copyUpper(cls);
    String *temp1 = copyUpper(p.name);
    p.nameUpper = NewStringf("%s%s", temp0, temp1);
    p.funcName = NewStringf("%s%s%s", prefix, temp0, temp1);
    validateFunctionName(p.funcName);
    Setattr(p.n, "rp:funcName", p.funcName);
    Setattr(n, "rp:funcRename", p.funcName);
    printf("cls=%s\n", Char(cls));
    printf("p.name=%s\n", Char(p.name));
    printf("temp0=%s\n", Char(temp0));
    printf("temp1=%s\n", Char(temp1));
    printf("p.nameUpper=%s\n", Char(p.nameUpper));
    printf("p.funcName=%s\n", Char(p.funcName));
    Printf(b_init, "@@@ MEMB Name=%s\n", Char(p.funcName));

    // Create from parms another list parms2 - prepend an argument to represent
    // the object ID which is passed in as the first parameter to every ctor.
    ParmList *parmsTemp = Getattr(p.parms, "nextSibling");
    Parm *parmsTemp2 = prependParm(parmsTemp, "objectID", "std::string");
    p.parms2 = prependParm(parmsTemp2, "Trigger", "reposit::property_t");

    // We are invoking the member function of a class.
    // Create a dummy node and attach to it the type of the class.
    // This allows us to apply a typemap to the node.
    p.node = NewHash();
    Setfile(p.node, Getfile(p.n));
    Setline(p.node, Getline(p.n));
    Setattr(p.node, "type", p.pname);
    // Attach to the node some values that might be referenced by the typemap:
    Setattr(p.node, "rp_typedef_obj_add", p.addinClass);    // The type of the addin wrapper object
    Setattr(p.node, "rp_typedef_obj_lib", p.pname);         // The type of the library object

    Printf(b_wrappers, "//***ABC\n");
    printList(p.parms2, b_wrappers);
    Printf(b_wrappers, "// *a0* %s <<\n", Char(ParmList_str(p.parms)));
    Printf(b_wrappers, "// *a1* %s <<\n", Char(ParmList_protostr(p.parms)));
    Printf(b_wrappers, "// *a2* %s <<\n", Char(ParmList_str(p.parms2)));
    Printf(b_wrappers, "// *a3* %s <<\n", Char(ParmList_protostr(p.parms2)));
    Printf(b_wrappers, "//***ABC\n");

    addinList_.functionWrapperImplMemb(p);

    // Delete the dummy node.
    Delete(p.node);

    return SWIG_OK;
}

void processLoopParameter(Node *n, String *functionName, ParmList *parms, String *loopParameter) {
    for (Parm *p=parms; p; p=nextSibling(p)) {
        String *name = Getattr(p, "name");
        if (0==Strcmp(loopParameter, name)) {
            SwigType *t = Getattr(p, "type");
            SwigType *t2 = SwigType_base(t);
            Parm *p2 = NewHash();
            Setattr(p2, "type", t2);
            SwigType *t3 = NewString("std::vector");
            SwigType_add_template(t3, p2);
            Setattr(p, "type", t3);
            Setattr(n, "rp:loopParameterType", SwigType_str(t2, 0));

            SwigType *t4 = Getattr(n, "type");
            Setattr(n, "rp:loopFunctionType", SwigType_str(t4, 0));
            Parm *p3 = NewHash();
            Setattr(p3, "type", t4);
            SwigType *t5 = NewString("std::vector");
            SwigType_add_template(t5, p3);
            Setattr(n, "type", t5);

            return;
        }
    }
    REPOSIT_SWIG_FAIL(
        "Error processing function '" << Char(functionName) <<
        "' - you specified loop parameter '" << Char(loopParameter) << "' " <<
        "but the function has no parameter with that name.");
}

void functionWrapperImplAll(Node *n) {
    String *nodeName = Getattr(n, "name");
    printf("Processing node name '%s'.\n", Char(nodeName));

    ParmList *parms  = Getattr(n, "parms");

    String *loopParameter = Getattr(n, "feature:rp:loopParameter");
    if (loopParameter)
        processLoopParameter(n, nodeName, parms, loopParameter);

    // Process the parameter list.
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

