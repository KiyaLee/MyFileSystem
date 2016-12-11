#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int InodeNumber = 1;

typedef struct binary_tree{
	int inodenumber;
	int depth;	//길이
	int ftype;	//0: 일반파일, 1: 디렉터리파일
	int check;	//0: 나머지,  1: 구조체 -> Down
	struct binary_tree* Down;
	struct binary_tree* Right;
}tree;

typedef struct inodemake{
	unsigned int ftype; // ftype = 0 is normal file, ftype = 1 is directory file 파일종류
	int inodenum; // inode 넘버
	char filename[5];
	int ftime[6]; // 파일 시간
	int fsize; // 파일 크기
	int datablocknum; // 데이터블록 정보
	int singlenum;
	int doublenum;
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
void CreateNewDir(tree* );
void CreateNewFile(tree* );
tree* CurrentFile(tree* , int);

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
			datablock[k].data[i] = '\0';
	}
    // 루트 설정
	super_i_up();
	super_d_up();
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
	// 불러오기
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
			for(i = 0; i < 1000; i++)
				typ[i] = '\0';
			if(access("myfs", F_OK) == -1){ 
				printf("error : no myfs\n");
				while(x != 1){
					printf("$");
					scanf("%[^\n]", &typ);
					getchar();
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
			if(!strncmp(typ,"mycheck", 7)){
				printf("%d\n", InodeNumber);
			}
//mycd 명령
			if(!strncmp(typ,"mycd", 4)){
				int n, i, l = 100, k = 0, m = 0;
				char checkname[5];
				
				if(!(((typ[5]>=48)&&(typ[5]<=57))||((typ[5]>=65)&&(typ[5]<=90))||((typ[5]>=97)&&(typ[5]<=122)))){//문자 숫자가 아니게 만들기
					InodeNumber = 1;
					strcpy(where, "/");
					where[1] = '\0';
				}
				else{
					if(!(((typ[8]>=48)&&(typ[8]<=57))||((typ[8]>=65)&&(typ[8]<=90))||((typ[8]>=97)&&(typ[8]<=122)))){//문자 숫자가 아니게 만들기
						printf("Please write more than 3!\n");
					}
					else{
						strncpy(checkname, &typ[5], 4);
						n = inode[InodeNumber - 1].datablocknum;
						while(strncmp(&(datablock[n-1].data[m*5]), checkname, 4)){
							m++;
							if(m == 25)
								break;
						}
						if(m == 25){
							printf("No Directory file!\n");
						}
						else{
							while(strncmp(inode[k].filename, checkname, 4)){
								k++;
							}
							InodeNumber = k+1;
							printf("1\n");
							while(where[l - 1] != '/'){ 
								l--;
							}
							printf("2\n");
							strncpy(&where[l], inode[k].filename, 4);
							printf("3\n");
							where[l + 4] = '/';
							where[l + 5] = '\0';
							
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
				char checkname[5];
				int n, l = 0, k = 0, m = 0;
				int i, d;
				if(!(((typ[11]>=48)&&(typ[11]<=57))||((typ[11]>=65)&&(typ[11]<=90))||((typ[11]>=97)&&(typ[11]<=122)))){//문자 숫자가 아니게 만들기
					printf("Please write more file name.\n");
				}
				else{
					strncpy(checkname, &typ[8], 4);
					n = inode[InodeNumber - 1].datablocknum;
					while(strncmp(&(datablock[n-1].data[m*5]), checkname, 4)){
							m++;
							if(m == 25)
								break;
					}
					if(m == 25){
						while(datablock[n-1].data[l*5] != '\0'){	
							l++;
							if(l == 25)
								break;
						}
						if(l != 25){
						//트리 추가
					/*	if(InodeNumber == 1){
							CreateNewDir(RootNode);
						}
						else{
							Temporary_Node = CurrentFile(RootNode, InodeNumber);
							CreateNewDir(Temporary_Node);
						}*/
						//파일 설정
						i = super_i_up();	
						d = super_d_up();
						strcpy(inode[i-1].filename, checkname);
						strcpy(file[i-1].name, checkname);
						inode[i-1].ftype = 1;
						inode[i-1].datablocknum = d;
						inode[i-1].fsize = 0;
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
							printf("Can't make more file!\n");
					}
					else{
						printf("Already exist file!\n");	
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
				char checkname[5];
				int n, l = 0, k = 0, m = 0;
				int i, d;
				if(!(((typ[11]>=48)&&(typ[11]<=57))||((typ[11]>=65)&&(typ[11]<=90))||((typ[11]>=97)&&(typ[11]<=122)))){//문자 숫자가 아니게 만들기
					printf("Please write more file name.\n");
				}
				else{
					strncpy(checkname, &typ[8], 4);
					n = inode[InodeNumber - 1].datablocknum;
					while(strncmp(&(datablock[n-1].data[m*5]), checkname, 4)){
							m++;
							if(m == 25)
								break;
					}
					if(m == 25){
						while(datablock[n-1].data[l*5] != '\0'){	
							l++;
							if(l == 25)
								break;
						}
						if(l != 25){
						//트리 추가
					/*	if(InodeNumber == 1)
							CreateNewFile(RootNode);
						else{
							Temporary_Node = CurrentFile(RootNode, InodeNumber);
							CreateNewFile(Temporary_Node);
						}*/
						//파일 설정
						i = super_i_up();	
						d = super_d_up();
						strcpy(inode[i-1].filename, checkname);
						strcpy(file[i-1].name, checkname);
						inode[i-1].ftype = 0;
						inode[i-1].datablocknum = d;
						inode[i-1].fsize = 0;
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
							printf("Can't make more file!\n");
					}
					else{
						while(strncmp(inode[k].filename, checkname, 4)){
							k++;
						}
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
				int size, k, findnum = 0;
				if((typ[12] >= 48)&&(typ[12] <= 57)){
					if(!((typ[13] >= 48)&&(typ[13] <= 57)))
						findnum = typ[12] - 48;
					else if(!((typ[14] >= 48)&&(typ[14] <= 57))){
						findnum = typ[13] - 48;
						findnum += (typ[12] - 48) * 10;
					}
					else if(!((typ[15] >= 48)&&(typ[15] <= 57))){
						findnum = typ[14] - 48;
						findnum += (typ[13] - 48)*10;
						findnum += (typ[12] - 48)*100;
					}

				printf("아이노드 번호 : %d\n", inode[findnum-1].inodenum);
				if(inode[findnum - 1].ftype == 0)
					printf("파일 종류 : 일반 파일\n");
				else
					printf("파일 종류 : 디렉터리 파일\n");
				printf("파일 이름 : %s\n", inode[findnum-1].filename);
				printf("파일 시간 : %d년 %d월 %d일 %d시 %d분 %d초\n", inode[findnum-1].ftime[0], inode[findnum -1].ftime[1], inode[findnum-1].ftime[2], inode[findnum-1].ftime[3], inode[findnum-1].ftime[4], inode[findnum-1].ftime[5]);
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
				if(inode[findnum-1].singlenum != 0)
					printf(", %d", inode[findnum -1].singlenum);
				if(inode[findnum-1].doublenum != 0)
					printf(", %d",inode[findnum -1].doublenum);
				printf("\n");
				}
				else
					printf("Please write number!\n");
			}
//myshowblock 명령
			if(!strncmp(typ,"myshowblock", 11)){
				int i, findnum = 0;	
				if((typ[12] >= 48)&&(typ[12] <= 57)){
					if(!((typ[13] >= 48)&&(typ[13] <= 57)))
						findnum = typ[12] - 48;
					else if(!((typ[14] >= 48)&&(typ[14] <= 57))){
						findnum = typ[13] - 48;
						findnum += (typ[12] - 48) * 10;
					}
					else if(!((typ[15] >= 48)&&(typ[15] <= 57))){
						findnum = typ[14] - 48;
						findnum += (typ[13] - 48)*10;
						findnum += (typ[12] - 48)*100;
					}
					else if(!((typ[16] >= 48)&&(typ[15] <= 57))){
						findnum = typ[15] - 48;
						findnum += (typ[14] - 48)*10;
						findnum += (typ[13] - 48)*100;
						findnum += (typ[12] - 48)*1000;
					}
					for(i = 0; i < 128; i++)
						putchar(datablock[findnum-1].data[i]);
					putchar('\n');
				}
				else
					printf("Please write number!\n");
			}
//mystate 명령
			if(!strncmp(typ,"mystate", 7)){
				int icount = 0, dcount = 0;
				int i, k, v;
				unsigned int sap;
				unsigned int temp;
				sap = 1;
				sap = sap << 31;
				for( v = 0; v < 16; v++){
					temp = superblock.inodeuse[v];
					for( k = 0; k < 32; k++){
						if((sap & temp) == 0){
							icount++;
							}
						temp = temp << 1;
					}
				}
				printf("free inode : %d\n", icount);
				for( v = 0; v < 32; v++){
					temp = superblock.datause[v];
					for( k = 0; k < 32; k++){
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
		
	free(Temporary_Node);
	free(RootNode);
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
			v++;
			comp = superblock.inodeuse[v];
		}
	}
	if((comp & to) == 0){ 
		comp = to;
		comp = comp >> s;
		superblock.inodeuse[v] += comp;
		k = 0;
	}
	if(v == 16)
		k = 0;
	num = v*32 + (s % 32) + 1;
	}
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
			v++;
			comp = superblock.datause[v];
		}
	}
	if((comp & to) == 0){ 
		comp = to;
		comp = comp >> s;
		superblock.datause[v] += comp;
		k = 0;
	}
	if(v == 32)
		k = 0;
	num = v*32 + (s % 32) + 1;
	}
	return num;
}
// 인자에 원하는 아이노드 번호를 넣으면 해당 슈퍼블록 정보 변경
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
// 인자에 원하는 데이터블록 번호를 넣으면 해당 슈퍼블록 정보 변경
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
tree* CurrentFile(tree* RootNode, int InodeNum){ //찾고자 하는 파일의 노드 찾는 함수
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
