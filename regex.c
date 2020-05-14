#include<stdio.h>
#include<assert.h>
#include<regex.h>
#include<string.h>
#include<stdlib.h>
int main(){
  char s1[30];
  char s2[30];
  char regex[20];
  char s[500];
  strcpy(regex,"(.*)?\\(.*<(.*?)>");
  strcpy(s,"mmap(NULL, 8192, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS, -1, 0) = 0x7fd0cb142000 <0.000055>");
  regex_t preg;
  regcomp(&preg,regex,REG_EXTENDED);
  regmatch_t matchptr[10];
  regexec(&preg,s,5,matchptr,0);
  int len1=matchptr[1].rm_eo-matchptr[1].rm_so;
  int len2=matchptr[2].rm_eo-matchptr[2].rm_so;
  strncpy(s1,s+matchptr[1].rm_so,len1);
  s1[len1]='\0';
  strncpy(s2,s+matchptr[2].rm_so,len2);
  s2[len2]='\0';
  printf("%s\n",s1);
  printf("%s\n",s2);
  printf("%s %s\n",s,regex);
  printf("%d %d\n",matchptr[0].rm_so,matchptr[0].rm_eo);
  printf("%d %d\n",matchptr[1].rm_so,matchptr[1].rm_eo);
  printf("%d %d\n",matchptr[2].rm_so,matchptr[2].rm_eo);
  double time=atof(s2);
  printf("%lf\n",time);
  return 0;
}