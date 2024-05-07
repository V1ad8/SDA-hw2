build:
	cd skel/ && make build && cd ../

clean:
	cd skel/ && make clean && cd ../

pack:
	make clean
	zip -FSr 315CA_UngureanuVlad-Marin_Homework2.zip README.md skel/ img/ Makefile