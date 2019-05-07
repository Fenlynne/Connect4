objects = main.o MCTS.o Node.o State.o Judge.o
main : $(objects)
	g++ $(objects) -o main

Judge.o: Judge.cpp Judge.h
	g++ -c Judge.cpp -std=c++11
main.o: main.cpp MCTS.h Node.h State.h
	g++ -c main.cpp -std=c++11

State.o: State.cpp State.h
	g++ -c State.cpp -std=c++11

Node.o: Node.cpp Node.h State.h
	g++ -c Node.cpp -std=c++11

MCTS.o: MCTS.cpp MCTS.h Node.h State.h
	g++ -c MCTS.cpp -std=c++11

clean:
	rm -rf $(objects)