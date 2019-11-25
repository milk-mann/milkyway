/**
Paul Wasilewicz
1007938
Citation: Used xmlTreeParser.c from xml library
**/

#include "GPXParser.h"
#include "LinkedListAPI.h"
#include <string.h>

typedef struct {
	float lat;
	float lon;
}Coordinates;


Waypoint* wpParser(xmlNode * node);
Route* rParser(xmlNode* node);
Track* tParser(xmlNode* node);
TrackSegment* tsParser(xmlNode* node);
Attribute* attrParser(xmlNode* node);
xmlDocPtr docToTree(GPXdoc* doc);
float calculateHaversine(Coordinates* points, int length);
Route* rteDeepCopy(Route* toCopy);
Track* trackDeepCopy(Track* toCopy);
TrackSegment* trkSegDeepCopy(TrackSegment* toCopy);
Waypoint* wpDeepCopy(Waypoint* toCopy);
Attribute* attrDeepCopy(Attribute* toCopy);
char* fileLogWrapper(char* fileName);
char* routeViewPanelWrapper(char* fileName);
char* trackViewPanelWrapper(char* fileName);
int writeWrapper(char* fileName, float version, char* creator);
int addRouteWrapper(char* filename, float lat, float lon, int check);
char* getBetweenRouteWrapper(char* filename,float latS, float lonS, float latE, float lonE, float accur);
char* getBetweenTrackWrapper(char* filename,float latS, float lonS, float latE, float lonE, float accur);
char* renameRouteWrapper(char* filename, char* newName,int index);
char* renameTrackWrapper(char* filename, char* newName,int index);
char* pointTable(char* fileName);
char* pointTableList(List *list);
char* waypoint(Waypoint* wp);



static void print_element_names(xmlNode * a_node,GPXdoc* doc){
		Waypoint* wpNode = NULL;
		Route* rouNode = NULL;
		Track* trkNode = NULL;
    xmlNode *cur_node = NULL;

		strcpy(doc->namespace, "http://www.topografix.com/GPX/1/1");

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
					if (xmlStrcmp(cur_node->name,(const xmlChar*)"wpt")==0) {//for wpt nodes
						wpNode = wpParser(cur_node);
						insertBack(doc->waypoints,wpNode);
					}
					if (xmlStrcmp(cur_node->name,(const xmlChar*)"rte")==0) {
						rouNode = rParser(cur_node);
						insertBack(doc->routes,rouNode);
					}
					if (xmlStrcmp(cur_node->name,(const xmlChar*)"trk")==0) {
						trkNode = tParser(cur_node);
						insertBack(doc->tracks,trkNode);
					}
					if (xmlStrcmp(cur_node->name,(const xmlChar*)"gpx") == 0) {

						xmlAttr* attribute = cur_node->properties;
            while(attribute){
              xmlChar* value = xmlNodeListGetString(cur_node->doc, attribute->children, 1);
							if (xmlStrcmp(attribute->name,(const xmlChar*)"version")==0) {
								doc->version = atof((char*)value);
							}
							if (xmlStrcmp(attribute->name, (const xmlChar*)"creator")==0) {
								//doc->creator = malloc(sizeof(char)*200);
								strcpy(doc->creator, (char *)value);
							}
              xmlFree(value);

              attribute = attribute->next;
            }
						print_element_names(cur_node->children,doc);
          }//endof gpx a_node
        }
    }
}


GPXdoc* createGPXdoc(char* fileName){


	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	doc = xmlReadFile(fileName, NULL, 0);

	LIBXML_TEST_VERSION

	if (doc == NULL) {
			printf("error: could not parse file %s\n", fileName);
			xmlFreeDoc(doc);
			xmlCleanupParser();
			return NULL;
	}

	root_element = xmlDocGetRootElement(doc);

	if(root_element == NULL){
		xmlFreeDoc(doc);
		xmlCleanupParser();
		return NULL;
	}

	GPXdoc* gpx = NULL;
	gpx = (GPXdoc*)malloc(sizeof(GPXdoc));
	gpx->creator = malloc(sizeof(char)*200);
	gpx->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
	gpx->routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
	gpx->tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);

	print_element_names(root_element,gpx);

	xmlFreeDoc(doc);

	xmlCleanupParser();

	return gpx;
}

Waypoint* wpParser(xmlNode * node){

	Waypoint* wNode = (Waypoint*)malloc(sizeof(Waypoint));
	wNode->name = (char*)malloc(sizeof(char)*256);
	strcpy(wNode->name,"");
	Attribute* attNode = NULL;
	wNode->attributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);

	xmlAttr* attribute = node->properties;
	while(attribute){
		xmlChar* value = xmlNodeListGetString(node->doc, attribute->children, 1);
		if (xmlStrcmp(attribute->name,(xmlChar*)"lon")==0) {
			wNode->longitude = atof((char*)value);
		}
		if (xmlStrcmp(attribute->name,(xmlChar*)"lat")==0) {
			wNode->latitude = atof((char*)value);
		}
		xmlFree(value);

		attribute = attribute->next;
	}

	xmlNode* temp = NULL;

	if (node->children != false){
			for(temp = node->children;temp;temp = temp->next){
				if (strcmp((char*)temp->name, "name")==0) {
					if(temp->children != NULL){
						strcpy(wNode->name,(char*)temp->children->content);
					}
				}
				else if(strcmp((char*)temp->name,"text")!=0){
					attNode = attrParser(temp);
					insertBack(wNode->attributes,attNode);
				}
		}
	}
	return wNode;
}

Route* rParser(xmlNode* node){
	Route* rNode = (Route*)malloc(sizeof(Route));
	rNode->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
	rNode->attributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
	rNode->name = (char*)malloc(sizeof(char)*256);
	strcpy(rNode->name,"");
	Waypoint* wNode = NULL;
	Attribute* attNode = NULL;

	xmlNode* temp = NULL;

	if (node->children != false){
			for(temp = node->children;temp;temp = temp->next){
				if (strcmp((char*)temp->name, "name")==0) {
					if(temp->children != NULL){
						strcpy(rNode->name,(char*)temp->children->content);
					}
				}
				else if(strcmp((char*)temp->name,"rtept")==0){
					wNode = wpParser(temp);
					insertBack(rNode->waypoints,wNode);
				}
				else if(strcmp((char*)temp->name,"text")!=0){
					attNode = attrParser(temp);
					insertBack(rNode->attributes,attNode);
				}
		}
	}

	return rNode;
}

Track* tParser(xmlNode* node){
	Track* tNode = (Track*)malloc(sizeof(Track));
	tNode->attributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
	tNode->segments = initializeList(&trackSegmentToString,&deleteTrackSegment,&compareTrackSegments);
	tNode->name = (char*)malloc(sizeof(char)*256);
	strcpy(tNode->name,"");
	TrackSegment* tsNode = NULL;
	Attribute* attNode = NULL;
	xmlNode* temp = NULL;

	if (node->children != false){
			for(temp = node->children;temp;temp = temp->next){
				if (strcmp((char*)temp->name, "name")==0) {
					if(temp->children != NULL){
						//printf("NULLLLL\n" );
					strcpy(tNode->name,(char*)temp->children->content);
					}
				}
				else if(strcmp((char*)temp->name,"trkseg")==0){
					tsNode = tsParser(temp->children);
					insertBack(tNode->segments,tsNode);
				}
				else if(strcmp((char*)temp->name,"text")!=0){
					attNode = attrParser(temp);
					insertBack(tNode->attributes,attNode);
				}
		}
	}
	return tNode;
}

TrackSegment* tsParser(xmlNode* node){

	TrackSegment* tsNode = (TrackSegment*)malloc(sizeof(TrackSegment));
	tsNode->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
	Waypoint* wpNode = NULL;
	xmlNode* curNode = NULL;

	for(curNode = node; curNode; curNode = curNode->next) {
		if (xmlStrcmp(curNode->name,(const xmlChar*)"trkpt")==0) {
			wpNode = wpParser(curNode);
			insertBack(tsNode->waypoints,wpNode);
		}
	}
return tsNode;
}

Attribute* attrParser(xmlNode* node){//this might not work for more nested attributes
	Attribute* atNode = (Attribute*)malloc(sizeof(Attribute));
	atNode->name = malloc(sizeof(char)*256);
	atNode->value = malloc(sizeof(char)*256);

	strcpy(atNode->name,(char*)node->name);
	if(node->children != NULL){
		strcpy(atNode->value,(char*)node->children->content);
	} else {
		strcpy(atNode->value,"");
	}

	return atNode;
}

