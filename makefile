CC=gcc
CTYPE=c
#CFLAGS=-c -Wall
CFLAGS=-c -Wall -DTIME_TEST
INFLAGS=-Iinclude
LDFLAGS=-L. -lm -lrt

OBJDIR:=objdir
SOURCES=main.$(CTYPE) fft2.$(CTYPE) fft_util.$(CTYPE)
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
