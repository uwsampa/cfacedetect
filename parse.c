
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xpath.h>

#define VALUE(X) (char *)X->nodesetval->nodeTab[0]->xmlChildrenNode->content

//Returns an xmlptr which returns the given nodeset
xmlXPathObjectPtr getnodeset (xmlDocPtr doc, xmlChar *xpath) {
	
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext(doc);
	if (context == NULL) {
		printf("Error in xmlXPathNewContext\n");
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath, context);
	xmlXPathFreeContext(context);
	if (result == NULL) {
		printf("Error in xmlXPathEvalExpression\n");
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
        printf("No result\n");
		return NULL;
	}
	return result;
}

//freeing the cascade
void freeCascade(Cascade* cas) {
	int i;
	for(i = 0; i < cas->stgNum; i++) {
		free(cas->stages[i].nodeList);
		cas->stages[i].nodeList = NULL;
	}
	free(cas->stages);
	cas->stages = NULL;
	free(cas);
	cas = NULL;
	printf("Cascade freed.\n");
}

// Loading from xml and returning the cascade ptr
// Pre: Only one cascade in a xml file
// Feature number of a node is not sorted
Cascade* loadCascade(char* path) {
	int i, j, k;
	xmlDocPtr doc;
	xmlXPathObjectPtr result;
	
	doc = xmlParseFile(path);
	if(doc == NULL) {
		printf("Error: xml path not found\n");
		return NULL;
	}

	int stageNum;
	int featureNum;

	Stage* stages = NULL;

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/stages/_");
	stageNum = result->nodesetval->nodeNr;
	stages = (Stage *)malloc(sizeof(Stage) * stageNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/features/_");
	featureNum = result->nodesetval->nodeNr;
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/height");
	Cascade* cas = (Cascade *)malloc(sizeof(Cascade));
	cas->dim = atoi(VALUE(result));
	cas->stgNum = stageNum;
	cas->featNum = featureNum;
	cas->stages = stages;
	xmlXPathFreeObject(result);

	char xpath[256];
	//first process the stage and its nodes, done

	for(i = 0; i < stageNum; i++) {
		//stage threshold
		sprintf(xpath, "%s%d%s", "/opencv_storage/cascade/stages/_[", i+1, "]/stageThreshold");
		result = getnodeset(doc, (xmlChar *)&xpath);
		stages[i].threshold = atof((char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content);
		xmlXPathFreeObject(result);
		//nodes in each stage
		sprintf(xpath, "%s%d%s", "/opencv_storage/cascade/stages/_[", i+1, "]/maxWeakCount");
		result = getnodeset(doc, (xmlChar *)&xpath);
		int nodeNum = atoi((char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content);
		stages[i].nodeNum = nodeNum;
		xmlXPathFreeObject(result);
		stages[i].nodeList = (Node *)malloc(sizeof(Node) * nodeNum);
		for (j = 0; j < nodeNum; j++) {
			//for node featind and threshold
			sprintf(xpath, "%s%d%s%d%s", "/opencv_storage/cascade/stages/_[", i+1, "]/weakClassifiers/_[", j+1, "]/internalNodes");
			result = getnodeset(doc, (xmlChar *)&xpath);
			char *line = VALUE(result);
			strtok(line, " ");
			strtok(NULL, " ");
			strtok(NULL, " ");
			stages[i].nodeList[j].featind = atoi(strtok(NULL, " "));
			stages[i].nodeList[j].threshold = atof(strtok(NULL, ""));
			xmlXPathFreeObject(result);
			//for tree value
			sprintf(xpath, "%s%d%s%d%s", "/opencv_storage/cascade/stages/_[", i+1, "]/weakClassifiers/_[", j+1, "]/leafValues");
			result = getnodeset(doc, (xmlChar *)&xpath);
			line = VALUE(result);
			strtok(line, " ");
			stages[i].nodeList[j].weights[0] = atof(strtok(NULL, " "));
			stages[i].nodeList[j].weights[1] = atof(strtok(NULL, ""));
			xmlXPathFreeObject(result);
		}
	}

	// Start to reconstruct
	for (i = 0; i < featureNum; ++i) {
		Node* thisNode = NULL;
		for (j = 0; j < stageNum; ++j) {
			for (k = 0 ; k < stages[j].nodeNum; ++k) {
				if (i == stages[j].nodeList[k].featind) {
					thisNode = &(stages[j].nodeList[k]);
					k = stages[j].nodeNum;
					j = stageNum;
					break;
				}
			}
		}

		sprintf(xpath, "%s%d%s", "/opencv_storage/cascade/features/_[", i+1, "]/rects/_");
		result = getnodeset(doc, (xmlChar *)&xpath);
		int rectNum = result->nodesetval->nodeNr;
		thisNode->rectNum = rectNum;
		xmlXPathFreeObject(result);
		for (j = 0; j < rectNum; ++j) {
			//construct each rects for feature's rectlist
			sprintf(xpath, "%s%d%s%d%s", "/opencv_storage/cascade/features/_[", i+1, "]/rects/_[", j+1, "]");
			result = getnodeset(doc, (xmlChar *)xpath);
			char *line = VALUE(result);
			strtok(line, " ");
			thisNode->rectList[j].width = atoi(strtok(NULL, " "));
			thisNode->rectList[j].height = atoi(strtok(NULL, " "));
			thisNode->rectList[j].x = atoi(strtok(NULL, " "));
			thisNode->rectList[j].y = atoi(strtok(NULL, " "));
			thisNode->rectList[j].weight = atoi(strtok(NULL, "."));
			xmlXPathFreeObject(result);
		}
		thisNode = NULL;
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	printf("Cascade built.\n");
	return cas;
}