# flags
CXXFLAGS = -O2 -std=c++14 -D_UNICODE -DUBICODE -municode -Wall
LDFLAGS = -static -s -Wl,--subsystem,windows

# toolchains
CXX = x86_64-w64-mingw32-g++
RC = x86_64-w64-mingw32-windres

# targets defs
EXE = sfn.exe
OBJS = out/main.o out/sfn_core.o out/res.o

.PHONY: all clean

all: out/${EXE}

# source dependencies, it should be simpler if we put all the source files in one folder
out/main.o: touchcursor/main.cpp
out/sfn_core.o: sfn/sfn_core.cpp
out/res.o: touchcursor/touchcursor.rc


# more common rules

clean:
	@rm -rf out/*

out:
	@mkdir -p "$@"

out/%.o: | out
	${CXX} ${CXXFLAGS} -c "$<" -o "$@"

out/res.o: | out
	${RC} -i "$<" -o "$@"

# Buld the executable
out/${EXE}: ${OBJS} | out
	${CXX} -o "$@" ${OBJS} ${LDFLAGS}
