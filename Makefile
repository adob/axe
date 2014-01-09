CC     = g++-4.8
CCCOLOR = colorgcc
CFLAGS = -Wno-parentheses -std=c++1y -Wall -Wextra -Wno-sign-compare -g -O0 -fnon-call-exceptions
DEFINES = -DMY_SYMBOL
INCPATH = -iquote src -Iinclude


%.o: %.cpp
	$(C++) -c $(CFLAGS) $*.c -o $*.o
	$(C++) -MM $(CFLAGS) $*.c > $*.d
	
DEPDIR = deps
OBJDIR = objs
CPP_FILES = $(shell find src -type f -name '*.cpp')

# Build a list of the object files to create, based on the .cpps we find
OTMP = $(patsubst %.cpp,%.o,$(CPP_FILES))

# Build the final list of objects
OBJS = $(patsubst %,$(OBJDIR)/%,$(OTMP))

# Build a list of dependency files
DEPS = $(patsubst %.o,$(DEPDIR)/%.d,$(OTMP))

all: init $(OBJS)
	$(CC) -o test $(OBJS) -lrt -g -ldwarf -lelf -fnon-call-exceptions

init:
	mkdir -p $(DEPDIR)
	mkdir -p $(OBJDIR)

# Pull in dependency info for our objects
-include $(DEPS)


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
$(OBJDIR)/%.o : %.cpp
	mkdir -p `dirname $@`
	mkdir -p `dirname $(DEPDIR)/$*.d`
	$(CCCOLOR) $(DEFINES) $(CFLAGS) $(INCPATH) -o $@ -c $<
	$(CC) -MM -MT $(OBJDIR)/$*.o $(DEFINES) $(CFLAGS) $(INCPATH) \
	$*.cpp > $(DEPDIR)/$*.d
	@cp -f $(DEPDIR)/$*.d $(DEPDIR)/$*.d.tmp
	@#sed -e 's/.*://' -e 's/\\\\$$//' < $(DEPDIR)/$*.d.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(DEPDIR)/$*.d
	@rm -f $(DEPDIR)/$*.d.tmp


clean:
	rm -r objs/
	rm -r deps/