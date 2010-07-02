CC=gcc -mno-cygwin
FI_INC=-IFreeImage
FI_LIB=-LFreeImage -lFreeImage
LIB = $(FI_LIB) -lstdc++ 
DEF=  -D_WIN32
debug:
	$(CC) -g -orm2gpraw.exe rm2gpraw.cpp $(FI_INC)  $(LIB) $(DEF) -DDEBUG

release:
	$(CC) -O3 -orm2gpraw.exe rm2gpraw.cpp $(FI_INC) $(LIB) $(DEF) -DRELEASE