int getNumWaypoints(const GPXdoc* doc){
	if (doc == NULL){return 0;}
	else{return getLength(doc->waypoints);}
}

//Number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){
	if (doc == NULL){
		return 0;}
	else{return getLength(doc->routes);}
}

//Number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){
	if (doc == NULL){
		return 0;}
	//if(doc->tracks == NULL){return 0;}
	else{return getLength(doc->tracks);}
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){

	if(doc == NULL){return 0;}
//	if(doc->tracks == NULL){return 0;}
		int total = 0;
		List* trk = doc->tracks;
		Node* trNode = trk->head;

		if(trNode == NULL){
			return 0;}
		else{
			while(trNode != NULL){
				Track* trackSeg = (Track*)trNode->data;
				total += getLength(trackSeg->segments);
				if (trNode->next != NULL) {
					trNode = trNode->next;
				}
				else{
					break;
				}
			}
		}
	return total;
}

//Total number of attributes in the document
int getNumAttributes(const GPXdoc* doc){

if (doc == NULL) {
	return 0;
}
 int total = 0;
 int flag = 0;

 	if (doc->waypoints != NULL) {
 		List* wayP = doc->waypoints;
		Node* wpNode = wayP->head;
		while (wpNode != NULL){
			Waypoint* wpStruct = (Waypoint*)wpNode->data;
			total += getLength(wpStruct->attributes);
			if (strlen(wpStruct->name) > 0){
				total += 1;
			}
			if (wpNode->next != NULL){
				wpNode = wpNode->next;
			}
			else{
				break;
			}
		}
 	}

//	if (doc->tracks != NULL){// this always gets entered, tracks list is intialized
 		List* trList = doc->tracks;
		Node* trNode = trList->head;
		while (trNode != NULL){//but trNode will be null off the bat so
			Track* trStruct = (Track*)trNode->data;
			total += getLength(trStruct->attributes);
			if (strlen(trStruct->name)>0){
				total += 1;
			}
			if (trNode->next != NULL){
				trNode = trNode->next;
			}
			else{
				break;
			}
		}

	//for trackSeg->waypoints->attributes
	//so im not looping to the next track, only looping to the next track segment in on track

	//if (doc->tracks != NULL) {//always enters this cause tracks it initlized
 		List* tr2List = doc->tracks;
		Node* tr2Node = tr2List->head;
		if (tr2Node == NULL) {
			flag = 1;
		}
		if (flag == 0) {
			Track* tr2Struct = (Track*)tr2Node->data;
			List*	tsList = tr2Struct->segments;
			Node* tsNode = tsList->head;
		if (tsNode == NULL) {
			flag = 1;
		}
		if (flag == 0) {
			TrackSegment* trkSegStruct = (TrackSegment*)tsNode->data;//use this to get waypoints

			List* wpHead = trkSegStruct->waypoints;
			Node* tswpNode = wpHead->head;

			while(tr2Node != NULL){
				tr2Struct = (Track*)tr2Node->data;
				tsList = tr2Struct->segments;
				tsNode = tsList->head;
				while (tsNode != NULL){
					trkSegStruct = (TrackSegment*)tsNode->data;
					wpHead = trkSegStruct->waypoints;
					tswpNode = wpHead->head;
					if(trkSegStruct->waypoints != NULL){
							while(tswpNode != NULL){
								Waypoint* waypointStruct = (Waypoint*)tswpNode->data;
								total += getLength(waypointStruct->attributes);
								if (strlen(waypointStruct->name)>0) {
									total += 1;
								}
								if(tswpNode->next != NULL){
									tswpNode = tswpNode->next;
								}
								else{
									break;
								}
							}
						if (tsNode->next != NULL){
							tsNode = tsNode->next;
						}
						else{
							break;
						}
					}
				}
				if (tr2Node->next != NULL){
					tr2Node = tr2Node->next;
				}
				else{
					break;
				}
				}
		}
	}
	flag = 0;


	if (doc->routes != NULL) {
		List* rList = doc->routes;
		Node* rNode = rList->head;
		while (rNode != NULL){
			Route* rStruct = (Route*)rNode->data;
			total += getLength(rStruct->attributes);
			if (strlen(rStruct->name) > 0){
				total += 1;
			}
			if (rNode->next != NULL){
				rNode = rNode->next;
			}
			else{
				break;
			}
		}
	}

	//for routes->waypoints->attributes
	if (doc->routes != NULL) {//always enters this cause route is initlized
 		List* rouList = doc->routes;
		Node* rouNode = rouList->head;
		if (rouNode == NULL) {
			flag = 1;
		}
		if (flag == 0) {
			Route* rouStruct = (Route*)rouNode->data;
			rouList = rouStruct->waypoints;
			rouNode = rouList->head;
		if (rouNode == NULL) {
			flag = 1;
		}
		if (flag == 0) {
			while(rouNode != NULL){
				Waypoint* rWpStruct = (Waypoint*)rouNode->data;//use this to get waypoints
				total += getLength(rWpStruct->attributes);
				if (strlen(rWpStruct->name)>0) {
					total += 1;
				}
				if(rouNode->next != NULL){
					rouNode = rouNode->next;
				}
				else{
					break;
				}
			}
		}
	}
 	}
	return total;
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.
Waypoint* getWaypoint(const GPXdoc* doc, char* name){

	if (name == NULL || doc == NULL) {
		return NULL;
	}

 		List* wayP = doc->waypoints;
		Node* wpNode = wayP->head;
		while (wpNode != NULL){
			Waypoint* wpStruct = (Waypoint*)wpNode->data;
			if (strcmp(wpStruct->name,name)==0) {
				return wpStruct;
			}
			if (wpNode->next != NULL){
				wpNode = wpNode->next;
			}
			else{
				break;
			}
		}


	return NULL;
}
// Function that returns a track with the given name.  If more than one exists, return the first one.
Track* getTrack(const GPXdoc* doc, char* name){

	if (name == NULL || doc == NULL) {
		return NULL;
	}
	List* trList = doc->tracks;
	Node* trNode = trList->head;
	while (trNode != NULL){//but trNode will be null off the bat so
		Track* trStruct = (Track*)trNode->data;
		if (strcmp(trStruct->name,name)==0) {
			return trStruct;
		}
		if (trNode->next != NULL){
			trNode = trNode->next;
		}
		else{
			break;
		}
	}
	return NULL;
}
// Function that returns a route with the given name.  If more than one exists, return the first one.
Route* getRoute(const GPXdoc* doc, char* name){

	if (name == NULL || doc == NULL) {
		return NULL;
	}

		List* rList = doc->routes;
		Node* rNode = rList->head;
		while (rNode != NULL){
			Route* rStruct = (Route*)rNode->data;
			if (strcmp(rStruct->name,name)==0) {
				return rStruct;
			}
			if (rNode->next != NULL){
				rNode = rNode->next;
			}
			else{
				break;
			}
		}
	return NULL;
}

char* GPXdocToString(GPXdoc* doc){
	void *element;
	int l;
	char *buffer;

	if(doc == NULL){
		return NULL;
	}

	l = strlen(doc->namespace)+1024;
	buffer = (char*)malloc(sizeof(char)*l+1000000);

	sprintf(buffer,"\n<gpx> version= %0.2f creator:%s\nxmlns=%s\n",doc->version,doc->creator,doc->namespace);;

	ListIterator iter = createIterator(doc->waypoints);
	while((element = nextElement(&iter)) != NULL){
		Waypoint* tempWP = (Waypoint*)element;
		char* strWP = doc->waypoints->printData(tempWP);
		sprintf(buffer + strlen(buffer), "%s\n",strWP);
		free(strWP);
	}

	iter = createIterator(doc->routes);
	while((element = nextElement(&iter)) != NULL){
		Route* tempR = (Route*)element;
		char* strR = doc->routes->printData(tempR);
		sprintf(buffer+strlen(buffer),"%s\n",strR);
		free(strR);
	}

	iter = createIterator(doc->tracks);
	while((element = nextElement(&iter)) != NULL){
		Track* tempT = (Track*)element;
		char* strT = doc->tracks->printData(tempT);
		sprintf(buffer+strlen(buffer),"%s\n",strT);
		free(strT);
	}

	return buffer;
}
char* attributeToString( void* data){
	Attribute* tempAt = (Attribute*)data;
	int len = strlen(tempAt->name);
	char* buffer = (char*)malloc(sizeof(char)*len+10000);

	sprintf(buffer,"	<%s>%s</%s>\n",tempAt->name,tempAt->value,tempAt->name);

	return buffer;
}

char* waypointToString( void* data){
	Waypoint* tempWp = (Waypoint*)data;
	int len = strlen(tempWp->name);
	char* buffer = (char*)malloc(sizeof(char)*len+10000);
	void *element;

	sprintf(buffer,"<wpt lat=%f lon=%f\n	<name>%s</name>\n",tempWp->latitude,tempWp->longitude,tempWp->name);

	ListIterator iter = createIterator(tempWp->attributes);
	while((element = nextElement(&iter)) != NULL){
		Attribute* tempA = (Attribute*)element;
		char* strA = tempWp->attributes->printData(tempA);
		sprintf(buffer + strlen(buffer), "%s\n",strA);
		free(strA);
	}

return buffer;
}

char* routeToString(void* data){
	Route* tempR = (Route*)data;
	int len = strlen(tempR->name);
	char* buffer = (char*)malloc(sizeof(char)*len+10000);
	void *element;

	sprintf(buffer,"<rte>\n	<name>%s</name>\n",tempR->name);

	ListIterator iter = createIterator(tempR->attributes);
	while((element = nextElement(&iter)) != NULL){
		Attribute* tempA = (Attribute*)element;
		char* strA = tempR->attributes->printData(tempA);
		sprintf(buffer + strlen(buffer), "%s\n",strA);
		free(strA);
	}

	iter = createIterator(tempR->waypoints);
	while((element = nextElement(&iter)) != NULL){
		Waypoint* tempWp = (Waypoint*)element;
		char* strW = tempR->waypoints->printData(tempWp);
		sprintf(buffer + strlen(buffer), "%s\n",strW);
		free(strW);
	}
	return buffer;
}
char* trackSegmentToString(void* data){
	TrackSegment* tempTs = (TrackSegment*)data;
	char* buffer = (char*)malloc(sizeof(char)*1000);
	void *element;

	sprintf(buffer,"	<trkseg>\n");

	ListIterator iter = createIterator(tempTs->waypoints);
	while((element = nextElement(&iter)) != NULL){
		Waypoint* tempWp = (Waypoint*)element;
		char* strW = tempTs->waypoints->printData(tempWp);
		sprintf(buffer + strlen(buffer), "			%s\n",strW);
		free(strW);
	}
	return buffer;
}

char* trackToString(void* data){
	Track* tempTrk = (Track*)data;
	int len = strlen(tempTrk->name);
	char* buffer = (char*)malloc(sizeof(char)*len+10000);
	void *element;

	sprintf(buffer,"<trk>\n	<name>%s</name>\n",tempTrk->name);

	ListIterator iter = createIterator(tempTrk->attributes);
	while((element = nextElement(&iter)) != NULL){
		Attribute* tempA = (Attribute*)element;
		char* strA = tempTrk->attributes->printData(tempA);
		sprintf(buffer + strlen(buffer), "%s\n",strA);
		free(strA);
	}

	iter = createIterator(tempTrk->segments);
	while((element = nextElement(&iter)) != NULL){
		TrackSegment* tempTs = (TrackSegment*)element;
		char* strTs = tempTrk->segments->printData(tempTs);
		sprintf(buffer + strlen(buffer), "	%s\n",strTs);
		free(strTs);
	}

	return buffer;
}

void deleteGPXdoc(GPXdoc* doc){
	if(doc == NULL) return;

	if(doc->waypoints != NULL){
		freeList(doc->waypoints);
	}
	if(doc->routes != NULL){
		freeList(doc->routes);
	}
	if(doc->tracks != NULL){
		freeList(doc->tracks);
	}
	if(doc->creator != NULL){
		free(doc->creator);
}
	free(doc);
}

void deleteAttribute( void* data){
	Attribute* attrTemp;
	if(data == NULL){
		return;
	}
	attrTemp = (Attribute*)data;

	free(attrTemp->name);
	free(attrTemp->value);
	free(attrTemp);
}
void deleteWaypoint(void* data){
	Waypoint* wpTemp;
	if(data == NULL){
		return;
	}
	wpTemp = (Waypoint*)data;
	freeList(wpTemp->attributes);
	free(wpTemp->name);
	free(wpTemp);
}
void deleteRoute(void* data){
	Route* rTemp = NULL;
	if(data == NULL){
		return;
	}
	rTemp = (Route*)data;
	freeList(rTemp->waypoints);
	freeList(rTemp->attributes);
	free(rTemp->name);
	free(rTemp);
}
void deleteTrackSegment(void* data){
	TrackSegment* tsTemp = NULL;
	if(data == NULL){
		return;
	}
	tsTemp = (TrackSegment*)data;
	freeList(tsTemp->waypoints);
	free(tsTemp);
}
void deleteTrack(void* data){
	Track* trkTemp;
	if(data == NULL){
		return;
	}
	trkTemp = (Track*)data;
	freeList(trkTemp->segments);
	freeList(trkTemp->attributes);
	free(trkTemp->name);
	free(trkTemp);
}

int compareAttributes(const void *first, const void *second){
	return 1;
}
int compareWaypoints(const void *first, const void *second){
	return 1;
}
int compareRoutes(const void *first, const void *second){
	return 1;
}
int compareTrackSegments(const void *first, const void *second){
	return 1;
}
int compareTracks(const void *first, const void *second){
	return 1;
}

xmlDocPtr docToTree(GPXdoc* doc){

	xmlDocPtr xmlDoc = NULL;
	xmlNodePtr rootNode = NULL;
	xmlNodePtr rootChild = NULL;
	xmlNodePtr childNode = NULL;
	xmlNodePtr childChildNode = NULL;

	List* wayP = NULL;
	Node* wpNode = NULL;
	Waypoint* wpStruct = NULL;
	List* rList = NULL;
	Node* rNode = NULL;
	Route* rStruct = NULL;
	List* trkList = NULL;
	Node* trkNode = NULL;
	Track* trkStruct = NULL;
	List* trkSegList = NULL;
	Node* trkSegNode = NULL;
	TrackSegment* trkSegStruct = NULL;
	List* attrList = NULL;
	Node* attrNode = NULL;
	Attribute* attrStruct = NULL;

	xmlDoc = xmlNewDoc(BAD_CAST "1.0");
	rootNode = xmlNewNode(NULL, BAD_CAST"gpx");
	xmlNsPtr ns = xmlNewNs(rootNode,BAD_CAST"http://www.topografix.com/GPX/1/1",NULL);
	xmlSetNs(rootNode,ns);

	xmlDocSetRootElement(xmlDoc,rootNode);

	char temp[1025];
	char temp2[1025];
	sprintf(temp,"%0.1f",doc->version);
	xmlNewProp(rootNode, BAD_CAST "version", BAD_CAST temp);
	sprintf(temp,"%s",doc->creator);
	xmlNewProp(rootNode, BAD_CAST "creator", BAD_CAST temp);

	//waypoint
	wayP = doc->waypoints;
	wpNode = wayP->head;
	while (wpNode != NULL){
		wpStruct = (Waypoint*)wpNode->data;
		rootChild = xmlNewChild(rootNode,NULL,BAD_CAST "wpt",NULL);

		sprintf(temp,"%f",wpStruct->latitude);
		xmlNewProp(rootChild, BAD_CAST "lat",BAD_CAST temp);
		sprintf(temp,"%f",wpStruct->longitude);
		xmlNewProp(rootChild, BAD_CAST "lon",BAD_CAST temp);
		if (strlen(wpStruct->name) > 0) {
			sprintf(temp,"%s",wpStruct->name);
			xmlNewChild(rootChild,NULL,BAD_CAST "name", BAD_CAST temp);
		}
		attrList = wpStruct->attributes;
		attrNode = attrList->head;
		while (attrNode != NULL){
				attrStruct = (Attribute*)attrNode->data;
				sprintf(temp,"%s",attrStruct->name);
				sprintf(temp2,"%s",attrStruct->value);
				xmlNewChild(rootChild,NULL,BAD_CAST temp,BAD_CAST temp2);
			if(attrNode->next != NULL){
				attrNode = attrNode->next;
			}
			else{
				break;
			}
		}
		if (wpNode->next != NULL){
			wpNode = wpNode->next;
		}
		else{
			break;
		}
	}
	//end of waypoint

	//route
	rList = doc->routes;
	rNode = rList->head;
	while (rNode != NULL){
		rStruct = (Route*)rNode->data;
		rootChild = xmlNewChild(rootNode,NULL,BAD_CAST "rte",NULL);
		if (strlen(rStruct->name) > 0) {
			sprintf(temp,"%s",rStruct->name);
			xmlNewChild(rootChild,NULL,BAD_CAST "name", BAD_CAST temp);
		}

		//rte attributes
		attrList = rStruct->attributes;
		attrNode = attrList->head;
		while (attrNode != NULL){
				attrStruct = (Attribute*)attrNode->data;
				sprintf(temp,"%s",attrStruct->name);
				sprintf(temp2,"%s",attrStruct->value);
				xmlNewChild(rootChild,NULL,BAD_CAST temp,BAD_CAST temp2);
			if(attrNode->next != NULL){
				attrNode = attrNode->next;
			}
			else{
				break;
			}
		}
		//rte waypoints/rtept
		wayP = rStruct->waypoints;
		wpNode = wayP->head;
		while (wpNode != NULL){
			wpStruct = (Waypoint*)wpNode->data;
			childNode = xmlNewChild(rootChild,NULL,BAD_CAST "rtept",NULL);

			sprintf(temp,"%f",wpStruct->latitude);
			xmlNewProp(childNode, BAD_CAST "lat",BAD_CAST temp);
			sprintf(temp,"%f",wpStruct->longitude);
			xmlNewProp(childNode, BAD_CAST "lon",BAD_CAST temp);
			if (strlen(wpStruct->name) > 0) {
				sprintf(temp,"%s",wpStruct->name);
				xmlNewChild(childNode,NULL,BAD_CAST "name", BAD_CAST temp);
			}
			//rtept attributes
			attrList = wpStruct->attributes;
			attrNode = attrList->head;
			while (attrNode != NULL){
					attrStruct = (Attribute*)attrNode->data;
					sprintf(temp,"%s",attrStruct->name);
					sprintf(temp2,"%s",attrStruct->value);
					xmlNewChild(childNode,NULL,BAD_CAST temp,BAD_CAST temp2);
				if(attrNode->next != NULL){
					attrNode = attrNode->next;
				}
				else{
					break;
				}
			}
			if (wpNode->next != NULL){
				wpNode = wpNode->next;
			}
			else{
				break;
			}
		}
		if (rNode->next != NULL){
			rNode = rNode->next;
		}
		else{
			break;
		}
	}
	//end of route

	//track
	trkList = doc->tracks;
	trkNode = trkList->head;
	while (trkNode != NULL){
		trkStruct = (Track*)trkNode->data;
		rootChild = xmlNewChild(rootNode,NULL,BAD_CAST "trk",NULL);

		if (strlen(trkStruct->name) > 0) {
			sprintf(temp,"%s",trkStruct->name);
			xmlNewChild(rootChild,NULL,BAD_CAST "name", BAD_CAST temp);
		}
		attrList = trkStruct->attributes;
		attrNode = attrList->head;
		while (attrNode != NULL){
				attrStruct = (Attribute*)attrNode->data;
				strcpy(temp,attrStruct->name);
				sprintf(temp2,"%s",attrStruct->value);
				xmlNewChild(rootChild,NULL,BAD_CAST temp,BAD_CAST temp2);
			if(attrNode->next != NULL){
				attrNode = attrNode->next;
			}
			else{
				break;
			}
		}
		//trackSegment
		trkSegList = trkStruct->segments;
		trkSegNode = trkSegList->head;
		while (trkSegNode != NULL){
				trkSegStruct = (TrackSegment*)trkSegNode->data;
				childNode = xmlNewChild(rootChild,NULL,BAD_CAST "trkseg",NULL);
				//trackSegment waypoints
				wayP = trkSegStruct->waypoints;
				wpNode = wayP->head;
				while (wpNode != NULL){
					wpStruct = (Waypoint*)wpNode->data;
					childChildNode = xmlNewChild(childNode,NULL,BAD_CAST "trkpt",NULL);

					sprintf(temp,"%f",wpStruct->latitude);
					xmlNewProp(childChildNode, BAD_CAST "lat",BAD_CAST temp);
					sprintf(temp,"%f",wpStruct->longitude);
					xmlNewProp(childChildNode, BAD_CAST "lon",BAD_CAST temp);
					if (strlen(wpStruct->name) > 0) {
						sprintf(temp,"%s",wpStruct->name);
						xmlNewChild(childChildNode,NULL,BAD_CAST "name", BAD_CAST temp);
					}
					//trkSeg->waypoints->attributes
					attrList = wpStruct->attributes;
					attrNode = attrList->head;
					while (attrNode != NULL){
							attrStruct = (Attribute*)attrNode->data;
							sprintf(temp,"%s",attrStruct->name);
							sprintf(temp2,"%s",attrStruct->value);
							xmlNewChild(childChildNode,NULL,BAD_CAST temp,BAD_CAST temp2);
						if(attrNode->next != NULL){
							attrNode = attrNode->next;
						}
						else{
							break;
						}
					}
					if (wpNode->next != NULL){
						wpNode = wpNode->next;
					}
					else{
						break;
					}
				}
				//end of track segment waypoint
			if(trkSegNode->next != NULL){
				trkSegNode = trkSegNode->next;
			}
			else{
				break;
			}
		}
		if (trkNode->next != NULL){
			trkNode = trkNode->next;
		}
		else{
			break;
		}
	}

	return xmlDoc;
}

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){
	if (doc == NULL) {
		return false;
	}
	if(strlen(gpxSchemaFile) <= 4){
	//	printf("Invalid filee\n");
		return false;
	}
	if(gpxSchemaFile[strlen(gpxSchemaFile)-4] != '.' || gpxSchemaFile[strlen(gpxSchemaFile)-3] != 'x' || gpxSchemaFile[strlen(gpxSchemaFile)-2] != 's' || gpxSchemaFile[strlen(gpxSchemaFile)-1] != 'd'){
	//	printf("Invalid file %c\n",gpxSchemaFile[strlen(gpxSchemaFile)-1]);
		return false;
	}

	bool TorF = false;
	int ret;

	xmlDocPtr xmlDocu = docToTree(doc);
	xmlSchemaParserCtxtPtr ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);
	xmlSchemaPtr schema = xmlSchemaParse(ctxt);
	xmlSchemaValidCtxtPtr ctxt2 = xmlSchemaNewValidCtxt(schema);

	ret = xmlSchemaValidateDoc(ctxt2,xmlDocu);

	if (ret == 0) {
		//printf("Validate success\n");
		TorF = true;
	}
	xmlSchemaFreeParserCtxt(ctxt);
	xmlFreeDoc(xmlDocu);
	xmlSchemaFreeValidCtxt(ctxt2);
	xmlSchemaFree(schema);
	xmlCleanupParser();
	//xmlMemoryDump(); //idk if needed

	return TorF;
}
bool writeGPXdoc(GPXdoc* doc, char* fileName){
	if(doc == NULL){return false;}//also check filename extension
	if(strlen(fileName) <= 4){
		return false;
	}
	if(fileName[strlen(fileName)-4] != '.' || fileName[strlen(fileName)-3] != 'g' || fileName[strlen(fileName)-2] != 'p' || fileName[strlen(fileName)-1] != 'x'){
		return false;
	}
//	bool boolean = false;
//	boolean = validateGPXDoc(doc, "gpx.xsd");
//	if (boolean == false) {return false;}

	xmlDocPtr xmlDoc = NULL;
	xmlDoc = docToTree(doc);

	if((xmlSaveFormatFileEnc(fileName,xmlDoc,NULL,1)) == -1){return false;}

	xmlFreeDoc(xmlDoc);
	xmlCleanupParser();

	return true;
}

