#include "RenderManager.h"
#include "CoreEngine.h"
#include "Shader.h"
#include "Mesh.h"
#include "Transform.h"


#define GLM_FORCE_SWIZZLE
#include "glm.hpp"
#include "GL/glew.h"
#include <GL/GL.h> // MUST follow glew.h...
#include <SDL_egl.h>
#include "SDL.h"
#undef main // Required to prevent SDL from redefining main...

//using std::ifstream;
using glm::vec3;
using glm::vec4;

// DEBUG:
#include <iostream>
using std::cout;
using std::to_string;


namespace BlazeEngine
{
	RenderManager::~RenderManager()
	{
		// Close our window in the destructor so we can still read any final OpenGL error messages before it is destroyed
		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(glWindow);
		SDL_Quit();
	}

	RenderManager& RenderManager::Instance()
	{
		static RenderManager* instance = new RenderManager();
		return *instance;
	}

	void RenderManager::Startup(CoreEngine * coreEngine)
	{
		EngineComponent::Startup(coreEngine);

		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager started!" });

		// Cache the relevant config data:
		this->xRes = coreEngine->GetConfig()->renderer.windowXRes;
		this->yRes = coreEngine->GetConfig()->renderer.windowYRes;
		this->windowTitle = coreEngine->GetConfig()->renderer.windowTitle;

		// Initialize SDL:
		/*SDL_Init(SDL_INIT_VIDEO);*/ // TO DO: IMPLEMENT PER-COMPONENT INITIALIZATION

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
		
		// Configure SDL:
		SDL_SetRelativeMouseMode(SDL_TRUE);	// Lock the mouse to the window

		/*SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);*/

		// /* Enable multisampling for a nice antialiased effect */
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

		/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);*/


		//// Make our buffer swap syncronized with the monitor's vertical refresh:
		//SDL_GL_SetSwapInterval(1);

		// Configure OpenGL:
		glEnable(GL_DEPTH_TEST);			// Enable Z depth testing
		//glDepthFunc(GL_LESS);				// How to sort Z
		
		

