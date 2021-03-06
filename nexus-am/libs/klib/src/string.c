#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t i=0;
	while(s[i] != '\0')
		i++;
	return i;
}

char *strcpy(char* dst,const char* src) {
  char* r = dst;
	while ((*r++ = *src++) != '\0') {};
	return dst;
}

char* strncpy(char* dest, const char* src, size_t n) {
	 size_t i;

	 for (i = 0; i < n && src[i] != '\0'; i++)
	    dest[i] = src[i];
   for ( ; i < n; i++)
      dest[i] = '\0';
   return dest;
}

char* strcat(char* dst, const char* src) {
  char* s;
	for (s = dst; *s != '\0'; s++){};
	  while ((*s++ = *src++) != '\0');
	return dst;
}

int strcmp(const char* s1, const char* s2) {
  size_t i;
  for (i = 0; s1[i] != '\0' && s1[i] == s2[i]; i++){};
	  if (s1[i] > s2[i]) {
	     return 1;
		 }
		 if (s1[i] < s2[i]) {
		   return -1;
		  }
	return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t i;
	for(i=0; s1[i]!= '\0' && s1[i]==s2[i] && i<n; i++){};
	if(s1[i]>s2[i])
		return 1;
	else if(s1[i] < s2[i])
		return -1;
	else return 0;
}

void* memset(void* v,int c,size_t n) {
  unsigned char* s = (unsigned char*)v;
  for (int i = 0; i < n; i++) {
    s[i] = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  return NULL;
}

int memcmp(const void* s1, const void* s2, size_t n){
 	unsigned char* p1 = (unsigned char*)s1;
	unsigned char* p2 = (unsigned char*)s2;
  size_t i;
  n--;
  for (i = 0; p1[i] != '\0' && p1[i] == p2[i] && i < n; i++){} ;
	  if (p1[i] > p2[i]) {
		    return 1;
	  }
	  if (p1[i] < p2[i]) {
	    return -1;
	  }
	return 0;
}

#endif
