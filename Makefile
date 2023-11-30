SFML_dir_linux := /home/flynn/code_dumpster/cpp_stuff/game_dev_resources/SFML-2.6.x-linux
SFML_dir_win := /home/flynn/code_dumpster/cpp_stuff/game_dev_resources/SFML-2.6.x-win

SFML_libs := -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
#SFML_libs_static := -DSFML_STATIC -MMD -MP -static-libstdc++ -static-libgcc -lsfml-main -lsfml-audio-s -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lsfml-network-s  -lopengl32 -lfreetype -lwinmm -lgdi32 -lopenal32 -lFLAC -lvorbisenc -lvorbis -logg -lws2_32

OBJECTS := main.o line.o point.o mesh.o line_work.o simulation_image.o pattern_work.o nc_generator.o

LINUX_RELEASE_OBJECTS = $(addprefix obj/linux_, $(OBJECTS))
LINUX_DEBUG_O3_OBJECTS = $(addprefix obj/debug_o3_, $(OBJECTS))
LINUX_DEBUG_O0_OBJECTS = $(addprefix obj/debug_o0_, $(OBJECTS))
WIN_RELEASE_OBJECTS = $(addprefix obj/win_, $(OBJECTS))


DEBUG_FLAGS_O3 := -g -ggdb -O3 -Wall -Wextra
DEBUG_FLAGS_O0 := -g -ggdb -O0 -Wall -Wextra
RELEASE_FLAGS := -O3 -std=c++17

LINUX_DEBUG_LDFLAGS := -Wl,-rpath=$(SFML_dir_linux)/lib -I $(SFML_dir_linux)/include -L$(SFML_dir_linux)/lib $(SFML_libs)
LINUX_LDFLAGS := -Wl,-rpath=./lib -I $(SFML_dir_linux)/include -L$(SFML_dir_linux)/lib $(SFML_libs)
WIN_LDFLAGS := -mwindows -I $(SFML_dir_win)/include -L$(SFML_dir_win)/lib -L/usr/x86_64-w64-mingw32/lib -L/usr/x86_64-w64-mingw32/bin $(SFML_libs) -lsfml-main

WIN_COMPILER := x86_64-w64-mingw32-g++-posix
LINUX_COMPILER := g++

BUILD_VER := 0.0.4
RELEASE_PATH := release_builds/v$(BUILD_VER)

.PHONY: clean release src/main.hpp

# ==== Linux Debug build ====
app: $(LINUX_DEBUG_O3_OBJECTS)
	$(LINUX_COMPILER) $(DEBUG_FLAGS_O3) $(LINUX_DEBUG_O3_OBJECTS) -o app $(LINUX_DEBUG_LDFLAGS)

o0: $(LINUX_DEBUG_O0_OBJECTS)
	$(LINUX_COMPILER) $(DEBUG_FLAGS_O0) $(LINUX_DEBUG_O0_OBJECTS) -o app $(LINUX_DEBUG_LDFLAGS)

$(LINUX_DEBUG_O3_OBJECTS): obj/debug_o3_%.o: src/%.cpp src/%.hpp
	$(LINUX_COMPILER) $(DEBUG_FLAGS_O3) -c $< -o $@ $(LINUX_LDFLAGS)

$(LINUX_DEBUG_O0_OBJECTS): obj/debug_o0_%.o: src/%.cpp src/%.hpp
	$(LINUX_COMPILER) $(DEBUG_FLAGS_O0) -c $< -o $@ $(LINUX_LDFLAGS)

clean:
	rm -f *.o obj/*.o app
	make app

# ==== Release commands ====
release: $(LINUX_RELEASE_OBJECTS) $(RELEASE_PATH)
	rm -r -f $(RELEASE_PATH) obj/linux_*.o obj/win_*.o
	make art_cheater_linux
	make art_cheater_win64
	tar -C ./$(RELEASE_PATH) -acf ./$(RELEASE_PATH)/linux64_$(BUILD_VER).tar linux
	zip -r -q ./$(RELEASE_PATH)/win64_$(BUILD_VER).zip ./$(RELEASE_PATH)/win

$(RELEASE_PATH):
	mkdir $(RELEASE_PATH)

$(RELEASE_PATH)/win: $(RELEASE_PATH)
	mkdir $(RELEASE_PATH)/win

$(RELEASE_PATH)/linux: $(RELEASE_PATH)
	mkdir $(RELEASE_PATH)/linux

# ==== Linux Release build ====
art_cheater_linux: $(RELEASE_PATH)/linux $(LINUX_RELEASE_OBJECTS)
	cp -r $(SFML_dir_linux)/lib $(RELEASE_PATH)/linux/lib
	cp -r gfx $(RELEASE_PATH)/linux/gfx
	$(LINUX_COMPILER) $(RELEASE_FLAGS) $(LINUX_RELEASE_OBJECTS) -o $(RELEASE_PATH)/linux/Journey_mkII_linux $(LINUX_LDFLAGS)

$(LINUX_RELEASE_OBJECTS): obj/linux_%.o : src/%.cpp
	$(LINUX_COMPILER) $(RELEASE_FLAGS) -c $< -o $@ $(LINUX_LDFLAGS)

# ==== Windows Release build ====
art_cheater_win64: $(RELEASE_PATH)/win $(WIN_RELEASE_OBJECTS)
	cp -r $(SFML_dir_win)/lib/sfml-graphics-2.dll $(RELEASE_PATH)/win/sfml-graphics-2.dll
	cp -r $(SFML_dir_win)/lib/sfml-system-2.dll $(RELEASE_PATH)/win/sfml-system-2.dll
	cp -r $(SFML_dir_win)/lib/sfml-window-2.dll $(RELEASE_PATH)/win/sfml-window-2.dll
	cp /usr/lib/gcc/x86_64-w64-mingw32/10-posix/libgcc_s_seh-1.dll $(RELEASE_PATH)/win/libgcc_s_seh-1.dll
	cp /usr/lib/gcc/x86_64-w64-mingw32/10-posix/libstdc++-6.dll $(RELEASE_PATH)/win/libstdc++-6.dll
	cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll $(RELEASE_PATH)/win/libwinpthread-1.dll
	cp -r gfx $(RELEASE_PATH)/win/gfx
	$(WIN_COMPILER) $(RELEASE_FLAGS) $(WIN_RELEASE_OBJECTS) -o $(RELEASE_PATH)/win/Journey_mkII_win64 $(WIN_LDFLAGS) 

$(WIN_RELEASE_OBJECTS): obj/win_%.o : src/%.cpp
	$(WIN_COMPILER) $(RELEASE_FLAGS) -c $< -o $@ $(WIN_LDFLAGS)


	


