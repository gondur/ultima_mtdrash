/* ********************************************************** */
/* ****    Linear Algebra Primitives                     **** */
/* ****                                                  **** */
/* ****    Copyright: Kasper Fauerby, Peroxide 2001      **** */
/* ****               telemachos@peroxide.dk             **** */
/* ********************************************************** */
#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "../basicTypes.h"
#include <math.h>
#include <stdlib.h>

#define EPSILON 1.0e-5f
#define PI 3.14159265358979323846f

// Forward declaration
class MATRIX;
class VECTOR;
class BBOX;

// Math utility functions:
inline float Min(float a, float b) { return (a < b) ? a : b; }
inline float Max(float a, float b) { return (a < b) ? b : a; }
inline bool  FuzzyCompare(const double a, const double b, const double margin=EPSILON) { return ( fabs(a-b) < margin) ? true : false; }
int roundedFloat(float val);

inline float randFloat(float min, float max) {
	float range = max - min;
	return min + (rand()/float(RAND_MAX))*range;
}

DWORD MixInAlpha(DWORD color, BYTE alpha);

float directionToRotation(const VECTOR& lookDir);


// COLOR class. Uses float values, but expects r,g,b values in the range 0->255
class COLOR {
 public:
  float r;
  float g;
  float b;
  float a;

  COLOR() : r(0), g(0), b(0), a(0)
  {}
  COLOR(float r, float g, float b, float a=255) : r(r), g(g), b(b), a(a)
  {}
  
  DWORD asDWORD() const;
  
  // Overloaded operators:
  inline bool operator==(const COLOR& c) const {return r == c.r && g == c.g && b == c.b && a == c.a;}
  inline bool operator!=(const COLOR& c) const {return !((*this) == c); } 
  inline COLOR operator-(const COLOR& c) const {return COLOR(r-c.r, g-c.g, b-c.b, a-c.a);}
  inline COLOR operator+(const COLOR& c) const {return COLOR(r+c.r, g+c.g, b+c.b, a+c.a);}
  inline COLOR operator*(const float f) const {return COLOR(r*f,g*f,b*f,a*f);}
  inline COLOR operator/(const float f) const {return COLOR(r/f,g/f,b/f,a/f);}
  inline COLOR operator/=(const float f) {r/=f; g/=f; b/=f; a/=f; return *this;}
  inline COLOR operator*=(const float f) {r*=f; g*=f; b*=f; a*=f; return *this;}
  
};


// Primitives: 

class POINT2D { 
 public:
  float x;
  float y;
  POINT2D() {};
  POINT2D(float vx, float vy) {x = vx; y = vy;}
   
  inline float length() {return sqrt(x*x + y*y);}
  POINT2D setLength(float length);
  inline bool isZeroVector() const {return (x==0.0 && y==0.0);}
  
  inline POINT2D rotate90() const {return POINT2D(-y, x);}
  inline POINT2D rotateMinus90() const {return POINT2D(y, -x);}
  
  
  //Operator overloads:
  inline const float& operator[](int index) const {return (&x)[index];}  
  inline float& operator[](int index) {return (&x)[index];}
  inline POINT2D operator-() {return POINT2D(-x, -y);} // unary minus   
  inline POINT2D operator-(const POINT2D &v) const {return POINT2D(x-v.x, y-v.y);}
  inline POINT2D operator+(const POINT2D &v) const {return POINT2D(x+v.x, y+v.y);}
  inline POINT2D operator+=(const POINT2D &v) {x +=v.x; y+=v.y; return *this;}     
  inline POINT2D operator-=(const POINT2D &v) {x -=v.x; y-=v.y; return *this;}         
  inline POINT2D operator/(const float f) const {return POINT2D(x/f, y/f);} // division by scalar
  inline POINT2D operator*(const float f) const {return POINT2D(x*f, y*f);} // multiplication by scalar
  
  inline float   operator%(const POINT2D &v) const {return (x*v.x + y*v.y);} //dot
  
  inline bool   operator==(const POINT2D &v) const {return (x==v.x && y==v.y);}  // equality test.  
  inline bool   operator!=(const POINT2D &v) const {return (x!=v.x || y!=v.y);}  // inequality test.
  
};	 



class VECTOR { 
 public:
  float x;
  float y;
  float z;
  VECTOR() {};
  VECTOR(float vx, float vy, float vz) {x = vx; y = vy; z = vz;}

