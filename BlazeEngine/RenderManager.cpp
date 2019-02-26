#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"

#include "glm.hpp"
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...
#include <SDL_egl.h>
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

using glm::vec3;
using glm::vec4;


namespace BlazeEngine
{
	RenderManager::~RenderManager()
	{
		
	}

	RenderManager& RenderManager::Instance()
	{
		static RenderManager* instance = new RenderManager();
		return *instance;
	}

	void RenderManager::Render(double alpha)
	{
		// Loop through every renderable:
		vector<Renderable const*> const* renderables = coreEngine->BlazeSceneManager->GetRenderables();
		for (int i = 0; i < renderables->size(); i++)
		{
			// Loop through every mesh:
			int numRenderables = (int)renderables->size();
			for (int j = 0; j < numRenderables; j++)
			{
				Mesh* mesh = renderables->at(i)->ViewMeshes()->at(j);

				// ??
				//glGenVertexArrays(1, &vertexArrayObject); // Size, target
				//glBindVertexArray(vertexArrayObject);

				// Allocate a vertex buffer:
				//glGenBuffers(VERTEX_BUFFER_SIZE, vertexArrayBuffers); // Allocate buffer on the GPU
				//glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[VERTEX_BUFFER_POSITION]); // Tell OpenGl to interpret buffer as an array
				//^^^ Moved to startup

				// Copy vertex data into the buffer:
				glBufferData(GL_ARRAY_BUFFER, mesh->NumVerts() * sizeof(mesh->Vertices()[0]), mesh->Vertices(), GL_STATIC_DRAW); // Put data into the buffer
				// TODO: Define which obects that use GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW

				//// Tell OpenGL how to interpet the data we've put on the GPU:
				//glEnableVertexAttribArray(0); // Treat data as an array
				//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell it how to read the array: attribute array, count (3 = 3 elements in vec3), data type, normalize?, space between steps(?), start offset

				//glBindVertexArray(0); // Prevent further vertex array object operations affecting our vertex array object

				//glBindVertexArray(vertexArrayObject);

				glDrawArrays(GL_TRIANGLES, 0, mesh->NumVerts()); // Type, start index, size

				/*renderables->at(i)->GetTransform();*/
			}
		}

		//// Copy vertices to the currently bound buffer:
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // SHOULDN'T THIS BE sizeof(vertices) * num of verts OR vertices[0] ????????

		// Display the new frame:
		SDL_GL_SwapWindow(glWindow);


		// DEBUG:
		SDL_Delay((unsigned int)(1000.0 / 60.0));
	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		// Cache the relevant config data:
		this->xRes = coreEngine->GetConfig()->renderer.windowXRes;
		this->yRes = coreEngine->GetConfig()->renderer.windowYRes;
		this->windowTitle = coreEngine->GetConfig()->renderer.windowTitle;

		// Initialize SDL:
		/*SDL_Init(SDL_INIT_VIDEO);*/ // Currently doing a global init... 

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		// Create a window:
		glWindow = SDL_CreateWindow(
			coreEngine->GetConfig()->renderer.windowTitle.c_str(), 
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
			xRes, 
			yRes, 
			SDL_WINDOW_OPENGL
		);

		// Create an OpenGL context:
		glContext = SDL_GL_CreateContext(glWindow);
		
		// Initialize glew:
		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, "Render manager start failed: glStatus not ok!" });
			return;
		}


		// ??
		glGenVertexArrays(1, &vertexArrayObject); // Size, target
		glBindVertexArray(vertexArrayObject);

		// Create and bind a vertex buffer:
		glGenBuffers(VERTEX_BUFFER_SIZE, vertexArrayBuffers); // Allocate buffer on the GPU
		glBindBuffer(GL_ARRAY_BUFFER, vertexArrayBuffers[VERTEX_BUFFER_POSITION]); // Tell OpenGl to interpret buffer as an array



		// Tell OpenGL how to interpet the data we've put on the GPU:
		glEnableVertexAttribArray(0); // Treat data as an array
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Tell it how to read the array: attribute array, count (3 = 3 elements in vec3), data type, normalize?, space between steps(?), start offset

		glBindVertexArray(0); // Prevent further vertex array object operations affecting our vertex array object

		glBindVertexArray(vertexArrayObject);


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));


		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager started!" });
	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager shutting down..." });

		glDeleteVertexArrays(1, &vertexArrayObject);

		// Close our window:
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(glWindow);
		SDL_Quit();
	}

	void RenderManager::Update()
	{
		
	}

	void RenderManager::ClearWindow(vec4 clearColor)
	{
		// Set the initial color in both buffers:
		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(glWindow);

		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT);
	}


}


