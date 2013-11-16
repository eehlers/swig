
%feature("oh:x", "abc") def;
%feature("featurename", "1");

%typemap(in) int {
$1 = PyInt_AsLong($input);
}