float calculateHaversine(Coordinates* points, int length){ // final distance is off by decimals
	long double M_PI = 3.141592653589793238462643383279502884197169;
	float dist = 0;
	long double d = 0;
	long double a = 0;
	long double c = 0;
	long double lat1 = 0;
	long double lat2 = 0;
	long double latDif = 0;
	long double lonDif = 0;
	double radius = 6371;
	int i = 0;

	for (i = 0; i<length;i++){
		latDif = (points[i].lat - points[i+1].lat) * (M_PI/180);
		lonDif = (points[i].lon - points[i+1].lon) * (M_PI/180);
		lat1 = (points[i].lat) * (M_PI/180);
		lat2 = (points[i+1].lat) * (M_PI/180);

		a = (pow((sin((latDif/2))),2))+(cos(lat1))*(cos(lat2))*(pow((sin((lonDif/2))),2));
		c = 2*(atan2((sqrt(a)),(sqrt(1-a))));
		d = radius*c;
	//	printf("lat%f lon%f :%Lf\n",points[i].lat,points[i].lon,d );
		dist = dist + d;
	}
	dist = dist/(0.001);
	return dist;
}

float getRouteLen(const Route *rt){
	if (rt == NULL){return 0;}
	int x = 0;
	float metres = 0;

	List* wList = rt->waypoints;

	Coordinates rteCoor[(getLength(wList))];

	Node* wNode = wList->head;
	while (wNode != NULL){
		Waypoint* wStruct = (Waypoint*)wNode->data;
		rteCoor[x].lat = wStruct->latitude;
		rteCoor[x].lon = wStruct->longitude;
		if (wNode->next != NULL){
			x++;
			wNode = wNode->next;
		}
		else{
			break;
		}
	}

	metres = calculateHaversine(rteCoor, x);

	return metres;
}

