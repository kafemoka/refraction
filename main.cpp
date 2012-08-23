#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#if defined(WIN32)
//#  pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#  include "glut.h"
#  include "glext.h"
PFNGLLOADTRANSPOSEMATRIXDPROC glLoadTransposeMatrixd;
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

/*
** �V�F�[�_�I�u�W�F�N�g
*/
#include "glsl.h"
static GLuint shader1, shader2;

/*
**�g���b�N�{�[������
*/
#include "Trackball.h"
static Trackball *tb1, *tb2;
static int btn = -1;

/*
** ��
*/
#include "Box.h"
static Box *box;

/*
** OBJ �t�@�C��
*/
#include "Obj.h"
static Obj *obj;

/*
** �e�N�X�`��
*/
#define TEXWIDTH  256                           /* �e�N�X�`���̕��@�@�@ */
#define TEXHEIGHT 256                           /* �e�N�X�`���̍����@�@ */
static GLuint texname[2];                       /* �e�N�X�`�����i�ԍ��j */
static const char *texfile[] = {                /* �e�N�X�`���t�@�C���� */
  "room3ny.raw", /* �� */
  "room3nz.raw", /* �� */
  "room3px.raw", /* �E */
  "room3pz.raw", /* �O */
  "room3nx.raw", /* �� */
  "room3py.raw", /* �� */
};
static const int target[] = {                /* �e�N�X�`���̃^�[�Q�b�g�� */
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
};

/*
** �V�F�[�_�v���O�����̍쐬
*/
static GLuint loadShader(const char *vert, const char *frag)
{
  /* �V�F�[�_�I�u�W�F�N�g�̍쐬 */
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  
  /* �V�F�[�_�̃\�[�X�v���O�����̓ǂݍ��� */
  if (readShaderSource(vertShader, vert)) exit(1);
  if (readShaderSource(fragShader, frag)) exit(1);
  
  /* �V�F�[�_�v���O�����̃R���p�C���^�����N���ʂ𓾂�ϐ� */
  GLint compiled, linked;

  /* �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� */
  glCompileShader(vertShader);
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(vertShader);
  if (compiled == GL_FALSE) {
    std::cerr << "Compile error in vertex shader." << std::endl;
    exit(1);
  }
  
  /* �t���O�����g�V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� */
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
  printShaderInfoLog(fragShader);
  if (compiled == GL_FALSE) {
    std::cerr << "Compile error in fragment shader." << std::endl;
    exit(1);
  }
  
  /* �v���O�����I�u�W�F�N�g�̍쐬 */
  GLuint gl2Program = glCreateProgram();
  
  /* �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ */
  glAttachShader(gl2Program, vertShader);
  glAttachShader(gl2Program, fragShader);
  
  /* �V�F�[�_�I�u�W�F�N�g�̍폜 */
  glDeleteShader(vertShader);
  glDeleteShader(fragShader);
  
  /* �V�F�[�_�v���O�����̃����N */
  glLinkProgram(gl2Program);
  glGetProgramiv(gl2Program, GL_LINK_STATUS, &linked);
  printProgramInfoLog(gl2Program);
  if (linked == GL_FALSE) {
    std::cerr << "Link error" << std::endl;
    exit(1);
  }

  return gl2Program;
}

