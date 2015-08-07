
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

//Loading from xml and returning the cascade ptr
Cascade* loadCascade(char* path) {
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
	Feature* features = NULL;

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/stages/_");
	stageNum = result->nodesetval->nodeNr;
	stages = (Stage *)malloc(sizeof(Stage) * stageNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/features/_");
	featureNum = result->nodesetval->nodeNr;
	features = (Feature *)malloc(sizeof(Feature) * featureNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"/opencv_storage/cascade/height");
	Cascade* cas = (Cascade *)malloc(sizeof(Cascade));
	cas->dim = atoi(VALUE(result));
	cas->stgNum = stageNum;
	cas->featNum = featureNum;
	cas->stages = stages;
	cas->features = features;
	xmlXPathFreeObject(result);


	char xpath[256];
	//first process the stage and its nodes, done
	int i, j;

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

	//second process the feature, done
	int k, l;

	for (k = 0; k < featureNum; k++) {
		//see how many rects in each feature
		features[k].id = k;
		sprintf(xpath, "%s%d%s", "/opencv_storage/cascade/features/_[", k+1, "]/rects/_");
		result = getnodeset(doc, (xmlChar *)&xpath);
		int rectNum = result->nodesetval->nodeNr;
		features[k].rectNum = rectNum;
		xmlXPathFreeObject(result);
		features[k].rectList = (Rect *)malloc(sizeof(Rect) * rectNum);
		for (l = 0; l < rectNum; l++) {
			//construct each rects for feature's rectlist
			sprintf(xpath, "%s%d%s%d%s", "/opencv_storage/cascade/features/_[", k+1, "]/rects/_[", l+1, "]");
			result = getnodeset(doc, (xmlChar *)xpath);
			char *line = VALUE(result);
			strtok(line, " ");
			features[k].rectList[l].width = atoi(strtok(NULL, " "));
			features[k].rectList[l].height = atoi(strtok(NULL, " "));
			features[k].rectList[l].x = atoi(strtok(NULL, " "));
			features[k].rectList[l].y = atoi(strtok(NULL, " "));
			features[k].rectList[l].weight = atoi(strtok(NULL, "."));
			xmlXPathFreeObject(result);
		}
	}

	for (i = 0; i < stageNum; ++i)
	{
		for (j = 0; j < stages[i].nodeNum; ++j)
		{
			int index = stages[i].nodeList[j].featind;
			Feature featAtIndex = features[index];
			stages[i].nodeList[j].feat = (Feature *)malloc(sizeof(Feature));
			// Copy the features into stages
			stages[i].nodeList[j].feat->rectNum = featAtIndex.rectNum;
			stages[i].nodeList[j].feat->rectList = (Rect *)malloc(sizeof(Rect) * featAtIndex.rectNum);
			for (l = 0; l < featAtIndex.rectNum; ++l)
			{
				stages[i].nodeList[j].feat->rectList[l].width  = features[j].rectList[l].width;
				stages[i].nodeList[j].feat->rectList[l].height = features[j].rectList[l].height;
				stages[i].nodeList[j].feat->rectList[l].x      = features[j].rectList[l].x;
				stages[i].nodeList[j].feat->rectList[l].y      = features[j].rectList[l].y;
				stages[i].nodeList[j].feat->rectList[l].weight = features[j].rectList[l].weight;
			}
		}
	}

	// Free the temperary rectList and features
	for(i = 0; i < featureNum; i++) {
		// free rectList first
		free(features[i].rectList);
		features[i].rectList = NULL;
	}
	// free features second
	free(features);
	features = NULL;

	xmlFreeDoc(doc);
	xmlCleanupParser();
	printf("Cascade built.\n");
	return cas;
}