#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int InodeNumber = 1;//현재 디렉터리를 나타내기 위해 사용하는 변수

// 바이너리 트리 구조체
typedef struct binary_tree{
	int inodenumber;
	int depth;	//길이
	int ftype;	//0: 일반파일, 1: 디렉터리파일
	int check;	//0: 나머지,  1: 구조체 -> Down
	struct binary_tree* Down;
	struct binary_tree* Right;
}tree;
// 아이노드 구조체
typedef struct inodemake{
	unsigned int ftype; // ftype = 0 is normal file, ftype = 1 is directory file 파일종류
	int inodenum; // inode 넘버
	char filename[5];// 파일 이름
	int ftime[6]; // 파일 시간
	int fsize; // 파일 크기
	int datablocknum; // 데이터블록 정보
	int singlenum;
	int doublenum;
}inodemake;
//데이터 블록 구조체
typedef struct datablockmake{
	char data[128]; // 128바이트 지정
	int datablocknum; // 데이터블록 넘버
}datablockmake;
//파일 이름 구조체
typedef struct filename{
	char name[5];//파일이름
	int inodeconnect;//아이노드넘버랑 같게 꽂아주는 역할
}filename;//디렉토리 파일 데이터엔 이 정보를 모두 저장
//슈퍼블록 구조체
typedef struct superblockmake{
	unsigned int inodeuse[16];// 아이노드 정보
	unsigned int datause[32];// 슈퍼블록 정보
}superblockmake;
superblockmake superblock;// 전역 변수로 선언

int super_i_up();// 슈퍼블록 아이노드정보  증가 함수
int super_d_up();// 슈퍼블록 데이터블록정보 증가 함수
void super_i_down(int);// 슈퍼블록 아이노드정보 하락 함수
void super_d_down(int);// 슈퍼블록 데이터블록 정보 하락 함수
void CreateNewDir(tree* );//디렉터리 파일을 트리 구조에 추가하는 함수
void CreateNewFile(tree* );//일반 파일을 트리 구조에 추가하는 함수
tree* CurrentFile(tree* , int);// 현재 디렉터리를 찾아서 리턴해주는 함수

int main(void){
	time_t t;
	struct tm *pt; // 시간 구조체
	FILE *ifp;// myfs를 열고 닫기 위한 변수
	int myfscheck = 0;//myfs확인용
	int *check;
	int i, k, m, j, x = 0, h, n = 0, read;
	char typ[1000];// 명령 입력용
	char where[100];// 프롬프트 작성용
	short int bootblock = 0;// 부트블록 설정
	datablockmake datablock[1024];// 데이터블록 1024개 지정
	inodemake inode[512]; // 아이노드 512개 지정
	filename file[512];// 파일 512개 지정
// 트리 설정
	tree* RootNode = (tree*)malloc(sizeof(tree));
	RootNode -> Down = NULL;
	RootNode -> Right = NULL;
	RootNode -> inodenumber = 1; //루트의 inodenumber = 1
	RootNode -> depth = 0; //루트의 길이 0
	RootNode -> ftype = 1; //디렉터리 = 1
	RootNode -> check = 0; 
	
	tree* Temporary_Node = (tree*)malloc(sizeof(tree));
	Temporary_Node -> Down = NULL;
	Temporary_Node -> Right = NULL;
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
		for(i = 0; i < 128; i++)
			datablock[k].data[i] = '\0';// 데이터 초기화
	}
    // 루트 설정
	super_i_up();
	super_d_up();// 루트 디렉터리의 아이노드 번호와 데이터블록 넘버는 1로 설정
	strcpy(file[0].name , "/");
	strcpy(inode[0].filename , "/");
	file[0].inodeconnect = 1;
	inode[0].ftype = 1;
	inode[0].datablocknum = 1;
	strcpy(where, file[0].name);
	t = time(NULL);
	pt=localtime(&t);
	inode[0].ftime[0] = pt->tm_year+1900;
	inode[0].ftime[1] = pt->tm_mon+1;
	inode[0].ftime[2] = pt->tm_mday;
	inode[0].ftime[3] = pt->tm_hour;
	inode[0].ftime[4] = pt->tm_min;
	inode[0].ftime[5] = pt->tm_sec;
	//myfs가 이미 있을때, 불러오기
	if(access("myfs", F_OK) == 0){
			ifp = fopen("myfs", "rb");
			fread(&bootblock, sizeof(short int), 1, ifp);
			fread(&superblock, sizeof(superblockmake), 1, ifp);
			for(i = 0; i < 512; i++)
				fread(&inode[i], sizeof(inodemake), 1, ifp);
			for(i = 0; i < 1024; i++)
				fread(&(datablock[i].data), sizeof(char [128]), 1, ifp);
			i = 0;
			fclose(ifp);
			for(i = 0; i < 512; i++){
				strcpy(file[i].name, inode[i].filename);
			}
	}

			
