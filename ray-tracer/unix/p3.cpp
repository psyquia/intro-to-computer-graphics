#include <iostream>
#include <vector>
#include <memory>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <float.h>

#include "obj.h"

using namespace std;

///////////////////////////////////////////
//CHOOSE SCENE WITH ARGUMENTS: 1, 2, 3, 4
///////////////////////////////////////////

//SETTINGS
//#define ds_x 1280              //screen resolution
//#define ds_y 720

#define ds_x 400               //screen resolution
#define ds_y 400

#define fov 45                  //vertical field of view
#define r_depth 5               //max recursion depth
#define bias 0.001              //bias to avoid self intersections
vec3 bgColor(0,0,0);            //background color
vec3 ambient(0.05,0.05,0.05);   //ambient color

//Globals
vector <Sphere> spheres;
Plane * plane;
vector <Light> lights;
GLfloat fovScale, aspectRatio;


//-----------------------------------------------------------------------------------------------//
Sphere * traceSpheres(Ray & ray){
  GLfloat tNear = FLT_MAX;
  Sphere * inc_sph = nullptr;
  for(int i = 0; i < spheres.size(); ++i){
    if(spheres[i].intersect(ray) && ray.t < tNear){
      inc_sph = &spheres[i];
      tNear = ray.t;
    }
  }
  if(tNear < FLT_MAX){
    ray.t = tNear;
  }
  return inc_sph;
}
//-----------------------------------------------------------------------------------------------//
vec3 getRefractVec(vec3 dir_vec, vec3 normal, float index)
{
  GLfloat n, c2;
  GLfloat c1 = dot(dir_vec, normal);

  if(c1 < 0){
    n = 1/index;
    c1 = -c1;
  }else{
    n = index/1;
    normal= -normal;
  }
  if((c2=1-(n*n*(1-c1*c1))) >= 0){
    return dir_vec * n + normal * (n * c1 - sqrt(c2));
  }
  return vec3(0,0,0);
}

//-----------------------------------------------------------------------------------------------//
GLfloat getFresnelReflection(vec3 normal, vec3 dir_vec, const GLfloat ior)
{
  GLfloat n1 = ior, n2 = 1;
  GLfloat cos1 = dot(dir_vec,normal);
  if(cos1 <= 0) swap(n1, n2);
  cos1 = abs(cos1);
  GLfloat sin_refr = n1 / n2 * sqrt(max(1 - cos1 * cos1, 0.0f));
  if(sin_refr < 1){
    GLfloat cos2 = sqrt(1 - sin_refr * sin_refr);
    GLfloat frParSqrd = ((n2*cos1) - (n1*cos2)) / ((n2*cos1) + (n1*cos2)) * ((n2*cos1) - (n1*cos2)) / ((n2*cos1) + (n1*cos2));
    GLfloat frPerSqrd = ((n1*cos1) - (n2*cos2)) / ((n1*cos1) + (n2*cos2)) * ((n1*cos1) - (n2*cos2)) / ((n1*cos1) + (n2*cos2));
    return 0.5 * (frParSqrd + frPerSqrd);
  }
  return 1;
}

