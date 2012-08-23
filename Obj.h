/*
** Alias OBJ �`���f�[�^
*/
#ifndef OBJ_H
#define OBJ_H

typedef float vec[3];
typedef unsigned int idx[3];

class Obj {
  int nv, nf;                   // ���_�̐��C�ʂ̐�
  vec *vert, *norm, *fnorm;     // ���_�C���_�̖@���C�ʂ̖@��
  idx *face;                    // �ʃf�[�^�i���_�̃C���f�b�N�X�j
  void init(void);              // ������
  void copy(const Obj &);       // �������̃R�s�[
public:
  Obj(void);
  Obj(const char *name);
  Obj(const Obj &o);
  ~Obj(void);
  Obj &operator=(const Obj &o);
  bool load(const char *name);  // OBJ �t�@�C���̓ǂݍ���
  void draw(void);              // �}�`�̕`��
};

#endif
