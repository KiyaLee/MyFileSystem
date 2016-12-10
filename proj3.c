#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

typedef struct inodemake{
	unsigned int ftype; // ftype = 0 is normal file, ftype = 1 is directory file 파일종류
	int inodenum; // inode 넘버
	int ftime[6]; // 파일 시간
	int fsize; // 파일 크기
	int datablocknum; // 데이터블록 정보
}inodemake;

typedef struct datablockmake{
	char data[128]; // 128바이트 지정
	int datablocknum; // 데이터블록 넘버
}datablockmake;

typedef struct filename{
	char name[5];//파일이름
	int inodeconnect;//아이노드넘버랑 같게 꽂아주는 역할
}filename;
//디렉토리 파일 데이터엔 이 정보를 모두 저장

typedef struct superblockmake{
	unsigned int inodeuse[16];
	unsigned int datause[32];
}superblockmake;
superblockmake superblock;

int findnowdir();
int super_i_up();
int super_d_up();
void super_i_down(int);
void super_d_down(int);

int main(void){
	time_t t;
	struct tm *pt;
	FILE *ifp;
	int myfscheck = 0;//myfs확인용
	int *check;
	int i, k, m, j, x = 0, h, n = 0, read;
	char typ[1000];// 명령 입력용
	char where[100];
	short int bootblock = 0;
	datablockmake datablock[1024];// 데이터블록 1024개 지정
	inodemake inode[512]; // 아이노드 512개 지정
	filename file[512];
// 아이노드 넘버 부여 밑 초기화
	for(i = 0; i < 512 ; i++){
		inode[i].inodenum = i + 1;
		inode[i].ftype = 0;
		inode[i].fsize = 0;
		inode[i].datablocknum = 0;
	} 
// 데이터블록 넘버 부여밑 초기화
	for(k = 0; k < 1024; k++){
		datablock[k].datablocknum = k + 1; // 데이터블록 넘버 부여
	}
    // 루트 설정 남은거 있음
	strcpy(file[0].name , "/");
	file[0].inodeconnect = 1;
	inode[0].ftype = 1;
	inode[0].datablocknum = 1;
	strcpy(where, file[0].name);
	// 루트 트리 설정
//실행
		while(1){

			if(access("myfs", F_OK) == -1){ 
				printf("error : no myfs\n");
				while(x != 1){
					printf("$");
					scanf("%[^\n]", &typ);
					getchar();
					if(!strncmp(typ,"mymkfs",6)){
						ifp = fopen("myfs", "wb");
						fclose(ifp);
					}
					else if(!strncmp(typ,"myfs_shell", 10)){
						if(access("myfs", F_OK) == 0)
						x = 1;
						else
							printf("error : no myfs\n");
					}
					else
						printf("command error\n");
			}
		}

			printf("[%s ]$", where); //프롬프트이름
			scanf("%[^\n]", &typ);//공백도 입력받기 위하여
			getchar();
//command 명령			
  			if(strncmp(typ, "my", 2)){
					system(typ);
					}
//mymkfs 명령	
			if(!strncmp(typ,"mymkfs",6)){
					printf("error : nyfs exists!\n");//확인용
                  }
//myls 명령
			if(!strncmp(typ,"myls", 4)){

				int e, i = 0, n = 0;
				char findname[5];
				char name[20][5];
				char tmp[5];
				// 이걸 해야함.. 현재 디렉터리 데이터넘버 찾기
				e = findnowdir(where);
					if(e == 1){
						h = 0;
					}
					else{
						strncpy(findname, &where[e-4], 4); 
						//findname이랑 file[i].name 비교해서 같은거 찾고 그 i로 데이터 블록 열기
						while(strcmp(findname, file[i].name)){
							i++;
						}
						h = inode[i].datablocknum - 1;
					}

			
			//	h = nowdatanumber find;

				for(i = 0; datablock[h].data[i] != '\0'; i++){

					for( k = 0; k < 4; k++)
						name[n][k] = datablock[h].data[i+k];

					name[n][5] = '\0';
					i += 4;
					n++;
				}

				//name[n] 들 정렬
				for(i=0; i < n-1; i++){

					for(j=0; j<n-1-i;j++){

						if(strcmp(name[j], name[j+1]) > 0){
							strcpy(tmp, name[j]);
							strcpy(name[j], name[j+1]);
							strcpy(name[j+1], tmp);
						}
					}
				}

				for(i = 0; i < n; i++)

					printf("%s ", name[i]);

				printf("\n");
			}
//mycat 명령
			if(!strncmp(typ,"mycat", 5)){
			}
//myshowfile 명령
			if(!strncmp(typ,"myshowfile", 10)){
			}
//mypwd 명령
			if(!strncmp(typ,"mypwd", 5)){
			}
//mycd 명령
			if(!strncmp(typ,"mycd", 4)){
			}
//mycp 명령
			if(!strncmp(typ,"mycp", 4)){
			}
//mycpto 명령
			if(!strncmp(typ,"mycpto", 6)){
			}
//mycpfrom 명령
			if(!strncmp(typ,"mycpfrom", 8)){
			}
//mymkdir 명령
			if(!strncmp(typ,"mymkdir",7)){
				int n, k;
				printf("!\n");//확인
				// 슈퍼블록으로 빈 아이노드 찾고 file[n]를 부여
				for(i = 0; i < 4; i++){
					file[n].name[i] = typ[i+8];//파일이름 넣기
				}
				printf("%s\n", file[n].name);
				file[n].inodeconnect = n+1; // 슈퍼블록 통해 배정하는 것으로 변경
				inode[n].ftype = 1;//디렉토리 명명
				t = time(NULL);
				pt=localtime(&t);
				inode[n].ftime[0] = pt->tm_year+1900;
				inode[n].ftime[1] = pt->tm_mon+1;
				inode[n].ftime[2] = pt->tm_mday;
				inode[n].ftime[3] = pt->tm_hour;
				inode[n].ftime[4] = pt->tm_min;
				inode[n].ftime[5] = pt->tm_sec;
				//슈퍼블록으로 빈 데이터블록 찾고 inode[n].datablocknum 부여= k
				inode[n].datablocknum = k;
				
			}
//myrmdir 명령
			if(!strncmp(typ,"myrmdir", 7)){
			}
//mymv 명령
			if(!strncmp(typ,"mymv", 4)){
			}
//mytouch 명령
			if(!strncmp(typ,"mytouch", 7)){
			}
//myshowinode 명령
			if(!strncmp(typ,"myshowinode", 11)){
			}
//myshowblock 명령
			if(!strncmp(typ,"myshowblock", 11)){
			}
//mystate 명령
			if(!strncmp(typ,"mystate", 7)){
			}
//mytree 명령
			if(!strncmp(typ,"mytree", 6)){
			}
		}
		return 0;
}
	 
