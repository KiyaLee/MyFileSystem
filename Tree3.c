
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int InodeNumber = 1;

typedef struct binary_tree{
	int inodenumber;
	int depth;	//길이
	int ftype;	//0: 일반파일, 1: 디렉터리파일
	int check;	//0: 나머지,  1: 구조체 -> Down
	struct binary_tree* Down;
	struct binary_tree* Right;
}tree;



//tree* CurrentDir(tree* RootNode, int InodeNum){ -> 불필요할듯
/*
tree* BeCurrentFile(tree* RootNode, int InodeNum){ //찾고자 하는 아이노드 넘버를 가진 파일을 연결해주는 노드(전 노드 찾기)
	if(InodeNum == 1)
		printf("error\n");
	if(RootNode -> Down -> inodenumber == 2){ //inodenumber가 1이라면 RootNode
		return RootNode;
	}
	if(RootNode -> Right -> inodenumber == InodeNum || RootNode -> Down -> inodenumber == InodeNum){
		return RootNode;
	}
	if(RootNode -> Right -> inodenumber != InodeNum	&& RootNode -> Down -> inodenumber != InodeNum)
		if(RootNode -> Right -> Right == NULL && RootNode -> Right -> Down != NULL)
			BeCurrentFile(RootNode -> Down, InodeNum); //반복
		if(RootNode -> Right -> Right != NULL && RootNode -> Right -> Down == NULL)
			BeCurrentFile(RootNode -> Right, InodeNum); //반복
		else if(RootNode -> Right -> Right != NULL && RootNode -> Right -> Down != NULL){
			BeCurrentFile(RootNode -> Right, InodeNum);
			BeCurrentFile(RootNode -> Down, InodeNum);
			}
		}
*/	//띠발...

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
				printf("e\n");
			
	}
	else if(Directory -> Down != NULL){//Directory -> Down 이 널이 아닐 때
		Directory = Directory -> Down;
			if(Directory -> Right != NULL){
				while(1){
				Directory = Directory -> Right;	
				printf("D");
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
			printf("%d\n", Current_Node -> inodenumber);
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
/*
void RemoveNode(tree* RootNode, int InodeNum){//삭제하고싶은 파일을 Node
	tree* Temporary_Node = (tree*)malloc(sizeof(tree));
	Temporary_Node -> Right = NULL;
	Temporary_Node -> Down = NULL;
	if(CurrentFile(RootNode, InodeNum) -> Down != NULL){ //Down이 존재한다면 삭제안됨
		printf("failed to remove : Directroy not empty");
	}
	if(CurrentFile(RootNode, InodeNum) -> Down == NULL && CurrentFile(RootNode, InodeNum) -> Right == NULL){ //노드 연결이 안되어 있다면
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
}*/
int main(void){

	int a, i;
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
	
	InodeNumber++; //2
	CreateNewFile(RootNode);
	InodeNumber++; //3
	CreateNewFile(RootNode);
	InodeNumber++; //4
	CreateNewDir(RootNode);
	InodeNumber++; //5
	CreateNewFile(RootNode);
	InodeNumber++; //6
	Temporary_Node = CurrentFile(RootNode, 4);
	CreateNewFile(Temporary_Node);
	InodeNumber++;//7
	CreateNewFile(Temporary_Node);
	//InodeNumber++; //8
	//CreateNewDir(Temporary_Node);
		
//	printf("%d\n", CurrentFile(RootNode  , 3)-> inodenumber);
//	printf("%d\n", CurrentFile(RootNode  , 2)-> inodenumber);
	printf("%d\n", CurrentFile(RootNode -> Down -> Right -> Right -> Down  , 7) -> inodenumber );
//	printf("%d\n", BeCurrentFile(RootNode, 2) -> inodenumber);

	free(Temporary_Node);
	free(RootNode);

	return 0;
}
