#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "face.h"
#include <assert.h>

/** Append face(window, x, y) to the end of linked list starting from head
  * @param head head of the list
  * @param window window size
  * @param x x coordinate
  * @param y y coordinate
  * @return the newly updated head
  */
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

/** Check if rect1 and rect2 overlap by more than 40%
  * @param rect1 face 1
  * @param rect2 face 2
  * @return true if overlaps, false otherwise
  */
bool overlap(Face* rect1, Face* rect2) {
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
			return true;
		}
	}
	return false;
}

/** Delete the face right after node(Note it cannot delete head)
  * @param node previous node before the deleted one
  * @return the node right after the deleted one
  */
Face* deleteNext(Face* node) {
	Face* nextnext;
	assert(node->next);
	nextnext = node->next->next;
	free(node->next);
	node->next = nextnext;
	return nextnext;
}

/** Prints all the faces from the linked list starting from head
  * and free them accordingly
  * @param head the head of list
  */
void printfree(Face* head) {
	while(head != NULL) {
		printf("[%d, %d, %d] | ", head->window, head->x, head->y);
		Face* temp = head;
		head = head->next;
		free(temp);
	}
}