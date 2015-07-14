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
	xmlChar *xpath;
	char buf[256];

	unsigned i;
	doc = xmlParseFile("5x5.xml");

	result = getNodeset(doc, (xmlChar *)"/opencv_storage/cascade/stages/_");
	int stageNum = result->nodesetval->nodeNr;
	xmlXPathFreeObject (result);

	printf("%d\n", stageNum);

	Stage* stages = (Stage *)malloc(sizeof(Stage) * stageNum);


	//result = getNodeset(doc, (xmlChar *)"/opencv_storage/cascade/stages/_[2]/stageThreshold");
	//printf("%s\n", (char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content);

	for (i = 0; i < stageNum; i++) {
		sprintf(buf, "%s%d%s", "/opencv_storage/cascade/stages/_[", i+1, "]/stageThreshold");
		xpath = (xmlChar *)&buf;
		printf("%s\n", (char *)xpath);
		result = getNodeset(doc, xpath);
		printf("%d: %s\n", i+1, (char *)result->nodesetval->nodeTab[0]->xmlChildrenNode->content);
		xmlXPathFreeObject(result);
	}
	
	free(stages);

	xmlFreeDoc(doc);
	xmlCleanupParser();

	return 0;
}
