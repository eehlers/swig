
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wempty-body"
#endif

#include "swigmod.h"
#include <string>
#include <vector>
#include <set>
#include <fstream>
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
bool legacy = false;

// Default names for directories for source code and headers.
// FIXME store these defaults in reposit.swg and retrieve them here.
String *objDir = NewString("../AddinObjects");
String *addDir = NewString("../AddinCpp");
String *xllDir = NewString("../AddinXl");
// Path to the documentation directory.  Default is null i.e. no documentation generated.
String *doxDir = 0;
// The path to the docstrings.txt file.  Default is null i.e. no docstrings generated.
String *docStr = 0;
String *objInc = NewString("AddinObjects");
String *addInc = NewString("AddinCpp");
String *xllInc = NewString("AddinXl");

//*****************************************************************************
// ERROR HANDLING
//*****************************************************************************

List *errorList = NewHash();

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

Node *getNode(Node *n, const char *c, bool allowNull = false) {
    Node *ret = Getattr(n, c);
    REPOSIT_SWIG_REQUIRE(allowNull || ret, "Can't find node '" << c << "'");
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
        Replaceall(tm, "$rp_value", Getattr(n, "rp_value"));
        Replaceall(tm, "$rp_docstr", Getattr(n, "rp_docstr"));
        return tm;
    }
    if (fatal) {
        SwigType *t  = Getattr(n, "type");
        Setattr(errorList, NewStringf("*** ERROR : typemap '%s' does not match type '%s'.\n", m, SwigType_str(t, 0)), NewString("x"));
        // Return an error string, this will be inserted into the source code.
        return NewStringf("#error NEED THIS TYPEMAP: >>> %%typemap(%s) %s \"XXX\"; <<<", m, SwigType_str(t, 0));
    }
    return 0;
}

String *getType(Parm *p, const char *m, bool fatal) {
    if (0==strcmp(m, "rp_tm_default"))
        return Getattr(p, "type");
    else
        return getTypeMap(p, m, fatal);
}

void printIndent(File *buf, int indent) {
    for (int i=0;i<indent;++i)
        Printf(buf, "    ");
}

void emitTypeMap(File *buf, Node *n, const char *m, int indent=0, bool fatal = true, bool comment = true) {
    SwigType *t  = Getattr(n, "type");
    if (comment) {
        printIndent(buf, indent);
        Printf(buf, "// BEGIN typemap %s %s\n", m, t);
    }
    printIndent(buf, indent);
    String *s = getType(n, m, fatal);
    if (Len(s)) {
        Printf(buf, "%s\n", s);
        printIndent(buf, indent);
    }
    if (comment) {
        Printf(buf, "// END   typemap %s\n", m);
    }
}

void emitParmList(
    ParmList *parms,
    File *buf,
    int mode=0,         // 0=name, 1=type, 2=both
    const char *map="rp_tm_default",
    const char *map2="rp_tm_default",
    int indent=1,
    char delim=',',
    bool fatal=true,
    bool skipHidden=false,
    bool append=false,
    bool comment=true) {

    REPOSIT_SWIG_REQUIRE(map, "Function emitParmList called with null type map.");

    bool first = true;

    if (comment) {
        printIndent(buf, indent);
        Printf(buf, "// BEGIN typemap %s\n", map);
    }
    printIndent(buf, indent);

    for (Parm *p = parms; p; p = nextSibling(p)) {

        if (skipHidden && Getattr(p, "hidden"))
            continue;

        String *name = Getattr(p,"name");
        String *value = Getattr(p,"value");
        String *type = 0;
        if (value)
            type  = getType(p, map2, fatal);
        else
            type  = getType(p, map, fatal);

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
    if (comment) {
        Printf(buf, "// END   typemap %s\n", map);
    }
}

std::string hexLen(String *c) {
    std::stringstream s;
    s << std::hex << std::setw(2) << std::setfill('0') << Len(c);
    return s.str();
}

void excelParamCodes(File *b, Node *n, ParmList *parms) {
    String *s = getTypeMap(n, "rp_tm_xll_cdrt");
    for (Parm *p = parms; p; p = nextSibling(p)) {

        String *value = Getattr(p, "value");
        String *tm = 0;
        if (value)
            tm  = getTypeMap(p, "rp_tm_xll_code2");
        else
            tm  = getTypeMap(p, "rp_tm_xll_code");
        Append(s, tm);
    }
    Append(s, "#");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(s).c_str(), s);
}

void excelParamList(File *b, ParmList *parms) {
    String *s = NewString("");
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (Getattr(p, "hidden")) continue;
        if (first) {
            first = false;
        } else {
            Append(s, ",");
        }
        String *name = Getattr(p, "name");
        Append(s, name);
    }
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(s).c_str(), s);
}

void excelParamListDocStrings(File *b, ParmList *parms) {
    bool first = true;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (Getattr(p, "hidden")) continue;
        if (first) {
            first = false;
        } else {
            Printf(b, ",\n");
        }
        String *s = Getattr(p, "rp_docstr");
        // There is a bug in the Excel API which causes docstrings to get corrupted.
        // The workaround is to pad the last one with two spaces.
        if (!nextSibling(p))
            Append(s, "  ");
        Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\")", hexLen(s).c_str(), s);
    }
    Printf(b, "\n");
}

// For some reason Len(parms) does not seem to return the value that I would expect.
int paramListSize(ParmList *parms) {
    int ret = 0;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (Getattr(p, "hidden")) continue;
        ret++;
    }
    return ret;
}

