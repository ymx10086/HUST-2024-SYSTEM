#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


void add_string(char **s, char *str);
void add_char(char **s, char c);
void add_number(char **s, int num,char mode);
void add_special_number(char **s, const char *fmt, va_list ap,char c);

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  char buf[1000];
  int length=vsprintf(buf,fmt,ap);
  buf[length]='\0';
  for(int i=0;i<length;i++){
    _putc(buf[i]);
  }
  va_end(ap);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *temp=out;
  while(*fmt!='\0'){
    if(*fmt=='%'){
      fmt++;
      switch(*fmt){
        case 's':{
          char *str=va_arg(ap,char*);
          add_string(&temp,str);
          break;
        }
        case 'd':{
          int num=va_arg(ap,int);
          add_number(&temp,num,'d');
          break;
        }
        case 'x':{
          int num=va_arg(ap,int);
          add_number(&temp,num,'x');
          break;
        }
        case '0':{
          fmt++;
          add_special_number(&temp,fmt,ap,'0');
          fmt++;
          break;
        }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':{
          add_special_number(&temp,fmt,ap,' ');
          fmt++;
          break;
        }
        case 'c':{
          char c=va_arg(ap,int);
          add_char(&temp,c);
          break;
        }
      }
      fmt++;
    }
    else{
      *temp++=*fmt++;
    }
  }
  *temp='\0';
  return temp-out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  int length=vsprintf(out,fmt,ap);
  va_end(ap);
  return length;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}


void add_string(char **s, char *str) {
  while (*str!='\0') {
    **s = *str;
    (*s)++;
    str++;
  }
}

void add_char(char **s, char c) {
  **s = c;
  (*s)++;
}

void add_number(char **s, int num,char mode) {
  if (num == 0) {
    add_char(s, '0');
    return;
  }
  char temp[100];
  int i = 0;
  if(num<0){
    if(mode == 'd')
      add_char(s,'-');
    num=-num;
  }
  if(mode=='d'){
    while (num) {
      temp[i] = num % 10 + '0';
      num /= 10;
      i++;
    }
    while (i) {
      add_char(s, temp[i - 1]);
      i--;
    }
  }
  else if(mode=='x'){
    add_string(s,"0x");
    while (num) {
      temp[i] = num % 16 + (num % 16 < 10 ? '0' : 'a' - 10);
      num /= 16;
      i++;
    }
    while (i) {
      add_char(s, temp[i - 1]);
      i--;
    }
  }
}


void add_special_number(char **s, const char *fmt, va_list ap,char c) {
  int num_of_digit=0;
  while(*fmt!='d'&&*fmt!='x'){
    num_of_digit=num_of_digit*10+*fmt-'0';
    fmt++;
  }
  int num = va_arg(ap, int);
  char temp[100];
  if(*fmt=='d'){
    int k = num==0?1:0;
    if(num<0){
      add_char(s,'-');
      num=-num;
    }
    while (num) {
      temp[k] = num % 10 + '0';
      num /= 10;
      k++;
    }
    int i=k;
    while(i<num_of_digit){
      add_char(s,c);
      i++;
    }
    while(k){
      add_char(s,temp[k-1]);
      k--;
    }
  }
  else if(*fmt=='x'){
    int k = num==0?1:0;
    while (num) {
      temp[k] = num % 16 + (num % 16 < 10 ? '0' : 'a' - 10);
      num /= 16;
      k++;
    }
    int i=k;
    while(i<num_of_digit){
      add_char(s,c);
      i++;
    }
    while(k){
      add_char(s,temp[k-1]);
      k--;
    }
  }
}

#endif