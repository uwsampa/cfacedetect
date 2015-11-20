

#ifndef PARSE_H_
#define PARSE_H_

/// Rectangle object
typedef struct Rect{
	int width;
	int height;
	int x;
	int y;
	// rectangle weight {-1, 2, 3}
	int weight;
}Rect;

// Feature
typedef struct Feature{
	// Number of rectangles
	int rectNum;
	// List of rectList
	Rect rectList[3];
}Feature;

// Node struct (weak classifier)
typedef struct Node{
	// threshold of the weak classifier
	float threshold;
	// Index for features
	int featind;
	// Number of rectangles
	int rectNum;
	// List of rectList
	Rect rectList[3];
	// leaf weights
	float weights[2];
}Node;

/// Stage struct (ensemble classifier)
typedef struct Stage{
	/// threshold of the ensemble classifier
	float threshold;
	/// number of weak classifiers
	int nodeNum;
	/// list of weak classifiers
	Node* nodeList;
}Stage;

/// Cascade struct
typedef struct Cascade{
	/// size of the smallest window
	int dim;
	/// number of stages in classifier
	int stgNum;
	/// total features across all stages
	int featNum;
	/// array of stgNum stages
	Stage* stages;
}Cascade;


Cascade* loadCascade(char* path);

void freeCascade(Cascade *cas);

#endif