//실행
		while(1){
			//typ 초기화
			for(i = 0; i < 1000; i++)
				typ[i] = '\0';
			
			if(access("myfs", F_OK) == -1){ 
				printf("error : no myfs\n");// myfs가 없을 경우 에러 처리
				while(x != 1){
					printf("$");
					scanf("%[^\n]", &typ);
					getchar();
					//mymkfs 명령
					if(!strncmp(typ,"mymkfs",6)){
						ifp = fopen("myfs", "wb");
						fwrite(&bootblock, sizeof(short int), 1, ifp);
						fwrite(&superblock, sizeof(superblockmake), 1, ifp);
						for(i = 0; i < 512; i++)
							fwrite(&inode[i], sizeof(inodemake), 1, ifp);
						for(i = 0; i < 1024; i++)
							fwrite(&(datablock[i].data), sizeof(char [128]), 1, ifp);
						i = 0;
						fclose(ifp);
					}
					//myfs가 있는지 판단후, 있으면 쉘 실행 없으면 에러 처리
					else if(!strncmp(typ,"myfs_shell", 10)){
						if(access("myfs", F_OK) == 0)
						x = 1;
						else
							printf("error : no myfs\n");
					}
					else
						//그 외의 명령은 모두 cygwin 상으로 보내기
						printf("command error\n");
			}
		}
// 쉘 실행
			printf("[%s ]$", where); //프롬프트이름
			scanf("%[^\n]", &typ);//공백도 입력받기 위하여
			getchar();// \n 처리
//byebye 명령
			if(!strncmp(typ,"byebye",6)){
				ifp = fopen("myfs", "wb");
				fwrite(&bootblock, sizeof(short int), 1, ifp);
				fwrite(&superblock, sizeof(superblockmake), 1, ifp);
				for(i = 0; i < 512; i++)
					fwrite(&inode[i], sizeof(inodemake), 1, ifp);
				for(i = 0; i < 1024; i++)
					fwrite(&(datablock[i].data), sizeof(char [128]), 1, ifp);
				i = 0;
				fclose(ifp);
				exit(1);
					}
			
//command 명령			
  			if(strncmp(typ, "my", 2)){
					system(typ);
					}
//mymkfs 명령	
			if(!strncmp(typ,"mymkfs",6)){
					printf("error : myfs exists!\n");//확인용
                  }
//myls 명령
			if(!strncmp(typ,"myls", 4)){

				int m = 0, l = 0, e, i = 0, n = 0;
				char findname[5];
				char name[200][5];
				char tmp[5];
				//현재 디렉터리 데이터넘버 찾기
				e = InodeNumber;
					if(e == 1){// 1이면 루트 디렉터리 이므로 루트의 데이터블록 번호 설정
						h = 0;
					}
					else{
						h = inode[e - 1].datablocknum - 1;// 현재 디렉터리의 아이노드에서 데이터블록 넘버 가져오기
					}

				// 디렉터리 파일에 들어있는 파일 이름들을 name[]들에 넣기
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
				if(!strncmp(&typ[5], "-i", 3)){//인자가 -i인 경우	
					printf(". %3d\n", InodeNumber);// 현재 디렉터리 정보
					for( i = 0; i < n; i++){// 파일 이름을 비교하여 아이노드 번호 찾기
						printf("%s ", name[i]);
						while(strcmp(name[i], inode[l].filename)){
								l++;
								}
						printf("%3d\n", l + 1);

						}
				}
				else if(!strncmp(&typ[5], "-l", 3)){//인자가 -l인 경우
					int x;
					x = InodeNumber - 1;// 현재 디렉터리 정보 출력
					// 정보 출력
					printf("d %4d %d/%d/%d %d:%d:%d .\n", inode[x].fsize, inode[x].ftime[0], inode[x].ftime[1], inode[x].ftime[2], inode[x].ftime[3], inode[x].ftime[4], inode[x].ftime[5]);
					for( i = 0; i < n; i++){// 파일 이름을 비교하여 아이노드 번호 찾기
						while(strcmp(name[i], inode[l].filename)){// 이름을 비교하여 아이노드 찾기
								l++;
								}
						if(inode[l].ftype == 1)
							printf("d ");
						else
							printf("- "); 
						//정보 출력
					printf("%4d %d/%d/%d %d:%d:%d %s\n", inode[l].fsize, inode[l].ftime[0], inode[l].ftime[1], inode[l].ftime[2], inode[l].ftime[3], inode[l].ftime[4], inode[l].ftime[5], name[i]);


						}
					
				}
				else if(!strncmp(&typ[5], "-il", 3)){//인자가 -il인 경우
					int x;
					x = InodeNumber - 1;// 현재 디렉터리 정보 출력
					// 정보 출력
					printf("d %3d %4d %d/%d/%d %d:%d:%d .\n", InodeNumber, inode[x].fsize, inode[x].ftime[0], inode[x].ftime[1], inode[x].ftime[2], inode[x].ftime[3], inode[x].ftime[4], inode[x].ftime[5]);
					for( i = 0; i < n; i++){// 파일 이름을 비교하여 아이노드 번호 찾기
						while(strcmp(name[i], inode[l].filename)){// 이름을 비교하여 아이노드 찾기
								l++;
								}
						if(inode[l].ftype == 1)
							printf("d ");
						else
							printf("- "); 
						//정보 출력
					printf("%3d %4d %d/%d/%d %d:%d:%d %s\n", l + 1, inode[l].fsize, inode[l].ftime[0], inode[l].ftime[1], inode[l].ftime[2], inode[l].ftime[3], inode[l].ftime[4], inode[l].ftime[5], name[i]);


						}
				}
				else{// 인자가 없는 경우
				printf(".\n..\n");
				// 사전 순으로 출력
				for(i = 0; i < n; i++)

					printf("%s\n", name[i]);
				}

			}
//mycat 명령
			if(!strncmp(typ,"mycat", 5)){
			}
//myshowfile 명령
			if(!strncmp(typ,"myshowfile", 10)){
			}
//mypwd 명령
			if(!strncmp(typ,"mypwd", 5)){
				int l = 100;// 프롬프트 명인 where[]의 수로 맞춰주기
				char check[100];// where에 손실이 없게 check에 담기
				strcpy(check, where);
				if(InodeNumber == 1){// 1이면 루트이므로 /만 출력
					printf("/\n");
				}
				else{
					while(check[l - 1] != '/'){ 
						l--;
					}
					check[l - 1] = '\0';
					printf("%s\n", check); // 현재 작업 디렉터리 출력
				}

			}
//mycd 명령
			if(!strncmp(typ,"mycd", 4)){
				int n, i, l = 100, k = 0, m = 0;
				char checkname[5];// 파일 이름 인자를 받기 위한 변수
				
				if(!(((typ[5]>=48)&&(typ[5]<=57))||((typ[5]>=65)&&(typ[5]<=90))||((typ[5]>=97)&&(typ[5]<=122)))){//문자 숫자가 아닌 것이 나오면 인자 없는 것으로 처리
					InodeNumber = 1;// 인자가 없으면 루트로 돌아오게 만들기 위해 InodeNumber를 1로 만들기
					for(i = 1; i < 100; i++)
						where[i] = '\0';// 프롬프트 바꿔주기
				}
				else{
					if(!(((typ[8]>=48)&&(typ[8]<=57))||((typ[8]>=65)&&(typ[8]<=90))||((typ[8]>=97)&&(typ[8]<=122)))){//문자 숫자가 아닌 것이 나오면 인자 없는 것으로 처리

						printf("Please write more than 3!\n");// 파일 이름이 4자 미만으로 입력되면 에러 처리 파일 이름은 4자 이상으로만 받게 만들기
					}
					else{
						strncpy(checkname, &typ[5], 4);// checkname에 인자 넣기
						n = inode[InodeNumber - 1].datablocknum;// 해당 디렉터리 데이터 블록 넘버
						while(strncmp(&(datablock[n-1].data[m*5]), checkname, 4)){// 파일 찾기
							m++;
							if(m == 25)
								break;// 한 파일은 128 바이트 이하라고 가정하여 m이 25이면 데이터 블록을 다 읽어본 것으로 처리
						}
						if(m == 25){
							printf("No exist File!\n");
						}
						else{
							while(strncmp(inode[k].filename, checkname, 4)){// checkname으로 파일을 찾아 해당 inode 찾기
								k++;
							}
							if(inode[k].ftype == 0){
								printf("No Directory File!\n"); // 일반 파일이면 에러 처리
							}
							else{
							InodeNumber = k+1; // InodeNumber 변경
							while(where[l - 1] != '/'){ //프롬프트를 위한 where 를 작성
								l--;
							}
							strncpy(&where[l], inode[k].filename, 4);
							where[l + 4] = '/';
							where[l + 5] = '\0';
							}
						}
					}
				}
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
				char checkname[5];// 인자 받는 변수
				int n, l = 0, k = 0, m = 0;
				int i, d;
				if(!(((typ[11]>=48)&&(typ[11]<=57))||((typ[11]>=65)&&(typ[11]<=90))||((typ[11]>=97)&&(typ[11]<=122)))){//문자 숫자가 아닌 것이 오면 인자가 없거나 3자리 이하인 것으로 처리
					printf("Please write more file name.\n");
				}
				else{
					strncpy(checkname, &typ[8], 4);
					n = inode[InodeNumber - 1].datablocknum; // 현재 디렉터리 데이터 블록 찾기
					while(strncmp(&(datablock[n-1].data[m*5]), checkname, 4)){// 파일 읽기
							m++;
							if(m == 25)// 해당 블록을 다 읽은 것으로 처리
								break;
					}
					if(m == 25){// 같은 이름 파일 없음
						while(datablock[n-1].data[l*5] != '\0'){	
							l++;
							if(l == 25)
								break;
						}
						if(l != 25){
						//파일 설정
						i = super_i_up();//아이노드 할당
						d = super_d_up();//데이터 블록 할당
						strcpy(inode[i-1].filename, checkname);//이름
						strcpy(file[i-1].name, checkname);//이름
						inode[i-1].ftype = 1;//디렉터리 설정
						inode[i-1].datablocknum = d;// 데이터블록 넘버 할당
						inode[i-1].fsize = 0;// 용량 초기 0으로 할당
						// 시간 설정
						t = time(NULL);
						pt=localtime(&t);
						inode[i-1].ftime[0] = pt->tm_year+1900;
						inode[i-1].ftime[1] = pt->tm_mon+1;
						inode[i-1].ftime[2] = pt->tm_mday;
						inode[i-1].ftime[3] = pt->tm_hour;
						inode[i-1].ftime[4] = pt->tm_min;
						inode[i-1].ftime[5] = pt->tm_sec;
						//상위 디렉터리 파일에 파일 이름 작성
						strncpy(&(datablock[n-1].data[l*5]), checkname, 4);
						}
						else
							printf("Can't make more file!\n");// 128바이트가 넘으면 더이상 파일 생성 불가 처리
					}
					else{
						printf("Already exist file!\n");//이미 있는 파일 알림
					}

				}
			}	
//myrmdir 명령
			if(!strncmp(typ,"myrmdir", 7)){
			}
//mymv 명령
			if(!strncmp(typ,"mymv", 4)){
			}
//mytouch 명령
			if(!strncmp(typ,"mytouch", 7)){
				char checkname[5];// 인자 받기
				int n, l = 0, k = 0, m = 0;
				int i, d;
				if(!(((typ[11]>=48)&&(typ[11]<=57))||((typ[11]>=65)&&(typ[11]<=90))||((typ[11]>=97)&&(typ[11]<=122)))){//문자 숫자가 아니면 인자 없거나 3자 이하로 적은 것으로 처리
					printf("Please write more file name.\n");
				}
				else{
					strncpy(checkname, &typ[8], 4);
					n = inode[InodeNumber - 1].datablocknum;// 해당 디렉터리 데이터블럭 찾기
					while(strncmp(&(datablock[n-1].data[m*5]), checkname, 4)){
							m++;
							if(m == 25)// 128바이트 제한 처리
								break;
					}
					if(m == 25){
						while(datablock[n-1].data[l*5] != '\0'){	
							l++;
							if(l == 25)// 파일 찾기
								break;
						}
						if(l != 25){// 파일이 없으면
						//파일 설정
						i = super_i_up();	
						d = super_d_up();
						strcpy(inode[i-1].filename, checkname);
						strcpy(file[i-1].name, checkname);
						inode[i-1].ftype = 0;
						inode[i-1].datablocknum = d;
						inode[i-1].fsize = 0;
						//시간 설정
						t = time(NULL);
						pt=localtime(&t);
						inode[i-1].ftime[0] = pt->tm_year+1900;
						inode[i-1].ftime[1] = pt->tm_mon+1;
						inode[i-1].ftime[2] = pt->tm_mday;
						inode[i-1].ftime[3] = pt->tm_hour;
						inode[i-1].ftime[4] = pt->tm_min;
						inode[i-1].ftime[5] = pt->tm_sec;
						strncpy(&(datablock[n-1].data[l*5]), checkname, 4);
						}
						else
							printf("Can't make more file!\n");// 128바이트가 넘으면 에러 처리
					}
					else{// 파일이 있으면
						while(strncmp(inode[k].filename, checkname, 4)){// 아이노드 찾아서
							k++;
						}
						//시간만 변
						t = time(NULL);
						pt=localtime(&t);
						inode[k].ftime[0] = pt->tm_year+1900;
						inode[k].ftime[1] = pt->tm_mon+1;
						inode[k].ftime[2] = pt->tm_mday;
						inode[k].ftime[3] = pt->tm_hour;
						inode[k].ftime[4] = pt->tm_min;
						inode[k].ftime[5] = pt->tm_sec;
						
					}

				}
			}
//myshowinode 명령
			if(!strncmp(typ,"myshowinode", 11)){
				int size, k, findnum = 0;// findnum으로 아이노드 설정
				if((typ[12] >= 48)&&(typ[12] <= 57)){// 인자가 유무 확인
					if(!((typ[13] >= 48)&&(typ[13] <= 57)))// 인자가 1자리 숫자
						findnum = typ[12] - 48;
					else if(!((typ[14] >= 48)&&(typ[14] <= 57))){// 인자가 2자리 숫자
						findnum = typ[13] - 48;
						findnum += (typ[12] - 48) * 10;
					}
					else if(!((typ[15] >= 48)&&(typ[15] <= 57))){// 인자가 3자리 숫자
						findnum = typ[14] - 48;
						findnum += (typ[13] - 48)*10;
						findnum += (typ[12] - 48)*100;
					}
				// 해당 아이노드 정보 출력
				printf("아이노드 번호 : %d\n", inode[findnum-1].inodenum);
				if(inode[findnum - 1].ftype == 0)
					printf("파일 종류 : 일반 파일\n");
				else
					printf("파일 종류 : 디렉터리 파일\n");
				printf("파일 이름 : %s\n", inode[findnum-1].filename);
				printf("파일 시간 : %d년 %d월 %d일 %d시 %d분 %d초\n", inode[findnum-1].ftime[0], inode[findnum -1].ftime[1], inode[findnum-1].ftime[2], inode[findnum-1].ftime[3], inode[findnum-1].ftime[4], inode[findnum-1].ftime[5]);
				//데이터블럭 넘버가 0이 아닐때 블록을 열어 용량 측정
				if(inode[findnum-1].datablocknum != 0){
					  k = 128;
					  while(datablock[inode[findnum-1].datablocknum -1].data[k-1] == '\0'){
						  if(k == 0)
							  break;
						  k--;
					  }
					  inode[findnum -1].fsize = k;
				}
				printf("파일 용량 : %d\n", inode[findnum -1].fsize);
				printf("데이터 블록 : %d",inode[findnum -1].datablocknum);
				//싱글인다이렉트블록 이나 더블인다이렉트블록이 있을 경우 출력
				if(inode[findnum-1].singlenum != 0)
					printf(", %d", inode[findnum -1].singlenum);
				if(inode[findnum-1].doublenum != 0)
					printf(", %d",inode[findnum -1].doublenum);
				printf("\n");
				}
				else
					printf("Please write number!\n");// 인자가 없을시 입력 경고 처리
			}
//myshowblock 명령
			if(!strncmp(typ,"myshowblock", 11)){
				int i, findnum = 0;	
				if((typ[12] >= 48)&&(typ[12] <= 57)){// 인자 유무 확인
					if(!((typ[13] >= 48)&&(typ[13] <= 57)))// 인자가 1자리
						findnum = typ[12] - 48;
					else if(!((typ[14] >= 48)&&(typ[14] <= 57))){// 인자가 2자리
						findnum = typ[13] - 48;
						findnum += (typ[12] - 48) * 10;
					}
					else if(!((typ[15] >= 48)&&(typ[15] <= 57))){// 인자가 3자리
						findnum = typ[14] - 48;
						findnum += (typ[13] - 48)*10;
						findnum += (typ[12] - 48)*100;
					}
					else if(!((typ[16] >= 48)&&(typ[15] <= 57))){// 인자가 4자리
						findnum = typ[15] - 48;
						findnum += (typ[14] - 48)*10;
						findnum += (typ[13] - 48)*100;
						findnum += (typ[12] - 48)*1000;
					}
					// 해당 데이터블록 출력
					for(i = 0; i < 128; i++)
						putchar(datablock[findnum-1].data[i]);
					putchar('\n');
				}
				else
					printf("Please write number!\n");// 인자 입력 경고
			}
//mystate 명령
			if(!strncmp(typ,"mystate", 7)){
				int icount = 0, dcount = 0;
				int i, k, v;
				unsigned int sap;
				unsigned int temp;
				//비트열 비교를 위한 초기 기준 값 설정
				sap = 1;
				sap = sap << 31;
				// 슈퍼 블록의
				//아이노드 정보 계산
				for( v = 0; v < 16; v++){
					temp = superblock.inodeuse[v];// 슈퍼블록 손실을 막기 위해 temp에 넣기
					for( k = 0; k < 32; k++){
						// 1비트씩 밀어내주며 비교하여 가용 갯수 계산
						if((sap & temp) == 0){
							icount++;
							}
						temp = temp << 1;
					}
				}
				printf("free inode : %d\n", icount);
				//데이터블록 정보 계산
				for( v = 0; v < 32; v++){
					temp = superblock.datause[v];//슈퍼블록 손실을 막기 위해 temp에 넣기
					for( k = 0; k < 32; k++){
						// 1비트씩 밀어내주며 비교하여 가용 갯수 계산
						if((sap & temp) == 0){
							dcount++;
						}
						temp = temp << 1;
					}
				}
				printf("free data block: %d\n", dcount);


			}
//mytree 명령
			if(!strncmp(typ,"mytree", 6)){
			}
			
		}
