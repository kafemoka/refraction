/*
** ��
*/
#ifndef BOX_H
#define BOX_H

class Box {
  float vertex[6][4][3];
public:
  Box(float x = 1.0f, float y = 1.0f, float z = 1.0f);
  ~Box(void) {};
  void size(float x, float y, float z); // ���̃T�C�Y�ݒ�
  void draw(void);                      // ���̕`��
};

#endif
