CC=gcc
CTYPE=c
CFLAGS=-c -Wall -DPRINT_OUTPUT -DFIXED_POINT
#CFLAGS=-c -Wall -DTIME_TEST -DFIXED_POINT -DPRINT_OUTPUT
INFLAGS=-Iinclude 
LDFLAGS=-L. -lm 
#LDFLAGS=-L. -lm -lrt

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