float getTrackLen(const Track *tr){
	if (tr == NULL){return 0;}
	int x = 0;
	int length = 0;
	float metres = 0;

	List* trkList = tr->segments;
	Node* trkSegNode = trkList->head;
	while (trkSegNode != NULL){ // just to loop through trkSeg list andget how many waypoints their are
		TrackSegment* trkSegStruct = (TrackSegment*)trkSegNode->data;
		List* wList = trkSegStruct->waypoints;
		length = length+getLength(wList);
		if (trkSegNode->next != NULL){
			trkSegNode = trkSegNode->next;
		}
		else{
			break;
		}
	}

	Coordinates trkCoor[length];

	trkList = tr->segments;
	trkSegNode = trkList->head;
	while (trkSegNode != NULL){
		TrackSegment* trkSegStruct = (TrackSegment*)trkSegNode->data;
			List* wpList = trkSegStruct->waypoints;
			Node* wpNode = wpList->head;
			while (wpNode != NULL){
				Waypoint* wpStruct = (Waypoint*)wpNode->data;
				trkCoor[x].lat = wpStruct->latitude;
				trkCoor[x].lon = wpStruct->longitude;
				if (wpNode->next != NULL){
					x++;
					wpNode = wpNode->next;
				}
				else{
					break;
				}
			}
		if (trkSegNode->next != NULL){
			x++;
			trkSegNode = trkSegNode->next;
		}
		else{
			break;
		}
	}

	metres = calculateHaversine(trkCoor,x );

	return metres;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta){

	if (doc == NULL || len < 0 || delta < 0) {return 0;}
	int numRoutes = 0;
	float metres = 0;


	List* rteList = doc->routes;
	Node* rteNode = rteList->head;
	while(rteNode != NULL){
		Route* rteStruct = (Route*)rteNode->data;
		metres = getRouteLen(rteStruct);
		if(metres > len){
			if ((metres - len) <= delta && (metres-len) >= 0){
				numRoutes++;
			}
		}
			else if(metres <= len){
				if ((len-metres) <= delta && (len-metres) >= 0){
					numRoutes++;
				}
			}
		if(rteNode->next != NULL){
			rteNode = rteNode->next;
		}
		else{
			break;
		}
	}
	return numRoutes;
}

