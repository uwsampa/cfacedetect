

#ifndef PARSE_H_
#define PARSE_H_

typedef struct Rect{
	int width;
	int height;
	int x;
	int y;
	int weight;//maybe
}Rect;

typedef struct Feature{
	int id;
	Rect* rectList;
}Feature;

typedef struct Node{
	float threshold;
	int featind;
	float weights[2];
}Node;

typedef struct Stage{
	float threshold;
	int fsize;
	Node* nodeList;
}Stage;

typedef struct Cascade{
	int dim;
	Stage* stages;
	Feature* features;
}Cascade;



//Cascade* loadCascade(char* path);

#endif