//-----------------------------------------------------------------------------------------------//
vec3 castRay(Ray & ray, int depth){
  inc_type inc_t = isphere;

  if(depth<1) return bgColor;

  Sphere * inc_sph = traceSpheres(ray);
  if(plane){
    GLfloat planeT = plane->intersect(ray,bias);
    if (planeT < ray.t) {                           //check if plane in front of sphere
      inc_t = iplane;
      ray.t = planeT;
    }
  }
  if(inc_sph && inc_t == isphere){
    vec3 inc_pt = ray.origin + ray.dir_vec * ray.t;
    vec3 light_vec, light_I, refl_dir, diff_col=vec3(0,0,0), spec_col=vec3(0,0,0);
    vec3 inc_normal = (inc_pt - inc_sph->center).normalize();

    vec3 rrColor(0,0,0);

    if(inc_sph->kT > 0 || inc_sph->kR > 0){

      GLfloat fR = getFresnelReflection(ray.dir_vec, inc_normal, inc_sph->ior);

      //reflection/refraction
      vec3 refr_dir = (getRefractVec(ray.dir_vec, inc_normal, inc_sph->ior)).normalize();
      vec3 offset = (dot(ray.dir_vec, inc_normal) < 0) ? -inc_normal * bias : inc_normal * bias;
      Ray refrRay(inc_pt+offset,refr_dir);
      vec3 refr_col = castRay(refrRay, depth - 1) * inc_sph->kT;

      refl_dir = ray.dir_vec - (inc_normal * 2 * dot(ray.dir_vec,inc_normal) );
      Ray reflRay(inc_pt-offset,refl_dir);
      vec3 refl_col = castRay(reflRay, depth - 1) * inc_sph->kR;

      rrColor = refr_col  * (1-fR) + refl_col * (fR);
    }

    for(int i = 0; i < lights.size(); ++i){
      light_vec = (inc_pt - lights[i].pos).normalize();
      light_I = lights[i].intensity;
      Ray sRay(inc_pt + inc_normal * bias,-light_vec);
      Sphere * shad_sph = traceSpheres(sRay);
      if(!shad_sph){
        diff_col = diff_col + light_I *  dot(inc_normal,-light_vec);
        refl_dir = light_vec - (inc_normal * 2 * dot(light_vec,inc_normal) );
        spec_col = (spec_col + light_I * pow(max(0.f,dot(refl_dir,-ray.dir_vec)),inc_sph->q));
      }
    }
    return ambient * inc_sph-> kD + rrColor + diff_col * inc_sph->kD +spec_col * inc_sph->kS;
  }
  else if(inc_t == iplane){
    vec3 inc_pt = ray.origin + ray.dir_vec * ray.t;
    vec3 light_vec, light_I, refl_dir, diff_col=vec3(0,0,0), spec_col=vec3(0,0,0);
    vec3 inc_normal = -plane->normal;

    vec3 rrColor(0,0,0);

    if(plane->kT > 0 || plane->kR > 0){

      GLfloat fR = getFresnelReflection(ray.dir_vec, inc_normal, plane->ior);

      //reflection/refraction
      vec3 refr_dir = (getRefractVec(ray.dir_vec, inc_normal, plane->ior)).normalize();
      vec3 offset = (dot(ray.dir_vec, inc_normal) < 0) ? -inc_normal * bias : inc_normal * bias;
      Ray refrRay(inc_pt+offset,refr_dir);
      vec3 refr_col = castRay(refrRay, depth - 1);

      refl_dir = ray.dir_vec - (inc_normal * 2 * dot(ray.dir_vec,inc_normal) );
      Ray reflRay(inc_pt-offset,refl_dir);
      vec3 refl_col = castRay(reflRay, depth - 1);

      rrColor = refr_col * plane->kT * (1-fR) + refl_col *plane->kR* (fR);
    }


    for(int i = 0; i < lights.size(); ++i){
      light_vec = (inc_pt - lights[i].pos).normalize();
      light_I = lights[i].intensity;
      Ray sRay(inc_pt + inc_normal * bias,-light_vec);
      Sphere * shad_sph = traceSpheres(sRay);
      if(!shad_sph){
        diff_col = diff_col + light_I *  dot(inc_normal,-light_vec);
      }
    }
    return ambient * plane-> kD + rrColor + diff_col * plane->kD;
  }

  return bgColor;
}

//-----------------------------------------------------------------------------------------------//
void render(){
  for(int y = 0; y < ds_y; y++){
    for(int x = 0; x < ds_x; x++){
      GLfloat w_x = fovScale * aspectRatio * (2 * (x + 0.5)/(GLfloat)ds_x - 1);
      GLfloat w_y = fovScale * (1 - 2 * (y + 0.5)/(GLfloat)ds_y);
      vec3 dir_vec = vec3(w_x,w_y,-1);
      Ray primRay = Ray(vec3(0,0,0),dir_vec);
      vec3 pixCol = castRay(primRay, r_depth);
      glBegin(GL_POINTS);
         glColor3f(pixCol.x,pixCol.y,pixCol.z);
         glVertex2i(x,y);
      glEnd();
    }
  }
}

//-----------------------------------------------------------------------------------------------//
void displayF(){
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);
  render();

  glutSwapBuffers();
}

