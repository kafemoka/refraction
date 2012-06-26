#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>

#if defined(WIN32)
#  include "glut.h"
#elif defined(__APPLE__) || defined(MACOSX)
#  include <GLUT/glut.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/glut.h>
#endif

#include "Obj.h"

/*
** �f�t�H���g�R���X�g���N�^
*/
Obj::Obj(void)
{
  init();
}

/*
** �R���X�g���N�^
*/
Obj::Obj(const char *name)
{
  init();
  load(name);
}

/*
** �R�s�[�R���X�g���N�^
*/
Obj::Obj(const Obj &o)
{
  copy(o);
}

/*
** �f�X�g���N�^
*/
Obj::~Obj()
{
  if (vert) delete[] vert;
  if (norm) delete[] norm;
  if (fnorm) delete[] fnorm;
  if (face) delete[] face;
}

/*
** ������Z�q
*/
Obj &Obj::operator=(const Obj &o)
{
  if (this != &o) {
    this->~Obj();
    copy(o);
  }
  return *this;
}

/*
** �I�u�W�F�N�g�̏�����
*/
void Obj::init(void)
{
  nv = nf = 0;
  vert = norm = fnorm = 0;
  face = 0;
}

/*
** �I�u�W�F�N�g�̃R�s�[
*/
void Obj::copy(const Obj &o)
{
  nv = o.nv;
  nf = o.nf;

  try {
    if (nv > 0) {
      vert = new vec[nv];
      norm = new vec[nv];

      memcpy(vert, o.vert, sizeof(vec) * nv);
      memcpy(norm, o.norm, sizeof(vec) * nv);
    }
    else {
      vert = norm = 0;
    }
    if (nf > 0) {
      fnorm = new vec[nf];
      face = new idx[nf];

      memcpy(fnorm, o.fnorm, sizeof(vec) * nf);
      memcpy(face, o.face, sizeof(idx) * nf);
    }
    else {
      fnorm = 0;
      face = 0;
    }
  }
  catch (std::bad_alloc e) {
    std::cerr << "������������܂���"<< std::endl;
    this->~Obj();
    init();
  }
}

/*
** �t�@�C���̓ǂݍ���
*/
bool Obj::load(const char *name)
{
  /* �������̊J�� */
  this->~Obj();

  /* �t�@�C���̓ǂݍ��� */
  std::ifstream file(name, std::ios::binary);
  if (!file) {
    std::cerr << name << " ���J���܂���" << std::endl;
    init();
    return false;
  }

  /* �f�[�^�̐��𒲂ׂ� */
  char buf[1024];
  int v, f;
  v = f = 0;
  while (file.getline(buf, sizeof buf)) {
    if (buf[0] == 'v' && buf[1] == ' ') {
      ++v;
    }
    else if (buf[0] == 'f' && buf[1] == ' ') {
      ++f;
    }
  }

  nv = v;
  nf = f;

  try {
    vert = new vec[v];
    norm = new vec[v];
    fnorm = new vec[f];
    face = new idx[f];
  }
  catch (std::bad_alloc e) {
    std::cerr << "������������܂���" << std::endl;
    this->~Obj();
    init();
    return false;
  }

  /* �t�@�C���̊����߂� */
  file.clear();
  file.seekg(0L, std::ios::beg);

  /* �f�[�^�̓ǂݍ��� */
  v = f = 0;
  while (file.getline(buf, sizeof buf)) {
    if (buf[0] == 'v' && buf[1] == ' ') {
      sscanf(buf, "%*s %f %f %f", vert[v], vert[v] + 1, vert[v] + 2);
      ++v;
    }
    else if (buf[0] == 'f' && buf[1] == ' ') {
      if (sscanf(buf + 2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", face[f], face[f] + 1, face[f] + 2) != 3) {
        if (sscanf(buf + 2, "%d//%*d %d//%*d %d//%*d", face[f], face[f] + 1, face[f] + 2) != 3) {
          sscanf(buf + 2, "%d %d %d", face[f], face[f] + 1, face[f] + 2);
        }
      }
      --face[f][0];
      --face[f][1];
      --face[f][2];
      ++f;
    }
  }

  /* �ʖ@���x�N�g���̎Z�o */
  int i;
  for (i = 0; i < f; ++i) {
    float dx1 = vert[face[i][1]][0] - vert[face[i][0]][0];
    float dy1 = vert[face[i][1]][1] - vert[face[i][0]][1];
    float dz1 = vert[face[i][1]][2] - vert[face[i][0]][2];
    float dx2 = vert[face[i][2]][0] - vert[face[i][0]][0];
    float dy2 = vert[face[i][2]][1] - vert[face[i][0]][1];
    float dz2 = vert[face[i][2]][2] - vert[face[i][0]][2];

    fnorm[i][0] = dy1 * dz2 - dz1 * dy2;
    fnorm[i][1] = dz1 * dx2 - dx1 * dz2;
    fnorm[i][2] = dx1 * dy2 - dy1 * dx2;
  }

  /* ���_�̉��z�@���x�N�g���̎Z�o */
  for (i = 0; i < v; ++i) {
    norm[i][0] = norm[i][1] = norm[i][2] = 0.0;
  }
  
  for (i = 0; i < f; ++i) {
    norm[face[i][0]][0] += fnorm[i][0];
    norm[face[i][0]][1] += fnorm[i][1];
    norm[face[i][0]][2] += fnorm[i][2];

    norm[face[i][1]][0] += fnorm[i][0];
    norm[face[i][1]][1] += fnorm[i][1];
    norm[face[i][1]][2] += fnorm[i][2];

    norm[face[i][2]][0] += fnorm[i][0];
    norm[face[i][2]][1] += fnorm[i][1];
    norm[face[i][2]][2] += fnorm[i][2];
  }

  /* ���_�̉��z�@���x�N�g���̐��K�� */
  for (i = 0; i < v; ++i) {
    float a = sqrt(norm[i][0] * norm[i][0]
                 + norm[i][1] * norm[i][1]
                 + norm[i][2] * norm[i][2]);

    if (a != 0.0) {
      norm[i][0] /= a;
      norm[i][1] /= a;
      norm[i][2] /= a;
    }
  }

  return true;
}

/*
** �}�`�̕\��
*/
void Obj::draw(void)
{
  /* ���_�f�[�^�C�@���f�[�^�C�e�N�X�`�����W�̔z���L���ɂ��� */
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  
  /* ���_�f�[�^�C�@���f�[�^�C�e�N�X�`�����W�̏ꏊ���w�肷�� */
  glNormalPointer(GL_FLOAT, 0, norm);
  glVertexPointer(3, GL_FLOAT, 0, vert);
  
  /* ���_�̃C���f�b�N�X�̏ꏊ���w�肵�Đ}�`��`�悷�� */
  glDrawElements(GL_TRIANGLES, nf * 3, GL_UNSIGNED_INT, face);

  /* ���_�f�[�^�C�@���f�[�^�C�e�N�X�`�����W�̔z��𖳌��ɂ��� */
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}
