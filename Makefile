CPPFILES = safecolors.cxx
FLTKDIR  = /usr/local/fltk
LDLIBS   = -L/usr/X11R6/lib -L$(FLTKDIR)/lib -lX11 -lXext  -lm -lfltk
OBJECTS  = $(CPPFILES:.cxx=.o)

all: safecolors

safecolors: $(OBJECTS) 
	g++ $(OBJECTS) $(LDLIBS) -o safecolors 


.SUFFIXES:	.cxx .h .o

.cxx.o :
	$(CXX) -g -I. -I$(FLTKDIR) $(CXXFLAGS) -c $< -o $(<:.cxx=.o)
.c.o :
	$(CC) -I. $(CFLAGS) -c -o $@ $<

clean :
	-@ rm -f *.o *.do $(LIBRARY) $(CLEAN) core *~ makedepend safecolors

