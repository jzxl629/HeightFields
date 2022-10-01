/*
  CSCI 420 Computer Graphics
  Assignment 1: Height Fields
  Zhouqian Lu
*/

#include <stdlib.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <pic.h>

int g_iMenuId;

int g_vMousePos[2] = {0, 0};
int g_iLeftMouseButton = 0; /* 1 if pressed, 0 if not */
int g_iMiddleMouseButton = 0;
int g_iRightMouseButton = 0;

typedef enum
{
  ROTATE,
  TRANSLATE,
  SCALE
} CONTROLSTATE;

CONTROLSTATE g_ControlState = ROTATE;

/* state of the world */
float g_vLandRotate[3] = {0.0, 0.0, 0.0};
float g_vLandTranslate[3] = {0.0, 0.0, 0.0};
float g_vLandScale[3] = {1.0, 1.0, 1.0};

/* see <your pic directory>/pic.h for type Pic */
Pic *g_pHeightData;
// arbitrary image
Pic *ag_pHeightData;

bool hasTexturemapImage = false;
bool displayTexture = false;

/* Write a screenshot to the specified filename */
void saveScreenshot(char *filename)
{
  int i, j;
  Pic *in = NULL;

  if (filename == NULL)
    return;

  /* Allocate a picture buffer */
  in = pic_alloc(640, 480, 3, NULL);

  printf("File to save to: %s\n", filename);

  for (i = 479; i >= 0; i--)
  {
    glReadPixels(0, 479 - i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 &in->pix[i * in->nx * in->bpp]);
  }

  if (jpeg_write(filename, in))
    printf("File saved Successfully\n");
  else
    printf("Error in Saving\n");

  pic_free(in);
}

void myinit()
{
  /* setup gl view here */
  glClearColor(0.0, 0.0, 0.0, 0.0);

  // enable depth buffering
  glEnable(GL_DEPTH_TEST);

  glShadeModel(GL_SMOOTH);
}