int numTracksWithLength(const GPXdoc* doc, float len, float delta){
	if (doc == NULL || len < 0 || delta < 0) {return 0;}
	int numTracks = 0;
	float metres = 0;

	//need to loop through all tracks, so have to loop through tracks which loops through segments which loops through waypoints
	List* trkList = doc->tracks;
	Node* trkNode = trkList->head;
	while(trkNode != NULL){
		Track* trkStruct = (Track*)trkNode->data;
		metres = getTrackLen(trkStruct);
		if(metres > len){
			if ((metres - len) <= delta && (metres-len) >= 0){
				numTracks++;
			}
		}
			else if(metres <= len){
				if ((len-metres) <= delta && (len-metres) >= 0){
					numTracks++;
				}
			}
		if(trkNode->next != NULL){
			trkNode = trkNode->next;
		}
		else{
			break;
		}
	}

	return numTracks;
}

bool isLoopRoute(const Route* route, float delta){
	if(route == NULL || delta < 0){return false;}

	int x = 0;
	float metres = 0;

	List* wList = route->waypoints;
	if (getLength(wList) < 4) {
		return false;
	}

	Coordinates rteCoor[(getLength(wList))];

	Node* wNode = wList->head;
	while (wNode != NULL){
		Waypoint* wStruct = (Waypoint*)wNode->data;
		rteCoor[x].lat = wStruct->latitude;
		rteCoor[x].lon = wStruct->longitude;
		if (wNode->next != NULL){
			x++;
			wNode = wNode->next;
		}
		else{
			break;
		}
	}

	Coordinates rteCoor2[2];
	rteCoor2[0].lat = rteCoor[0].lat;
	rteCoor2[0].lon = rteCoor[0].lon;
	rteCoor2[1].lat = rteCoor[x].lat;
	rteCoor2[1].lon = rteCoor[x].lon;

	metres = calculateHaversine(rteCoor2, 1);

	if (metres < delta) {
		return true;
	}

	return false;
}

bool isLoopTrack(const Track *tr, float delta){//first and last points of segments?
	if (tr == NULL || delta < 0) {return false;}

	int x = 0;
	int length = 0;
	float metres = 0;

	List* segList = tr->segments;
	Node* segNode = segList->head;
	while(segNode != NULL){
		TrackSegment* segStruct = (TrackSegment*)segNode->data;
		List* wpList = segStruct->waypoints;
		length = length+getLength(wpList);
		if(segNode->next != NULL) {
			segNode = segNode->next;
		}
		else{
			break;
		}
	}

	if (length < 4) {
		return false;
	}

	Coordinates trkCoor[length];

	segList = tr->segments;
	segNode = segList->head;
	while (segNode != NULL){
		TrackSegment* segStruct = (TrackSegment*)segNode->data;
			List* wpList = segStruct->waypoints;
			Node* wpNode = wpList->head;
			while (wpNode != NULL){
				Waypoint* wpStruct = (Waypoint*)wpNode->data;
				trkCoor[x].lat = wpStruct->latitude;
				trkCoor[x].lon = wpStruct->longitude;
				if (wpNode->next != NULL){
					x++;
					wpNode = wpNode->next;
				}
				else{
					break;
				}
			}
		if (segNode->next != NULL){
			x++;
			segNode = segNode->next;
		}
		else{
			break;
		}
	}

	Coordinates trkCoor2[2];
	trkCoor2[0].lat = trkCoor[0].lat;
	trkCoor2[0].lon = trkCoor[0].lon;
	trkCoor2[1].lat = trkCoor[x].lat;
	trkCoor2[1].lon = trkCoor[x].lon;

	metres = calculateHaversine(trkCoor2,1);

	if (metres < delta) {
		return true;
	}

	return false;
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
	if (doc == NULL) {return NULL;}

	float metres = 0;
	int length = 0;
	int x = 0;
	List* rteCopyList = initializeList(&routeToString,&deleteRoute,&compareRoutes);
	Route* rteCopy = NULL;

	List* rteList = doc->routes;
	Node* rteNode = rteList->head;
	while (rteNode != NULL) {
		Route* rteStruct = (Route*) rteNode->data;
		length = length + getLength(rteStruct->waypoints); // i could do if length is greater then length then reassign instead of just adding
		if(rteNode->next != NULL){
			rteNode = rteNode->next;
		}
		else{
			break;
		}
	}

	Coordinates rteCoor[length];
	Coordinates test[2];

	rteList = doc->routes;
	rteNode = rteList->head;
	while (rteNode != NULL) {
		Route* rteStruct = (Route*) rteNode->data;
		List* wpList = rteStruct->waypoints;
		Node* wpNode = wpList->head;
		while (wpNode != NULL) {
			Waypoint* wpStruct = (Waypoint*) wpNode->data;
			rteCoor[x].lat = wpStruct->latitude;
			rteCoor[x].lon = wpStruct->longitude;
			if(wpNode->next != NULL){
				x++;
				wpNode = wpNode->next;
			}
			else{
				break;
			}
		}
		test[0].lat = rteCoor[0].lat;
		test[0].lon = rteCoor[0].lon;
		test[1].lat = sourceLat;
		test[1].lon = sourceLong;
		metres = calculateHaversine(test, 1);
		if (metres <= delta) {
			test[0].lat = rteCoor[x].lat;
			test[0].lon = rteCoor[x].lon;
			test[1].lat = destLat;
			test[1].lon = destLong;
			metres = calculateHaversine(test,1);
			if (metres <= delta) {
				rteCopy = rteDeepCopy(rteStruct);
				insertBack(rteCopyList, rteCopy);
			}
		}
		if(rteNode->next != NULL){
			x = 0;
			rteNode = rteNode->next;
		}
		else{
			break;
		}
	}
	if(getLength(rteCopyList) == 0){
		freeList(rteCopyList);
		return NULL;
	}

return rteCopyList;
}

