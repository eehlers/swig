
#include "swigmod.h"

String *prefix = 0;

class OBJECTHANDLER : public Language {

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

    // OH buffers
   File *b_cpp_cpp;
   File *b_cpp_hpp;
   File *b_obj_cpp;
   File *b_obj_hpp0;
   File *b_obj_hpp1;
   File *b_val_cpp;
   File *b_val_hpp;

    // OH output files
   File *f_cpp_cpp;
   File *f_cpp_hpp;
   File *f_obj_cpp;
   File *f_obj_hpp;
   File *f_val_cpp;
   File *f_val_hpp;

   String *module;

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

File *initFile(const char *filename) {
   String *outfile = NewStringf(filename);
   File *f = NewFile(outfile, "w", SWIG_output_files());
   if (!f) {
      FileErrorDisplay(outfile);
      SWIG_exit(EXIT_FAILURE);
   }
    return f;
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

    b_cpp_cpp = NewString("");
    b_cpp_hpp = NewString("");
    b_obj_cpp = NewString("");
    b_obj_hpp0 = NewString("");
    b_obj_hpp1 = NewString("");
    b_val_cpp = NewString("");
    b_val_hpp = NewString("");

   /* Output module initialization code */
   Swig_banner(b_begin);

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

    Printf(b_obj_hpp1,"namespace %s {\n", module);
    Printf(b_obj_hpp1, "\n");

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

   /* Emit code for children */
   Language::top(n);

    Printf(b_val_hpp, "} // namespace %s\n", module);
    Printf(b_val_hpp, "\n");
    Printf(b_val_hpp, "} // namespace ValueObjects\n");
    Printf(b_val_hpp, "\n");
    Printf(b_val_hpp, "#endif\n");
    Printf(b_val_hpp, "\n");

    Printf(b_val_cpp, "} // namespace %s\n", module);
    Printf(b_val_cpp, "\n");
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

    // OH output files
    f_cpp_cpp = initFile("AddinCpp/sla.cpp");
    f_cpp_hpp = initFile("AddinCpp/sla.hpp");
    f_obj_cpp = initFile("AddinObjects/obj.cpp");
    f_obj_hpp = initFile("AddinObjects/obj.hpp");
    f_val_cpp = initFile("ValueObjects/vo.cpp");
    f_val_hpp = initFile("ValueObjects/vo.hpp");

   /* Write all to the file */
   //Dump(b_runtime, b_begin);
    Dump(b_cpp_cpp, f_cpp_cpp);
    Dump(b_cpp_hpp, f_cpp_hpp);
    Dump(b_obj_cpp, f_obj_cpp);
    Dump(b_obj_hpp0, f_obj_hpp);    // OH #includes
    Dump(b_header, f_obj_hpp);      // SWIG #includes from the .i file
    Dump(b_obj_hpp1, f_obj_hpp);    // The rest of the file
    Dump(b_val_cpp, f_val_cpp);
    Dump(b_val_hpp, f_val_hpp);
   //Wrapper_pretty_print(b_init, b_begin);

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

    Delete(b_cpp_cpp);
    Delete(b_cpp_hpp);
    Delete(b_obj_cpp);
    Delete(b_obj_hpp0);
    Delete(b_obj_hpp1);
    Delete(b_val_cpp);
    Delete(b_val_hpp);

    Delete(f_cpp_cpp);
    Delete(f_cpp_hpp);
    Delete(f_obj_cpp);
    Delete(f_obj_hpp);
    Delete(f_val_cpp);
    Delete(f_val_hpp);

   return SWIG_OK;
  }

//String *printList2(Node *n) {
//  String   *s = NewString("");
//    while (n) {
//        Append(s, Getattr(n,"type"));
//        Append(s, " ");
//        Append(s, Getattr(n,"name"));
//        Append(s, " ");
//        n = Getattr(n,"nextSibling");
//    }
//    return s;
//}

// "double d, string s"
void emitParmList(ParmList *parms, File *buf, bool first = true, bool skipFirst = false) {
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (skipFirst) {
            skipFirst = false;
            continue;
        }
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
void emitParmList2(ParmList *parms, File *buf, bool first = true, bool skipFirst = false) {
    for (Parm *p = parms; p; p = nextSibling(p)) {
        if (skipFirst) {
            skipFirst = false;
            continue;
        }
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
    Printf(b_cpp_cpp,"//****FUNC*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    String   *symname   = Getattr(n,"sym:name");
    //String   *action = Getattr(n,"wrap:action");

    Printf(b_cpp_hpp,"    %s %s(", type, symname);
    emitParmList(parms, b_cpp_hpp);
    Printf(b_cpp_hpp,");\n");

    Printf(b_cpp_cpp,"%s %s::%s(", type, module, symname);
    emitParmList(parms, b_cpp_cpp);
    Printf(b_cpp_cpp,") {\n");

    Printf(b_cpp_cpp,"    return %s(", name);
    emitParmList2(parms, b_cpp_cpp);
    Printf(b_cpp_cpp,");\n");
    Printf(b_cpp_cpp,"}\n");
}

void printMemb(Node *n) {
    Printf(b_cpp_cpp,"//****MEMB*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    Node *p = Getattr(n,"parentNode");
    String   *cls   = Getattr(p,"sym:name");
    String   *pname   = Getattr(p,"name");
    ParmList *parms  = Getattr(n,"parms");

    while (SwigType *t = SwigType_typedef_resolve(type)) {
        type = t;
    }
    Printf(b_cpp_hpp,"    %s %s%s%s(const std::string &objectID", type, prefix, cls, name);
    emitParmList(parms, b_cpp_hpp, false, true);
    Printf(b_cpp_hpp,");\n");


    Printf(b_cpp_cpp,"%s %s::%s%s%s(const std::string &objectID\n", type, module, prefix, cls, name);
    emitParmList(parms, b_cpp_cpp, false, true);
    Printf(b_cpp_cpp,") {\n");

    Printf(b_cpp_cpp,"    OH_GET_REFERENCE(x, objectID, %s::%s, %s);\n", module, cls, pname);

    Printf(b_cpp_cpp,"    return x->%s(", name);
    emitParmList2(parms, b_cpp_cpp, true, true);
    Printf(b_cpp_cpp,");\n", name);

    Printf(b_cpp_cpp,"}\n");
}

void printCtor(Node *n) {
    Printf(b_cpp_cpp,"//****CTOR*****\n");
    String   *name   = Getattr(n,"name");
    SwigType *type   = Getattr(n,"type");
    ParmList *parms  = Getattr(n,"parms");
    Node *p = Getattr(n,"parentNode");
    String *pname   = Getattr(p,"name");

    while (SwigType *t = SwigType_typedef_resolve(type)) {
        type = t;
    }

    Printf(b_val_hpp,"        class %s%s : public ObjectHandler::ValueObject {\n", prefix, name);
    Printf(b_val_hpp,"            friend class boost::serialization::access;\n");
    Printf(b_val_hpp,"        public:\n");
    Printf(b_val_hpp,"            %s%s() {}\n", prefix, name);
    Printf(b_val_hpp,"            %s%s(\n", prefix, name);
    Printf(b_val_hpp,"                const std::string& ObjectId,\n");
    Printf(b_val_hpp,"                bool Permanent);\n");
    Printf(b_val_hpp,"\n");
    Printf(b_val_hpp,"            const std::set<std::string>& getSystemPropertyNames() const;\n");
    Printf(b_val_hpp,"            std::vector<std::string> getPropertyNamesVector() const;\n");
    Printf(b_val_hpp,"            ObjectHandler::property_t getSystemProperty(const std::string&) const;\n");
    Printf(b_val_hpp,"            void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);\n");
    Printf(b_val_hpp,"\n");
    Printf(b_val_hpp,"        protected:\n");
    Printf(b_val_hpp,"            static const char* mPropertyNames[];\n");
    Printf(b_val_hpp,"            static std::set<std::string> mSystemPropertyNames;\n");
    Printf(b_val_hpp,"            bool Permanent_;\n");
    Printf(b_val_hpp,"\n");
    Printf(b_val_hpp,"            template<class Archive>\n");
    Printf(b_val_hpp,"            void serialize(Archive& ar, const unsigned int) {\n");
    Printf(b_val_hpp,"            boost::serialization::void_cast_register<%s%s, ObjectHandler::ValueObject>(this, this);\n", prefix, name);
    Printf(b_val_hpp,"                ar  & boost::serialization::make_nvp(\"ObjectId\", objectId_)\n");
    Printf(b_val_hpp,"                    & boost::serialization::make_nvp(\"Permanent\", Permanent_)\n");
    Printf(b_val_hpp,"                    & boost::serialization::make_nvp(\"UserProperties\", userProperties);\n");
    Printf(b_val_hpp,"            }\n");
    Printf(b_val_hpp,"        };\n");
    Printf(b_val_hpp,"\n");

    Printf(b_val_cpp,"        const char* %s%s::mPropertyNames[] = {\n", prefix, name);
    Printf(b_val_cpp,"            \"Permanent\"\n");
    Printf(b_val_cpp,"        };\n");
    Printf(b_val_cpp,"\n");
    Printf(b_val_cpp,"        std::set<std::string> %s%s::mSystemPropertyNames(\n", prefix, name);
    Printf(b_val_cpp,"            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(b_val_cpp,"\n");
    Printf(b_val_cpp,"        const std::set<std::string>& %s%s::getSystemPropertyNames() const {\n", prefix, name);
    Printf(b_val_cpp,"            return mSystemPropertyNames;\n");
    Printf(b_val_cpp,"        }\n");
    Printf(b_val_cpp,"\n");
    Printf(b_val_cpp,"        std::vector<std::string> %s%s::getPropertyNamesVector() const {\n", prefix, name);
    Printf(b_val_cpp,"            std::vector<std::string> ret(\n");
    Printf(b_val_cpp,"                mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));\n");
    Printf(b_val_cpp,"            for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();\n");
    Printf(b_val_cpp,"                i != userProperties.end(); ++i)\n");
    Printf(b_val_cpp,"                ret.push_back(i->first);\n");
    Printf(b_val_cpp,"            return ret;\n");
    Printf(b_val_cpp,"        }\n");
    Printf(b_val_cpp,"\n");
    Printf(b_val_cpp,"        ObjectHandler::property_t %s%s::getSystemProperty(const std::string& name) const {\n", prefix, name);
    Printf(b_val_cpp,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(b_val_cpp,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(b_val_cpp,"                return objectId_;\n");
    Printf(b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(b_val_cpp,"                return className_;\n");
    Printf(b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(b_val_cpp,"                return Permanent_;\n");
    Printf(b_val_cpp,"            else\n");
    Printf(b_val_cpp,"                OH_FAIL(\"Error: attempt to retrieve non-existent Property: '\" + name + \"'\");\n");
    Printf(b_val_cpp,"        }\n");
    Printf(b_val_cpp,"\n");
    Printf(b_val_cpp,"        void %s%s::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {\n", prefix, name);
    Printf(b_val_cpp,"            std::string nameUpper = boost::algorithm::to_upper_copy(name);\n");
    Printf(b_val_cpp,"            if(strcmp(nameUpper.c_str(), \"OBJECTID\")==0)\n");
    Printf(b_val_cpp,"                objectId_ = boost::get<std::string>(value);\n");
    Printf(b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"CLASSNAME\")==0)\n");
    Printf(b_val_cpp,"                className_ = boost::get<std::string>(value);\n");
    Printf(b_val_cpp,"            else if(strcmp(nameUpper.c_str(), \"PERMANENT\")==0)\n");
    Printf(b_val_cpp,"                Permanent_ = ObjectHandler::convert2<bool>(value);\n");
    Printf(b_val_cpp,"            else\n");
    Printf(b_val_cpp,"                OH_FAIL(\"Error: attempt to set non-existent Property: '\" + name + \"'\");\n");
    Printf(b_val_cpp,"        }\n");
    Printf(b_val_cpp,"\n");
    Printf(b_val_cpp,"        %s%s::%s%s(\n", prefix, name, prefix, name);
    Printf(b_val_cpp,"                const std::string& ObjectId,\n");
    Printf(b_val_cpp,"                bool Permanent) :\n");
    Printf(b_val_cpp,"            ObjectHandler::ValueObject(ObjectId, \"%s%s\", Permanent),\n", prefix, name);
    Printf(b_val_cpp,"            Permanent_(Permanent) {\n");
    Printf(b_val_cpp,"        }\n");

    Printf(b_obj_hpp1,"\n");
    Printf(b_obj_hpp1,"    class %s : \n", name);
    Printf(b_obj_hpp1,"        public ObjectHandler::LibraryObject<%s> {\n", pname);
    Printf(b_obj_hpp1,"    public:\n");
    Printf(b_obj_hpp1,"        %s(\n", name);
    Printf(b_obj_hpp1,"            const boost::shared_ptr<ObjectHandler::ValueObject>& properties\n");
    emitParmList(parms, b_obj_hpp1, false);
    Printf(b_obj_hpp1,"            ,bool permanent)\n");
    Printf(b_obj_hpp1,"        : ObjectHandler::LibraryObject<%s>(properties, permanent) {\n", pname);
    Printf(b_obj_hpp1,"            libraryObject_ = boost::shared_ptr<%s>(new %s(", pname, pname);
    emitParmList2(parms, b_obj_hpp1);
    Printf(b_obj_hpp1,"));\n");
    Printf(b_obj_hpp1,"        }\n");
    Printf(b_obj_hpp1,"    };\n");
    Printf(b_obj_hpp1,"\n");

    Printf(b_cpp_hpp,"    std::string %s%s(const std::string &objectID", prefix, name);
    emitParmList(parms, b_cpp_hpp, false);
    Printf(b_cpp_hpp,");\n");

    Printf(b_cpp_cpp,"std::string %s::%s%s(const std::string &objectID", module, prefix, name);
    emitParmList(parms, b_cpp_cpp, false);
    Printf(b_cpp_cpp,") {\n");
    Printf(b_cpp_cpp,"    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(\n");
    Printf(b_cpp_cpp,"        new %s::ValueObjects::%s%s(\n", module, prefix, name);
    Printf(b_cpp_cpp,"            objectID, false));\n");
    Printf(b_cpp_cpp,"    boost::shared_ptr<ObjectHandler::Object> object(\n");
    Printf(b_cpp_cpp,"        new %s::%s(\n", module, name);
    Printf(b_cpp_cpp,"            valueObject");
    emitParmList2(parms, b_cpp_cpp, false);
    Printf(b_cpp_cpp,", false));\n");
    Printf(b_cpp_cpp,"    std::string returnValue =\n");
    Printf(b_cpp_cpp,"        ObjectHandler::Repository::instance().storeObject(\n");
    Printf(b_cpp_cpp,"            objectID, object, false, valueObject);\n");
    Printf(b_cpp_cpp,"    return returnValue;\n");
    Printf(b_cpp_cpp,"}\n");
}

//void printNode(Node *n) {
//    List *list1 = Keys(n);
//    for(int i=0; i<Len(list1); ++i) {
//        String *key = Getitem(list1, i);
//        Printf(b_wrappers,"/* %d %s %s */\n", i, key, Getattr(n, key));
//    }
//}
//
//void printList(Node *n) {
//    while (n) {
//        printNode(n);
//        n = Getattr(n,"nextSibling");
//    }
//}

int functionWrapper(Node *n) {
    //String   *name   = Getattr(n,"name");

    //Printf(b_wrappers,"//module=%s\n", module);
    //Printf(b_wrappers,"//XXX***functionWrapper*******\n");
    //printNode(n);
    //printList(Getattr(n, "parms"));

    //Printf(b_wrappers,"//*************\n");

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

