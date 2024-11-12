#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  int i = 0;
  while (src[i] != '\0') {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  int i = 0;
  while (src[i] != '\0' && i < n) {
    dst[i] = src[i];
    i++;
  }
  dst[i] = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  int i = 0;
  while (dst[i] != '\0') {
    i++;
  }
  int j = 0;
  while (src[j] != '\0') {
    dst[i] = src[j];
    i++;
    j++;
  }
  dst[i] = '\0';
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  int i = 0;
  while (s1[i] != '\0' && s2[i] != '\0') {
    if (s1[i] < s2[i]) {
      return -1;
    } else if (s1[i] > s2[i]) {
      return 1;
    }
    i++;
  }
  if (s1[i] == '\0' && s2[i] == '\0') return 0;
  else if (s1[i] == '\0') return -1;
  else return 1;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  int i = 0;
  while (s1[i] != '\0' && s2[i] != '\0' && i < n) {
    if (s1[i] < s2[i]) return -1;
    else if (s1[i] > s2[i]) return 1;
    i++;
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  char *p = (char *)v;
  for (int i = 0; i < n; i++) {
    p[i] = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *p = (char *)out;
  char *q = (char *)in;
  for (int i = 0; i < n; i++) {
    p[i] = q[i];
  }
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
  char *p = (char *)s1;
  char *q = (char *)s2;
  for (int i = 0; i < n; i++) {
    if (p[i] < q[i]) return -1;
    else if (p[i] > q[i]) return 1;
  }
  return 0;
}

#endif
