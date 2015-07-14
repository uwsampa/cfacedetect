#include <stdio.h>
#include <stdlib.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/xpath.h>
#include "parse.h"

xmlXPathObjectPtr getNodeset (xmlDocPtr doc, xmlChar *xpath) {
	
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


int main() {
	/*int c1;
	int c2;
	float f;
	char *line = NULL;
	size_t len = 0;
	int num;
	FILE *fp = fopen("5x5.xml", "r");

	unsigned i;
	for (i = 0; i < 28; i++) {
		getline(&line, &len, fp);
	}

	printf("%s", line);

    c1 = atoi(strtok(line, " "));
    num = atoi(strtok(NULL, " "));
    c2 = atoi(strtok(NULL, " "));    
    f = atof(strtok(NULL, "<"));
    //num = 1;
    //c2 = "hello";

	printf("%d, %d, %d, %f", c1, num, c2, f);
	fclose(fp);*/
	xmlDocPtr doc;
	xmlXPathObjectPtr result;
	//xmlNodeSetPtr nodeset;
	char *xpath;
	char buf[256];
	//xmlChar *keyword;

	unsigned i;
	doc = xmlParseFile("5x5.xml");

	result = getNodeset(doc, (xmlChar *)"/opencv_storage/cascade/stages/_");
	int stageNum = result->nodesetval->nodeNr;
	xmlXPathFreeObject (result);

	printf("%d\n", stageNum);

	Stage* stages = (Stage *)malloc(sizeof(Stage) * stageNum);

	for (i = 0; i < stageNum; i++) {
		sprintf(buf, "%s%d%s", "opencv-storage/cascade/stages/_[", i+1, "]/stageThreshold");
		printf("%s\n", buf);
		xpath = (char *)&buf;
		result = getNodeset(doc, (xmlChar *)xpath);
		printf("%d: %s\n", i+1, (char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content);
		xmlXPathFreeObject(result);
	}
	/*if(result) {
		//nodeset = result->nodesetval;
		for(i = 0; i < result->nodesetval->nodeNr; i++) {
			printf("keyword: %d\n", result->nodesetval->nodeNr);//(char *)result->nodesetval->nodeTab[i]->xmlChildrenNode->content);
		}
				
	}*/
	free(stages);

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 0;
}
