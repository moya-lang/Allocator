default, all, measure:
	mkdir -p Build
	g++ --std=c++17 -Wall -IAllocator Example/Measure.cpp -O3 -o Build/Measure

clean:
	$(RM) -rf Build