void excelRegister(File *b, Node *n, String *funcName, String *funcDoc, ParmList *parms, String *groupFunctionWizard) {
    Printf(b, "        // BEGIN function excelRegister\n");
    Printf(b, "        Excel(xlfRegister, 0, %d, &xDll,\n", 10 + paramListSize(parms));
    Printf(b, "            // function code name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // parameter codes\n");
    excelParamCodes(b, n, parms);
    Printf(b, "            // function display name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // comma-delimited list of parameter names\n");
    excelParamList(b, parms);
    Printf(b, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
    Printf(b, "            TempStrNoSize(\"\\x01\"\"1\"),\n");
    Printf(b, "            // function category\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(groupFunctionWizard).c_str(), groupFunctionWizard);
    Printf(b, "            // shortcut text (command macros only)\n");
    Printf(b, "            TempStrNoSize(\"\\x00\"\"\"),\n");
    Printf(b, "            // path to help file\n");
    Printf(b, "            TempStrNoSize(\"\\x00\"\"\"),\n");
    Printf(b, "            // function description\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcDoc).c_str(), funcDoc);
    Printf(b, "            // parameter descriptions\n");
    excelParamListDocStrings(b, parms);
    Printf(b, "        );\n");
    Printf(b, "        // END   function excelRegister\n\n");
}

void excelUnregister(File *b, Node *n, String *funcName, String *funcDoc, ParmList *parms, String *groupFunctionWizard) {
    Printf(b, "        // BEGIN function excelUnregister\n");
    Printf(b, "        Excel(xlfRegister, 0, %d, &xDll,\n", 10 + paramListSize(parms));
    Printf(b, "            // function code name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // parameter codes\n");
    excelParamCodes(b, n, parms);
    Printf(b, "            // function display name\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
    Printf(b, "            // comma-delimited list of parameter names\n");
    excelParamList(b, parms);
    Printf(b, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
    Printf(b, "            TempStrNoSize(\"\\x01\"\"0\"),\n");
    Printf(b, "            // function category\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(groupFunctionWizard).c_str(), groupFunctionWizard);
    Printf(b, "            // shortcut text (command macros only)\n");
    Printf(b, "            TempStrNoSize(\"\\x00\"\"\"),\n");
    Printf(b, "            // path to help file\n");
    Printf(b, "            TempStrNoSize(\"\\x00\"\"\"),\n");
    Printf(b, "            // function description\n");
    Printf(b, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcDoc).c_str(), funcDoc);
    Printf(b, "            // parameter descriptions\n");
    excelParamListDocStrings(b, parms);
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

// convert Class_memberFunction to ClassMemberFunction
String *copyUpper3(String *s) {
    String *ret = Copy(s);
    char *c = Char(ret);
    for (unsigned int i=0; i<strlen(c); i++) {
        if ('_'==c[i] && i+1<strlen(c))
            c[i+1] = toupper(c[i+1]);
    }
    Replaceall(ret, "_", "");
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
// DOCSTRINGS
//*****************************************************************************

std::map<std::string, std::string> m;

void initializeDocStrings() {
    if (!docStr) {
        printf("the path to the docstrings.txt file is not specified. "
            "No docstrings will be generated.\n");
        return;
    }
    std::ifstream f(Char(docStr));
    if (!f) {
        printf("Error opening docstrings file %s. "
            "No docstrings will be generated.\n", Char(docStr));
        return;
    }
    std::string s;
    getline(f, s);
    while (f) {
        size_t i = s.find("=");
        if (std::string::npos == i)
            REPOSIT_SWIG_FAIL("docstrings file - invalid format");
        std::string k = s.substr(0, i);
        std::string v = s.substr(i+1, s.length()-i-1);
        printf("k=%s v=%s\n", k.c_str(), v.c_str());
        m[k]=v;
        getline(f, s);
    }
}

std::string getDocString(const std::string &scope, const std::string &func, const std::string &parm) {
    std::string k = scope + "|" + func + "|" + parm;
    printf("k=%s\n", k.c_str());
    std::map<std::string, std::string>::const_iterator i = m.find(k);
    if (m.end()==i)
        return std::string();
    else
        return m[k];
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
        functions(0), constructors(0), members(0), total2(0) {}
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
    String *groupName_;
    String *displayName_;
    String *groupFunctionWizard_;
    String *lib_inc;
    String *add_inc;
    bool automatic_;
    Pragmas() : groupName_(0), lib_inc(0), add_inc(0), automatic_(true) {}
    Pragmas & operator= (const Pragmas & other) {
        groupName_ = other.groupName_;
        displayName_ = other.displayName_;
        groupFunctionWizard_ = other.groupFunctionWizard_;
        lib_inc = other.lib_inc;
        add_inc = other.add_inc;
        automatic_ = other.automatic_;
        return *this;
    }
    void setGroupName(String *groupName) {
        groupName_ = groupName;
        displayName_ = groupName;
        groupFunctionWizard_ = module;
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
    String *funcName;
    std::string docStr;
};

struct ParmsCtor {
    Node *n;
    String *name;
    String *funcName;
    String *funcRename;
    String *alias;
    ParmList *parms;
    ParmList *parms2;
    String *pname;
    String *base;
    bool multipleBaseClasses;
    std::string docStr;
};

struct ParmsMemb {
    Node *n;
    //String *nameUpper;
    String *funcName;
    String *funcRename;
    String *alias;
    ParmList *parms;
    ParmList *parms2;
    String *pname;
    String *addinClass;
    String *name;
    Node *node;
    std::string docStr;
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
    Buffer *b_lib_loop_hpp;
    bool generateHppFile;
    bool generateLoopFile;

    GroupLibraryObjects(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count), generateHppFile(false), generateLoopFile(false) {

        if (pragmas_.automatic_) {
            b_lib_grp_hpp = new Buffer("b_lib_grp_hpp", NewStringf("%s/objects/obj_%s.hpp", objDir, pragmas_.groupName_));
        } else {
            b_lib_grp_hpp = new Buffer("b_lib_grp_hpp", NewStringf("%s/objects/objmanual_%s.hpp.template", objDir, pragmas_.groupName_));
        }
        b_lib_loop_hpp = new Buffer("b_lib_loop_hpp", NewStringf("%s/loop/loop_%s.hpp", objDir, pragmas_.groupName_));

        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#ifndef obj_%s_hpp\n", pragmas_.groupName_);
        Printf(b_lib_grp_hpp->b0, "#define obj_%s_hpp\n", pragmas_.groupName_);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#include <string>\n");
        Printf(b_lib_grp_hpp->b0, "#include <rp/libraryobject.hpp>\n");
        Printf(b_lib_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_lib_grp_hpp->b0, "#include <boost/shared_ptr.hpp>\n");

        Append(b_lib_grp_hpp->b0, pragmas_.lib_inc);

        Printf(b_lib_grp_hpp->b0, "// FIXME get rid of this:\n");
        Printf(b_lib_grp_hpp->b0, "using namespace %s;\n", nmspace);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0,"namespace %s {\n", module);
        Printf(b_lib_grp_hpp->b0, "\n");


        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "#ifndef loop_%s_hpp\n", pragmas_.groupName_);
        Printf(b_lib_loop_hpp->b0, "#define loop_%s_hpp\n", pragmas_.groupName_);
        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "#include <boost/bind.hpp>\n");
        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "namespace %s {\n", module);
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        if (String *loopParameterName = Getattr(p.n, "feature:rp:loopParameter")) {
            Node *x1 = Getattr(p.n, "rp:loopFunctionNode");
            String *loopFunctionType = getTypeMap(x1, "rp_tm_xll_lpfn");
            Parm *x2 = Getattr(p.n, "rp:loopParameterNode");
            String *loopParameterType = getTypeMap(x2, "rp_tm_xll_lppm");

            Printf(b_lib_loop_hpp->b0, "    // %s\n", p.funcName);
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "        typedef     boost::_bi::bind_t<\n");
            Printf(b_lib_loop_hpp->b0, "                    %s,\n", loopFunctionType);
            Printf(b_lib_loop_hpp->b0, "                    %s (__cdecl*)(\n", loopFunctionType);
            bool first = true;
            for (Parm *x=p.parms; x; x=nextSibling(x)) {
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }

                if (0==Strcmp(loopParameterName, Getattr(x, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                        %s", SwigType_str(Getattr(x2, "type"), 0));
                } else {
                    Printf(b_lib_loop_hpp->b0, "                        %s", SwigType_str(Getattr(x, "type"), 0));
                }
            }
            Printf(b_lib_loop_hpp->b0, "),\n");
            Printf(b_lib_loop_hpp->b0, "                    boost::_bi::list%d<\n", paramListSize(p.parms));
            first = true;
            for (Parm *x=p.parms; x; x=nextSibling(x)) {
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(x, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                        boost::arg<1>");
                } else {
                    Printf(b_lib_loop_hpp->b0, "                        boost::_bi::value<%s>", SwigType_str(SwigType_base(Getattr(x, "type")), 0));
                }
            }
            Printf(b_lib_loop_hpp->b0, " > >\n");
            Printf(b_lib_loop_hpp->b0, "                    %sBind;\n", p.funcName);
            Printf(b_lib_loop_hpp->b0, "\n");
            generateLoopFile=true;
        }
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        if (generateCtor) {

            String *s0 = NewString("");
            String *s1 = NewString("");
            if (p.base) {
                // Autogeneration of object wrapper code is not supported for multiple inheritance.
                REPOSIT_SWIG_REQUIRE(!pragmas_.automatic_ || !p.multipleBaseClasses,
                    "Class '" << Char(p.name) << "' has multiple base classes.\n"
                    "Autogeneration of object wrapper code is not supported for multiple inheritance.\n"
                    "Use the %override directive to suppress autogeneration, and implement the code manually.");
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
            emitParmList(p.parms, b_lib_grp_hpp->b0, 2, "rp_tm_default", "rp_tm_default", 3, ',', true, false, true);
            Printf(b_lib_grp_hpp->b0,"            bool permanent)\n");
            Printf(b_lib_grp_hpp->b0,"        : %s(properties, permanent) {\n", s0);
            Printf(b_lib_grp_hpp->b0,"            libraryObject_ = boost::shared_ptr<%s>(new %s(\n", s1, p.pname);
            emitParmList(p.parms, b_lib_grp_hpp->b0, 0, "rp_tm_default", "rp_tm_default", 4);
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

    void functionWrapperImplMemb(ParmsMemb &p) {

        if (String *loopParameterName = Getattr(p.n, "feature:rp:loopParameter")) {
            Node *x1 = Getattr(p.n, "rp:loopFunctionNode");
            String *loopFunctionType = getTypeMap(x1, "rp_tm_xll_lpfn");
            Parm *x2 = Getattr(p.n, "rp:loopParameterNode");
            String *loopParameterType = getTypeMap(x2, "rp_tm_xll_lppm");
            String *loopParameterType2 = getTypeMap(x2, "rp_tm_xll_lppm2");
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "    // %s\n", p.funcName);
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "    typedef     boost::_bi::bind_t<\n");
            Printf(b_lib_loop_hpp->b0, "                %s,\n", loopFunctionType);
            if (SwigType_isconst(Getattr(p.n, "decl")))
                Printf(b_lib_loop_hpp->b0, "                boost::_mfi::cmf%d<\n", paramListSize(p.parms));
            else
                Printf(b_lib_loop_hpp->b0, "                boost::_mfi::mf%d<\n", paramListSize(p.parms));
            Printf(b_lib_loop_hpp->b0, "                    %s,\n", loopFunctionType);
            Printf(b_lib_loop_hpp->b0, "                    %s,\n", p.pname);
            bool first = true;
            for (Parm *x=p.parms; x; x=nextSibling(x)) {
                if (Getattr(x, "hidden"))
                    continue;
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(x, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                    %s", SwigType_str(Getattr(x2, "type"), 0));
                } else {
                    Printf(b_lib_loop_hpp->b0, "                    %s", SwigType_str(Getattr(x, "type"), 0));
                }
            }
            Printf(b_lib_loop_hpp->b0, ">,\n");

            Printf(b_lib_loop_hpp->b0, "                boost::_bi::list%d<\n", paramListSize(p.parms)+1);
            Printf(b_lib_loop_hpp->b0, "                    boost::_bi::value<%s >,\n", getTypeMap(p.node, "rp_tm_lib_loop"));
            first = true;
            for (Parm *x=p.parms; x; x=nextSibling(x)) {
                if (Getattr(x, "hidden"))
                    continue;
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(x, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                    boost::arg<1>");
                } else {
                    Printf(b_lib_loop_hpp->b0, "                    boost::_bi::value<%s>", SwigType_str(SwigType_base(Getattr(x, "type")), 0));
                }
            }
            Printf(b_lib_loop_hpp->b0, " > >\n");
            Printf(b_lib_loop_hpp->b0, "                    %sBind;\n", p.funcName);
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "    typedef     %s\n", loopFunctionType);
            Printf(b_lib_loop_hpp->b0, "                (%s::* %sSignature)(\n", p.pname, p.funcName);
            first = true;
            for (Parm *x=p.parms; x; x=nextSibling(x)) {
                if (Getattr(x, "hidden"))
                    continue;
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(x, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                    %s", loopParameterType2);
                } else {
                    Printf(b_lib_loop_hpp->b0, "                    %s", SwigType_str(Getattr(x, "type"), 0));
                }
            }
            if (SwigType_isconst(Getattr(p.n, "decl")))
                Printf(b_lib_loop_hpp->b0, ") const;\n");
            else
                Printf(b_lib_loop_hpp->b0, ");\n");
            Printf(b_lib_loop_hpp->b0, "\n");
            generateLoopFile=true;
        }
    }

    void clear() {

        Printf(b_lib_grp_hpp->b0, "} // namespace %s\n", module);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#endif\n");

        Printf(b_lib_loop_hpp->b0, "}\n");
        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "#endif\n");
        Printf(b_lib_loop_hpp->b0, "\n");

        b_lib_grp_hpp->clear(count_, generateHppFile);
        b_lib_loop_hpp->clear(count_, generateLoopFile);
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
        emitParmList(p.parms, b_vob_grp_hpp->b0, 2, "rp_tm_vob_parm", "rp_tm_vob_parm", 4, ',', true, false, true);
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
        emitParmList(p.parms, b_vob_grp_hpp->b0, 1, "rp_tm_vob_mbvr", "rp_tm_vob_mbvr", 3, ';', true, false, true);
        Printf(b_vob_grp_hpp->b0,"            bool Permanent_;\n");
        if (String *processorName = Getattr(p.n, "feature:rp:processorName"))
            Printf(b_vob_grp_hpp->b0,"            virtual std::string processorName() { return \"%s\"; }\n", processorName);
        Printf(b_vob_grp_hpp->b0,"\n");
        Printf(b_vob_grp_hpp->b0,"            template<class Archive>\n");
        Printf(b_vob_grp_hpp->b0,"            void serialize(Archive& ar, const unsigned int) {\n");
        Printf(b_vob_grp_hpp->b0,"            boost::serialization::void_cast_register<%s, reposit::ValueObject>(this, this);\n", p.funcRename);
        Printf(b_vob_grp_hpp->b0,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"ClassName\", className_)\n");
        emitParmList(p.parms, b_vob_grp_hpp->b0, 1, "rp_tm_vob_srmv", "rp_tm_vob_srmv", 5, 0);
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
        Printf(b_vob_grp_hpp->b0,"            }\n");
        Printf(b_vob_grp_hpp->b0,"        };\n");
        Printf(b_vob_grp_hpp->b0,"\n");

        Printf(b_vob_grp_cpp->b0,"        const char* %s::mPropertyNames[] = {\n", p.funcRename);
        emitParmList(p.parms, b_vob_grp_cpp->b0, 1, "rp_tm_vob_name", "rp_tm_vob_name", 3, ',', true, false, true);
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
        emitParmList(p.parms, b_vob_grp_cpp->b0, 2, "rp_tm_vob_parm", "rp_tm_vob_parm", 4, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"                bool Permanent) :\n");
        Printf(b_vob_grp_cpp->b0,"            reposit::ValueObject(ObjectId, \"%s\", Permanent),\n", p.funcRename);
        emitParmList(p.parms, b_vob_grp_cpp->b0, 1, "rp_tm_vob_init", "rp_tm_vob_init", 3, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"            Permanent_(Permanent) {\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");

        count_.constructors++;
        count_.total2++;

        generateOutput = true;
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
        //Printf(b_scr_grp_cpp->b0, "//#include <%s/objects/handle.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b0, "\n");
        if (pragmas_.automatic_) {
            Printf(b_scr_grp_cpp->b0, "#include <%s/objects/obj_%s.hpp>\n", objInc, pragmas_.groupName_);
        } else {
            Printf(b_scr_grp_cpp->b0, "#include <%s/objects/objmanual_%s.hpp>\n", objInc, pragmas_.groupName_);
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
        emitParmList(p.parms, b_scr_grp_cpp->b0, 1, "rp_tm_scr_cnvt", "rp_tm_scr_cnvt", 1, 0);
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    bool Permanent =\n");
        Printf(b_scr_grp_cpp->b0, "        reposit::convert2<bool>(valueObject->getProperty(\"Permanent\"));\n");
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    // construct and return the object\n");
        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "    boost::shared_ptr<reposit::Object> object(\n");
        Printf(b_scr_grp_cpp->b0, "        new %s::%s(\n", module, p.name);
        Printf(b_scr_grp_cpp->b0, "            valueObject,\n");
        emitParmList(p.parms, b_scr_grp_cpp->b0, 0, "rp_tm_default", "rp_tm_default", 3, ',', true, false, true);
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

struct GroupDoxygen : public GroupBase {

    Buffer *b_dox_grp_dox;
    std::vector<std::string> v;

    GroupDoxygen(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count) {

        b_dox_grp_dox = new Buffer("b_dox_grp_dox", NewStringf("%s/functions_01_%s.dox", doxDir, pragmas_.groupName_));

        Printf(b_dox_grp_dox->b0, "\n");
        Printf(b_dox_grp_dox->b0, "/*! \\page func_%s Functions - %s\n", pragmas_.groupName_, pragmas_.displayName_);
        Printf(b_dox_grp_dox->b0, "\\section overview_%s Overview\n", pragmas_.groupName_);
        Printf(b_dox_grp_dox->b0, "Documentation for function group %s.\n", pragmas_.groupName_);
        Printf(b_dox_grp_dox->b0, "\\section functions_%s Function List\n", pragmas_.groupName_);
        Printf(b_dox_grp_dox->b0, "\n");
        // Start writing to b1 instead of b0.  Later we will append more text to b0.
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        Printf(b_dox_grp_dox->b1,"\\subsection %s\n", p.funcName);
        Printf(b_dox_grp_dox->b1,"\\code\n");
        Printf(b_dox_grp_dox->b1, "\n");
        emitTypeMap(b_dox_grp_dox->b1, p.n, "rp_tm_dox_rtdc", 0, true, false);
        Printf(b_dox_grp_dox->b1, "\n");
        Printf(b_dox_grp_dox->b1, "%s(\n", p.funcName);
        emitParmList(p.parms2, b_dox_grp_dox->b1, 1, "rp_tm_dox_mbvr", "rp_tm_dox_mbvr", 1, ',', true, true, false, false);
        Printf(b_dox_grp_dox->b1, ")\n");
        Printf(b_dox_grp_dox->b1,"\\endcode\n");
        Printf(b_dox_grp_dox->b1,"\\par Type:\n");
        Printf(b_dox_grp_dox->b1,"Function\n");
        Printf(b_dox_grp_dox->b1,"\\par Description:\n");
        Printf(b_dox_grp_dox->b1, "%s\n", Char(p.docStr.c_str()));
        Printf(b_dox_grp_dox->b1,"\\par Supported Platforms:\n");
        Printf(b_dox_grp_dox->b1, "Excel");
        if (checkAttribute(p.n, "feature:rp:generate:c++", "1"))
            Printf(b_dox_grp_dox->b1, ", C++");
        if (checkAttribute(p.n, "feature:rp:generate:c#", "1"))
            Printf(b_dox_grp_dox->b1, ", C#");
        Printf(b_dox_grp_dox->b1, "\n");
        emitParmList(p.parms2, b_dox_grp_dox->b1, 1, "rp_tm_dox_mbv2", "rp_tm_dox_mbv2", 0, 0, true, false, false, false);
        Printf(b_dox_grp_dox->b1, "\n");
        Printf(b_dox_grp_dox->b1, "\n");

        v.push_back(Char(p.funcName));

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        if (generateCtor) {

            Printf(b_dox_grp_dox->b1,"\\subsection %s\n", p.funcRename);
            Printf(b_dox_grp_dox->b1,"\\code\n");
            Printf(b_dox_grp_dox->b1, "\n");
            Printf(b_dox_grp_dox->b1, "string returnValue\n");
            Printf(b_dox_grp_dox->b1, "\n");
            Printf(b_dox_grp_dox->b1, "%s(\n", p.funcRename);
            emitParmList(p.parms2, b_dox_grp_dox->b1, 1, "rp_tm_dox_mbvr", "rp_tm_dox_mbvr", 1, ',', true, true, false, false);
            Printf(b_dox_grp_dox->b1, ")\n");
            Printf(b_dox_grp_dox->b1,"\\endcode\n");
            Printf(b_dox_grp_dox->b1,"\\par Type:\n");
            Printf(b_dox_grp_dox->b1,"Constructor\n");
            Printf(b_dox_grp_dox->b1,"\\par Description:\n");
            Printf(b_dox_grp_dox->b1, "%s\n", Char(p.docStr.c_str()));
            Printf(b_dox_grp_dox->b1,"\\par Supported Platforms:\n");
            Printf(b_dox_grp_dox->b1, "Excel");
            if (checkAttribute(p.n, "feature:rp:generate:c++", "1"))
                Printf(b_dox_grp_dox->b1, ", C++");
            if (checkAttribute(p.n, "feature:rp:generate:c#", "1"))
                Printf(b_dox_grp_dox->b1, ", C#");
            Printf(b_dox_grp_dox->b1, "\n");
            emitParmList(p.parms2, b_dox_grp_dox->b1, 1, "rp_tm_dox_mbv2", "rp_tm_dox_mbv2", 0, 0, true, false, false, false);
            Printf(b_dox_grp_dox->b1, "\n");
            Printf(b_dox_grp_dox->b1, "\n");

            v.push_back(Char(p.funcRename));

            count_.constructors++;
            count_.total2++;
        }
    }

    void functionWrapperImplMemb(ParmsMemb &p) {
        Printf(b_dox_grp_dox->b1,"\\subsection %s\n", p.funcRename);
        Printf(b_dox_grp_dox->b1,"\\code\n");
        Printf(b_dox_grp_dox->b1, "\n");
        emitTypeMap(b_dox_grp_dox->b1, p.n, "rp_tm_dox_rtdc", 0, true, false);
        Printf(b_dox_grp_dox->b1, "\n");
        Printf(b_dox_grp_dox->b1, "%s(\n", p.funcRename);
        emitParmList(p.parms2, b_dox_grp_dox->b1, 1, "rp_tm_dox_mbvr", "rp_tm_dox_mbvr", 1, ',', true, true, false, false);
        Printf(b_dox_grp_dox->b1, ")\n");
        Printf(b_dox_grp_dox->b1,"\\endcode\n");
        Printf(b_dox_grp_dox->b1,"\\par Type:\n");
        Printf(b_dox_grp_dox->b1,"Member\n");
        Printf(b_dox_grp_dox->b1,"\\par Description:\n");
        Printf(b_dox_grp_dox->b1, "%s\n", Char(p.docStr.c_str()));
        if (String *loopParameterName = Getattr(p.n, "feature:rp:loopParameter")) {
            Printf(b_dox_grp_dox->b1,"\\par Looping Function:\n");
            Printf(b_dox_grp_dox->b1, "This function loops on parameter \\b %s.\n", loopParameterName);
        }
        Printf(b_dox_grp_dox->b1,"\\par Supported Platforms:\n");
        Printf(b_dox_grp_dox->b1, "Excel");
        if (checkAttribute(p.n, "feature:rp:generate:c++", "1"))
            Printf(b_dox_grp_dox->b1, ", C++");
        if (checkAttribute(p.n, "feature:rp:generate:c#", "1"))
            Printf(b_dox_grp_dox->b1, ", C#");
        Printf(b_dox_grp_dox->b1, "\n");
        emitParmList(p.parms2, b_dox_grp_dox->b1, 1, "rp_tm_dox_mbv2", "rp_tm_dox_mbv2", 0, 0, true, false, false, false);
        Printf(b_dox_grp_dox->b1, "\n");
        Printf(b_dox_grp_dox->b1, "\n");

        v.push_back(Char(p.funcRename));

        count_.members++;
        count_.total2++;
    }

    void clear() {

        // Append to b0 the alphabetical list of links
        std::sort(v.begin(), v.end());

        typedef std::vector<std::string>::const_iterator iter;
        for (iter i=v.begin(); i!=v.end(); ++i) {
            Printf(b_dox_grp_dox->b0, "\\ref %s ()\\n\n", i->c_str());
        }

        Printf(b_dox_grp_dox->b0, "\n");
        Printf(b_dox_grp_dox->b0, "\\section documentation_%s Function Documentation\n", pragmas_.groupName_);

        Printf(b_dox_grp_dox->b1, "\n");
        Printf(b_dox_grp_dox->b1, "*/\n");
        Printf(b_dox_grp_dox->b1, "\n");

        b_dox_grp_dox->clear(count_);
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
        Printf(b_cpp_grp_cpp->b0, "//FIXME this #include is only required if the file contains conversions\n");
        Printf(b_cpp_grp_cpp->b0, "#include <%s/conversions/all.hpp>\n", objInc);
        //Printf(b_cpp_grp_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
        Printf(b_cpp_grp_cpp->b0, "#include <%s/conversions/coercetermstructure.hpp>\n", objInc);
        // FIXME this #include is only required if the file contains enumerations.
        //Printf(b_cpp_grp_cpp->b0, "#include <rp/enumerations/typefactory.hpp>\n");

        // From this point on we stop writing to b0 and write to b1 instead.
        // After all processing finishes we will append some more #includes to b0 depending on what code this group requires.

        Printf(b_cpp_grp_cpp->b1, "//FIXME include only factories for types used in the current file\n");
        Printf(b_cpp_grp_cpp->b1, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        Printf(b_cpp_grp_cpp->b1, "#include <boost/shared_ptr.hpp>\n");
        Printf(b_cpp_grp_cpp->b1, "#include <rp/repository.hpp>\n");
        //Printf(b_cpp_grp_cpp->b1, "#include <AddinCpp/add_all.hpp>\n");
        Printf(b_cpp_grp_cpp->b1, "\n");
    }

    void functionWrapperImplFunc(ParmsFunc &p) {

        emitTypeMap(b_cpp_grp_hpp->b0, p.n, "rp_tm_cpp_rttp", 1);
        Printf(b_cpp_grp_hpp->b0,"    %s(\n", p.funcName);
        emitParmList(p.parms, b_cpp_grp_hpp->b0, 2, "rp_tm_cpp_parm", "rp_tm_cpp_parm2", 2);
        Printf(b_cpp_grp_hpp->b0,"    );\n");

        Printf(b_cpp_grp_cpp->b1,"//****FUNC*****\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rttp");
        Printf(b_cpp_grp_cpp->b1,"%s::%s(\n", addinCppNameSpace, p.funcName);
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 2, "rp_tm_cpp_parm", "rp_tm_cpp_parm2");
        Printf(b_cpp_grp_cpp->b1,") {\n");
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_cnvt", "rp_tm_cpp_cnvt2", 1, 0, false);
        Printf(b_cpp_grp_cpp->b1,"\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtdc", 2);
        Printf(b_cpp_grp_cpp->b1,"    %s::%s(\n", nmspace, p.symname);
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_args", "rp_tm_cpp_args2", 2, ',', true, true);
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
            emitParmList(p.parms2, b_cpp_grp_hpp->b0, 2, "rp_tm_cpp_parm", "rp_tm_cpp_parm", 2);
            Printf(b_cpp_grp_hpp->b0,"    );\n\n");

            Printf(b_cpp_grp_cpp->b1,"//****CTOR*****\n");
            Printf(b_cpp_grp_cpp->b1,"std::string %s::%s(\n", addinCppNameSpace, p.funcName);
            emitParmList(p.parms2, b_cpp_grp_cpp->b1, 2, "rp_tm_cpp_parm", "rp_tm_cpp_parm", 2);
            Printf(b_cpp_grp_cpp->b1,"    ) {\n");
            Printf(b_cpp_grp_cpp->b1,"\n");
            Printf(b_cpp_grp_cpp->b1,"    // Convert input types into Library types\n\n");
            emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_cnvt", "rp_tm_cpp_cnvt", 1, 0, false);
            Printf(b_cpp_grp_cpp->b1,"\n");
            Printf(b_cpp_grp_cpp->b1,"    boost::shared_ptr<reposit::ValueObject> valueObject(\n");
            Printf(b_cpp_grp_cpp->b1,"        new %s::ValueObjects::%s(\n", module, p.funcName);
            Printf(b_cpp_grp_cpp->b1,"            objectID,\n");
            emitParmList(p.parms, b_cpp_grp_cpp->b1, 0, "rp_tm_default", "rp_tm_default", 3, ',', true, false, true);
            Printf(b_cpp_grp_cpp->b1,"            false));\n");
            Printf(b_cpp_grp_cpp->b1,"    boost::shared_ptr<reposit::Object> object(\n");
            Printf(b_cpp_grp_cpp->b1,"        new %s::%s(\n", module, p.name);
            Printf(b_cpp_grp_cpp->b1,"            valueObject,\n");
            emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_args", "rp_tm_cpp_args", 3, ',', true, true, true);
            Printf(b_cpp_grp_cpp->b1,"            false));\n");
            Printf(b_cpp_grp_cpp->b1,"    std::string returnValue =\n");
            Printf(b_cpp_grp_cpp->b1,"        reposit::Repository::instance().storeObject(\n");
            Printf(b_cpp_grp_cpp->b1,"            objectID, object, Overwrite, valueObject);\n");
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
        emitParmList(p.parms2, b_cpp_grp_hpp->b0, 2, "rp_tm_cpp_parm", "rp_tm_cpp_parm", 2);
        Printf(b_cpp_grp_hpp->b0,"    );\n\n");

        Printf(b_cpp_grp_cpp->b1,"//****MEMB*****\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtmb");
        Printf(b_cpp_grp_cpp->b1,"%s::%s(\n", addinCppNameSpace, p.funcName);
        emitParmList(p.parms2, b_cpp_grp_cpp->b1, 2, "rp_tm_cpp_parm", "rp_tm_cpp_parm", 2);
        Printf(b_cpp_grp_cpp->b1,"    ) {\n\n");
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_cnvt", "rp_tm_cpp_cnvt", 1, 0, false);
        Printf(b_cpp_grp_cpp->b1,"\n");
        emitTypeMap(b_cpp_grp_cpp->b1, p.node, "rp_tm_xxx_rp_get");
        emitTypeMap(b_cpp_grp_cpp->b1, p.n, "rp_tm_cpp_rtdc", 2);
        Printf(b_cpp_grp_cpp->b1,"    xxx->%s(\n", p.name);
        emitParmList(p.parms, b_cpp_grp_cpp->b1, 1, "rp_tm_cpp_args", "rp_tm_cpp_args", 3, ',', true, true);
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
                Printf(b_cpp_grp_cpp->b0, "#include \"%s/objects/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
            } else {
                Printf(b_cpp_grp_cpp->b0, "#include \"%s/objects/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
            }
        }
        Append(b_cpp_grp_cpp->b0, pragmas_.add_inc);

        Printf(b_cpp_grp_cpp->b1, "\n");

        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "} // namespace %s\n", addinCppNameSpace);
        Printf(b_cpp_grp_hpp->b0, "\n");
        Printf(b_cpp_grp_hpp->b0, "#endif\n");
        Printf(b_cpp_grp_hpp->b0, "\n");

        b_cpp_grp_hpp->clear(count_);
        b_cpp_grp_cpp->clear(count_);
    }
};

struct GroupExcelFunctions : public GroupBase {

    Buffer *b_xlf_grp_cpp;
    bool groupContainsClass;
    bool groupContainsConstructor;
    bool groupContainsLoopFunction;

    GroupExcelFunctions(const Pragmas &pragmas, Count &count) : GroupBase(pragmas, count),
        groupContainsClass(false), groupContainsConstructor(false), groupContainsLoopFunction(false) {

        b_xlf_grp_cpp = new Buffer("b_xlf_grp_cpp", NewStringf("%s/functions/function_%s.cpp", xllDir, pragmas_.groupName_));

        Printf(b_xlf_grp_cpp->b0, "\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/repositxl.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/register/register_all.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/functions/export.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/utilities/xlutilities.hpp>\n");
        Printf(b_xlf_grp_cpp->b0, "#include <rpxl/objectwrapperxl.hpp>\n");
        //Printf(b_xlf_grp_cpp->b0, "#include <%s/coercions/all.hpp>\n", objInc);
        Printf(b_xlf_grp_cpp->b0, "#include \"%s/enumerations/factories/all.hpp\"\n", objInc);
        // From this point on we stop writing to b0 and write to b1 instead.
        // After all processing finishes we will append some more #includes to b0 depending on what code this group requires.

        Printf(b_xlf_grp_cpp->b1, "//#include \"%s/conversions/convert2.hpp\"\n", objInc);
        Printf(b_xlf_grp_cpp->b1, "#include \"%s/conversions/conversions.hpp\"\n", objInc);
        Printf(b_xlf_grp_cpp->b1, "#include \"%s/conversions/all.hpp\"\n", xllInc);
        Printf(b_xlf_grp_cpp->b1, "\n");
    }

    void emitLoopFunc(ParmsFunc &p, String *loopParameterName) {
        Node *x1 = Getattr(p.n, "rp:loopFunctionNode");
        String *loopFunctionType = getTypeMap(x1, "rp_tm_xll_lpfn");
        Parm *x2 = Getattr(p.n, "rp:loopParameterNode");
        String *loopParameterType = getTypeMap(x2, "rp_tm_xll_lppm");
        Printf(b_xlf_grp_cpp->b1, "        // BEGIN function emitLoopFunc\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        static XLOPER returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        %s::%sBind bindObject =\n", module, p.funcName);
        Printf(b_xlf_grp_cpp->b1, "            boost::bind(\n");
        Printf(b_xlf_grp_cpp->b1, "                %s,\n", p.name);
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_loop", "rp_tm_xll_loop2", 4, ',', true, true);
        Printf(b_xlf_grp_cpp->b1, "            );\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::loop\n");
        Printf(b_xlf_grp_cpp->b1, "            <%s::%sBind, %s, %s>\n", module, p.funcName, loopParameterType, loopFunctionType);
        Printf(b_xlf_grp_cpp->b1, "            (functionCall, bindObject, %s, returnValue);\n", loopParameterName);
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
        emitParmList(p.parms2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", "rp_tm_xll_parm2", 1);
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
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", "rp_tm_xll_cnvt2", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (String *loopParameterName = Getattr(p.n, "feature:rp:loopParameter")) {
            emitLoopFunc(p, loopParameterName);
            groupContainsLoopFunction = true;
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtdc", 2);
            //Printf(b_xlf_grp_cpp->b1, "        %s::%s(\n", nmspace, p.symname);
            Printf(b_xlf_grp_cpp->b1, "        %s(\n", p.name);
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", "rp_tm_xll_argf2", 3, ',', true, true);
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

    void functionWrapperImplCtorImpl(ParmsCtor &p, String *funcName) {

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****CTOR*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT char *%s(\n", funcName);
        emitParmList(p.parms2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", "rp_tm_xll_parm2");
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", "rp_tm_xll_cnvt2", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        boost::shared_ptr<reposit::ValueObject> valueObject(\n");
        Printf(b_xlf_grp_cpp->b1, "            new %s::ValueObjects::%s(\n", module, funcName);
        Printf(b_xlf_grp_cpp->b1, "                objectID,\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argfv", "rp_tm_xll_argfv2", 4, ',', true, true, true);
        Printf(b_xlf_grp_cpp->b1, "                false));\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        boost::shared_ptr<reposit::Object> object(\n");
        Printf(b_xlf_grp_cpp->b1, "            new %s::%s(\n", module, p.name);
        Printf(b_xlf_grp_cpp->b1, "                valueObject,\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", "rp_tm_xll_argf2", 4, ',', true, true, true);
        Printf(b_xlf_grp_cpp->b1, "                false));\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        std::string returnValue =\n");
        Printf(b_xlf_grp_cpp->b1, "            reposit::RepositoryXL::instance().storeObject(\n");
        Printf(b_xlf_grp_cpp->b1, "                objectID, object, *Overwrite, valueObject);\n");
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
    }

    void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            functionWrapperImplCtorImpl(p, p.funcRename);
            if (p.alias)
                functionWrapperImplCtorImpl(p, p.alias);
            groupContainsConstructor = true;
        }
        groupContainsClass = true;
    }

    void emitLoopFunc(ParmsMemb &p, String *loopParameterName) {
        Node *x1 = Getattr(p.n, "rp:loopFunctionNode");
        String *loopFunctionType = getTypeMap(x1, "rp_tm_xll_lpfn");
        Parm *x2 = Getattr(p.n, "rp:loopParameterNode");
        String *loopParameterType = getTypeMap(x2, "rp_tm_xll_lppm");
        Printf(b_xlf_grp_cpp->b1, "        // BEGIN function emitLoopFunc\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        static XLOPER returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        %s::%sBind bindObject =\n", module, p.funcName);
        Printf(b_xlf_grp_cpp->b1, "            boost::bind((%s::%sSignature)\n", module, p.funcName);
        Printf(b_xlf_grp_cpp->b1, "                &%s::%s,\n", p.pname, p.name);
        Printf(b_xlf_grp_cpp->b1, "                xxx,\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_loop", "rp_tm_xll_loop2", 4, ',', true, true);
        Printf(b_xlf_grp_cpp->b1, "            );\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::loop\n");
        Printf(b_xlf_grp_cpp->b1, "            <%s::%sBind, %s, %s>\n", module, p.funcName, loopParameterType, loopFunctionType);
        Printf(b_xlf_grp_cpp->b1, "            (functionCall, bindObject, %s, returnValue);\n", loopParameterName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        return &returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        // END   function emitLoopFunc\n");
    }

    void functionWrapperImplMembImpl(ParmsMemb &p, String *funcName) {

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****MEMB*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT\n");
        emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtft");
        Printf(b_xlf_grp_cpp->b1, "%s(\n", funcName);
        emitParmList(p.parms2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", "rp_tm_xll_parm2");
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", "rp_tm_xll_cnvt2", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitTypeMap(b_xlf_grp_cpp->b1, p.node, "rp_tm_xxx_rp_get", 2);
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (String *loopParameterName = Getattr(p.n, "feature:rp:loopParameter")) {
            emitLoopFunc(p, loopParameterName);
            groupContainsLoopFunction = true;
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, p.n, "rp_tm_xll_rtdc", 2);
            Printf(b_xlf_grp_cpp->b1, "        xxx->%s(\n", p.name);
            emitParmList(p.parms, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", "rp_tm_xll_argf2", 3, ',', true, true);
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

    void functionWrapperImplMemb(ParmsMemb &p) {
        functionWrapperImplMembImpl(p, p.funcRename);
        if (p.alias)
            functionWrapperImplMembImpl(p, p.alias);
    }

    void clear() {

        if (groupContainsConstructor)
            Printf(b_xlf_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, pragmas_.groupName_);
        if (groupContainsClass) {
            if (pragmas_.automatic_) {
                Printf(b_xlf_grp_cpp->b0, "#include \"%s/objects/obj_%s.hpp\"\n", objInc, pragmas_.groupName_);
            } else {
                Printf(b_xlf_grp_cpp->b0, "#include \"%s/objects/objmanual_%s.hpp\"\n", objInc, pragmas_.groupName_);
            }
        }
        if (groupContainsLoopFunction) {
            Printf(b_xlf_grp_cpp->b0, "#include <rpxl/loop.hpp>\n");
            Printf(b_xlf_grp_cpp->b0, "#include \"%s/loop/loop_%s.hpp\"\n", objInc, pragmas_.groupName_);
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

        excelRegister(b_xlr_grp_cpp->b0, p.n, p.funcName, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        excelUnregister(b_xlr_grp_cpp->b1, p.n, p.funcName, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);

        count_.functions++;
        count_.total2++;
    }

    void functionWrapperImplCtor(ParmsCtor &p) {

        excelRegister(b_xlr_grp_cpp->b0, p.n, p.funcRename, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        excelUnregister(b_xlr_grp_cpp->b1, p.n, p.funcRename, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        if (p.alias) {
            excelRegister(b_xlr_grp_cpp->b0, p.n, p.alias, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
            excelUnregister(b_xlr_grp_cpp->b1, p.n, p.alias, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        }

        count_.constructors++;
        count_.total2++;
    }

    void functionWrapperImplMemb(ParmsMemb &p) {
        excelRegister(b_xlr_grp_cpp->b0, p.n, p.funcRename, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        excelUnregister(b_xlr_grp_cpp->b1, p.n, p.funcRename, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        if (p.alias) {
            excelRegister(b_xlr_grp_cpp->b0, p.n, p.alias, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
            excelUnregister(b_xlr_grp_cpp->b1, p.n, p.alias, Char(p.docStr.c_str()), p.parms2, pragmas_.groupFunctionWizard_);
        }

        count_.members++;
        count_.total2++;
    }

    void clear() {

        Printf(b_xlr_grp_cpp->b0, "}\n");
        Printf(b_xlr_grp_cpp->b1, "}\n");

        b_xlr_grp_cpp->clear(count_);
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

        b_lib_add_hpp = new Buffer("b_lib_add_hpp", NewStringf("%s/objects/obj_all.hpp", objDir));

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
                Printf(b_lib_add_hpp->b0, "#include <%s/objects/obj_%s.hpp>\n", objInc, group->pragmas_.groupName_);
            } else {
                Printf(b_lib_add_hpp->b0, "#include <%s/objects/objmanual_%s.hpp>\n", objInc, group->pragmas_.groupName_);
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

struct AddinDoxygen : public AddinImpl<GroupDoxygen> {

    Buffer *b_dox_add_all;
    std::vector<std::string> v;

    AddinDoxygen() : AddinImpl("Doxygen") {}

    virtual void top() {

        b_dox_add_all = new Buffer("b_dox_add_all", NewStringf("%s/functions_00_all.dox", doxDir));

        Printf(b_dox_add_all->b0, "\n");
        Printf(b_dox_add_all->b0, "/*! \\page func_all Functions - All\n");
        Printf(b_dox_add_all->b0, "\n");
        // Start writing to b1 instead of b0.  Later we will append more text to b0.
        Printf(b_dox_add_all->b1, "\\section functions Function List\n");
        Printf(b_dox_add_all->b1, "\n");
    }

    virtual void functionWrapperImplFunc(ParmsFunc &p) {
        AddinImpl::functionWrapperImplFunc(p);
        v.push_back(Char(p.funcName));
    }

    virtual void functionWrapperImplCtor(ParmsCtor &p) {
        if (generateCtor) {
            AddinImpl::functionWrapperImplCtor(p);
            v.push_back(Char(p.funcRename));
        }
    }

    virtual void functionWrapperImplMemb(ParmsMemb &p) {
        AddinImpl::functionWrapperImplMemb(p);
        v.push_back(Char(p.funcRename));
    }

    virtual void clear() {

        AddinImpl::clear();

        std::sort(v.begin(), v.end());

        std::vector<std::string>::const_iterator i0;
        for (i0=v.begin(); i0!=v.end(); ++i0) {
            Printf(b_dox_add_all->b1, "\\ref %s ()\\n\n", i0->c_str());
        }

        Printf(b_dox_add_all->b1, "*/\n");
        Printf(b_dox_add_all->b1, "\n");

        // Append the function count to b0
        Printf(b_dox_add_all->b0, "Below is an alphabetical list of links to documentation for all functions (%d) in %%%s.\n", v.size(), module);
        Printf(b_dox_add_all->b0, "\n");

        b_dox_add_all->clear(count);

        // statistics

        printf("%s - Addin Files", name_.c_str());
        printf("%s\n", std::string(66-name_.length(), '=').c_str());
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
        } else if ( (strcmp(argv[i],"-genxll") == 0)) {
            addinList_.appendAddin(new AddinExcelFunctions);
            addinList_.appendAddin(new AddinExcelRegister);
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
        } else if ( (strcmp(argv[i],"-legacy") == 0)) {
            legacy = true;
            Swig_mark_arg(i);
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
        if (String *s = getNode(n3, "rp_obj_dir"))
            objDir = s;
        if (String *s = getNode(n3, "rp_add_dir"))
            addDir = s;
        if (String *s = getNode(n3, "rp_xll_dir"))
            xllDir = s;
        if (String *s = getNode(n3, "rp_obj_inc"))
            objInc = s;
        if (String *s = getNode(n3, "rp_add_inc"))
            addInc = s;
        if (String *s = getNode(n3, "rp_xll_inc"))
            xllInc = s;
        if (String *s = getNode(n3, "rp_doc_str", true))
            docStr = s;
        if (String *s = getNode(n3, "rp_dox_dir", true))
            doxDir = s;
    }

    printf("module=%s\n", Char(module));
    printf("addinCppNameSpace=%s\n", Char(addinCppNameSpace));
    printf("rp_obj_dir=%s\n", Char(objDir));
    printf("rp_add_dir=%s\n", Char(addDir));
    printf("rp_xll_dir=%s\n", Char(xllDir));
    printf("rp_obj_inc=%s\n", Char(objInc));
    printf("rp_add_inc=%s\n", Char(addInc));
    printf("rp_xll_inc=%s\n", Char(xllInc));
    printf("rp_doc_str=%s\n", Char(docStr));
    printf("rp_dox_dir=%s\n", Char(doxDir));

    //if (doxDir)
    //    addinList_.appendAddin(new AddinDoxygen);

    if (docStr)
        initializeDocStrings();

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

    List *x = Keys(errorList);
    for (int i=0; i<Len(x); ++i) {
        String *errorMessage = Getitem(x, i);
        printf("%s", Char(errorMessage));
    }
    Delete(x);
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
            } else if (0 == Strcmp(name, "groupCaption")) {
                pragmas_.displayName_ = NewString(value);
            } else if (0 == Strcmp(name, "groupFunctionWizard")) {
                pragmas_.groupFunctionWizard_ = NewString(value);
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

Parm *createParm(const char *name, const char *type, bool constRef, bool hidden, const char *docStr) {
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
    if (docStr)
        Setattr(ret, "rp_docstr", Char(docStr));
    processParm(ret);
    return ret;
}

ParmList *prependParm(ParmList *parms, const char *name, const char *type, bool constRef = true, bool hidden = false, const char *docStr = 0) {
    Parm *ret = createParm(name, type, constRef, hidden, docStr);
    Setattr(ret, "nextSibling", parms);
    return ret;
}

ParmList *appendParm(ParmList *parms, const char *name, const char *type, bool constRef = true, bool hidden = false, const char *docStr = 0) {
    Parm *p = createParm(name, type, constRef, hidden, docStr);
    if (parms && ParmList_len(parms)) {
        Parm *lastParm;
        for (Parm *p2 = parms; p2; p2 = nextSibling(p2))
            lastParm = p2;
        Setattr(lastParm, "nextSibling", p);
        return parms;
    } else {
        return p;
    }
}

// Validate the function name.
// For the moment, the only test that we perform is to check whether the function name
// clashes with Excel range names.  It is possible that a function name could pass this
// test but still be invalid for other reasons.
// Excel cell names lie in the range A1 - XFD1048576.
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
    p.name = Getattr(n,"name");
    p.parms = Getattr(n,"parms");
    bool staticMember = 1 == checkAttribute(p.n, "globalfunctionHandler:view", "staticmemberfunctionHandler");
    if (staticMember)
        p.symname = copyUpper3(Getattr(n, "sym:name"));
    else
        p.symname = Getattr(n, "sym:name");

    String *tempx1 = Getattr(p.n, "feature:rp:rename2");
    if (!tempx1)
        tempx1 = p.symname;
    String *tempx2 = copyUpper(tempx1);
    p.funcName = NewStringf("%s%s", prefix, tempx2);
    validateFunctionName(p.funcName);

    String *scope = 0;
    String *funcName = 0;
    if (staticMember) {
        Node *x=Getattr(n, "parentNode");
        scope = Getattr(x, "name");
        funcName = Getattr(n, "staticmemberfunctionHandler:name");
    } else {
        scope = nmspace;
        funcName = p.symname;
    }
    p.docStr = getDocString(Char(scope), Char(funcName), "");

    printf("p.name=%s\n", Char(p.name));
    printf("p.symname=%s\n", Char(p.symname));
    printf("p.funcName=%s\n", Char(p.funcName));
    printf("staticMember=%d\n", staticMember);
    Printf(b_init, "@@@ FUNC Name=%s\n", p.funcName);
    Printf(b_init, "&&& p.symname=%s\n", p.symname);
    Printf(b_init, "&&& prefix=%s\n", prefix);
    Printf(b_init, "&&& p.funcName=%s\n", p.funcName);

    for (Parm *x = p.parms; x; x = nextSibling(x)) {
        std::string docStr = getDocString(Char(scope), Char(funcName), Char(Getattr(x, "name")));
        Setattr(x, "rp_docstr", Char(docStr.c_str()));
    }

    // Create from parms another list parms2 with argument Trigger.
    if (legacy) {
        // In legacy mode, append the argument to the list.
        p.parms2 = CopyParmList(p.parms);
        p.parms2 = appendParm(p.parms2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    } else {
        // For new projects, Trigger is the first argument.
        p.parms2 = prependParm(p.parms, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    }

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

    String *value = Getattr(p,"value");
    Setattr(p, "rp_value", value);

    String *nameUpper = copyUpper2(name);
    Setattr(p, "rp_name_upper", nameUpper);
}

int functionWrapperImplCtor(Node *n) {

    ParmsCtor p;

    p.n = n;
    p.name = Getattr(n, "name");
    p.parms = Getattr(n, "parms");
    Node *n1 = Getattr(n, "parentNode");
    p.pname = Getattr(n1, "name");

    p.base = 0;
    if (List *baseList = Getattr(n1, "baselist")) {
        p.multipleBaseClasses = Len(baseList) > 1;
        p.base = Getitem(baseList, 0);
        printf("base = %s\n", Char(p.base));
    } else {
        printf("no bases\n");
    }

    String *temp = copyUpper(p.name);
    p.funcName = NewStringf("%s%s", prefix, temp);
    String *tempx1 = Getattr(p.n, "feature:rp:rename2");
    if (!tempx1)
        tempx1 = Getattr(n, "constructorDeclaration:sym:name");
    String *tempx2 = copyUpper(tempx1);
    p.funcRename = NewStringf("%s%s", prefix, tempx2);
    validateFunctionName(p.funcRename);
    if (String *alias = Getattr(p.n, "feature:rp:alias"))
        p.alias = NewStringf("%s%s", prefix, alias);
    else
        p.alias = 0;
    printf("funcName=%s\n", Char(p.funcName));
    Printf(b_init, "@@@ CTOR Name=%s\n", Char(p.funcName));

    String *className = Getattr(Getattr(p.n, "parentNode"), "name");
    p.docStr = getDocString(Char(className), Char(p.name), "");
    if (p.docStr.empty()) {
        std::stringstream s;
        s << "Construct an object of class " << Char(p.name) << " and return its id";
        p.docStr = s.str();
    }

    for (Parm *x = p.parms; x; x = nextSibling(x)) {
        std::string docStr = getDocString(Char(className), Char(p.name), Char(Getattr(x, "name")));
        Setattr(x, "rp_docstr", Char(docStr.c_str()));
    }

    // Create from parms another list parms2 containing additional parameters.
    if (legacy) {
        // In legacy mode the signature is: (ObjectId, ..., Permanent, Trigger, Overwrite)
        p.parms2 = CopyParmList(p.parms);
        p.parms2 = appendParm(p.parms2, "Permanent", "bool", false, false, "Permanent flag");
        p.parms2 = appendParm(p.parms2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
        p.parms2 = appendParm(p.parms2, "Overwrite", "bool", false, false, "Overwrite flag");
        p.parms2 = prependParm(p.parms2, "objectID", "std::string", true, false, "Object ID");
    } else {
        // For new projects the signature is: (Trigger, ObjectId, Overwrite, Permanent, ...)
        Parm *temp1 = prependParm(p.parms, "Permanent", "bool", false, false, "Permanent flag");
        Parm *temp2 = prependParm(temp1, "Overwrite", "bool", false, false, "Overwrite flag");
        Parm *temp3 = prependParm(temp2, "objectID", "std::string", true, false, "Object ID");
        p.parms2 = prependParm(temp3, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    }

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
    p.funcName = NewStringf("%s%s%s", prefix, temp0, temp1);

    String *tempx1 = Getattr(p.n, "feature:rp:rename2");
    if (tempx1) {
        String *tempx2 = copyUpper(tempx1);
        p.funcRename = NewStringf("%s%s", prefix, tempx2);
    } else {
        tempx1 = Getattr(n, "memberfunctionHandler:sym:name");
        String *tempx2 = copyUpper(tempx1);
        p.funcRename = NewStringf("%s%s%s", prefix, temp0, tempx2);
    }
    validateFunctionName(p.funcRename);

    if (String *alias = Getattr(p.n, "feature:rp:alias"))
        p.alias = NewStringf("%s%s", prefix, alias);
    else
        p.alias = 0;

    printf("cls=%s\n", Char(cls));
    printf("p.name=%s\n", Char(p.name));
    printf("p.funcName=%s\n", Char(p.funcName));
    Printf(b_init, "@@@ MEMB Name=%s\n", Char(p.funcName));

    String *className = Getattr(Getattr(p.n, "parentNode"), "name");
    p.docStr = getDocString(Char(className), Char(p.name), "");

    for (Parm *x = p.parms; x; x = nextSibling(x)) {
        std::string docStr = getDocString(Char(className), Char(p.name), Char(Getattr(x, "name")));
        Setattr(x, "rp_docstr", Char(docStr.c_str()));
    }

    String *s = getTypeMap(p.n, "rp_tm_dox_rtd2");

    // Create from parms another list parms2 containing additional parameters.
    if (legacy) {
        // In legacy mode the signature is: (ObjectId, ..., Trigger)
        p.parms2 = CopyParmList(Getattr(p.parms, "nextSibling"));
        p.parms2 = appendParm(p.parms2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
        //p.parms2 = prependParm(p.parms2, "objectID", "std::string", true, false, "Object ID");
        p.parms2 = prependParm(p.parms2, "objectID", "std::string", true, false, Char(s));
    } else {
        // For new projects the signature is: (Trigger, ObjectId, ...)
        ParmList *parmsTemp = Getattr(p.parms, "nextSibling");
        Parm *parmsTemp2 = prependParm(parmsTemp, "objectID", "std::string", true, false, "Object ID");
        p.parms2 = prependParm(parmsTemp2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    }

    // We are invoking the member function of a class.
    // Create a dummy node and attach to it the type of the class.
    // This allows us to apply a typemap to the node.
    p.node = NewHash();
    Setfile(p.node, Getfile(p.n));
    Setline(p.node, Getline(p.n));
    Setattr(p.node, "type", p.pname);

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

// The user has indicated in the SWIG interface file that the current function should
// exhibit looping behavior (see the documentation for more details).
// Take a backup of the nodes for the function and for the loop parameter.
// Modify the two original nodes and change T to vector<T>.
// For most purposes the modified nodes are required (vector<T>).
// The backups of the original nodes may also be retrieved if necessary (T).
void processLoopParameter(Node *n, String *functionName, ParmList *parms, String *loopParameterName) {
    for (Parm *p=parms; p; p=nextSibling(p)) {
        String *name = Getattr(p, "name");
        if (0==Strcmp(loopParameterName, name)) {
            Setattr(n, "rp:loopParameterNode", CopyParm(p));
            SwigType *t = Getattr(p, "type");
            SwigType *t2 = SwigType_base(t);
            Parm *p2 = NewHash();
            Setattr(p2, "type", t2);
            SwigType *t3 = NewString("std::vector");
            SwigType_add_template(t3, p2);
            Setattr(p, "type", t3);

            Setattr(n, "rp:loopFunctionNode", Copy(n));
            SwigType *t4 = Getattr(n, "type");
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
        "' - you specified loop parameter '" << Char(loopParameterName) << "' " <<
        "but the function has no parameter with that name.");
}

void functionWrapperImplAll(Node *n) {
    String *nodeName = Getattr(n, "name");
    printf("Processing node name '%s'.\n", Char(nodeName));

    ParmList *parms  = Getattr(n, "parms");

    if (String *loopParameterName = Getattr(n, "feature:rp:loopParameter"))
        processLoopParameter(n, nodeName, parms, loopParameterName);

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

