NAME = rapport

all: $(NAME:=.tex)
	@pdflatex $^
	@pdflatex $^ >/dev/null
	@evince $(NAME:=.pdf) &

clean:
	rm -rf *.log *.aux *.toc *.pdf *.out
