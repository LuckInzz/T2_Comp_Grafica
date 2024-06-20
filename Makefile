# Makefile para Linux e macOS

PROG = BasicoOpenGL
#FONTES = Linha.cpp Ponto.cpp InterseccaoEntreTodasAsLinhas.cpp Temporizador.cpp
FONTES = Ponto.cpp Poligono.cpp Temporizador.cpp ListaDeCoresRGB.cpp Instancia.cpp ModeloMatricial.cpp TransformacoesGeometricas.cpp 

OBJETOS = $(FONTES:.cpp=.o)
CPPFLAGS = -g -O3 -DGL_SILENCE_DEPRECATION # -Wall -g  # Todas as warnings, infos de debug

UNAME = `uname`

all: $(TARGET)
	-@make $(UNAME)

Darwin: $(OBJETOS)
#	g++ $(OBJETOS) -O3 -Wno-deprecated -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)
	g++ $(OBJETOS) -O3 -framework OpenGL -framework Cocoa -framework GLUT -lm -o $(PROG)

Linux: $(OBJETOS)
	g++ $(OBJETOS) -O3 -lGL -lGLU -lglut -lm -o $(PROG)

clean:
	-@ rm -f $(OBJETOS) $(PROG)
