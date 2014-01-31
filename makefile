#------------------------------------
# File:		Makefile
# Author:	M. Coahran
# Purpose:  splatter painter
# Date: 	12/28/2008
#FOR MAC
#------------------------------------

CC=g++
#  -c (don't link, just create object file)
#  -g (add debugging information)
#  -O (optimize)

FLAGS=-c -gstabs -I/usr/include

LINKTO=-o splatterer 

# This specifies where the libraries (listed below) are located. To specify
# more than one directory, simply add another '-L/...' to the line.
#LIBDIRS = -L/usr/X11R6/lib

# This specifies what libraries to link to. The actual library filenames
# have the form 'libGLU.so' or 'libGLU.a', and they are located as above.
#LIBS = -lGLU -lGL -lglut -lXmu -lXext -lX11 -lXi -lm
LIBS=-framework OpenGL -framework GLUT

#------------------------------------
pollock: pollock.o
	$(CC) -o pollock pollock.o $(LIBDIRS) $(LIBS)

pollock.o: pollock.c
	$(CC) $(FLAGS) pollock.c
	
splatterer: splatterer.o
	$(CC) $(LINKTO) splatterer.o $(LIBDIRS) $(LIBS)

splatterer.o: splatterer.c
	$(CC) $(FLAGS) splatterer.c




#------------------------------------
clean:
	rm -f *.o *~ core
