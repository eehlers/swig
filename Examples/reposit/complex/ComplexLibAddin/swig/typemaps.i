
// rp_val_* - valueobjects

%typemap(rp_tm_val_dcl) ComplexLib::Long "ObjectHandler::property_t $1_name_";
%typemap(rp_tm_val_cnv) ComplexLib::Long "value";
%typemap(rp_tm_val_prm) ComplexLib::Long "const ObjectHandler::property_t&";
%typemap(rp_tm_val_prm) ComplexLib::Account::Type "const std::string&";
%typemap(rp_tm_val_prm) boost::shared_ptr<ComplexLib::Color> "const std::string&";

// rp_ser_* - serialization

// rp_obj_* - addin objects

%typemap(rp_tm_obj_cls) ComplexLib::Base * %{
    OH_LIB_CLASS(Base, ComplexLib::Base);
%}

// rp_add_* - C++ addin

%typemap(rp_tm_add_prm) ComplexLib::Long "const ObjectHandler::property_t&";
%typemap(rp_tm_add_prm) ComplexLib::Account::Type "const std::string&";
%typemap(rp_tm_add_prm) boost::shared_ptr<ComplexLib::Color> "const std::string&";

%typemap(rp_tm_add_cnv) ComplexLib::Long %{
    ComplexLib::Long $1_name_cnv =
        ObjectHandler::convert2<ComplexLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(rp_tm_add_cnv) ComplexLib::Account::Type %{
    ComplexLib::Account::Type $1_name_enum =
        ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_tm_add_cnv) boost::shared_ptr<ComplexLib::Color> %{
    boost::shared_ptr<ComplexLib::Color> $1_name_enum =
        ObjectHandler::Create<boost::shared_ptr<ComplexLib::Color> >()($1_name);
%} 

%typemap(rp_tm_add_cll) ComplexLib::Long "$1_name_cnv";
%typemap(rp_tm_add_cll) ComplexLib::Account::Type "$1_name_enum";
%typemap(rp_tm_add_cll) boost::shared_ptr<ComplexLib::Color> "$1_name_enum";

// rp_xll_* - Excel addin

%typemap(rp_tm_xll_cod) ComplexLib::Long "P";
%typemap(rp_tm_xll_cod) ComplexLib::Account::Type "P";
%typemap(rp_tm_xll_cod) boost::shared_ptr<ComplexLib::Color> "P";

%typemap(rp_tm_xll_prm) ComplexLib::Long "OPER*";
%typemap(rp_tm_xll_prm) ComplexLib::Account::Type "const char*";
%typemap(rp_tm_xll_prm) boost::shared_ptr<ComplexLib::Color> "const char*";

%typemap(rp_tm_xll_cnv) ComplexLib::Long %{
        ComplexLib::Long $1_name_cnv = ObjectHandler::convert2<ComplexLib::Long>(
            ObjectHandler::ConvertOper(*$1_name), "$1_name", ComplexLib::Long());

        ObjectHandler::property_t $1_name_any = ObjectHandler::convert2<ObjectHandler::property_t>(
            ObjectHandler::ConvertOper(*$1_name));
%} 

%typemap(rp_tm_xll_cnv) boost::shared_ptr<ComplexLib::Color> %{
        boost::shared_ptr<ComplexLib::Color> $1_name_enum =
            ObjectHandler::Create<boost::shared_ptr<ComplexLib::Color> >()($1_name);
%} 

%typemap(rp_tm_xll_cnv) ComplexLib::Account::Type %{
        ComplexLib::Account::Type $1_name_enum =
            ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_tm_xll_cll_val) ComplexLib::Long "$1_name_any";

%typemap(rp_tm_xll_cll_obj) ComplexLib::Long "$1_name_cnv";
%typemap(rp_tm_xll_cll_obj) ComplexLib::Account::Type "$1_name_enum";
%typemap(rp_tm_xll_cll_obj) boost::shared_ptr<ComplexLib::Color> "$1_name_enum";

