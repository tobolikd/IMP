PROJ=meteostanice

SHELL=/bin/bash

all: $(PROJ)
.PHONY: all clean

$(PROJ):
	latexmk -pdf -bibtex -pdflatex="pdflatex -interaction=nonstopmode" $(PROJ).tex

clean:
	latexmk -c

purge:
	latexmk -C
	rm -f *.bbl *.xml
