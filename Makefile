default, all, measure:
	g++ --std=c++17 -Wall Measure.cpp -O3 -o Measure

clean:
	$(RM) -rf Build
