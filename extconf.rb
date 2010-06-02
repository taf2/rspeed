require 'mkmf'
$CFLAGS << ' -I ../ -DPAGESPEED_PNG_OPTIMIZER_GIF_READER -DNDEBUG -DUSE_SYSTEM_ZLIB -DUSE_SYSTEM_LIBPNG -DUSE_SYSTEM_LIBJPEG'

# XXX: linker order is hacky..
$LIBS << ' -L ../out/lib -lpng12 -ljpeg -lz -lstdc++ -lm -lbase -lcJSON -ljsmin -loptipng -lpagespeed  -lprotobuf -lgoogleurl -lbase -ljsmin -loptipng'

create_makefile('speed_core')
