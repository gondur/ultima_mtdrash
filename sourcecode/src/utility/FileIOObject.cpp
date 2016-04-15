// --** Includes: **---------------------------------------------------
#include <stdio.h>
#include "FileIOObject.h"
#include "utility/Logger.h"

// --** Globals: **----------------------------------------------------
int openFiles = 0;

// This controls whether an uppercase in a filename results in a warning
const bool LOG_ON_FILENAME_CONVENTION = false;
const bool MESSAGE_BOX_ON_FILENAME_CONVENTION = false;

// --** Static Methods: **---------------------------------------------

void changePathSeparator(string* s)
{
  // Change '\' to '/'
  for (int i = 0; i < s->size(); i++) {
    if ((*s)[i] == '\\')
      (*s)[i] = '/';
  }
}

FileIOObject::FileIOObject(char const *fn, const OpenMode& mode, bool warnFailure) 
  : filename(*(new string(fn))), mode(mode) {

  initialize(warnFailure);
}

FileIOObject::FileIOObject(const string& fn, const OpenMode& mode, bool warnFailure) 
  : filename(fn), mode(mode) {

  initialize(warnFailure);
}

void FileIOObject::initialize(bool warnFailure) {

  valid = false;
  eof = false;
  fileLength = 0;
  f = NULL;
  buffer = NULL;
  bufferPosition = 0;

  // Check that filename != ""
  if (filename == "")
    return;

	filename = changeFilenameToConvention(filename);

	// TODO debug
  
  //string fullPath = string(settings->engine.basepath) + filename;
  
  string fullPath = filename;
  changePathSeparator(&fullPath);
   
  // open file based on mode
  switch(mode) {
  case OPEN_READ:{
    //Log("Trying to open file '%s' for reading ...", filename.c_str());
    
    if ((f = fopen(filename.c_str(),"rb")) == NULL) {
      //Log("Trying to open file '%s'...", fullPath.c_str());
      
      if ((f = fopen(fullPath.c_str(),"rb")) == NULL) {

				if (warnFailure)
					Log("ERROR: FileIOObject couldn't open file '%s' ", fullPath.c_str());

				return;
      }
    }
    
    // Read entire file into buffer
    fseek(f,0L, SEEK_END);
    fileLength = ftell(f);

    if (fileLength == 0) {
				if (warnFailure)
					Log("ERROR: FileIOObject couldn't open file '%s' ", fullPath.c_str());
      
          fclose(f);

        return;
    }

    fseek(f,0, SEEK_SET);	
 
    buffer = new BYTE[fileLength];

    int read = fread(buffer, fileLength, 1, f);
    if (read != 1){
      Log("ERROR: FileIOObject couldn't read contents of file '%s' into buffer", fullPath.c_str());
      delete[] buffer;
      fclose(f);
      return;
    }

    fclose(f); 
    f = NULL;
    break;
  }
  case OPEN_WRITE:{
    //Log("Trying to open file '%s' for writing ...", filename.c_str());
    if ((f = fopen(filename.c_str(),"wb")) == NULL) {
      // use full path:
      if ((f = fopen(fullPath.c_str(),"wb")) == NULL) {
				if (warnFailure)
					Log("ERROR: FileIOObject couldn't open file '%s' ", fullPath.c_str());
				return;
      }
    }
    // count number of open files
    openFiles++;

    break;
  }
  }        

  valid = true;
}

FileIOObject::~FileIOObject() {
  
  if (f) {
    openFiles--;
    fclose(f);
  }
 
  if (buffer) {
    delete buffer;
    buffer = NULL;
  }

  //Log("closed file %s - open files are now %i", filename.c_str(), openFiles);
}

string FileIOObject::changeFilenameToConvention(const string& filename)
{
	string result(filename);

	// Make filename all lowercase and warn if it wasn't so already
	bool containsUpperCase = false;
	for (int i = 0; i < result.size(); i++) {
		if (isupper(result[i])) {
			result[i] = tolower(result[i]);
			containsUpperCase = true;
		} 
	}
 	if (containsUpperCase && LOG_ON_FILENAME_CONVENTION)
		Log("WARNING: FileIOObject::changeFilenameToConvention: '%s' contains uppercase characters", filename.c_str());

 	if (containsUpperCase && MESSAGE_BOX_ON_FILENAME_CONVENTION) {
		string message = "File '" + filename + "' contains uppercase characters";
		Log("Filename-convention violation: %s", message.c_str());
	}

  changePathSeparator(&result);

	return result;
}

DWORD FileIOObject::getFileLength() const {

  return fileLength;
}

void FileIOObject::setAbsoluteFilePos(DWORD pos) {
  if (mode == OPEN_WRITE)
    return;
  
  bufferPosition = pos;
  
  if (bufferPosition < fileLength) {
    eof = false; 
  }
}

// --** Public methods **-----------------------------------------------------