Route* rteDeepCopy(Route* toCopy){
	Route* rte = malloc(sizeof(Route));
	rte->name = malloc(sizeof(char)*256);
	Waypoint* wp = NULL;
	Attribute* attr = NULL;
	List* wpCopyList = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
	List* attrCopyList = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);

	strcpy(rte->name,toCopy->name);

	List* wpList = toCopy->waypoints;
	Node* wpNode = wpList->head;
	while (wpNode != NULL) {
		Waypoint* wpStruct = wpNode->data;
		wp = wpDeepCopy(wpStruct);
		insertBack(wpCopyList, wp);
		if(wpNode->next != NULL){
			wpNode = wpNode->next;
		}
		else{
			break;
		}
	}

	List* attrList = toCopy->attributes;
	Node* attrNode = attrList->head;
	while (attrNode != NULL) {
		Attribute* attrStruct = attrNode->data;
		attr = attrDeepCopy(attrStruct);
		insertBack(attrCopyList, attr);
		if(attrNode->next != NULL){
			attrNode = attrNode->next;
		}
		else{
			break;
		}
	}
	rte->waypoints = wpCopyList;
	rte->attributes = attrCopyList;

	return rte;
}

Waypoint* wpDeepCopy(Waypoint* toCopy){
	Waypoint* wp = (Waypoint*)malloc(sizeof(Waypoint));
	wp->name = malloc(sizeof(char)*256);
	Attribute* attr = NULL;
	List* attrCopyList = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);
	strcpy(wp->name, toCopy->name);
	wp->latitude = toCopy->latitude;
	wp->longitude = toCopy->longitude;


	List* attrList = toCopy->attributes;
	Node* attrNode = attrList->head;
	while (attrNode != NULL){
		Attribute* attrStruct = (Attribute*)attrNode->data;
		attr = attrDeepCopy(attrStruct);
		insertBack(attrCopyList, attr);
		if(attrNode->next != NULL){
			attrNode = attrNode->next;
		}
		else{
			break;
		}
	}
	wp->attributes = attrCopyList;
	return wp;
}

Attribute* attrDeepCopy(Attribute* toCopy){
	Attribute* attr = (Attribute*)malloc(sizeof(Attribute));
	attr->value = malloc(sizeof(char)*256);

	if(strlen(toCopy->name)>0){
		attr->name = malloc(sizeof(char)*256);
		strcpy(attr->name, toCopy->name);
	}

	strcpy(attr->value, toCopy->value);

	return attr;
}

List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
if (doc == NULL || delta < 0) {return false;}

	List* trkCopyList = initializeList(&trackToString, &deleteTrack, &compareTracks);
	Track* trkCopy = NULL;

	int x = 0;
	int length = 0;
	float metres = 0;

	List* trkList = doc->tracks;
	Node* trkNode = trkList->head;
	while(trkNode != NULL){
	Track* trkStruct = (Track*)trkNode->data;
	List* segList = trkStruct->segments;
	Node* segNode = segList->head;
		while(segNode != NULL){
			TrackSegment* segStruct = (TrackSegment*)segNode->data;
			List* wpList = segStruct->waypoints;
			length = length+getLength(wpList);
			if(segNode->next != NULL) {
				segNode = segNode->next;
			}
			else{break;}
		}
		if (trkNode->next != NULL) {
			trkNode = trkNode->next;
		}
		else{break;}
	}

	Coordinates trkCoor[length];
	Coordinates test[2];

	trkList = doc->tracks;
	trkNode = trkList->head;
	while(trkNode != NULL){
		Track* trkStruct = (Track*)trkNode->data;
		List* segList = trkStruct->segments;
		Node* segNode = segList->head;
		while(segNode != NULL){
			TrackSegment* segStruct = (TrackSegment*)segNode->data;
			List* wpList = segStruct->waypoints;
			Node* wpNode = wpList->head;
			while(wpNode != NULL){
				Waypoint* wpStruct = (Waypoint*)wpNode->data;
				trkCoor[x].lat = wpStruct->latitude;
				trkCoor[x].lon = wpStruct->longitude;
				if (wpNode->next != NULL) {
					x++;
					wpNode = wpNode->next;
				}
				else{break;}
			}
			if(segNode->next != NULL){
				x++;
				segNode = segNode->next;
			}
			else{break;}
		}
		test[0].lat = trkCoor[0].lat;
		test[0].lon = trkCoor[0].lon;
		test[1].lat = sourceLat;
		test[1].lon = sourceLong;
		metres = calculateHaversine(test, 1);
		if (metres <= delta) {
			test[0].lat = trkCoor[x].lat;
			test[0].lon = trkCoor[x].lon;
			test[1].lat = destLat;
			test[1].lon = destLong;
			metres = calculateHaversine(test,1);
			if (metres <= delta) {
				trkCopy = trackDeepCopy(trkStruct);
				insertBack(trkCopyList, trkCopy);
			}
		}
		if (trkNode->next != NULL) {
			x = 0;
			trkNode = trkNode->next;
		}
		else{break;}
	}
	if(getLength(trkCopyList)==0){
		freeList(trkCopyList);
		return NULL;
	}
	return trkCopyList;
}

Track* trackDeepCopy(Track* toCopy){
	Track* trk = malloc(sizeof(Track));
	trk->name = malloc(sizeof(char)*256);
	Attribute* attr = NULL;
	TrackSegment* trkSeg = NULL;
	List* attrCopyList = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
	List* trkSegCopyList = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);

	strcpy(trk->name, toCopy->name);

	List* attrList = toCopy->attributes;
	Node* attrNode = attrList->head;
	while (attrNode != NULL) {
		Attribute* attrStruct = attrNode->data;
		attr = attrDeepCopy(attrStruct);
		insertBack(attrCopyList, attr);
		if(attrNode->next != NULL){
			attrNode = attrNode->next;
		}
		else{
			break;
		}
	}

	List* trkSegList = toCopy->segments;
	Node* trkSegNode = trkSegList->head;
	while(trkSegNode != NULL){
		TrackSegment* trkSegStruct = (TrackSegment*)trkSegNode->data;
		trkSeg = trkSegDeepCopy(trkSegStruct);
		insertBack(trkSegCopyList, trkSeg);
		if(trkSegNode->next != NULL){
			trkSegNode = trkSegNode->next;
		}
		else{break;}
	}

	trk->segments = trkSegCopyList;
	trk->attributes = attrCopyList;


	return trk;
}

TrackSegment* trkSegDeepCopy(TrackSegment* toCopy){

	TrackSegment* trkSeg = malloc(sizeof(TrackSegment));
	Waypoint* wp = NULL;
	List* wpCopyList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

	List* wpList = toCopy->waypoints;
	Node* wpNode = wpList->head;
	while(wpNode != NULL){
		Waypoint* wpStruct = (Waypoint*)wpNode->data;
		wp = wpDeepCopy(wpStruct);
		insertBack(wpCopyList, wp);
		if(wpNode->next != NULL){
			wpNode = wpNode->next;
		}
		else{break;}
	}
trkSeg->waypoints = wpCopyList;

return trkSeg;
}


char* trackToJSON(const Track *tr){
	if(tr == NULL){return "{}";}
	char tOrF[6];
	float trkLen = 0;
	char trkName[strlen(tr->name)+5];

	if (strlen(tr->name)>0) {
		strcpy(trkName, tr->name);
	}
	else{
		strcpy(trkName, "None");
	}

	if(isLoopTrack(tr, 15)){
		strcpy(tOrF, "true");
	}
	else{
		strcpy(tOrF, "false");
	}
	trkLen = round10(getTrackLen(tr));
	char* toReturn = malloc(sizeof(char)*(strlen(tr->name)+256));
	sprintf(toReturn,"{\"name\":\"%s\",\"len\":%.01f,\"loop\":%s}",trkName, trkLen, tOrF);


	return toReturn;
}


