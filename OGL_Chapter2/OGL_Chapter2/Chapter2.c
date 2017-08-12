#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#define WINDOW_TITLE_PREFIX "Chapter 2"

int
CurrentWidth = 800,
CurrentHeight = 600,
WindowHandle = 0;

unsigned FrameCount = 0;

void Initialize(int, char*[]);
void InitWindow(int, char*[]);
void ResizeFunction(int, int);
void RenderFunction(void);
void TimerFunction(int);

//cause FreeGLUT to redraw as soon as possibly
//only run when the program has no more work to do
void IdleFunction(void);

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

	GlewInitResult = glewInit();

	if (GLEW_OK != GlewInitResult)
	{
		fprintf(stderr, "ERROR:%s\n", glewGetErrorString(GlewInitResult));
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void InitWindow(int argc, char* argv[])
{
	glutInit(&argc,argv);

	glutInitContextVersion(4, 0);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//to make sure gameloop returns and not exit the program.
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE, 
		GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowSize(CurrentWidth, CurrentHeight);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);
	if (WindowHandle < 1)
	{
		fprintf(stderr, "ERROR: Could not create a new rendering window.\n");
		exit(EXIT_FAILURE);
	}

	glutReshapeFunc(ResizeFunction);
	glutDisplayFunc(RenderFunction);
	glutIdleFunc(IdleFunction);
	glutTimerFunc(0,TimerFunction,0);
}

void ResizeFunction(int width, int height)
{
	CurrentWidth = width;
	CurrentHeight = height;
	glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void RenderFunction()
{
	++FrameCount;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glutSwapBuffers();
	glutPostRedisplay();
}

void IdleFunction()
{
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	if (0 != value)
	{
		char* TmpStr = (char*)malloc(512 + strlen(WINDOW_TITLE_PREFIX));

		sprintf(TmpStr, "%s: %d FPS @ %d x %d", WINDOW_TITLE_PREFIX, FrameCount * 4, CurrentWidth, CurrentHeight);
		glutSetWindowTitle(TmpStr);
		free(TmpStr);
	}

	FrameCount = 0; 
	glutTimerFunc(250, TimerFunction, 1);
}