int findnowdir(char a[]){
	int i = 0;
		while(a[i] != '\0'){
			i++;
		}

		return i;
}

//super_i_up 파일을 생성시 가용 아이노드슈퍼블록 1로 변경 후 아이노드 넘버 num으로 리턴;
int super_i_up(){
	int num, r, i, t, v = 0, k = 1, s;
	unsigned int comp;
	unsigned int to;
	to = 1;
	to = to << 31;
	while(k){
		t = 1;
		s = 0;
	comp = superblock.inodeuse[v]; 
	while((comp & to) == to){
		comp = comp << 1;
		s++;
		if((s % 32) == 0){
			printf("R");
			v++;
			comp = superblock.inodeuse[v];
		}
		printf("O");
	}
	if((comp & to) == 0){ 
		comp = to;
		comp = comp >> s;
		superblock.inodeuse[v] += comp;
		k = 0;
	}
	if(v == 16)
		k = 0;
	printf("X");
	num = v*32 + (s % 32) + 1;
	}
	printf("num: %d\n", num);
	printf("\n");
	return num;
}
//super_d_up 파일을 생성시 가용 데이터블록넘버슈퍼블록 1로 변경후 데이터블록 넘버 num으로 리턴;
int super_d_up(){
	int num, r, i, t, v = 0, k = 1, s;
	unsigned int comp;
	unsigned int to;
	to = 1;
	to = to << 31;
	while(k){
		t = 1;
		s = 0;
	comp = superblock.datause[v]; 
	while((comp & to) == to){
		comp = comp << 1;
		s++;
		if((s % 32) == 0){
			printf("R");
			v++;
			comp = superblock.datause[v];
		}
		printf("O");
	}
	if((comp & to) == 0){ 
		comp = to;
		comp = comp >> s;
		superblock.datause[v] += comp;
		k = 0;
	}
	if(v == 32)
		k = 0;
	printf("X");
	num = v*32 + (s % 32) + 1;
	}
	printf("num: %d\n", num);
	printf("\n");
	return num;
}
void super_i_down(int i){
	int m, n;
	unsigned int to;
	to = 1;
	to = to << 31;
	m = i / 32;
	n = i % 32;
	to = to >> n - 1;

	superblock.inodeuse[m] -= to;
}
	
void super_d_down(int i){
	int m, n;
	unsigned int to;
	to = 1;
	to = to << 31;
	m = i / 32;
	n = i % 32;
	to = to >> n - 1;

	superblock.datause[m] -= to;
}
