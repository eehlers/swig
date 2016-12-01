
#include "swigmod.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <stdio.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

//*****************************************************************************
// GLOBAL VARIABLES
//*****************************************************************************

File *b_debug;

String *prefix = 0;

String *objDir = 0;
String *objInc = 0;
String *addDir = 0;
String *addInc = 0;
String *xllDir = 0;
String *xllInc = 0;
String *doxDir = 0;
String *docStr = 0;

long idNum = 4;

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

File *initFile(String *outfile) {
    File *f = NewFile(outfile, "w", SWIG_output_files());
    if (!f) {
        FileErrorDisplay(outfile);
        REPOSIT_SWIG_FAIL("Error initializing file '" << Char(outfile) << "'");
    }
    return f;
}

int paramListSize(ParmList *parms) {
    int ret = 0;
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (Getattr(p, "hidden")) continue;
        ret++;
    }
    return ret;
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

void processParm(Parm *p) {

    String *name = Getattr(p, "name");
    REPOSIT_SWIG_REQUIRE(name, "parameter has no name");

    SwigType *t = Getattr(p, "type");
    REPOSIT_SWIG_REQUIRE(t, "parameter '" << Char(name) << "' has no type");

    String *value = Getattr(p, "value");
    Setattr(p, "rp_value", value);

    String *nameUpper = copyUpper2(name);
    Setattr(p, "rp_name_upper", nameUpper);
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
    // 1-7 digits.  At this point the only way the string could not clash with Excel is if it
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

//*****************************************************************************
// EMITTING PARAMETER LISTS
//*****************************************************************************

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

//*****************************************************************************
// STACK
//*****************************************************************************

struct Stack {

    String *module_;
    String *groupName_;
    String *groupCaption_;
    String *namespace_;
    String *obj_cpp_;
    bool automatic_;
    bool active_;

    Stack() : module_(0), groupName_(0), groupCaption_(0), namespace_(0), obj_cpp_(0), automatic_(true), active_(false) {}

    void setModule(String *module) {
        module_ = module;
    }

    void setGroupName(String *groupName) {
        groupName_ = groupName;
    }

    void initializeGroup(String *groupCaption) {
        groupCaption_ = groupCaption;
        active_ = true;
        obj_cpp_ = NewString("");
        Swig_register_filebyname("obj_cpp", obj_cpp_);
    }

    void setNamespace(String *ns) {
        namespace_ = ns;
    }

    void setAutomatic(bool automatic) {
        automatic_ = automatic;
    }

    bool active() { return active_; }

    void clearGroup() {
        groupName_ = 0;
        groupCaption_ = 0;
        Delete(obj_cpp_);
        obj_cpp_=0;
        automatic_ = true;
        active_ = false;
    }
};
Stack stack;

//*****************************************************************************
// REPORT
//*****************************************************************************

// The Report struct emits to stdout a summary of all source code files generated.

const char* STATUS_ENUM_TO_STRING[] = { "Unchanged", "Updated", "Created" };
const int LINE_WIDTH=80;

struct Report {

    struct File {
        enum Type { Section, Group };
        enum Status { Unchanged, Updated, Created, Total };
        Status status_;
        File() : status_(Total) {}
        File(Status status) : status_(status) {}
    };

    struct Section {
        enum FunctionType { Constructor, Member, Function, Total };
        std::map<std::string, File> sectionFiles_;
        std::map<std::string, File> groupFiles_;
        std::map<File::Status, int> fileTotals_;
        std::map<Section::FunctionType, int> functionTotals_;
    };

    std::map<std::string, Section*> sections_;
    std::vector<std::string> sectionKeys_;
    std::map<File::Status, int> fileTotals_;

    void addSection(const std::string &name) {
        Section *section = new Section();
        sections_[name]=section;
        sectionKeys_.push_back(name);
    }

    void addFile(const std::string &sectionName, const std::string &fileName, File::Type type, File::Status status) {
        if (File::Section == type)
            sections_[sectionName]->sectionFiles_[fileName]=File(status);
        else
            sections_[sectionName]->groupFiles_[fileName]=File(status);
        sections_[sectionName]->fileTotals_[status]++;
        sections_[sectionName]->fileTotals_[File::Total]++;
        fileTotals_[status]++;
        fileTotals_[File::Total]++;
    }

    void countFunction(const std::string &sectionName, Section::FunctionType functionType) {
        sections_[sectionName]->functionTotals_[functionType]++;
        sections_[sectionName]->functionTotals_[Section::Total]++;
    }

    void printLine() {
        printf("%s\n", std::string(LINE_WIDTH, '=').c_str());
    }

    void printHeader(const std::string &s) {
        printf("%s", s.c_str());
        printf("%s\n", std::string(LINE_WIDTH-s.length(), '=').c_str());
    }

    void printFile(const std::string &s, File::Status status) {
        const int MAX_HEADER_WIDTH=70;
        const char *header = s.c_str();
        const char *stat = STATUS_ENUM_TO_STRING[status];

        if (Len(header)<MAX_HEADER_WIDTH) {
            printf("%s", Char(header));
        } else {
            printf("%s", Char(header) + (Len(header) - MAX_HEADER_WIDTH));
        }
        printf("%s", std::string(LINE_WIDTH - Len(header) - Len(stat), '.').c_str());
        printf("%s\n", stat);
    }

    void printFiles(const std::map<std::string, File> &m) {
        for (std::map<std::string, File>::const_iterator f = m.begin(); f!=m.end(); f++) {
            std::string fileName = f->first;
            File file = f->second;
            printFile(fileName, file.status_);
        }
    }

    void print() {
        for (std::vector<std::string>::const_iterator i = sectionKeys_.begin(); i!=sectionKeys_.end(); i++) {
            std::string sectionName = *i;
            Section *section = sections_[sectionName];
            printHeader(sectionName + " - Section Files");
            printFiles(section->sectionFiles_);
            printHeader(sectionName + " - Group Files");
            printFiles(section->groupFiles_);
        }
        printLine();

        printf("Function Count                   Constructor      Member    Function       Total\n");
        printLine();
        for (std::vector<std::string>::const_iterator i = sectionKeys_.begin(); i!=sectionKeys_.end(); i++) {
            std::string sectionName = *i;
            Section *section = sections_[sectionName];
            printf("%s", sectionName.c_str());
            printf("%s", std::string(32-sectionName.length(), ' ').c_str());
            printf("%12d%12d%12d%12d\n",
                section->functionTotals_[Section::Constructor], section->functionTotals_[Section::Member], section->functionTotals_[Section::Function], section->functionTotals_[Section::Total]);
        }
        printLine();

        printf("File Count                         Unchanged     Updated     Created       Total\n");
        printLine();
        for (std::vector<std::string>::const_iterator i = sectionKeys_.begin(); i!=sectionKeys_.end(); i++) {
            std::string sectionName = *i;
            Section *section = sections_[sectionName];
            printf("%s", sectionName.c_str());
            printf("%s", std::string(32-sectionName.length(), ' ').c_str());
            printf("%12d%12d%12d%12d\n",
                section->fileTotals_[File::Unchanged], section->fileTotals_[File::Updated], section->fileTotals_[File::Created], section->fileTotals_[File::Total]);
        }
        printLine();
        printf("Total");
        printf("%s", std::string(27, ' ').c_str());
        printf("%12d%12d%12d%12d\n",
                fileTotals_[File::Unchanged], fileTotals_[File::Updated], fileTotals_[File::Created], fileTotals_[File::Total]);
        printLine();

    }
};
Report report;

//*****************************************************************************
// BUFFER
//*****************************************************************************

struct Buffer {

    std::string sectionName_;
    std::string fileName_;
    Report::File::Type type_;
    String *path_;
    bool active_;
    File *b0;
    File *b1;
    File *b2;
    File *b3;

    Buffer(const std::string &sectionName, const std::string &fileName, Report::File::Type type, String *path, bool active=true) :
        sectionName_(sectionName),
        fileName_(fileName),
        type_(type),
        path_(path),
        active_(active),
        b0(NewString("")),
        b1(NewString("")),
        b2(NewString("")),
        b3(NewString(""))
    {
        Printf(b0, "\n");
        Printf(b0, "// BEGIN buffer %s\n", fileName_.c_str());
        Printf(b0, "\n");
    }

    void activate() { active_=true; }
        
    bool fileExists() {
#ifdef WIN32
        return (-1 != _access(Char(path_), 0));
#else
        return (-1 != access(Char(path_), F_OK));
#endif
    }

    void writeFile(String *buf, String *path) {
        File *f = initFile(path);
        Dump(buf, f);
        Delete(f);
    }

    // Write the file to disk.  Here is the pseudocode/logic:
    //
    //  If the file exists already {
    //     If the file has changed {
    //          If a *.old file exists {
    //              do not overwrite the *.old file
    //          } else no *.old file {
    //              write the old buffer to *.old
    //          }
    //          write new file : status=UPDATED
    //      } else {
    //          do nothing : status=UNCHANGED
    //      }
    //  } else file does not exist already {
    //      write new file : status=CREATED
    //  }
    //
    // During development, keep the *.old files around as a basis for comparison.
    // They will not be overwritten.
    // When development is done, delete the *.old files.
    // If they get generated again it means that something has changed.

    void flush() {

        Printf(b3, "\n");
        Printf(b3, "// END buffer %s\n", fileName_.c_str());
        Printf(b3, "\n");

        String *buf = NewString("");

        Dump(b0, buf);
        Dump(b1, buf);
        Dump(b2, buf);
        Dump(b3, buf);

        String *backupPath = Copy(path_);
        Append(backupPath, ".old");

        if (fileExists()) {

            String *oldBuf = 0;
            bool fileChanged = false;

            if (FILE *f = Swig_open(path_)) {
                oldBuf = Swig_read_file(f);
                if (0!=Strcmp(oldBuf, buf))
                    fileChanged = true;
                fclose(f);
            }

            if (fileChanged) {

                if (FILE *f = Swig_open(backupPath))
                    fclose(f);
                else
                    writeFile(oldBuf, backupPath);

                writeFile(buf, path_);
                report.addFile(sectionName_, Char(path_), type_, Report::File::Updated);

            } else {
                report.addFile(sectionName_, Char(path_), type_, Report::File::Unchanged);
            }

            Delete(oldBuf);

        } else {
            writeFile(buf, path_);
            report.addFile(sectionName_, Char(path_), type_, Report::File::Created);
        }

        // If the *.old file is the same as the original then delete it.
        bool deleteOldFile = false;
        if (FILE *f = Swig_open(backupPath)) {
            String *oldBuf = Swig_read_file(f);
            if (0==Strcmp(oldBuf, buf))
                deleteOldFile = true;
            Delete(oldBuf);
            fclose(f);
        }
        if (deleteOldFile)
            std::remove(Char(backupPath));

        Delete(backupPath);
        Delete(buf);
    }

    ~Buffer() {
        if (active_)
            flush();
        Delete(b0);
        Delete(b1);
        Delete(b2);
        Delete(b3);
        Delete(path_);
    }
};

//*****************************************************************************
// GROUPS
//*****************************************************************************

// Addin functions are organized into groups.
// The Group struct generates one hpp file and/or one cpp file
// for all of the functions in a given group, for a given Section of the Addin.

struct Group {

    std::string sectionName_;
    String *groupName_;
    bool active_;

    Group(const std::string &sectionName, String *groupName) : sectionName_(sectionName), groupName_(groupName), active_(false) {}
    virtual ~Group() {}
    virtual void classHandlerBefore() {}
    virtual void classHandlerAfter() {}
    virtual void functionWrapperCtor(Node*) {}
    virtual void functionWrapperMember(Node*) {}
    virtual void functionWrapperFunc(Node*) {}
};

struct GroupLibraryObjects : public Group {

    Buffer *b_lib_grp_hpp;
    Buffer *b_lib_grp_cpp;
    Buffer *b_lib_loop_hpp;
    bool needClassBegin;
    bool needClassEnd;
    String *rp_class;
    String *rp_namespace;

    GroupLibraryObjects(const std::string &sectionName, String *groupName) :
        Group(sectionName, groupName),
        b_lib_grp_hpp(0), b_lib_grp_cpp(0), b_lib_loop_hpp(0),
        needClassBegin(false), needClassEnd(false),
        rp_class(0)
    {
        rp_namespace = NewString("");
        Swig_register_filebyname("rp_namespace", rp_namespace);

        if (stack.automatic_) {
            b_lib_grp_hpp = new Buffer(sectionName, "b_lib_grp_hpp", Report::File::Group, NewStringf("%s/objects/obj_%s.hpp", objDir, groupName_), false);
            b_lib_grp_cpp = new Buffer(sectionName, "b_lib_grp_cpp", Report::File::Group, NewStringf("%s/objects/obj_%s.cpp", objDir, groupName_), false);
            Swig_register_filebyname("obj_hpp", b_lib_grp_hpp->b1);
        } else {
            b_lib_grp_hpp = new Buffer(sectionName, "b_lib_grp_hpp", Report::File::Group, NewStringf("%s/objects/objmanual_%s.hpp.template", objDir, groupName_), false);
            b_lib_grp_cpp = 0;
            Swig_register_filebyname("obj_hpp", b_lib_grp_hpp->b1);
        }
        b_lib_loop_hpp = new Buffer(sectionName, "b_lib_loop_hpp", Report::File::Group, NewStringf("%s/loop/loop_%s.hpp", objDir, groupName_), false);

        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#ifndef obj_%s_hpp\n", groupName_);
        Printf(b_lib_grp_hpp->b0, "#define obj_%s_hpp\n", groupName_);
        Printf(b_lib_grp_hpp->b0, "\n");
        Printf(b_lib_grp_hpp->b0, "#include <string>\n");
        Printf(b_lib_grp_hpp->b0, "#include <rp/libraryobject.hpp>\n");
        Printf(b_lib_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_lib_grp_hpp->b0, "#include <boost/shared_ptr.hpp>\n");

        Printf(b_lib_grp_hpp->b2, "\n");
        Printf(b_lib_grp_hpp->b2,"namespace %s {\n", stack.module_);
        Printf(b_lib_grp_hpp->b2, "\n");

        if (stack.automatic_) {
            Printf(b_lib_grp_cpp->b0, "\n");
            Printf(b_lib_grp_cpp->b0, "#include <%s/objects/obj_%s.hpp>\n", objInc, groupName_);
            Printf(b_lib_grp_cpp->b0, "\n");
        }

        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "#ifndef loop_%s_hpp\n", groupName_);
        Printf(b_lib_loop_hpp->b0, "#define loop_%s_hpp\n", groupName_);
        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "#include <boost/bind.hpp>\n");
        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "namespace %s {\n", stack.module_);
    }

    void classHandlerBefore() {
        needClassBegin = true;
        needClassEnd = false;
        rp_class = NewString("");
        Swig_register_filebyname("rp_class", rp_class);
    }

    void classHandlerAfter() {
        if (needClassEnd) {
            Append(b_lib_grp_hpp->b2, rp_class);
            Printf(b_lib_grp_hpp->b2, "    };\n");
        }
        needClassBegin = false;
        needClassEnd = false;
        Delete(rp_class);
        rp_class=0;
    }

    void emitClassBegin(String *className, String *baseClassName) {
        Printf(b_lib_grp_hpp->b2, "\n");
        Printf(b_lib_grp_hpp->b2, "    class %s : \n", className);
        Printf(b_lib_grp_hpp->b2, "        public %s {\n", baseClassName);
        Printf(b_lib_grp_hpp->b2, "    public:\n");
        needClassBegin = false;
        needClassEnd = true;
    }

    void functionWrapperCtor(Node *n) {

        String *className = Getattr(n, "name");
        Node *p = Getattr(n, "parentNode");

        String *parentName = Getattr(p, "name");
        String *baseClass = 0;
        if (Node *l=Getattr(p, "baselist"))
            baseClass = Getitem(l, 0);

        String *baseClass2 = 0;
        String *wrapClass = 0;

        if (baseClass) {
            //// Autogeneration of object wrapper code is not supported for multiple inheritance.
            //REPOSIT_SWIG_REQUIRE(!stack.automatic_ || !p.multipleBaseClasses,
            //    "Class '" << Char(p.name) << "' has multiple base classes.\n"
            //    "Autogeneration of object wrapper code is not supported for multiple inheritance.\n"
            //    "Use the %override directive to suppress autogeneration, and implement the code manually.");
            baseClass2 = baseClass;
            wrapClass = NewStringf("%s::%s", stack.namespace_, baseClass);
        } else {
            baseClass2 = NewStringf("reposit::LibraryObject<%s>", parentName);
            wrapClass = parentName;
        }

        // If the user has set the %noctor directive for the class (the parent node) then do nothing.
        if (checkAttribute(p, "feature:rp:noctor", "1"))
            return;

        if (Getattr(n, "default_constructor")) {

            Printf(b_lib_grp_hpp->b2, "    // BEGIN typemap rp_tm_lib_cls\n");
            if (baseClass) {
                Printf(b_lib_grp_hpp->b2, "    RP_OBJ_CLASS(%s, %s);\n", className, baseClass);
            } else {
                Printf(b_lib_grp_hpp->b2, "    RP_LIB_CLASS(%s, %s);\n", className, parentName);
            }
            Printf(b_lib_grp_hpp->b2, "    // END   typemap rp_tm_lib_cls\n");

        } else {

            if (needClassBegin)
                emitClassBegin(className, baseClass2);

            Printf(b_lib_grp_hpp->b2, "\n");
            Printf(b_lib_grp_hpp->b2,"        //****CTOR*****\n");
            Printf(b_lib_grp_hpp->b2,"        %s(\n", className);
            Printf(b_lib_grp_hpp->b2,"            const boost::shared_ptr<reposit::ValueObject>& properties,\n");
            emitParmList(Getattr(n, "parms"), b_lib_grp_hpp->b2, 2, "rp_tm_default", "rp_tm_default", 3, ',', true, false, true);
            Printf(b_lib_grp_hpp->b2,"            bool permanent);\n");

            if (stack.automatic_) {
                Printf(b_lib_grp_cpp->b2,"\n");
                if (checkAttribute(n, "feature:rp:noimpl", "1"))
                    Printf(b_lib_grp_cpp->b2,"/* noimpl\n");
                Printf(b_lib_grp_cpp->b2,"%s::%s::%s(\n", stack.module_, className, className);
                Printf(b_lib_grp_cpp->b2,"    const boost::shared_ptr<reposit::ValueObject>& properties,\n");
                emitParmList(Getattr(n, "parms"), b_lib_grp_cpp->b2, 2, "rp_tm_default", "rp_tm_default", 3, ',', true, false, true);
                Printf(b_lib_grp_cpp->b2,"    bool permanent)\n");
                Printf(b_lib_grp_cpp->b2,": %s(properties, permanent) {\n", baseClass2);
                Printf(b_lib_grp_cpp->b2,"    libraryObject_ = boost::shared_ptr<%s>(new %s(\n", wrapClass, parentName);
                emitParmList(Getattr(n, "parms"), b_lib_grp_cpp->b2, 0, "rp_tm_default", "rp_tm_default", 4);
                Printf(b_lib_grp_cpp->b2,"    ));\n");
                Printf(b_lib_grp_cpp->b2,"}\n");
                if (checkAttribute(n, "feature:rp:noimpl", "1"))
                    Printf(b_lib_grp_cpp->b2,"noimpl */\n");
                Printf(b_lib_grp_cpp->b2,"\n");
                b_lib_grp_cpp->activate();
            }
        }

        report.countFunction(sectionName_, Report::Section::Constructor);
        b_lib_grp_hpp->activate();
    }

    // Returns true if the given member function has the const qualifier
    bool memberFunctionIsConst(Node *n) {
        // Grab the first parameter of the member function, it's a reference to "this".
        Parm *firstParameter  = Getattr(n, "parms");
        // Take a copy of the type since we're going to modify it.
        SwigType *t = Copy(Getattr(firstParameter, "type"));
        // Pop off the reference.
        SwigType_pop(t);
        // Return true if the type of the member function ("this") is const.
        return SwigType_isconst(t);
    }

    void functionWrapperMember(Node *n) {

        String *funcName = Getattr(n, "name");
        Node *p = Getattr(n, "parentNode");
        String *className = Getattr(p, "sym:name");

        ParmList *parms = Getattr(n, "parms");
        ParmList *parms2 = Getattr(parms, "nextSibling");
        String *parentName = Getattr(p, "name");

        if (0==Strcmp(stack.namespace_, stack.module_) || checkAttribute(n, "feature:rp:wrap", "1")) {

            String *baseClass = 0;
            if (Node *l=Getattr(p, "baselist"))
                baseClass = Getitem(l, 0);

            String *baseClass2 = 0;
            if (baseClass) {
                // Autogeneration of object wrapper code is not supported for multiple inheritance.
                //REPOSIT_SWIG_REQUIRE(!stack.automatic_ || !p.multipleBaseClasses,
                //    "Class '" << Char(p.name) << "' has multiple base classes.\n"
                //    "Autogeneration of object wrapper code is not supported for multiple inheritance.\n"
                //    "Use the %override directive to suppress autogeneration, and implement the code manually.");
                baseClass2 = baseClass;
            } else {
                baseClass2 = NewStringf("reposit::LibraryObject<%s>", parentName);
            }

            if (needClassBegin)
                emitClassBegin(className, baseClass2);

            Printf(b_lib_grp_hpp->b2, "\n");
            Printf(b_lib_grp_hpp->b2,"        //****MEMBER*****\n");
            Printf(b_lib_grp_hpp->b2,"        %s\n", SwigType_str(Getattr(n, "type"), 0));
            Printf(b_lib_grp_hpp->b2,"        %s(\n", funcName);
            emitParmList(parms2, b_lib_grp_hpp->b2, 2, "rp_tm_default", "rp_tm_default", 3);
            Printf(b_lib_grp_hpp->b2,"        )");
            if (memberFunctionIsConst(n))
                Printf(b_lib_grp_hpp->b2, " const");
            Printf(b_lib_grp_hpp->b2, ";\n");
            b_lib_grp_hpp->activate();
        }

        if (String *loopParameterName = Getattr(n, "feature:rp:loopParameter")) {

            String *funcName2 = Getattr(n, "rp:funcName");
            Node *loopFunctionNode = Getattr(n, "rp:loopFunctionNode");
            Node *memberType = Getattr(n, "rp:memberType");
            bool isconst = SwigType_isconst(Getattr(n, "decl"));
            int i = paramListSize(parms);

            String *loopFunctionType = getTypeMap(n, "rp_tm_xll_lpfn");
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "    // %s\n", funcName2);
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "    typedef     boost::_bi::bind_t<\n");
            Printf(b_lib_loop_hpp->b0, "                %s,\n", loopFunctionType);
            if (isconst)
                Printf(b_lib_loop_hpp->b0, "                boost::_mfi::cmf%d<\n", i);
            else
                Printf(b_lib_loop_hpp->b0, "                boost::_mfi::mf%d<\n", i);
            Printf(b_lib_loop_hpp->b0, "                    %s,\n", loopFunctionType);
            Printf(b_lib_loop_hpp->b0, "                    %s,\n", parentName);
            bool first = true;
            for (Parm *p=parms; p; p=nextSibling(p)) {
                if (Getattr(p, "hidden"))
                    continue;
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                Printf(b_lib_loop_hpp->b0, "                    %s", SwigType_str(Getattr(p, "type"), 0));
            }
            Printf(b_lib_loop_hpp->b0, ">,\n");

            Printf(b_lib_loop_hpp->b0, "                boost::_bi::list%d<\n", paramListSize(parms)+1);
            Printf(b_lib_loop_hpp->b0, "                    boost::_bi::value<%s >,\n", getTypeMap(memberType, "rp_tm_lib_loop"));
            first = true;
            for (Parm *p=parms; p; p=nextSibling(p)) {
                if (Getattr(p, "hidden"))
                    continue;
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(p, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                    boost::arg<1>");
                } else {
                    Printf(b_lib_loop_hpp->b0, "                    boost::_bi::value<%s>", SwigType_str(SwigType_base(Getattr(p, "type")), 0));
                }
            }
            Printf(b_lib_loop_hpp->b0, " > >\n");
            Printf(b_lib_loop_hpp->b0, "                    %sBind;\n", funcName2);
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "    typedef     %s\n", loopFunctionType);
            Printf(b_lib_loop_hpp->b0, "                (%s::* %sSignature)(\n", parentName, funcName2);
            first = true;
            for (Parm *p=parms; p; p=nextSibling(p)) {
                if (Getattr(p, "hidden"))
                    continue;
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(p, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                    %s", getTypeMap(p, "rp_tm_xll_lppm2"));
                } else {
                    Printf(b_lib_loop_hpp->b0, "                    %s", SwigType_str(Getattr(p, "type"), 0));
                }
            }
            if (isconst)
                Printf(b_lib_loop_hpp->b0, ") const;\n");
            else
                Printf(b_lib_loop_hpp->b0, ");\n");
            Printf(b_lib_loop_hpp->b0, "\n");
            b_lib_loop_hpp->activate();
        }

        report.countFunction(sectionName_, Report::Section::Member);
    }

    void functionWrapperFunc(Node *n) {

        if (0==Strcmp(stack.namespace_, stack.module_)) {

            ParmList *parms = Getattr(n, "parms");
            String *symName = Getattr(n, "rp:symName");

            Printf(b_lib_grp_hpp->b2,"    //****FUNC*****\n");
            Printf(b_lib_grp_hpp->b2,"    %s\n", SwigType_str(Getattr(n, "type"), 0));
            Printf(b_lib_grp_hpp->b2,"    %s(\n", symName);
            emitParmList(parms, b_lib_grp_hpp->b2, 2, "rp_tm_default", "rp_tm_default", 2);
            Printf(b_lib_grp_hpp->b2,"        );\n");
            Printf(b_lib_grp_hpp->b2,"\n");
            b_lib_grp_hpp->activate();
        }

        if (String *loopParameterName = Getattr(n, "feature:rp:loopParameter")) {

            ParmList *parms = Getattr(n, "parms");
            String *funcName2 = Getattr(n, "rp:funcName");
            String *loopFunctionType = getTypeMap(n, "rp_tm_xll_lpfn");
            int i = paramListSize(parms);

            Printf(b_lib_loop_hpp->b0, "    // %s\n", funcName2);
            Printf(b_lib_loop_hpp->b0, "\n");
            Printf(b_lib_loop_hpp->b0, "        typedef     boost::_bi::bind_t<\n");
            Printf(b_lib_loop_hpp->b0, "                    %s,\n", loopFunctionType);
            Printf(b_lib_loop_hpp->b0, "                    %s (__cdecl*)(\n", loopFunctionType);
            bool first = true;
            for (Parm *p=parms; p; p=nextSibling(p)) {
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }

                Printf(b_lib_loop_hpp->b0, "                        %s", SwigType_str(Getattr(p, "type"), 0));
            }
            Printf(b_lib_loop_hpp->b0, "),\n");
            Printf(b_lib_loop_hpp->b0, "                    boost::_bi::list%d<\n", i);
            first = true;
            for (Parm *p=parms; p; p=nextSibling(p)) {
                if (first) {
                    first = false;
                } else {
                    Printf(b_lib_loop_hpp->b0, ",\n");
                }
                if (0==Strcmp(loopParameterName, Getattr(p, "name"))) {
                    Printf(b_lib_loop_hpp->b0, "                        boost::arg<1>");
                } else {
                    Printf(b_lib_loop_hpp->b0, "                        boost::_bi::value<%s>", SwigType_str(SwigType_base(Getattr(p, "type")), 0));
                }
            }
            Printf(b_lib_loop_hpp->b0, " > >\n");
            Printf(b_lib_loop_hpp->b0, "                    %sBind;\n", funcName2);
            Printf(b_lib_loop_hpp->b0, "\n");
            b_lib_loop_hpp->activate();
        }

        report.countFunction(sectionName_, Report::Section::Function);
    }

    virtual ~GroupLibraryObjects() {

        Append(b_lib_grp_hpp->b2, rp_namespace);
        Printf(b_lib_grp_hpp->b2, "} // namespace %s\n", stack.module_);
        Printf(b_lib_grp_hpp->b2, "\n");
        Printf(b_lib_grp_hpp->b2, "#endif\n");

        Printf(b_lib_loop_hpp->b0, "}\n");
        Printf(b_lib_loop_hpp->b0, "\n");
        Printf(b_lib_loop_hpp->b0, "#endif\n");
        Printf(b_lib_loop_hpp->b0, "\n");

        delete(b_lib_grp_hpp);
        if (stack.automatic_) {
            Append(b_lib_grp_cpp->b1, stack.obj_cpp_);
            delete(b_lib_grp_cpp);
        }
        delete(b_lib_loop_hpp);
    }
};

