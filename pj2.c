#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct inodemake{
	unsigned int ftype; // ftype = 0 is normal file, ftype = 1 is directory file 파일종류
	int inodenum; // inode 넘버
	int ftime; // 파일 시간
	int fsize; // 파일 크기
	int datablocknum; // 데이터블록 정보
	struct inodemake *up; // 디렉터리 연결 하는 linkedlist 역할
	struct inodemake *down;
}inodemake;

typedef struct datablockmake{
	char data[128]; // 128바이트 지정
	int datablocknum; // 데이터블록 넘버
}datablockmake;

typedef struct filename{
	char name[5];//파일이름
	int inodeconnect;//아이노드넘버랑 같게 꽂아주는 역할
}filename;

int main(void){
	FILE *ifp;
	int myfscheck = 0;//myfs확인용
	int i, k, m;
	char typ[1000];// 명령 입력용
	short int bootblock;
	datablockmake datablock[1024];// 데이터블록 1024개 지정
	inodemake inode[512]; // 아이노드 512개 지정
// 아이노드 넘버 부여 밑 초기화
	for(i = 0; i < 512 ; i++){
		inode[i].inodenum = i + 1;
		inode[i].ftype = 0;
		inode[i].ftime = 0;
		inode[i].fsize = 0;
		inode[i].datablocknum = 0;
	} 
// 데이터블록 넘버 부여밑 초기화
	for(k = 0; k < 1024; k++){
		datablock[k].datablocknum = k + 1; // 데이터블록 넘버 부여
		for( m = 0; m < 128; m++)
			datablock[k].data[m] = 0;//데이터 0 초기화
	}
    // 루트 설정 남은거 있음
	filename root;
	strcpy(root.name , "/");
	root.inodeconnect = 1;
	inode[0].ftype = 1;
	inode[0].datablocknum = 1;
//실행
		while(1){
			printf("[%s ]$", root.name); //프롬프트이름
			scanf("%[^\n]", &typ);//공백도 입력받기 위하여
			getchar();
//mymkfs 명령	
			if(!strncmp(typ,"mymkfs",6)){
				if(myfscheck == 0){
					ifp = fopen("myfs","wb");//열기 입력 작업 필요
					fclose(ifp);//닫기 
					printf("!\n");//확인용
					myfscheck++;//myfs만들었나안했나 체크용
				}
				else
					printf("error : myfs exists.\n");//에러

                  }
//mymkdir 명령
			if(!strncmp(typ,"mymkdir",7)){
				printf("!\n");//확인
				static filename file;//파일 생성
				for(i = 0; i < 4; i++){
					file.name[i] = typ[i+8];//파일이름 넣기
				}
				printf("%s\n", file.name);
				file.inodeconnect = 2; // 슈퍼블록 통해 배정하는 것으로 변경
				inode[file.inodeconnect - 1].ftype = 1;//디렉토리 명명
				inode[file.inodeconnect - 1].up = &inode[root.inodeconnect - 1]; // 루트에 꽂아줌
			}
		}
		return 0;
}
	 

