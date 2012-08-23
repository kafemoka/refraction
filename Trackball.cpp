/*
** 簡易トラックボール処理
*/
#include <cmath>
#include "Trackball.h"

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

#define SCALE (2.0 * M_PI) // マウスの相対位置→回転角の換算係数

/*
** r <- p x q
*/
static void qmul(double r[], const double p[], const double q[])
{
  r[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
  r[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
  r[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
  r[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}

/*
** 回転変換行列 r <- クォータニオン q
*/
static void qrot(double r[], double q[])
{
  double x2 = q[1] * q[1] * 2.0;
  double y2 = q[2] * q[2] * 2.0;
  double z2 = q[3] * q[3] * 2.0;
  double xy = q[1] * q[2] * 2.0;
  double yz = q[2] * q[3] * 2.0;
  double zx = q[3] * q[1] * 2.0;
  double xw = q[1] * q[0] * 2.0;
  double yw = q[2] * q[0] * 2.0;
  double zw = q[3] * q[0] * 2.0;
  
  r[ 0] = 1.0 - y2 - z2;
  r[ 1] = xy + zw;
  r[ 2] = zx - yw;
  r[ 4] = xy - zw;
  r[ 5] = 1.0 - z2 - x2;
  r[ 6] = yz + xw;
  r[ 8] = zx + yw;
  r[ 9] = yz - xw;
  r[10] = 1.0 - x2 - y2;
  r[ 3] = r[ 7] = r[11] = r[12] = r[13] = r[14] = 0.0;
  r[15] = 1.0;
}

/*
** トラックボール処理の初期化
** 　　プログラムの初期化処理のところで実行する
*/
Trackball::Trackball(void)
{
  /* ドラッグ中ではない */
  drag = false;

  /* 単位クォーターニオン */
  cq[0] = 1.0;
  cq[1] = 0.0;
  cq[2] = 0.0;
  cq[3] = 0.0;
  
  /* 回転行列の初期化 */
  rt[ 0] = 1.0; rt[ 1] = 0.0; rt[ 2] = 0.0; rt[ 3] = 0.0;
  rt[ 4] = 0.0; rt[ 5] = 1.0; rt[ 6] = 0.0; rt[ 7] = 0.0;
  rt[ 8] = 0.0; rt[ 9] = 0.0; rt[10] = 1.0; rt[11] = 0.0;
  rt[12] = 0.0; rt[13] = 0.0; rt[14] = 0.0; rt[15] = 1.0;
}

/*
** トラックボールする領域
** 　　Reshape コールバック (resize) の中で実行する
*/
void Trackball::region(int w, int h)
{
  /* マウスポインタ位置のウィンドウ内の相対的位置への換算用 */
  sx = 1.0 / (double)w;
  sy = 1.0 / (double)h;
}

/*
** ドラッグ開始
** 　　マウスボタンを押したときに実行する
*/
void Trackball::start(int x, int y)
{
  /* ドラッグ開始 */
  drag = true;

  /* ドラッグ開始点を記録 */
  cx = x;
  cy = y;
}

/*
** ドラッグ中
** 　　マウスのドラッグ中に実行する
*/
void Trackball::motion(int x, int y)
{
  if (drag) {
    double dx, dy, a;
    
    /* マウスポインタの位置のドラッグ開始位置からの変位 */
    dx = (x - cx) * sx;
    dy = (y - cy) * sy;
    
    /* マウスポインタの位置のドラッグ開始位置からの距離 */
    a = sqrt(dx * dx + dy * dy);
    
    if (a != 0.0) {
      double ar = a * SCALE * 0.5;
      double as = sin(ar) / a;
      double dq[4] = { cos(ar), dy * as, dx * as, 0.0 };
      
      /* クォータニオンを掛けて回転を合成 */
      qmul(tq, dq, cq);
      
      /* クォータニオンから回転の変換行列を求める */
      qrot(rt, tq);
    }
  }
}

/*
** 停止
** 　　マウスボタンを離したときに実行する
*/
void Trackball::stop(int x, int y)
{
  /* ドラッグ終了点における回転を求める */
  motion(x, y);

  /* 回転の保存 */
  cq[0] = tq[0];
  cq[1] = tq[1];
  cq[2] = tq[2];
  cq[3] = tq[3];

  /* ドラッグ終了 */
  drag = false;
}

/*
** 回転の変換行列を戻す
** 　　戻り値を glMultMatrixd() などで使用してオブジェクトを回転する
*/
const double *Trackball::rotation(void)
{
  return rt;
}
