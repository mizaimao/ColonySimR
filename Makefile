all:
	clang++ -std=c++20 -fcolor-diagnostics -fansi-escape-codes -g main.cpp -I . -I src/include -L src/lib -o main -l sfml-graphics -l sfml-window -l sfml-system -l sfml-audio -v