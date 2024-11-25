#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t ramdisk_read(void*, size_t, size_t);  
extern size_t ramdisk_write(const void*, size_t, size_t); 

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

// PA3.3 updated
int fs_open(const char *pathname, int flags, int mode){
  // printf("open %s\n", pathname);
  for(int i = 0; i < NR_FILES;i++){
    if(strcmp(pathname, file_table[i].name) == 0){
        // printf("open %s OK\n", pathname);
        return i;
    }
  }
  assert(0 && "Can't find file");
}

size_t fs_read(int fd, void *buf, size_t len) {  
//   printf("open_offset = %d, disk_offset = %d\n", file_table[fd].open_offset, file_table[fd].disk_offset);
  assert(fd >= 3 && fd < NR_FILES);
  if (file_table[fd].open_offset + len >= file_table[fd].size) {
    if (file_table[fd].size > file_table[fd].open_offset)
      len = file_table[fd].size - file_table[fd].open_offset;
    else
      len = 0;
  }
//   printf("read %d bytes\n", len);
  if (!file_table[fd].read) {
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  }
  else {
    len = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += len;
//   printf("open_offset = %d, disk_offset = %d\n", file_table[fd].open_offset, file_table[fd].disk_offset);
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  // printf("open_offset = %d, disk_offset = %d\n", file_table[fd].open_offset, file_table[fd].disk_offset);
  if(fd == 1 || fd == 2) {
    for(int i = 0; i < len; i++) {
      _putc(((char*)buf)[i]);
    }
    return len;
  }
  if (fd >= 3 && (file_table[fd].open_offset + len > file_table[fd].size)) {
    if (file_table[fd].size > file_table[fd].open_offset)
      len = file_table[fd].size - file_table[fd].open_offset;
    else
      len = 0;
  }
  // printf("write %d bytes\n", len);
  if (!file_table[fd].write) {
    ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  }
  else {
    file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  }
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  // printf("lseek %d %d %d\n", fd, offset, whence);
  assert(fd >= 3 && fd < NR_FILES);
  switch(whence) {
    case SEEK_SET: file_table[fd].open_offset = offset; break;
    case SEEK_CUR: file_table[fd].open_offset += offset; break;
    case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
    default: assert(0 && "Invalid whence");
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd){
    file_table[fd].open_offset = 0;
    return 0;
}