void display()
{
  /* draw 1x1 cube about origin */
  /* replace this code with your height field implementation */
  /* you may also want to precede it with your
rotation/translation/scaling */

  /* clear buffers */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);

  /* reset transformation */
  glLoadIdentity();

  // sets the camera position
  gluLookAt(0.0, 0.0, 6.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);

  // sets translation, rotation, and scale
  glTranslatef(g_vLandTranslate[0], g_vLandTranslate[1], g_vLandTranslate[2]);
  glRotatef(g_vLandRotate[0], 1, 0, 0);
  glRotatef(g_vLandRotate[1], 0, 1, 0);
  glRotatef(g_vLandRotate[2], 0, 0, 1);
  glScalef(g_vLandScale[0], g_vLandScale[1], g_vLandScale[2]);

  /*

  glBegin(GL_POLYGON);

  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(-0.5, -0.5, 0.0);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(-0.5, 0.5, 0.0);
  glColor3f(0.0, 0.0, 0.0);
  glVertex3f(0.5, 0.5, 0.0);
  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(0.5, -0.5, 0.0);

  glEnd();
  */

  int width = int(g_pHeightData->nx);
  int height = int(g_pHeightData->ny);
  int bpp = int(g_pHeightData->bpp);
  for (int i = 0; i < height - 1; i++)
  {
    glBegin(GL_TRIANGLE_STRIP);
    for (int j = 0; j < width; j++)
    {
      // if the input JPEG file has 1 bpp
      if (bpp == 1)
      {
        float indx0 = PIC_PIXEL(g_pHeightData, j, i, 0);
        float indx1 = PIC_PIXEL(g_pHeightData, j, i + 1, 0);

        indx0 = GLfloat(indx0);
        indx1 = GLfloat(indx1);

        float z0 = indx0 / 255;
        float z1 = indx1 / 255;

        glColor3f(z0, z0, z0);
        glVertex3f((j - 0.5 * width) / width * 5, (i - 0.5 * height) / height * 5, -z0 * 2);

        glColor3f(z1, z1, z1);
        glVertex3f((j - 0.5 * width) / width * 5, (i + 1 - 0.5 * height) / height * 5, -z1 * 2);
      }
      // if the input JPEG file has 3bpp
      else if (bpp == 3)
      {
        float rIndx0 = PIC_PIXEL(g_pHeightData, j, i, 0);
        float gIndx0 = PIC_PIXEL(g_pHeightData, j, i, 1);
        float bIndx0 = PIC_PIXEL(g_pHeightData, j, i, 2);

        float rIndx1 = PIC_PIXEL(g_pHeightData, j, i + 1, 0);
        float gIndx1 = PIC_PIXEL(g_pHeightData, j, i + 1, 1);
        float bIndx1 = PIC_PIXEL(g_pHeightData, j, i + 1, 2);

        // scale the sum of three colors by 3 * 255 * 10
        float z0 = (rIndx0 + gIndx0 + bIndx0) / 7650;
        float z1 = (rIndx1 + gIndx1 + bIndx1) / 7650;

        // If rendering color using another image of equal size
        if (displayTexture)
        {
          float rIndx0ag = PIC_PIXEL(ag_pHeightData, j, i, 0);
          float gIndx0ag = PIC_PIXEL(ag_pHeightData, j, i, 1);
          float bIndx0ag = PIC_PIXEL(ag_pHeightData, j, i, 2);

          float rIndx1ag = PIC_PIXEL(ag_pHeightData, j, i + 1, 0);
          float gIndx1ag = PIC_PIXEL(ag_pHeightData, j, i + 1, 1);
          float bIndx1ag = PIC_PIXEL(ag_pHeightData, j, i + 1, 2);

          // set the color using pixels values of the arbitrary image
          glColor3f(rIndx0ag / 255, gIndx0ag / 255, bIndx0ag / 255);
          glVertex3f((j - 0.5 * width) / width * 5, (i - 0.5 * height) / height * 5, -z0 * 2);

          glColor3f(rIndx1ag / 255, gIndx1ag / 255, bIndx1ag / 255);
          glVertex3f((j - 0.5 * width) / width * 5, (i + 1 - 0.5 * height) / height * 5, -z1 * 2);
        }
        else
        {

          glColor3f(rIndx0 / 255, gIndx0 / 255, bIndx0 / 255);
          glVertex3f((j - 0.5 * width) / width * 5, (i - 0.5 * height) / height * 5, -z0 * 2);

          glColor3f(rIndx1 / 255, gIndx1 / 255, bIndx1 / 255);
          glVertex3f((j - 0.5 * width) / width * 5, (i + 1 - 0.5 * height) / height * 5, -z1 * 2);
        }
      }
    }
    glEnd();
  }
  glutSwapBuffers();
}

void reshape(int w, int h)
{
  GLfloat aspect = (GLfloat)w / (GLfloat)h;
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, aspect, 0.001, 1000.0);
  glMatrixMode(GL_MODELVIEW);
}

void menufunc(int value)
{
  switch (value)
  {
  case 0:
    exit(0);
    break;
  }
}

void doIdle()
{
  /* do some stuff... */

  /* make the screen update */
  glutPostRedisplay();
}