//트리 구조체 동적 메모리 반납		
	free(Temporary_Node);
	free(RootNode);
		return 0;
}
	 

//super_i_up 파일을 생성시 가용 아이노드슈퍼블록 1로 변경 후 아이노드 넘버 num으로 리턴;
int super_i_up(){
	int num, r, i, t, v = 0, k = 1, s;
	unsigned int comp;
	unsigned int to;
	//비트열 비교를 위한 기준 값 설정
	to = 1;
	to = to << 31;
	//슈퍼블록 아이노드 정보에서 맨 앞부터 0인것 찾기
	while(k){
		t = 1;
		s = 0;
	comp = superblock.inodeuse[v]; //데이터 손실 방지를 위해 comp에 넣기
	while((comp & to) == to){// 비교
		comp = comp << 1;
		s++;
		if((s % 32) == 0){// unsigned int 변수 하나가 4바이트 = 32비트 이므로 32번 비교한 후 다음 배열로 바꿔주기
			v++;
			comp = superblock.inodeuse[v];
		}
	}
	if((comp & to) == 0){ // 0 인것 찾아서 1로 바꿔주기
		comp = to;
		comp = comp >> s;
		superblock.inodeuse[v] += comp;
		k = 0;
	}
	if(v == 16)
		k = 0;
	num = v*32 + (s % 32) + 1;//해당 아이노드 번호를 리턴하기
	}
	return num;
}
//super_d_up 파일을 생성시 가용 데이터블록넘버슈퍼블록 1로 변경후 데이터블록 넘버 num으로 리턴;
int super_d_up(){
	int num, r, i, t, v = 0, k = 1, s;
	unsigned int comp;
	unsigned int to;
	// 비트열 비교를 위한 기준 값 설정
	to = 1;
	to = to << 31;
	//슈퍼블록 데이터블록 정보에서 맨 앞부터 0인것 찾기
	while(k){
		t = 1;
		s = 0;
	comp = superblock.datause[v];// 데이터 손실 방지를 위해 comp에 넣기 
	while((comp & to) == to){// 비교
		comp = comp << 1;
		s++;
		if((s % 32) == 0){// unsigned int 변수 하나가 4바이트 = 32비트 이므로 32번 비교한 후 다음 배열로 바꿔주기
			v++;
			comp = superblock.datause[v];
		}
	}
	if((comp & to) == 0){ // 0인것 찾아서 1로 바꿔주기
		comp = to;
		comp = comp >> s;
		superblock.datause[v] += comp;
		k = 0;
	}
	if(v == 32)
		k = 0;
	num = v*32 + (s % 32) + 1;//해당 데이터블록 번호를 리턴하기
	}
	return num;
}
// 인자에 원하는 아이노드 번호를 넣으면 해당 슈퍼블록 정보 변경
void super_i_down(int i){
	int m, n;
	unsigned int to;
	//비트열 비교를 위한 기준 값 설정
	to = 1;
	to = to << 31;
	m = i / 32;//배열 인덱스 값 찾기
	n = i % 32;//비트열 찾기
	to = to >> n - 1;

	superblock.inodeuse[m] -= to;//1 에서 0으로 변경
}
// 인자에 원하는 데이터블록 번호를 넣으면 해당 슈퍼블록 정보 변경
void super_d_down(int i){
	int m, n;
	unsigned int to;
	//비트열 비교를 위한 기준 값 설정
	to = 1;
	to = to << 31;
	m = i / 32;//배열 인덱스 값 찾기
	n = i % 32;//비트열 찾기
	to = to >> n - 1;

	superblock.datause[m] -= to;// 1에서 0으로 변경
}
//찾고자 하는 파일의 노드 찾는 함수
tree* CurrentFile(tree* RootNode, int InodeNum){
	if(RootNode -> inodenumber == InodeNum)
		return RootNode;
	if(RootNode -> inodenumber != InodeNum){
		if(RootNode -> Right == NULL && RootNode -> Down != NULL)
			CurrentFile(RootNode -> Down, InodeNum); //계속 반복
		else if(RootNode -> Right != NULL && RootNode -> Down == NULL)
			CurrentFile(RootNode -> Right, InodeNum); //계속 반복
		else if(RootNode -> Right != NULL && RootNode -> Down != NULL){
			CurrentFile(RootNode -> Right, InodeNum);
			CurrentFile(RootNode -> Down, InodeNum);
		}
	}
}

