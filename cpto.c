#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>



int main(void)
{






//mycpto 명령
			if(!strncmp(typ,"mycpto", 6)){
				char line[1000];
				char cptoname [5];
				char cptonameto [5];
				if(!(((typ[11]>=48)&&(typ[11]<=57))||((typ[11]>=65)&&(typ[11]<=90))||((typ[11]>=97)&&(typ[11]<=122)))){//문자 숫자가 아니게 만들기
				printf("Please write more file name.\n");
			}

				else{
					strncpy(cptoname, &typ[7],4);//이 부분에서 복사할 것을 지정해야 할 듯	한데....

					 if(strncpy(cptonameto, &typ[7], 4))
					 {	FILE *cp;;
						 cp=fopen("cptonameto","w");//cptoname은 새로 생성할 파일, 명령시 적힌 글자가 들어가면 좋을 텐데...
						FILE *cpf;
							cpf=fopen("cptoname", "r");
						while(fgets(line, 999, cpf))
						{
							fputs(line,cp);
						}
						fclose(cp);
						fclose(cpf);
			}}}




}



