install: RunTemplates

RunTemplates: obj/RunTemplates.o 
	g++ -o RunTemplates obj/RunTemplates.o `root-config --libs`

obj/RunTemplates.o: src/RunTemplates.cxx src/TemplateMaker.cxx src/TemplateMaker.h
	g++ -c `root-config --cflags` src/RunTemplates.cxx
	mkdir -p obj
	mv RunTemplates.o obj

clean:
	rm obj/*.o RunTemplates




