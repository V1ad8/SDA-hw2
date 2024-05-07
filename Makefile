build:
	cd skel/ && make build && cd ../

clean:
	cd skel/ && make clean && cd ../

pack:
	cd skel/ && make pack && cd ../
	mv skel/315CA_UngureanuVlad-Marin_Homework2.zip ./