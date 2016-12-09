#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int InodeNumber;

typedef struct binary_tree{
	int inodenumber;
	int depth;	//길이
	int ftype;	//0: 일반파일, 1: 디렉터리파일
	int check;	//0: 나머지,  1: 구조체 -> Down
	struct binary_tree* Down;
	struct binary_tree* Right;
}tree;

tree* CreateRoot(void){
	tree* NewRoot = (tree*)malloc(sizeof(tree));
	NewRoot -> Down = NULL;
	NewRoot -> Right = NULL;
	NewRoot -> inodenumber = 1; //루트의 inodenumber = 1
	NewRoot -> depth = 0; //루트의 길이 0
	NewRoot -> ftype = 1; //디렉터리 = 1
	NewRoot -> check = 0; 
	return NewRoot;
}

//tree* CurrentDir(tree* RootNode, int InodeNum){ -> 불필요할듯
		
tree* BeCurrentFile(tree* RootNode, int InodeNum){ //찾고자 하는 아이노드 넘버를 가진 파일을 연결해주는 노드(전 노드 찾기)
	if(RootNode -> inodenumber = 1){ //inodenumber가 1이라면 RootNode
		return RootNode;
	}
	if(RootNode -> Right -> inodenumber = InodeNum){
		return RootNode;
	}
	else if(RootNode -> Down -> inodenumber = InodeNum){
		return RootNode;
	}
	else{
		BeCurrentFile(RootNode -> Down, InodeNum); //반복
		BeCurrentFile(RootNode -> Right, InodeNum); //반복
		}
	}
tree* CurrentFile(tree* RootNode, int InodeNum){ //찾고자 하는 파일의 노드 찾는 함수
	if(RootNode -> inodenumber = InodeNum)
		return RootNode;
	else{
		CurrentFile(RootNode -> Down, InodeNum); //계속 반복
		CurrentFile(RootNode -> Right, InodeNum); //계속 반복
	}
	if(RootNode == NULL)//.....????? 이거 맞나
		return 0;
}


void CreateNewFile(tree* Directory){
	if(Directory -> Down == NULL){
		tree* Current_Node = (tree*)malloc(sizeof(tree));
		Current_Node = Directory -> Down;
		Current_Node -> Down = NULL; 
		Current_Node -> Right = NULL;
		Current_Node -> inodenumber = InodeNumber; //InodeNumber 
		Current_Node -> depth = (Directory -> depth) + 1; //Directory + 1
		Current_Node -> ftype = 0; //일반 파일은 0
		Current_Node -> check = 1; //구조체 -> Down = 1
	}
	else if(Directory -> Down != NULL){
		Directory = Directory -> Down;
		if(Directory -> Right == NULL){
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node = Directory -> Right;
			Current_Node -> Down = NULL; 
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber; //InodeNumbeㄱ
			Current_Node -> depth = Directory -> depth;
			Current_Node -> ftype = 0;//일반 파일은 0
		   	Current_Node -> check = 0;//구조체 -> Right = 0	
		}
		else if(Directory -> Right != NULL){
			while(Directory -> Right = NULL){
				Directory = Directory -> Right;
			}
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node = Directory -> Right; 
			Current_Node -> Down = NULL;
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber;
			Current_Node -> depth = Directory -> depth;
			Current_Node -> ftype = 0;
			Current_Node -> check = 0;
		}
	}
}

void CreateDir(tree* Directory){
	if(Directory -> Down == NULL){
		tree* Current_Node = (tree*)malloc(sizeof(tree));
		Current_Node = Directory -> Down;
		Current_Node -> Down = NULL; 
		Current_Node -> Right = NULL;
	   	Current_Node -> inodenumber = InodeNumber; //InodNumber 배정 필수
	   	Current_Node -> depth = (Directory -> depth) + 1;
	   	Current_Node -> ftype = 1; //디렉토리 파일은 1 
	   	Current_Node -> check = 1; //구조체 -> Down = 1 
	}
	else if(Directory -> Down != NULL){ //Down에 노드가 있을 때
		Directory = Directory -> Down;
		if(Directory -> Right == NULL){ //Right에 노드가 없을 때
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node = Directory -> Right;
			Current_Node -> Down = NULL; 
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber; //main함수에서 증가시킬거면 ++ 빼야됨.
			Current_Node -> depth = Directory -> depth;//같은 길이상에 있기에
			Current_Node -> ftype = 1; // 디렉터리 = 1
			Current_Node -> check = 0; // 구조체 -> Right = 0
		}
		else if(Directory -> Right != NULL){ //Right에 노드가 있을 경우
			while(Directory -> Right == NULL){ //Right 노드가 없을때까지
				Directory = Directory -> Right;
			}
			tree* Current_Node = (tree*)malloc(sizeof(tree));
			Current_Node = Directory -> Right;
			Current_Node -> Down = NULL; 
			Current_Node -> Right = NULL;
			Current_Node -> inodenumber = InodeNumber; //마찬가지
			Current_Node -> depth = Directory -> depth;
			Current_Node -> ftype = 1; //디렉터리 = 1
			Current_Node -> check = 0; //구조체 -> Right = 0
		}
	}
}

void RemoveNode(int InodeNum){//삭제하고싶은 파일을 Node
	tree* Temporary_Node = (tree*)malloc(sizeof(tree));
	Temporary_Node -> Right = NULL;
	Temporary_Node -> Down = NULL;
	if(CurrentFile(RootNode/*Root노드 구조체 입력해주세영*/, InodeNum) -> Down != NULL){ //Down이 존재한다면 삭제안됨
		printf("failed to remove : Directroy not empty");
	}
	if(Node -> Down == NULL && Node -> Right == NULL){ //노드 연결이 안되어 있다면
		Temporary_Node = CurrentFile(RootNode, InodeNum);
		free(Temporary_Node);
	}
	else{//연결되어 있는 노드가 존재한다면
		if(CurrentFile(RootNode, InodeNum) -> check == 0){
			Temporary_Node = CurrentFile(RootNode, InodeNum);
			Temporary_Node -> Right = CurrentFile(RootNode, InodeNum) -> Right;
			BeCurrentFile(RootNode, InodeNum) -> Right = Temporary_Node -> Right;
			free(Temporary_Node);
		}
		else if(CurrentFile(RootNode, InodeNum) -> check == 1){
			Temporary_Node = CurrentFile(RootNode, InodeNum);
			Temporary_Node -> Right = CurrentFile(RootNode, InodeNum) -> Right;
			BeCurrentFile(RootNode, InodeNum) -> Down = Temporary_Node -> Right;
			free(Temporary_Node);
		}
	}
}