/* converts mouse drags into information about
rotation/translation/scaling */
void mousedrag(int x, int y)
{
  int vMouseDelta[2] = {x - g_vMousePos[0], y - g_vMousePos[1]};

  switch (g_ControlState)
  {
  case TRANSLATE:
    if (g_iLeftMouseButton)
    {
      g_vLandTranslate[0] += vMouseDelta[0] * 0.01;
      g_vLandTranslate[1] -= vMouseDelta[1] * 0.01;
    }
    if (g_iMiddleMouseButton)
    {
      g_vLandTranslate[2] += vMouseDelta[1] * 0.01;
    }
    break;
  case ROTATE:
    if (g_iLeftMouseButton)
    {
      g_vLandRotate[0] += vMouseDelta[1];
      g_vLandRotate[1] += vMouseDelta[0];
    }
    if (g_iMiddleMouseButton)
    {
      g_vLandRotate[2] += vMouseDelta[1];
    }
    break;
  case SCALE:
    if (g_iLeftMouseButton)
    {
      g_vLandScale[0] *= 1.0 + vMouseDelta[0] * 0.01;
      g_vLandScale[1] *= 1.0 - vMouseDelta[1] * 0.01;
    }
    if (g_iMiddleMouseButton)
    {
      g_vLandScale[2] *= 1.0 - vMouseDelta[1] * 0.01;
    }
    break;
  }
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mouseidle(int x, int y)
{
  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

void mousebutton(int button, int state, int x, int y)
{

  switch (button)
  {
  case GLUT_LEFT_BUTTON:
    g_iLeftMouseButton = (state == GLUT_DOWN);
    break;
  case GLUT_MIDDLE_BUTTON:
    g_iMiddleMouseButton = (state == GLUT_DOWN);
    break;
  case GLUT_RIGHT_BUTTON:
    g_iRightMouseButton = (state == GLUT_DOWN);
    break;
  }

  switch (glutGetModifiers())
  {
  case GLUT_ACTIVE_CTRL:
    g_ControlState = TRANSLATE;
    break;
  case GLUT_ACTIVE_SHIFT:
    g_ControlState = SCALE;
    break;
  default:
    g_ControlState = ROTATE;
    break;
  }

  g_vMousePos[0] = x;
  g_vMousePos[1] = y;
}

// Function to bind keys to different functions
void keyboard(unsigned char key, int x, int y)
{
  if (key == '1')
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
  if (key == '2')
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  if (key == '3')
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // rendering using color from the other image
  if (key == '4')
  {
    if (hasTexturemapImage == true)
    {
      displayTexture = true;
      glClearColor(0.0, 0.0, 0.0, 0.0);
      // reruns the display function
      glutPostRedisplay();
    }
  }

  /*
  if (key == '4')
    wireframeSolid();
  */
  // binds "t" key to translation
  if (key == 't')
    g_ControlState = TRANSLATE;
  // binds "q" key to rotation
  if (key == 'q')
    g_ControlState = ROTATE;
}

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    printf("usage: %s heightfield.jpg\n", argv[0]);
    exit(1);
  }

  g_pHeightData = jpeg_read(argv[1], NULL);

  // if users specifies another image
  if (argc == 3)
  {
    // read data from the other image
    ag_pHeightData = jpeg_read(argv[2], NULL);
    hasTexturemapImage = true;
  }
  if (!g_pHeightData)
  {
    printf("error reading %s.\n", argv[1]);
    exit(1);
  }

  glutInit(&argc, argv);

  /*
    create a window here..should be double buffered and use depth testing

    the code past here will segfault if you don't have a window set up....
    replace the exit once you add those calls.
  */

  // request double buffer
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

  // set window size
  glutInitWindowSize(640, 480);

  // set window position
  glutInitWindowPosition(0, 0);

  // creates a window
  glutCreateWindow("yooooooo");

  /* do initialization */
  myinit();

  glutReshapeFunc(reshape);

  /* tells glut to use a particular display function to redraw */
  glutDisplayFunc(display);

  /* allow the user to quit using the right mouse button menu */
  g_iMenuId = glutCreateMenu(menufunc);
  glutSetMenu(g_iMenuId);
  glutAddMenuEntry("Quit", 0);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  /* replace with any animate code */
  glutIdleFunc(doIdle);

  /* callback for mouse drags */
  glutMotionFunc(mousedrag);
  /* callback for idle mouse movement */
  glutPassiveMotionFunc(mouseidle);
  /* callback for mouse button changes */
  glutMouseFunc(mousebutton);
  glutKeyboardFunc(keyboard);

  glutMainLoop();
  return (0);
}
