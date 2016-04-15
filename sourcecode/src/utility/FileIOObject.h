#ifndef _FILEIOOBJECT_H_
#define _FILEIOOBJECT_H_

// --** Includes: **---------------------------------------------------
#include <stdio.h>
#include "math/primitives.h"

typedef enum {OPEN_READ, OPEN_WRITE} OpenMode;

// --** Classes: **----------------------------------------------------
class FileIOObject {
protected:

  void initialize(bool warnFailure);

  bool valid;         // is the file there?
  bool eof;           // is the file pointer currently EOF
  DWORD fileLength;
  FILE *f;           
  string filename;
  const OpenMode mode;

  DWORD bufferPosition;
  BYTE *buffer;

 public:
  
  FileIOObject(const char *fn, const OpenMode& mode = OPEN_READ, bool warnFailure = true);
  FileIOObject(const string& fn, const OpenMode& mode = OPEN_READ, bool warnFailure = true);
  ~FileIOObject();

	static string changeFilenameToConvention(const string& filename);

  bool isValid() const { return(valid); }
  bool isEOF() const { return(eof); }
  bool isReadyForWrite() const { return (mode == OPEN_WRITE && !eof); }
	
  DWORD getFileLength() const;
  void  setAbsoluteFilePos(DWORD pos);
  DWORD getAbsoluteFilePos() const { return bufferPosition; }
  
  const string& getFilename() const { return filename; }

  // Input:
  char *getStringZ();
  string getString();
  char *getSymbol();
  char *getStringSymbol();
  float getFloat();
  int   getInt();
  BYTE  getByte();
  char  getChar();
  short getShortInt();
  bool getBool();
  BYTE* getByteArray(int length);
	const BYTE* const getDataPointer() const { return &buffer[bufferPosition]; };
  VECTOR getVector();
  MATRIX getMatrix();
        
  // Output:
  void writeByte(BYTE b);
  void writeStringZ(const char *str);
  void writeString(const string &str);
  void writeShortInt(short i);
  void writeFloat(float val);
  void writeInt(int i);
  void writeBool(bool b);
  void writeVector(const VECTOR& v);
  void writeMatrix(const MATRIX& m);
};



#endif

