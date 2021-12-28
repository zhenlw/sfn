# flags
CXXFLAGS = -O2 -std=c++14 -D_UNICODE -DUBICODE -municode -Wall
LDFLAGS = -static -s -Wl,--subsystem,windows

# targets defs
EXE = sfn.exe
OBJS = out/main.o out/sfn_core.o out/res.o

# toolchains
CXX = x86_64-w64-mingw32-g++
RC = x86_64-w64-mingw32-windres

.PHONY: all clean

all: out/${EXE}

# more common rules

clean:
	@rm -rf out/*

out:
	@mkdir -p "$@"

out/%.o: sfn/%.cpp | out
	${CXX} ${CXXFLAGS} -c "$<" -o "$@"

out/res.o: sfn/sfn.rc | out
	${RC} -i "$<" -o "$@"

# Buld the executable
out/${EXE}: ${OBJS} | out
	${CXX} -o "$@" ${OBJS} ${LDFLAGS}
