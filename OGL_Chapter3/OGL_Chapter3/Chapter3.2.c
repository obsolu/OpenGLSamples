#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define WINDOW_TITLE_PREFIX "Chapter 2.1"

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

unsigned FrameCount = 0;

GLuint
VertexShaderId,
FragmentShaderId,
ProgramId,
VaoId,
VboId,
ColorBufferId;

/*
	#version 400 //version of the GLSL 4.00

	layout(location=0) in vec4 in_Position; //vertex's positional data stored in vec4, "in" means this is a parameter into the shader
	layout(location=1) in vec4 in_Color;
	out vec4 ex_Color; // the final vertex color to be copied to the next shader stage

	void main(void)
	{
		gl_Position = in_Position;
		ex_Color = in_Color;
	}

*/

const GLchar* VertexShader =
{
	"#version 400\n"\

	"layout(location=0) in vec4 in_Position;\n"\
	"layout(location=1) in vec4 in_Color;\n"\
	"out vec4 ex_Color;\n"\

	"void main(void)\n"\
	"{\n"\
	"  gl_Position = in_Position;\n"\
	"  ex_Color = in_Color;\n"\
	"}\n"
};

const GLchar* FragmentShader =
{
	"#version 400\n"\

	"in vec4 ex_Color;\n"\
	"out vec4 out_Color;\n"\

	"void main(void)\n"\
	"{\n"\
	"  out_Color = ex_Color;\n"\
	"}\n"
};

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);
void IdleFunction(void);
void Cleanup(void);
void CreateVBO(void);
void DestroyVBO(void);
void CreateShaders(void);
void DestroyShaders(void);

int main(int argc, char* argv[])
{
	Initialize(argc, argv);

	glutMainLoop();

	exit(EXIT_SUCCESS);
}

void Initialize(int argc, char* argv[])
{
	GLenum GlewInitResult;

	InitWindow(argc, argv);

	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult) {
		fprintf(
			stderr,
			"ERROR: %s\n",
			glewGetErrorString(GlewInitResult)
			);
		exit(EXIT_FAILURE);
	}

	fprintf(
		stdout,
		"INFO: OpenGL Version: %s\n",
		glGetString(GL_VERSION)
		);

	CreateShaders();
	CreateVBO();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS
		);

	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);

	if (WindowHandle < 1) {
		fprintf(
			stderr,
			"ERROR: Could not create a new rendering window.\n"
			);
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0, TimerFunction, 0);
	glutCloseFunc(Cleanup);
}

