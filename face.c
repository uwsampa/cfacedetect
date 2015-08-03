#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "face.h"

//Append face(window, x, y) to the end of linked list starting from head
Face* push(Face* head, int window, int x, int y) {
    Face* temp = (Face *)malloc(sizeof(Face));
    temp->window = window;
    temp->x = x;
    temp->y = y;
    temp->next = NULL;
    Face* cur = head;
    if (head == NULL) {
        head = temp;
    } else {
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = temp;
    }
    return head;
}

//Returns true if face is within the linked list starting from head
bool contains(Face* face, Face* head) {
    Face* temp = head;
    while(temp != NULL) {
        if(equals(face, temp)) {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

//Returns true if rect1 equals rect2
bool equals(Face* rect1, Face* rect2) {
    return rect1->window == rect2->window && rect1->x == rect2->x && rect1->y == rect2->y;
}

//Returns true if rect1 and rect2  areas overlaps each other
bool overlap(Face* rect1, Face* rect2, Face* head) {
    int r1x1 = rect1->x;
    int r1x2 = r1x1 + rect1->window;
    int r1y1 = rect1->y;
    int r1y2 = r1y1 + rect1->window;

    int r2x1 = rect2->x;
    int r2x2 = r2x1 + rect2->window;
    int r2y1 = rect2->y;
    int r2y2 = r2y1 + rect2->window;

    if(r1x1 < r2x2 && r1x2 > r2x1 && r1y1 < r2y2 && r1y2 > r2y1) {
        int a1 = rect1->window * rect1->window;
        int a2 = rect2->window * rect2->window;
        int aIntersect = max(0, min(r1x2, r2x2) - max(r1x1, r2x1)) * max(0, min(r1y2, r2y2) - max(r1y1, r2y1));
        int aUnion = a1 + a2 - aIntersect;
        if ((float)(aIntersect) / aUnion > 0.4) {
            rect1->x = min(r1x1, r2x1);
            rect1->y = min(r1y1, r2y1);
            rect1->window = max(max(r1x2, r2x2) - rect1->x, max(r1y2, r2y2) - rect1->y);
            if (contains(rect2, head)) {
                return true;
            }
        }
    }
    return false;
}

//Delete the face from the linked list starting from head and
//returns the next face after the deleted one
Face* delete(Face* face, Face* head) {
    Face *temp, *prev;
    prev = head;
    temp = head;
    while(temp != NULL) {
    	if(equals(face, temp)) {
            if(temp == head) {
                //use an assertion here since by construct
                //we should not reach this point
                // assert(temp!=head);
                head = temp->next;
                free(temp);
                return head;
            } else {
                prev->next = temp->next;
                free(temp);
                return prev->next;
            }
            break;
        } else {
            prev = temp;
            temp = temp->next;
        }
    }
    return NULL;
}

//Prints all the faces from the linked list starting from head
//and free them accordingly
void printfree(Face* head) {
    while(head != NULL) {
        printf("[%d, %d, %d] | ", head->window, head->x, head->y);
        Face* temp = head;
        head = head->next;
        free(temp);
    }
}