		// Initialize glew:
		GLenum glStatus = glewInit();
		if (glStatus != GLEW_OK)
		{
			this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_ENGINE_QUIT, this, "Render manager start failed: glStatus not ok!" });
			return;
		}

		// Generate and bind our Vertex Array Object as active:
		glGenVertexArrays(1, &vertexArrayObject); // Allocate the required number of vertex array objects (VAO's)
		glBindVertexArray(vertexArrayObject); // Bind our VAO

		// Create and bind a vertex buffer to a buffer binding point allocated on the GPU suitable for vertices:
		glGenBuffers(1, &vertexBufferObjects[VERTEX_BUFFER_POSITION]); // Create a vertex position buffer object and store its handle
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_POSITION]); // Bind our VBO to GL_ARRAY_BUFFER
		glEnableClientState(GL_VERTEX_ARRAY); // SHOULD WE CALL THIS ONCE, OR EVERY FRAME? (undone in Shutdown())

		// Tell OpenGL how to interpet the data we'll put on the GPU:
		//glVertexPointer(3, GL_FLOAT, 0, 0); // GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)(0)); // GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // Define array of vertex attribute data: index, number of components (3 = 3 elements in vec3), type, should data be normalized?, stride, offset from start to 1st component
		//glEnableVertexAttribArray(0); // Indicate that the vertex attribute at index 0 is being used

		
		// Create and bind the color buffer:
		glGenBuffers(1, &vertexBufferObjects[VERTEX_BUFFER_COLOR]);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_COLOR]);



		// TO DO: OTHER BUFFERS



		// Bind our index buffer:
		glGenBuffers(1, &vertexBufferObjects[VERTEX_BUFFER_INDEXES]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_INDEXES]);

		// Cleanup: Bind object 0 to GL_ARRAY_BUFFER to unbind vertexBufferObjects[VERTEX_BUFFER_POSITION]
		glBindBuffer(GL_ARRAY_BUFFER, 0); 


		ClearWindow(vec4(0.79f, 0.32f, 0.0f, 1.0f));
	}

	void RenderManager::Shutdown()
	{
		this->coreEngine->BlazeEventManager->Notify(new EventInfo{ EVENT_LOG, this, "Render manager shutting down..." });

		glDeleteVertexArrays(VERTEX_BUFFER_SIZE, &vertexArrayObject);
		glDeleteBuffers(VERTEX_BUFFER_SIZE, vertexBufferObjects);

		glDisableClientState(GL_VERTEX_ARRAY);
	}

	void RenderManager::Update()
	{
		
	}


	void RenderManager::Render(double alpha)
	{
		// Clear the frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TO DO: Loop by material, shader, mesh:
		// Pre-store all vertices for the scene in (material, shader) buffers?


		// Loop through every renderable:
		vector<Renderable const*> const* renderables = coreEngine->BlazeSceneManager->GetRenderables();
		vector<Shader>* shaders = coreEngine->BlazeSceneManager->GetShaders(); // TO DO: Cache these off during Startup() ?

		for (int i = 0; i < renderables->size(); i++)
		{
			// Loop through every view mesh:
			int numViewMeshes = (int)renderables->at(i)->ViewMeshes()->size();

			for (int j = 0; j < numViewMeshes; j++)
			{
				Mesh* mesh = renderables->at(i)->ViewMeshes()->at(j);
				unsigned int shaderIndex = mesh->GetMaterial()->GetShaderIndex();

				// Assemble the model matrix for this mesh:
				Transform const* transform = renderables->at(i)->GetTransform();
				mat4 model = renderables->at(i)->GetTransform()->Model();


				// Bind the required VAO:
				glBindVertexArray(vertexArrayObject);

				// Bind our position VBO as active and copy vertex data into the buffer
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_POSITION]);		// TO DO: Define when/which obects should use GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW ??
				glBufferData(GL_ARRAY_BUFFER, mesh->NumVerts() * sizeof(Vertex), &mesh->Vertices()[0].position.x, GL_STATIC_DRAW); // <- should we be null checking?

				//// Colors:
				//glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_COLOR]);
				//glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

				// Bind our index VBO as active:
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[VERTEX_BUFFER_INDEXES]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->NumIndices() * sizeof(GLubyte), &mesh->Indices()[0], GL_STATIC_DRAW);

			
				// Set the active shader: ...TO DO: Decide whether to use this directly, or via BindShader() ?
				glUseProgram(shaders->at(shaderIndex).ShaderReference()); // ...TO DO: Decide whether to use this directly, or via BindShader() ?

				// Updload the mvp to the shader:
				mat4 mvp = this->coreEngine->BlazeSceneManager->MainCamera()->ViewProjection() * model;
				GLuint matrixID = glGetUniformLocation(shaders->at(shaderIndex).ShaderReference(), "in_mvp");
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);



				// Draw!
				//glDrawArrays(GL_TRIANGLES, 0, mesh->NumVerts()); // Type, start index, size

				/* Invoke glDrawElements telling it to draw a triangle strip using 6 indicies */
				glDrawElements(GL_TRIANGLE_STRIP, mesh->NumIndices(), GL_UNSIGNED_BYTE, (void*)(0)); // (GLenum mode, GLsizei count, GLenum type,const GLvoid * indices);
				// ^^^ GL_TRIANGLE_STRIP ??
				// GL_UNSIGNED_BYTE
				// mesh->NumIndices()

				glBindBuffer(GL_ARRAY_BUFFER, 0); // Cleanup: Bind object 0 to GL_ARRAY_BUFFER to unbind vertexBufferObjects[VERTEX_BUFFER_POSITION]
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		// Display the new frame:
		SDL_GL_SwapWindow(glWindow);


		// DEBUG:
		//SDL_Delay((unsigned int)(1000.0 / 60.0));
	}



	void RenderManager::ClearWindow(vec4 clearColor)
	{
		// Set the initial color in both buffers:
		glClearColor(GLclampf(clearColor.r), GLclampf(clearColor.g), GLclampf(clearColor.b), GLclampf(clearColor.a));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(glWindow);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}


