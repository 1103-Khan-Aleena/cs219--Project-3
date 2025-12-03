sim: main.o cpu.o parser.o helpers.o
	g++ -o sim main.o cpu.o parser.o helpers.o

main.o: main.cpp cpu.h parser.h helpers.h
	g++ -c main.cpp -g

cpu.o: cpu.cpp cpu.h instr.h helpers.h
	g++ -c cpu.cpp -g

parser.o: parser.cpp parser.h instr.h helpers.h
	g++ -c parser.cpp -g

helpers.o: helpers.cpp helpers.h
	g++ -c helpers.cpp -g

# Clean up
clean:
	rm -f *.o sim
