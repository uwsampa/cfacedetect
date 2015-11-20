
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "parse.h"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xpath.h>

/// evaluates the content in node
#define VALUE(X) (char *)X->nodesetval->nodeTab[0]->xmlChildrenNode->content

/** Retrieve the wanted nodeset from doc
  * @param[in] doc xml document
  * @param[in] xpath path to the nodeset
  * @return an xmlptr points to the wanted nodeset
  */
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

/** Freeing the cascade from memory
  * @param[in] cas pointer to cascade
  */
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
}

/** Load the cascade from path
  * @pre only one cascade in a xml file
  * @param[in] path file path to xml file
  * @return pointer to the loaded cascade
  */
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

	// Filling the cascade with stages and nodes
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
			stages[i].nodeList[j].rectNum = -1;
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

	struct Feature* featList = malloc(sizeof(Node)*featureNum);

	// Filling the nodes with rectangles from the features
	for (i = 0; i < featureNum; ++i) {

		// Node* thisNode = NULL;
		// for (j = 0; j < stageNum; ++j) {
		// 	for (k = 0 ; k < stages[j].nodeNum; ++k) {
		// 		if (i == stages[j].nodeList[k].featind) {
		// 			thisNode = &(stages[j].nodeList[k]);
		// 			// k = stages[j].nodeNum;
		// 			// j = stageNum;
		// 			break;
		// 		}
		// 	}
		// }

		sprintf(xpath, "%s%d%s", "/opencv_storage/cascade/features/_[", i+1, "]/rects/_");
		result = getnodeset(doc, (xmlChar *)&xpath);
		int rectNum = result->nodesetval->nodeNr;
		// printf("id=%d, rectNum=%d\n", i, rectNum);
		// thisNode->rectNum = rectNum;
		featList[i].rectNum = rectNum;
		xmlXPathFreeObject(result);
		for (j = 0; j < rectNum; ++j) {
			//construct each rects for feature's rectlist
			sprintf(xpath, "%s%d%s%d%s", "/opencv_storage/cascade/features/_[", i+1, "]/rects/_[", j+1, "]");
			result = getnodeset(doc, (xmlChar *)xpath);
			char *line = VALUE(result);
			strtok(line, " ");
			// thisNode->rectList[j].width = atoi(strtok(NULL, " "));
			// thisNode->rectList[j].height = atoi(strtok(NULL, " "));
			// thisNode->rectList[j].x = atoi(strtok(NULL, " "));
			// thisNode->rectList[j].y = atoi(strtok(NULL, " "));
			// thisNode->rectList[j].weight = atoi(strtok(NULL, "."));
			featList[i].rectList[j].width = atoi(strtok(NULL, " "));
			featList[i].rectList[j].height = atoi(strtok(NULL, " "));
			featList[i].rectList[j].x = atoi(strtok(NULL, " "));
			featList[i].rectList[j].y = atoi(strtok(NULL, " "));
			featList[i].rectList[j].weight = atoi(strtok(NULL, "."));
			// printf("\t%d %d %d %d %d\n", featList[i].width, featList[i].height, \
			// 	featList[i].x, featList[i].y, featList[i].weight);
			xmlXPathFreeObject(result);
		}
		// thisNode = NULL;
	}

	#if DEBUG
		printf("Cascade dim:     %d\n", cas->dim);
		printf("Cascade stgNum:  %d\n", cas->stgNum);
		printf("Cascade featNum: %d\n", cas->featNum);
	#endif
	int featind = 0;
	for (i = 0; i < stageNum; ++i) {
		#if DEBUG
			printf("\tStage thrs:    %f\n", stages[i].threshold);
			printf("\tStage nodeNum: %d\n", stages[i].nodeNum);
		#endif
		for (j = 0 ; j < stages[i].nodeNum; ++j) {
			featind = stages[i].nodeList[j].featind;
			stages[i].nodeList[j].rectNum = featList[featind].rectNum;
			#if DEBUG
				printf("\t\tNode thrs:     %f\n", stages[i].nodeList[j].threshold);
				printf("\t\tNode featind:  %d\n", stages[i].nodeList[j].featind);
				printf("\t\tNode rectNum:  %d\n", stages[i].nodeList[j].rectNum);
			#endif
			for (k = 0; k < stages[i].nodeList[j].rectNum; ++k) {
				stages[i].nodeList[j].rectList[k].width = featList[featind].rectList[k].width;
				stages[i].nodeList[j].rectList[k].height = featList[featind].rectList[k].height;
				stages[i].nodeList[j].rectList[k].x = featList[featind].rectList[k].x;
				stages[i].nodeList[j].rectList[k].y = featList[featind].rectList[k].y;
				stages[i].nodeList[j].rectList[k].weight = featList[featind].rectList[k].weight;
				#if DEBUG
					printf("\t\t\tRecangle width:  %d\n", stages[i].nodeList[j].rectList[k].width);
					printf("\t\t\tRecangle height: %d\n", stages[i].nodeList[j].rectList[k].height);
					printf("\t\t\tRecangle x:      %d\n", stages[i].nodeList[j].rectList[k].x);
					printf("\t\t\tRecangle y:      %d\n", stages[i].nodeList[j].rectList[k].y);
					printf("\t\t\tRecangle weight: %d\n", stages[i].nodeList[j].rectList[k].weight);
				#endif
			}
			for (k=0; k<2; k++) {
				#if DEBUG
					printf("\t\t\tWeight:          %f\n", stages[i].nodeList[j].weights[k]);
				#endif
			}
		}
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return cas;
}