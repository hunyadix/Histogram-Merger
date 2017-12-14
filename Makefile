include Makefile.arch

CXXFLAGS += -std=c++17 -fdiagnostics-color=always -Wall -Wextra

MAIN_S = ./src/main.$(SrcSuf)
MAIN_O = ./obj/main.$(ObjSuf)
MAIN_A = ./bin/Histogram_merger$(ExeSuf)
OBJS          += $(MAIN_O)
PROGRAMS      += $(MAIN_A)

HISTOGRAM_MERGER_S = ./src/Histogram_merger.$(SrcSuf)
HISTOGRAM_MERGER_O = ./obj/Histogram_merger.$(ObjSuf)

all:                  $(PROGRAMS)

$(MAIN_A): $(MAIN_O) $(HISTOGRAM_MERGER_O)
	@printf "Compiling done, linking...\n"
	@$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	$(MT_EXE)
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(MAIN_O): $(MAIN_S)
	@printf "Compiling main...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Main compiled.\n"

$(HISTOGRAM_MERGER_O): $(HISTOGRAM_MERGER_S)
	@printf "Compiling utility: \"Histogram_merger\"...\n"
	@$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core


distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)


#.$(SrcSuf).$(ObjSuf):
#	$(CXX) $(CXXFLAGS) -c $<

