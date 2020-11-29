#include "scene.h"
#include "scenerunner.h"
#include "scenerendertotex.h"
#include "scenediffibl.h"

enum {FBO = 0, IBL = 1};

int main(int argc, char *argv[])
{
	int recipe = FBO;
	recipe = IBL;

	SceneRunner runner("FrameBufferObject", 1280, 720, 4);
	std::unique_ptr<Scene> scene;

	switch(recipe) 
	{
		case FBO: 
			scene = std::unique_ptr<Scene>(new SceneRenderToTex()); break;
		case IBL:
			scene = std::unique_ptr<Scene>(new SceneDiffIbl()); break;
		default:  
			printf("Unknown recipe");
			exit(EXIT_FAILURE);
			break;
	}

    return runner.run(std::move(scene));
}
