
DOXYGEN=/home/erik/doxygen-1.8.7/bin/doxygen

all: docstrings

.PHONY: docstrings
docstrings:
	$(DOXYGEN) Doxyfile
	python docstrings.py

clean:
	rm -rf xml docstrings.txt

