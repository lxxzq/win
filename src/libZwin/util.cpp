
#include <gl\glew.h>
#include <gl\freeglut.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")

#if 0
GLuint imageFBO;
GLuint imageID;
GLuint depthTextureID;
void SetFrameBufferObject(int fbowidth, int fboheight)
{
	// 创建FBO对象
	glGenFramebuffersEXT(1, &imageFBO);
	// 启用FBO
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, imageFBO);

	// 创建纹理
	glGenTextures(1, &imageID);
	// 启用纹理
	glBindTexture(GL_TEXTURE_2D, imageID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, fbowidth, fboheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, imageID, 0);

	//
	//glGenRenderbuffersEXT(1, &depthTextureID);
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthTextureID);
	//glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, fbowidth, fboheight);
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthTextureID);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
		printf("FBO Initialization Failed.");

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

int IMAGE_WIDTH = 250;
int IMAGE_HEIGHT = 250;

void GenImage()
{
	glPushMatrix();
	glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	glShadeModel(GL_SMOOTH);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, imageFBO);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
	glColor3f(0.0, 1.0, 0.0); glVertex3f(-1.0, 1.0, 0.0);
	glColor3f(0.0, 0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glColor3f(1.0, 0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glEnd();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopMatrix();
}

static GLuint texName;

void display(void)
{
	// 离屏渲染纹理
	GenImage();

	glViewport(0, 0, 250, 250);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//切换屏幕
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	// 启用离屏渲染的纹理影像
	glBindTexture(GL_TEXTURE_2D, imageID);
	// 创建Mipmap(多重细节层)
	glGenerateMipmapEXT(GL_TEXTURE_2D);

	// 绘制纹理影像
	glBegin(GL_QUADS);
	glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, 0.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
	glEnd();

	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glewInit();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	// 设置创建虚拟屏幕
	SetFrameBufferObject(IMAGE_WIDTH, IMAGE_HEIGHT);

	glutMainLoop();
	return 0;
}
#else

GLuint imageFBO;
GLuint imageID;
GLuint depthTextureID;


/****************************************************************************************************
* 全局变量定义
*****************************************************************************************************/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TEXTURE_WIDTH = 512;
const int TEXTURE_HEIGHT = 512;
const double NEAR_PLANE = 1.0f;
const double FAR_PLANE = 1000.0f;


GLuint fbo = 0;		// FBO对象的句柄
GLuint depthbuffer = 0;
GLuint rendertarget = 0;		// 纹理对象的句柄


void SetupResource(void)
{
	// 创建纹理
	glGenTextures(1, &rendertarget);
	glBindTexture(GL_TEXTURE_2D, rendertarget);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// 创建深度缓冲区
	glGenRenderbuffersEXT(1, &depthbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	// 创建FBO对象
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rendertarget, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		//SDL_Quit();
	}
}
// 初始化摄像机
void SetupCamera(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double)SCREEN_WIDTH / (double)SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);
	gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);

	// 各种变换应该在GL_MODELVIEW模式下进行
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Z-buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// 启用2D贴图
	glEnable(GL_TEXTURE_2D);
}

// 渲染到纹理
void RenderToTarget(void)
{
	glBindTexture(GL_TEXTURE_2D, 0); // 取消绑定，因为如果不取消，渲染到纹理的时候会使用纹理本身

	// 绑定渲染目标
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);

	// 渲染
	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_POLYGON);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1, 0, 0, 1);
	glVertex3d(0, 1, 0);
	glVertex3d(-1, -1, 0);
	glVertex3d(1, -1, 0);

	glEnd();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
void display(void)
{
	RenderToTarget(); 
	// 绑定默认FBO（窗体帧缓冲区的ID是0）
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, rendertarget);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// 渲染
	glClearColor(0, 0, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_POLYGON);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1, 1, 1);

	glTexCoord2f(1, 1);
	glVertex3d(1, 1, 0);

	glTexCoord2f(0, 1);
	glVertex3d(-1, 1, 0);

	glTexCoord2f(0, 0);
	glVertex3d(-1, -1, 0);

	glTexCoord2f(1, 0);
	glVertex3d(1, -1, 0);

	glEnd();

	glutSwapBuffers();

}

void reshape(int w, int h)
{
	SetupCamera();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(250, 250);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	glewInit();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	// 设置创建虚拟屏幕
	SetupResource();

	glutMainLoop();
	return 0;
}


#endif
