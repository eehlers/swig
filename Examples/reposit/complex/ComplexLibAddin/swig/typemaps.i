
// rp_val_* - valueobjects

%typemap(rp_val_declare) ComplexLib::Long "ObjectHandler::property_t $1_name_";
%typemap(rp_val_cnv) ComplexLib::Long "value";
%typemap(rp_val_in) ComplexLib::Long "const ObjectHandler::property_t&";
%typemap(rp_val_in) ComplexLib::Account::Type "const std::string&";
%typemap(rp_val_in) boost::shared_ptr<ComplexLib::Color> "const std::string&";

// rp_ser_* - serialization

// rp_obj_* - addin objects

%typemap(rp_obj_class) ComplexLib::Base * %{
    OH_LIB_CLASS(Base, ComplexLib::Base);
%}

// rp_add_* - C++ addin

%typemap(rp_add_in) ComplexLib::Long "const ObjectHandler::property_t&";
%typemap(rp_add_in) ComplexLib::Account::Type "const std::string&";
%typemap(rp_add_in) boost::shared_ptr<ComplexLib::Color> "const std::string&";

%typemap(rp_add_cnv) ComplexLib::Long %{
    ComplexLib::Long $1_name_cnv =
        ObjectHandler::convert2<ComplexLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(rp_add_cnv) ComplexLib::Account::Type %{
    ComplexLib::Account::Type $1_name_enum =
        ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_add_cnv) boost::shared_ptr<ComplexLib::Color> %{
    boost::shared_ptr<ComplexLib::Color> $1_name_enum =
        ObjectHandler::Create<boost::shared_ptr<ComplexLib::Color> >()($1_name);
%} 

%typemap(rp_add_call) ComplexLib::Long "$1_name_cnv";
%typemap(rp_add_call) ComplexLib::Account::Type "$1_name_enum";
%typemap(rp_add_call) boost::shared_ptr<ComplexLib::Color> "$1_name_enum";

// rp_xll_* - Excel addin

%typemap(rp_xll) ComplexLib::Long "P";
%typemap(rp_xll) ComplexLib::Account::Type "P";
%typemap(rp_xll) boost::shared_ptr<ComplexLib::Color> "P";

%typemap(rp_xll_in) ComplexLib::Long "OPER*";
%typemap(rp_xll_in) ComplexLib::Account::Type "const char*";
%typemap(rp_xll_in) boost::shared_ptr<ComplexLib::Color> "const char*";

%typemap(rp_xll_cnv) ComplexLib::Long %{
        ComplexLib::Long $1_name_cnv = ObjectHandler::convert2<ComplexLib::Long>(
            ObjectHandler::ConvertOper(*$1_name), "$1_name", ComplexLib::Long());

        ObjectHandler::property_t $1_name_any = ObjectHandler::convert2<ObjectHandler::property_t>(
            ObjectHandler::ConvertOper(*$1_name));
%} 

%typemap(rp_xll_cnv) boost::shared_ptr<ComplexLib::Color> %{
        boost::shared_ptr<ComplexLib::Color> $1_name_enum =
            ObjectHandler::Create<boost::shared_ptr<ComplexLib::Color> >()($1_name);
%} 

%typemap(rp_xll_cnv) ComplexLib::Account::Type %{
        ComplexLib::Account::Type $1_name_enum =
            ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_xll_call_val) ComplexLib::Long "$1_name_any";

%typemap(rp_xll_call_obj) ComplexLib::Long "$1_name_cnv";
%typemap(rp_xll_call_obj) ComplexLib::Account::Type "$1_name_enum";
%typemap(rp_xll_call_obj) boost::shared_ptr<ComplexLib::Color> "$1_name_enum";