// read a zero terminated string from a binary file.
char *FileIOObject::getStringZ() {

  if (mode == OPEN_WRITE || eof)
    return NULL;
   
  // Find length of ZString:
  int length = 0;
  while (bufferPosition + length < fileLength  && buffer[bufferPosition + length] != 0)
    length++;

  if (length == 0) {// the empty string
   bufferPosition++;
   return NULL;
  }
  // allocate space:
  char *res = new char[length+1];

  // copy from buffer (this includes zero terminate):
  memcpy(res, &(buffer[bufferPosition]), length);
  res[length] = 0;
  
  // update buffer pointer
  bufferPosition += (length+1);

  if (bufferPosition >= fileLength)
    eof = true;

  // return result:
  return res;
}


string FileIOObject::getString() {
  char *str = getStringZ();
  if (str == NULL)
    return string("");

  return string(str);
}


// Get one 'word' from an ascii file:
char *FileIOObject::getSymbol() {

  if (mode == OPEN_WRITE || eof)
    return NULL;
  
  // run past initial white-space:
  char temp;
  do {
    temp = buffer[bufferPosition];
    
    if (temp==0x0a || temp==0x0d || temp==' ' || temp==9)
      bufferPosition++;
     
  } while(bufferPosition < fileLength && (temp==0x0a || temp==0x0d || temp==' ' || temp==9));
  
   
  // Check EOF
  if (bufferPosition >= fileLength) {
    eof = true;
    return NULL;
  }
	
		
  // Find size of symbol
  int size = 0;
  
  temp = buffer[bufferPosition + 0];
  while((bufferPosition + size) < fileLength && temp!=0x0a && temp!=0x0d && temp!=' ' && temp!=9){
    size++;
    temp = buffer[bufferPosition + size]; // Fetch next character.
  }
  
  char* symbol = new char[size + 1];
  memcpy(symbol, &buffer[bufferPosition], size);
  symbol[size] = 0;

  bufferPosition += size;

  // Check EOF
  if (bufferPosition >= fileLength) {
    eof = true;
  }
	
  return symbol;
}

// Read string enclosed by quotes ('"').
char * FileIOObject::getStringSymbol() {

  if (mode == OPEN_WRITE || eof)
    return NULL;
  
  // run past initial white-space:
  char temp;
  do {
    temp = buffer[bufferPosition];
  
    if (temp==0x0a || temp==0x0d || temp==' ' || temp==9)
     bufferPosition++;
  
  } while(bufferPosition < fileLength && (temp==0x0a || temp==0x0d || temp==' ' || temp==9));
    
  // Check for quote:
  if (buffer[bufferPosition] != '"') {
    return NULL;
  }
  bufferPosition++;
  
  // Check EOF
  if (bufferPosition >= fileLength) {
    eof = true;
    return NULL;
  }
		
  // Find size of symbol
  int size = 0;
  
  temp = buffer[bufferPosition + 0];
  while((bufferPosition + size) < fileLength && temp!='"')
  {
    size++;
    temp = buffer[bufferPosition + size]; // Fetch next character.
  }
  
  // Check for quote:
  if (buffer[bufferPosition + size] != '"') {
    return NULL;
  }
  
  // Copy symbol:
  char* symbol = new char[size + 1];
  memcpy(symbol, &buffer[bufferPosition], size);
  symbol[size] = 0;
  
  bufferPosition += size + 1;

  // Check EOF
  if (bufferPosition >= fileLength) {
    eof = true;
  }
 
  return symbol;
}
               
float FileIOObject::getFloat() {

  if (mode == OPEN_WRITE || bufferPosition+4 > fileLength)
    return -1;
  
  // build a float from buffer 
  float res;
  
#ifdef __BIG_ENDIAN__
  // SGI  
  union {
    float floatValue;
    byte byteValue[4];
  } temp;
  
  for(int i = 0; i < 4; i++)
    temp.byteValue[3-i] = buffer[bufferPosition + i];
  
  res = temp.floatValue;
  
#else
  // WINDOWS & LINUX
  memcpy(&res, &buffer[bufferPosition], 4);
#endif
  
  bufferPosition += 4;
  
  // Check EOF
  if (bufferPosition >= fileLength) 
    eof = true;
  
  return res;
}

int FileIOObject::getInt() {
 
  if (mode == OPEN_WRITE || bufferPosition+4 > fileLength)
    return -1;
  
  // build a float from buffer 
  int res;
  
#ifdef __BIG_ENDIAN__
  // SGI  
  union {
    int intValue;
    byte byteValue[4];
  } temp;
  
  for(int i = 0; i < 4; i++)
    temp.byteValue[3-i] = buffer[bufferPosition + i];
  
  res = temp.intValue;
  
#else
  // WINDOWS & LINUX
  memcpy(&res, &buffer[bufferPosition], 4);
#endif
  
  bufferPosition += 4;

  // Check EOF
  if (bufferPosition >= fileLength) 
    eof = true;
  
  return res;
}

BYTE FileIOObject::getByte() {
 
  if ((mode == OPEN_WRITE) || eof)
    return 0;
 
  BYTE res = buffer[bufferPosition++];

  // Check EOF
  if (bufferPosition >= fileLength) 
    eof = true;
  
  return res;
}
	
