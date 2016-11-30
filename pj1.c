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
	char name[5];
	int inodeconnect;
}filename;

int main(void){
	int i, k, m;
	char typ[20];
	short int bootblock;
	inodemake inode[512]; // 아이노드 512개 지정
	datablockmake datablock[1024]; // 데이터블록 1024개 지정

	for(i = 0; i < 512 ; i++){
		inode[i].inodenum = i + 1; // 아이노드넘버 부여
		inode[i].ftype = 0;
		inode[i].ftime = 0;
		inode[i].fsize = 0;
		inode[i].datablocknum = 0;
	} 
	for(k = 0; k < 1024; k++){
		datablock[k].datablocknum = k + 1; // 데이터블록 넘버 부여
		for( m = 0; m < 128; m++)
			datablock[k].data[m] = 0;//데이터 0 초기화
	}
    
	filename root;
	strcpy(root.name , "/");
	root.inodeconnect = 1;
	inode[0].ftype = 1;
	inode[0].datablocknum = 1;

		while(1){
			printf("[%s ]$", root.name);
			scanf("%s", &typ);
			filename file;
			strcpy(file.name , typ);
			file.name[4] = '\0';
			printf("%s\n", file.name);
	

		}
		return 0;


}
	 

