default:
	g++ -Iinclude `pkg-config --cflags opencv` -o vision src/vision.cpp `pkg-config --libs opencv`
regr:
	g++ -Iinclude `pkg-config --cflags opencv` -o regression src/regression.cpp `pkg-config --libs opencv`

