
#include "swigmod.h"

String *prefix = 0;

class OBJECTHANDLER : public Language {
protected:
   /* General DOH objects used for holding the strings */
   File *f_begin;
   File *f_runtime;
   File *f_header;
   File *f_wrappers;
   File *f_init;
   File *f_test;
   File *f_excel_wrappers;
   File *f_excel_file;
   String *module;
    Wrapper *w;
    Wrapper *w2;
public:

  virtual void main(int argc, char *argv[]) {
    printf("I'm the Reposit module.\n");

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
   /*String **/module = Getattr(n,"name");

   /* Initialize I/O */
   String *outfile = NewStringf("%s.cpp", module);
   f_begin = NewFile(outfile, "w", SWIG_output_files());
   if (!f_begin) {
      FileErrorDisplay(outfile);
      SWIG_exit(EXIT_FAILURE);
   }
   f_runtime = NewString("");
   f_init = NewString("");
   f_header = NewString("");
   f_wrappers = NewString("");
   String *testfile = NewStringf("%s.hpp", module);
   f_test = NewFile(testfile, "w", SWIG_output_files());
   if (!f_test) {
      FileErrorDisplay(testfile);
      SWIG_exit(EXIT_FAILURE);
   }
   String *excelfile = NewStringf("%sXL.cpp", module);
   f_excel_file = NewFile(excelfile, "w", SWIG_output_files());
   if (!f_excel_file) {
      FileErrorDisplay(excelfile);
      SWIG_exit(EXIT_FAILURE);
   }
   f_excel_wrappers = NewString("");

   /* Register file targets with the SWIG file handler */
   Swig_register_filebyname("begin", f_begin);
   Swig_register_filebyname("header", f_header);
   Swig_register_filebyname("wrapper", f_wrappers);
   Swig_register_filebyname("runtime", f_runtime);
   Swig_register_filebyname("init", f_init);
   Swig_register_filebyname("test", f_test);
   Swig_register_filebyname("excel_wrappers", f_excel_wrappers);
   Swig_register_filebyname("excel_file", f_excel_file);

    w = NewWrapper();
    w2 = NewWrapper();

   /* Output module initialization code */
   //Swig_banner(f_begin);

    Printf(f_test,"#include <string>\n");
    Printf(f_header,"#include \"%s.hpp\"\n", module);
    Printf(f_header,"#include <oh/objecthandler.hpp>\n");
    Printf(f_init, "static ObjectHandler::Repository repository;\n");
    Printf(w2->code,"namespace %s {\n", module);

    Printf(f_excel_wrappers, "#include <windows.h>\n");
    Printf(f_excel_wrappers, "#include \"xlcall.h\"\n");
    Printf(f_excel_wrappers, "#include \"framewrk.hpp\"\n");
    Printf(f_excel_wrappers, "#include <fstream>\n");
    Printf(f_excel_wrappers, "using namespace std;\n");
    Printf(f_excel_wrappers, "\n");
    Printf(f_excel_wrappers, "#define g_rgWorksheetFuncsRows 2\n");
    Printf(f_excel_wrappers, "#define g_rgWorksheetFuncsCols 10\n");
    Printf(f_excel_wrappers, "\n");
    Printf(f_excel_wrappers, "static LPSTR g_rgWorksheetFuncs[g_rgWorksheetFuncsRows][g_rgWorksheetFuncsCols] =\n");
    Printf(f_excel_wrappers, "{\n");
    Printf(f_excel_wrappers, "{\" Junk2\", \" EPP\", \" Junk2\", \" Arg 1, Arg 2\", \" 1\", \" Generic Add-In\", \" \", \" \", \" Returns the product of the numbers\", \" \"},\n");
    Printf(f_excel_wrappers, "{\" Junk3\", \" A\"  , \" Junk3\", \" \",             \" 2\", \" Generic Add-In\", \" \", \" \", \" Returns the product of the numbers\", \" \"},\n");
    Printf(f_excel_wrappers, "};\n");
    Printf(f_excel_wrappers, "\n");
    Printf(f_excel_wrappers, "__declspec(dllexport) int WINAPI xlAutoOpen(void) {\n");
    Printf(f_excel_wrappers, "  static XLOPER xDLL;\n");
    Printf(f_excel_wrappers, "  Excel(xlGetName, &xDLL, 0);\n");
    Printf(f_excel_wrappers, "  for (int i=0; i<g_rgWorksheetFuncsRows; i++) {\n");
    Printf(f_excel_wrappers, "		Excel(xlfRegister, 0, 1+ g_rgWorksheetFuncsCols,\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) &xDLL,\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][0]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][1]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][2]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][3]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][4]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][5]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][6]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][7]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][8]),\n");
    Printf(f_excel_wrappers, "	    	(LPXLOPER) TempStrNoSize(g_rgWorksheetFuncs[i][9]));\n");
    Printf(f_excel_wrappers, "    }\n");
    Printf(f_excel_wrappers, "  return 1;\n");
    Printf(f_excel_wrappers, "}\n");
    Printf(f_excel_wrappers, "\n");
    Printf(f_excel_wrappers, "__declspec(dllexport) int WINAPI xlAutoClose(void){\n");
    Printf(f_excel_wrappers, "    return 1;\n");
    Printf(f_excel_wrappers, "}\n");

   /* Emit code for children */
   Language::top(n);
   Printf(w2->code,"}\n");

    Wrapper_print(w, f_wrappers);
    DelWrapper(w);
    Wrapper_print(w2, f_test);
    DelWrapper(w2);

   /* Write all to the file */
   //Dump(f_runtime, f_begin);
   Dump(f_header, f_begin);
   Dump(f_wrappers, f_begin);
   Wrapper_pretty_print(f_init, f_begin);

   Dump(f_excel_wrappers, f_excel_file);

   /* Cleanup files */
   Delete(f_runtime);
   Delete(f_header);
   Delete(f_wrappers);
   Delete(f_init);
   //Close(f_begin);
   Delete(f_begin);
   Delete(f_test);
   Delete(f_excel_wrappers);
   Delete(f_excel_file);

   return SWIG_OK;
  }

String *printList2(Node *n) {
  String   *s = NewString("");
    //bool first = true;
    while (n) {
        Append(s, Getattr(n,"type"));
        Append(s, " ");
        Append(s, Getattr(n,"name"));
        Append(s, " ");
        n = Getattr(n,"nextSibling");
    }
    return s;
}

// "double d, string s"
void emitParmList(ParmList *parms, String *buf, bool first = true) {
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        SwigType *type  = Getattr(p,"type");
        String   *name  = Getattr(p,"name");
        while (SwigType *t = SwigType_typedef_resolve(type)) {
            type = t;
        }
        Printf(buf, "%s %s", type, name);
    }
}