  // Methods:
  VECTOR SetLength(float l);
  VECTOR normalize();
  
  inline float length() {return sqrt(squaredLength());}
  inline float squaredLength() {return x*x + y*y + z*z;}
  inline bool isZeroVector() const {return (x==0.0 && y==0.0 && z==0.0);}
  VECTOR getPerpendicular() const;
    
  //Operator overloads:
  inline VECTOR operator^(const VECTOR &v) const {return VECTOR(y*v.z-v.y*z, z*v.x-v.z*x, x*v.y-v.x*y);}  //cross product
   
  inline const float& operator[](int index) const {return (&x)[index];}  
  inline float& operator[](int index) {return (&x)[index];}
  inline VECTOR operator-() {return VECTOR(-x, -y, -z);} // unary minus   
  inline VECTOR operator-(const VECTOR &v) const {return VECTOR(x-v.x, y-v.y, z-v.z);}
  inline VECTOR operator-=(const VECTOR &v) {x -=v.x; y-=v.y; z-=v.z; return *this;}         
  inline VECTOR operator+(const VECTOR &v) const {return VECTOR(x+v.x, y+v.y, z+v.z);}
  inline VECTOR operator+=(const VECTOR &v) {x +=v.x; y+=v.y; z+=v.z; return *this;}     
  inline float  operator%(const VECTOR &v) const {return (x*v.x + y*v.y + z*v.z);}     //dot
  inline VECTOR operator/(const VECTOR &v) const {return VECTOR(x/v.x, y/v.y, z/v.z);} // component-wise division
  inline VECTOR operator/=(const VECTOR &v) {x/=v.x; y/=v.y; z/=v.z; return *this;}    // component-wise division
  inline VECTOR operator/(const float f) const {return VECTOR(x/f, y/f, z/f);}         // division by scalar
  inline VECTOR operator/=(const float f) {x/=f; y/=f; z/=f; return *this;}            // division by scalar
  inline VECTOR operator*(const VECTOR &v) const {return VECTOR(x*v.x, y*v.y, z*v.z);} // component-wise multiplication
  inline VECTOR operator*=(const VECTOR &v){x*=v.x; y*=v.y; z*=v.z; return *this;}     // component-wise multiplication	
  inline VECTOR operator*(const float f) const {return VECTOR(x*f, y*f, z*f);}         // multiplication by scalar
	inline VECTOR operator*=(const float f) {x*=f; y*=f; z*=f; return *this;}            // multiplication by scalar
 
  // Multiply by matrix
	VECTOR& operator*=(const MATRIX& m);
	  
  inline bool   operator==(const VECTOR &v) const {return (x==v.x && y==v.y && z==v.z);}  // equality test.  
  inline bool   operator!=(const VECTOR &v) const {return (x!=v.x || y!=v.y || z!=v.z);}  // inequality test.
  
  POINT2D inXZPlane() const {return POINT2D(x, z);}
  
};	 

// Each vertex is 32bytes for optimal memory alignment
class VERTEX {
 public:
  float x,y,z;
  float nx,ny,nz;
  float u,v;
  VERTEX() {};
  VERTEX(float vx, float vy, float vz, float vnx, float vny, float vnz, float vu, float vv) {
    x = vx; y = vy; z = vz;
    nx = vnx; ny = vny; nz = vnz;
    u = vu; v = vv;	
  }
  VERTEX(VECTOR pos, VECTOR normal, float vu, float vv) {
    x = pos.x;	y = pos.y; z = pos.z;
    nx = normal.x; ny = normal.y; nz = normal.z;	
    u = vu;
    v = vv;	
  } 
  
  inline bool operator==(const VERTEX &V) const {return (x==V.x && y==V.y && z==V.z && nx==V.nx && ny==V.ny && nz==V.nz && u==V.u && v==V.u);}  // equality test.  
  inline bool operator!=(const VERTEX &V) const {return (x!=V.x || y!=V.y || z!=V.z || nx!=V.nx || ny!=V.ny || nz!=V.nz || u!=V.u || v!=V.u);}  // inequality test.  
 
  inline VECTOR asVector() {return VECTOR(x,y,z);}
  inline VECTOR normal() {return VECTOR(nx,ny,nz);}
};

class VECTOR4D
{
public:
  float x,y,z,w;
  VECTOR4D() {}
  VECTOR4D(float x, float y, float z, float w = 1) : x(x), y(y), z(z), w(w) {}
  VECTOR4D(const VECTOR& v, float w = 1) : x(v.x), y(v.y), z(v.z), w(w) {}
  VECTOR4D(float v[4]) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}
  VECTOR4D(const COLOR& c) : x(c.r), y(c.g), z(c.b), w(c.a) {}
  

  inline VECTOR4D operator-() {return VECTOR4D(-x, -y, -z, -w);} // unary minus   
  inline VECTOR4D operator-(const VECTOR4D &v) const {return VECTOR4D(x-v.x, y-v.y, z-v.z, w-v.w);}
  inline VECTOR4D operator-=(const VECTOR4D &v) {x -=v.x; y-=v.y; z-=v.z; w-=v.w; return *this;}         
  inline VECTOR4D operator+(const VECTOR4D &v) const {return VECTOR4D(x+v.x, y+v.y, z+v.z, w+v.w);}
  inline VECTOR4D operator+=(const VECTOR4D &v) {x +=v.x; y+=v.y; z+=v.z; w+=v.w; return *this;}     
  inline float  operator%(const VECTOR4D &v) const {return (x*v.x + y*v.y + z*v.z + w*v.w);}     //dot
  inline VECTOR4D operator/(const VECTOR4D &v) const {return VECTOR4D(x/v.x, y/v.y, z/v.z, w/v.w);} // component-wise division
  inline VECTOR4D operator/=(const VECTOR4D &v) {x/=v.x; y/=v.y; z/=v.z; w/=v.w; return *this;}    // component-wise division
  inline VECTOR4D operator/(const float f) const {return VECTOR4D(x/f, y/f, z/f, w/f);}         // division by scalar
  inline VECTOR4D operator/=(const float f) {x/=f; y/=f; z/=f; w/=f; return *this;}            // division by scalar
  inline VECTOR4D operator*(const VECTOR4D &v) const {return VECTOR4D(x*v.x, y*v.y, z*v.z, w*v.w);} // component-wise multiplication
  inline VECTOR4D operator*=(const VECTOR4D &v){x*=v.x; y*=v.y; z*=v.z; w*=v.w; return *this;}     // component-wise multiplication	
  inline VECTOR4D operator*(const float f) const {return VECTOR4D(x*f, y*f, z*f, w*f);}         // multiplication by scalar
	inline VECTOR4D operator*=(const float f) {x*=f; y*=f; z*=f; w*=f; return *this;}            // multiplication by scalar
};



// non optimal memory alignment - 36 bytes..
class CVERTEX {
 public:
  float x,y,z;
  float nx,ny,nz;
  DWORD color;
  float u,v;
  CVERTEX() {};
  CVERTEX(float x, float y, float z, float nx, float ny, float nz, DWORD c, float u, float v)
    : x(x), y(y), z(z), nx(nx), ny(ny), nz(nz), color(c), u(u), v(v) {}
  CVERTEX(VECTOR pos, VECTOR normal, DWORD c, float u, float v)
    : x(pos.x), y(pos.y), z(pos.z), nx(normal.x), ny(normal.y), nz(normal.z), color(c), u(u), v(v) {} 
  
  inline bool operator==(const CVERTEX &V) const {return (x==V.x && y==V.y && z==V.z && nx==V.nx && ny==V.ny && nz==V.nz && color==V.color && u==V.u && v==V.u);}  // equality test.  
  inline bool operator!=(const CVERTEX &V) const {return (x!=V.x || y!=V.y || z!=V.z || nx!=V.nx || ny!=V.ny || nz!=V.nz || color!=V.color || u!=V.u || v!=V.u);}  // inequality test.  
  
  // Arithmetic operators:
  // Note that the color component is uaffected by the operators, since it can't
  //  contain negative values.
  inline CVERTEX operator-() {return CVERTEX(-x, -y, -z, -nx, -ny, -nz, color, -u, -v);} // unary minus
  inline CVERTEX operator-(const CVERTEX &a) const {return CVERTEX(x-a.x, y-a.y, z-a.z, nx-a.nx, ny-a.ny, nz-a.nz, color, u-a.u, v-a.v);}
  inline CVERTEX operator-=(const CVERTEX &a) {x -=a.x; y-=a.y; z-=a.z; nx-=a.nx; ny-=a.ny; nz-=a.nz; u-=a.u; v-=a.v; return *this;}
  inline CVERTEX operator+(const CVERTEX &a) const {return CVERTEX(x+a.x, y+a.y, z+a.z, nx+a.nx, ny+a.ny, nz+a.nz, color, u+a.u, v+a.v);}
  inline CVERTEX operator+=(const CVERTEX &a) {x +=a.x; y+=a.y; z+=a.z; nx+=a.nx; ny+=a.ny; nz+=a.nz; u+=a.u; v+=a.v; return *this;}
  inline CVERTEX operator/(const CVERTEX &a) const {return CVERTEX(x/a.x, y/a.y, z/a.z, nx/a.nx, ny/a.ny, nz/a.nz, color, u/a.u, v/a.v);} // component-wise division
  inline CVERTEX operator/=(const CVERTEX &a) {x/=a.x; y/=a.y; z/=a.z; nx/=a.nx; ny/=a.ny; nz/=a.nz; u/=a.u; v/=a.v; return *this;}    // component-wise division
  inline CVERTEX operator/(const float f) const {return CVERTEX(x/f, y/f, z/f, nx/f, ny/f, nz/f, color, u/f, v/f);}         // division by scalar
  inline CVERTEX operator/=(const float f) {x/=f; y/=f; z/=f; nx/=f; ny/=f; nz/=f; u/=f; v/=f; return *this;}            // division by scalar
  inline CVERTEX operator*(const CVERTEX &a) const {return CVERTEX(x*a.x, y*a.y, z*a.z, nx*a.nx, ny*a.ny, nz*a.nz, color, u*a.u, v*a.v);} // component-wise multiplication
  inline CVERTEX operator*=(const CVERTEX &a){x*=a.x; y*=a.y; z*=a.z; nx*=a.nx; ny*=a.ny; nz*=a.nz; u*=a.u; v*=a.v; return *this;}     // component-wise multiplication	
  inline CVERTEX operator*(const float f) const {return CVERTEX(x*f, y*f, z*f, nx*f, ny*f, nz*f, color, u*f, v*f);}         // multiplication by scalar
	inline CVERTEX operator*=(const float f) {x*=f; y*=f; z*=f; nx*=f; ny*=f; nz*=f; u*=f; v*=f; return *this;}            // multiplication by scalar
  
  inline VECTOR asVector() {return VECTOR(x,y,z);}
  inline VECTOR normal() {return VECTOR(nx,ny,nz);}
};


class GRASSVERTEX {
 public:
  float x,y,z;
  DWORD indices; 
  float u,v;
  float d1,d2,d3; // mapped as second set of texture coords
 
  GRASSVERTEX() {}

  GRASSVERTEX(const CVERTEX& cv) {
    x = cv.x; y = cv.y; z = cv.z;
    u = cv.u; v = cv.v;	
  }
  
  void setIndices(byte i1, byte i2, byte i3, byte i4);
};

class INDEXVECTOR {
 public:
  float x,y,z;
  DWORD indices;

  INDEXVECTOR() {}
  INDEXVECTOR(const VECTOR& v) : x(v.x), y(v.y), z(v.z) {}
  INDEXVECTOR(float x, float y, float z) : x(x), y(y), z(z) {}
  INDEXVECTOR(float x, float y, float z, DWORD indices) : x(x), y(y), z(z), indices(indices) {}
  void setIndices(byte i1, byte i2, byte i3, byte i4);

  inline INDEXVECTOR operator*(const float f) const {return INDEXVECTOR(x*f, y*f, z*f, indices);}        
};


class LVERTEX {
 public:
  VECTOR pos;
  DWORD color;
  DWORD specular;
  float u,v;

  LVERTEX() :
    pos(), color(0), specular(0), u(0), v(0)
  {}

  LVERTEX(const VECTOR& pos, DWORD color, DWORD specular, float u, float v) :
    pos(pos), color(color), specular(specular), u(u), v(v)
  {}

  LVERTEX(const VECTOR& pos, DWORD color) :
    pos(pos), color(color), specular(0), u(0), v(0)
  {}
};

class TLVERTEX { // In screen space coords!
 public:
  VECTOR pos;
  float rhw;
  DWORD color;
  DWORD specular;
  float u,v;
 
  TLVERTEX()
    : pos(), rhw(1), color(0), specular(0), u(0), v(0)
  {}

  TLVERTEX(const VECTOR& pos, DWORD color)
    : pos(pos), rhw(1), color(color), specular(0), u(0), v(0)
  {}

  TLVERTEX(const POINT2D& pos, DWORD color)
    : pos(pos.x, pos.y, 0), rhw(1), color(color), specular(0), u(0), v(0)
  {}
  
  TLVERTEX(const VECTOR& pos, DWORD color, float u, float v)
    : pos(pos), rhw(1), color(color), specular(0), u(u), v(v)
  {}

  TLVERTEX(const POINT2D& pos, DWORD color, float u, float v)
    : pos(pos.x, pos.y, 0 ), rhw(1), color(color), specular(0), u(u), v(v)
  {}

  TLVERTEX(const VECTOR& pos, float rhw, DWORD color, DWORD specular, float u, float v)
    : pos(pos), rhw(1), color(color), specular(specular), u(u), v(v)
  {}

  // TODO this should be removed
  TLVERTEX(float x, float y, float z, float rhw, DWORD color, DWORD specular, float u, float v)
    : pos(x,y,z), rhw(1), color(color), specular(specular), u(u), v(v)
  {}

  inline VECTOR toVector() {return pos;}
};

// 24 bytes
class SHADOWVERTEX {
 public:
  float x,y,z;          // Point
  float nx,ny,nz;       // Normal of the tri this point belongs to

  SHADOWVERTEX() {};
  SHADOWVERTEX(float x, float y, float z, float nx, float ny, float nz) :
    x(x), y(y), z(z), nx(nx), ny(ny), nz(nz)
  {
  }
  SHADOWVERTEX(VECTOR pos, VECTOR normal) :
    x(pos.x), y(pos.y), z(pos.z), nx(normal.x), ny(normal.y), nz(normal.z)
  {
  } 
  
  inline bool operator==(const SHADOWVERTEX &v) const {return (x==v.x && y==v.y && z==v.z && nx==v.nx && ny==v.ny && nz==v.nz);}  // equality test.  
  inline bool operator!=(const SHADOWVERTEX &v) const {return !(*this == v);}  // inequality test.  
 
  inline VECTOR asVector() {return VECTOR(x,y,z);}
  inline VECTOR normal() {return VECTOR(nx,ny,nz);}
};

// ? bytes

class WEDGE_VERTEX {
 public:
  VECTOR pos;                     // Point (in model space)
  VECTOR edgePoint0, edgePoint1;  // Position of creating edge (in model space)
  
  WEDGE_VERTEX(VECTOR pos, VECTOR edgePoint0, VECTOR edgePoint1) :
    pos(pos), edgePoint0(edgePoint0), edgePoint1(edgePoint1) {}
  WEDGE_VERTEX() {}
 
  inline bool operator==(const WEDGE_VERTEX &v) const {return (pos==v.pos && edgePoint0 == v.edgePoint0 && edgePoint1 == v.edgePoint1);}  // equality test.  
  inline bool operator!=(const WEDGE_VERTEX &v) const {return !(*this == v);}  // inequality test.  
};

class MATRIX { // NOTE: Indices are (column,row)!
 public:
  float _11, _12, _13, _14; 
  float _21, _22, _23, _24; 
  float _31, _32, _33, _34; 
  float _41, _42, _43, _44; 
  
  MATRIX() {}
  MATRIX(VECTOR x, VECTOR y, VECTOR z, VECTOR pos) {identity(); setX(x); setY(y); setZ(z); setPosition(pos);}
  
  MATRIX identity();
  MATRIX zero();
  bool isIdentity();
  MATRIX SetTranslate(float x, float y, float z);
  MATRIX SetTranslate(VECTOR &v);
  MATRIX SetScale(float x, float y, float z);
  MATRIX SetScale(const VECTOR& v);
  
  MATRIX SetRotateX(float fRads);
  MATRIX SetRotateY(float fRads);
  MATRIX SetRotateZ(float fRads);
  MATRIX SetRotation(VECTOR &vDir, float fRads);
  
  MATRIX BuildFromEuler(VECTOR angle);
  
  void makeUnitTripod();
  
  void setX(VECTOR vector);
  void setY(VECTOR vector);
  void setZ(VECTOR vector);
  void setPosition(VECTOR pos);
  
  VECTOR getX() const {return VECTOR(_11, _12, _13);}
  VECTOR getY() const {return VECTOR(_21, _22, _23);}
  VECTOR getZ() const {return VECTOR(_31, _32, _33);}
  VECTOR getPosition() const {return VECTOR(_41, _42, _43);}
  
  void setBillboard(const VECTOR& billboardPosition, const VECTOR& viewerPosition);
  MATRIX inverse() const;
  
  MATRIX upper3x3() const;
  
  void logOut() const;
  
  // Operator overloads:
  MATRIX operator*(const MATRIX &m) const;
  VECTOR operator*(const VECTOR &v) const;
  VECTOR4D operator*(const VECTOR4D& v) const;
  VERTEX operator*(const VERTEX &v) const;
  CVERTEX operator*(const CVERTEX &v) const;
  MATRIX operator*(float f) const;
  MATRIX operator+(const MATRIX &m) const;
  MATRIX operator-(const MATRIX &m) const;
  MATRIX operator+=(const MATRIX &m) {return (*this = *this + m);}
  MATRIX operator-=(const MATRIX &m) {return (*this = *this - m);}
};

class RECTANGLE {
 public:
  POINT2D min;
  POINT2D max;

  RECTANGLE() {}
  RECTANGLE(const POINT2D &min, const POINT2D &max) {this->min = min; this->max = max;}

  float area() const {return ((max.x-min.x) * (max.y - min.y));}

  void merge(const RECTANGLE& otherRect) {
    if (otherRect.max.x > max.x) max.x = otherRect.max.x;
    if (otherRect.max.y > max.y) max.y = otherRect.max.y;
    if (otherRect.min.x < min.x) min.x = otherRect.min.x;
    if (otherRect.min.y < min.y) min.y = otherRect.min.y;
  }

  bool contains(const POINT2D& point) const {
    return (point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y);
  }
};

class BBOX {
 public:
  VECTOR min;
  VECTOR max;
  
  BBOX(const VECTOR &bbmin, const VECTOR &bbmax) {min = bbmin; max = bbmax;}
  BBOX() {min = VECTOR(0,0,0); max = VECTOR(0,0,0);}
  
  VECTOR center() const;
  
  float ProjectedSize(MATRIX &mat);
  float diagonalLength();
  double volume();
  void merge(const BBOX& otherBox);
  void addPoint(const VECTOR& p);

  bool intersectSphere(const VECTOR& center, float radius) const;
  
  inline bool contains(const VECTOR& point) const {
     return (point.x >= min.x && point.y >= min.y && point.z >= min.z &&
             point.x <= max.x && point.y <= max.y && point.z <= max.z);
  }

  inline bool contains(const BBOX& otherBox) const {
     return (contains(otherBox.min) && contains(otherBox.max));
  }

  BBOX extrude(const VECTOR& extrusion);
  BBOX extrudeFromPointLight(const VECTOR& lightPos, float lightRange);
};


class QUATERNION {
 public:
  float x,y,z,s;
 
  QUATERNION() {s=1; x=0; y=0; z=0;}    // multiplication identity.
  QUATERNION(float X, float Y, float Z, float S) {x=X; y=Y; z=Z; s=S;}       
  QUATERNION(VECTOR angle);             // build from euler angles
  QUATERNION(VECTOR axis, float angle); // build from axis/angle
  QUATERNION(const MATRIX &mat);               // buikd from a rotation matrix
  
  QUATERNION operator*(const QUATERNION &q) const;
  
  MATRIX toMatrix();      
  QUATERNION Slerp(QUATERNION& to, float t);      
};


typedef enum {PLANE_BACK, PLANE_FRONT, PLANE_ON} PointPlaneRelation;
class PLANE {
 public:
  float equation[4];
  VECTOR origin;
  VECTOR normal;
  
  PLANE(); 
  PLANE(const VECTOR& origin, const VECTOR& normal);

  bool isFrontFacingTo(const VECTOR& direction) const;
  PointPlaneRelation classifyPoint(const VECTOR& point) const;
  double signedDistanceTo(const VECTOR& point) const;

  // Base is defined by (outU, outV, normal)
  void getBasisVectors(VECTOR* outU, VECTOR* outV) const;

  POINT2D projectToPlane(const VECTOR& point) const;
};

class RAY {
 public:
  VECTOR origin;
  VECTOR dir;

  RAY(const VECTOR& origin, const VECTOR& dir);

  bool intersectsPlane(const PLANE& p, double* distanceToPlane);
  bool intersectsBBox(const BBOX& box) const;
};





// Ordering of operands
VECTOR operator*(float f, const VECTOR &v);
COLOR operator*(float f, const COLOR &c);



#endif
