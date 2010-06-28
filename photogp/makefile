CC=g++ -mno-cygwin
PHOTOGP=src
INC=-I$(PHOTOGP) -Iphotoshopapi/photoshop -Iphotoshopapi/pica_sp -Icommon/includes 
DEF=-DWIN32=1 -D_WINDOWS
SRC_PATH=./common/sources
DEP_SRC=$(PHOTOGP)/photogpUIWin.cpp $(PHOTOGP)/photogpScripting.cpp \
        $(SRC_PATH)/PIUtilities.cpp $(SRC_PATH)/PropertyUtils.cpp \
        $(SRC_PATH)/FileUtilitiesWin.cpp $(SRC_PATH)/FileUtilities.cpp \
        $(SRC_PATH)/PIUtilitiesWin.cpp $(SRC_PATH)/DialogUtilitiesWin.cpp \
        $(SRC_PATH)/PIDLLInstance.cpp $(SRC_PATH)/PIUSuites.cpp 

LIB=-lstdc++ -lComdlg32 -lVersion

RES_CYGWIN = $(PHOTOGP)/res_cygwin
RES = $(RES_CYGWIN)/photogp.res
RES_INC = -Icommon/resources -Icommon/includes  -Iphotoshopapi/photoshop -Iphotoshopapi/pica_sp -Iphotoshopapi/resources -Isrc

release: res
	$(CC) $(PHOTOGP)/photogp.cpp -O3 -o photogp.8be -shared $(RES) $(DEP_SRC) $(DEF) $(INC) $(LIB) $(FLAGS)
debug: res
	$(CC) $(PHOTOGP)/photogp.cpp -g -o photogp.8be -shared $(RES) $(DEP_SRC) $(DEF) $(INC) $(LIB) $(FLAGS)

res:
	windres -O coff $(RES_INC) $(RES_CYGWIN)/photogp.rc $(RES_CYGWIN)/photogp.res