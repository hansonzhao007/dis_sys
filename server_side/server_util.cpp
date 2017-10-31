/*
 * Author: Xingsheng Zhao
 * Purpose: File server operations
 * Data: Sep. 26th, 2017
 */
#include "server_util.h"

int FileServerUtil::ReName(const char* old_name, const char* new_name) {
  return rename(old_name, new_name);
}

int FileServerUtil::Delete(const char* file_name) {
  // On success, zero is returned.  On error, -1 is returned, and errno is set  appropriately
  return remove(file_name);
}

int FileServerUtil::Create(const char* file_name, const char* file_data, uint64_t size) {
  int fd = open(file_name, O_CREAT | O_RDWR |O_TRUNC);
  if (fd != -1) {
    close(fd);
  }
  
  return fd;
}
ClientMsgType FileServerUtil::MsgType(const char* msg) {
  if (strcmp(msg, "upload") == 0) return kUpload;
  else if (strcmp(msg, "rename") == 0) return kRename;
  else if (strcmp(msg, "delete") == 0) return kDelete;
  else if (strcmp(msg, "downld") == 0) return kDownld;
	return kError;
}