char* routeToJSON(const Route *rt){
	if(rt == NULL){return "{}";}

	int numPoints = 0;
	char tOrF[6];
	float rtLen = round10(getRouteLen(rt));
	char rtName[strlen(rt->name)+5];

	if (strlen(rt->name)>0) {
		strcpy(rtName, rt->name);
	}
	else{
		strcpy(rtName, "None");
	}

	//count points
	List* wpList = rt->waypoints;
	numPoints = getLength(wpList);

	if(isLoopRoute(rt,15)){
		strcpy(tOrF, "true");
	}
	else{
		strcpy(tOrF, "false");
	}
	char* toReturn = malloc(sizeof(char)*(strlen(rt->name)+256));

	sprintf(toReturn,"{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.01f,\"loop\":%s}",rtName, numPoints, rtLen, tOrF);

	return toReturn;
}


char* routeListToJSON(const List *list){
	if(list == NULL){return "[]";}
	List* testList = (List*) list;
	if(getLength(testList) == 0){
		return "[]";
	}
	char* toReturn = malloc(sizeof(char)*2);
	strcpy(toReturn,"[");
	char* temp;

	Node* rteNode = list->head;
	while(rteNode != NULL){
		Route* rteStruct = (Route*)rteNode->data;
		temp = routeToJSON(rteStruct);
		toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+2);
		strcat(toReturn, temp);
		free(temp);
		strcat(toReturn, ",");
		if(rteNode->next != NULL){
			rteNode = rteNode->next;
		}
		else{break;}
	}
	toReturn[strlen(toReturn)-1] = ']';

	return toReturn;
}


char* trackListToJSON(const List *list){
	if(list == NULL){return "[]";}
	List* testList = (List*) list;
	if(getLength(testList) == 0){
		return "[]";
	}
	char* toReturn = malloc(sizeof(char)*2);
	strcpy(toReturn,"[");
	char* temp;

	Node* trkNode = list->head;
	while(trkNode != NULL){
		Track* trkStruct = (Track*)trkNode->data;
		temp = trackToJSON(trkStruct);
		toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+2);
		strcat(toReturn, temp);
		free(temp);
		strcat(toReturn, ",");
		if(trkNode->next != NULL){
			trkNode = trkNode->next;
		}
		else{break;}
	}
	toReturn[strlen(toReturn)-1] = ']';
	return toReturn;
}


char* GPXtoJSON(const GPXdoc* gpx){
	if(gpx == NULL){return "{}";}
	char* toReturn = malloc(sizeof(char)+strlen(gpx->creator)+256);

	sprintf(toReturn,"{\"version\":%0.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",gpx->version, gpx->creator, getNumWaypoints(gpx),getNumRoutes(gpx),getNumTracks(gpx));

	return toReturn;
}

float round10(float len){
	if(len<5){return 0;}
	if (len<10) {return (float)(10-(int)len) + ((int)len);}

	int num = ((int)len)%10;
	if (num>=5) {num = (int)len + (10-num);}
	else{num = (int)len - num;}

	return (float)num;
}

void addWaypoint(Route *rt, Waypoint *pt){
	if(rt == NULL || pt == NULL){return;}

	List* wpList = rt->waypoints;
	insertBack(wpList, pt);

	return;
}

void addRoute(GPXdoc* doc, Route* rt){//is the route already malloced, can i just insert
	if(doc == NULL || rt == NULL){return;}

	List* rteList = doc->routes;
	insertBack(rteList, rt);
	//can i just do insertBack(doc->routes, rt);
	return;
}

GPXdoc* JSONtoGPX(const char* gpxString){
	if(gpxString == NULL){return NULL;}
	char* copy = malloc(sizeof(char)+strlen(gpxString)+2);
	strcpy(copy,gpxString);
	char* token;
	int count = 0;
	GPXdoc* doc = malloc(sizeof(GPXdoc));
	doc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
	doc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
	doc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

	token = strtok(copy,":");
	while(token != NULL){
		if(count == 1){
			doc->version = atof(token);
		}
		else if(count == 3){
			doc->creator = malloc(strlen(token)+2);
			strcpy(doc->creator, token);
		}
		token = strtok(NULL,",:\"");
		count++;
	}

	free(copy);

	return doc;
}

Waypoint* JSONtoWaypoint(const char* gpxString){
	if(gpxString == NULL){return NULL;}
	char* copy = malloc(sizeof(char)+strlen(gpxString)+2);
	strcpy(copy,gpxString);
	char* token;
	int count = 0;
	Waypoint* wp = malloc(sizeof(Waypoint));
	wp->name = malloc(sizeof(char));
	wp->attributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

	token = strtok(copy,":");
	while(token != NULL){
		if(count == 1){
			wp->latitude = atof(token);
		}
		else if(count == 3){
			wp->longitude = atof(token);
		}
		token = strtok(NULL,",:\"}");
		count++;
	}

	free(copy);
	return wp;
}

Route* JSONtoRoute(const char* gpxString){
	if(gpxString == NULL){return NULL;}

	char* copy = malloc(sizeof(char)+strlen(gpxString)+2);
	strcpy(copy,gpxString);
	char* token;
	int count = 0;
	Route* rte = malloc(sizeof(Waypoint));
	rte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
	rte->attributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

	token = strtok(copy,":");
	while(token != NULL){
		if(count == 1){
			rte->name = malloc(sizeof(char)+strlen(token)+2);
			strcpy(rte->name, token);
		}
		token = strtok(NULL,",:\"}");
		count++;
	}

	free(copy);

	return rte;
}

char* attrToJSON(const Attribute *at, int listLength){
	if(at == NULL){return "{}";}

	char atName[strlen(at->name)+5];
	if(strlen(at->name) > 0){
		strcpy(atName, at->name);
	}
	else{
		strcpy(atName, "None");
	}
	char atValue[strlen(at->value)+5];
	strcpy(atName, at->name);
	strcpy(atValue, at->value);

	char* toReturn = malloc(sizeof(char)+(strlen(at->name)+strlen(at->value)+256));
	sprintf(toReturn,"{\"name\":\"%s\",\"value\":\"%s\"}",atName, atValue);


	return toReturn;
}

char* attrListToJSON(List *list){
	char* toReturn = malloc(sizeof(char)*3);
	if(list == NULL){return "{}";}
	if(getLength(list) == 0){
		strcpy(toReturn,"{}");
		return toReturn;
	}
	int len = 0;
	toReturn[0] = '\0';
	char* temp;

	Node* atNode = list->head;
	while(atNode != NULL){
		Attribute* atStruct = (Attribute*)atNode->data;
		len = getLength(list);
		temp = attrToJSON(atStruct, len);
		toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+2);
		strcat(toReturn, temp);
		free(temp);
		strcat(toReturn, ",");
		if(atNode->next != NULL){
			atNode = atNode->next;
		}
		else{break;}
	}
	toReturn[strlen(toReturn)-1] = '\0';
	return toReturn;
}

char* fileLogWrapper(char* fileName){

	GPXdoc* doc = createGPXdoc(fileName);
	//call validate, but ask where to keep the gpx schema xsd filee
	char* toReturn = GPXtoJSON(doc);
	deleteGPXdoc(doc);

	return toReturn;
}

char* routeViewPanelWrapper(char* fileName){

	GPXdoc* doc = createGPXdoc(fileName);
	//validate
	char* toReturn = routeListToJSON(doc->routes);
	deleteGPXdoc(doc);
	return toReturn;
}
char* trackViewPanelWrapper(char* fileName){
	GPXdoc* doc = createGPXdoc(fileName);
	//validate
	char* toReturn = trackListToJSON(doc->tracks);
	deleteGPXdoc(doc);
	return toReturn;
}

char* attributeRouteWrapper(char* fileName, int index){

	GPXdoc* doc = createGPXdoc(fileName);
	int count = 0;
	//validateGPXDoc
	char* toReturn = malloc(sizeof(char)*3);
	char* temp;
	strcpy(toReturn, "[");

	List* rtList = doc->routes;
	Node* rtNode = rtList->head;

	while (rtNode != NULL){
		Route* rtStruct = (Route*)rtNode->data;
		if(index == count){
			temp = attrListToJSON(rtStruct->attributes);
			toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+3);
			strcat(toReturn, temp);
			free(temp);
			strcat(toReturn, ",");
		}
		if(rtNode->next != NULL){
			rtNode = rtNode->next;
			count++;
		}
		else{break;}
	}


	toReturn[strlen(toReturn)-1] = ']';

	deleteGPXdoc(doc);
	return toReturn;
}