struct GroupValueObjects : public Group {

    Buffer *b_vob_grp_hpp;
    Buffer *b_vob_grp_cpp;

    GroupValueObjects(const std::string &sectionName, String *groupName) : Group(sectionName, groupName),
        b_vob_grp_hpp(0), b_vob_grp_cpp(0) {

        b_vob_grp_hpp = new Buffer(sectionName, "b_vob_grp_hpp", Report::File::Group, NewStringf("%s/valueobjects/vo_%s.hpp", objDir, groupName_), false);
        b_vob_grp_cpp = new Buffer(sectionName, "b_vob_grp_cpp", Report::File::Group, NewStringf("%s/valueobjects/vo_%s.cpp", objDir, groupName_), false);

        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "#ifndef vo_%s_hpp\n", groupName_);
        Printf(b_vob_grp_hpp->b0, "#define vo_%s_hpp\n", groupName_);
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_vob_grp_hpp->b0, "#include <string>\n");
        Printf(b_vob_grp_hpp->b0, "#include <vector>\n");
        Printf(b_vob_grp_hpp->b0, "#include <set>\n");
        Printf(b_vob_grp_hpp->b0, "#include <boost/serialization/map.hpp>\n");
        Printf(b_vob_grp_hpp->b0, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0,"namespace %s {\n", stack.module_);
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "namespace ValueObjects {\n");
        Printf(b_vob_grp_hpp->b0, "\n");

        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, groupName_);
        Printf(b_vob_grp_cpp->b0, "#include <boost/algorithm/string/case_conv.hpp>\n");
        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0,"namespace %s {\n", stack.module_);
        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "namespace ValueObjects {\n");
        Printf(b_vob_grp_cpp->b0, "\n");
    }

    void voGetProp(File *f, ParmList *parms) {
        Printf(f, "            // BEGIN func voGetProp\n");
        for (Parm *p = parms; p; p = nextSibling(p)) {
            String *name = Getattr(p, "name");
            String *nameUpper = Getattr(p, "rp_name_upper");
            Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
            Printf(f, "                return %s_;\n", name);
        }
        Printf(f, "            // END   func voGetProp\n");
    }

    void voSetProp(File *f, ParmList *parms) {
        Printf(f, "            // BEGIN func voSetProp (using typemap rp_tm_vob_cnvt)\n");
        for (Parm *p = parms; p; p = nextSibling(p)) {
            String *name = Getattr(p, "name");
            String *nameUpper = Getattr(p, "rp_name_upper");
            String *cnv = getTypeMap(p, "rp_tm_vob_cnvt");
            Printf(f, "            else if(strcmp(nameUpper.c_str(), \"%s\")==0)\n", nameUpper);
            Printf(f, "                %s_ = %s;\n", name, cnv);
        }
        Printf(f, "            // END   func voSetProp (using typemap rp_tm_vob_cnvt)\n");
    }

    void functionWrapperCtor(Node *n) {

        String *funcRename = Getattr(n, "rp:funcRename");
        ParmList *parmList = Getattr(n, "parms");

        Printf(b_vob_grp_hpp->b0,"        class %s : public reposit::ValueObject {\n", funcRename);
        Printf(b_vob_grp_hpp->b0,"            friend class boost::serialization::access;\n");
        Printf(b_vob_grp_hpp->b0,"        public:\n");
        Printf(b_vob_grp_hpp->b0,"            %s() {}\n", funcRename);
        Printf(b_vob_grp_hpp->b0,"            %s(\n", funcRename);
        Printf(b_vob_grp_hpp->b0,"                const std::string& ObjectId,\n");
        emitParmList(parmList, b_vob_grp_hpp->b0, 2, "rp_tm_vob_parm", "rp_tm_vob_parm", 4, ',', true, false, true);
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
        emitParmList(parmList, b_vob_grp_hpp->b0, 1, "rp_tm_vob_mbvr", "rp_tm_vob_mbvr", 3, ';', true, false, true);
        Printf(b_vob_grp_hpp->b0,"            bool Permanent_;\n");
        if (String *processorName = Getattr(n, "feature:rp:processorName"))
            Printf(b_vob_grp_hpp->b0,"            virtual std::string processorName() { return \"%s\"; }\n", processorName);
        Printf(b_vob_grp_hpp->b0,"\n");
        Printf(b_vob_grp_hpp->b0,"            template<class Archive>\n");
        Printf(b_vob_grp_hpp->b0,"            void serialize(Archive& ar, const unsigned int) {\n");
        Printf(b_vob_grp_hpp->b0,"            boost::serialization::void_cast_register<%s, reposit::ValueObject>(this, this);\n", funcRename);
        Printf(b_vob_grp_hpp->b0,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"ClassName\", className_)\n");
        emitParmList(parmList, b_vob_grp_hpp->b0, 1, "rp_tm_vob_srmv", "rp_tm_vob_srmv", 5, 0);
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
        Printf(b_vob_grp_hpp->b0,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
        Printf(b_vob_grp_hpp->b0,"            }\n");
        Printf(b_vob_grp_hpp->b0,"        };\n");
        Printf(b_vob_grp_hpp->b0,"\n");

        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        const char* %s::mPropertyNames[] = {\n", funcRename);
        emitParmList(parmList, b_vob_grp_cpp->b0, 1, "rp_tm_vob_name", "rp_tm_vob_name", 3, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"            \"Permanent\"\n");
        Printf(b_vob_grp_cpp->b0,"        };\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        std::set<std::string> %s::mSystemPropertyNames(\n", funcRename);
        Printf(b_vob_grp_cpp->b0,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        const std::set<std::string>& %s::getSystemPropertyNames() const {\n", funcRename);
        Printf(b_vob_grp_cpp->b0,"            return mSystemPropertyNames;\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        std::vector<std::string> %s::getPropertyNamesVector() const {\n", funcRename);
        Printf(b_vob_grp_cpp->b0,"            std::vector<std::string> ret(\n");
        Printf(b_vob_grp_cpp->b0,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
        Printf(b_vob_grp_cpp->b0,"            for (std::map<std::string, reposit::property_t>::const_iterator i = userProperties.begin();\n");
        Printf(b_vob_grp_cpp->b0,"                i != userProperties.end(); ++i)\n");
        Printf(b_vob_grp_cpp->b0,"                ret.push_back(i->first);\n");
        Printf(b_vob_grp_cpp->b0,"            return ret;\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        reposit::property_t %s::getSystemProperty(const std::string& name) const {\n", funcRename);
        Printf(b_vob_grp_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
        Printf(b_vob_grp_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                return objectId_;\n");
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                return className_;\n");
        voGetProp(b_vob_grp_cpp->b0, parmList);
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                return Permanent_;\n");
        Printf(b_vob_grp_cpp->b0,"            else\n");
        Printf(b_vob_grp_cpp->b0,"                RP_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        void %s::setSystemProperty(const std::string& name, const reposit::property_t& value) {\n", funcRename);
        Printf(b_vob_grp_cpp->b0,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
        Printf(b_vob_grp_cpp->b0,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                objectId_ = boost::get<std::string>(value);\n");
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                className_ = boost::get<std::string>(value);\n");
        voSetProp(b_vob_grp_cpp->b0, parmList);
        Printf(b_vob_grp_cpp->b0,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
        Printf(b_vob_grp_cpp->b0,"                Permanent_ = reposit::convert<bool>(value);\n");
        Printf(b_vob_grp_cpp->b0,"            else\n");
        Printf(b_vob_grp_cpp->b0,"                RP_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");
        Printf(b_vob_grp_cpp->b0,"\n");
        Printf(b_vob_grp_cpp->b0,"        %s::%s(\n", funcRename, funcRename);
        Printf(b_vob_grp_cpp->b0,"                const std::string& ObjectId,\n");
        emitParmList(parmList, b_vob_grp_cpp->b0, 2, "rp_tm_vob_parm", "rp_tm_vob_parm", 4, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"                bool Permanent) :\n");
        Printf(b_vob_grp_cpp->b0,"            reposit::ValueObject(ObjectId, \"%s\", Permanent),\n", funcRename);
        emitParmList(parmList, b_vob_grp_cpp->b0, 1, "rp_tm_vob_init", "rp_tm_vob_init", 3, ',', true, false, true);
        Printf(b_vob_grp_cpp->b0,"            Permanent_(Permanent) {\n");
        Printf(b_vob_grp_cpp->b0,"                // FIXME need to call processPrecedentID() here.\n");
        Printf(b_vob_grp_cpp->b0,"        }\n");

        report.countFunction(sectionName_, Report::Section::Constructor);

        b_vob_grp_hpp->activate();
        b_vob_grp_cpp->activate();
    }

    virtual ~GroupValueObjects() {

        Printf(b_vob_grp_hpp->b0, "} // namespace %s\n", stack.module_);
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "} // namespace ValueObjects\n");
        Printf(b_vob_grp_hpp->b0, "\n");
        Printf(b_vob_grp_hpp->b0, "#endif\n");
        Printf(b_vob_grp_hpp->b0, "\n");

        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "} // namespace %s\n", stack.module_);
        Printf(b_vob_grp_cpp->b0, "\n");
        Printf(b_vob_grp_cpp->b0, "} // namespace ValueObjects\n");
        Printf(b_vob_grp_cpp->b0, "\n");

        delete(b_vob_grp_hpp);
        delete(b_vob_grp_cpp);
    }
};

struct GroupSerializationCreate : public Group {

    Buffer *b_scr_grp_hpp;
    Buffer *b_scr_grp_cpp;

    GroupSerializationCreate(const std::string &sectionName, String *groupName) : Group(sectionName, groupName),
        b_scr_grp_hpp(0), b_scr_grp_cpp(0) {

        b_scr_grp_hpp = new Buffer(sectionName, "b_scr_grp_hpp", Report::File::Group, NewStringf("%s/serialization/create/create_%s.hpp", objDir, groupName_), false);
        b_scr_grp_cpp = new Buffer(sectionName, "b_scr_grp_cpp", Report::File::Group, NewStringf("%s/serialization/create/create_%s.cpp", objDir, groupName_), false);

        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "#ifndef create_%s_hpp\n", groupName_);
        Printf(b_scr_grp_hpp->b0, "#define create_%s_hpp\n", groupName_);
        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "#include <rp/rpdefines.hpp>\n");
        Printf(b_scr_grp_hpp->b0, "#include <rp/object.hpp>\n");
        Printf(b_scr_grp_hpp->b0, "#include <rp/valueobject.hpp>\n");
        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "namespace %s {\n", stack.module_);
        Printf(b_scr_grp_hpp->b0, "\n");

        Printf(b_scr_grp_cpp->b0, "\n");
        Printf(b_scr_grp_cpp->b0, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, groupName_);
        Printf(b_scr_grp_cpp->b0, "//#include <%s/qladdindefines.hpp>\n", objInc);
        //Printf(b_scr_grp_cpp->b0, "#include <%s/conversions/convert2.hpp>\n", objInc);
        //Printf(b_scr_grp_cpp->b0, "//#include <%s/objects/handle.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b0, "\n");
        if (stack.automatic_) {
            Printf(b_scr_grp_cpp->b0, "#include <%s/objects/obj_%s.hpp>\n", objInc, groupName_);
        } else {
            Printf(b_scr_grp_cpp->b0, "#include <%s/objects/objmanual_%s.hpp>\n", objInc, groupName_);
        }
        Printf(b_scr_grp_cpp->b2, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, groupName_);
        Printf(b_scr_grp_cpp->b2, "\n");
        Printf(b_scr_grp_cpp->b2, "#include <%s/conversions/all.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b2, "#include <%s/enumerations/factories/all.hpp>\n", objInc);
        Printf(b_scr_grp_cpp->b2, "#include <rp/property.hpp>\n");
        Printf(b_scr_grp_cpp->b2, "\n");
    }

    void functionWrapperCtor(Node *n) {

        String *name = Getattr(n, "name");
        String *funcRename = Getattr(n, "rp:funcRename");
        ParmList *parmList = Getattr(n, "parms");

        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "boost::shared_ptr<reposit::Object> create_%s(\n", funcRename);
        Printf(b_scr_grp_hpp->b0, "    const boost::shared_ptr<reposit::ValueObject>&);\n");

        Printf(b_scr_grp_cpp->b2, "\n");
        Printf(b_scr_grp_cpp->b2, "boost::shared_ptr<reposit::Object> %s::create_%s(\n", stack.module_, funcRename);
        Printf(b_scr_grp_cpp->b2, "    const boost::shared_ptr<reposit::ValueObject> &valueObject) {\n");
        Printf(b_scr_grp_cpp->b2, "\n");
        Printf(b_scr_grp_cpp->b2, "    // conversions\n\n");
        emitParmList(parmList, b_scr_grp_cpp->b2, 1, "rp_tm_scr_cnvt", "rp_tm_scr_cnvt", 1, 0);
        Printf(b_scr_grp_cpp->b2, "\n");
        Printf(b_scr_grp_cpp->b2, "    bool Permanent =\n");
        Printf(b_scr_grp_cpp->b2, "        reposit::convert<bool>(valueObject->getProperty(\"Permanent\"));\n");
        Printf(b_scr_grp_cpp->b2, "\n");
        Printf(b_scr_grp_cpp->b2, "    // construct and return the object\n");
        Printf(b_scr_grp_cpp->b2, "\n");
        Printf(b_scr_grp_cpp->b2, "    boost::shared_ptr<reposit::Object> object(\n");
        Printf(b_scr_grp_cpp->b2, "        new %s::%s(\n", stack.module_, name);
        Printf(b_scr_grp_cpp->b2, "            valueObject,\n");
        emitParmList(parmList, b_scr_grp_cpp->b2, 0, "rp_tm_default", "rp_tm_default", 3, ',', true, false, true);
        Printf(b_scr_grp_cpp->b2, "            Permanent));\n");
        Printf(b_scr_grp_cpp->b2, "    return object;\n");
        Printf(b_scr_grp_cpp->b2, "}\n");

        report.countFunction(sectionName_, Report::Section::Constructor);

        active_=true;
        b_scr_grp_hpp->activate();
        b_scr_grp_cpp->activate();
    }

    virtual ~GroupSerializationCreate() {

        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "} // namespace %s\n", stack.module_);
        Printf(b_scr_grp_hpp->b0, "\n");
        Printf(b_scr_grp_hpp->b0, "#endif\n");
        Printf(b_scr_grp_hpp->b0, "\n");

        Append(b_scr_grp_cpp->b1, stack.obj_cpp_);
        Printf(b_scr_grp_cpp->b2, "\n");

        delete(b_scr_grp_hpp);
        delete(b_scr_grp_cpp);
    }
};

struct GroupSerializationRegister : public Group {

    Buffer *b_srg_grp_hpp;
    Buffer *b_srg_grp_cpp;

    GroupSerializationRegister(const std::string &sectionName, String *groupName) : Group(sectionName, groupName),
        b_srg_grp_hpp(0), b_srg_grp_cpp(0) {

        b_srg_grp_hpp = new Buffer(sectionName, "b_srg_grp_hpp", Report::File::Group, NewStringf("%s/serialization/register/serialization_%s.hpp", objDir, groupName_), false);
        b_srg_grp_cpp = new Buffer(sectionName, "b_srg_grp_cpp", Report::File::Group, NewStringf("%s/serialization/register/serialization_%s.cpp", objDir, groupName_), false);

        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "#ifndef serialization_%s_hpp\n", groupName_);
        Printf(b_srg_grp_hpp->b0, "#define serialization_%s_hpp\n", groupName_);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "#include <boost/archive/xml_iarchive.hpp>\n");
        Printf(b_srg_grp_hpp->b0, "#include <boost/archive/xml_oarchive.hpp>\n");
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "namespace %s {\n", stack.module_);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "    void register_%s(boost::archive::xml_oarchive &ar);\n", groupName_);
        Printf(b_srg_grp_hpp->b0, "    void register_%s(boost::archive::xml_iarchive &ar);\n", groupName_);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "} // namespace %s\n", stack.module_);
        Printf(b_srg_grp_hpp->b0, "\n");
        Printf(b_srg_grp_hpp->b0, "#endif\n");
        Printf(b_srg_grp_hpp->b0, "\n");

        Printf(b_srg_grp_cpp->b0, "\n");
        Printf(b_srg_grp_cpp->b0, "#include <rp/rpdefines.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "\n");
        Printf(b_srg_grp_cpp->b0, "// Suppress a very long warning message which occurs at least in VC9 (2008) + boost::serialization 1.58\n");
        Printf(b_srg_grp_cpp->b0, "// (and possibly for other/all combinations of VC + boost::serialization)?\n");
        Printf(b_srg_grp_cpp->b0, "// warning C4018: '<' : signed/unsigned mismatch\n");
        Printf(b_srg_grp_cpp->b0, "#if defined BOOST_MSVC\n");
        Printf(b_srg_grp_cpp->b0, "#pragma warning(disable:4018)\n");
        Printf(b_srg_grp_cpp->b0, "#endif\n");
        Printf(b_srg_grp_cpp->b0, "\n");
        Printf(b_srg_grp_cpp->b0, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, groupName_);
        Printf(b_srg_grp_cpp->b0, "#include <%s/valueobjects/vo_%s.hpp>\n", objInc, groupName_);
        Printf(b_srg_grp_cpp->b0, "#include <boost/serialization/shared_ptr.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "#include <boost/serialization/variant.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "#include <boost/serialization/vector.hpp>\n");
        Printf(b_srg_grp_cpp->b0, "\n");
        Printf(b_srg_grp_cpp->b0, "void %s::register_%s(boost::archive::xml_oarchive &ar) {\n", stack.module_, groupName_);
        Printf(b_srg_grp_cpp->b0, "\n");

        Printf(b_srg_grp_cpp->b1, "\n");
        Printf(b_srg_grp_cpp->b1, "void %s::register_%s(boost::archive::xml_iarchive &ar) {\n", stack.module_, groupName_);
        Printf(b_srg_grp_cpp->b1, "\n");
    }

    void functionWrapperCtor(Node *n) {

        String *funcRename = Getattr(n, "rp:funcRename");

        Printf(b_srg_grp_cpp->b0, "    // class ID %d in the boost serialization framework\n", idNum);
        Printf(b_srg_grp_cpp->b0, "    ar.register_type<%s::ValueObjects::%s>();\n", stack.module_, funcRename);

        Printf(b_srg_grp_cpp->b1, "    // class ID %d in the boost serialization framework\n", idNum);
        Printf(b_srg_grp_cpp->b1, "    ar.register_type<%s::ValueObjects::%s>();\n", stack.module_, funcRename);

        idNum++;

        report.countFunction(sectionName_, Report::Section::Constructor);

        active_=true;
        b_srg_grp_hpp->activate();
        b_srg_grp_cpp->activate();
    }

    virtual ~GroupSerializationRegister() {

        Printf(b_srg_grp_hpp->b0, "\n");

        Printf(b_srg_grp_cpp->b0, "}\n");
        Printf(b_srg_grp_cpp->b0, "\n");

        Printf(b_srg_grp_cpp->b1, "}\n");
        Printf(b_srg_grp_cpp->b1, "\n");

        delete(b_srg_grp_hpp);
        delete(b_srg_grp_cpp);
    }
};

