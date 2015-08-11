
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
	int i, j, k;
	for(i = 0; i < cas->stgNum; i++) {
		for (j = 0; j < cas->stages[i].nodeNum; ++j)
		{
			for (k = 0; k < cas->stages[i].nodeList[j].feat->rectNum; ++k)
			{
				free(cas->stages[i].nodeList[j].feat->rectList);
				cas->stages[i].nodeList[j].feat->rectList = NULL;
			}
			free(cas->stages[i].nodeList[j].feat);
			cas->stages[i].nodeList[j].feat = NULL;
		}
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
	// Feature* features = NULL;

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/stages/_");
	stageNum = result->nodesetval->nodeNr;
	stages = (Stage *)malloc(sizeof(Stage) * stageNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/features/_");
	featureNum = result->nodesetval->nodeNr;
	// features = (Feature *)malloc(sizeof(Feature) * featureNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/height");
	Cascade* cas = (Cascade *)malloc(sizeof(Cascade));
	cas->dim = atoi(VALUE(result));
	cas->stgNum = stageNum;
	cas->featNum = featureNum;
	cas->stages = stages;
	// cas->features = features; 			// Should be removed
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
			stages[i].nodeList[j].feat = (Feature *)malloc(sizeof(Feature));
			if (NULL == stages[i].nodeList[j].feat) { 
				printf("memory allocation for feature failed.\n"); 
			}
		}
	}

	// Start to reconstruct
	for (i = 0; i < featureNum; ++i) {
		Feature* thisFeature = NULL;
		for (j = 0; j < stageNum; ++j) {
			for (k = 0 ; k < stages[j].nodeNum; ++k) {
				if (i == stages[j].nodeList[k].featind) {
					thisFeature = stages[j].nodeList[k].feat;
					k = stages[j].nodeNum;
					j = stageNum;
					break;
				}
			}
		}

		sprintf(xpath, "%s%d%s", "/opencv_storage/cascade/features/_[", i+1, "]/rects/_");
		result = getnodeset(doc, (xmlChar *)&xpath);
		int rectNum = result->nodesetval->nodeNr;
		thisFeature->rectNum = rectNum;
		xmlXPathFreeObject(result);
		thisFeature->rectList = (Rect *)malloc(sizeof(Rect) * rectNum);
		for (j = 0; j < rectNum; ++j) {
			//construct each rects for feature's rectlist
			sprintf(xpath, "%s%d%s%d%s", "/opencv_storage/cascade/features/_[", i+1, "]/rects/_[", j+1, "]");
			result = getnodeset(doc, (xmlChar *)xpath);
			char *line = VALUE(result);
			strtok(line, " ");
			thisFeature->rectList[j].width = atoi(strtok(NULL, " "));
			thisFeature->rectList[j].height = atoi(strtok(NULL, " "));
			thisFeature->rectList[j].x = atoi(strtok(NULL, " "));
			thisFeature->rectList[j].y = atoi(strtok(NULL, " "));
			thisFeature->rectList[j].weight = atoi(strtok(NULL, "."));
			xmlXPathFreeObject(result);
		}
		thisFeature = NULL;
	}

	// int m, n, o;
	// for (m = 0; m < stageNum; ++m)
	// {
	// 	for (n = 0; n < stages[m].nodeNum; ++n)
	// 	{
	// 		int index = stages[m].nodeList[n].featind;
	// 		Feature featAtindex = features[index];
	// 		stages[m].nodeList[n].feat = (Feature *)malloc(sizeof(Feature));
	// 		// Copy the features into stages
	// 		stages[m].nodeList[n].feat->rectNum = featAtindex.rectNum;
	// 		stages[m].nodeList[n].feat->rectList = (Rect *)malloc(sizeof(Rect) * featAtindex.rectNum);
	// 		for (o = 0; o < featAtindex.rectNum; ++o)
	// 		{
	// 			stages[m].nodeList[n].feat->rectList[o].width  = featAtindex.rectList[o].width;
	// 			stages[m].nodeList[n].feat->rectList[o].height = featAtindex.rectList[o].height;
	// 			stages[m].nodeList[n].feat->rectList[o].x      = featAtindex.rectList[o].x;
	// 			stages[m].nodeList[n].feat->rectList[o].y      = featAtindex.rectList[o].y;
	// 			stages[m].nodeList[n].feat->rectList[o].weight = featAtindex.rectList[o].weight;
	// 		}
	// 	}
	// }

	// // Free the temperary rectList and features
	// for(i = 0; i < featureNum; i++) {
	// 	// free rectList first
	// 	free(features[i].rectList);
	// 	features[i].rectList = NULL;
	// }
	// // free features second
	// free(features);
	// features = NULL;

	xmlFreeDoc(doc);
	xmlCleanupParser();
	printf("Cascade built.\n");
	return cas;
}