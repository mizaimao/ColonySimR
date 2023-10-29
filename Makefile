all:
	clang++ -std=gnu++14 -fcolor-diagnostics -fansi-escape-codes -g main.cpp -I src/include -I src/include/vis -L src/lib -o main -l sfml-graphics -l sfml-window -l sfml-system -l sfml-audio -v