OPTFLAGS        = -O3 -Wall -Werror -Wextra -pedantic -g
CC              = gcc
MAKE            = make
RM              = rm -f
DOXYGEN         = doxygen

%.o : %.c
	$(CC) $(OPTFLAGS) -o $@ -c $<

all: datei

myfind: datei.o
	$(CC) $(OPTFLAGS) datei.o -o myfind

clean:
	$(RM) *.o *.h.gch myfind 

clean_doc:
	$(RM) -r doc/ html/ latex/

doc: clean_doc
	$(DOXYGEN)

clean_all: clean clean_doc

doc: html pfd

html: 
  $(DOXYGEN) doxygencgf

pdf: html 
  $(CD) doc/pdf && \ 
   $(MV) refman.tex refman_save.tex && \ 
   $(GREP) -v $(EXCLUDE_PATTERN) refman_save-tex > refman.tex && \
   $(RM) refman_save.tex && \ 
   make && \ $(MV) refman.pdf refman.save && \ 
   $(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \ *.ilg *.toc *.tps makefile && \ 
   $(MV) refman.save refman.pdf