char FileIOObject::getChar() {
 
  if ((mode == OPEN_WRITE) || eof)
    return 0;
 
  char res;
  memcpy(&res, &buffer[bufferPosition], 1);
  bufferPosition += 1;
 
  // Check EOF
  if (bufferPosition >= fileLength) 
    eof = true;
  
  return res;
}	
	
	
short FileIOObject::getShortInt() {

  if (mode == OPEN_WRITE) {
    Log("mode is write");
    return -1;
  }


  if (mode == OPEN_WRITE || bufferPosition+2 > fileLength) {
    Log("getShortInt returned -1");
    return -1;
  }

  // build a float from buffer 
  short res;
  
#ifdef __BIG_ENDIAN__
  // SGI  
  union {
    short intValue;
    byte byteValue[2];
  } temp;
  
  for(int i = 0; i < 2; i++)
    temp.byteValue[1-i] = buffer[bufferPosition + i];
  
  res = temp.intValue;
  
#else
  // WINDOWS & LINUX
  memcpy(&res, &buffer[bufferPosition], 2);
#endif
  
  bufferPosition += 2;

  // Check EOF
  if (bufferPosition >= fileLength) 
    eof = true;
  
  return res;
}

bool FileIOObject::getBool() {
  
  if (mode == OPEN_WRITE || bufferPosition+1 > fileLength) {
    Log("Error in getBool - will return 0");
    if (bufferPosition+1 >= fileLength) 
      Log("reading would extend buffer - filesize is %i", fileLength);
    else
      Log("File is opened for write only");  
    return 0;
  }
  
  // build a float from buffer 
  bool res;
  
#ifdef __BIG_ENDIAN__
  // SGI  
  union {
    bool boolValue;
    byte byteValue[1];
  } temp;
  
  for(int i = 0; i < 1; i++)
    temp.byteValue[0-i] = buffer[bufferPosition + i];
  
  res = temp.boolValue;
  
#else
  // WINDOWS & LINUX
  memcpy(&res, &buffer[bufferPosition], 1);
#endif
  
  bufferPosition += 1;

  // Check EOF
  if (bufferPosition >= fileLength) 
    eof = true;
  
  return res;
}	

BYTE* FileIOObject::getByteArray(int length)
{
  if ((mode == OPEN_WRITE) || (bufferPosition + length > fileLength))
    return NULL;
  
  BYTE* res = new BYTE[length];
  memcpy(res, &buffer[bufferPosition], length);
  
  bufferPosition += length;

  return res;
}

VECTOR FileIOObject::getVector()
{
  VECTOR result;
  
  result.x = getFloat();
  result.y = getFloat();
  result.z = getFloat();
  
  return result;
}

MATRIX FileIOObject::getMatrix()
{
  MATRIX result;
  
  result._11 = getFloat();
  result._12 = getFloat();
  result._13 = getFloat();
  result._14 = getFloat();
  
  result._21 = getFloat();
  result._22 = getFloat();
  result._23 = getFloat();
  result._24 = getFloat();
  
  result._31 = getFloat();
  result._32 = getFloat();
  result._33 = getFloat();
  result._34 = getFloat();
  
  result._41 = getFloat();
  result._42 = getFloat();
  result._43 = getFloat();
  result._44 = getFloat();
  
  return result;
}

	
/* OUTPUT */

void FileIOObject::writeByte(unsigned char b) {
  fwrite(&b, 1,1, f);       
}


// A NULL string is saved as the empty string
void FileIOObject::writeStringZ(const char *str) {
  
  const char *s = str;
  
  char singlechar;
  while(s && *s) {
    singlechar = *s++;
    fwrite(&singlechar, 1,1,f);
  }  
  // Write trailing zero:
  singlechar = 0;
  fwrite(&singlechar, 1,1,f);
}	

void FileIOObject::writeString(const string &str) {
  writeStringZ(str.c_str());
}


void FileIOObject::writeShortInt(short int i) {
  fwrite(&i, sizeof(short int), 1, f);       
}	


void FileIOObject::writeFloat(float val) {
  fwrite(&val, sizeof(float),1,f);       
}

void FileIOObject::writeInt(int i) {
  fwrite(&i, sizeof(int), 1, f);       
}

void FileIOObject::writeBool(bool b) {
  fwrite(&b, sizeof(bool), 1, f);       
}

void FileIOObject::writeVector(const VECTOR& v)
{
  writeFloat(v.x);
  writeFloat(v.y);
  writeFloat(v.z);
}

void FileIOObject::writeMatrix(const MATRIX& m)
{
  writeFloat(m._11);
  writeFloat(m._12);
  writeFloat(m._13);
  writeFloat(m._14);

  writeFloat(m._21);
  writeFloat(m._22);
  writeFloat(m._23);
  writeFloat(m._24);

  writeFloat(m._31);
  writeFloat(m._32);
  writeFloat(m._33);
  writeFloat(m._34);

  writeFloat(m._41);
  writeFloat(m._42);
  writeFloat(m._43);
  writeFloat(m._44);
}

