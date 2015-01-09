NAME = rapport

.PHONY:

all: $(NAME:=.tex)
	@pdflatex $^
	@pdflatex $^ 1>/dev/null 2>/dev/null
	@evince $(NAME:=.pdf) &

clean:
	rm -rf *.log *.aux *.toc *.pdf
