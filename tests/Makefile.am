check_PROGRAMS = runner

runner_SOURCES = runner.cpp ArrayTestSuite.cpp

runner_LDADD =

#
# Nothing below this line should need customization on a per-project basis.
#

AM_CPPFLAGS = -I../src/

all: check

# A rule that runs the unit tests
check: $(check_PROGRAMS)
	rm -f tdata*
	@for PROGRAM in $(check_PROGRAMS:%=./%); do echo $$PROGRAM; $$PROGRAM; done

# How to generate the test runner (this is the scaffolding that makes things go)
runner.cpp: 
	cxxtestgen.pl --error-printer --root -o $@

# turn your test files (the headers) into source code
.hpp.cpp:
	cxxtestgen.pl --part  $< -o $@

clean-local:
	rm -f $(runner_SOURCES) $(runner_SOURCES:.cpp=.o) $(check_PROGRAMS) $(check_PROGRAMS:${EXEEXT}=) tdata*

distclean-local: clean-local
	rm -f Makefile

.NOTPARALLEL:
