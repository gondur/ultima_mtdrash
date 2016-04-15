#include "primitives.h"
#include "basicTypes.h"
#include "utility/Logger.h"

DWORD MixInAlpha(DWORD color, BYTE alpha) {
	// clear top 8 bits 
	DWORD res = color << 8;
	res = res >> 8;
 
	// set new alpha value
	return (res | (alpha << 24));
}

int roundedFloat(float val) {
  int res;
  float fraction = val - (int)val;
  if (fraction > 0.5)
    res = (int)val + 1;
  else
    res = (int)val;

  return res;
}

float directionToRotation(const VECTOR& lookDir) {
  float angle = 0;
	VECTOR dir = lookDir;

	if (dir.length() > 0.01f) {
		dir.normalize();

		VECTOR neutralDir = VECTOR(0,0,-1);
		VECTOR crossDir = VECTOR(1,0,0);
  
		float dot = neutralDir%dir;
		float factor = crossDir%dir;
		if (factor < 0)
			factor = -1;
		else
			factor = 1;
  
		angle = factor*acos(dot);
	}

 return angle;
}
POINT2D POINT2D::setLength(float length)
{
  float divisor = this->length() / length;
  (*this) = (*this) / divisor;
  
  return *this;
}



void GRASSVERTEX::setIndices(byte i1, byte i2, byte i3, byte i4) {
  indices = COLOR(i1,i2,i3,i4).asDWORD();
}

void INDEXVECTOR::setIndices(byte i1, byte i2, byte i3, byte i4) {
 indices = COLOR(i1,i2,i3,i4).asDWORD();
}

// The COLOR class:

DWORD COLOR::asDWORD() const 
{
  Log("WARNING - COLOR::asDWORD called from project that does not include the gfxdriver");
  return 0;
  //return gfxdriver->colorToDword(int(r), int(g), int(b), int(a));
}

// The VECTOR class:
VECTOR VECTOR::normalize() {
  float l = sqrt(x*x + y*y + z*z);
  if (!(l < EPSILON || l == 1.0f)) {
    x /= l;
    y /= l;
    z /= l;
  }
  
  return *this;	
}


VECTOR VECTOR::SetLength(float l) {
  float len = sqrt(x*x + y*y + z*z);

  if (len > 0.0f) {
    x *= l/len;
    y *= l/len;
    z *= l/len;
  }
  
  return *this;
}



VECTOR VECTOR::getPerpendicular() const {
 if (fabs(x) > fabs(y)) {
   float len = sqrt(x * x + z * z);		
   return VECTOR(z / len, 0.0f, -x / len);
 }
 else {
   float len = sqrt(y * y + z * z);		
   return VECTOR(0.0f, z / len, -y / len);
 }
}



// The MATRIX class:
MATRIX MATRIX::identity() {
    _12 = _13 = _14 = _21 = _23 = _24 = 0.0f;
    _31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
    _11 = _22 = _33 = _44 = 1.0f;

   return *this;
}

// The MATRIX class:
MATRIX MATRIX::zero() {
  _12 = _13 = _14 = _21 = _23 = _24 = 0.0f;
  _31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
  _11 = _22 = _33 = _44 = 0.0f;
    
  return *this;    
}

// Assumes that matrix is invertible! Use with care!
MATRIX MATRIX::inverse() const {
  MATRIX res;

  float fDetInv = 1.0f / ( _11 * ( _22 * _33 - _23 * _32 ) -
                           _12 * ( _21 * _33 - _23 * _31 ) +
                           _13 * ( _21 * _32 - _22 * _31 ));

  res._11 =  fDetInv * (_22 * _33 - _23 * _32);
  res._12 = -fDetInv * (_12 * _33 - _13 * _32);
  res._13 =  fDetInv * (_12 * _23 - _13 * _22);
  res._14 = 0.0f;

  res._21 = -fDetInv * (_21 * _33 - _23 * _31);
  res._22 =  fDetInv * (_11 * _33 - _13 * _31);
  res._23 = -fDetInv * (_11 * _23 - _13 * _21);
  res._24 = 0.0f;

  res._31 =  fDetInv * (_21 * _32 - _22 * _31);
  res._32 = -fDetInv * (_11 * _32 - _12 * _31);
  res._33 =  fDetInv * (_11 * _22 - _12 * _21);
  res._34 = 0.0f;

  res._41 = -(_41 * res._11 + _42 * res._21 + _43 * res._31);
  res._42 = -(_41 * res._12 + _42 * res._22 + _43 * res._32);
  res._43 = -(_41 * res._13 + _42 * res._23 + _43 * res._33);
  res._44 = 1.0f;

  return res;
}

MATRIX MATRIX::upper3x3() const {

  MATRIX res = *this;
  res._41 = 0.0f;
  res._42 = 0.0f;
  res._43 = 0.0f;
  res._44 = 1.0f;
  res._14 = 0.0f;
  res._24 = 0.0f;
  res._34 = 0.0f;

  return res;
}


MATRIX MATRIX::SetTranslate(float x, float y, float z) {
 
  identity();

  // Set translation:
  _41 = x; _42 = y; _43 = z;	

  return *this;
}

MATRIX MATRIX::SetTranslate(VECTOR &v) {
 
  identity();

  // Set translation:
  _41 = v.x; _42 = v.y; _43 = v.z;	

  return *this;
}


MATRIX MATRIX::SetScale(float x, float y, float z) {
 
  identity();

  // Set Scale:
  _11 = x; _22 = y; _33 = z;
 
  return *this;
}

MATRIX MATRIX::SetScale(const VECTOR& v) {
 
  identity();

  // Set Scale:
  _11 = v.x; _22 = v.y; _33 = v.z;

  return *this;
}

MATRIX MATRIX::SetRotateX(float fRads) {
	
  identity();

  // Rotation
  _22 = (float)cos( fRads );
  _23 = (float)sin( fRads );
  _32 = (float)-sin( fRads );
  _33 = (float)cos( fRads );

  return *this;
}

MATRIX MATRIX::SetRotateY(float fRads) {
 
  identity();

  // Rotation:
  _11 = (float)cos( fRads );
  _13 = (float)-sin( fRads );
  _31 = (float)sin( fRads );
  _33 = (float)cos( fRads );

  return *this;
}

MATRIX MATRIX::SetRotateZ(float fRads) {

  identity();

  // Rotation:
  _11  = (float) cos( fRads );
  _12  = (float) sin( fRads );
  _21  = (float)-sin( fRads );
  _22  = (float) cos( fRads );

  return *this;
}

MATRIX MATRIX::SetRotation( VECTOR &vDir, float fRads ) {

  float fCos = (float)cos( fRads );
  float fSin = (float)sin( fRads );
  VECTOR v = vDir;
  v.normalize();

  _11 = ( v.x * v.x ) * ( 1.0f - fCos ) + fCos;
  _12 = ( v.x * v.y ) * ( 1.0f - fCos ) - (v.z * fSin);
  _13 = ( v.x * v.z ) * ( 1.0f - fCos ) + (v.y * fSin);
  _14 = 0.0f;

  _21 = ( v.y * v.x ) * ( 1.0f - fCos ) + (v.z * fSin);
  _22 = ( v.y * v.y ) * ( 1.0f - fCos ) + fCos ;
  _23 = ( v.y * v.z ) * ( 1.0f - fCos ) - (v.x * fSin);
  _24 = 0.0f;

  _31 = ( v.z * v.x ) * ( 1.0f - fCos ) - (v.y * fSin);
  _32 = ( v.z * v.y ) * ( 1.0f - fCos ) + (v.x * fSin);
  _33 = ( v.z * v.z ) * ( 1.0f - fCos ) + fCos;
  _34 = 0.0f;

  _41 = 0.0f;
  _42 = 0.0f;
  _43 = 0.0f;
  _44 = 1.0f;

  return *this;
}

MATRIX MATRIX::BuildFromEuler(VECTOR angle) {
  MATRIX xmat; xmat.SetRotateX(angle.x);
  MATRIX ymat; ymat.SetRotateY(angle.y);
  MATRIX zmat; zmat.SetRotateZ(angle.z);
  
  *this = xmat * ymat * zmat;
  
  return *this;
}



// Matrix / Matrix mult. Returns 'this * m'
MATRIX MATRIX::operator*(const MATRIX &m) const {
  MATRIX res;	

//  res._11 = 
 float* pA = (float*)this;
 float* pB = (float*)&m;
 float  pM[16];

 for( int i=0; i<4; i++ ) 
  for( int j=0; j<4; j++ ) 
   for( int k=0; k<4; k++ ) 
     pM[4*i+j] += pA[4*i+k] * pB[4*k+j];

 memcpy( &res, pM, sizeof(MATRIX) );	

 return res;	
}

// Vector / Matrix mult
VECTOR MATRIX::operator*(const VECTOR &v) const {
  VECTOR res;	
  
  float x = v.x*_11 + v.y*_21 + v.z* _31 + _41;
  float y = v.x*_12 + v.y*_22 + v.z* _32 + _42;
  float z = v.x*_13 + v.y*_23 + v.z* _33 + _43;
  float w = v.x*_14 + v.y*_24 + v.z* _34 + _44;
    
  if( fabs( w ) < EPSILON ) {
    //return NULL-vector if error TODO
    res.x = 0;
    res.y = 0;
    res.z = 0;
    return res;
  }
  
  res.x = x/w;
  res.y = y/w;
  res.z = z/w;
  
  return res;
}
VECTOR4D MATRIX::operator*(const VECTOR4D &v) const {
  
  float x = v.x*_11 + v.y*_21 + v.z* _31 + v.w*_41;
  float y = v.x*_12 + v.y*_22 + v.z* _32 + v.w*_42;
  float z = v.x*_13 + v.y*_23 + v.z* _33 + v.w*_43;
  float w = v.x*_14 + v.y*_24 + v.z* _34 + v.w*_44;
 
  return VECTOR4D(x,y,z,w);
}

VECTOR& VECTOR::operator*=(const MATRIX& m)
{
  float w = x*m._14 + y*m._24 + z*m._34 + m._44;

  if( fabs( w ) < EPSILON ) {
    //return NULL-vector if error TODO
    x = 0;
    y = 0;
    z = 0;
    return *this;
  }

  float tempX = x*m._11 + y*m._21 + z*m._31 + m._41;
  float tempY = x*m._12 + y*m._22 + z*m._32 + m._42;

  z = (x*m._13 + y*m._23 + z*m._33 + m._43) / w;

  x = tempX / w;
  y = tempY / w;
 
	return *this;
}

// Vertex / Matrix mult
VERTEX MATRIX::operator*(const VERTEX &v) const {
  VERTEX res;	
  res.u = v.u;
  res.v = v.v;
  
   
  // the position:
  float x = v.x*_11 + v.y*_21 + v.z* _31 + _41;
  float y = v.x*_12 + v.y*_22 + v.z* _32 + _42;
  float z = v.x*_13 + v.y*_23 + v.z* _33 + _43;
  float w = v.x*_14 + v.y*_24 + v.z* _34 + _44;
    
  if( fabs( w ) < EPSILON ) {
    //return NULL-vector if error TODO
    res.x = 0;
    res.y = 0;
    res.z = 0;
    return res;
  }

  res.x = x/w;
  res.y = y/w;
  res.z = z/w;
 
  // the normal
  VECTOR n;
  x = v.nx*_11 + v.ny*_21 + v.nz* _31;
  y = v.nx*_12 + v.ny*_22 + v.nz* _32;
  z = v.nx*_13 + v.ny*_23 + v.nz* _33;
  w = v.nx*_14 + v.ny*_24 + v.nz* _34 + _44;  
 
    
  if( fabs( w ) < EPSILON ) {
    //return NULL-vector if error TODO
    res.nx = 0;
    res.ny = 0;
    res.nz = 0;
    return res;
  }

  n.x = x/w;
  n.y = y/w;
  n.z = z/w;
 
  // normarlize
  n.normalize();
  
  res.nx = n.x;
  res.ny = n.y;
  res.nz = n.z;
 
  return res;
} 


// CVertex / Matrix mult
CVERTEX MATRIX::operator*(const CVERTEX &v) const {
  CVERTEX res;	
  res.u = v.u;
  res.v = v.v;
  res.color = v.color;
   
  // the position:
  float x = v.x*_11 + v.y*_21 + v.z* _31 + _41;
  float y = v.x*_12 + v.y*_22 + v.z* _32 + _42;
  float z = v.x*_13 + v.y*_23 + v.z* _33 + _43;
  float w = v.x*_14 + v.y*_24 + v.z* _34 + _44;
    
  if( fabs( w ) < EPSILON ) {
    //return NULL-vector if error TODO
    res.x = 0;
    res.y = 0;
    res.z = 0;
    return res;
  }

  res.x = x/w;
  res.y = y/w;
  res.z = z/w;
 
  // the normal
  VECTOR n;
  x = v.nx*_11 + v.ny*_21 + v.nz* _31;
  y = v.nx*_12 + v.ny*_22 + v.nz* _32;
  z = v.nx*_13 + v.ny*_23 + v.nz* _33;
  w = v.nx*_14 + v.ny*_24 + v.nz* _34 + _44;  
 
    
  if( fabs( w ) < EPSILON ) {
    //return NULL-vector if error TODO
    res.nx = 0;
    res.ny = 0;
    res.nz = 0;
    return res;
  }

  n.x = x/w;
  n.y = y/w;
  n.z = z/w;
 
  // normarlize
  n.normalize();
  
  res.nx = n.x;
  res.ny = n.y;
  res.nz = n.z;
 
  return res;
} 


 
MATRIX MATRIX::operator*(float f) const
{
  MATRIX res;
  
  res._11 = _11*f;
  res._21 = _21*f;
  res._31 = _31*f;
  res._41 = _41*f;
  
  res._12 = _12*f;
  res._22 = _22*f;
  res._32 = _32*f;
  res._42 = _42*f;
  
  res._13 = _13*f;
  res._23 = _23*f;
  res._33 = _33*f;
  res._43 = _43*f;
  
  res._14 = _14*f;
  res._24 = _24*f;
  res._34 = _34*f;
  res._44 = _44*f;
  
  return res;
}

MATRIX MATRIX::operator+(const MATRIX &m) const
{
  MATRIX res;
  
  res._11 = _11 + m._11;
  res._21 = _21 + m._21;
  res._31 = _31 + m._31;
  res._41 = _41 + m._41;
  
  res._12 = _12 + m._12;
  res._22 = _22 + m._22;
  res._32 = _32 + m._32;
  res._42 = _42 + m._42;
  
  res._13 = _13 + m._13;
  res._23 = _23 + m._23;
  res._33 = _33 + m._33;
  res._43 = _43 + m._43;
  
  res._14 = _14 + m._14;
  res._24 = _24 + m._24;
  res._34 = _34 + m._34;
  res._44 = _44 + m._44;
  
  return res;
}

MATRIX MATRIX::operator-(const MATRIX &m) const
{
  MATRIX res;
  
  res._11 = _11 - m._11;
  res._21 = _21 - m._21;
  res._31 = _31 - m._31;
  res._41 = _41 - m._41;
  
  res._12 = _12 - m._12;
  res._22 = _22 - m._22;
  res._32 = _32 - m._32;
  res._42 = _42 - m._42;
  
  res._13 = _13 - m._13;
  res._23 = _23 - m._23;
  res._33 = _33 - m._33;
  res._43 = _43 - m._43;
  
  res._14 = _14 - m._14;
  res._24 = _24 - m._24;
  res._34 = _34 - m._34;
  res._44 = _44 - m._44;
  
  return res;
}


bool MATRIX::isIdentity() {
 return (_11 == 1.0f && _12 == 0.0f && _13 == 0.0f && _14 == 0.0f &&
         _21 == 0.0f && _22 == 1.0f && _23 == 0.0f && _24 == 0.0f &&
         _31 == 0.0f && _32 == 0.0f && _33 == 1.0f && _34 == 0.0f &&
         _41 == 0.0f && _42 == 0.0f && _43 == 0.0f && _44 == 1.0f);       
        
}

void MATRIX::logOut() const {
 Log("%.4f  %.4f  %.4f  %.4f", _11, _12, _13, _14);
 Log("%.4f  %.4f  %.4f  %.4f", _21, _22, _23, _24);
 Log("%.4f  %.4f  %.4f  %.4f", _31, _32, _33, _34);
 Log("%.4f  %.4f  %.4f  %.4f", _41, _42, _43, _44);
}


/*
MATRIX Matrix;
  VECTOR View, Right,Up;
  
  View = BillboardPosition - CameraPosition;
  View.normalize();
  
  VECTOR worldUp = VECTOR(0,1,0);
  
  float angle =  worldUp%View; // dot product
  
  Up = worldUp - angle*View;
  Up.normalize();

  Right = Up^View; // cross product
  Right.normalize();

  Matrix._11 = Right.x; Matrix._21 = Up.x; Matrix._31 = View.x;
  Matrix._12 = Right.y; Matrix._22 = Up.y; Matrix._32 = View.y;
  Matrix._13 = Right.z; Matrix._23 = Up.z; Matrix._33 = View.z;

  Matrix._41=BillboardPosition.x;
  Matrix._42=BillboardPosition.y;
  Matrix._43=BillboardPosition.z;

  Matrix._14=0.0f;
  Matrix._24=0.0f;
  Matrix._34=0.0f;
  Matrix._44=1.0f;
  

*/

void MATRIX::setBillboard(const VECTOR& billboardPosition, const VECTOR& viewerPosition) {
  
  _14 = 0.0f; 
  _24 = 0.0f; 
  _34 = 0.0f; 
  _44 = 1.0f;
  
  VECTOR view = billboardPosition - viewerPosition;
  view.normalize();
  
  /* 
  // This always works - but is slower...
  // Extract theta and phi:
  float theta = acos(view.y);
  float phi   = 0.0;
  
  if (sin(theta) != 0.0) {
    phi = acos(-view.z/sin(theta));
    
    if (view.x/sin(theta) < 0)
      phi = -phi;
  }
    
  // Find camera up vector:
  up.x = sin(theta-PI/2)*sin(phi);
  up.y = cos(theta-PI/2);
  up.z = -sin(theta-PI/2)*cos(phi);
   */
   
  
  VECTOR worldUp = VECTOR(0,1,0);
  float angle =  worldUp%view; // dot product
  
  VECTOR up = worldUp - angle*view;
  up.normalize();
  
  // cross product - will give normalized result because up and view are perpendicular to each other
  VECTOR right = up^view; 
  
  _11 = right.x; _21 = up.x; _31 = view.x;
  _12 = right.y; _22 = up.y; _32 = view.y;
  _13 = right.z; _23 = up.z; _33 = view.z;

  _41 = billboardPosition.x;
  _42 = billboardPosition.y;
  _43 = billboardPosition.z;

}


    
// The BBOX class



bool BBOX::intersectSphere(const VECTOR& center, float radius) const {

 // if the spheres center is inside the box they intersect
 if ((center.x >= min.x) && (center.y >= min.y) && (center.z >= min.z) &&	
     (center.x <= max.x) && (center.y <= max.y) && (center.z <= max.z))	
   return true;

 // Not inside:
 float s = 0;
 float d = 0;
 
 for (int i=0; i<3; i++) {
   if (center[i] < min[i]) {
     s = center[i] - min[i];
     d += s*s;	
   }	
   else
   if (center[i] > max[i]) {
     s = center[i] - max[i];
     d += s*s;
   }     
 }
 return (d <= radius*radius);	
}


VECTOR BBOX::center() const {
  VECTOR diagonal = max - min;
  diagonal.x *= 0.5f;
  diagonal.y *= 0.5f;
  diagonal.z *= 0.5f;
  return (min + diagonal);      
}    


float BBOX::ProjectedSize(MATRIX &mat) {
 
 // Create full box
 VECTOR box[8];
 box[0] = min;
 box[1] = VECTOR(max.x, min.y, min.z);	
 box[2] = VECTOR(max.x, min.y, max.z);	
 box[3] = VECTOR(min.x, min.y, max.z);	

 box[4] = VECTOR(min.x, max.y, min.z);
 box[5] = VECTOR(max.x, max.y, min.z);	
 box[6] = max;	
 box[7] = VECTOR(min.x, max.y, max.z);	
  
  // Rotate box
  for (int i=0; i<8; i++)
    box[i] = mat * box[i]; // translate vector
    
        
 // Get rotated bbox
 VECTOR transmin = box[0];
 VECTOR transmax = box[0];
    
 for (int i=1; i<8; i++) {
   
   if (box[i].x < transmin.x) transmin.x = box[i].x;
   if (box[i].y < transmin.y) transmin.y = box[i].y;
   
   if (box[i].x > transmax.x) transmax.x = box[i].x;
   if (box[i].y > transmax.y) transmax.y = box[i].y;
  }	
 
 return ((transmax - transmin).length());       
        
} 

float BBOX::diagonalLength() {
  return (max-min).length();
}


double BBOX::volume() {
 return (double)(fabs(max.x-min.x)/100.0f)*(double)(fabs(max.y-min.y)/100.0f)*(double)(fabs(max.z-min.z)/100.0f);
}
 
void BBOX::merge(const BBOX& otherBox) {
  if (otherBox.max.x > max.x) max.x = otherBox.max.x;
  if (otherBox.max.y > max.y) max.y = otherBox.max.y;
  if (otherBox.max.z > max.z) max.z = otherBox.max.z;

  if (otherBox.min.x < min.x) min.x = otherBox.min.x;
  if (otherBox.min.y < min.y) min.y = otherBox.min.y;
  if (otherBox.min.z < min.z) min.z = otherBox.min.z;
}

// Make sure bbox also contains this point
void BBOX::addPoint(const VECTOR& p) {
  if (p.x > max.x) max.x = p.x;
  if (p.y > max.y) max.y = p.y;
  if (p.z > max.z) max.z = p.z;

  if (p.x < min.x) min.x = p.x;
  if (p.y < min.y) min.y = p.y;
  if (p.z < min.z) min.z = p.z;
}

BBOX BBOX::extrudeFromPointLight(const VECTOR& lightPos, float lightRange) {
  BBOX res = *this;

  // Extrude all points in bbox and add those to the bbox
  VECTOR extrudedBox[8];
  extrudedBox[0] = max;
  extrudedBox[1] = VECTOR(max.x, max.y, min.z);
  extrudedBox[2] = VECTOR(min.x, max.y, min.z);
  extrudedBox[3] = VECTOR(min.x, max.y, max.z);
  extrudedBox[4] = VECTOR(max.x, min.y, max.z);
  extrudedBox[5] = VECTOR(min.x, min.y, max.z);
  extrudedBox[6] = VECTOR(max.x, min.y, min.z);
  extrudedBox[7] = min;
  
  for (int i = 0; i < 8; i++) {
    VECTOR extrusion = extrudedBox[i] - lightPos;
    extrusion.SetLength(Max(lightRange - extrusion.length(), 0.0f));
    res.addPoint(extrudedBox[i] + extrusion);
  }
  
  return res;
}

BBOX BBOX::extrude(const VECTOR& extrusion) {
  BBOX res = *this;

  // Extrude all points in bbox and add those to the bbox
  VECTOR extrudedBox[8];
  extrudedBox[0] = max;
  extrudedBox[1] = VECTOR(max.x, max.y, min.z);
  extrudedBox[2] = VECTOR(min.x, max.y, min.z);
  extrudedBox[3] = VECTOR(min.x, max.y, max.z);
  extrudedBox[4] = VECTOR(max.x, min.y, max.z);
  extrudedBox[5] = VECTOR(min.x, min.y, max.z);
  extrudedBox[6] = VECTOR(max.x, min.y, min.z);
  extrudedBox[7] = min;
  
  for (int i = 0; i < 8; i++) 
    res.addPoint(extrudedBox[i] + extrusion);
    
  return res;
}


// Quaternion:

QUATERNION QUATERNION::operator*(const QUATERNION &q) const {
	
 QUATERNION res;

 res.s = s*q.s - x*q.x - y*q.y - z*q.z;
 res.x = s*q.x + x*q.s + y*q.z - z*q.y; 
 res.y = s*q.y + y*q.s + z*q.x - x*q.z;
 res.z = s*q.z + z*q.s + x*q.y - y*q.x; 

 return res;
}


// Build from axis/angle:
QUATERNION::QUATERNION(VECTOR axis, float angle) {
  
  VECTOR a = axis;
  a.normalize();

  s   =   (float)cos(angle/2);    
  x   =   a.x * (float)sin(angle/2);    
  y   =   a.y * (float)sin(angle/2);
  z   =   a.z * (float)sin(angle/2);
}



// Build from euler angles:
QUATERNION::QUATERNION(VECTOR angle) {
  
 float ang = (float)angle.z * 0.5f;
 float sy = (float)sin(ang);
 float cy = (float)cos(ang);
 
 ang = angle.y * 0.5f;
 float sp = (float)sin(ang);
 float cp = (float)cos(ang);
 
 ang = angle.x *0.5f;
 float sr = (float)sin(ang);
 float cr = (float)cos(ang);	
 
 x = sr * cp * cy - cr * sp * sy;  // X
 y = cr * sp * cy + sr * cp * sy;  // Y
 z = cr * cp * sy - sr * sp * cy;  // Z
 s = cr * cp * cy + sr * sp * sy;  // W	      
}



void MatToQuat(float m[4][4], QUATERNION &quat) {
  float  tr, s, q[4];
  int    i, j, k;

  int nxt[3] = {1, 2, 0};
  tr = m[0][0] + m[1][1] + m[2][2];

  // check the diagonal
  if (tr > 0.0) {
    s = sqrt (tr + 1.0);
    quat.s = s / 2.0;
    s = 0.5 / s;
    quat.x = (m[1][2] - m[2][1]) * s;
    quat.y = (m[2][0] - m[0][2]) * s;
    quat.z = (m[0][1] - m[1][0]) * s;
  } else {		
	   // diagonal is negative
     i = 0;
     if (m[1][1] > m[0][0]) i = 1;
	   if (m[2][2] > m[i][i]) i = 2;

     // i gives column nr.
     j = nxt[i];
     k = nxt[j];

     s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
     q[i] = s * 0.5;
            
     if (s != 0.0) s = 0.5 / s;
 
     q[3] = (m[j][k] - m[k][j]) * s;
     q[j] = (m[i][j] + m[j][i]) * s;
     q[k] = (m[i][k] + m[k][i]) * s;

	   quat.x = q[0];
	   quat.y = q[1];
	   quat.z = q[2];
	   quat.s = q[3];
  }
}

QUATERNION::QUATERNION(const MATRIX &mat) {
  
  float m[4][4];
  m[0][0] = mat._11;
  m[0][1] = mat._12;
  m[0][2] = mat._13;
  m[0][3] = mat._14;

  m[1][0] = mat._21;
  m[1][1] = mat._22;
  m[1][2] = mat._23;
  m[1][3] = mat._24;

  m[2][0] = mat._31;
  m[2][1] = mat._32;
  m[2][2] = mat._33;
  m[2][3] = mat._34;

  m[3][0] = mat._41;
  m[3][1] = mat._42;
  m[3][2] = mat._43;
  m[3][3] = mat._44;

  QUATERNION res;
  MatToQuat(m, res);

  x = res.x;
  y = res.y;
  z = res.z;
  s = res.s;
}

/*
QUATERNION::QUATERNION(const MATRIX &mat) {
  float  tr, w;

  tr = mat._11 + mat._22 + mat._33;

  // check the diagonal
  if (tr > 0.0) {
    w = sqrt (tr + 1.0);
    s = w / 2.0;

    w = 0.5 / w;
    x = (mat._23 - mat._32) * s;
    y = (mat._31 - mat._13) * s;
    z = (mat._12 - mat._21) * s;
  } else {
    // diagonal is negative
    if (mat._11 > mat._22 && mat._11 > mat._33) {
    // Column 1:
    // i = 0, j = 1, k = 2
      w = sqrt ((mat._11 - (mat._22 + mat._33)) + 1.0);
      x = w * 0.5;

      if (w != 0.0)
        w = 0.5 / w;

      s = (mat._23 - mat._32) * w;
      y = (mat._12 + mat._21) * w;
      z = (mat._13 + mat._31) * w;
    } else {
      if (mat._22 > mat._33) {
      // Column 2:
      // i = 1, j = 2, k = 0
        w = sqrt ((mat._22 - (mat._33 + mat._11)) + 1.0);
        y = w * 0.5;

        if (w != 0.0)
          w = 0.5 / w;

        s = (mat._31 - mat._13) * w;
        z = (mat._23 + mat._32) * w;
        x = (mat._21 + mat._12) * w;
      } else {
      // Column 3:
      // i = 2, j = 0, k = 1
        w = sqrt ((mat._33 - (mat._11 + mat._22)) + 1.0);
        z = w * 0.5;

        if (w != 0.0)
          w = 0.5 / w;

        s = (mat._12 - mat._21) * w;
        x = (mat._31 + mat._13) * w;
        z = (mat._32 + mat._23) * w;
      }
    }
  }
}
*/


MATRIX QUATERNION::toMatrix() {
        
 MATRIX res;
 
 res._11 = 1.0f - 2.0f*y*y - 2.0f*z*z;
 res._12 = 2.0f*x*y + 2.0f*s*z;
 res._13 = 2.0f*x*z - 2.0f*s*y;
 res._14 = 0.0f;
 
 res._21 = 2.0f*x*y - 2.0f*s*z;
 res._22 = 1.0f - 2.0f*x*x - 2.0f*z*z;
 res._23 = 2.0f*y*z + 2.0f*s*x;
 res._24 = 0.0f;
 
 res._31 = 2.0f*x*z + 2.0f*s*y;
 res._32 = 2.0f*y*z - 2.0f*s*x;
 res._33 = 1.0f - 2.0f*x*x - 2.0f*y*y;       
 res._34 = 0.0f;
 
 res._41 = 0.0f;
 res._42 = 0.0f;
 res._43 = 0.0f;
 res._44 = 1.0f;
 
 return res;       
}



/*

VOID D3DMath_QuaternionFromMatrix( FLOAT& x, FLOAT& y, FLOAT& z, FLOAT& w,
                                   D3DMATRIX& mat )
{
    if( mat._11 + mat._22 + mat._33 > 0.0f )
    {
        FLOAT s = sqrtf( mat._11 + mat._22 + mat._33 + mat._44 );

        x = (mat._23-mat._32) / (2*s);
        y = (mat._31-mat._13) / (2*s);
        z = (mat._12-mat._21) / (2*s);
        w = 0.5f * s;
    }
    else
    {


    }
    FLOAT xx = x*x; FLOAT yy = y*y; FLOAT zz = z*z;
    FLOAT xy = x*y; FLOAT xz = x*z; FLOAT yz = y*z;
    FLOAT wx = w*x; FLOAT wy = w*y; FLOAT wz = w*z;
    
    mat._11 = 1 - 2 * ( yy + zz ); 
    mat._12 =     2 * ( xy - wz );
    mat._13 =     2 * ( xz + wy );

    mat._21 =     2 * ( xy + wz );
    mat._22 = 1 - 2 * ( xx + zz );
    mat._23 =     2 * ( yz - wx );

    mat._31 =     2 * ( xz - wy );
    mat._32 =     2 * ( yz + wx );
    mat._33 = 1 - 2 * ( xx + yy );

    mat._14 = mat._24 = mat._34 = 0.0f;
    mat._41 = mat._42 = mat._43 = 0.0f;
    mat._44 = 1.0f;
}
*/


// Use SLERP to calculate a quarternion between 'this' and 'to'.
// Note : The quaternions should be of unit length
QUATERNION QUATERNION::Slerp(QUATERNION& to, float t) {
   
   QUATERNION res;
   
   float to1[4];
   double omega, cosom, sinom, scale0, scale1;        
   
   // calc cosine
   cosom = x*to.x + y*to.y + z*to.z + s*to.s;        
   
   // adjust signs (if necessary)
   if ( cosom < 0.0 ) { 
   	 cosom = -cosom; 
   	 to1[0] = -to.x;
   	 to1[1] = -to.y;
   	 to1[2] = -to.z;
   	 to1[3] = -to.s;
   } else  {
   	 to1[0] = to.x;
	   to1[1] = to.y;
	   to1[2] = to.z;
	   to1[3] = to.s;
   }
        
        
   // calculate coefficients
   if ( (1.0 - cosom) > EPSILON ) {
     // standard case (slerp)
     omega = (float)acos(cosom);
     sinom = (float)sin(omega);
     scale0 = (float)sin((1.0 - t) * omega) / sinom;
     scale1 = (float)sin(t * omega) / sinom;
   } else {        
     // "from" and "to" quaternions are very close so we can do a linear interpolation
     scale0 = 1.0 - t;
     scale1 = t;
   }

   // calculate final values
   res.x = scale0 * x + scale1 * to1[0];
   res.y = scale0 * y + scale1 * to1[1];
   res.z = scale0 * z + scale1 * to1[2];
   res.s = scale0 * s + scale1 * to1[3];
   
   return res;   
}
  
  
 
 
// Misc. 
 
// get around problems with order of arguments     
VECTOR operator*(float f, const VECTOR &v) {
  return v*f;
}

COLOR operator*(float f, const COLOR &c) {
  return c*f;
}


void MATRIX::makeUnitTripod()
{
  float len = sqrt(_11*_11 + _21*_21 + _31*_31);
  if (len > 0.0f) {
    _11 /= len;
    _21 /= len;
    _31 /= len;
  }
  
  len = sqrt(_12*_12 + _22*_22 + _32*_32);
  if (len > 0.0f) {
    _12 /= len;
    _22 /= len;
    _32 /= len;
  }
  
  len = sqrt(_13*_13 + _23*_23 + _33*_33);
  if (len > 0.0f) {
    _13 /= len;
    _23 /= len;
    _33 /= len;
  }
}

void MATRIX::setX(VECTOR vector)
{
  _11 = vector[0];
  _12 = vector[1];
  _13 = vector[2];
}

void MATRIX::setY(VECTOR vector)
{
  _21 = vector[0];
  _22 = vector[1];
  _23 = vector[2];
}

void MATRIX::setZ(VECTOR vector)
{
  _31 = vector[0];
  _32 = vector[1];
  _33 = vector[2];
}

void MATRIX::setPosition(VECTOR pos)
{
  _41 = pos.x;
  _42 = pos.y;
  _43 = pos.z;
}



// default constructor - the XZ plane
PLANE::PLANE() {
  normal = VECTOR(0,1,0);
  origin = VECTOR(0,0,0);
  equation[0] = 0; 
  equation[1] = 1; 
  equation[2] = 0; 
  equation[3] = 0;
}

PLANE::PLANE(const VECTOR& origin, const VECTOR& normal) {
  this->normal = normal;
  this->origin = origin;
  equation[0] = normal.x;
  equation[1] = normal.y;
  equation[2] = normal.z;
  equation[3] = -(normal.x*origin.x + normal.y*origin.y + normal.z*origin.z); // dot product
}

bool PLANE::isFrontFacingTo(const VECTOR& direction) const {
  double dot = normal%direction;
  return (dot <= 0);
}

double PLANE::signedDistanceTo(const VECTOR& point) const {
  return (point%normal) + equation[3];
}

void PLANE::getBasisVectors(VECTOR* outU, VECTOR* outV) const {
  VECTOR perpendicular = normal.getPerpendicular();
  *outU = perpendicular;
	*outV = normal^perpendicular;
}

POINT2D PLANE::projectToPlane(const VECTOR& point) const {
  VECTOR u,v;
  getBasisVectors(&u,&v);

  return POINT2D(u%point, v%point); 
}


PointPlaneRelation PLANE::classifyPoint(const VECTOR& point) const {
  double d = (point - origin)%normal;
 
  if (d > EPSILON)
    return PLANE_FRONT;	
 
  if (d < -EPSILON)
    return PLANE_BACK;	

  return PLANE_ON;	
}


RAY::RAY(const VECTOR& origin, const VECTOR& dir) : origin(origin), dir(dir) {}


bool RAY::intersectsPlane(const PLANE& p, double* distanceToPlane) {
  double a = p.normal % dir; 
     
  // if ray is parrallel to the plane: 
  if (fabs(a) < EPSILON) return false;
   
  double b = -(p.normal % origin + p.equation[3]);
  double t = b/a;
  if (t < 0) return false;
  
  *distanceToPlane = t;
  return true;
}


bool RAY::intersectsBBox(const BBOX& box) const {
 const int RIGHT=0;
 const int LEFT=1;
 const int MIDDLE=2;

 bool inside = true;
 char quadrant[3];
 int whichPlane;
 double maxT[3];
 double candidatePlane[3];
 
 // Find candidate planes
 int i;
 for (i=0; i<3; i++) {
  if (origin[i] < box.min[i]) {
    quadrant[i] = LEFT;	
    candidatePlane[i] = box.min[i];
    inside = false;
  }
  else if (origin[i] > box.max[i]) {
    quadrant[i] = RIGHT;	
    candidatePlane[i] = box.max[i];
    inside = false;
  }  	
  else {
    quadrant[i] = MIDDLE;	
  }    
 }
  
 // Ray origin inside bounding box 
 if(inside) 
   return (true);

 
 // Calculate T distances to candidate planes 
 for (i=0; i<3; i++) {
  if (quadrant[i] != MIDDLE && dir[i] != 0.0f)
    maxT[i] = (candidatePlane[i]-origin[i]) / dir[i];
  else 
    maxT[i] = -1.0f;    
 }
 
    
 // Get largest of the maxT's for final choice of intersection    
 whichPlane = 0;
 
 //X 
 if (maxT[whichPlane] < maxT[0])   
  whichPlane = 0;
	
 //Y 
 if (maxT[whichPlane] < maxT[1])   
  whichPlane = 1;
  
 //Z 
 if (maxT[whichPlane] < maxT[2])   
  whichPlane = 2; 	
	
	
 // Check final candidate actually inside box 
 if (maxT[whichPlane] < 0.0f) return (false);

 for (i=0; i<3; i++) {
  if (whichPlane != i) {
   if ((origin[i] + maxT[whichPlane] * dir[i]) < box.min[i] || (origin[i] + maxT[whichPlane] * dir[i]) > box.max[i])
     return false;
  }	     
 }
  	
 // Ray hits box
 return true;	
}






