#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct superblockmake{
	unsigned int inodeuse[16];
	unsigned int datause[32];
}superblockmake;

superblockmake superblock = { {0}, {0} };

int super_i_up();

int main(void){
	int i , p;
	p = 0;
	while(p != 1024){
		super_d_up();
		p++;
	}
	for(i = 0; i < 32; i++)
	printf("%u\n", superblock.datause[i]);
	return 0;
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