// "d, s"
void emitParmList2(ParmList *parms, String *buf, bool first = true) {
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (first) {
            first = false;
        } else {
            Append(buf,", ");
        }
        String *name  = Getattr(p,"name");
        Printf(buf, "%s", name);
    }
}

void printFunc(Node *n) {
    Append(w->code,"//****FUNC*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    //String   *parmstr= ParmList_str_defaultargs(parms); // to string
    String   *symname   = Getattr(n,"sym:name");
    //String   *func   = SwigType_str(type, NewStringf("%s(%s)", name, parmstr));
    String   *action = Getattr(n,"wrap:action");

    Printf(w2->code,"    %s %s(", type, symname);
    emitParmList(parms, w2->code);
    Append(w2->code,");\n");

    Printf(w->code,"%s %s::%s(", type, module, symname);
    emitParmList(parms, w->code);
    Append(w->code,") {\n");

    Printf(w->code,"    return %s(", name);
    emitParmList2(parms, w->code);
    Append(w->code,");\n");
    Append(w->code,"}\n");
}

void printMemb(Node *n) {
    Append(w->code,"//****MEMB*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    //ParmList *parms  = Getattr(n,"parms");
    //String   *parmstr= ParmList_str_defaultargs(parms); // to string
    //String   *func   = SwigType_str(type, NewStringf("%s(%s)", name, parmstr));
    Node *p = Getattr(n,"parentNode");
    String   *cls   = Getattr(p,"sym:name");
    String   *pname   = Getattr(p,"name");

    while (SwigType *t = SwigType_typedef_resolve(type)) {
        type = t;
    }
    Printf(w2->code,"    %s %s%s%s(const std::string &objectID);\n", type, prefix, cls, name);

    Printf(w->code,"%s %s::%s%s%s(const std::string &objectID) {\n", type, module, prefix, cls, name);
    Printf(w->code,"    OH_GET_REFERENCE(x, objectID, %s::%s, %s);\n", module, cls, pname);
    Printf(w->code,"    return x->%s();\n", name);
    Append(w->code,"}\n");
}

void printCtor(Node *n) {
    Append(w->code,"//****CTOR*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    Node *p = Getattr(n,"parentNode");
    String *pname   = Getattr(p,"name");

    while (SwigType *t = SwigType_typedef_resolve(type)) {
        type = t;
    }
    Printf(w2->code,"    std::string %s%s(const std::string &objectID", prefix, name);
    emitParmList(parms, w2->code, false);
    Append(w2->code,");\n");

    Printf(w->def,"namespace %s {\n", module);
    Append(w->def,"\n");
    Printf(w->def,"    class %s : \n", name);
    Printf(w->def,"        public ObjectHandler::LibraryObject<%s> {\n", pname);
    Append(w->def,"    public:\n");
    Printf(w->def,"        %s(\n", name);
    Append(w->def,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties\n");
    emitParmList(parms, w->def, false);
    Append(w->def,"            ,bool permanent)\n");
    Printf(w->def,"        : ObjectHandler::LibraryObject<%s>(properties, permanent) {\n", pname);
    Printf(w->def,"            libraryObject_ = boost::shared_ptr<%s>(new %s(", pname, pname);
    emitParmList2(parms, w->def);
    Append(w->def,"));\n");
    Append(w->def,"        }\n");
    Append(w->def,"    };\n");
    Append(w->def,"\n");
    Append(w->def,"    namespace ValueObjects {\n");
    Append(w->def,"\n");
    Printf(w->def,"        class %s%s : public ObjectHandler::ValueObject {\n", prefix, name);
    Append(w->def,"            friend class boost::serialization::access;\n");
    Append(w->def,"        public:\n");
    Printf(w->def,"            %s%s() {}\n", prefix, name);
    Printf(w->def,"            %s%s(\n", prefix, name);
    Append(w->def,"                const std::string& ObjectId,\n");
    Append(w->def,"                bool Permanent);\n");
    Append(w->def,"\n");
    Append(w->def,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
    Append(w->def,"            std::vector<std::string> getPropertyNamesVector() const;\n");
    Append(w->def,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
    Append(w->def,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
    Append(w->def,"\n");
    Append(w->def,"        protected:\n");
    Append(w->def,"            static const char* mPropertyNames[];\n");
    Append(w->def,"            static std::set<std::string> mSystemPropertyNames;\n");
    Append(w->def,"            bool Permanent_;\n");
    Append(w->def,"\n");
    Append(w->def,"            template<class Archive>\n");
    Append(w->def,"            void serialize(Archive& ar, const unsigned int) {\n");
    Printf(w->def,"            boost::serialization::void_cast_register<%s%s, ObjectHandler::ValueObject>(this, this);\n", prefix, name);
    Append(w->def,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
    Append(w->def,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
    Append(w->def,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
    Append(w->def,"            }\n");
    Append(w->def,"        };\n");
    Append(w->def,"\n");
    Printf(w->def,"        const char* %s%s::mPropertyNames[] = {\n", prefix, name);
    Append(w->def,"            \"Permanent\"\n");
    Append(w->def,"        };\n");
    Append(w->def,"\n");
    Printf(w->def,"        std::set<std::string> %s%s::mSystemPropertyNames(\n", prefix, name);
    Append(w->def,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Append(w->def,"\n");
    Printf(w->def,"        const std::set<std::string>& %s%s::getSystemPropertyNames() const {\n", prefix, name);
    Append(w->def,"            return mSystemPropertyNames;\n");
    Append(w->def,"        }\n");
    Append(w->def,"\n");
    Printf(w->def,"        std::vector<std::string> %s%s::getPropertyNamesVector() const {\n", prefix, name);
    Append(w->def,"            std::vector<std::string> ret(\n");
    Append(w->def,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Append(w->def,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
    Append(w->def,"                i != userProperties.end(); ++i)\n");
    Append(w->def,"                ret.push_back(i->first);\n");
    Append(w->def,"            return ret;\n");
    Append(w->def,"        }\n");
    Append(w->def,"\n");
    Printf(w->def,"        ObjectHandler::property_t %s%s::getSystemProperty(const std::string& name) const {\n", prefix, name);
    Append(w->def,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Append(w->def,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Append(w->def,"                return objectId_;\n");
    Append(w->def,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Append(w->def,"                return className_;\n");
    Append(w->def,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Append(w->def,"                return Permanent_;\n");
    Append(w->def,"            else\n");
    Append(w->def,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
    Append(w->def,"        }\n");
    Append(w->def,"\n");
    Printf(w->def,"        void %s%s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", prefix, name);
    Append(w->def,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Append(w->def,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Append(w->def,"                objectId_ = boost::get<std::string>(value);\n");
    Append(w->def,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Append(w->def,"                className_ = boost::get<std::string>(value);\n");
    Append(w->def,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Append(w->def,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
    Append(w->def,"            else\n");
    Append(w->def,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
    Append(w->def,"        }\n");
    Append(w->def,"\n");
    Printf(w->def,"        %s%s::%s%s(\n", prefix, name, prefix, name);
    Append(w->def,"                const std::string& ObjectId,\n");
    Append(w->def,"                bool Permanent) :\n");
    Printf(w->def,"            ObjectHandler::ValueObject(ObjectId, \"%s%s\", Permanent),\n", prefix, name);
    Append(w->def,"            Permanent_(Permanent) {\n");
    Append(w->def,"        }\n");
    Append(w->def,"    }\n");
    Append(w->def,"}\n");

    Printf(w->code,"std::string %s::%s%s(const std::string &objectID", module, prefix, name);
    emitParmList(parms, w->code, false);
    Append(w->code,") {\n");

    Append(w->code,"    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(w->code,"        new %s::ValueObjects::%s%s(\n", module, prefix, name);
    Append(w->code,"            objectID, false));\n");
    Append(w->code,"    boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(w->code,"        new %s::%s(\n", module, name);
    Append(w->code,"            valueObject");
    emitParmList2(parms, w->code, false);
    Append(w->code,", false));\n");
    Append(w->code,"    std::string returnValue =\n");
    Append(w->code,"        ObjectHandler::Repository::instance().storeObject(\n");
    Append(w->code,"            objectID, object, false, valueObject);\n");
    Append(w->code,"    return returnValue;\n");
    Append(w->code,"}\n");

    Printf(f_excel_wrappers, "char* __stdcall %s(\n", name);
    Printf(f_excel_wrappers, "    char *ObjectId");
    for (Parm *p = parms; p; p = nextSibling(p)) {
        Append(buf,",\n");
        String *name  = Getattr(p,"name");
        Printf(buf, "OPER *%s", name);
    }
    Printf(f_excel_wrappers, ") {\n");
    Printf(f_excel_wrappers, "}\n");
}

void printNode(Node *n) {
    List *list1 = Keys(n);
    for(int i=0; i<Len(list1); ++i) {
        String *key = Getitem(list1, i);
        Printf(f_wrappers,"/* %d %s %s */\n", i, key, Getattr(n, key));
    }
}

void printList(Node *n) {
    while (n) {
        printNode(n);
        n = Getattr(n,"nextSibling");
    }
}

int functionWrapper(Node *n) {
  String   *name   = Getattr(n,"name");

    Printf(f_runtime,"//f_runtime name=%s\n", name);
    Printf(f_header,"//f_header name=%s\n", name);
    Printf(f_init,"//f_init name=%s\n", name);
    Printf(f_begin,"//f_begin name=%s\n", name);

    Printf(f_wrappers,"//module=%s\n", module);
    Printf(f_wrappers,"//XXX***functionWrapper*******\n");
    printNode(n);
    printList(Getattr(n, "parms"));

    Printf(f_wrappers,"//*************\n");

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

};

extern "C" Language *
swig_objecthandler(void) {
  return new OBJECTHANDLER();
}

