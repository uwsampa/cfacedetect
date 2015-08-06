
#ifndef FACE_H_
#define FACE_H_



#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

//Structure for storing a face with x, y and window size
typedef struct Face {
    int window;
    int x;
    int y;
    struct Face* next;
} Face;


Face* push(Face* head, int window, int x, int y);

bool contains(Face* face, Face* head);

bool equals(Face* rect1, Face* rect2);

bool overlap(Face* rect1, Face* rect2);

Face* deleteNext(Face* node);

void printfree(Face* head);


#endif