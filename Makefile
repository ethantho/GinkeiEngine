main:    
	clang++ -std=c++17 \
	-I./src/ThirdParty/SDL2-2.28.5/include/ \
	-I./src/ThirdParty/SDL2_image-2.6.3/include/ \
	-I./src/ThirdParty/SDL2_ttf-2.20.2/include/ \
	-I./src/ThirdParty/SDL2_mixer-2.6.3/include/ \
	-I./LuaInstall/include/ \
	-lSDL2 \
	-lSDL2_mixer \
	-lSDL2_ttf \
	-lSDL2_image \
	-L./LuaInstall/lib \
	-llua5.4 \
	-I./src/ThirdParty/LuaBridge/ \
	-I./src/ThirdParty/rapidjson-1.1.0/include/ \
	-I./src/ThirdParty \
	-I./src/ThirdParty/glm-0.9.9.8 \
	-I./src/ThirdParty/box2d-2.4.1/include \
	-I./src/ThirdParty/box2d-2.4.1/src \
	src/FirstParty/*.cpp \
	src/ThirdParty/box2d-2.4.1/src/collision/*.cpp \
	src/ThirdParty/box2d-2.4.1/src/common/*.cpp \
	src/ThirdParty/box2d-2.4.1/src/dynamics/*.cpp \
	src/ThirdParty/box2d-2.4.1/src/rope/*.cpp \
	-o game_engine_linux -O3