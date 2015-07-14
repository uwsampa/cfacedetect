
#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xpath.h>

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

Cascade loadCascade(char* path) {
	xmlDocPtr doc;
	xmlXPathObjectPtr result;
	
	doc = xmlParseFile(path);
	if(doc == NULL) {
		printf("xml path not found\n");
		return 0;
	}

	unsigned stageNum;
	unsigned featureNum;

	Stage* stages;
	Feature* features;

	result = getnodeset(doc, (xmlChar *)"opencv_storage/cascade/stageNum/_");
	stageNum = result->nodesetval->nodeNr;
	stages = (Stage *)malloc(sizeof(Stage) * stageNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"opencv_storage/cascade/features/_");
	featureNum = result->nodesetval->nodeNr;
	features = (Feature *)malloc(sizeof(Feature) * featureNum);
	xmlXPathFreeObject(result);

	result = getnodeset(doc, (xmlChar *)"opencv_storage/cascade/height");
	Cascade cas = {atoi((char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content), stages, features};
	xmlXPathFreeObject(result);

	xmlChar *xpath;
	//first process the stage and its nodes
	unsigned i, j;

	for(i = 0; i < stageNum; i++) {
		xpath = (xmlChar *)"opencv-storage/cascade/stages/_[" + (i+1) + "]/stageThreshold";
		result = getnodeset(doc, xpath);
		stages[i]->threshold = atof((char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content);
		for(j = 0; j < ?(variable, depends on how many nodes per stage); j++) {
	//now can access stages as an array
		}
		xmlXPathFreeObject(result);
	}

	//second process the feature
/*	unsigned k, l;

	for (k = 0; k < featureNum; k++) {
		for (l = 0; l < ?(depends on how many rects); l++) {

		}
	}

	/*
	Stage curstage;//those can be local variable instead of global
	node curnode;
	
	unsigned fcount = 0;//not sure if this is necessary
	int total_features;//this also maybe deleted
	bool ins = FALSE;
	bool inr = FALSE;
	bool nf = FALSE;
	bool ns = FALSE;
	size_t len = 0;
	char* line = NULL;
	ssize_t read;

	//first process the stage

	/*while(getline(&line, &len, fp) != -1) {

		if (strstr(line, "<stages") != NULL) {
			ins = TRUE;
			inr = FALSE;
			continue;
		}
		if (strstr(line, "<features") != NULL) {
			ins = FALSE;
			inr = TRUE;
			nf = TRUE;
			continue;
		}

		if (ins) {
			if (ns) {
				strtok(line, ">");
    			int num = atoi(strtok(NULL, "<"));
				curstage = {0.0, num, NULL);//when reference change here, if already appended to stages will it erase the stage before?
				ns = FALSE;

			} else if (strstr(line, "stageThreshold") != NULL) {
				char* c = strtok(line, ">");
				curstage.threshold = atof(strtok(NULL, "<"));

			} else if (strstr(line, "/internalNodes") != NULL) {
					strtok(line, " ");
					strtok(NULL, " ");
					int featind = atoi(strtok(NULL, " "));
					float threshold = atof(strtok(NULL, "<"));
					curnode = {threshold, featind, [0.0, 0.0]};

			} else if (strstr(line, "/leafValues") != NULL) {
					curnode.weights[0] = atof(strtok(line, " "));
					curnode.weights[1] = atof(strtok(line, "<"));
					curstage.node_list.append(curr_node)//how should i append the nodes?
			}

			if (strstr(line, "<_>") != NULL) { 
				if (len(line.split(" ")) == 5) {//not sure if necessary
					ns = TRUE;
				}

			} else if (strstr(line, "</_>") != NULL) {
				if (strstr(line, "</_></weakClassifiers></_>") != NULL) {
					stages.append(curr_stage);
				}
			}

		} else if (inRects) {
			if (strstr(line, "<_>") != NULL || strstr(line, "<rects>") != NULL) {
				continue;
			}
				
			if (nf) {
				nf = FALSE;
					//otherway to initialize curr_rectlist = []
			}
			if (strstr(line, "</_>") != NULL) {
				int width = strtok(line, " ");
				int height = strtok(NULL, " ");
				int x = strtok(NULL, " ");
				int y = strtok(NULL, " ");
				int weight = strtok(NULL, ".");
				rect currect = {width, height, x, y, weight};
				rect* rectlist.append(currect) 
				if (strstr(line, "</_></rects></_>") != NULL) {
					Feature curfeat = {fcount, rectlist};
					features.append(curfeat)
					fcount++;
					nf = TRUE;
				}

			} else if (strstr(line, "</opencv_storage>") != NULL) {
				continue;
			}

		} else {
				// pre processing
				if (strstr(line, "height") != NULL) {
					// wow seriously
					strtok(line, ">");
					cas.dim = atoi(strtok(NULL, "<"));
				}
				if (strstr(line, "</maxWeakCount></stageParams>") {
					strtok(line, ">");
					total_features = atoi(strtok(NULL, "<"));
				}
		}
	} */
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return cas;

}