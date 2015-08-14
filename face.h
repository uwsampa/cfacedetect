
#ifndef FACE_H_
#define FACE_H_



#define min(X, Y)  ((X) < (Y) ? (X) : (Y))
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

/// Linked list structure for storing a face with x, y and window size
typedef struct Face {
	/// window size
    int window;
    /// x coordinate
    int x;
    /// y coordinate
    int y;
    /// links to the next face
    struct Face* next;
} Face;


Face* push(Face* head, int window, int x, int y);

bool overlap(Face* rect1, Face* rect2);

Face* deleteNext(Face* node);

void printfree(Face* head);


#endif