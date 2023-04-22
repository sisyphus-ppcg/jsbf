.DEFAULT_GOAL := all

all:
	rm -rf build _pyfuzzilli.so
	python3 setup.py build
	cp build/lib.*/_pyfuzzilli.*.so _pyfuzzilli.so
	rm -rf build