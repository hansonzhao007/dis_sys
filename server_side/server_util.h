#ifndef SERVER_UTIL_H__
#define SERVER_UTIL_H__

#include <sys/socket.h>                                        
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <fcntl.h>

enum ClientMsgType {
	kError = -1,
  kUpload = 0,
  kRename,
  kDelete,
  kDownld
};

class FileServerUtil {
public:
  FileServerUtil(){};
  ~FileServerUtil(){};

  // Implementations of file server util
  int ReName(const char* old_name, const char* new_name);
  int Delete(const char* file_name);
  int Create(const char* file_name, const char* file_data, uint64_t size);
  ClientMsgType MsgType(const char* msg);
};

#endif
