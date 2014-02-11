CXX=clang++
CXXFLAGS=-O2 -Iinclude
LDFLAGS=`pkg-config --cflags opencv` `pkg-config --libs opencv sqlite3` -lboost_system -lboost_timer

default:
	$(CXX) $(CXXFLAGS) -o vision src/BiomarkerImageProcessor.cpp src/DataPersistence.cpp src/imgtest.cpp $(LDFLAGS)
regr:
	$(CXX) $(CXXFLAGS) -o regression src/ModelComponent.cpp src/RegressionModel.cpp src/RegressionFactory.cpp src/regrtest.cpp $(LDFLAGS)