// 파일을 생성시 트리에도 추가
void CreateNewFile(tree* Directory){
	if(Directory -> Down == NULL){
		tree* Current_Node = (tree*)malloc(sizeof(tree));
		Current_Node -> Down = NULL; 
		Current_Node -> Right = NULL;
		Current_Node -> inodenumber = InodeNumber; //InodeNumber 
		Current_Node -> depth = (Directory -> depth) + 1; //Directory + 1
		Current_Node -> ftype = 0; //일반 파일은 0
		Current_Node -> check = 1; //구조체 -> Down = 1
		Directory -> Down = Current_Node;
		Current_Node = Directory -> Down;
			
	}
	else if(Directory -> Down != NULL){//Directory -> Down 이 널이 아닐 때
		Directory = Directory -> Down;
			if(Directory -> Right != NULL){
				while(1){
				Directory = Directory -> Right;	
				if(Directory -> Right == NULL)
				break;
				}
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node -> Down = NULL;
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber;
			Current_Node -> depth = Directory -> depth;
			Current_Node -> ftype = 0;
			Directory -> Right = Current_Node; 
			Current_Node = Directory -> Right;
		}
			else if(Directory -> Right == NULL){//Directory -> Right가 NULL 일 때
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node -> Down = NULL; 
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber; //InodeNumbeㄱ
			Current_Node -> depth = Directory -> depth;
			Current_Node -> ftype = 0;//일반 파일은 0
		   	Current_Node -> check = 0;//구조체 -> Right = 0	
			Directory -> Right =  Current_Node;
			Current_Node = Directory -> Right;

		}
	}
}
// 디렉터리 생성시 트리에 추가
void CreateNewDir(tree* Directory){
	if(Directory -> Down == NULL){
		tree* Current_Node = (tree*)malloc(sizeof(tree));
		Current_Node -> Down = NULL; 
		Current_Node -> Right = NULL;
	   	Current_Node -> inodenumber = InodeNumber; //InodNumber 배정 필수
	   	Current_Node -> depth = (Directory -> depth) + 1;
	   	Current_Node -> ftype = 1; //디렉토리 파일은 1 
	   	Current_Node -> check = 1; //구조체 -> Down = 1 
		Directory -> Down = Current_Node;
		Current_Node = Directory -> Down;
	}
	else if(Directory -> Down != NULL){ //Down에 노드가 있을 때
		Directory = Directory -> Down;
		if(Directory -> Right == NULL){ //Right에 노드가 없을 때
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node -> Down = NULL; 
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber; //main함수에서 증가시킬거면 ++ 빼야됨.
			Current_Node -> depth = Directory -> depth;//같은 길이상에 있기에
			Current_Node -> ftype = 1; // 디렉터리 = 1
			Current_Node -> check = 0; // 구조체 -> Right = 0
			Directory -> Right = Current_Node;
			Current_Node = Directory -> Right;
		}
		else if(Directory -> Right != NULL){ //Right에 노드가 있을 경우
			while(1){ //Right 노드가 없을때까지
				Directory = Directory -> Right;
				if(Directory -> Right == NULL);
				break;
			}
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node -> Down = NULL; 
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber; //마찬가지
			Current_Node -> depth = Directory -> depth;
			Current_Node -> ftype = 1; //디렉터리 = 1
			Current_Node -> check = 0; //구조체 -> Right = 0
			Directory -> Right = Current_Node;
			Current_Node = Directory -> Right;
		}
	}
}
