
%feature("oh:x", "abc") def;
%feature("featurename", "1");

%typemap(excel) std::string "C";
%typemap(excel) long "N";

