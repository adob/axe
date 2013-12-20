CC     = colorgcc
CFLAGS = -std=c++1y -Wall -Wextra -O3 -g
DEFINES = -DMY_SYMBOL
INCPATH = -I./

DEPDIR = deps
OBJDIR = objs
CPP_FILES = $(shell find * -type f -name '*.cpp')

# Build a list of the object files to create, based on the .cpps we find
OTMP = $(patsubst %.cpp,%.o,$(CPP_FILES))

# Build the final list of objects
OBJS = $(patsubst %,$(OBJDIR)/%,$(OTMP))

# Build a list of dependency files
DEPS = $(patsubst %.o,$(DEPDIR)/%.d,$(OTMP))



# all: init $(OBJS)
# 	$(CC) -o test $(OBJS)

all: test

test: $(OBJS)
	$(CC) -o test $(OBJS)

init:
	mkdir -p $(DEPDIR)
	mkdir -p $(OBJDIR)

# Pull in dependency info for our objects
-include $(OBJS:.o=.d)

# Compile and generate dependency info
# 1. Compile the .cpp file
# 2. Generate dependency information, explicitly specifying the target name
# 3. The final three lines do a little bit of sed magic. The following
#    sub-items all correspond to the single sed command below:
#    a. sed: Strip the target (everything before the colon)
#    b. sed: Remove any continuation backslashes
#    c. fmt -1: List words one per line
#    d. sed: Strip leading spaces
#    e. sed: Add trailing colons
objs/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CC) $(DEFINES) $(CFLAGS) $(INCPATH) -o $@ -c $<
	$(CC) -MM $(DEFINES) $(CFLAGS) $(INCPATH)  $*.cpp > $*.d
	@#cp -f $*.d $*.d.tmp
	@#sed -e 's/.*://' -e 's/\\$$//' < $*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $*.d
	@#rm -f $*.d.tmp



clean:
	rm -r objs/
	rm -r deps/