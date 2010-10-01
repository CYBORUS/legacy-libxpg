g++ -o test-xpg -O2 -Isource/XPG/glew -Isource source/XPG/glew/glew.c source/test/*.cpp source/XPG/core/*.cpp source/XPG/unix/*.cpp source/XPG/linux/*.cpp -lGL -lX11
