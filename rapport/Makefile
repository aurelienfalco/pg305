NAME = rapport

all: $(NAME:=.tex)
	@pdflatex $^
	@pdflatex $^ >/dev/null
	@evince $(NAME:=.pdf) &

automate: img/automat-slave.png img/automat-master.png

%.png: %.dot
	dot $^ -Tpng -o $@

clean:
	rm -rf *.log *.aux *.toc *.pdf *.out img/automat*.png
