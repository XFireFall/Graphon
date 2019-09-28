#test: a.out
#	./a.out

gen: tut_1.cpp vector.hpp cube.hpp matrix.hpp canvas.hpp triangle.hpp
	g++ $^ -o $@

clean:
	rm -f a.out

help:
	@echo "  test  - Run testbench"
	@echo "  clean - Remove most generated files"
	@echo "  help  - Display this text"

.PHONY: clean gen help