/*
** ������
*/
static void init(void)
{
  /* ���̃I�u�W�F�N�g�𐶐� */
  box = new Box(500.0f, 500.0f, 500.0f);

  /* OBJ �t�@�C���̓ǂݍ��� */
  obj = new Obj("bunny.obj");

  /* �g���b�N�{�[�������p�I�u�W�F�N�g�̐��� */
  tb1 = new Trackball;
  tb2 = new Trackball;

  /* �e�N�X�`�������Q���� */
  glGenTextures(2, texname);
  
  /* �e�N�X�`���摜�̓��[�h�P�ʂɋl�ߍ��܂�Ă��� */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  /* �O���̗����̂̃e�N�X�`���̊��蓖�āi�W�����j */
  glBindTexture(GL_TEXTURE_2D, texname[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXWIDTH * 8, TEXHEIGHT, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, 0);
  
  /* �e�N�X�`���摜�̓ǂݍ��� */
  for (int i = 0; i < 6; ++i) {
    std::ifstream file;

    file.open(texfile[i], std::ios::binary);
    if (file) {
      GLubyte image[TEXHEIGHT * TEXWIDTH * 4]; // �e�N�X�`���摜�̓ǂݍ��ݗp

      file.read(reinterpret_cast<char *>(image), sizeof image);
      file.close();

      /* �O���̗����̂̃e�N�X�`���̒u������ */
      glTexSubImage2D(GL_TEXTURE_2D, 0, TEXWIDTH * i, 0, TEXWIDTH, TEXHEIGHT,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

      /* �L���[�u�}�b�s���O�̃e�N�X�`���̊��蓖�� */
      glBindTexture(GL_TEXTURE_CUBE_MAP, texname[1]);
      glTexImage2D(target[i], 0, GL_RGBA, TEXWIDTH, TEXHEIGHT, 0, 
        GL_RGBA, GL_UNSIGNED_BYTE, image);

      /* �ݒ�Ώۂ��O���̗����̂̃e�N�X�`���ɖ߂� */
      glBindTexture(GL_TEXTURE_2D, texname[0]);
    }
  }
  
  /* �e�N�X�`�����g��E�k��������@�̎w�� */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  /* �e�N�X�`���̌J��Ԃ����@�̎w�� */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /* �ݒ�Ώۂ��L���[�u�}�b�s���O�̃e�N�X�`���ɐ؂�ւ��� */
  glBindTexture(GL_TEXTURE_CUBE_MAP, texname[1]);

  /* �e�N�X�`�����g��E�k��������@�̎w�� */
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  
  /* �e�N�X�`���̌J��Ԃ����@�̎w�� */
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  /* �ݒ�Ώۂ𖳖��e�N�X�`���ɖ߂� */
  glBindTexture(GL_TEXTURE_2D, 0);
  
  /* GLSL �̏����� */
  if (glslInit()) exit(1);

  /* �V�F�[�_�v���O�����̍쐬 */
  shader1 = loadShader("replace.vert", "replace.frag");
  shader2 = loadShader("refract.vert", "refract.frag");
  
  /* �e�N�X�`�����j�b�g�O���w�肷�� */
  glUniform1i(glGetUniformLocation(shader1, "texture"), 0);
  glUniform1i(glGetUniformLocation(shader2, "cubemap"), 0);

  /* �����ݒ� */
  glClearColor(0.3, 0.3, 1.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  
#if defined(WIN32)
  glLoadTransposeMatrixd =
    (PFNGLLOADTRANSPOSEMATRIXDPROC)wglGetProcAddress("glLoadTransposeMatrixd");
#endif
}

/*
** �V�[���̕`��
*/
static void scene(void)
{
  /* �ݒ�Ώۂ��O���̗����̂̃e�N�X�`���ɐ؂�ւ��� */
  glBindTexture(GL_TEXTURE_2D, texname[0]);
  
  /* ���̃e�N�X�`���̃V�F�[�_�v���O������K�p���� */
  glUseProgram(shader1);

  /* ����`�� */
  glPushMatrix();
  glMultMatrixd(tb2->rotation());
  box->draw();
  glPopMatrix();

  /* �ݒ�Ώۂ��L���[�u�}�b�s���O�̃e�N�X�`���ɐ؂�ւ���*/
  glBindTexture(GL_TEXTURE_CUBE_MAP, texname[1]);

  /* �L���[�u�}�b�s���O�̃V�F�[�_�v���O������K�p���� */
  glUseProgram(shader2);

  /* �e�N�X�`���ϊ��s��Ƀg���b�N�{�[�����̉�]�������� */
  glMatrixMode(GL_TEXTURE);
  glLoadTransposeMatrixd(tb2->rotation());
  glMatrixMode(GL_MODELVIEW);

  /* ���_��菭�����ɃI�u�W�F�N�g��`���ăg���b�N�{�[�����̉�]�������� */
  glPushMatrix();
  glTranslated(0.0, 0.0, -200.0);
  glMultMatrixd(tb1->rotation());
  obj->draw();
  glPopMatrix();
  
  /* �e�N�X�`���ϊ��s������ɖ߂� */
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);

  /* �ݒ�Ώۂ𖳖��e�N�X�`���ɖ߂� */
  glBindTexture(GL_TEXTURE_2D, 0);
}


/****************************
** GLUT �̃R�[���o�b�N�֐� **
****************************/

static void display(void)
{
  /* ��ʃN���A */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  /* ���f���r���[�ϊ��s��̐ݒ� */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /* �V�[���̕`�� */
  scene();
  
  /* �_�u���o�b�t�@�����O */
  glutSwapBuffers();
}

static void resize(int w, int h)
{
  /* �g���b�N�{�[������͈� */
  tb1->region(w, h);
  tb2->region(w, h);
  
  /* �E�B���h�E�S�̂��r���[�|�[�g�ɂ��� */
  glViewport(0, 0, w, h);
  
  /* �����ϊ��s��̎w�� */
  glMatrixMode(GL_PROJECTION);
  
  /* �����ϊ��s��̏����� */
  glLoadIdentity();
  gluPerspective(60.0, (double)w / (double)h, 100.0, 500.0);
}

static void idle(void)
{
  /* ��ʂ̕`���ւ� */
  glutPostRedisplay();
}

static void mouse(int button, int state, int x, int y)
{
  btn = button;

  switch (btn) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      /* �g���b�N�{�[���J�n */
      tb1->start(x, y);
      glutIdleFunc(idle);
    }
    else {
      /* �g���b�N�{�[����~ */
      tb1->stop(x, y);
      glutIdleFunc(0);
    }
    break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN) {
      /* �g���b�N�{�[���J�n */
      tb2->start(x, y);
      glutIdleFunc(idle);
    }
    else {
      /* �g���b�N�{�[����~ */
      tb2->stop(x, y);
      glutIdleFunc(0);
    }
    break;
  default:
    break;
  }
}

static void motion(int x, int y)
{
  switch (btn) {
  case GLUT_LEFT_BUTTON:
    /* �g���b�N�{�[���ړ� */
    tb1->motion(x, y);
    break;
  case GLUT_RIGHT_BUTTON:
    /* �g���b�N�{�[���ړ� */
    tb2->motion(x, y);
    break;
  default:
    break;
  }
}

static void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
    /* ESC �� q �� Q ���^�C�v������I�� */
    exit(0);
  default:
    break;
  }
}

/*
** ���C���v���O����
*/
int main(int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  init();
  glutMainLoop();
  return 0;
}
