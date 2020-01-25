#include <vector>

#include "vec3.h"

enum inc_type{
  isphere,iplane
};

//-----------------------------------------------------------------------------------------------//
int findRoots(const GLfloat & a, const GLfloat & b, const GLfloat & c, GLfloat * roots){
// a = D•D, b = 2 (O-C)•D, c = (O-C)•(O-C) - R^2 ::   D = dir vec, O = origin pt, R^2 = radius
  GLfloat discrim = b*b - 4*a*c;
  if(discrim == 0){
    roots[0] = roots[1] = -0.5 * b/a;
    return 0;
  }else if(discrim > 0){
    GLfloat sqDisc = (b > 0) ? sqrt(discrim) : -sqrt(discrim);
    roots[0] = -0.5 * (b + sqDisc) / a;
    roots[1] = c / (-0.5 * (b + sqDisc));
    return 1;
  }else {
    return -1;
  }
}

//-----------------------------------------------------------------------------------------------//
class Light{
public:
  vec3 pos;
  vec3 intensity;
  Light(vec3 ps, vec3 ins) : pos(ps), intensity(ins) {}
  ~Light() {}
};

//-----------------------------------------------------------------------------------------------//
class Ray{
public:
  vec3 origin;
  vec3 dir_vec;
  GLfloat t;

  Ray(vec3 org, vec3 dir) : origin(org), dir_vec(dir), t(FLT_MAX) {}
  ~Ray() {}
};

//-----------------------------------------------------------------------------------------------//
class Sphere{
public:
  vec3 center;
  GLfloat radius;
  //For shading
  vec3 kD; //diffuse coefficient
  GLfloat kS; //specular coefficient
  GLfloat q; //specular exponent
  GLfloat kR; //reflection coefficient
  GLfloat kT; //refraction coefficient
  GLfloat ior; //index of refraction

  Sphere(vec3 cen, GLfloat r, vec3 kDiff, GLfloat kSpec, GLfloat sq, GLfloat kr_c, GLfloat kt_c, GLfloat rIndex) : center(cen), radius(r), kD(kDiff), kS(kSpec), q(sq), kR(kr_c), kT(kt_c), ior(rIndex) {}
  ~Sphere() {}

  bool intersect(Ray &ry){
    GLfloat a = dot(ry.dir_vec, ry.dir_vec);
    vec3 omc = ry.origin - center;
    GLfloat b = 2 * dot(ry.dir_vec, omc);
    GLfloat c = dot(omc,omc) - radius*radius;
    GLfloat roots[2] {-1,-1};
    int rType = findRoots(a,b,c,roots);
    if(rType == -1) {
      return false;
    }
    else if(rType == 0){
      ry.t = roots[0];
    }else{
      GLfloat minR = (roots[0] < roots[1] && roots[0] > 0 || roots[1] < 0) ? roots[0] : roots[1];
      if(minR < 0){
        return false;
      }
      ry.t = minR;
    }
    return true;
  }
};

//-----------------------------------------------------------------------------------------------//
class Plane{
public:
  vec3 point;
  vec3 normal;
  vec3 kD;      //diffuse coefficient
  GLfloat kS;   //specular coefficient
  GLfloat q;    //specular exponent
  GLfloat kR;   //reflection coefficient
  GLfloat kT;   //refraction coefficient
  GLfloat ior;  //index of refraction

  Plane(vec3 pt, vec3 n, vec3 kDiff, GLfloat kSpec, GLfloat pq, GLfloat kr_c, GLfloat kt_c, GLfloat rIndex) : point(pt), normal(n), kD(kDiff), kS(kSpec), q(pq), kR(kr_c), kT(kt_c), ior(rIndex) {}

  Plane(vec3 pt, vec3 n, vec3 kDiff) : point(pt), normal(n), kD(kDiff) {}
  ~Plane() {}

  GLfloat intersect(Ray & ry, GLfloat bias)
  {
    GLfloat cos_nd;
    if((cos_nd = dot(normal,ry.dir_vec)) > bias*0.01) {
        vec3 testRay = point - ry.origin;
        return dot(testRay, normal) / cos_nd;
    }
    return FLT_MAX;
  }
};
