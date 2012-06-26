// aberration.frag

uniform samplerCube cubemap;

varying vec3 r;   // �����̔��˃x�N�g��
varying vec3 s_r; // �����̐ԕ����̋��܃x�N�g��
varying vec3 s_g; // �����̗Ε����̋��܃x�N�g��
varying vec3 s_b; // �����̐����̋��܃x�N�g��
varying float t;  // ���E�ʂł̔��˗�

void main(void)
{
  vec4 c;
  
  c.r = textureCube(cubemap, s_r).r;
  c.g = textureCube(cubemap, s_g).g;
  c.b = textureCube(cubemap, s_b).b;
  c.a = 1.0;
  
  gl_FragColor = mix(c, textureCube(cubemap, r), t);
}
