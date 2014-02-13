CXX=clang++
CXXFLAGS=-O2 -Iinclude
LDFLAGS=`pkg-config --cflags opencv` `pkg-config --libs opencv sqlite3` -lboost_system -lboost_timer -lboost_serialization

default:
	$(CXX) $(CXXFLAGS) -o vision src/BiomarkerImageProcessor.cpp src/persist/DataStore.cpp src/imgtest.cpp $(LDFLAGS)
regr:
	$(CXX) $(CXXFLAGS) -o regression src/SerializableMat.cpp src/SerializableScalar.cpp src/ModelComponent.cpp src/RegressionModel.cpp src/RegressionFactory.cpp src/regrtest.cpp $(LDFLAGS)

