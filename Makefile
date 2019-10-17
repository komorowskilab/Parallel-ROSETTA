################################################################################
#
# File:         Makefile
# Description:  A simple GNU make file for compiling the full ROSETTA C++ 
#               library. See comments below. You can fiddle around with this
#               make file if you want to, e.g., only use portions of the
#               library.
# Author:       Written by Staal A. Vinterbo (staalv@idi.ntnu.no). 
#               Documented by Aleksander ?hrn (aleks@idi.ntnu.no).
#
################################################################################

################################################################################
#
# Specify preprocessor flags. (See library documentation.)
#
################################################################################

#should be set by configure

#ugly hack but after autoconf it should disappear
# -DMPI should be "-DMPI "

#DEFINES = -DGCC_STL -DLINUX
DEFINES = -DGCC_STL -DLINUX -DX86_64
#DEFINES = -DGCC_STL -DLINUX -DI386
#DEFINES = -DGCC_STL -DLINUX -DX86_64 -DPARALLEL -DMPI 
#DEFINES = -DGCC_STL -DLINUX -DPARALLEL -DMPI 

################################################################################
#
# Specify which compiler you are using.
#
################################################################################

ifneq (,$(findstring PARALLEL, $(DEFINES)))
	ifneq (,$(findstring MPI , $(DEFINES)))
		CC=mpicxx
	endif
	ifneq (,$(findstring MPI2, $(DEFINES)))
		CC=/home/tinkerer/work/mpich2_bin/bin/mpicxx # FIXME use system location
	endif
else
CC=g++
endif


################################################################################
#
# The 'ar' utility maintains groups of files combined into a single 
# archive file.
#
################################################################################
AR = ar -cq

################################################################################
#
# The 'ranlib' utility was used in SunOS 4.x to add a table of contents
# to archive libraries, which converted each archive to a form that could
# be linked more rapidly. Strictly, this is no longer needed as the 'ar' 
# command automatically provides all the functionality 'ranlib' used to 
# provide.
#
################################################################################
RANLIB = ranlib

################################################################################
#
# Specify search order of include paths. (See library documentation.)
#
################################################################################
INCLUDES = -Ikernel/system/stl -I.
#INCLUDES = -Ikernel/system/stl -I/usr/include/cryptopp -I/usr/lib/cryptopp -I.
#INCLUDES = -Ikernel/system/stl -I. -I/home/tinkerer/work/mpich2_bin/include

################################################################################
#
# Specify compiler flags.
#
################################################################################
#CCFLAGS = -c -O0 -g
#CCFLAGS = -c -O2 -fopenmp -msse -msse2 -msse3 -mfpmath=sse -std=c++11
CCFLAGS = -c -O3 -fopenmp -msse -msse2 -msse3 -mfpmath=sse -std=c++11
#CCFLAGS = -c -O0 -fopenmp -g -msse -msse2 -msse3 -mfpmath=sse -std=c++11
#Use gperftools profiler and tcmalloc..
#CCFLAGS = -c -O0 -fopenmp -g

################################################################################
#
# Specify the name of the library file that will be produced.
#
################################################################################
LIBRARY = librosetta.a 

#Addendum to LIBRARY, but it should not be deleted upon completion.
#CRYPTO = -lkeyczar -lcrypto
################################################################################
#
# Specify linker flags.
#
################################################################################
# mpi location is _very temporary one_
LNFLAGS = -L. $(LIBRARY) 
#LNFLAGS = -L. $(LIBRARY) $(CRYPTO)
#With tcmalloc.
#LNFLAGS = -ltcmalloc -L. $(LIBRARY) 

################################################################################
#
# Specify a name for the program/executable that will be produced.
#
################################################################################
ifneq (,$(findstring PARALLEL, $(DEFINES)))
PROGRAM=mprosetta
else
PROGRAM=clrosetta
endif

################################################################################
#
# Identify which source files that make up the core ROSETTA C++ library. 
#
################################################################################
ifneq (,$(findstring PARALLEL, $(DEFINES)))

