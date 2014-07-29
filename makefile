CC=gcc
CTYPE=c

# For fft7+, you must compile with -DFIXED_POINT
#CFLAGS=-c -Wall -DTIME_TEST -DFIXED_POINT -DPRINT_OUTPUT
CFLAGS=-c -Wall -DPRINT_OUTPUT -DFIXED_POINT


INFLAGS=-Iinclude 

# -lrt is needed for timing libraries on linux machines
#LDFLAGS=-L. -lm -lrt
LDFLAGS=-L. -lm 


OBJDIR:=objdir
SOURCES=main.$(CTYPE) fft8.$(CTYPE) fft_util.$(CTYPE)
OBJECTS=$(addprefix $(OBJDIR)/, $(SOURCES:.$(CTYPE)=.o) )

EXECUTABLE=noin

# -g allow for debugging
# -E  amcro expanded
# -pg
# -S

all: $(SOURCES) $(EXECUTABLE)

$(OBJDIR)/%.o: %.$(CTYPE)
	$(CC) $(CFLAGS) $(INFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJECTS): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

clean:	
	rm -rf $(OBJDIR)  noin* *.o *~ *.test
