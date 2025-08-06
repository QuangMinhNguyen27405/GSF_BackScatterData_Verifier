CXX = g++
CXXFLAGS = -I./gsf
SRC = main.cpp \
      gsf/gsf.c \
      gsf/gsf_dec.c \
      gsf/gsf_enc.c \
      gsf/gsf_compress.c \
      gsf/gsf_info.c \
      gsf/gsf_indx.c \
      gsf/gsf_geo.c
OUT = checkgsf.exe

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) -lws2_32
