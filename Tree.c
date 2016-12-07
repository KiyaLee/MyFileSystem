#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct filename{
	char name[5];
	//int inodeconnect;
}filename;

typedef struct inodemake{
	unsigned int ftype; // ftype = 0 is normal file, ftype = 1 is directory file 파일종류
	int inodenum; // inode 넘버
	int ftime; // 파일 시간
	int fsize; // 파일 크기
	int datablocknum; // 데이터블록 정보
}inodemake;

static inodemake inode[512];

typedef struct binary_tree{
	int Data;//데이터블록이 포인팅 했으면 좋겠는데 될까..?
	int inodenumber;//inodemake의 inodenum과 비교하기 위해?
	int depth;//길이
	struct binary_tree* Down = NULL;//길이가 바뀌는  자기참조 구조체
	struct binary_tree* Right = NULL;//길이가 같은 자기참조 구조체
}tree;

tree* CreateRoot(void){//루트 디렉터리 
	tree* NewRoot = (tree*)malloc(sizeof(tree)); //Node를 위한 동적 메모리 할당
	NewRoot -> Down = NULL;
	NewRoot -> Right = NULL;
	NewRoot -> depth = 0;
	NewRoot -> inodenumber = 1;
	inodemake.inodenum = 1; //고민
	return NewRoot;
}

void CreateNewFile(tree* parent, int data){//새로운 일반파일
	if(parent -> Right == NULL){//오른쪽에 위치한 Node가 NULL값일 때
		tree* CurrentNode = parent -> Right;//parent의 오른쪽에 Node 배정
		CurrentNode -> Data = data;
		(parent -> depth) + 1;//길이 증가
		(parent -> inodenumber)++;//inodenumber 증가
	}
	else if(parent -> Right != NULL){//NULL값이 아닐 때
		CreateNodeFile(parent -> Right); //NULL값이 나올 때까지 반복
	}
}

void CreateNew(tree* parent){//반복을 위한 함수
	if(parent -> Right == NULL){//parent의 오른쪽 Node가 NULL값일 때
		tree* CurrentNode = parent -> Right;//parent의 오른쪽에 Node 배정
		(parent -> inodenumber)++;//inodenumber 증가
	}
	else if(parent -> Right != NULL){//parent의 오른쪽 Node가 NULL값이 아닐 때
		CreateNodeFile(parent -> Right);//NULL값이 나올 때까지 반복
	}
}


void CreatNewDir(tree* parent){//새로운 디렉터리 파일
	if(parent -> Down == NULL){
		tree* CurrentNode = parent -> Down;
		(parent -> depth) + 1;
		(parent -> inodenumber)++;
	}
	else if(parent -> Down != NULL){
		if(parent -> Right == NULL){
			tree* CurrentNode = parent -> Right;
			(parent -> inodenumber)++;
		}
		else if(parent -> Right != NULL){
			CreateNew(parent -> Right);
		}
	}
}

void RemoveNode(tree* Node,){//수정해주세여 ㅎㅎㅎ
	tree* Temporary_Node;

	if(Node -> Down == NULL && Node -> Right == NULL){
		Temporaray_Node = Node;
		free(Temporary_Node);	
	}
	else if(Node -> Down == NULL && Node -> Right != NULL){
		RemoveNode(Node -> Right);
	}
	else if(Node -> Down != NULL){
		printf("failed to remove : Directory not empty");
	}
	
int main(){
	printf("[%s ]$", Node_Name);
	

	if(inode.ftype == 0)
		CreateNode();
	else if(inode.ftype == 1){


	








