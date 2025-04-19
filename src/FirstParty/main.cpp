#include "Engine.h"

int g_uuid = 0; //global!
int g_n = 0; //used for adding components at runtime

int main(int argc, char* argv[]){
	
	Engine e;

	e.GameLoop();

	return 0;
}