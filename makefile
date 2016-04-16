install: RunTemplates RunSignalCutflow

RunTemplates: obj/RunTemplates.o 
	g++ -o RunTemplates obj/RunTemplates.o `root-config --libs`

RunSignalCutflow: obj/RunSignalCutflow.o
	g++ -o RunSignalCutflow obj/RunSignalCutflow.o `root-config --libs`

obj/RunTemplates.o: src/RunTemplates.cxx src/TemplateMaker.cxx src/TemplateMaker.h
	g++ -c `root-config --cflags` src/RunTemplates.cxx
	mkdir -p obj
	mv RunTemplates.o obj

obj/RunSignalCutflow.o: src/RunSignalCutflow.cxx src/TemplateMaker.cxx src/TemplateMaker.h
	g++ -c `root-config --cflags` src/RunSignalCutflow.cxx
	mkdir -p obj
	mv RunSignalCutflow.o obj

clean:
	rm obj/*.o RunTemplates RunSignalCutflow