//-----------------------------------------------------------------------------------------------//
void init(char * scene_no){
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  gluOrtho2D( 0.0, ds_x, ds_y,0.0 );

  fovScale = tan(M_PI/180 * fov * 0.5);
  aspectRatio = (GLfloat)ds_x/(GLfloat)ds_y;


  if(*scene_no == '1'){
    lights.push_back(Light(vec3(-150,250,0),vec3(1,1,1)));

    plane = new Plane(vec3(0,-9,0),vec3(0,-1,0),vec3(0.3,0.0,0.3),0,150,1,0,2.4);

    spheres.push_back(Sphere(vec3(-10,0,-30),2,vec3(1,0,0),1,2,0,0,1.5));
    spheres.push_back(Sphere(vec3(-5,0,-30),2,vec3(1,1,0),1,10,0,0,1.5));
    spheres.push_back(Sphere(vec3(-0.5,0,-30),2,vec3(0,1,0),1,50,0,0,1.5));
    spheres.push_back(Sphere(vec3(4,0,-30),2,vec3(0,0,1),1,150,0,0,1.5));
  }else if(*scene_no == '2'){
    //point light position, light color vector
    lights.push_back(Light(vec3(-50,250,0),vec3(1,1,1)));

    //point on plane, normal, kD, kS, q, kR, kT, ior
    plane = new Plane(vec3(0,-9,0),vec3(0,-1,0),vec3(0.8,0.8,0.8),0,150,1,0,2.4);

    //center, radius, kD, kS, q, kR, kT, ior
    spheres.push_back(Sphere(vec3(-10,0,-38),4,vec3(1,0,0),1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-2,4,-30),2.7,vec3(0.8,0.2,0.8),1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-4.8,-6,-33),2,vec3(0,1,1),1,150,1,0,2.4));
    spheres.push_back(Sphere(vec3(10,1,-38),8,vec3(0.2,0.2,0.2),1,150,1,0,2.4));
  }
  else if(*scene_no == '3'){
    //point light position, light color vector
    lights.push_back(Light(vec3(50,200,0),vec3(1,1,1)));

    //point on plane, normal, kD, kS, q, kR, kT, ior
    plane = new Plane(vec3(0,-9,0),vec3(0,-1,0),vec3(0.3,0.4,0.3),0,150,1,0,2.4);

    //center, radius, kD, kS, q, kR, kT, ior
    spheres.push_back(Sphere(vec3(0,-1,-20),4,vec3(0.3,0.3,0.3),1,150,0,1,1.5));
    spheres.push_back(Sphere(vec3(-4,5,-40),4,vec3(1,0.3,0.3),1,150,0,1,1.5));
    spheres.push_back(Sphere(vec3(9,-5,-55),4,vec3(0,0.5,1),1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(0,0,-500),150,vec3(1,0.5,0),1,150,0.8,0,1.5));
  }
  else if(*scene_no == '4'){
    //SCENE 4A

    //point light position, light color vector
    lights.push_back(Light(vec3(-20,50,0),vec3(1,1,1)));

    //point on plane, normal, kD, kS, q, kR, kT, ior
    plane = new Plane(vec3(0,-9,0),vec3(0,-1,0),vec3(0.3,0.3,0.3),0,150,1,0,2.4);

    vec3 global_t(0,0,0);

    //head
    vec3 eggCol(0.4,0.8,1);
    spheres.push_back(Sphere(vec3(-2.5,0,-70)+ global_t,17,eggCol,1,100,0.5,0.5,1.5));

    //nose
    spheres.push_back(Sphere(vec3(-1.35,1.3,-38),1.8,vec3(1,0,0),1,50,0.8,0,1.5));

    //mouth
    vec3 lipCol(1,0.3,0.2823);
    vec3 lip_t = vec3(-0.5,1,0) + global_t;
    spheres.push_back(Sphere(vec3(0,-2,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-1.5,-2,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-3,-2,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-4.5,-2,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-5.7,-3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-5.7,-4.5,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-4.5,-5.3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-3,-5.3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-1.5,-5.3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(0,-5.3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(1.5,-5.3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(3.0,-5.3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(1.5,-2,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(3.0,-2,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(4.2,-3,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(4.2,-4.5,-38) + lip_t,0.75,lipCol,1,150,0.8,0,1.5));

    //  teeth
    vec3 teethCol(1,1,1);
    vec3 teeth_t = vec3(-0.5,1,0) + global_t;
    GLfloat teeth_ior = 5;
    spheres.push_back(Sphere(vec3(-3.8,-3,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(-2.3,-3,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(-0.8,-3,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(0.7,-3,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(2.2,-3,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(-3.8,-4.5,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(-2.3,-4.5,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(-0.8,-4.5,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(0.7,-4.5,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));
    spheres.push_back(Sphere(vec3(2.2,-4.5,-38.1) + teeth_t, 0.75, teethCol,1,150,1,0,teeth_ior));

    //eyes
    vec3 eyes_t = vec3(0,0,0) + global_t;
    spheres.push_back(Sphere(vec3(-4.5,3.5,-38),2,vec3(0.0,0.0,0.0),1,150,0.8,1,1.5));
    spheres.push_back(Sphere(vec3(1.8,3.5,-38),2,vec3(0.0,0.0,0.0),1,150,0.8,1,1.5));
    //  pupil
    spheres.push_back(Sphere(vec3(-5.5,4.5,-45),1,vec3(0.0,0.0,0.0),1,150,0,0,1.5));
    spheres.push_back(Sphere(vec3(1.7,4.5,-45),1,vec3(0.0,0.0,0.0),1,150,0,0,1.5));

    //background
    spheres.push_back(Sphere(vec3(0,0,-1000),800,vec3(1,1,1),0.001,150,0.8,0,1.5));

    //mirror objects
    spheres.push_back(Sphere(vec3(-0.5,-7.5,-35),1.5,vec3(0,0,1),1,50,0.8,0.3,1.5));
    spheres.push_back(Sphere(vec3(3,-7,-35),2,vec3(1,0,0),1,50,0.8,0.3,1.5));
    spheres.push_back(Sphere(vec3(8,-6,-35),3,vec3(0,0.4,0),1,50,1,0,1.5));
    spheres.push_back(Sphere(vec3(-7,-6,-28),4,vec3(0.2,0.2,0.2),1,50,0.5,0.5,2.4));


  }else if(*scene_no == '5'){
    //SCENE 4B
    lights.push_back(Light(vec3(-20,100,0),vec3(1,1,1)));

    plane = new Plane(vec3(0,-9,0),vec3(0,-1,0),vec3(0.8,0.8,0.8),0,150,1,0,2.4);

    spheres.push_back(Sphere(vec3(-10,0,-38),4,vec3(1,0,0),1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(-9.5,10,-70),7,vec3(0,1,0),1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(0,0,-45),7,vec3(0,0,0.8),1,150,1,1,1.5));
    spheres.push_back(Sphere(vec3(13,0,-80),5,vec3(1,1,0),1,150,0.8,0.3,1.5));
    spheres.push_back(Sphere(vec3(1.5,-7,-32),1,vec3(0,0.5,1),1,150,0.8,0,1.5));
    spheres.push_back(Sphere(vec3(0,22,-85),7,vec3(0.2,0.2,0.2),1,150,1,0,0.16666));
    spheres.push_back(Sphere(vec3(12,-5,-40),3,vec3(0,1,1),0.5,150,0.8,0.2,1.5));

  }
}

//-----------------------------------------------------------------------------------------------//
void timer(int x){
  glutTimerFunc(1,timer,x);
}

//-----------------------------------------------------------------------------------------------//
void keys(unsigned char key, int x, int y){
  if (key == 'q'){
    delete plane;
    exit(EXIT_SUCCESS);
  }
}

//-----------------------------------------------------------------------------------------------//
int main(int argc, char *argv[]){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(80, 80);
  glutInitWindowSize(ds_x,ds_y);
  glutCreateWindow("Ray Tracing Test");
  bool del = false;
  char * argm = argv[1];
  if(!argm){
    del = true;
    argm = new char('1');
  }

  init(argm);
  if(del) delete argm;
  glutDisplayFunc(displayF);
  glutIdleFunc(displayF);
  glutKeyboardFunc(keys);
  glutTimerFunc(0,timer,0);
  glutMainLoop();
}