void ResizeFunction(int Width, int Height)
{
	CurrentWidth = Width;
	CurrentHeight = Height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void RenderFunction(void)
{
	++FrameCount;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw the content of the VBO to the screen
	/*
		GLenum mode: specify the type of array data that's going to be drawn to the screen.
		GLint first: specify the first index of the enabled vertex attribute arrays that we want to draw.
		GLsizei count:  specify how many of the enabled indices to draw
	*/
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glutSwapBuffers();
}

void IdleFunction(void)
{
	glutPostRedisplay();
}

void TimerFunction(int Value)
{
	if (0 != Value) {
		char* TempString = (char*)
			malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(
			TempString,
			"%s: %d Frames Per Second @ %d x %d",
			WINDOW_TITLE_PREFIX,
			FrameCount * 4,
			CurrentWidth,
			CurrentHeight
			);

		glutSetWindowTitle(TempString);
		free(TempString);
	}

	FrameCount = 0;
	glutTimerFunc(250, TimerFunction, 1);
}

void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

void CreateVBO(void)
{
	GLfloat Vertices[] = {
		-0.8f, 0.8f, 0.0f, 1.0f,
		0.8f, 0.8f, 0.0f, 1.0f,
		-0.8f, -0.8f, 0.0f, 1.0f,
		0.8f, -0.8f, 0.0f, 1.0f
	};

	GLfloat Colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	GLenum ErrorCheckValue = glGetError();

	/*
		A VAO is an object that describes how the vertex attributes are stored in a VBO,
		the VAO is not a actual object storing the vertex data,but the descriptor of the vertex data.
		Vertex attributes can be described by the glVertexAttribPointer function and its two sister functions
		glVertexAttribIPointer and glVertexAttribLPointer
	*/

	glGenVertexArrays(1, &VaoId); //generate VAOs in the GPU's memory
	glBindVertexArray(VaoId);

	/*
		glGenBuffers generate buffers in the GPU's memory.
		GLSizei n: the number of buffers requested
		GLuint * buffers: store the identifiers of the buffers.
	*/
	glGenBuffers(1, &VboId);

	/*
		Buffers can be bound to several different targets.
		GL_ARRAY_BUFFER signifies that the data provided contains vertex attributes.
		It also activates the current buffer object as the target
	*/
	glBindBuffer(GL_ARRAY_BUFFER, VboId);

	/*
		After binding buffer, glBufferData copies vertex data over to GPU's memory.
		GLenum target: copy data to the currently activated buffer with this target type.
		GLsizeiptr size:the size parameter is set to the size in bytes of our Vertices array.
		GGLvoid* data:datas uploaded to the GPU's memory.
		GLenum usage:how the data is used  in the GPU.
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	
	//provide OpenGL all of the necessary metadata to use the block of raw data that we've uploaded to the GPU
	/*
		GLuint index:a user-maintained number,used in GLSL such as :layout(location=index)
		GLint size: the amount of components that make up a vertex.
		GLenum type: the data-type of each component passed to the type parameter
		GLboolean normalized:normalize the values to a certain range before using them. signed integers to [-1, 1] unsigned int to [0,1] for example
		GLsizei stride:signifies how many bytes are between the significant blocks of data
		const GLvoid* pointer:	a numerical offset in bytes in the block of data  supplied in glBufferData to where the significant data starts. work with the stride
	*/
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	//enable the attribute,allows the vertex data to be used for drawing purposes.
	//GLuint index: the same value as glVertexAttribPointer's first parameter
	//when you are done drawing the buffer object, a call to glDisableVertexAttribArray should be made.
	glEnableVertexAttribArray(0);

	//Color data  same as Vertex Data
	glGenBuffers(1, &ColorBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void DestroyVBO(void)
{
	GLenum ErrorCheckValue = glGetError();

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	//0 means no buffers should be tied to GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// opposite of glGenBuffers
	glDeleteBuffers(1, &ColorBufferId);
	glDeleteBuffers(1, &VboId);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void CreateShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	//create shader object
	//GLenum shadertype : shader type
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);

	//copy GLSL source code to OpenGL for compilation
	/*
		GLuint shader: associated shader object .
		GLsizei count: how many strings are present in the string parameter.
		const GLchar** string: GLSL source code string.
		const GLint * length : a array of integers denoting the lengths of the strings in the string parameter.you can usually leave this at NULL if you are normal null-terminated strings
	*/
	glShaderSource(VertexShaderId, 1, &VertexShader, NULL);

	//compile the source code
	glCompileShader(VertexShaderId);

	//same as vertex shader 
	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
	glCompileShader(FragmentShaderId);

	//create a shader program object to combine all individual shaders.
	ProgramId = glCreateProgram();
	// attach individual shaders to shader program object.
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	//link the shader program object.
	glLinkProgram(ProgramId);

	//use the shader program object and keep active until next call.
	glUseProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}

void DestroyShaders(void)
{
	GLenum ErrorCheckValue = glGetError();

	//by passing 0 to glUseProgram to tell OpenGL to stop using the shader program.
	glUseProgram(0);

	// opposite of attach shader 
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);

	glDeleteProgram(ProgramId);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not destroy the shaders: %s \n",
			gluErrorString(ErrorCheckValue)
			);

		exit(-1);
	}
}