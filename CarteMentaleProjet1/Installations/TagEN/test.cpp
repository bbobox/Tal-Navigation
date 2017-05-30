#include <stdio.h>

#define START 2664020
#define END   2664032

int main(void)
{
  char buffer[1024];
  int i;
  for(i=0;i<1024;i++)
    buffer[i]=0;
  FILE *f=fopen("transcript-epure.sgml","rb");
  fseek(f,START,SEEK_SET);
  printf("Résultat: %d\n",fread(buffer,1,END-START+1,f));
  printf("%s\n",buffer);
  fclose(f);
}
