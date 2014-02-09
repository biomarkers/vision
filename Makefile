CXX=clang++
CXXFLAGS=-O2 -Iinclude
LDFLAGS=`pkg-config --cflags opencv` `pkg-config --libs opencv` -lboost_system -lboost_timer

default:
	$(CXX) $(CXXFLAGS) -o vision src/BiomarkerImageProcessor.cpp $(LDFLAGS)
regr:
	$(CXX) $(CXXFLAGS) -o regression src/ModelComponent.cpp src/RegressionModel.cpp src/RegressionFactory.cpp src/regrtest.cpp $(LDFLAGS)

