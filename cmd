./configure --prefix=$IDIR \
    PCRE_CONFIG=$IDIR/bin/pcre-config \
    CPPFLAGS=-I$IDIR/include \
    LDFLAGS=-L$IDIR/lib
