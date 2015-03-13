OPTFLAGS        = -O3 -Wall -Werror -Wextra -Wstrict-prototypes -fno-common -pedantic -g
CC              = gcc
MAKE            = make
RM              = rm -f
DOXYGEN         = doxygen
CD              = cd
MV              = mv
GREP            = grep
EXCLUDE_PATTERN = footrulewidth
OBJECTS          = datei.o

%.o : %.c
	$(CC) $(OPTFLAGS) -o $@ -c $<

all: datei

myfind: $(OBJECTS)
	$(CC) $(OPTFLAGS) -o $@ $^

clean:
	$(RM) *.o *.h.gch datei

clean_doc:
	$(RM) -r doc/ html/ latex/

doc: clean_doc
	$(DOXYGEN)

clean_all: clean clean_doc

doc: html pfd

html: 
  $(DOXYGEN) doxygen.dcf

pdf: html 
  $(CD) doc/pdf && \ 
   $(MV) refman.tex refman_save.tex && \ 
   $(GREP) -v $(EXCLUDE_PATTERN) refman_save-tex > refman.tex && \
   $(RM) refman_save.tex && \ 
   make && \ 
   $(MV) refman.pdf refman.save && \ 
   $(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \ *.ilg *.toc *.tps makefile && \ 
   $(MV) refman.save refman.pdf
