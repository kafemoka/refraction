// aberratoion.vert

varying vec3 r;   // �����̔��˃x�N�g��
varying vec3 s_r; // �����̐ԕ����̋��܃x�N�g��
varying vec3 s_g; // �����̗Ε����̋��܃x�N�g��
varying vec3 s_b; // �����̐����̋��܃x�N�g��
varying float t;  // ���E�ʂł̔��˗�

const float eta_r = 0.69;  // �Ԃ̋��ܗ��̔�
const float eta_g = 0.67;  // �΂̋��ܗ��̔�
const float eta_b = 0.65;  // �̋��ܗ��̔�
const float f = (1.0 - eta_g) * (1.0 - eta_g) / ((1.0 + eta_g) * (1.0 + eta_g));

void main(void)
{
  vec4 p = gl_ModelViewMatrix * gl_Vertex;  // ���_�ʒu
  vec3 v = normalize(p.xyz / p.w);          // �����x�N�g��
  vec3 n = gl_NormalMatrix * gl_Normal;     // �@���x�N�g��
  r = vec3(gl_TextureMatrix[0] * vec4(reflect(v, n), 1.0));
  s_r = vec3(gl_TextureMatrix[0] * vec4(refract(v, n, eta_r), 1.0));
  s_g = vec3(gl_TextureMatrix[0] * vec4(refract(v, n, eta_g), 1.0));
  s_b = vec3(gl_TextureMatrix[0] * vec4(refract(v, n, eta_b), 1.0));
  t = f + (1.0 - f) * pow(1.0 - dot(-v, n), 5.0);
  gl_Position = ftransform();
}
