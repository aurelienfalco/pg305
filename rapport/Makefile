NAME = rapport
IMG = img

all: $(NAME:=.tex) automate
	@pdflatex $<
	@pdflatex $< >/dev/null
	@evince $(NAME:=.pdf) &

automate: $(IMG)/automat-slave.png $(IMG)/automat-master.png

%.png: %.dot
	dot $^ -Tpng -o $@

clean:
	rm -rf *.log *.aux *.toc *.pdf *.out $(IMG)/automat*.png
