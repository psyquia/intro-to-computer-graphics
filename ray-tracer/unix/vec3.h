struct vec3 {
  GLfloat x, y, z;
  vec3() : x(0), y(0), z(0) {}
  vec3(GLfloat x, GLfloat y, GLfloat z): x(x), y(y), z(z) {}
  GLfloat& operator [] ( int i ) { return *(&x + i); }
  const GLfloat operator [] ( int i ) const { return *(&x + i); }

  vec3 operator - () const {return vec3( -x, -y, -z ); }
  vec3 operator + (const vec3& v) const {return vec3( x + v.x, y + v.y, z + v.z );}
  vec3 operator - (const vec3& v) const	{return vec3( x - v.x, y - v.y, z - v.z );}
  vec3 operator * (const GLfloat s) const {return vec3( s*x, s*y, s*z );}
  vec3 operator * (const vec3& v) const {return vec3( x*v.x, y*v.y, z*v.z );}
  vec3 operator / (const GLfloat s) const {return vec3( x/s, y/s, z/s);}
  vec3 normalize() const{
    GLfloat mag = sqrt(x*x + y*y + z*z);
    return vec3(x/mag, y/mag, z/mag);
  }
  friend std::ostream& operator << ( std::ostream& os, const vec3& v ) {
return os << "( " << v.x << ", " << v.y << ", " << v.z <<  " )";
  }

  friend std::istream& operator >> ( std::istream& is, vec3& v )
{ return is >> v.x >> v.y >> v.z ; }
};

GLfloat dot( const vec3& u, const vec3& v ) {
  return u.x*v.x + u.y*v.y + u.z*v.z ;
}
GLfloat length( const vec3& v ){
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}
