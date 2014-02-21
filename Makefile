CXX=clang++
CXXFLAGS=-O2 -Iinclude
LDFLAGS=`pkg-config --cflags opencv` `pkg-config --libs opencv sqlite3` -lboost_system -lboost_timer -lboost_serialization

default:
	$(CXX) $(CXXFLAGS) -o vision src/LinearRegression.cpp src/ExponentialRegression.cpp src/PointAnalysis.cpp src/ModelComponent.cpp src/RegressionFactory.cpp src/RegressionModel.cpp src/SerializableScalar.cpp src/SerializableMat.cpp src/BiomarkerImageProcessor.cpp src/persist/DataStore.cpp src/imgtest.cpp $(LDFLAGS)
regr:
	$(CXX) $(CXXFLAGS) -o regression src/LinearRegression.cpp src/ExponentialRegression.cpp src/PointAnalysis.cpp src/SerializableMat.cpp src/SerializableScalar.cpp src/ModelComponent.cpp src/RegressionModel.cpp src/RegressionFactory.cpp src/regrtest.cpp $(LDFLAGS)

