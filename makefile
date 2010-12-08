IFLAGS=-Isource -Isource/XPG/glew

test-xpg: main.o FancyTestModule.o Display.o Event.o IndexVBO.o Module.o Shader.o Surface.o Timer.o
	g++ $(IFLAGS) -o test-xpg -lGL -lX11 *.o

main.o: source/test/main.cpp source/test/FancyTestModule.h source/XPG/Display.hpp
	g++ $(IFLAGS) -c source/test/main.cpp

FancyTestModule.o: source/test/FancyTestModule.cpp source/test/FancyTestModule.h source/XPG/Timer.hpp source/XPG/ClusterVBO.hpp source/XPG/IndexVBO.hpp source/XPG/Module.hpp source/XPG/Program.hpp
	g++ $(IFLAGS) -c source/test/FancyTestModule.cpp

glew.o: source/XPG/glew/glew.c source/XPG/glew/GL/glew.h source/XPG/glew/GL/glxew.h source/XPG/glew/GL/wglew.h
	gcc $(IFLAGS) -c source/XPG/glew/glew.c

Display.o: source/XPG/core/Display.cpp source/XPG/linux/Display.cpp source/XPG/Display.hpp source/XPG/Event.hpp source/XPG/Module.hpp source/XPG/Timer.hpp source/XPG/glew/GL/glew.h source/XPG/glew/GL/glxew.h glew.o
	g++ $(IFLAGS) -c source/XPG/linux/Display.cpp source/XPG/core/Display.cpp

Event.o: source/XPG/core/Event.cpp source/XPG/Event.hpp source/XPG/DataTypes.hpp
	g++ $(IFLAGS) -c source/XPG/core/Event.cpp
	
IndexVBO.o: source/XPG/core/IndexVBO.cpp source/XPG/IndexVBO.hpp source/XPG/OpenGL.hpp
	g++ $(IFLAGS) -c source/XPG/core/IndexVBO.cpp

Module.o: source/XPG/core/Module.cpp source/XPG/Module.hpp source/XPG/OpenGL.hpp source/XPG/DataTypes.hpp source/XPG/Event.hpp
	g++ $(IFLAGS) -c source/XPG/core/Module.cpp

Shader.o: source/XPG/core/Shader.cpp source/XPG/Shader.hpp source/XPG/OpenGL.hpp
	g++ $(IFLAGS) -c source/XPG/core/Shader.cpp

Surface.o: source/XPG/core/Surface.cpp source/XPG/Surface.hpp source/XPG/DataTypes.hpp
	g++ $(IFLAGS) -c source/XPG/core/Surface.cpp

Timer.o: source/XPG/unix/Timer.cpp source/XPG/Timer.hpp
	g++ $(IFLAGS) -c source/XPG/unix/Timer.cpp