char* attributeTrackWrapper(char* fileName, int index){

	GPXdoc* doc = createGPXdoc(fileName);
	//validateGPXDoc
	int count = 0;
	char* toReturn = malloc(sizeof(char)*2);
	char* temp;
	strcpy(toReturn, "[");

	List* trkList = doc->tracks;
	Node* trkNode = trkList->head;

	while (trkNode != NULL){
		Track* trkStruct = (Track*)trkNode->data;
		if(index == count){
			temp = attrListToJSON(trkStruct->attributes);
			toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+2);
			strcat(toReturn, temp);
			free(temp);
			strcat(toReturn, ",");
		}
		if(trkNode->next != NULL){
			trkNode = trkNode->next;
			count++;
		}
		else{break;}
	}


	toReturn[strlen(toReturn)-1] = ']';

	deleteGPXdoc(doc);
	return toReturn;
}

int writeWrapper(char* fileName, float version, char* creator){

	bool yesorno = false;
	char* fileDirect = malloc(sizeof(char)+(strlen(fileName)+50));
	strcpy(fileDirect, "./uploads/");
	strcat(fileDirect, fileName);

	GPXdoc* newDoc = NULL;
	newDoc = (GPXdoc*)malloc(sizeof(GPXdoc));
	newDoc->creator = malloc(sizeof(char)*200);
	newDoc->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
	newDoc->routes = initializeList(&routeToString,&deleteRoute,&compareRoutes);
	newDoc->tracks = initializeList(&trackToString,&deleteTrack,&compareTracks);

	strcpy(newDoc->creator, creator);
	strcpy(newDoc->namespace, "http://www.topografix.com/GPX/1/1");
	newDoc->version = version;


	yesorno = writeGPXdoc(newDoc, fileDirect);

	if (yesorno == true) {return 1;}
	else{return 0;}
}

int validateWrapper(char* filename){

	GPXdoc* doc = createGPXdoc(filename);

	if(validateGPXDoc(doc,"./parser/src/gpx.xsd") == true){
		return 1;
	}
	else{return 0;}
}


int addRouteWrapper(char* filename, float lat, float lon, int check){


	GPXdoc* doc = createGPXdoc(filename);

	Waypoint* newWp = malloc(sizeof(Waypoint));
	newWp->name = malloc(sizeof(char)+6);
	strcpy(newWp->name, "");
	newWp->latitude = lat;
	newWp->longitude = lon;
	newWp->attributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);

	if(check == 0){
		Route* newRte = malloc(sizeof(Route));
		newRte->name = malloc(sizeof(char)+6);
		strcpy(newRte->name, "");
		newRte->waypoints = initializeList(&waypointToString,&deleteWaypoint,&compareWaypoints);
		newRte->attributes = initializeList(&attributeToString,&deleteAttribute,&compareAttributes);

		addWaypoint(newRte, newWp);
		addRoute(doc, newRte);

		writeGPXdoc(doc, filename);
	}
	else{
		List* rteList = doc->routes;
		Node* rteNode = rteList->head;
		Route* rteStruct;
		while(rteNode != NULL){
			rteStruct = (Route*)rteNode->data;
			if(rteNode->next != NULL){
				rteNode = rteNode->next;
			}
			else{
				addWaypoint(rteStruct, newWp);
				writeGPXdoc(doc, filename);
				break;
			}
		}
	}

	deleteGPXdoc(doc);

	return 1;
}


char* getBetweenRouteWrapper(char* filename,float latS, float lonS, float latE, float lonE, float accur){

	GPXdoc* doc = createGPXdoc(filename);

	List* rteList = getRoutesBetween(doc, latS, lonS, latE, lonE, accur);

	char* toReturn = routeListToJSON(rteList);

	deleteGPXdoc(doc);
	return toReturn;
}

char* getBetweenTrackWrapper(char* filename,float latS, float lonS, float latE, float lonE, float accur){
	GPXdoc* doc = createGPXdoc(filename);

	List* trkList = getTracksBetween(doc, latS, lonS, latE, lonE, accur);

	char* toReturn = trackListToJSON(trkList);

	deleteGPXdoc(doc);
	return toReturn;
}

char* renameRouteWrapper(char* filename, char* newName,int index){

	GPXdoc* doc = createGPXdoc(filename);
	int count = 0;

	List* rteList = doc->routes;
	Node* rteNode = rteList->head;

	while(rteNode != NULL){
		Route* rteStruct = (Route*)rteNode->data;
		if(index == count){
			rteStruct->name = realloc(rteStruct->name, sizeof(char)+strlen(newName)+2);
			strcpy(rteStruct->name, newName);
			writeGPXdoc(doc, filename);
			return "Success";
		}
		if(rteNode->next != NULL){
			rteNode = rteNode->next;
			count++;
		}
		else{
			break;
		}
	}

	deleteGPXdoc(doc);
	return "fail";
}
char* renameTrackWrapper(char* filename, char* newName,int index){

	GPXdoc* doc = createGPXdoc(filename);
	int count = 0;

	List* trkList = doc->tracks;
	Node* trkNode = trkList->head;

	while(trkNode != NULL){
		Track* trkStruct = (Track*)trkNode->data;
		if(index == count){
			trkStruct->name = realloc(trkStruct->name, sizeof(char)+strlen(newName)+2);
			strcpy(trkStruct->name, newName);
			writeGPXdoc(doc, filename);
			return "Success";
		}
		if(trkNode->next != NULL){
			trkNode = trkNode->next;
			count++;
		}
		else{
			break;
		}
	}

	deleteGPXdoc(doc);
	return "fail";
}

char* pointTable(char* fileName){

	GPXdoc* doc = createGPXdoc(fileName);

	int count = 0;
	char* toReturn = malloc(sizeof(char)*20);
	char* temp;
	char* temp2 = malloc(sizeof(char)*20);
	strcpy(toReturn, "{");

	List* rteList = doc->routes;
	if(getLength(rteList) == 0){
		strcpy(toReturn,"{}");
		return toReturn;
	}
	Node* rteNode = rteList->head;

	while (rteNode != NULL){
		sprintf(temp2,"\"i%d\":",count);
		strcat(toReturn,temp2);
		Route* rteStruct = (Route*)rteNode->data;
			temp = pointTableList(rteStruct->waypoints);
			toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+5);
			strcat(toReturn, temp);
			free(temp);
			strcat(toReturn, "],");
		if(rteNode->next != NULL){
			rteNode = rteNode->next;
			count++;
		}
		else{break;}
	}


	toReturn[strlen(toReturn)-1] = '}';

	deleteGPXdoc(doc);
	return toReturn;
}

char* pointTableList(List *list){
	char* toReturn = malloc(sizeof(char)*5);
	//if(list == NULL){return "{}";}
	if(getLength(list) == 0){
		strcpy(toReturn,"[{}]");
		return toReturn;
	}
	toReturn[0] = '\0';
	strcat(toReturn,"[");

	char* temp;

	Node* wpNode = list->head;
	while(wpNode != NULL){
		Waypoint* wpStruct = (Waypoint*)wpNode->data;
		temp = waypoint(wpStruct);
		toReturn = realloc(toReturn, strlen(temp)+strlen(toReturn)+2);
		strcat(toReturn, temp);
		free(temp);
		strcat(toReturn, ",");
		if(wpNode->next != NULL){
			wpNode = wpNode->next;
		}
		else{break;}
	}
	toReturn[strlen(toReturn)-1] = '\0';
	return toReturn;
}

char* waypoint(Waypoint* wp){
	if(wp == NULL){return "{}";}

	char wpName[strlen(wp->name)+5];
	float lat = 0;
	float lon = 0;

	if((strlen(wp->name)) > 0){
		strcpy(wpName, wp->name);
	}
	else{
	 	strcpy(wpName, "None");
	}

	lat = wp->latitude;
	lon = wp->longitude;

	char* toReturn = malloc(sizeof(char)+(strlen(wp->name)+256));
	sprintf(toReturn,"{\"name\":\"%s\",\"lat\":\"%f\",\"lon\":\"%f\"}",wpName, lat, lon);


	return toReturn;
}
