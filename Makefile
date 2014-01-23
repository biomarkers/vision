CXX=clang++
CXXFLAGS=-O2 -Iinclude
LDFLAGS=`pkg-config --cflags opencv` `pkg-config --libs opencv`

default:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o vision src/BiomarkerImageProcessor.cpp
regr:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o regression src/regression.cpp