struct GroupExcelFunctions : public Group {

    Buffer *b_xlf_grp_cpp;
    bool groupContainsClass;
    bool groupContainsConstructor;
    bool groupContainsLoopFunction;

    GroupExcelFunctions(const std::string &sectionName, String *groupName) : Group(sectionName, groupName),
        groupContainsClass(false), groupContainsConstructor(false), groupContainsLoopFunction(false) {

        b_xlf_grp_cpp = new Buffer(sectionName, "b_xlf_grp_cpp", Report::File::Group, NewStringf("%s/functions/function_%s.cpp", xllDir, groupName_), false);

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

        //Printf(b_xlf_grp_cpp->b1, "//#include \"%s/conversions/convert2.hpp\"\n", objInc);
        //Printf(b_xlf_grp_cpp->b1, "#include \"%s/conversions/conversions.hpp\"\n", objInc);
        Printf(b_xlf_grp_cpp->b1, "#include \"%s/conversions/all.hpp\"\n", xllInc);
        Printf(b_xlf_grp_cpp->b1, "\n");
    }

    void functionWrapperCtorImpl(Node *n, String *funcName) {

        String *name = Getattr(n, "name");
        ParmList *parmList = Getattr(n, "parms");
        ParmList *parmList2 = Getattr(n, "rp:parms2");

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****CTOR*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT char *%s(\n", funcName);
        emitParmList(parmList2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", "rp_tm_xll_parm2");
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(parmList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", "rp_tm_xll_cnvt2", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        boost::shared_ptr<reposit::ValueObject> valueObject(\n");
        Printf(b_xlf_grp_cpp->b1, "            new %s::ValueObjects::%s(\n", stack.module_, funcName);
        Printf(b_xlf_grp_cpp->b1, "                objectID,\n");
        emitParmList(parmList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argv", "rp_tm_xll_argv2", 4, ',', true, true, true);
        Printf(b_xlf_grp_cpp->b1, "                false));\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        boost::shared_ptr<reposit::Object> object(\n");
        Printf(b_xlf_grp_cpp->b1, "            new %s::%s(\n", stack.module_, name);
        Printf(b_xlf_grp_cpp->b1, "                valueObject,\n");
        emitParmList(parmList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", "rp_tm_xll_argf2", 4, ',', true, true, true);
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

        report.countFunction(sectionName_, Report::Section::Constructor);
        active_=true;
        b_xlf_grp_cpp->activate();
    }

    void functionWrapperCtor(Node *n) {

        if (!Getattr(n, "default_constructor")) {
            String *funcRename = Getattr(n, "rp:funcRename");
            functionWrapperCtorImpl(n, funcRename);
            if (String *alias = Getattr(n, "rp:alias"))
                functionWrapperCtorImpl(n, alias);
            groupContainsConstructor = true;
        }

        groupContainsClass = true;
    }

    void emitLoopFunc(Node *n) {

        String *name = Getattr(n, "name");
        String *funcName = Getattr(n, "rp:funcName");
        String *memberTypeName = Getattr(n, "rp:memberTypeName");
        String *loopParameterName = Getattr(n, "feature:rp:loopParameter");
        Parm *loopParameter = Getattr(n, "rp:loopParameterOrig");
        String *loopParameterType = getTypeMap(loopParameter, "rp_tm_xll_lppm");
        String *loopFunctionType = getTypeMap(n, "rp_tm_xll_lpfn");
        //ParmList *parmList = Getattr(n, "parms");
        //ParmList *parmList2 = Getattr(n, "rp:parms2");
        ParmList *loopParameterList = Getattr(n, "rp:loopParameterList");

        Printf(b_xlf_grp_cpp->b1, "        // BEGIN function emitLoopFunc\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        static XLOPER returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        %s::%sBind bindObject =\n", stack.module_, funcName);
        Printf(b_xlf_grp_cpp->b1, "            boost::bind((%s::%sSignature)\n", stack.module_, funcName);
        Printf(b_xlf_grp_cpp->b1, "                &%s::%s,\n", memberTypeName, name);
        Printf(b_xlf_grp_cpp->b1, "                xxx,\n");
        emitParmList(loopParameterList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_loop", "rp_tm_xll_loop2", 4, ',', true, true);
        Printf(b_xlf_grp_cpp->b1, "            );\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::loop\n");
        Printf(b_xlf_grp_cpp->b1, "            <%s::%sBind, %s, %s>\n", stack.module_, funcName, loopParameterType, loopFunctionType);
        Printf(b_xlf_grp_cpp->b1, "            (functionCall, bindObject, %s, returnValue);\n", loopParameterName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        return &returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        // END   function emitLoopFunc\n");
    }

    void functionWrapperMemberImpl(Node *n, String *funcName) {

        String *name = Getattr(n, "name");
        Node *memberType = Getattr(n, "rp:memberType");
        Node *nodeForTypeMaps = Getattr(n, "rp:loopFunctionNode");
        ParmList *parmList = Getattr(n, "parms");
        ParmList *parmList2 = Getattr(n, "rp:parms2");
        ParmList *loopParameterList = Getattr(n, "rp:loopParameterList");

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****MEMB*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT\n");
        emitTypeMap(b_xlf_grp_cpp->b1, nodeForTypeMaps, "rp_tm_xll_rtft");
        Printf(b_xlf_grp_cpp->b1, "%s(\n", funcName);
        emitParmList(parmList2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", "rp_tm_xll_parm2");
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(loopParameterList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", "rp_tm_xll_cnvt2", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (checkAttribute(n, "feature:rp:wrap", "1")) {
            emitTypeMap(b_xlf_grp_cpp->b1, memberType, "rp_tm_xxx_rp_wrp", 2);
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, memberType, "rp_tm_xxx_rp_get", 2);
        }
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (String *loopParameterName = Getattr(n, "feature:rp:loopParameter")) {
            emitLoopFunc(n);
            groupContainsLoopFunction = true;
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, nodeForTypeMaps, "rp_tm_xll_rtdc", 2);
            emitTypeMap(b_xlf_grp_cpp->b1, memberType, "rp_tm_xxx_rp_call", 2);
            Printf(b_xlf_grp_cpp->b1, "        %s(\n", name);
            emitParmList(loopParameterList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", "rp_tm_xll_argf2", 3, ',', true, true);
            Printf(b_xlf_grp_cpp->b1, "        );\n\n");
            emitTypeMap(b_xlf_grp_cpp->b1, nodeForTypeMaps, "rp_tm_xll_rtst", 2);
        }
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    } catch (const std::exception &e) {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::RepositoryXL::instance().logError(e.what(), functionCall);\n");
        Printf(b_xlf_grp_cpp->b1, "        return 0;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    }\n");
        Printf(b_xlf_grp_cpp->b1, "}\n");

        report.countFunction(sectionName_, Report::Section::Member);
        active_=true;
        b_xlf_grp_cpp->activate();
    }

    void functionWrapperMember(Node *n) {
        String *funcRename = Getattr(n, "rp:funcRename");
        functionWrapperMemberImpl(n, funcRename);
        if (String *alias = Getattr(n, "rp:alias"))
            functionWrapperMemberImpl(n, alias);
    }

    void emitLoopFunc2(Node *n) {

        String *name = Getattr(n, "name");
        String *funcName = Getattr(n, "rp:funcName");
        ParmList *parmList = Getattr(n, "parms");
        String *loopParameterName = Getattr(n, "feature:rp:loopParameter");
        Parm *loopParameter = Getattr(n, "rp:loopParameterOrig");//?
        String *loopParameterType = getTypeMap(loopParameter, "rp_tm_xll_lppm");
        String *loopFunctionType = getTypeMap(n, "rp_tm_xll_lpfn");
        ParmList *loopParameterList = Getattr(n, "rp:loopParameterList");

        Printf(b_xlf_grp_cpp->b1, "        // BEGIN function emitLoopFunc\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        static XLOPER returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        %s::%sBind bindObject =\n", stack.module_, funcName);
        Printf(b_xlf_grp_cpp->b1, "            boost::bind(\n");
        Printf(b_xlf_grp_cpp->b1, "                %s,\n", name);
        emitParmList(loopParameterList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_loop", "rp_tm_xll_loop2", 4, ',', true, true);
        Printf(b_xlf_grp_cpp->b1, "            );\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::loop\n");
        Printf(b_xlf_grp_cpp->b1, "            <%s::%sBind, %s, %s>\n", stack.module_, funcName, loopParameterType, loopFunctionType);
        Printf(b_xlf_grp_cpp->b1, "            (functionCall, bindObject, %s, returnValue);\n", loopParameterName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        return &returnValue;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        // END   function emitLoopFunc\n");
    }

    void functionWrapperFunc(Node *n) {

        String *name = Getattr(n, "name");
        String *funcName = Getattr(n, "rp:funcName");
        Node *nodeForTypeMaps = Getattr(n, "rp:loopFunctionNode");
        ParmList *parmList = Getattr(n, "parms");
        ParmList *parmList2 = Getattr(n, "rp:parms2");
        ParmList *loopParameterList = Getattr(n, "rp:loopParameterList");

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1,"//****FUNC*****\n");
        Printf(b_xlf_grp_cpp->b1, "DLLEXPORT\n");
        emitTypeMap(b_xlf_grp_cpp->b1, nodeForTypeMaps, "rp_tm_xll_rtft");
        Printf(b_xlf_grp_cpp->b1, "%s(\n", funcName);
        emitParmList(parmList2, b_xlf_grp_cpp->b1, 2, "rp_tm_xll_parm", "rp_tm_xll_parm2", 1);
        Printf(b_xlf_grp_cpp->b1, ") {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    boost::shared_ptr<reposit::FunctionCall> functionCall;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    try {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        functionCall = boost::shared_ptr<reposit::FunctionCall>\n");
        Printf(b_xlf_grp_cpp->b1, "            (new reposit::FunctionCall(\"%s\"));\n", funcName);
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::validateRange(Trigger, \"Trigger\");\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        emitParmList(loopParameterList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_cnvt", "rp_tm_xll_cnvt2", 2, 0, false);
        Printf(b_xlf_grp_cpp->b1, "\n");
        if (String *loopParameterName = Getattr(n, "feature:rp:loopParameter")) {
            emitLoopFunc2(n);
            groupContainsLoopFunction = true;
        } else {
            emitTypeMap(b_xlf_grp_cpp->b1, nodeForTypeMaps, "rp_tm_xll_rtdc", 2);
            Printf(b_xlf_grp_cpp->b1, "        %s(\n", name);
            emitParmList(loopParameterList, b_xlf_grp_cpp->b1, 1, "rp_tm_xll_argf", "rp_tm_xll_argf2", 3, ',', true, true);
            Printf(b_xlf_grp_cpp->b1, "        );\n\n");
            emitTypeMap(b_xlf_grp_cpp->b1, nodeForTypeMaps, "rp_tm_xll_rtst", 2);
        }

        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    } catch (const std::exception &e) {\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "        reposit::RepositoryXL::instance().logError(e.what(), functionCall);\n");
        Printf(b_xlf_grp_cpp->b1, "        return 0;\n");
        Printf(b_xlf_grp_cpp->b1, "\n");
        Printf(b_xlf_grp_cpp->b1, "    }\n");
        Printf(b_xlf_grp_cpp->b1, "}\n");

        report.countFunction(sectionName_, Report::Section::Function);
        active_=true;
        b_xlf_grp_cpp->activate();
    }

    virtual ~GroupExcelFunctions() {

        if (groupContainsConstructor)
            Printf(b_xlf_grp_cpp->b0, "#include \"%s/valueobjects/vo_%s.hpp\"\n", objInc, groupName_);

        if (groupContainsClass) {
            if (stack.automatic_) {
                Printf(b_xlf_grp_cpp->b0, "#include \"%s/objects/obj_%s.hpp\"\n", objInc, groupName_);
            } else {
                Printf(b_xlf_grp_cpp->b0, "#include \"%s/objects/objmanual_%s.hpp\"\n", objInc, groupName_);
            }
        }

        if (groupContainsLoopFunction) {
            Printf(b_xlf_grp_cpp->b0, "#include <rpxl/loop.hpp>\n");
            Printf(b_xlf_grp_cpp->b0, "#include \"%s/loop/loop_%s.hpp\"\n", objInc, groupName_);
        }
        Append(b_xlf_grp_cpp->b0, stack.obj_cpp_);

        delete(b_xlf_grp_cpp);
    }
};

struct GroupExcelRegister : public Group {

    Buffer *b_xlr_grp_cpp;

    GroupExcelRegister(const std::string &sectionName, String *groupName) : Group(sectionName, groupName) {

        b_xlr_grp_cpp = new Buffer(sectionName, "b_xlr_grp_cpp", Report::File::Group, NewStringf("%s/register/register_%s.cpp", xllDir, groupName_), false);

        Printf(b_xlr_grp_cpp->b0, "\n");
        Printf(b_xlr_grp_cpp->b0, "#include <xlsdk/xlsdkdefines.hpp>\n");
        Printf(b_xlr_grp_cpp->b0, "\n");
        Printf(b_xlr_grp_cpp->b0, "void register_%s(const XLOPER &xDll) {\n", groupName_);
        Printf(b_xlr_grp_cpp->b0, "\n");
        Printf(b_xlr_grp_cpp->b1, "\n");
        Printf(b_xlr_grp_cpp->b1, "void unregister_%s(const XLOPER &xDll) {\n", groupName_);
        Printf(b_xlr_grp_cpp->b1, "\n");
        Printf(b_xlr_grp_cpp->b1, "    XLOPER xlRegID;\n");
        Printf(b_xlr_grp_cpp->b1, "\n");
    }

    void functionWrapperCtor(Node *n) {

        String *funcRename = Getattr(n, "rp:funcRename");

        excelRegister(n, funcRename);
        if (String *alias = Getattr(n, "rp:alias"))
            excelRegister(n, alias);

        report.countFunction(sectionName_, Report::Section::Constructor);
        active_=true;
        b_xlr_grp_cpp->activate();
    }

    void functionWrapperMember(Node *n) {

        String *funcRename = Getattr(n, "rp:funcRename");

        excelRegister(n, funcRename);
        if (String *alias = Getattr(n, "rp:alias"))
            excelRegister(n, alias);

        report.countFunction(sectionName_, Report::Section::Member);
        active_=true;
        b_xlr_grp_cpp->activate();
    }

    void functionWrapperFunc(Node *n) {

        String *funcName = Getattr(n, "rp:funcName");
        excelRegister(n, funcName);

        report.countFunction(sectionName_, Report::Section::Function);
        active_=true;
        b_xlr_grp_cpp->activate();
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

    void excelRegister(Node *n, String *funcName) {

        ParmList *parmList2 = Getattr(n, "rp:parms2");
        String *groupFunctionWizard = stack.module_;
        String *funcDoc = NewString("");
        Node *nodeForTypeMaps = Getattr(n, "rp:loopFunctionNode");

        Printf(b_xlr_grp_cpp->b0, "        // BEGIN function excelRegister\n");
        Printf(b_xlr_grp_cpp->b0, "        Excel(xlfRegister, 0, %d, &xDll,\n", 10 + paramListSize(parmList2));
        Printf(b_xlr_grp_cpp->b0, "            // function code name\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
        Printf(b_xlr_grp_cpp->b0, "            // parameter codes (function excelParamCodes() using typemaps rp_tm_xll_cdrt, rp_tm_xll_code, rp_tm_xll_code2).\n");
        excelParamCodes(b_xlr_grp_cpp->b0, nodeForTypeMaps, parmList2);
        Printf(b_xlr_grp_cpp->b0, "            // function display name\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
        Printf(b_xlr_grp_cpp->b0, "            // comma-delimited list of parameter names\n");
        excelParamList(b_xlr_grp_cpp->b0, parmList2);
        Printf(b_xlr_grp_cpp->b0, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x01\"\"1\"),\n");
        Printf(b_xlr_grp_cpp->b0, "            // function category\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(groupFunctionWizard).c_str(), groupFunctionWizard);
        Printf(b_xlr_grp_cpp->b0, "            // shortcut text (command macros only)\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x00\"\"\"),\n");
        Printf(b_xlr_grp_cpp->b0, "            // path to help file\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x00\"\"\"),\n");
        Printf(b_xlr_grp_cpp->b0, "            // function description\n");
        Printf(b_xlr_grp_cpp->b0, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcDoc).c_str(), funcDoc);
        Printf(b_xlr_grp_cpp->b0, "            // parameter descriptions\n");
        excelParamListDocStrings(b_xlr_grp_cpp->b0, parmList2);
        Printf(b_xlr_grp_cpp->b0, "        );\n");
        Printf(b_xlr_grp_cpp->b0, "        // END   function excelRegister\n\n");

        Printf(b_xlr_grp_cpp->b1, "        // BEGIN function excelUnregister\n");
        Printf(b_xlr_grp_cpp->b1, "        Excel(xlfRegister, 0, %d, &xDll,\n", 10 + paramListSize(parmList2));
        Printf(b_xlr_grp_cpp->b1, "            // function code name\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
        Printf(b_xlr_grp_cpp->b1, "            // parameter codes (function excelParamCodes() using typemaps rp_tm_xll_cdrt, rp_tm_xll_code, rp_tm_xll_code2).\n");
        excelParamCodes(b_xlr_grp_cpp->b1, nodeForTypeMaps, parmList2);
        Printf(b_xlr_grp_cpp->b1, "            // function display name\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcName).c_str(), funcName);
        Printf(b_xlr_grp_cpp->b1, "            // comma-delimited list of parameter names\n");
        excelParamList(b_xlr_grp_cpp->b1, parmList2);
        Printf(b_xlr_grp_cpp->b1, "            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x01\"\"0\"),\n");
        Printf(b_xlr_grp_cpp->b1, "            // function category\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(groupFunctionWizard).c_str(), groupFunctionWizard);
        Printf(b_xlr_grp_cpp->b1, "            // shortcut text (command macros only)\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x00\"\"\"),\n");
        Printf(b_xlr_grp_cpp->b1, "            // path to help file\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x00\"\"\"),\n");
        Printf(b_xlr_grp_cpp->b1, "            // function description\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x%s\"\"%s\"),\n", hexLen(funcDoc).c_str(), funcDoc);
        Printf(b_xlr_grp_cpp->b1, "            // parameter descriptions\n");
        excelParamListDocStrings(b_xlr_grp_cpp->b1, parmList2);
        Printf(b_xlr_grp_cpp->b1, "        );\n");
        Printf(b_xlr_grp_cpp->b1, "\n");
        Printf(b_xlr_grp_cpp->b1, "        Excel4(xlfRegisterId, &xlRegID, 2, &xDll,\n");
        Printf(b_xlr_grp_cpp->b1, "            TempStrNoSize(\"\\x%s\"\"%s\"));\n", hexLen(funcName).c_str(), funcName);
        Printf(b_xlr_grp_cpp->b1, "        Excel4(xlfUnregister, 0, 1, &xlRegID);\n");
        Printf(b_xlr_grp_cpp->b1, "        // END   function excelUnregister\n\n");
    }

    virtual ~GroupExcelRegister() {

        Printf(b_xlr_grp_cpp->b0, "}\n");
        Printf(b_xlr_grp_cpp->b1, "}\n");

        delete(b_xlr_grp_cpp);
    }
};

//*****************************************************************************
// SECTIONS
//*****************************************************************************

// The autogenerated source code for an Addin is organized into one or more Sections.
// The Section struct represents a directory in the source code tree
// containing one hpp file and/or one cpp file for each Group of functions.
// The Section struct might also generate one or more summary source code files
// relevant to all of the Groups in the section, e.g. an all.hpp file.

struct Section {

    std::string name_;

    Section(const std::string &name) : name_(name) {}
    virtual ~Section() {}
    virtual void top() {}
    virtual void createGroup(String *groupName) = 0;
    virtual void deleteGroup() = 0;
    virtual void classHandlerBefore() = 0;
    virtual void classHandlerAfter() = 0;
    virtual void functionWrapperCtor(Node*) = 0;
    virtual void functionWrapperMember(Node*) = 0;
    virtual void functionWrapperFunc(Node*) = 0;
};

template <class Group>
struct SectionImpl : public Section {

    Group *group_;

    SectionImpl(const std::string &name) : Section(name), group_(0) {
        report.addSection(name);
    }

    virtual void createGroup(String *groupName) {
        group_ = new Group(name_, groupName);
    }

    virtual void deleteGroup() {
        delete(group_);
        group_ = 0;
    }

    virtual void classHandlerBefore() {
        group_->classHandlerBefore();
    }

    virtual void classHandlerAfter() {
        group_->classHandlerAfter();
    }

    virtual void functionWrapperCtor(Node *n) {
        group_->functionWrapperCtor(n);
    }

    virtual void functionWrapperMember(Node *n) {
        group_->functionWrapperMember(n);
    }

    virtual void functionWrapperFunc(Node *n) {
        group_->functionWrapperFunc(n);
    }
};

struct SectionLibraryObjects : public SectionImpl<GroupLibraryObjects> {

    Buffer *b_lib_add_hpp;

    SectionLibraryObjects() : SectionImpl("Library Objects") {}

    virtual void top() {

        b_lib_add_hpp = new Buffer(name_, "b_lib_add_hpp", Report::File::Section, NewStringf("%s/objects/obj_all.hpp", objDir));

        Printf(b_lib_add_hpp->b0, "\n");
        Printf(b_lib_add_hpp->b0, "#ifndef obj_all_hpp\n");
        Printf(b_lib_add_hpp->b0, "#define obj_all_hpp\n");
        Printf(b_lib_add_hpp->b0, "\n");
    }

    virtual ~SectionLibraryObjects() {

        Printf(b_lib_add_hpp->b0, "\n");
        Printf(b_lib_add_hpp->b0, "#endif\n");
        Printf(b_lib_add_hpp->b0, "\n");
    }
};

struct SectionValueObjects : public SectionImpl<GroupValueObjects> {

    SectionValueObjects() : SectionImpl("Value Objects") {}

    virtual void functionWrapperCtor(Node *n) {
        if (!Getattr(n, "default_constructor"))
            SectionImpl::functionWrapperCtor(n);
    }
};

struct SectionSerializationCreate : public SectionImpl<GroupSerializationCreate> {

    Buffer *b_scr_add_hpp;
    List *activeGroups_;

    SectionSerializationCreate() : SectionImpl("Serialization - Create") {
        activeGroups_ = NewList();
    }

    virtual void top() {
        b_scr_add_hpp = new Buffer(name_, "b_scr_add_hpp", Report::File::Section, NewStringf("%s/serialization/create/create_all.hpp", objDir));

        Printf(b_scr_add_hpp->b0, "\n");
        Printf(b_scr_add_hpp->b0, "#ifndef create_all_hpp\n");
        Printf(b_scr_add_hpp->b0, "#define create_all_hpp\n");
        Printf(b_scr_add_hpp->b0, "\n");
    }

    virtual void functionWrapperCtor(Node *n) {
        if (!Getattr(n, "default_constructor"))
            SectionImpl::functionWrapperCtor(n);
    }

    virtual void deleteGroup() {
        if (group_ && group_->active_)
            Append(activeGroups_, group_->groupName_);
        SectionImpl::deleteGroup();
    }

    virtual ~SectionSerializationCreate() {

        for (int i=0; i<Len(activeGroups_); i++) {
            String *groupName = Getitem(activeGroups_, i);
            Printf(b_scr_add_hpp->b0, "#include <%s/serialization/create/create_%s.hpp>\n", objInc, groupName);
        }

        Printf(b_scr_add_hpp->b0, "\n");
        Printf(b_scr_add_hpp->b0, "#endif\n");
        Printf(b_scr_add_hpp->b0, "\n");

        delete(b_scr_add_hpp);
    }
};

struct SectionSerializationRegister : public SectionImpl<GroupSerializationRegister> {

    Buffer *b_srg_add_hpp;
    Buffer *b_srg_add_cpp;
    Buffer *b_sra_add_hpp;
    List *activeGroups_;

    SectionSerializationRegister() : SectionImpl("Serialization - Register") {
        activeGroups_ = NewList();
    }

    virtual void top() {

        b_srg_add_hpp = new Buffer(name_, "b_srg_add_hpp", Report::File::Section, NewStringf("%s/serialization/register/serialization_register.hpp", objDir));
        b_srg_add_cpp = new Buffer(name_, "b_srg_add_cpp", Report::File::Section, NewStringf("%s/serialization/register_creators.cpp", objDir));
        b_sra_add_hpp = new Buffer(name_, "b_sra_add_hpp", Report::File::Section, NewStringf("%s/serialization/register/serialization_all.hpp", objDir));

        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "#ifndef serialization_register_hpp\n");
        Printf(b_srg_add_hpp->b0, "#define serialization_register_hpp\n");
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "#include <%s/serialization/register/serialization_all.hpp>\n", objInc);
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "namespace %s {\n", stack.module_);
        Printf(b_srg_add_hpp->b0, "\n");
        Printf(b_srg_add_hpp->b0, "    template<class Archive>\n");
        Printf(b_srg_add_hpp->b0, "    void tpl_register_classes(Archive& ar) {\n");
        Printf(b_srg_add_hpp->b0, "\n");

        Printf(b_srg_add_cpp->b0, "\n");
        Printf(b_srg_add_cpp->b0, "#include <%s/serialization/serializationfactory.hpp>\n", objInc);
        Printf(b_srg_add_cpp->b0, "#include <%s/serialization/create/create_all.hpp>\n", objInc);
        Printf(b_srg_add_cpp->b0, "\n");
        Printf(b_srg_add_cpp->b0, "void %s::SerializationFactory::registerCreators() {\n", stack.module_);
        Printf(b_srg_add_cpp->b0, "\n");

        Printf(b_sra_add_hpp->b0, "\n");
        Printf(b_sra_add_hpp->b0, "#ifndef serialization_all_hpp\n");
        Printf(b_sra_add_hpp->b0, "#define serialization_all_hpp\n");
        Printf(b_sra_add_hpp->b0, "\n");
    }

    virtual void functionWrapperCtor(Node *n) {
        if (!Getattr(n, "default_constructor")) {
            SectionImpl::functionWrapperCtor(n);
            String *funcRename = Getattr(n, "rp:funcRename");
            Printf(b_srg_add_cpp->b0, "    registerCreator(\"%s\", create_%s);\n", funcRename, funcRename);
        }
    }

    virtual void deleteGroup() {
        if (group_ && group_->active_)
            Append(activeGroups_, group_->groupName_);
        SectionImpl::deleteGroup();
    }

    virtual ~SectionSerializationRegister() {

        for (int i=0; i<Len(activeGroups_); i++) {
            String *groupName = Getitem(activeGroups_, i);
            Printf(b_sra_add_hpp->b0, "#include <%s/serialization/register/serialization_%s.hpp>\n", objInc, groupName);
            Printf(b_srg_add_hpp->b0, "        register_%s(ar);\n", groupName);
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

        delete(b_srg_add_hpp);
        delete(b_srg_add_cpp);
        delete(b_sra_add_hpp);
    }
};

struct SectionExcelFunctions : public SectionImpl<GroupExcelFunctions> {

    SectionExcelFunctions() : SectionImpl("Excel Addin - Functions") {}
};

struct SectionExcelRegister : public SectionImpl<GroupExcelRegister> {

    Buffer *b_xlr_add_all_cpp;
    List *activeGroups_;

    SectionExcelRegister() : SectionImpl("Excel Addin - Register") {
        activeGroups_ = NewList();
    }

    virtual void top() {
        b_xlr_add_all_cpp = new Buffer(name_, "b_xlr_add_all_cpp", Report::File::Section, NewStringf("%s/register/register_all.cpp", xllDir));

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

    virtual void functionWrapperCtor(Node *n) {
        if (!Getattr(n, "default_constructor")) {
            SectionImpl::functionWrapperCtor(n);
        }
    }

    virtual void deleteGroup() {
        if (group_ && group_->active_)
            Append(activeGroups_, group_->groupName_);
        SectionImpl::deleteGroup();
    }

    ~SectionExcelRegister() {

        for (int i=0; i<Len(activeGroups_); i++) {
            String *groupName = Getitem(activeGroups_, i);
            Printf(b_xlr_add_all_cpp->b0, "extern void register_%s(const XLOPER&);\n", groupName);
            Printf(b_xlr_add_all_cpp->b1, "extern void unregister_%s(const XLOPER&);\n", groupName);
            Printf(b_xlr_add_all_cpp->b2, "    register_%s(xDll);\n", groupName);
            Printf(b_xlr_add_all_cpp->b3, "    unregister_%s(xDll);\n", groupName);
        }

        Printf(b_xlr_add_all_cpp->b2, "\n");
        Printf(b_xlr_add_all_cpp->b2, "}\n");
        Printf(b_xlr_add_all_cpp->b2, "\n");
        Printf(b_xlr_add_all_cpp->b3, "\n");
        Printf(b_xlr_add_all_cpp->b3, "}\n");
        Printf(b_xlr_add_all_cpp->b3, "\n");

        delete(b_xlr_add_all_cpp);
    }
};

struct SectionList {

    std::vector<Section*> sectionList_;
    typedef std::vector<Section*>::const_iterator iter;

    void appendSection(Section *section) {
        sectionList_.push_back(section);
    }

    void top() {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->top();
        }
    }

    virtual void createGroup(String *groupName) {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->createGroup(groupName);
        }
    }

    virtual void deleteGroup() {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->deleteGroup();
        }
    }

    void classHandlerBefore() {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->classHandlerBefore();
        }
    }

    void classHandlerAfter() {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->classHandlerAfter();
        }
    }

    void functionWrapperCtor(Node *n) {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->functionWrapperCtor(n);
        }
    }

    void functionWrapperMember(Node *n) {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->functionWrapperMember(n);
        }
    }

    void functionWrapperFunc(Node *n) {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            section->functionWrapperFunc(n);
        }
    }

    void clear() {
        for (iter i=sectionList_.begin(); i!=sectionList_.end(); ++i) {
            Section *section = *i;
            delete(section);
        }
    }
};

//*****************************************************************************
// LOGGING
//*****************************************************************************

struct LogStack {

    File *f;
    Node *n;
    String *s;
    String *d;
    static int i;

    void printNode(Node *n) {
        List *l = Keys(n);
        for (int i=0; i<Len(l); ++i) {
            String *key = Getitem(l, i);
            Printf(f, "%d %s %s\n", i, key, Getattr(n, key));
        }
    }

    void printParms() {
        Node *p = Getattr(n, "parms");
        while (p) {
            printNode(p);
            p = Getattr(p, "nextSibling");
        }
    }

    LogStack(File *f, Node *n, String *s) : f(f), n(n), s(s) {
        d = NewString("");
        for (int x=0; x<i; x++)
            Append(d, ".");
        Printf(f, "%sBEGIN %s() %s.\n", d, s, Getattr(n, "name"));
        //printNode(n);
        i++;
    }

    ~LogStack() {
        Printf(f, "%sEND   %s() %s.\n", d, s, Getattr(n, "name"));
        i--;
        Delete(d);
    }
};
int LogStack::i = 0;

//*****************************************************************************
// LANGUAGE
//*****************************************************************************

class REPOSIT : public Language {

    SectionList sectionList_;

protected:

    // SWIG buffers.  We do not use these at all but we must initialize them
    // because SWIG writes some default info to them and crashes if they are not there.
    File *b_begin;
    File *b_runtime;
    File *b_header;
    File *b_wrappers;
    File *b_init;

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

   sectionList_.appendSection(new SectionLibraryObjects);
   sectionList_.appendSection(new SectionValueObjects);
   sectionList_.appendSection(new SectionSerializationCreate);
   sectionList_.appendSection(new SectionSerializationRegister);

    for (int i = 1; i < argc; i++) {
        if ( (strcmp(argv[i],"-genc++") == 0)) {
            //sectionList_.appendSection(new SectionCpp);
            Swig_mark_arg(i);
        } else if ( (strcmp(argv[i],"-genxll") == 0)) {
            sectionList_.appendSection(new SectionExcelFunctions);
            sectionList_.appendSection(new SectionExcelRegister);
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
            //legacy = true;
            Swig_mark_arg(i);
        }
    }
}

virtual int top(Node *n) {

    printf("Generating code.\n");
    b_debug = NewString("");
    Printf(b_debug, "BEGIN top() %s.\n", Getattr(n, "name"));

    /* Get the module name */
    stack.setModule(Getattr(n, "name"));

    // Capture module properties
    Node *m = Getattr(n, "module");
    Node *o = Getattr(m, "options");
    objDir = Getattr(o, "rp_obj_dir");
    objInc = Getattr(o, "rp_obj_inc");
    addDir = Getattr(o, "rp_add_dir");
    addInc = Getattr(o, "rp_add_inc");
    xllDir = Getattr(o, "rp_xll_dir");
    xllInc = Getattr(o, "rp_xll_inc");

    /* Initialize I/O */
    b_begin = NewString("");
    b_runtime = NewString("");
    b_header = NewString("");
    b_wrappers = NewString("");
    b_init = NewString("");

    /* Register file targets with the SWIG file handler */
    Swig_register_filebyname("begin", b_begin);
    Swig_register_filebyname("runtime", b_runtime);
    Swig_register_filebyname("header", b_header);
    Swig_register_filebyname("wrapper", b_wrappers);
    Swig_register_filebyname("init", b_init);

    sectionList_.top();

    /* Emit code for children */
    Language::top(n);

    sectionList_.clear();

    /* Cleanup files */
    Delete(b_begin);
    Delete(b_runtime);
    Delete(b_header);
    Delete(b_wrappers);
    Delete(b_init);

    Printf(b_debug, "END top() %s.\n", Getattr(n, "name"));
    printf("Generating log file debug.txt.\n");
    File *f_test = initFile("debug.txt");
    Dump(b_debug, f_test);
    Delete(b_debug);

    report.print();

    List *x = Keys(errorList);
    for (int i=0; i<Len(x); ++i) {
        String *errorMessage = Getitem(x, i);
        printf("%s", Char(errorMessage));
    }
    Delete(x);
    Delete(errorList);//FIXME also delete each item individually

    printf("Done.\n");
    return SWIG_OK;
}

// overrride base class members, write debug info to b_init,
// and possibly pass control to a handler.

int moduleDirective(Node *n) {
    LogStack l(b_debug, n, "moduleDirective");
    return Language::moduleDirective(n);
}

int includeDirective(Node *n) {
    LogStack l(b_debug, n, "includeDirective");
    String *name = Getattr(n, "name");
    String *fileName = Swig_file_filename(name);
    String *baseName = Swig_file_basename(fileName);
    stack.setGroupName(baseName);
    int ret=Language::includeDirective(n);
    sectionList_.deleteGroup();
    stack.clearGroup();
    return ret;
}

int pragmaDirective(Node *n) {
    LogStack l(b_debug, n, "pragmaDirective");

    if (0 == Strcmp("reposit", Getattr(n, "lang"))) {

        String *name = Getattr(n, "name");
        String *value = Getattr(n, "value");
        Printf(b_debug, "pragmaDirective name=%s value=%s.\n", name, value);

        if (0 == Strcmp(name, "group")) {
            Printf(b_debug, "creategroup.\n");
            stack.initializeGroup(value);
            sectionList_.createGroup(stack.groupName_);
        } else if (0 == Strcmp(name, "rp_obj_dir")) {
            if (!objDir) objDir = value;
        } else if (0 == Strcmp(name, "rp_obj_inc")) {
            if (!objInc) objInc = value;
        } else if (0 == Strcmp(name, "rp_add_dir")) {
            if (!addDir) addDir = value;
        } else if (0 == Strcmp(name, "rp_add_inc")) {
            if (!addInc) addInc = value;
        } else if (0 == Strcmp(name, "rp_xll_dir")) {
            if (!xllDir) xllDir = value;
        } else if (0 == Strcmp(name, "rp_xll_inc")) {
            if (!xllInc) xllInc = value;
        } else if (0 == Strcmp(name, "override_obj")) {
            // For the user writing the config file, it is more intuitive to assume automatic (default)
            // unless overridden with '%feature("rp:override_obj");' :
            bool manual = 0 == Strcmp(value, "true");
            // The source code for this SWIG module is cleaner if we think of it the opposite way:
            stack.setAutomatic(!manual);
        } else {
            REPOSIT_SWIG_ERROR("Unrecognized pragma: " << Char(name));
        }
    }

    return Language::pragmaDirective(n);
}

int namespaceDeclaration(Node *n) {
    LogStack l(b_debug, n, "namespaceDeclaration");
    if (stack.active())
        stack.setNamespace(Getattr(n, "name"));
    return Language::namespaceDeclaration(n);
}

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
//BEGIN functionHandler - node name='SimpleLib::func'.  functionHandlerImpl - set functionType=0
//.BEGIN functionWrapper - node name='SimpleLib::func'. functionWrapperImpl - call functionWrapperImplAll then functionWrapperImplFunc
//.END   functionWrapper - node name='SimpleLib::func'.
//END   functionHandler - node name='SimpleLib::func'.

int classDeclaration(Node *n) {
    LogStack l(b_debug, n, "classDeclaration");
    return Language::classDeclaration(n);
}

int classHandler(Node *n) {
    LogStack l(b_debug, n, "classHandler");
    if (stack.active())
        sectionList_.classHandlerBefore();
    int ret=Language::classHandler(n);
    if (stack.active())
        sectionList_.classHandlerAfter();
    return ret;
}

int constructorDeclaration(Node *n) {
    LogStack l(b_debug, n, "constructorDeclaration");
    return Language::constructorDeclaration(n);
}

int constructorHandler(Node *n) {
    LogStack l(b_debug, n, "constructorHandler");
    Setattr(n, "rp:constructor", "1");
    return Language::constructorHandler(n);
}

int memberfunctionHandler(Node *n) {
    LogStack l(b_debug, n, "memberfunctionHandler");
    Setattr(n, "rp:member", "1");
    //l.printParms();
    return Language::memberfunctionHandler(n);
}

int functionHandler(Node *n) {
    LogStack l(b_debug, n, "functionHandler");
    Setattr(n, "rp:function", "1");
    return Language::functionHandler(n);
}

int functionWrapper(Node *n) {
    LogStack l(b_debug, n, "functionWrapper");
    //l.printParms();

    if (!stack.active())
        return Language::functionWrapper(n);

    if (Getattr(n, "rp:constructor")) {
        functionWrapperCtor(n);
        sectionList_.functionWrapperCtor(n);
    } else if (Getattr(n, "rp:member")) {
        functionWrapperMember(n);
        sectionList_.functionWrapperMember(n);
    } else if (Getattr(n, "rp:function")) {
        functionWrapperFunc(n);
        sectionList_.functionWrapperFunc(n);
    }

    return Language::functionWrapper(n);
}

void functionWrapperCtor(Node *n) {

    ParmList *parms = Getattr(n, "parms");
    for (Parm *p = parms; p; p = nextSibling(p))
        processParm(p);

    String *name = Getattr(n, "name");
    
    String *temp = copyUpper(name);
    String *funcName = NewStringf("%s%s", prefix, temp);
    Setattr(n, "rp:funcName", funcName);

    String *s1 = Getattr(n, "feature:rp:rename2");
    if (!s1)
        s1 = Getattr(n, "constructorDeclaration:sym:name");
    String *s2 = copyUpper(s1);
    String *funcRename = NewStringf("%s%s", prefix, s2);
    Setattr(n, "rp:funcRename", funcRename);

    if (String *alias = Getattr(n, "feature:rp:alias"))
        Setattr(n, "rp:alias", NewStringf("%s%s", prefix, alias));

    // Constructors cannot loop but we call the function below in order to set certain default attributes of the node.
    Setattr(n, "rp:loopFunctionNode", n);

    ParmList *parms2 = CopyParmList(parms);
    parms2 = appendParm(parms2, "Permanent", "bool", false, false, "Permanent flag");
    parms2 = appendParm(parms2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    parms2 = appendParm(parms2, "Overwrite", "bool", false, false, "Overwrite flag");
    parms2 = prependParm(parms2, "objectID", "std::string", true, false, "Object ID");
    Setattr(n, "rp:parms2", parms2);
}

void functionWrapperMember(Node *n) {

    ParmList *parms  = Getattr(n, "parms");
    for (Parm *p = parms; p; p = nextSibling(p))
        processParm(p);

    String *name = Getattr(n, "name");
    Node *p = Getattr(n, "parentNode");
    String *memberTypeName = Getattr(p, "name");
    Setattr(n, "rp:memberTypeName", memberTypeName);
    String *className = Getattr(p, "sym:name");
    String *classNameUpper = copyUpper(className);
    String *nameUpper = copyUpper(name);
    String *funcName = NewStringf("%s%s%s", prefix, classNameUpper, nameUpper);
    validateFunctionName(funcName);
    Setattr(n, "rp:funcName", funcName);

    String *funcRename = 0;
    String *s1 = Getattr(n, "feature:rp:rename2");
    if (s1) {
        String *s2 = copyUpper(s1);
        funcRename = NewStringf("%s%s", prefix, s2);
    } else {
        s1 = Getattr(n, "memberfunctionHandler:sym:name");
        String *s2 = copyUpper(s1);
        funcRename = NewStringf("%s%s%s", prefix, classNameUpper, s2);
    }
    validateFunctionName(funcRename);
    Setattr(n, "rp:funcRename", funcRename);

    if (String *rpalias = Getattr(n, "feature:rp:alias")) {
        String *alias = NewStringf("%s%s", prefix, rpalias);
        validateFunctionName(alias);
        Setattr(n, "rp:alias", alias);
    }

    // Getattr(n, "type") - The return type of the member function
    // Getattr(p, "name") - The class type -> rp:memberType
    Node *n2 = NewHash();
    Setfile(n2, Getfile(n));
    Setline(n2, Getline(n));
    Setattr(n2, "type", memberTypeName);
    Setattr(n, "rp:memberType", n2);

    processLoopParameter(n);

    ParmList *loopParameterList = Getattr(n, "rp:loopParameterList");
    ParmList *parms2 = CopyParmList(Getattr(loopParameterList, "nextSibling"));
    parms2 = appendParm(parms2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    parms2 = prependParm(parms2, "objectID", "std::string", true, false, "Object ID");
    Setattr(n, "rp:parms2", parms2);
}

void functionWrapperFunc(Node *n) {

    ParmList *parms  = Getattr(n, "parms");
    for (Parm *p = parms; p; p = nextSibling(p))
        processParm(p);

    String *symName = 0;
    bool staticMember = 1 == checkAttribute(n, "globalfunctionHandler:view", "staticmemberfunctionHandler");
    if (staticMember)
        symName = copyUpper3(Getattr(n, "sym:name"));
    else
        symName = Getattr(n, "sym:name");
    Setattr(n, "rp:symName", symName);

    String *s1 = Getattr(n, "feature:rp:rename2");
    if (!s1)
        s1 = symName;
    String *s2 = copyUpper(s1);
    String *funcName = NewStringf("%s%s", prefix, s2);
    validateFunctionName(funcName);
    Setattr(n, "rp:funcName", funcName);

    processLoopParameter(n);

    ParmList *loopParameterList = Getattr(n, "rp:loopParameterList");
    ParmList *parms2 = CopyParmList(loopParameterList);
    parms2 = appendParm(parms2, "Trigger", "reposit::property_t", true, false, "Dependency tracking trigger");
    Setattr(n, "rp:parms2", parms2);
}

// Test whether this function is a looping function.
// The user indicates a looping function by setting feature rp:loopParameter equal to the name of the parameter on which to loop.
//
// Take copies of the function's return type and parameters.
// If this is a looping function, then in the copies, change the type of the loop parameter and function return value from T to vector<T>.
//
// Here is a list of all relevant properties after the above changes are performed:
//
// n - The original node, with its original return type (T).
// Getattr(n, "parms") - The original parameter list, unchanged.
// Getattr(n, "rp:loopParameterOrig") - A reference to the loop parameter, if there is one, with its return type unchanged (T).
// Getattr(n, "rp:loopParameterList") - A copy of the parameter list.  If this is a looping function then change the type of the loop parameter from T to vector<T>.
// Getattr(n, "rp:loopFunctionNode") - A copy of n.  If this is a looping function then change the type of the function's return value from T to vector<T>.

void processLoopParameter(Node *n) {

    String *functionName = Getattr(n, "name");
    ParmList *loopParameterList = CopyParmList(Getattr(n, "parms"));

    String *loopParameterName = Getattr(n, "feature:rp:loopParameter");
    if (!loopParameterName) {
        Setattr(n, "rp:loopParameterList", loopParameterList);
        Setattr(n, "rp:loopFunctionNode", n);
        return;
    }

    bool found = false;
    for (Parm *p=loopParameterList; p; p=nextSibling(p)) {

        String *name = Getattr(p, "name");
        if (0==Strcmp(loopParameterName, name)) {

            Setattr(n, "rp:loopParameterOrig", CopyParm(p));

            SwigType *t1 = Getattr(p, "type");
            SwigType *t2 = SwigType_base(t1);
            Parm *p1 = NewHash();
            Setattr(p1, "type", t2);
            SwigType *t3 = NewString("std::vector");
            SwigType_add_template(t3, p1);
            Setattr(p, "type", t3);

            Setattr(n, "rp:loopParameter", p);
            Setattr(n, "rp:loopParameterList", loopParameterList);

            found = true;
            break;
        }
    }

    REPOSIT_SWIG_REQUIRE(found,
        "Error processing function '" << Char(functionName) <<
        "' - you specified loop parameter '" << Char(loopParameterName) << "' " <<
        "but the function has no parameter with that name.");

    Node *n2 = Copy(n);
    SwigType *t1 = Getattr(n, "type");
    Parm *p1 = NewHash();
    Setattr(p1, "type", t1);
    SwigType *t2 = NewString("std::vector");
    SwigType_add_template(t2, p1);
    Setattr(n2, "type", t2);
    Setattr(n, "rp:loopFunctionNode", n2);
}

}; // class REPOSIT

extern "C" Language *
swig_reposit(void) {
  return new REPOSIT();
}