LIBRARYSOURCES = \
	$(wildcard ./kernel/*.cpp)\
	./kernel/algorithms/kidnapper.cpp\
	$(wildcard ./kernel/algorithms/*.cpp)\
	$(wildcard ./kernel/basic/*.cpp)\
	$(wildcard ./kernel/sav/*.cpp)\
	$(wildcard ./kernel/sav/algorithms/*.cpp)\
	$(wildcard ./kernel/sav/library/ea/*.cpp)\
	$(wildcard ./kernel/sav/library/hits/*.cpp)\
	$(wildcard ./kernel/sav/structures/*.cpp)\
	$(wildcard ./kernel/genomics/algorithms/*.cpp)\
	$(wildcard ./kernel/genomics/utilities/*.cpp)\
	$(wildcard ./kernel/structures/*.cpp)\
	$(wildcard ./kernel/system/*.cpp)\
	$(wildcard ./kernel/system/sys/*.cpp)\
	$(wildcard ./kernel/utilities/*.cpp)\
	$(wildcard ./kernel/parallel/*.cpp)\
	$(wildcard ./common/*.cpp)

else

LIBRARYSOURCES = \
	$(wildcard ./kernel/*.cpp)\
	./kernel/algorithms/kidnapper.cpp\
	$(wildcard ./kernel/algorithms/*.cpp)\
	$(wildcard ./kernel/basic/*.cpp)\
	$(wildcard ./kernel/sav/*.cpp)\
	$(wildcard ./kernel/sav/algorithms/*.cpp)\
	$(wildcard ./kernel/sav/library/ea/*.cpp)\
	$(wildcard ./kernel/sav/library/hits/*.cpp)\
	$(wildcard ./kernel/sav/structures/*.cpp)\
	$(wildcard ./kernel/genomics/algorithms/*.cpp)\
	$(wildcard ./kernel/genomics/utilities/*.cpp)\
	$(wildcard ./kernel/structures/*.cpp)\
	$(wildcard ./kernel/system/*.cpp)\
	$(wildcard ./kernel/system/sys/*.cpp)\
	$(wildcard ./kernel/utilities/*.cpp)\
	$(wildcard ./common/*.cpp)

endif

################################################################################
#
# Specify which object files to incorporate into the library file that we produce.
# The library file will contain the object files from the "core" ROSETTA C++ 
# library.
#
################################################################################
LIBRARYOBJECTS = $(LIBRARYSOURCES:.cpp=.o)

################################################################################
#
# Specify which source files that constitute our main program.
#
################################################################################

ifneq (,$(findstring PARALLEL, $(DEFINES)))
PROGRAMSOURCES=mprosetta.cpp
else
PROGRAMSOURCES=clrosetta.cpp
endif
################################################################################
#
# Specify which object files that constitute our main program.
#
################################################################################

ifneq (,$(findstring PARALLEL, $(DEFINES)))
PROGRAMOBJECTS=mprosetta.o
else
PROGRAMOBJECTS=clrosetta.o
endif
################################################################################
#
# Target specifying how to make 'all'.
#
################################################################################
what:
	@echo "Usage: make target"
	@echo "";
	@echo "Targets:";
	@echo "deps - recompute dependencies" 
	@echo "mprosetta - build mpi enchanced rosetta";
	@echo "clrosetta - build command line rosetta";
	@echo "";

rosetta: $(PROGRAM)

################################################################################
#
# Target specifying how to make our main program.
# -g adds debugging data to the compiled program. Son't use with release candidate.
#
################################################################################
$(PROGRAM): $(notdir $(PROGRAMOBJECTS)) $(LIBRARY)
	$(CC) -o $@ $(PROGRAMOBJECTS) $(LNFLAGS) -fopenmp -Wall
#	$(CC) -o $@ $(PROGRAMOBJECTS) $(LNFLAGS) -fopenmp -Wall -g
#	$(CC) -o $@ $(PROGRAMOBJECTS) $(LNFLAGS) -Wall --lefence -g

################################################################################
#
# Target specifying how to make the library.
#
################################################################################
$(LIBRARY): $(LIBRARYOBJECTS)
	rm -f $(LIBRARY)
	$(AR) $@ $(LIBRARYOBJECTS)
	$(RANLIB) $@

################################################################################
#
# Target specifying how to make object files from source files.
#
################################################################################
%.o: %.cpp
	$(CC) -o $(<:.cpp=.o) $(CCFLAGS) $(INCLUDES) $(DEFINES) $<

################################################################################
#
# Target specifying how to clean up all intermediate files.
#
################################################################################
clean:
	rm -f $(LIBRARYOBJECTS) $(PROGRAMOBJECTS) Makefile~

################################################################################
#
# Target specifying how to make the list of dependencies, appended to the end
# of this file. (Note that -MM is a g++ specific option.)
#
################################################################################

deps:
	cp Makefile Makefile~
	rm Makefile
	cat Makefile~ | gawk "{print} /^#--Dep/ {exit}" > Makefile
	$(CC) -MM $(INCLUDES) $(DEFINES) $(PROGRAMSOURCES) $(LIBRARYSOURCES) >> Makefile

################################################################################
#
# The 'deps' target above constructs the list of dependencies and appends it
# below.
#
################################################################################
#--Dependencies do not edit this line, or beyond this line ----
clrosetta.o: clrosetta.cpp common/installer.h copyright.h \
 common/objectmanager.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/handle.h kernel/basic/macros.h kernel/basic/message.h \
 kernel/basic/map.h kernel/system/stl/map kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/algorithms/algorithm.h \
 common/configuration.h kernel/utilities/creator.h \
 kernel/utilities/threadcontrol.h
kidnapper.o: kernel/algorithms/kidnapper.cpp stdafx.h copyright.h \
 kernel/algorithms/kidnapper.h kernel/algorithms/scriptalgorithm.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/structure.h
discernibilityfunctionexporter.o: \
 kernel/algorithms/discernibilityfunctionexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/discernibilityfunctionexporter.h \
 kernel/algorithms/discernibilityexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/objectselector.h kernel/structures/reduct.h \
 kernel/algorithms/keyword.h kernel/structures/discernibilitymatrix.h \
 kernel/basic/bits.h kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/basic/set.h \
 kernel/system/stl/set kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/basic/algorithm.h \
 kernel/system/fstream.h kernel/system/stl/fstream
prologreductexporter.o: kernel/algorithms/prologreductexporter.cpp \
 stdafx.h copyright.h kernel/algorithms/prologreductexporter.h \
 kernel/algorithms/reductexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/reduct.h kernel/system/fstream.h \
 kernel/system/stl/fstream
equalfrequencyscaler.o: kernel/algorithms/equalfrequencyscaler.cpp \
 stdafx.h copyright.h kernel/algorithms/equalfrequencyscaler.h \
 kernel/algorithms/orthogonalscaler.h kernel/algorithms/scaler.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
combinatorialcompleter.o: kernel/algorithms/combinatorialcompleter.cpp \
 stdafx.h copyright.h kernel/algorithms/combinatorialcompleter.h \
 kernel/algorithms/completer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/bits.h
serialexecutor.o: kernel/algorithms/serialexecutor.cpp stdafx.h \
 copyright.h kernel/algorithms/serialexecutor.h \
 kernel/algorithms/executor.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h kernel/structures/project.h \
 kernel/structures/projectmanager.h kernel/utilities/creator.h \
 common/objectmanager.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/classifier.h kernel/algorithms/serialexecutorloop.h \
 kernel/utilities/rng.h kernel/algorithms/serialexecutorloop.cpp \
 kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
seminaivescaler.o: kernel/algorithms/seminaivescaler.cpp stdafx.h \
 copyright.h kernel/algorithms/seminaivescaler.h \
 kernel/algorithms/naivescaler.h kernel/algorithms/orthogonalscaler.h \
 kernel/algorithms/scaler.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/algorithm.h
filter.o: kernel/algorithms/filter.cpp stdafx.h copyright.h \
 kernel/algorithms/filter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h
reductexporter.o: kernel/algorithms/reductexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/reductexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/reduct.h kernel/system/fstream.h \
 kernel/system/stl/fstream
naivebayesclassifier.o: kernel/algorithms/naivebayesclassifier.cpp \
 stdafx.h copyright.h kernel/algorithms/naivebayesclassifier.h \
 kernel/algorithms/classifier.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/algorithms/keyword.h kernel/structures/informationvector.h \
 kernel/structures/classification.h kernel/structures/dictionary.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/rule.h kernel/structures/reduct.h \
 kernel/basic/interval.h kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
ruleexporter.o: kernel/algorithms/ruleexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/ruleexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
reductimporter.o: kernel/algorithms/reductimporter.cpp stdafx.h \
 copyright.h kernel/algorithms/reductimporter.h \
 kernel/algorithms/importer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h
htmlreporter.o: kernel/algorithms/htmlreporter.cpp stdafx.h copyright.h \
 kernel/algorithms/htmlreporter.h kernel/algorithms/reporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/system/fstream.h kernel/system/stl/fstream
decisiontableexporter.o: kernel/algorithms/decisiontableexporter.cpp \
 stdafx.h copyright.h kernel/algorithms/decisiontableexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/system/fstream.h kernel/system/stl/fstream
valuesplitter.o: kernel/algorithms/valuesplitter.cpp stdafx.h copyright.h \
 kernel/algorithms/valuesplitter.h kernel/algorithms/splitter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/decisiontables.h kernel/structures/structures.h \
 kernel/utilities/creator.h
ruleevaluator.o: kernel/algorithms/ruleevaluator.cpp stdafx.h copyright.h \
 kernel/algorithms/ruleevaluator.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/macros.h kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h kernel/system/math.h \
 kernel/system/stl/cmath
scriptalgorithm.o: kernel/algorithms/scriptalgorithm.cpp stdafx.h \
 copyright.h kernel/algorithms/scriptalgorithm.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector
partitioner.o: kernel/algorithms/partitioner.cpp stdafx.h copyright.h \
 kernel/algorithms/partitioner.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reduct.h kernel/structures/equivalenceclass.h \
 kernel/structures/equivalenceclasses.h kernel/structures/structures.h \
 kernel/utilities/partitionkit.h kernel/utilities/creator.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath
rulepairwisegeneralizer.o: kernel/algorithms/rulepairwisegeneralizer.cpp \
 stdafx.h copyright.h kernel/algorithms/rulepairwisegeneralizer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/rulegeneralizer.h \
 kernel/algorithms/ruletuner.h kernel/structures/rule.h \
 kernel/structures/structure.h kernel/structures/reduct.h \
 kernel/basic/pair.h kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/generalrule.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/simplecoverfilter.h kernel/algorithms/keyword.h \
 kernel/structures/reducts.h kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h kernel/basic/algorithm.h
orthogonalfilescaler.o: kernel/algorithms/orthogonalfilescaler.cpp \
 stdafx.h copyright.h kernel/algorithms/orthogonalfilescaler.h \
 kernel/algorithms/orthogonalscaler.h kernel/algorithms/scaler.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
generalruleexporter.o: kernel/algorithms/generalruleexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/generalruleexporter.h \
 kernel/algorithms/ruleexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/generalrule.h kernel/structures/rule.h \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
myreductshortener.o: kernel/algorithms/myreductshortener.cpp stdafx.h \
 copyright.h kernel/algorithms/myreductshortener.h \
 kernel/algorithms/reductshortener.h kernel/algorithms/reductfilter.h \
 kernel/algorithms/filter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/reduct.h \
 kernel/structures/structure.h kernel/structures/reducts.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/basic/bits.h
mydecisiontableimporter.o: kernel/algorithms/mydecisiontableimporter.cpp \
 stdafx.h copyright.h kernel/algorithms/mydecisiontableimporter.h \
 kernel/algorithms/decisiontableimporter.h kernel/algorithms/importer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/attribute.h \
 kernel/structures/structure.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/generalizeddecision.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/structures/stringattribute.h kernel/structures/dictionary.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
rulebasedclassifier.o: kernel/algorithms/rulebasedclassifier.cpp stdafx.h \
 copyright.h kernel/algorithms/rulebasedclassifier.h \
 kernel/algorithms/classifier.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h kernel/structures/dictionary.h \
 common/objectmanager.h
kidnapper.o: kernel/algorithms/kidnapper.cpp stdafx.h copyright.h \
 kernel/algorithms/kidnapper.h kernel/algorithms/scriptalgorithm.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/structure.h
conditionedmeancompleter.o: \
 kernel/algorithms/conditionedmeancompleter.cpp stdafx.h copyright.h \
 kernel/algorithms/conditionedmeancompleter.h \
 kernel/algorithms/conditionedcompleter.h kernel/algorithms/completer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/meancompleter.h \
 common/objectmanager.h kernel/structures/structure.h
adxdecisiontableimporter.o: \
 kernel/algorithms/adxdecisiontableimporter.cpp stdafx.h copyright.h \
 kernel/algorithms/adxdecisiontableimporter.h \
 kernel/algorithms/decisiontableimporter.h kernel/algorithms/importer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/attribute.h \
 kernel/structures/structure.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/generalizeddecision.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/structures/stringattribute.h kernel/structures/dictionary.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
manualreducer.o: kernel/algorithms/manualreducer.cpp stdafx.h copyright.h \
 kernel/algorithms/manualreducer.h kernel/algorithms/reducer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/utilities/creator.h
prologruleexporter.o: kernel/algorithms/prologruleexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/prologruleexporter.h \
 kernel/algorithms/ruleexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
simplecoverfilter.o: kernel/algorithms/simplecoverfilter.cpp stdafx.h \
 copyright.h kernel/algorithms/simplecoverfilter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h kernel/structures/informationvector.h \
 kernel/utilities/creator.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/basic/bits.h
myreductfilter.o: kernel/algorithms/myreductfilter.cpp stdafx.h \
 copyright.h kernel/algorithms/myreductfilter.h \
 kernel/algorithms/reductfilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/reduct.h kernel/structures/structure.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h
reyreducer.o: kernel/algorithms/reyreducer.cpp stdafx.h copyright.h \
 kernel/utilities/iokit.h kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/basic/types.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/algorithms/reyreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/system/stddef.h \
 kernel/system/stl/cstddef kernel/basic/handle.h kernel/basic/message.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/basic/map.h kernel/system/stl/map kernel/basic/ids.h \
 kernel/basic/idholder.h kernel/basic/undefined.h kernel/system/limits.h \
 kernel/system/stl/limits kernel/system/float.h kernel/system/stl/cfloat \
 kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/discernibilitymatrix.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
myrulefilter.o: kernel/algorithms/myrulefilter.cpp stdafx.h copyright.h \
 kernel/algorithms/myrulefilter.h kernel/algorithms/rulefilter.h \
 kernel/algorithms/filter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/rule.h \
 kernel/structures/structure.h kernel/structures/reduct.h \
 kernel/basic/pair.h kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
ruleshortener.o: kernel/algorithms/ruleshortener.cpp stdafx.h copyright.h \
 kernel/algorithms/ruleshortener.h kernel/algorithms/ruletuner.h \
 kernel/structures/rule.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/structures/generalrule.h \
 kernel/structures/generalizeddecision.h kernel/basic/set.h \
 kernel/system/stl/set kernel/structures/generalrules.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/algorithm.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/basic/bits.h \
 kernel/algorithms/keyword.h kernel/structures/reducts.h \
 kernel/utilities/creator.h
myreductimporter.o: kernel/algorithms/myreductimporter.cpp stdafx.h \
 copyright.h kernel/algorithms/myreductimporter.h \
 kernel/algorithms/reductimporter.h kernel/algorithms/importer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/reduct.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/creator.h
qualityrulefilterloop.o: kernel/algorithms/qualityrulefilterloop.cpp \
 stdafx.h copyright.h kernel/algorithms/qualityrulefilterloop.h \
 kernel/algorithms/qualityrulefilter.h kernel/algorithms/rulefilter.h \
 kernel/algorithms/filter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/ruleevaluator.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/voter.h kernel/algorithms/rulebasedclassifier.h \
 kernel/algorithms/classifier.h kernel/basic/set.h kernel/system/stl/set \
 kernel/algorithms/keyword.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h \
 kernel/structures/classification.h \
 kernel/structures/rulebasedclassification.h \
 kernel/structures/informationvector.h kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath common/objectmanager.h
brorthogonalscaler.o: kernel/algorithms/brorthogonalscaler.cpp stdafx.h \
 copyright.h kernel/algorithms/brorthogonalscaler.h \
 kernel/algorithms/orthogonalscaler.h kernel/algorithms/scaler.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/algorithms/naivescaler.h kernel/algorithms/johnsonreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/basic/bits.h \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/structures/booleansopfunction.h \
 kernel/structures/reduct.h kernel/structures/reducts.h \
 kernel/structures/structures.h kernel/algorithms/keyword.h \
 kernel/basic/algorithm.h
discernibilityexporter.o: kernel/algorithms/discernibilityexporter.cpp \
 stdafx.h copyright.h kernel/algorithms/discernibilityexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/indiscernibilitygraph.h \
 kernel/structures/graph.h kernel/basic/bits.h \
 kernel/utilities/discerner.h kernel/utilities/creator.h \
 kernel/system/fstream.h kernel/system/stl/fstream
xmlreporter.o: kernel/algorithms/xmlreporter.cpp stdafx.h copyright.h \
 kernel/algorithms/xmlreporter.h kernel/algorithms/reporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/xmlkit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
johnsonreducer.o: kernel/algorithms/johnsonreducer.cpp stdafx.h \
 copyright.h kernel/algorithms/johnsonreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/keyword.h \
 kernel/structures/discernibilitymatrix.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
rulegroupgeneralizer.o: kernel/algorithms/rulegroupgeneralizer.cpp \
 stdafx.h copyright.h kernel/algorithms/rulegroupgeneralizer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/rulegeneralizer.h \
 kernel/algorithms/ruletuner.h kernel/structures/rule.h \
 kernel/structures/structure.h kernel/structures/reduct.h \
 kernel/basic/pair.h kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/generalrule.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/simplecoverfilter.h kernel/algorithms/keyword.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h common/objectmanager.h kernel/basic/algorithm.h
xmldecisiontableexporter.o: \
 kernel/algorithms/xmldecisiontableexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/xmldecisiontableexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/utilities/xmlkit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
saver.o: kernel/algorithms/saver.cpp stdafx.h copyright.h \
 kernel/algorithms/saver.h kernel/algorithms/scriptalgorithm.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/structure.h
xmlreductexporter.o: kernel/algorithms/xmlreductexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/xmlreductexporter.h \
 kernel/algorithms/reductexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/reduct.h kernel/utilities/xmlkit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
conditionedcombinatorialcompleter.o: \
 kernel/algorithms/conditionedcombinatorialcompleter.cpp stdafx.h \
 copyright.h kernel/algorithms/conditionedcombinatorialcompleter.h \
 kernel/algorithms/conditionedcompleter.h kernel/algorithms/completer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/combinatorialcompleter.h \
 common/objectmanager.h kernel/structures/structure.h
xmlruleexporter.o: kernel/algorithms/xmlruleexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/xmlruleexporter.h \
 kernel/algorithms/ruleexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/xmlkit.h
matlabdecisiontableexporter.o: \
 kernel/algorithms/matlabdecisiontableexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/matlabdecisiontableexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/system/fstream.h kernel/system/stl/fstream
prologdecisiontableexporter.o: \
 kernel/algorithms/prologdecisiontableexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/prologdecisiontableexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/utilities/creator.h \
 kernel/system/fstream.h kernel/system/stl/fstream
scaler.o: kernel/algorithms/scaler.cpp stdafx.h copyright.h \
 kernel/algorithms/scaler.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/dictionary.h \
 kernel/structures/floatattribute.h kernel/structures/integerattribute.h \
 kernel/structures/stringattribute.h kernel/basic/algorithm.h \
 kernel/utilities/creator.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/system/fstream.h kernel/system/stl/fstream
parallelexecutor.o: kernel/algorithms/parallelexecutor.cpp stdafx.h \
 copyright.h kernel/algorithms/parallelexecutor.h \
 kernel/algorithms/executor.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
classifier.o: kernel/algorithms/classifier.cpp stdafx.h copyright.h \
 kernel/algorithms/classifier.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/informationvector.h kernel/structures/structure.h
reducer.o: kernel/algorithms/reducer.cpp stdafx.h copyright.h \
 kernel/algorithms/reducer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/discernibilitymatrix.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/utilities/creator.h \
 kernel/algorithms/executor.h kernel/structures/rules.h \
 kernel/structures/rule.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/threadcontrol.h
exporter.o: kernel/algorithms/exporter.cpp stdafx.h copyright.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
cppgeneralruleexporter.o: kernel/algorithms/cppgeneralruleexporter.cpp \
 stdafx.h copyright.h kernel/algorithms/cppgeneralruleexporter.h \
 kernel/algorithms/cppruleexporter.h kernel/algorithms/ruleexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/generalrules.h kernel/structures/generalrule.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h kernel/basic/set.h \
 kernel/system/stl/set kernel/structures/rules.h \
 kernel/structures/structures.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
importer.o: kernel/algorithms/importer.cpp stdafx.h copyright.h \
 kernel/algorithms/importer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h
dynamicreducer.o: kernel/algorithms/dynamicreducer.cpp stdafx.h \
 copyright.h kernel/algorithms/dynamicreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/algorithms/keyword.h \
 kernel/algorithms/rulegenerator.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/rules.h \
 kernel/structures/rule.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream common/objectmanager.h \
 kernel/utilities/creator.h kernel/utilities/discerner.h \
 kernel/structures/graph.h
cppruleexporter.o: kernel/algorithms/cppruleexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/cppruleexporter.h \
 kernel/algorithms/ruleexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
entropyscaler.o: kernel/algorithms/entropyscaler.cpp stdafx.h copyright.h \
 kernel/algorithms/entropyscaler.h kernel/algorithms/orthogonalscaler.h \
 kernel/algorithms/scaler.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/system/math.h \
 kernel/system/stl/cmath
splitter.o: kernel/algorithms/splitter.cpp stdafx.h copyright.h \
 kernel/algorithms/splitter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h
manualscaler.o: kernel/algorithms/manualscaler.cpp stdafx.h copyright.h \
 kernel/algorithms/manualscaler.h kernel/algorithms/scaler.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/interval.h kernel/algorithms/keyword.h \
 kernel/structures/dictionary.h kernel/structures/stringattribute.h \
 kernel/utilities/creator.h
adxdecisiontableexporter.o: \
 kernel/algorithms/adxdecisiontableexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/adxdecisiontableexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/structures/dictionary.h \
 kernel/system/fstream.h kernel/system/stl/fstream
rulefilter.o: kernel/algorithms/rulefilter.cpp stdafx.h copyright.h \
 kernel/algorithms/rulefilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
qualityrulefilter.o: kernel/algorithms/qualityrulefilter.cpp stdafx.h \
 copyright.h kernel/algorithms/qualityrulefilter.h \
 kernel/algorithms/rulefilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/ruleevaluator.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h
completer.o: kernel/algorithms/completer.cpp stdafx.h copyright.h \
 kernel/algorithms/completer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h
myreductexporter.o: kernel/algorithms/myreductexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/myreductexporter.h \
 kernel/algorithms/reductexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/reduct.h kernel/system/fstream.h \
 kernel/system/stl/fstream
algorithm.o: kernel/algorithms/algorithm.cpp stdafx.h copyright.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/structure.h
reporter.o: kernel/algorithms/reporter.cpp stdafx.h copyright.h \
 kernel/algorithms/reporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h
serialexecutorloop.o: kernel/algorithms/serialexecutorloop.cpp stdafx.h \
 copyright.h kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/utilities/rng.h \
 kernel/algorithms/keyword.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/classifier.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/project.h kernel/structures/projectmanager.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/creator.h \
 common/objectmanager.h
ksrulegenerator.o: kernel/algorithms/ksrulegenerator.cpp stdafx.h \
 copyright.h kernel/algorithms/ksrulegenerator.h \
 kernel/algorithms/rulegenerator.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/reduct.h kernel/structures/rules.h \
 kernel/structures/rule.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h
standardvoter.o: kernel/algorithms/standardvoter.cpp stdafx.h copyright.h \
 kernel/algorithms/standardvoter.h kernel/algorithms/voter.h \
 kernel/algorithms/rulebasedclassifier.h kernel/algorithms/classifier.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h kernel/structures/informationvector.h \
 kernel/structures/rulebasedclassification.h \
 kernel/structures/classification.h kernel/utilities/creator.h
holte1rreducer.o: kernel/algorithms/holte1rreducer.cpp stdafx.h \
 copyright.h kernel/algorithms/holte1rreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/rulegenerator.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/rules.h kernel/structures/rule.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h \
 common/objectmanager.h
myposdecisiontableimporter.o: \
 kernel/algorithms/myposdecisiontableimporter.cpp stdafx.h copyright.h \
 kernel/algorithms/myposdecisiontableimporter.h \
 kernel/algorithms/decisiontableimporter.h kernel/algorithms/importer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/basic/bits.h \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/structures/integerattribute.h \
 kernel/structures/dictionary.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/creator.h kernel/basic/algorithm.h
reductshortener.o: kernel/algorithms/reductshortener.cpp stdafx.h \
 copyright.h kernel/algorithms/reductshortener.h \
 kernel/algorithms/reductfilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector
approximator.o: kernel/algorithms/approximator.cpp stdafx.h copyright.h \
 kernel/algorithms/approximator.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/algorithms/partitioner.h kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reduct.h kernel/structures/equivalenceclass.h \
 kernel/structures/equivalenceclasses.h kernel/structures/structures.h \
 kernel/structures/approximation.h kernel/utilities/creator.h \
 common/objectmanager.h
structurecreator.o: kernel/algorithms/structurecreator.cpp stdafx.h \
 copyright.h kernel/algorithms/structurecreator.h \
 kernel/algorithms/scriptalgorithm.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/structure.h \
 kernel/utilities/creator.h
dictionaryimporter.o: kernel/algorithms/dictionaryimporter.cpp stdafx.h \
 copyright.h kernel/algorithms/dictionaryimporter.h \
 kernel/algorithms/importer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/attribute.h kernel/structures/structure.h \
 kernel/structures/dictionary.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/generalizeddecision.h \
 kernel/structures/floatattribute.h kernel/structures/integerattribute.h \
 kernel/structures/stringattribute.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/iokit.h \
 kernel/utilities/creator.h
reductfilter.o: kernel/algorithms/reductfilter.cpp stdafx.h copyright.h \
 kernel/algorithms/reductfilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/reducts.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/reduct.h
conditionedcompleter.o: kernel/algorithms/conditionedcompleter.cpp \
 stdafx.h copyright.h kernel/algorithms/conditionedcompleter.h \
 kernel/algorithms/completer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/valuesplitter.h kernel/algorithms/splitter.h \
 kernel/algorithms/keyword.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/decisiontables.h kernel/structures/structures.h
dictionaryexporter.o: kernel/algorithms/dictionaryexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/dictionaryexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/dictionary.h kernel/system/fstream.h \
 kernel/system/stl/fstream
rulegeneralizer.o: kernel/algorithms/rulegeneralizer.cpp stdafx.h \
 copyright.h kernel/algorithms/rulegeneralizer.h \
 kernel/algorithms/ruletuner.h kernel/structures/rule.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/generalrule.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/algorithm.h kernel/algorithms/keyword.h \
 kernel/utilities/creator.h kernel/basic/algorithm.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h
meancompleter.o: kernel/algorithms/meancompleter.cpp stdafx.h copyright.h \
 kernel/algorithms/meancompleter.h kernel/algorithms/completer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/creator.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
prologgeneralruleexporter.o: \
 kernel/algorithms/prologgeneralruleexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/prologgeneralruleexporter.h \
 kernel/algorithms/generalruleexporter.h kernel/algorithms/ruleexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/generalrule.h kernel/structures/rule.h \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
rulegenerator.o: kernel/algorithms/rulegenerator.cpp stdafx.h copyright.h \
 kernel/algorithms/rulegenerator.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/reducts.h \
 kernel/structures/structures.h kernel/structures/reduct.h
mygeneralrulefilter.o: kernel/algorithms/mygeneralrulefilter.cpp stdafx.h \
 copyright.h kernel/algorithms/mygeneralrulefilter.h \
 kernel/algorithms/myrulefilter.h kernel/algorithms/rulefilter.h \
 kernel/algorithms/filter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/generalrule.h \
 kernel/structures/rule.h kernel/structures/structure.h \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
myruleexporter.o: kernel/algorithms/myruleexporter.cpp stdafx.h \
 copyright.h kernel/algorithms/myruleexporter.h \
 kernel/algorithms/ruleexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
decisiontableimporter.o: kernel/algorithms/decisiontableimporter.cpp \
 stdafx.h copyright.h kernel/algorithms/decisiontableimporter.h \
 kernel/algorithms/importer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h
reductcostfilter.o: kernel/algorithms/reductcostfilter.cpp stdafx.h \
 copyright.h kernel/algorithms/reductcostfilter.h \
 kernel/algorithms/reductfilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/costinformation.h \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/algorithms/keyword.h kernel/structures/reduct.h \
 kernel/structures/structure.h kernel/structures/reducts.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
objecttrackingvoter.o: kernel/algorithms/objecttrackingvoter.cpp stdafx.h \
 copyright.h kernel/algorithms/objecttrackingvoter.h \
 kernel/algorithms/voter.h kernel/algorithms/rulebasedclassifier.h \
 kernel/algorithms/classifier.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/basic/bits.h \
 kernel/structures/informationvector.h \
 kernel/structures/rulebasedclassification.h \
 kernel/structures/classification.h kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/structures/graph.h
indiscernibilitygraphexporter.o: \
 kernel/algorithms/indiscernibilitygraphexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/indiscernibilitygraphexporter.h \
 kernel/algorithms/discernibilityexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/discernibilitymatrix.h \
 kernel/basic/bits.h kernel/structures/indiscernibilitygraph.h \
 kernel/structures/graph.h kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/system/fstream.h \
 kernel/system/stl/fstream
orthogonalscaler.o: kernel/algorithms/orthogonalscaler.cpp stdafx.h \
 copyright.h kernel/algorithms/orthogonalscaler.h \
 kernel/algorithms/scaler.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/dictionary.h \
 kernel/structures/floatattribute.h kernel/structures/integerattribute.h \
 kernel/structures/stringattribute.h kernel/utilities/creator.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/basic/interval.h common/configuration.h
voter.o: kernel/algorithms/voter.cpp stdafx.h copyright.h \
 kernel/algorithms/voter.h kernel/algorithms/rulebasedclassifier.h \
 kernel/algorithms/classifier.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h kernel/structures/informationvector.h \
 kernel/structures/rulebasedclassification.h \
 kernel/structures/classification.h kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h
loader.o: kernel/algorithms/loader.cpp stdafx.h copyright.h \
 kernel/algorithms/loader.h kernel/algorithms/scriptalgorithm.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/structure.h
removalcompleter.o: kernel/algorithms/removalcompleter.cpp stdafx.h \
 copyright.h kernel/algorithms/removalcompleter.h \
 kernel/algorithms/completer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/creator.h
arffdecisiontableimporter.o: \
 kernel/algorithms/arffdecisiontableimporter.cpp stdafx.h copyright.h \
 kernel/algorithms/arffdecisiontableimporter.h \
 kernel/algorithms/decisiontableimporter.h kernel/algorithms/importer.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/attribute.h \
 kernel/structures/structure.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/generalizeddecision.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/structures/stringattribute.h kernel/structures/dictionary.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
executor.o: kernel/algorithms/executor.cpp stdafx.h copyright.h \
 kernel/algorithms/executor.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/keyword.h kernel/utilities/creator.h \
 common/objectmanager.h kernel/utilities/threadcontrol.h
mydecisiontableexporter.o: kernel/algorithms/mydecisiontableexporter.cpp \
 stdafx.h copyright.h kernel/algorithms/mydecisiontableexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/system/fstream.h \
 kernel/system/stl/fstream
cvserialexecutor.o: kernel/algorithms/cvserialexecutor.cpp stdafx.h \
 copyright.h kernel/algorithms/cvserialexecutor.h \
 kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/utilities/rng.h \
 kernel/algorithms/keyword.h kernel/utilities/mathkit.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath
arffdecisiontableexporter.o: \
 kernel/algorithms/arffdecisiontableexporter.cpp stdafx.h copyright.h \
 kernel/algorithms/arffdecisiontableexporter.h \
 kernel/algorithms/decisiontableexporter.h kernel/algorithms/exporter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/structures/dictionary.h \
 kernel/system/fstream.h kernel/system/stl/fstream
binarysplitter.o: kernel/algorithms/binarysplitter.cpp stdafx.h \
 copyright.h kernel/algorithms/binarysplitter.h \
 kernel/algorithms/splitter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/keyword.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/decisiontables.h kernel/structures/structures.h \
 kernel/structures/dictionary.h kernel/utilities/rng.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/creator.h
naivescaler.o: kernel/algorithms/naivescaler.cpp stdafx.h copyright.h \
 kernel/algorithms/naivescaler.h kernel/algorithms/orthogonalscaler.h \
 kernel/algorithms/scaler.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/algorithm.h kernel/system/fstream.h \
 kernel/system/stl/fstream
costinformation.o: kernel/algorithms/costinformation.cpp stdafx.h \
 copyright.h kernel/algorithms/costinformation.h kernel/basic/vector.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/basic/types.h kernel/system/stddef.h \
 kernel/system/stl/cstddef kernel/basic/handle.h kernel/basic/message.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/reduct.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
xmlgeneralruleexporter.o: kernel/algorithms/xmlgeneralruleexporter.cpp \
 stdafx.h copyright.h kernel/algorithms/xmlgeneralruleexporter.h \
 kernel/algorithms/generalruleexporter.h kernel/algorithms/ruleexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/generalrule.h kernel/structures/rule.h \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/xmlkit.h
batchclassifier.o: kernel/algorithms/batchclassifier.cpp stdafx.h \
 copyright.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/classifier.h \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/algorithms/keyword.h kernel/structures/structure.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/structures/classification.h \
 kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h \
 kernel/structures/calibrationcurve.h kernel/structures/dictionary.h \
 kernel/basic/algorithm.h kernel/utilities/creator.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream common/objectmanager.h
objectselector.o: kernel/algorithms/objectselector.cpp stdafx.h \
 copyright.h kernel/algorithms/objectselector.h kernel/basic/macros.h \
 kernel/basic/string.h kernel/basic/types.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/keyword.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/basic/set.h kernel/system/stl/set
ruletuner.o: kernel/algorithms/ruletuner.cpp stdafx.h copyright.h \
 kernel/algorithms/ruletuner.h kernel/structures/rule.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/generalrule.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set \
 kernel/structures/generalrules.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/algorithms/algorithm.h kernel/algorithms/keyword.h \
 kernel/structures/reducts.h kernel/utilities/creator.h
reductperformancefilter.o: kernel/algorithms/reductperformancefilter.cpp \
 stdafx.h copyright.h kernel/algorithms/reductperformancefilter.h \
 kernel/algorithms/reductfilter.h kernel/algorithms/filter.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/algorithms/rulegenerator.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/batchclassifier.h kernel/algorithms/classifier.h \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/algorithms/keyword.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/rules.h kernel/structures/rule.h \
 kernel/basic/interval.h kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 common/objectmanager.h
ids.o: kernel/basic/ids.cpp stdafx.h copyright.h kernel/basic/ids.h \
 kernel/basic/types.h kernel/basic/macros.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/system/stl/vector
message.o: kernel/basic/message.cpp stdafx.h copyright.h \
 kernel/basic/message.h kernel/basic/string.h kernel/basic/types.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/basic/macros.h kernel/system/stl/map common/messagehelper.h \
 kernel/basic/vector.h kernel/system/stl/vector
bits.o: kernel/basic/bits.cpp stdafx.h copyright.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/vector.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/system/limits.h kernel/system/stl/limits \
 kernel/basic/undefined.h kernel/system/float.h kernel/system/stl/cfloat \
 kernel/system/memory.h kernel/system/stl/memory
string.o: kernel/basic/string.cpp stdafx.h copyright.h \
 kernel/basic/string.h kernel/basic/types.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/macros.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/system/ctype.h kernel/system/stl/cctype
persistent.o: kernel/basic/persistent.cpp stdafx.h copyright.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
referent.o: kernel/basic/referent.cpp stdafx.h copyright.h \
 kernel/basic/referent.h kernel/basic/types.h kernel/system/stddef.h \
 kernel/system/stl/cstddef kernel/basic/macros.h kernel/basic/message.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/system/malloc.h
identifier.o: kernel/basic/identifier.cpp stdafx.h copyright.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector
interval.o: kernel/basic/interval.cpp stdafx.h copyright.h \
 kernel/basic/interval.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/message.h \
 kernel/basic/map.h kernel/basic/macros.h kernel/system/stl/map \
 kernel/system/math.h kernel/system/stl/cmath
memory.o: kernel/basic/memory.cpp stdafx.h copyright.h \
 kernel/system/stdlib.h kernel/system/stl/cstdlib
idholder.o: kernel/basic/idholder.cpp stdafx.h copyright.h \
 kernel/basic/idholder.h kernel/basic/types.h kernel/basic/macros.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/system/stl/vector kernel/basic/message.h \
 kernel/basic/map.h kernel/system/stl/map
savgeneticreducer.o: kernel/sav/algorithms/savgeneticreducer.cpp stdafx.h \
 copyright.h kernel/sav/algorithms/savgeneticreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/costinformation.h \
 kernel/sav/library/hits/Hits.h \
 kernel/sav/library/hits/../ea/CrossoverOperator.h \
 kernel/sav/library/hits/../ea/AbstractBinaryOperator.h \
 kernel/sav/library/hits/../ea/Individual.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/algorithm.h \
 kernel/sav/library/hits/../ea/InversionOperator.h \
 kernel/sav/library/hits/../ea/AbstractUnaryOperator.h \
 kernel/sav/library/hits/../ea/MutateOperator.h \
 kernel/sav/library/hits/../ea/CloneOperator.h \
 kernel/sav/library/hits/../ea/BinaryOperation.h \
 kernel/sav/library/hits/../ea/AbstractOperation.h \
 kernel/sav/library/hits/../ea/Population.h \
 kernel/sav/library/hits/../ea/UnaryOperation.h \
 kernel/sav/library/hits/../ea/EliteOperation.h \
 kernel/sav/library/hits/../ea/Stats.h \
 kernel/sav/library/hits/../ea/ParentsSelector.h \
 kernel/sav/library/hits/../ea/AbstractSampler.h \
 kernel/sav/library/hits/../ea/Rand.h kernel/utilities/rng.h \
 kernel/system/math.h kernel/system/stl/cmath \
 kernel/sav/library/hits/../ea/Reaper.h \
 kernel/sav/library/hits/../ea/Evaluator.h \
 kernel/sav/library/hits/../ea/AbstractFitness.h \
 kernel/sav/library/hits/../ea/AbstractEvaluator.h \
 kernel/sav/library/hits/../ea/Stats.h \
 kernel/sav/library/hits/../ea/RecombineReplaceSample.h \
 kernel/sav/library/hits/../ea/RecombineReplaceParents.h \
 kernel/sav/library/hits/../ea/RecombineAdd.h \
 kernel/sav/library/hits/../ea/Boltzscaler.h \
 kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h \
 kernel/sav/library/hits/HitsEngine.h \
 kernel/sav/library/hits/../ea/Engine.h \
 kernel/sav/library/hits/../ea/AbstractInitializer.h \
 kernel/sav/library/hits/../ea/AbstractStoppingCriteria.h \
 kernel/sav/library/hits/../ea/Keeper.h \
 kernel/sav/library/hits/HitsFitness.h \
 kernel/sav/library/hits/../ea/AbstractFitness.h \
 kernel/sav/library/hits/../ea/macros.h \
 kernel/sav/library/hits/HistStop.h \
 kernel/sav/library/hits/../ea/AbstractStoppingCriteria.h \
 kernel/sav/library/hits/HitsInit.h \
 kernel/sav/library/hits/../ea/AbstractInitializer.h \
 kernel/algorithms/keyword.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/mathkit.h
CloneOperator.o: kernel/sav/library/ea/CloneOperator.cpp stdafx.h \
 kernel/sav/library/ea/CloneOperator.h \
 kernel/sav/library/ea/AbstractUnaryOperator.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/system/iostream.h kernel/system/stl/iostream
Stats.o: kernel/sav/library/ea/Stats.cpp stdafx.h \
 kernel/sav/library/ea/Stats.h kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/system/math.h kernel/system/stl/cmath \
 kernel/system/float.h kernel/system/stl/cfloat
CrossoverOperator.o: kernel/sav/library/ea/CrossoverOperator.cpp stdafx.h \
 kernel/sav/library/ea/CrossoverOperator.h \
 kernel/sav/library/ea/AbstractBinaryOperator.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/sav/library/ea/EABitset.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/ea/Rand.h \
 kernel/utilities/rng.h
AbstractBinaryOperator.o: \
 kernel/sav/library/ea/AbstractBinaryOperator.cpp stdafx.h \
 kernel/sav/library/ea/AbstractBinaryOperator.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/system/iostream.h kernel/system/stl/iostream
Engine.o: kernel/sav/library/ea/Engine.cpp stdafx.h \
 kernel/sav/library/ea/Engine.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/Individual.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/sav/library/ea/AbstractInitializer.h \
 kernel/sav/library/ea/AbstractStoppingCriteria.h \
 kernel/sav/library/ea/AbstractEvaluator.h \
 kernel/sav/library/ea/AbstractOperation.h kernel/system/assert.h \
 kernel/system/stl/cassert
RecombineReplaceSample.o: \
 kernel/sav/library/ea/RecombineReplaceSample.cpp stdafx.h \
 kernel/sav/library/ea/RecombineReplaceSample.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/AbstractSampler.h \
 kernel/sav/library/ea/Rand.h kernel/utilities/rng.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/system/assert.h kernel/system/stl/cassert
MutateOperator.o: kernel/sav/library/ea/MutateOperator.cpp stdafx.h \
 kernel/sav/library/ea/MutateOperator.h \
 kernel/sav/library/ea/AbstractUnaryOperator.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/sav/library/ea/EABitset.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/ea/Rand.h \
 kernel/utilities/rng.h
AbstractUnaryOperator.o: kernel/sav/library/ea/AbstractUnaryOperator.cpp \
 stdafx.h kernel/sav/library/ea/AbstractUnaryOperator.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/system/iostream.h kernel/system/stl/iostream
BitVRandomInitializer.o: kernel/sav/library/ea/BitVRandomInitializer.cpp \
 kernel/sav/library/ea/BitVRandomInitializer.h \
 kernel/sav/library/ea/AbstractInitializer.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/Rand.h \
 kernel/utilities/rng.h kernel/sav/library/ea/EABitset.h \
 kernel/basic/bits.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits
RecombineReplaceParents.o: \
 kernel/sav/library/ea/RecombineReplaceParents.cpp stdafx.h \
 kernel/sav/library/ea/RecombineReplaceParents.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream
AbstractStoppingCriteria.o: \
 kernel/sav/library/ea/AbstractStoppingCriteria.cpp stdafx.h \
 kernel/sav/library/ea/AbstractStoppingCriteria.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream
UnaryOperation.o: kernel/sav/library/ea/UnaryOperation.cpp stdafx.h \
 kernel/system/math.h kernel/system/stl/cmath \
 kernel/sav/library/ea/UnaryOperation.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/AbstractUnaryOperator.h
AbstractEvaluator.o: kernel/sav/library/ea/AbstractEvaluator.cpp stdafx.h \
 kernel/sav/library/ea/AbstractEvaluator.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream
AbstractSampler.o: kernel/sav/library/ea/AbstractSampler.cpp stdafx.h \
 kernel/sav/library/ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/sav/library/ea/AbstractSampler.h \
 kernel/sav/library/ea/Rand.h kernel/utilities/rng.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/SampleVector.h
Rand.o: kernel/sav/library/ea/Rand.cpp stdafx.h kernel/basic/message.h \
 copyright.h kernel/basic/string.h kernel/basic/types.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/basic/macros.h kernel/system/stl/map kernel/sav/library/ea/Rand.h \
 kernel/utilities/rng.h
Boltzscaler.o: kernel/sav/library/ea/Boltzscaler.cpp stdafx.h \
 kernel/sav/library/ea/Boltzscaler.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/Stats.h \
 kernel/system/math.h kernel/system/stl/cmath
ExternalFitness.o: kernel/sav/library/ea/ExternalFitness.cpp \
 kernel/sav/library/ea/ExternalFitness.h \
 kernel/sav/library/ea/AbstractFitness.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/system/iostream.h kernel/system/stl/iostream
Reaper.o: kernel/sav/library/ea/Reaper.cpp stdafx.h \
 kernel/sav/library/ea/Reaper.h kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/Stats.h \
 kernel/system/assert.h kernel/system/stl/cassert kernel/basic/message.h \
 kernel/basic/string.h kernel/basic/types.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib
Keeper.o: kernel/sav/library/ea/Keeper.cpp stdafx.h \
 kernel/sav/library/ea/Keeper.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/message.h kernel/basic/string.h \
 kernel/basic/types.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/system/assert.h \
 kernel/system/stl/cassert
InversionOperator.o: kernel/sav/library/ea/InversionOperator.cpp stdafx.h \
 kernel/sav/library/ea/EABitset.h kernel/sav/library/ea/Individual.h \
 kernel/sav/library/ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/bits.h kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/ea/InversionOperator.h \
 kernel/sav/library/ea/AbstractUnaryOperator.h \
 kernel/sav/library/ea/Rand.h kernel/utilities/rng.h
RecombineAdd.o: kernel/sav/library/ea/RecombineAdd.cpp stdafx.h \
 kernel/sav/library/ea/RecombineAdd.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream \
 kernel/sav/library/ea/RecombineReplaceParents.h
BinaryOperator.o: kernel/sav/library/ea/BinaryOperator.cpp stdafx.h \
 kernel/sav/library/ea/BinaryOperator.h
BinaryOperation.o: kernel/sav/library/ea/BinaryOperation.cpp stdafx.h \
 kernel/sav/library/ea/BinaryOperation.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream \
 kernel/sav/library/ea/AbstractBinaryOperator.h kernel/basic/message.h \
 kernel/basic/string.h kernel/basic/types.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib
ParentsSelector.o: kernel/sav/library/ea/ParentsSelector.cpp stdafx.h \
 kernel/sav/library/ea/ParentsSelector.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/AbstractSampler.h \
 kernel/sav/library/ea/Rand.h kernel/utilities/rng.h kernel/system/math.h \
 kernel/system/stl/cmath
EABitset.o: kernel/sav/library/ea/EABitset.cpp stdafx.h \
 kernel/sav/library/ea/EABitset.h kernel/sav/library/ea/Individual.h \
 kernel/sav/library/ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/bits.h kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits
EliteOperation.o: kernel/sav/library/ea/EliteOperation.cpp stdafx.h \
 kernel/sav/library/ea/EliteOperation.h \
 kernel/sav/library/ea/AbstractOperation.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/ea/Stats.h
AbstractInitializer.o: kernel/sav/library/ea/AbstractInitializer.cpp \
 stdafx.h kernel/sav/library/ea/AbstractInitializer.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h copyright.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream
Evaluator.o: kernel/sav/library/ea/Evaluator.cpp stdafx.h \
 kernel/sav/library/ea/Evaluator.h kernel/sav/library/ea/Population.h \
 kernel/sav/library/ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/sav/library/ea/Individual.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/sav/library/ea/AbstractFitness.h \
 kernel/sav/library/ea/AbstractEvaluator.h kernel/sav/library/ea/Stats.h \
 kernel/sav/library/ea/AbstractOperation.h kernel/system/float.h \
 kernel/system/stl/cfloat
ExternalEvaluator.o: kernel/sav/library/ea/ExternalEvaluator.cpp \
 kernel/sav/library/ea/ExternalEvaluator.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/string.h copyright.h \
 kernel/basic/types.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/sav/library/ea/AbstractEvaluator.h \
 kernel/sav/library/ea/Population.h kernel/sav/library/ea/macros.h \
 kernel/basic/vector.h kernel/basic/macros.h kernel/system/stl/vector \
 kernel/basic/map.h kernel/system/stl/map kernel/basic/set.h \
 kernel/system/stl/set kernel/basic/algorithm.h \
 kernel/sav/library/ea/Individual.h kernel/sav/library/ea/Stats.h \
 kernel/sav/library/ea/AbstractOperation.h kernel/system/fstream.h \
 kernel/system/stl/fstream
HitsInit.o: kernel/sav/library/hits/HitsInit.cpp stdafx.h \
 kernel/sav/library/hits/HitsInit.h \
 kernel/sav/library/hits/../ea/AbstractInitializer.h \
 kernel/sav/library/hits/../ea/Population.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/sav/library/hits/../ea/Individual.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/hits/HitsMessage.h \
 kernel/sav/library/hits/../ea/Rand.h kernel/utilities/rng.h \
 kernel/system/assert.h kernel/system/stl/cassert
HistStop.o: kernel/sav/library/hits/HistStop.cpp stdafx.h \
 kernel/sav/library/hits/HistStop.h \
 kernel/sav/library/hits/../ea/AbstractStoppingCriteria.h \
 kernel/sav/library/hits/../ea/Population.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/sav/library/hits/../ea/Individual.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits
Hits.o: kernel/sav/library/hits/Hits.cpp stdafx.h \
 kernel/sav/library/hits/Hits.h \
 kernel/sav/library/hits/../ea/CrossoverOperator.h \
 kernel/sav/library/hits/../ea/AbstractBinaryOperator.h \
 kernel/sav/library/hits/../ea/Individual.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/system/iostream.h \
 kernel/system/stl/iostream \
 kernel/sav/library/hits/../ea/InversionOperator.h \
 kernel/sav/library/hits/../ea/AbstractUnaryOperator.h \
 kernel/sav/library/hits/../ea/MutateOperator.h \
 kernel/sav/library/hits/../ea/CloneOperator.h \
 kernel/sav/library/hits/../ea/BinaryOperation.h \
 kernel/sav/library/hits/../ea/AbstractOperation.h \
 kernel/sav/library/hits/../ea/Population.h \
 kernel/sav/library/hits/../ea/UnaryOperation.h \
 kernel/sav/library/hits/../ea/EliteOperation.h \
 kernel/sav/library/hits/../ea/Stats.h \
 kernel/sav/library/hits/../ea/ParentsSelector.h \
 kernel/sav/library/hits/../ea/AbstractSampler.h \
 kernel/sav/library/hits/../ea/Rand.h kernel/utilities/rng.h \
 kernel/system/math.h kernel/system/stl/cmath \
 kernel/sav/library/hits/../ea/Reaper.h \
 kernel/sav/library/hits/../ea/Evaluator.h \
 kernel/sav/library/hits/../ea/AbstractFitness.h \
 kernel/sav/library/hits/../ea/AbstractEvaluator.h \
 kernel/sav/library/hits/../ea/Stats.h \
 kernel/sav/library/hits/../ea/RecombineReplaceSample.h \
 kernel/sav/library/hits/../ea/RecombineReplaceParents.h \
 kernel/sav/library/hits/../ea/RecombineAdd.h \
 kernel/sav/library/hits/../ea/Boltzscaler.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h \
 kernel/sav/library/hits/HitsEngine.h \
 kernel/sav/library/hits/../ea/Engine.h \
 kernel/sav/library/hits/../ea/AbstractInitializer.h \
 kernel/sav/library/hits/../ea/AbstractStoppingCriteria.h \
 kernel/sav/library/hits/../ea/Keeper.h kernel/basic/message.h \
 kernel/sav/library/hits/HitsFitness.h \
 kernel/sav/library/hits/../ea/AbstractFitness.h \
 kernel/sav/library/hits/../ea/macros.h \
 kernel/sav/library/hits/HistStop.h \
 kernel/sav/library/hits/../ea/AbstractStoppingCriteria.h \
 kernel/sav/library/hits/HitsInit.h \
 kernel/sav/library/hits/../ea/AbstractInitializer.h \
 kernel/sav/library/hits/HitsMessage.h kernel/system/assert.h \
 kernel/system/stl/cassert kernel/sav/library/hits/HitsKeeper.h
HitsKeeper.o: kernel/sav/library/hits/HitsKeeper.cpp stdafx.h \
 kernel/sav/library/hits/HitsKeeper.h \
 kernel/sav/library/hits/../ea/Keeper.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/sav/library/hits/../ea/Individual.h \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/message.h kernel/basic/string.h kernel/basic/types.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h kernel/basic/bits.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat
HitsFitness.o: kernel/sav/library/hits/HitsFitness.cpp stdafx.h \
 kernel/sav/library/hits/HitsFitness.h \
 kernel/sav/library/hits/../ea/AbstractFitness.h \
 kernel/sav/library/hits/../ea/Individual.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/sav/library/hits/../ea/EABitset.h \
 kernel/basic/bits.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/hits/../ea/macros.h \
 kernel/sav/library/hits/Context.h kernel/system/assert.h \
 kernel/system/stl/cassert
Context.o: kernel/sav/library/hits/Context.cpp stdafx.h \
 kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h \
 kernel/sav/library/hits/../ea/Individual.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/bits.h kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/basic/message.h kernel/system/assert.h \
 kernel/system/stl/cassert kernel/sav/library/hits/HitsMessage.h
HitsMessage.o: kernel/sav/library/hits/HitsMessage.cpp stdafx.h \
 kernel/sav/library/hits/HitsMessage.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/string.h copyright.h \
 kernel/basic/types.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib kernel/basic/message.h \
 kernel/basic/map.h kernel/basic/macros.h kernel/system/stl/map
HitsEngine.o: kernel/sav/library/hits/HitsEngine.cpp stdafx.h \
 kernel/sav/library/hits/HitsEngine.h \
 kernel/sav/library/hits/../ea/Engine.h \
 kernel/sav/library/hits/../ea/macros.h kernel/basic/vector.h copyright.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/sav/library/hits/../ea/Population.h \
 kernel/sav/library/hits/../ea/Individual.h kernel/system/iostream.h \
 kernel/system/stl/iostream \
 kernel/sav/library/hits/../ea/AbstractInitializer.h \
 kernel/sav/library/hits/../ea/AbstractStoppingCriteria.h \
 kernel/sav/library/hits/../ea/AbstractEvaluator.h \
 kernel/sav/library/hits/../ea/AbstractOperation.h \
 kernel/sav/library/hits/Context.h \
 kernel/sav/library/hits/../ea/EABitset.h kernel/basic/bits.h \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/limits.h \
 kernel/system/stl/limits kernel/sav/library/hits/../ea/Keeper.h \
 kernel/basic/message.h kernel/sav/library/hits/../ea/Evaluator.h \
 kernel/sav/library/hits/../ea/AbstractFitness.h \
 kernel/sav/library/hits/../ea/Stats.h \
 kernel/sav/library/hits/HitsMessage.h
rfeatureselector.o: kernel/genomics/algorithms/rfeatureselector.cpp \
 stdafx.h copyright.h kernel/genomics/algorithms/rfeatureselector.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/genomics/algorithms/loadfeatureselection.h \
 kernel/algorithms/keyword.h kernel/algorithms/decisiontableexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/mydecisiontableimporter.h \
 kernel/algorithms/decisiontableimporter.h kernel/algorithms/importer.h \
 kernel/structures/decisiontables.h kernel/structures/structures.h \
 kernel/structures/dictionary.h kernel/structures/stringattribute.h \
 kernel/structures/integerattribute.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/iokit.h \
 kernel/utilities/rng.h kernel/utilities/mathkit.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/utilities/creator.h
pca.o: kernel/genomics/algorithms/pca.cpp stdafx.h copyright.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/basic/macros.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/genomics/algorithms/pca.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/referent.h kernel/system/stddef.h \
 kernel/system/stl/cstddef kernel/basic/handle.h kernel/basic/message.h \
 kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/genomics/utilities/statkit.h kernel/genomics/basic/matrix.h \
 kernel/utilities/creator.h kernel/structures/dictionary.h \
 kernel/structures/stringattribute.h kernel/structures/integerattribute.h \
 kernel/structures/floatattribute.h kernel/genomics/utilities/nrutil.h \
 kernel/algorithms/keyword.h kernel/structures/decisiontables.h \
 kernel/structures/structures.h
loocvserialexecutor.o: kernel/genomics/algorithms/loocvserialexecutor.cpp \
 stdafx.h copyright.h kernel/genomics/algorithms/loocvserialexecutor.h \
 kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/utilities/rng.h \
 kernel/algorithms/keyword.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/classifier.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/project.h kernel/structures/projectmanager.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/creator.h \
 common/objectmanager.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h
ftest.o: kernel/genomics/algorithms/ftest.cpp stdafx.h copyright.h \
 kernel/genomics/algorithms/ftest.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/genomics/utilities/statkit.h \
 kernel/system/math.h kernel/system/stl/cmath \
 kernel/genomics/basic/matrix.h kernel/utilities/creator.h \
 kernel/structures/dictionary.h kernel/structures/stringattribute.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/structures/decisiontables.h kernel/structures/structures.h \
 kernel/utilities/rng.h kernel/utilities/mathkit.h \
 kernel/basic/algorithm.h kernel/system/time.h kernel/system/stl/ctime
permutationtest.o: kernel/genomics/algorithms/permutationtest.cpp \
 stdafx.h copyright.h kernel/genomics/algorithms/permutationtest.h \
 kernel/algorithms/splitter.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/genomics/algorithms/permloocvserialexecutor.h \
 kernel/genomics/algorithms/loocvserialexecutor.h \
 kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/utilities/rng.h \
 kernel/algorithms/keyword.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/classifier.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/project.h kernel/structures/projectmanager.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/creator.h \
 common/objectmanager.h kernel/genomics/algorithms/permcvserialexecutor.h \
 kernel/structures/decisiontables.h kernel/structures/dictionary.h \
 kernel/structures/textfile.h
ttest.o: kernel/genomics/algorithms/ttest.cpp stdafx.h copyright.h \
 kernel/genomics/algorithms/ttest.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/genomics/utilities/statkit.h \
 kernel/system/math.h kernel/system/stl/cmath \
 kernel/genomics/basic/matrix.h kernel/utilities/creator.h \
 kernel/structures/dictionary.h kernel/structures/stringattribute.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/structures/decisiontables.h kernel/structures/structures.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/iokit.h kernel/utilities/rng.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h
featureselector.o: kernel/genomics/algorithms/featureselector.cpp \
 stdafx.h copyright.h kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/utilities/creator.h \
 kernel/structures/dictionary.h kernel/structures/stringattribute.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h
functionalclassifier.o: \
 kernel/genomics/algorithms/functionalclassifier.cpp stdafx.h copyright.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/genomics/algorithms/functionalclassifier.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/pair.h kernel/system/stl/utility kernel/utilities/rng.h \
 kernel/structures/classification.h kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/rule.h \
 kernel/structures/reduct.h kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/algorithms/reducer.h \
 kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/basic/bits.h \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/structures/booleansopfunction.h \
 kernel/structures/reducts.h kernel/genomics/algorithms/cvclassifier.h \
 kernel/structures/project.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/textfile.h \
 kernel/structures/stringattribute.h kernel/structures/dictionary.h \
 kernel/structures/informationvector.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h \
 kernel/structures/projectmanager.h kernel/algorithms/keyword.h \
 kernel/algorithms/batchclassifier.h kernel/algorithms/classifier.h \
 kernel/sav/algorithms/savgeneticreducer.h \
 kernel/algorithms/costinformation.h common/objectmanager.h \
 kernel/basic/algorithm.h kernel/utilities/creator.h \
 kernel/utilities/discerner.h kernel/structures/graph.h
loadfeatureselection.o: \
 kernel/genomics/algorithms/loadfeatureselection.cpp stdafx.h copyright.h \
 kernel/genomics/algorithms/loadfeatureselection.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/decisiontables.h \
 kernel/structures/structures.h kernel/structures/dictionary.h \
 kernel/structures/stringattribute.h kernel/structures/integerattribute.h \
 kernel/structures/floatattribute.h kernel/utilities/creator.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream
mcfeatureselector.o: kernel/genomics/algorithms/mcfeatureselector.cpp \
 stdafx.h copyright.h kernel/genomics/algorithms/mcfeatureselector.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/genomics/algorithms/manualfeatureselector.h \
 kernel/algorithms/keyword.h kernel/algorithms/johnsonreducer.h \
 kernel/algorithms/reducer.h kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/basic/bits.h \
 kernel/basic/set.h kernel/system/stl/set kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/structures/booleansopfunction.h \
 kernel/structures/reduct.h kernel/structures/reducts.h \
 kernel/structures/structures.h kernel/algorithms/rulegenerator.h \
 kernel/structures/decisiontables.h kernel/structures/dictionary.h \
 kernel/structures/rule.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/structures/rules.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/structures/stringattribute.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/utilities/creator.h common/objectmanager.h \
 kernel/utilities/threadcontrol.h
templatescaler.o: kernel/genomics/algorithms/templatescaler.cpp stdafx.h \
 copyright.h kernel/genomics/algorithms/templatescaler.h \
 kernel/algorithms/scaler.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/algorithms/keyword.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/algorithm.h kernel/utilities/creator.h \
 kernel/structures/stringattribute.h kernel/structures/floatattribute.h \
 kernel/structures/dictionary.h common/objectmanager.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/system/math.h \
 kernel/system/stl/cmath
cvclassifier.o: kernel/genomics/algorithms/cvclassifier.cpp stdafx.h \
 copyright.h kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/system/math.h kernel/system/stl/cmath \
 kernel/genomics/algorithms/cvclassifier.h kernel/algorithms/algorithm.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/algorithms/reducer.h kernel/algorithms/objectselector.h \
 kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/booleansopfunction.h kernel/structures/reduct.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/utilities/rng.h \
 kernel/structures/classification.h kernel/structures/rules.h \
 kernel/structures/rule.h kernel/basic/interval.h \
 kernel/utilities/iokit.h kernel/genomics/algorithms/templatescaler.h \
 kernel/algorithms/scaler.h kernel/structures/project.h \
 kernel/structures/projectmanager.h \
 kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/textfile.h \
 kernel/structures/informationvector.h \
 kernel/structures/stringattribute.h kernel/structures/floatattribute.h \
 kernel/structures/dictionary.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h kernel/algorithms/keyword.h \
 kernel/algorithms/batchclassifier.h kernel/algorithms/classifier.h \
 kernel/sav/algorithms/savgeneticreducer.h \
 kernel/algorithms/costinformation.h kernel/basic/algorithm.h \
 kernel/utilities/creator.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/utilities/mathkit.h \
 common/objectmanager.h
permcvserialexecutor.o: \
 kernel/genomics/algorithms/permcvserialexecutor.cpp stdafx.h copyright.h \
 kernel/genomics/algorithms/permcvserialexecutor.h \
 kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/utilities/rng.h \
 kernel/algorithms/keyword.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/classifier.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/project.h kernel/structures/projectmanager.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/creator.h \
 common/objectmanager.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h
permloocvserialexecutor.o: \
 kernel/genomics/algorithms/permloocvserialexecutor.cpp stdafx.h \
 copyright.h kernel/genomics/algorithms/permloocvserialexecutor.h \
 kernel/genomics/algorithms/loocvserialexecutor.h \
 kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream kernel/utilities/rng.h \
 kernel/algorithms/keyword.h kernel/algorithms/batchclassifier.h \
 kernel/algorithms/classifier.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/project.h kernel/structures/projectmanager.h \
 kernel/utilities/mathkit.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/creator.h \
 common/objectmanager.h kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h
manualfeatureselector.o: \
 kernel/genomics/algorithms/manualfeatureselector.cpp stdafx.h \
 copyright.h kernel/genomics/algorithms/manualfeatureselector.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/algorithms/algorithm.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/algorithms/keyword.h kernel/structures/decisiontables.h \
 kernel/structures/structures.h kernel/structures/dictionary.h \
 kernel/structures/stringattribute.h kernel/structures/integerattribute.h \
 kernel/structures/floatattribute.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/iokit.h \
 kernel/utilities/creator.h
statkit.o: kernel/genomics/utilities/statkit.cpp stdafx.h copyright.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/basic/macros.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/genomics/utilities/statkit.h \
 kernel/basic/message.h kernel/genomics/basic/matrix.h \
 kernel/utilities/creator.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/dictionary.h kernel/structures/stringattribute.h \
 kernel/structures/integerattribute.h kernel/structures/floatattribute.h \
 kernel/genomics/utilities/nrutil.h
calibrationcurve.o: kernel/structures/calibrationcurve.cpp stdafx.h \
 copyright.h kernel/structures/calibrationcurve.h \
 kernel/structures/binaryoutcomecurve.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/basic/pair.h kernel/system/stl/utility \
 kernel/structures/classification.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/systemkit.h \
 kernel/utilities/permuter.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream common/configuration.h
indiscernibilitygraph.o: kernel/structures/indiscernibilitygraph.cpp \
 stdafx.h copyright.h kernel/structures/indiscernibilitygraph.h \
 kernel/structures/graph.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/basic/bits.h kernel/structures/generalizeddecision.h \
 kernel/structures/discernibilitymatrix.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/utilities/partitionkit.h kernel/utilities/discerner.h \
 kernel/utilities/creator.h
rulebasedclassification.o: kernel/structures/rulebasedclassification.cpp \
 stdafx.h copyright.h kernel/structures/rulebasedclassification.h \
 kernel/structures/classification.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h
ksdecisiontable.o: kernel/structures/ksdecisiontable.cpp stdafx.h \
 copyright.h kernel/structures/ksdecisiontable.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h
discernibilitymatrix.o: kernel/structures/discernibilitymatrix.cpp \
 stdafx.h copyright.h kernel/structures/discernibilitymatrix.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/generalizeddecision.h \
 kernel/basic/bits.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/utilities/partitionkit.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath
rule.o: kernel/structures/rule.cpp stdafx.h copyright.h \
 kernel/structures/rule.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/basic/bits.h
stringattribute.o: kernel/structures/stringattribute.cpp stdafx.h \
 copyright.h kernel/structures/stringattribute.h \
 kernel/structures/attribute.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/basic/pair.h kernel/system/stl/utility
decisiontable.o: kernel/structures/decisiontable.cpp stdafx.h copyright.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/structures.h kernel/structures/dictionary.h \
 kernel/structures/floatattribute.h kernel/structures/integerattribute.h \
 kernel/structures/indiscernibilitygraph.h kernel/structures/graph.h \
 kernel/basic/bits.h kernel/structures/rules.h kernel/structures/rule.h \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/discerner.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/utilities/creator.h
history.o: kernel/structures/history.cpp stdafx.h copyright.h \
 kernel/structures/history.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/delimiter.h
generalrules.o: kernel/structures/generalrules.cpp stdafx.h copyright.h \
 kernel/structures/generalrules.h kernel/structures/generalrule.h \
 kernel/structures/rule.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/structures/generalizeddecision.h \
 kernel/basic/set.h kernel/system/stl/set kernel/structures/rules.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/structures/informationvector.h kernel/structures/reducts.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/basic/bits.h kernel/basic/algorithm.h
structure.o: kernel/structures/structure.cpp stdafx.h copyright.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/projectmanager.h \
 kernel/structures/project.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/algorithms/algorithm.h
floatattribute.o: kernel/structures/floatattribute.cpp stdafx.h \
 copyright.h kernel/structures/floatattribute.h \
 kernel/structures/attribute.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
discernibilityfunction.o: kernel/structures/discernibilityfunction.cpp \
 stdafx.h copyright.h kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set \
 kernel/structures/generalizeddecision.h \
 kernel/structures/discernibilitymatrix.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/basic/algorithm.h
generalizeddecision.o: kernel/structures/generalizeddecision.cpp stdafx.h \
 copyright.h kernel/structures/generalizeddecision.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath
generalrule.o: kernel/structures/generalrule.cpp stdafx.h copyright.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/basic/bits.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h \
 kernel/structures/informationvector.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/structures/generalrule.h kernel/structures/rule.h \
 kernel/structures/reduct.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/basic/set.h kernel/system/stl/set
booleanfunction.o: kernel/structures/booleanfunction.cpp stdafx.h \
 copyright.h kernel/structures/booleanfunction.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/basic/bits.h kernel/basic/set.h \
 kernel/system/stl/set kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/basic/algorithm.h common/configuration.h
structures.o: kernel/structures/structures.cpp stdafx.h copyright.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
integerattribute.o: kernel/structures/integerattribute.cpp stdafx.h \
 copyright.h kernel/structures/integerattribute.h \
 kernel/structures/attribute.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath
rules.o: kernel/structures/rules.cpp stdafx.h copyright.h \
 kernel/structures/rules.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/structures/informationvector.h kernel/structures/reducts.h \
 kernel/algorithms/ruleevaluator.h kernel/utilities/partitionkit.h \
 kernel/utilities/creator.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/basic/bits.h kernel/basic/algorithm.h
equivalenceclass.o: kernel/structures/equivalenceclass.cpp stdafx.h \
 copyright.h kernel/structures/equivalenceclass.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/basic/algorithm.h
equivalenceclasses.o: kernel/structures/equivalenceclasses.cpp stdafx.h \
 copyright.h kernel/structures/equivalenceclasses.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/equivalenceclass.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/basic/algorithm.h
ksreduct.o: kernel/structures/ksreduct.cpp stdafx.h copyright.h \
 kernel/structures/ksreduct.h kernel/structures/reduct.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector
reducts.o: kernel/structures/reducts.cpp stdafx.h copyright.h \
 kernel/structures/reducts.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/reduct.h kernel/structures/rules.h \
 kernel/structures/rule.h kernel/basic/pair.h kernel/system/stl/utility \
 kernel/basic/interval.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/basic/algorithm.h \
 kernel/utilities/creator.h kernel/utilities/discerner.h \
 kernel/structures/graph.h kernel/basic/bits.h
roccurve.o: kernel/structures/roccurve.cpp stdafx.h copyright.h \
 kernel/structures/roccurve.h kernel/structures/binaryoutcomecurve.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/classification.h \
 kernel/basic/pair.h kernel/system/stl/utility kernel/basic/algorithm.h \
 kernel/utilities/systemkit.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/cindexcomputer.h kernel/system/math.h \
 kernel/system/stl/cmath common/configuration.h
projectmanager.o: kernel/structures/projectmanager.cpp stdafx.h \
 copyright.h kernel/structures/projectmanager.h \
 kernel/structures/project.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h
textfile.o: kernel/structures/textfile.cpp stdafx.h copyright.h \
 kernel/structures/textfile.h kernel/structures/annotatedstructure.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
approximation.o: kernel/structures/approximation.cpp stdafx.h copyright.h \
 kernel/structures/approximation.h kernel/structures/annotatedstructure.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/equivalenceclasses.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/equivalenceclass.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
informationvector.o: kernel/structures/informationvector.cpp stdafx.h \
 copyright.h kernel/structures/informationvector.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h
dictionary.o: kernel/structures/dictionary.cpp stdafx.h copyright.h \
 kernel/structures/dictionary.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/attribute.h kernel/structures/integerattribute.h \
 kernel/structures/floatattribute.h kernel/structures/stringattribute.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
annotatedstructure.o: kernel/structures/annotatedstructure.cpp stdafx.h \
 copyright.h kernel/structures/annotatedstructure.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/creator.h
annotation.o: kernel/structures/annotation.cpp stdafx.h copyright.h \
 kernel/structures/annotation.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/delimiter.h
binaryoutcomecurve.o: kernel/structures/binaryoutcomecurve.cpp stdafx.h \
 copyright.h kernel/structures/binaryoutcomecurve.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/utilities/permuter.h
confusionmatrix.o: kernel/structures/confusionmatrix.cpp stdafx.h \
 copyright.h kernel/structures/confusionmatrix.h \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/creator.h
reduct.o: kernel/structures/reduct.cpp stdafx.h copyright.h \
 kernel/structures/reduct.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/decisiontable.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream
decisiontables.o: kernel/structures/decisiontables.cpp stdafx.h \
 copyright.h kernel/structures/decisiontables.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h
graph.o: kernel/structures/graph.cpp stdafx.h copyright.h \
 kernel/structures/graph.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/basic/bits.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath
batchclassification.o: kernel/structures/batchclassification.cpp stdafx.h \
 copyright.h kernel/structures/batchclassification.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/confusionmatrix.h kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h \
 kernel/structures/classification.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/creator.h
booleansopfunction.o: kernel/structures/booleansopfunction.cpp stdafx.h \
 copyright.h kernel/structures/booleansopfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set
parentstructure.o: kernel/structures/parentstructure.cpp stdafx.h \
 copyright.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/structures.h kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/classification.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/utilities/creator.h common/objectmanager.h \
 kernel/algorithms/algorithm.h
project.o: kernel/structures/project.cpp stdafx.h copyright.h \
 kernel/structures/project.h kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/structures/annotation.h kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/projectmanager.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream common/objectmanager.h \
 kernel/algorithms/algorithm.h
classification.o: kernel/structures/classification.cpp stdafx.h \
 copyright.h kernel/structures/classification.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/basic/algorithm.h
attribute.o: kernel/structures/attribute.cpp stdafx.h copyright.h \
 kernel/structures/attribute.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/basic/interval.h
historyentry.o: kernel/structures/historyentry.cpp stdafx.h copyright.h \
 kernel/structures/historyentry.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/utilities/systemkit.h kernel/system/time.h \
 kernel/system/stl/ctime kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/iokit.h
ksinformationvector.o: kernel/structures/ksinformationvector.cpp stdafx.h \
 copyright.h kernel/structures/ksinformationvector.h \
 kernel/structures/informationvector.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector
booleanposfunction.o: kernel/structures/booleanposfunction.cpp stdafx.h \
 copyright.h kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/macros.h kernel/basic/referent.h kernel/basic/types.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/basic/vector.h kernel/system/stl/vector \
 kernel/basic/bits.h kernel/basic/set.h kernel/system/stl/set \
 kernel/basic/hashmap.h kernel/system/stl/ext/hash_map \
 kernel/basic/hashset.h kernel/system/stl/ext/hash_set
ksrule.o: kernel/structures/ksrule.cpp stdafx.h copyright.h \
 kernel/structures/ksrule.h kernel/structures/rule.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h \
 kernel/structures/generalizeddecision.h
mcnemarcomparator.o: kernel/utilities/mcnemarcomparator.cpp stdafx.h \
 copyright.h kernel/utilities/mcnemarcomparator.h \
 kernel/utilities/binaryoutcomecomparator.h kernel/basic/types.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/basic/macros.h kernel/system/stl/vector \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/basic/message.h
discerner.o: kernel/utilities/discerner.cpp stdafx.h copyright.h \
 kernel/utilities/discerner.h kernel/structures/graph.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/basic/types.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/basic/bits.h kernel/utilities/creator.h \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h
partitionkit.o: kernel/utilities/partitionkit.cpp stdafx.h copyright.h \
 kernel/utilities/partitionkit.h kernel/basic/vector.h \
 kernel/basic/macros.h kernel/system/stl/vector \
 kernel/utilities/creator.h kernel/basic/types.h kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/structures/decisiontable.h \
 kernel/structures/parentstructure.h \
 kernel/structures/annotatedstructure.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h kernel/structures/reduct.h \
 kernel/basic/algorithm.h
xmlkit.o: kernel/utilities/xmlkit.cpp stdafx.h copyright.h \
 kernel/utilities/xmlkit.h kernel/basic/string.h kernel/basic/types.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/macros.h
permuter.o: kernel/utilities/permuter.cpp stdafx.h copyright.h \
 kernel/utilities/permuter.h kernel/basic/vector.h kernel/basic/macros.h \
 kernel/system/stl/vector kernel/basic/algorithm.h
rngshort.o: kernel/utilities/rngshort.cpp kernel/utilities/rngshort.h \
 kernel/utilities/rngxxx.h
iokit.o: kernel/utilities/iokit.cpp stdafx.h copyright.h \
 kernel/utilities/iokit.h kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/basic/types.h kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/delimiter.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio
rngxxx.o: kernel/utilities/rngxxx.cpp kernel/utilities/rngxxx.h
threadcontrol.o: kernel/utilities/threadcontrol.cpp \
 kernel/utilities/threadcontrol.h
systemkit.o: kernel/utilities/systemkit.cpp stdafx.h copyright.h \
 kernel/utilities/systemkit.h kernel/basic/string.h kernel/basic/types.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/message.h \
 kernel/basic/map.h kernel/basic/macros.h kernel/system/stl/map \
 kernel/system/time.h kernel/system/stl/ctime kernel/system/direct.h \
 kernel/system/sys/stat.h
rnglong.o: kernel/utilities/rnglong.cpp kernel/utilities/rnglong.h \
 kernel/utilities/rngxxx.h
mathkit.o: kernel/utilities/mathkit.cpp stdafx.h copyright.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/basic/macros.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
binaryoutcomecomparator.o: kernel/utilities/binaryoutcomecomparator.cpp \
 stdafx.h copyright.h kernel/utilities/binaryoutcomecomparator.h \
 kernel/basic/types.h kernel/basic/undefined.h kernel/system/limits.h \
 kernel/system/stl/limits kernel/system/float.h kernel/system/stl/cfloat \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/basic/macros.h kernel/system/stl/vector \
 kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/mathkit.h \
 kernel/utilities/rng.h kernel/basic/map.h kernel/system/stl/map \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/utilities/permuter.h kernel/structures/binaryoutcomecurve.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/referent.h kernel/system/stddef.h \
 kernel/system/stl/cstddef kernel/basic/handle.h kernel/basic/message.h \
 kernel/basic/ids.h kernel/basic/idholder.h
rng.o: kernel/utilities/rng.cpp stdafx.h copyright.h \
 kernel/utilities/rng.h kernel/utilities/mathkit.h kernel/basic/macros.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/types.h \
 kernel/basic/string.h kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/system/stl/vector kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath
cindexcomputer.o: kernel/utilities/cindexcomputer.cpp stdafx.h \
 copyright.h kernel/utilities/cindexcomputer.h kernel/basic/vector.h \
 kernel/basic/macros.h kernel/system/stl/vector kernel/system/math.h \
 kernel/system/stl/cmath kernel/basic/undefined.h kernel/system/limits.h \
 kernel/system/stl/limits kernel/system/float.h kernel/system/stl/cfloat \
 kernel/basic/types.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/basic/algorithm.h
hanleymcneilcomparator.o: kernel/utilities/hanleymcneilcomparator.cpp \
 stdafx.h copyright.h kernel/utilities/hanleymcneilcomparator.h \
 kernel/utilities/binaryoutcomecomparator.h kernel/basic/types.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/basic/macros.h kernel/system/stl/vector \
 kernel/utilities/mathkit.h kernel/utilities/rng.h kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/algorithm.h kernel/system/math.h \
 kernel/system/stl/cmath kernel/structures/roccurve.h \
 kernel/structures/binaryoutcomecurve.h kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/handle.h kernel/basic/message.h kernel/basic/ids.h \
 kernel/basic/idholder.h kernel/structures/classification.h \
 kernel/basic/pair.h kernel/system/stl/utility
generator.o: kernel/utilities/generator.cpp kernel/utilities/generator.h
creator.o: kernel/utilities/creator.cpp stdafx.h copyright.h \
 kernel/utilities/creator.h kernel/basic/types.h kernel/system/stdio.h \
 kernel/system/stl/cstdio kernel/utilities/iokit.h kernel/system/stdlib.h \
 kernel/system/stl/cstdlib kernel/system/iostream.h \
 kernel/system/stl/iostream kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/structures/annotation.h \
 kernel/structures/structure.h kernel/basic/persistent.h \
 kernel/basic/identifier.h kernel/basic/macros.h kernel/basic/referent.h \
 kernel/system/stddef.h kernel/system/stl/cstddef kernel/basic/handle.h \
 kernel/basic/message.h kernel/basic/string.h kernel/system/string.h \
 kernel/system/stl/cstring kernel/system/stl/string kernel/basic/map.h \
 kernel/system/stl/map kernel/basic/ids.h kernel/basic/idholder.h \
 kernel/basic/undefined.h kernel/system/limits.h kernel/system/stl/limits \
 kernel/system/float.h kernel/system/stl/cfloat kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/history.h \
 kernel/structures/historyentry.h kernel/utilities/systemkit.h \
 kernel/structures/approximation.h kernel/structures/annotatedstructure.h \
 kernel/structures/equivalenceclasses.h kernel/structures/structures.h \
 kernel/structures/parentstructure.h kernel/structures/equivalenceclass.h \
 kernel/structures/decisiontable.h kernel/structures/attribute.h \
 kernel/structures/generalizeddecision.h \
 kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/decisiontables.h kernel/structures/dictionary.h \
 kernel/structures/floatattribute.h kernel/structures/generalrule.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h kernel/basic/set.h \
 kernel/system/stl/set kernel/structures/generalrules.h \
 kernel/structures/rules.h kernel/structures/graph.h kernel/basic/bits.h \
 kernel/structures/informationvector.h \
 kernel/structures/integerattribute.h kernel/structures/project.h \
 kernel/structures/reducts.h kernel/structures/rulebasedclassification.h \
 kernel/structures/stringattribute.h common/objectmanager.h \
 kernel/algorithms/algorithm.h
rnggen.o: kernel/utilities/rnggen.cpp kernel/utilities/rnggen.h \
 kernel/utilities/rngxxx.h kernel/utilities/rngshort.h \
 kernel/utilities/rnglong.h
configuration.o: common/configuration.cpp stdafx.h copyright.h \
 common/configuration.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/message.h kernel/basic/map.h kernel/basic/macros.h \
 kernel/system/stl/map kernel/utilities/iokit.h kernel/system/fstream.h \
 kernel/system/stl/fstream kernel/utilities/systemkit.h
messagehelper.o: common/messagehelper.cpp stdafx.h copyright.h \
 common/messagehelper.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/vector.h kernel/basic/macros.h kernel/system/stl/vector
installer.o: common/installer.cpp stdafx.h copyright.h common/installer.h \
 common/objectmanager.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/handle.h kernel/basic/macros.h kernel/basic/message.h \
 kernel/basic/map.h kernel/system/stl/map kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/algorithms/algorithm.h \
 common/modemanager.h kernel/structures/approximation.h \
 kernel/structures/annotatedstructure.h kernel/structures/annotation.h \
 kernel/structures/history.h kernel/structures/historyentry.h \
 kernel/utilities/systemkit.h kernel/structures/equivalenceclasses.h \
 kernel/structures/structures.h kernel/structures/parentstructure.h \
 kernel/structures/equivalenceclass.h kernel/structures/decisiontable.h \
 kernel/structures/attribute.h kernel/structures/generalizeddecision.h \
 kernel/structures/batchclassification.h \
 kernel/structures/confusionmatrix.h kernel/structures/classification.h \
 kernel/structures/decisiontables.h kernel/structures/dictionary.h \
 kernel/structures/floatattribute.h kernel/structures/generalrule.h \
 kernel/structures/rule.h kernel/structures/reduct.h kernel/basic/pair.h \
 kernel/system/stl/utility kernel/basic/interval.h kernel/basic/set.h \
 kernel/system/stl/set kernel/structures/generalrules.h \
 kernel/structures/rules.h kernel/utilities/iokit.h \
 kernel/system/fstream.h kernel/system/stl/fstream \
 kernel/structures/graph.h kernel/basic/bits.h \
 kernel/structures/integerattribute.h kernel/structures/project.h \
 kernel/structures/reducts.h kernel/structures/rulebasedclassification.h \
 kernel/structures/stringattribute.h kernel/structures/textfile.h \
 kernel/structures/ksdecisiontable.h \
 kernel/structures/ksinformationvector.h \
 kernel/structures/informationvector.h kernel/structures/ksreduct.h \
 kernel/structures/ksrule.h kernel/algorithms/approximator.h \
 kernel/utilities/mathkit.h kernel/utilities/rng.h \
 kernel/basic/algorithm.h kernel/system/math.h kernel/system/stl/cmath \
 kernel/algorithms/batchclassifier.h kernel/algorithms/classifier.h \
 kernel/algorithms/binarysplitter.h kernel/algorithms/splitter.h \
 kernel/algorithms/brorthogonalscaler.h \
 kernel/algorithms/orthogonalscaler.h kernel/algorithms/scaler.h \
 kernel/algorithms/combinatorialcompleter.h kernel/algorithms/completer.h \
 kernel/algorithms/conditionedcombinatorialcompleter.h \
 kernel/algorithms/conditionedcompleter.h \
 kernel/algorithms/conditionedmeancompleter.h \
 kernel/algorithms/cppgeneralruleexporter.h \
 kernel/algorithms/cppruleexporter.h kernel/algorithms/ruleexporter.h \
 kernel/algorithms/exporter.h kernel/algorithms/cvserialexecutor.h \
 kernel/algorithms/serialexecutorloop.h \
 kernel/algorithms/serialexecutor.h kernel/algorithms/executor.h \
 kernel/algorithms/dictionaryexporter.h \
 kernel/algorithms/dictionaryimporter.h kernel/algorithms/importer.h \
 kernel/algorithms/discernibilityfunctionexporter.h \
 kernel/algorithms/discernibilityexporter.h \
 kernel/algorithms/decisiontableexporter.h \
 kernel/algorithms/objectselector.h kernel/algorithms/dynamicreducer.h \
 kernel/algorithms/reducer.h kernel/structures/discernibilityfunction.h \
 kernel/structures/booleanposfunction.h \
 kernel/structures/booleanfunction.h kernel/basic/hashmap.h \
 kernel/system/stl/ext/hash_map kernel/basic/hashset.h \
 kernel/system/stl/ext/hash_set kernel/structures/booleansopfunction.h \
 kernel/algorithms/entropyscaler.h \
 kernel/algorithms/equalfrequencyscaler.h kernel/algorithms/reyreducer.h \
 kernel/algorithms/holte1rreducer.h kernel/algorithms/htmlreporter.h \
 kernel/algorithms/reporter.h \
 kernel/algorithms/indiscernibilitygraphexporter.h \
 kernel/algorithms/johnsonreducer.h kernel/algorithms/kidnapper.h \
 kernel/algorithms/scriptalgorithm.h kernel/algorithms/loader.h \
 kernel/algorithms/manualreducer.h kernel/algorithms/manualscaler.h \
 kernel/algorithms/matlabdecisiontableexporter.h \
 kernel/algorithms/meancompleter.h \
 kernel/algorithms/mydecisiontableexporter.h \
 kernel/algorithms/mydecisiontableimporter.h \
 kernel/algorithms/decisiontableimporter.h \
 kernel/algorithms/arffdecisiontableimporter.h \
 kernel/algorithms/adxdecisiontableimporter.h \
 kernel/algorithms/arffdecisiontableexporter.h \
 kernel/algorithms/adxdecisiontableexporter.h \
 kernel/algorithms/mygeneralrulefilter.h kernel/algorithms/myrulefilter.h \
 kernel/algorithms/rulefilter.h kernel/algorithms/filter.h \
 kernel/algorithms/myposdecisiontableimporter.h \
 kernel/algorithms/myreductexporter.h kernel/algorithms/reductexporter.h \
 kernel/algorithms/myreductfilter.h kernel/algorithms/reductfilter.h \
 kernel/algorithms/myreductimporter.h kernel/algorithms/reductimporter.h \
 kernel/algorithms/myreductshortener.h \
 kernel/algorithms/reductshortener.h kernel/algorithms/myruleexporter.h \
 kernel/algorithms/naivebayesclassifier.h kernel/algorithms/naivescaler.h \
 kernel/algorithms/objecttrackingvoter.h kernel/algorithms/voter.h \
 kernel/algorithms/rulebasedclassifier.h \
 kernel/algorithms/orthogonalfilescaler.h \
 kernel/algorithms/parallelexecutor.h kernel/algorithms/partitioner.h \
 kernel/algorithms/prologdecisiontableexporter.h \
 kernel/algorithms/prologgeneralruleexporter.h \
 kernel/algorithms/generalruleexporter.h \
 kernel/algorithms/prologreductexporter.h \
 kernel/algorithms/prologruleexporter.h \
 kernel/algorithms/qualityrulefilter.h kernel/algorithms/ruleevaluator.h \
 kernel/algorithms/qualityrulefilterloop.h \
 kernel/algorithms/reductcostfilter.h kernel/algorithms/costinformation.h \
 kernel/algorithms/reductperformancefilter.h \
 kernel/algorithms/rulegenerator.h kernel/algorithms/removalcompleter.h \
 kernel/algorithms/ruleshortener.h kernel/algorithms/ruletuner.h \
 kernel/utilities/discerner.h kernel/algorithms/rulepairwisegeneralizer.h \
 kernel/algorithms/rulegeneralizer.h \
 kernel/algorithms/rulegroupgeneralizer.h kernel/algorithms/saver.h \
 kernel/algorithms/seminaivescaler.h \
 kernel/algorithms/simplecoverfilter.h kernel/algorithms/standardvoter.h \
 kernel/algorithms/structurecreator.h kernel/algorithms/valuesplitter.h \
 kernel/algorithms/xmldecisiontableexporter.h \
 kernel/algorithms/xmlgeneralruleexporter.h \
 kernel/algorithms/xmlreductexporter.h kernel/algorithms/xmlreporter.h \
 kernel/algorithms/xmlruleexporter.h \
 kernel/sav/algorithms/savgeneticreducer.h \
 kernel/genomics/algorithms/templatescaler.h \
 kernel/genomics/algorithms/cvclassifier.h \
 kernel/genomics/algorithms/functionalclassifier.h \
 kernel/genomics/algorithms/mcfeatureselector.h \
 kernel/genomics/algorithms/featureselector.h \
 kernel/genomics/algorithms/rfeatureselector.h \
 kernel/genomics/algorithms/ftest.h kernel/genomics/algorithms/ttest.h \
 kernel/genomics/algorithms/manualfeatureselector.h \
 kernel/genomics/algorithms/pca.h kernel/genomics/utilities/statkit.h \
 kernel/genomics/basic/matrix.h kernel/utilities/creator.h \
 kernel/genomics/utilities/nrutil.h \
 kernel/genomics/algorithms/loadfeatureselection.h \
 kernel/genomics/algorithms/loocvserialexecutor.h \
 kernel/algorithms/keyword.h kernel/structures/projectmanager.h \
 kernel/algorithms/ksrulegenerator.h
modemanager.o: common/modemanager.cpp stdafx.h copyright.h \
 common/modemanager.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/handle.h kernel/basic/macros.h kernel/basic/message.h \
 kernel/basic/map.h kernel/system/stl/map kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/algorithms/algorithm.h \
 common/objectmanager.h
objectmanager.o: common/objectmanager.cpp stdafx.h copyright.h \
 common/objectmanager.h kernel/basic/types.h kernel/basic/string.h \
 kernel/system/string.h kernel/system/stl/cstring \
 kernel/system/stl/string kernel/system/stdio.h kernel/system/stl/cstdio \
 kernel/system/stdlib.h kernel/system/stl/cstdlib \
 kernel/system/iostream.h kernel/system/stl/iostream \
 kernel/basic/handle.h kernel/basic/macros.h kernel/basic/message.h \
 kernel/basic/map.h kernel/system/stl/map kernel/basic/vector.h \
 kernel/system/stl/vector kernel/structures/structure.h \
 kernel/basic/persistent.h kernel/basic/identifier.h \
 kernel/basic/referent.h kernel/system/stddef.h kernel/system/stl/cstddef \
 kernel/basic/ids.h kernel/basic/idholder.h kernel/basic/undefined.h \
 kernel/system/limits.h kernel/system/stl/limits kernel/system/float.h \
 kernel/system/stl/cfloat kernel/algorithms/algorithm.h \
 common/modemanager.h
