#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

/** Load the cascade manually
  * from 5x5.xml path
  */
Cascade* loadCascade() {
	int stageNum = 5;
	int nodeNum = 5;
	int featureNum = 25;

	static Node nodes0[5];
	static Node nodes1[5];
	static Node nodes2[5];
	static Node nodes3[5];
	static Node nodes4[5];
	static Stage stages[5];
	static Cascade cas;
	// Cascade* casPtr = &cas;
	cas.dim = 28;
	cas.stgNum = stageNum;
	cas.featNum = featureNum;
	cas.stages = stages; 

	stages[0].nodeNum = nodeNum;
	stages[0].threshold = -2.1603300571441650e+00;
	stages[0].nodeList = &nodes0[0];

	stages[0].nodeList[0].featind = 5;
	stages[0].nodeList[0].threshold = -1.0908465832471848e-02;
	stages[0].nodeList[0].weights[0] = 7.2472470998764038e-01;
	stages[0].nodeList[0].weights[1] = -5.5860346555709839e-01;
     
	stages[0].nodeList[1].featind = 16;
	stages[0].nodeList[1].threshold = -2.5180257856845856e-02;
	stages[0].nodeList[1].weights[0] = 5.0931680202484131e-01;
	stages[0].nodeList[1].weights[1] = -6.4492475986480713e-01;

	stages[0].nodeList[2].featind = 9;
	stages[0].nodeList[2].threshold = 2.4860017001628876e-02;
	stages[0].nodeList[2].weights[0] = -3.5726237297058105e-01;
	stages[0].nodeList[2].weights[1] = 6.8936747312545776e-01;

	stages[0].nodeList[3].featind = 3;
	stages[0].nodeList[3].threshold = 1.3906182721257210e-02;
	stages[0].nodeList[3].weights[0] = -3.9029541611671448e-01;
	stages[0].nodeList[3].weights[1] = 5.5122274160385132e-01;

	stages[0].nodeList[4].featind = 24;
	stages[0].nodeList[4].threshold = -1.7670307308435440e-02;
	stages[0].nodeList[4].weights[0] = 7.9804414510726929e-01;
	stages[0].nodeList[4].weights[1] = -2.0924402773380280e-01;

	stages[1].nodeNum = nodeNum;
	stages[1].threshold = -2.2771575450897217e+00;
	stages[1].nodeList = &nodes1[0];

	stages[1].nodeList[0].featind = 15;
	stages[1].nodeList[0].threshold = -1.3321962207555771e-02;
	stages[1].nodeList[0].weights[0] = 6.9554811716079712e-01;
	stages[1].nodeList[0].weights[1] = -6.3713079690933228e-01;
     
	stages[1].nodeList[1].featind = 8;
	stages[1].nodeList[1].threshold = -1.0744407773017883e-02;
	stages[1].nodeList[1].weights[0] = 4.8026466369628906e-01;
	stages[1].nodeList[1].weights[1] = -5.6461578607559204e-01;

	stages[1].nodeList[2].featind = 2;
	stages[1].nodeList[2].threshold = 1.4804419130086899e-02;
	stages[1].nodeList[2].weights[0] = -3.8163685798645020e-01;
	stages[1].nodeList[2].weights[1] = 6.2129914760589600e-01;

	stages[1].nodeList[3].featind = 13;
	stages[1].nodeList[3].threshold = 2.3367188870906830e-02;
	stages[1].nodeList[3].weights[0] = -3.8672772049903870e-01;
	stages[1].nodeList[3].weights[1] = 5.5250954627990723e-01;

	stages[1].nodeList[4].featind = 20;
	stages[1].nodeList[4].threshold = -2.2723155096173286e-02;
	stages[1].nodeList[4].weights[0] = 5.7661956548690796e-01;
	stages[1].nodeList[4].weights[1] = -3.0704644322395325e-01;

	stages[2].nodeNum = nodeNum;
	stages[2].threshold = -1.8846212625503540e+00;
	stages[2].nodeList = &nodes2[0];

	stages[2].nodeList[0].featind = 19;
	stages[2].nodeList[0].threshold = -2.0842146128416061e-02;
	stages[2].nodeList[0].weights[0] = 8.1668627262115479e-01;
	stages[2].nodeList[0].weights[1] = -3.5519126057624817e-01;
     
	stages[2].nodeList[1].featind = 4;
	stages[2].nodeList[1].threshold = 1.0296155698597431e-02;
	stages[2].nodeList[1].weights[0] = -4.0442407131195068e-01;
	stages[2].nodeList[1].weights[1] = 6.6626828908920288e-01;

	stages[2].nodeList[2].featind = 11;
	stages[2].nodeList[2].threshold = 2.7367379516363144e-02;
	stages[2].nodeList[2].weights[0] = -3.4111016988754272e-01;
	stages[2].nodeList[2].weights[1] = 7.0092225074768066e-01;

	stages[2].nodeList[3].featind = 23;
	stages[2].nodeList[3].threshold = -4.8640649765729904e-03;
	stages[2].nodeList[3].weights[0] = 5.3481566905975342e-01;
	stages[2].nodeList[3].weights[1] = -4.0548992156982422e-01;

	stages[2].nodeList[4].featind = 18;
	stages[2].nodeList[4].threshold = 6.7999726161360741e-04;
	stages[2].nodeList[4].weights[0] = -3.7840580940246582e-01;
	stages[2].nodeList[4].weights[1] = 5.5945038795471191e-01;

	stages[3].nodeNum = nodeNum;
	stages[3].threshold = -1.2427784204483032e+00;
	stages[3].nodeList = &nodes3[0];

	stages[3].nodeList[0].featind = 1;
	stages[3].nodeList[0].threshold = 3.5535857081413269e-02;
	stages[3].nodeList[0].weights[0] = -4.4545453786849976e-01;
	stages[3].nodeList[0].weights[1] = 7.2500002384185791e-01;
     
	stages[3].nodeList[1].featind = 21;
	stages[3].nodeList[1].threshold = -1.2467866763472557e-02;
	stages[3].nodeList[1].weights[0] = 6.5345251560211182e-01;
	stages[3].nodeList[1].weights[1] = -4.3595570325851440e-01;

	stages[3].nodeList[2].featind = 14;
	stages[3].nodeList[2].threshold = 1.8763069063425064e-02;
	stages[3].nodeList[2].weights[0] = -3.1731674075126648e-01;
	stages[3].nodeList[2].weights[1] = 6.5307712554931641e-01;

	stages[3].nodeList[3].featind = 17;
	stages[3].nodeList[3].threshold = 4.2791347950696945e-03;
	stages[3].nodeList[3].weights[0] = -3.2575166225433350e-01;
	stages[3].nodeList[3].weights[1] = 5.9760004281997681e-01;

	stages[3].nodeList[4].featind = 6;
	stages[3].nodeList[4].threshold = -5.6842635385692120e-03;
	stages[3].nodeList[4].weights[0] = 2.9152497649192810e-01;
	stages[3].nodeList[4].weights[1] = -6.4165151119232178e-01;

	stages[4].nodeNum = nodeNum;
	stages[4].threshold = -1.9334802627563477e+00;
	stages[4].nodeList = &nodes4[0];

	stages[4].nodeList[0].featind = 10;
	stages[4].nodeList[0].threshold = -2.3836195468902588e-02;
	stages[4].nodeList[0].weights[0] = 6.6205251216888428e-01;
	stages[4].nodeList[0].weights[1] = -5.4893618822097778e-01;
     
	stages[4].nodeList[1].featind = 22;
	stages[4].nodeList[1].threshold = -1.8733048811554909e-02;
	stages[4].nodeList[1].weights[0] = 6.2552785873413086e-01;
	stages[4].nodeList[1].weights[1] = -3.6316964030265808e-01;

	stages[4].nodeList[2].featind = 12;
	stages[4].nodeList[2].threshold = 2.4535857141017914e-02;
	stages[4].nodeList[2].weights[0] = -3.5934367775917053e-01;
	stages[4].nodeList[2].weights[1] = 6.5213596820831299e-01;

	stages[4].nodeList[3].featind = 0;
	stages[4].nodeList[3].threshold = 1.1029189452528954e-02;
	stages[4].nodeList[3].weights[0] = -2.5696563720703125e-01;
	stages[4].nodeList[3].weights[1] = 6.2718594074249268e-01;

	stages[4].nodeList[4].featind = 7;
	stages[4].nodeList[4].threshold = 2.5213288143277168e-02;
	stages[4].nodeList[4].weights[0] = -4.0506520867347717e-01;
	stages[4].nodeList[4].weights[1] = 3.8767409324645996e-01;

	/// features match
	/// Node 0
	stages[4].nodeList[3].rectList[0].width  = 0;
	stages[4].nodeList[3].rectList[0].height = 3;
	stages[4].nodeList[3].rectList[0].x      = 4;
	stages[4].nodeList[3].rectList[0].y      = 14;
	stages[4].nodeList[3].rectList[0].weight = -1;

	stages[4].nodeList[3].rectList[1].width  = 0;
	stages[4].nodeList[3].rectList[1].height = 3;
	stages[4].nodeList[3].rectList[1].x      = 2;
	stages[4].nodeList[3].rectList[1].y      = 7;
	stages[4].nodeList[3].rectList[1].weight = 2;

	stages[4].nodeList[3].rectList[2].width  = 2;
	stages[4].nodeList[3].rectList[2].height = 10;
	stages[4].nodeList[3].rectList[2].x      = 2;
	stages[4].nodeList[3].rectList[2].y      = 7;
	stages[4].nodeList[3].rectList[2].weight = 2;

	/// Node 1
	stages[3].nodeList[0].rectList[0].width  = 0;
	stages[3].nodeList[0].rectList[0].height = 7;
	stages[3].nodeList[0].rectList[0].x      = 21;
	stages[3].nodeList[0].rectList[0].y      = 8;
	stages[3].nodeList[0].rectList[0].weight = -1;

	stages[3].nodeList[0].rectList[1].width  = 7;
	stages[3].nodeList[0].rectList[1].height = 7;
	stages[3].nodeList[0].rectList[1].x      = 7;
	stages[3].nodeList[0].rectList[1].y      = 8;
	stages[3].nodeList[0].rectList[1].weight = 3;

	// stages[3].nodeList[0].rectList[2].width  = 0;
	// stages[3].nodeList[0].rectList[2].height = 0;
	// stages[3].nodeList[0].rectList[2].x      = 0;
	// stages[3].nodeList[0].rectList[2].y      = 0;
	// stages[3].nodeList[0].rectList[2].weight = 0;

	/// Node 2
	stages[1].nodeList[2].rectList[0].width  = 0;
	stages[1].nodeList[2].rectList[0].height = 10;
	stages[1].nodeList[2].rectList[0].x      = 6;
	stages[1].nodeList[2].rectList[0].y      = 6;
	stages[1].nodeList[2].rectList[0].weight = -1;

	stages[1].nodeList[2].rectList[1].width  = 3;
	stages[1].nodeList[2].rectList[1].height = 10;
	stages[1].nodeList[2].rectList[1].x      = 3;
	stages[1].nodeList[2].rectList[1].y      = 6;
	stages[1].nodeList[2].rectList[1].weight = 2;

	// stages[1].nodeList[2].rectList[2].width  = 0;
	// stages[1].nodeList[2].rectList[2].height = 0;
	// stages[1].nodeList[2].rectList[2].x      = 0;
	// stages[1].nodeList[2].rectList[2].y      = 0;
	// stages[1].nodeList[2].rectList[2].weight = 0;

	/// Node 3
	stages[0].nodeList[3].rectList[0].width  = 0;
	stages[0].nodeList[3].rectList[0].height = 10;
	stages[0].nodeList[3].rectList[0].x      = 6;
	stages[0].nodeList[3].rectList[0].y      = 8;
	stages[0].nodeList[3].rectList[0].weight = -1;

	stages[0].nodeList[3].rectList[1].width  = 3;
	stages[0].nodeList[3].rectList[1].height = 10;
	stages[0].nodeList[3].rectList[1].x      = 3;
	stages[0].nodeList[3].rectList[1].y      = 8;
	stages[0].nodeList[3].rectList[1].weight = 2;

	// stages[0].nodeList[3].rectList[2].width  = 0;
	// stages[0].nodeList[3].rectList[2].height = 0;
	// stages[0].nodeList[3].rectList[2].x      = 0;
	// stages[0].nodeList[3].rectList[2].y      = 0;
	// stages[0].nodeList[3].rectList[2].weight = 0;

	/// Node 4
	stages[2].nodeList[1].rectList[0].width  = 0;
	stages[2].nodeList[1].rectList[0].height = 11;
	stages[2].nodeList[1].rectList[0].x      = 6;
	stages[2].nodeList[1].rectList[0].y      = 7;
	stages[2].nodeList[1].rectList[0].weight = -1;

	stages[2].nodeList[1].rectList[1].width  = 3;
	stages[2].nodeList[1].rectList[1].height = 11;
	stages[2].nodeList[1].rectList[1].x      = 3;
	stages[2].nodeList[1].rectList[1].y      = 7;
	stages[2].nodeList[1].rectList[1].weight = 2;

	// stages[2].nodeList[1].rectList[2].width  = 0;
	// stages[2].nodeList[1].rectList[2].height = 0;
	// stages[2].nodeList[1].rectList[2].x      = 0;
	// stages[2].nodeList[1].rectList[2].y      = 0;
	// stages[2].nodeList[1].rectList[2].weight = 0;

	/// Node 5
	stages[0].nodeList[0].rectList[0].width  = 3;
	stages[0].nodeList[0].rectList[0].height = 15;
	stages[0].nodeList[0].rectList[0].x      = 16;
	stages[0].nodeList[0].rectList[0].y      = 8;
	stages[0].nodeList[0].rectList[0].weight = -1;

	stages[0].nodeList[0].rectList[1].width  = 3;
	stages[0].nodeList[0].rectList[1].height = 19;
	stages[0].nodeList[0].rectList[1].x      = 16;
	stages[0].nodeList[0].rectList[1].y      = 4;
	stages[0].nodeList[0].rectList[1].weight = 2;

	// stages[0].nodeList[0].rectList[2].width  = 0;
	// stages[0].nodeList[0].rectList[2].height = 0;
	// stages[0].nodeList[0].rectList[2].x      = 0;
	// stages[0].nodeList[0].rectList[2].y      = 0;
	// stages[0].nodeList[0].rectList[2].weight = 0;

	/// Node 6
	stages[3].nodeList[4].rectList[0].width  = 3;
	stages[3].nodeList[4].rectList[0].height = 16;
	stages[3].nodeList[4].rectList[0].x      = 18;
	stages[3].nodeList[4].rectList[0].y      = 6;
	stages[3].nodeList[4].rectList[0].weight = -1;

	stages[3].nodeList[4].rectList[1].width  = 3;
	stages[3].nodeList[4].rectList[1].height = 19;
	stages[3].nodeList[4].rectList[1].x      = 18;
	stages[3].nodeList[4].rectList[1].y      = 3;
	stages[3].nodeList[4].rectList[1].weight = 2;

	// stages[3].nodeList[4].rectList[2].width  = 0;
	// stages[3].nodeList[4].rectList[2].height = 0;
	// stages[3].nodeList[4].rectList[2].x      = 0;
	// stages[3].nodeList[4].rectList[2].y      = 0;
	// stages[3].nodeList[4].rectList[2].weight = 0;

	/// Node 7
	stages[4].nodeList[4].rectList[0].width  = 4;
	stages[4].nodeList[4].rectList[0].height = 11;
	stages[4].nodeList[4].rectList[0].x      = 14;
	stages[4].nodeList[4].rectList[0].y      = 12;
	stages[4].nodeList[4].rectList[0].weight = -1;

	stages[4].nodeList[4].rectList[1].width  = 4;
	stages[4].nodeList[4].rectList[1].height = 15;
	stages[4].nodeList[4].rectList[1].x      = 14;
	stages[4].nodeList[4].rectList[1].y      = 4;
	stages[4].nodeList[4].rectList[1].weight = 3;

	// stages[4].nodeList[4].rectList[2].width  = 0;
	// stages[4].nodeList[4].rectList[2].height = 0;
	// stages[4].nodeList[4].rectList[2].x      = 0;
	// stages[4].nodeList[4].rectList[2].y      = 0;
	// stages[4].nodeList[4].rectList[2].weight = 0;

	/// Node 8
	stages[1].nodeList[1].rectList[0].width  = 4;
	stages[1].nodeList[1].rectList[0].height = 16;
	stages[1].nodeList[1].rectList[0].x      = 17;
	stages[1].nodeList[1].rectList[0].y      = 6;
	stages[1].nodeList[1].rectList[0].weight = -1;

	stages[1].nodeList[1].rectList[1].width  = 4;
	stages[1].nodeList[1].rectList[1].height = 19;
	stages[1].nodeList[1].rectList[1].x      = 17;
	stages[1].nodeList[1].rectList[1].y      = 3;
	stages[1].nodeList[1].rectList[1].weight = 2;

	// stages[1].nodeList[1].rectList[2].width  = 0;
	// stages[1].nodeList[1].rectList[2].height = 0;
	// stages[1].nodeList[1].rectList[2].x      = 0;
	// stages[1].nodeList[1].rectList[2].y      = 0;
	// stages[1].nodeList[1].rectList[2].weight = 0;

	/// Node 9
	stages[0].nodeList[2].rectList[0].width  = 7;
	stages[0].nodeList[2].rectList[0].height = 5;
	stages[0].nodeList[2].rectList[0].x      = 7;
	stages[0].nodeList[2].rectList[0].y      = 9;
	stages[0].nodeList[2].rectList[0].weight = -1;

	stages[0].nodeList[2].rectList[1].width  = 7;
	stages[0].nodeList[2].rectList[1].height = 8;
	stages[0].nodeList[2].rectList[1].x      = 7;
	stages[0].nodeList[2].rectList[1].y      = 3;
	stages[0].nodeList[2].rectList[1].weight = 3;

	// stages[0].nodeList[2].rectList[2].width  = 0;
	// stages[0].nodeList[2].rectList[2].height = 0;
	// stages[0].nodeList[2].rectList[2].x      = 0;
	// stages[0].nodeList[2].rectList[2].y      = 0;
	// stages[0].nodeList[2].rectList[2].weight = 0;

	/// Node 10
	stages[4].nodeList[0].rectList[0].width  = 7;
	stages[4].nodeList[0].rectList[0].height = 7;
	stages[4].nodeList[0].rectList[0].x      = 16;
	stages[4].nodeList[0].rectList[0].y      = 10;
	stages[4].nodeList[0].rectList[0].weight = -1;

	stages[4].nodeList[0].rectList[1].width  = 15;
	stages[4].nodeList[0].rectList[1].height = 7;
	stages[4].nodeList[0].rectList[1].x      = 8;
	stages[4].nodeList[0].rectList[1].y      = 10;
	stages[4].nodeList[0].rectList[1].weight = 2;

	// stages[4].nodeList[0].rectList[2].width  = 0;
	// stages[4].nodeList[0].rectList[2].height = 0;
	// stages[4].nodeList[0].rectList[2].x      = 0;
	// stages[4].nodeList[0].rectList[2].y      = 0;
	// stages[4].nodeList[0].rectList[2].weight = 0;

	/// Node 11
	stages[2].nodeList[2].rectList[0].width  = 8;
	stages[2].nodeList[2].rectList[0].height = 1;
	stages[2].nodeList[2].rectList[0].x      = 8;
	stages[2].nodeList[2].rectList[0].y      = 10;
	stages[2].nodeList[2].rectList[0].weight = -1;

	stages[2].nodeList[2].rectList[1].width  = 8;
	stages[2].nodeList[2].rectList[1].height = 6;
	stages[2].nodeList[2].rectList[1].x      = 8;
	stages[2].nodeList[2].rectList[1].y      = 5;
	stages[2].nodeList[2].rectList[1].weight = 2;

	// stages[2].nodeList[2].rectList[2].width  = 0;
	// stages[2].nodeList[2].rectList[2].height = 0;
	// stages[2].nodeList[2].rectList[2].x      = 0;
	// stages[2].nodeList[2].rectList[2].y      = 0;
	// stages[2].nodeList[2].rectList[2].weight = 0;

	/// Node 12
	stages[4].nodeList[2].rectList[0].width  = 8;
	stages[4].nodeList[2].rectList[0].height = 2;
	stages[4].nodeList[2].rectList[0].x      = 7;
	stages[4].nodeList[2].rectList[0].y      = 8;
	stages[4].nodeList[2].rectList[0].weight = -1;

	stages[4].nodeList[2].rectList[1].width  = 8;
	stages[4].nodeList[2].rectList[1].height = 6;
	stages[4].nodeList[2].rectList[1].x      = 7;
	stages[4].nodeList[2].rectList[1].y      = 4;
	stages[4].nodeList[2].rectList[1].weight = 2;

	// stages[4].nodeList[2].rectList[2].width  = 0;
	// stages[4].nodeList[2].rectList[2].height = 0;
	// stages[4].nodeList[2].rectList[2].x      = 0;
	// stages[4].nodeList[2].rectList[2].y      = 0;
	// stages[4].nodeList[2].rectList[2].weight = 0;

	/// Node 13
	stages[1].nodeList[3].rectList[0].width  = 9;
	stages[1].nodeList[3].rectList[0].height = 5;
	stages[1].nodeList[3].rectList[0].x      = 6;
	stages[1].nodeList[3].rectList[0].y      = 9;
	stages[1].nodeList[3].rectList[0].weight = -1;

	stages[1].nodeList[3].rectList[1].width  = 11;
	stages[1].nodeList[3].rectList[1].height = 8;
	stages[1].nodeList[3].rectList[1].x      = 2;
	stages[1].nodeList[3].rectList[1].y      = 3;
	stages[1].nodeList[3].rectList[1].weight = 9;

	// stages[1].nodeList[3].rectList[2].width  = 0;
	// stages[1].nodeList[3].rectList[2].height = 0;
	// stages[1].nodeList[3].rectList[2].x      = 0;
	// stages[1].nodeList[3].rectList[2].y      = 0;
	// stages[1].nodeList[3].rectList[2].weight = 0;

	/// Node 14
	stages[3].nodeList[2].rectList[0].width  = 9;
	stages[3].nodeList[2].rectList[0].height = 5;
	stages[3].nodeList[2].rectList[0].x      = 6;
	stages[3].nodeList[2].rectList[0].y      = 9;
	stages[3].nodeList[2].rectList[0].weight = -1;

	stages[3].nodeList[2].rectList[1].width  = 9;
	stages[3].nodeList[2].rectList[1].height = 8;
	stages[3].nodeList[2].rectList[1].x      = 6;
	stages[3].nodeList[2].rectList[1].y      = 3;
	stages[3].nodeList[2].rectList[1].weight = 3;

	// stages[3].nodeList[2].rectList[2].width  = 0;
	// stages[3].nodeList[2].rectList[2].height = 0;
	// stages[3].nodeList[2].rectList[2].x      = 0;
	// stages[3].nodeList[2].rectList[2].y      = 0;
	// stages[3].nodeList[2].rectList[2].weight = 0;

	/// Node 15
	stages[1].nodeList[0].rectList[0].width  = 9;
	stages[1].nodeList[0].rectList[0].height = 7;
	stages[1].nodeList[0].rectList[0].x      = 14;
	stages[1].nodeList[0].rectList[0].y      = 9;
	stages[1].nodeList[0].rectList[0].weight = -1;

	stages[1].nodeList[0].rectList[1].width  = 16;
	stages[1].nodeList[0].rectList[1].height = 7;
	stages[1].nodeList[0].rectList[1].x      = 7;
	stages[1].nodeList[0].rectList[1].y      = 9;
	stages[1].nodeList[0].rectList[1].weight = 2;

	// stages[1].nodeList[0].rectList[2].width  = 0;
	// stages[1].nodeList[0].rectList[2].height = 0;
	// stages[1].nodeList[0].rectList[2].x      = 0;
	// stages[1].nodeList[0].rectList[2].y      = 0;
	// stages[1].nodeList[0].rectList[2].weight = 0;

	/// Node 16
	stages[0].nodeList[1].rectList[0].width  = 9;
	stages[0].nodeList[1].rectList[0].height = 7;
	stages[0].nodeList[1].rectList[0].x      = 14;
	stages[0].nodeList[1].rectList[0].y      = 10;
	stages[0].nodeList[1].rectList[0].weight = -1;

	stages[0].nodeList[1].rectList[1].width  = 16;
	stages[0].nodeList[1].rectList[1].height = 7;
	stages[0].nodeList[1].rectList[1].x      = 7;
	stages[0].nodeList[1].rectList[1].y      = 10;
	stages[0].nodeList[1].rectList[1].weight = 2;

	// stages[0].nodeList[1].rectList[2].width  = 0;
	// stages[0].nodeList[1].rectList[2].height = 0;
	// stages[0].nodeList[1].rectList[2].x      = 0;
	// stages[0].nodeList[1].rectList[2].y      = 0;
	// stages[0].nodeList[1].rectList[2].weight = 0;

	/// Node 17
	stages[3].nodeList[3].rectList[0].width  = 10;
	stages[3].nodeList[3].rectList[0].height = 12;
	stages[3].nodeList[3].rectList[0].x      = 3;
	stages[3].nodeList[3].rectList[0].y      = 6;
	stages[3].nodeList[3].rectList[0].weight = -1;

	stages[3].nodeList[3].rectList[1].width  = 10;
	stages[3].nodeList[3].rectList[1].height = 14;
	stages[3].nodeList[3].rectList[1].x      = 3;
	stages[3].nodeList[3].rectList[1].y      = 2;
	stages[3].nodeList[3].rectList[1].weight = 3;

	// stages[3].nodeList[3].rectList[2].width  = 0;
	// stages[3].nodeList[3].rectList[2].height = 0;
	// stages[3].nodeList[3].rectList[2].x      = 0;
	// stages[3].nodeList[3].rectList[2].y      = 0;
	// stages[3].nodeList[3].rectList[2].weight = 0;

	/// Node 18
	stages[2].nodeList[4].rectList[0].width  = 11;
	stages[2].nodeList[4].rectList[0].height = 13;
	stages[2].nodeList[4].rectList[0].x      = 4;
	stages[2].nodeList[4].rectList[0].y      = 2;
	stages[2].nodeList[4].rectList[0].weight = -1;

	stages[2].nodeList[4].rectList[1].width  = 11;
	stages[2].nodeList[4].rectList[1].height = 14;
	stages[2].nodeList[4].rectList[1].x      = 4;
	stages[2].nodeList[4].rectList[1].y      = 1;
	stages[2].nodeList[4].rectList[1].weight = 2;

	// stages[2].nodeList[4].rectList[2].width  = 0;
	// stages[2].nodeList[4].rectList[2].height = 0;
	// stages[2].nodeList[4].rectList[2].x      = 0;
	// stages[2].nodeList[4].rectList[2].y      = 0;
	// stages[2].nodeList[4].rectList[2].weight = 0;

	/// Node 19
	stages[2].nodeList[0].rectList[0].width  = 15;
	stages[2].nodeList[0].rectList[0].height = 2;
	stages[2].nodeList[0].rectList[0].x      = 8;
	stages[2].nodeList[0].rectList[0].y      = 6;
	stages[2].nodeList[0].rectList[0].weight = -1;

	stages[2].nodeList[0].rectList[1].width  = 15;
	stages[2].nodeList[0].rectList[1].height = 2;
	stages[2].nodeList[0].rectList[1].x      = 8;
	stages[2].nodeList[0].rectList[1].y      = 3;
	stages[2].nodeList[0].rectList[1].weight = 2;

	// stages[2].nodeList[0].rectList[2].width  = 0;
	// stages[2].nodeList[0].rectList[2].height = 0;
	// stages[2].nodeList[0].rectList[2].x      = 0;
	// stages[2].nodeList[0].rectList[2].y      = 0;
	// stages[2].nodeList[0].rectList[2].weight = 0;

	/// Node 20
	stages[1].nodeList[4].rectList[0].width  = 17;
	stages[1].nodeList[4].rectList[0].height = 0;
	stages[1].nodeList[4].rectList[0].x      = 6;
	stages[1].nodeList[4].rectList[0].y      = 18;
	stages[1].nodeList[4].rectList[0].weight = -1;

	stages[1].nodeList[4].rectList[1].width  = 17;
	stages[1].nodeList[4].rectList[1].height = 0;
	stages[1].nodeList[4].rectList[1].x      = 3;
	stages[1].nodeList[4].rectList[1].y      = 9;
	stages[1].nodeList[4].rectList[1].weight = 2;

	stages[1].nodeList[4].rectList[2].width  = 20;
	stages[1].nodeList[4].rectList[2].height = 9;
	stages[1].nodeList[4].rectList[2].x      = 3;
	stages[1].nodeList[4].rectList[2].y      = 9;
	stages[1].nodeList[4].rectList[2].weight = 2;

	/// Node 21
	stages[3].nodeList[1].rectList[0].width  = 17;
	stages[3].nodeList[1].rectList[0].height = 11;
	stages[3].nodeList[1].rectList[0].x      = 6;
	stages[3].nodeList[1].rectList[0].y      = 7;
	stages[3].nodeList[1].rectList[0].weight = -1;

	stages[3].nodeList[1].rectList[1].width  = 20;
	stages[3].nodeList[1].rectList[1].height = 11;
	stages[3].nodeList[1].rectList[1].x      = 3;
	stages[3].nodeList[1].rectList[1].y      = 7;
	stages[3].nodeList[1].rectList[1].weight = 2;

	// stages[3].nodeList[1].rectList[2].width  = 0;
	// stages[3].nodeList[1].rectList[2].height = 0;
	// stages[3].nodeList[1].rectList[2].x      = 0;
	// stages[3].nodeList[1].rectList[2].y      = 0;
	// stages[3].nodeList[1].rectList[2].weight = 0;

	/// Node 22
	stages[4].nodeList[1].rectList[0].width  = 17;
	stages[4].nodeList[1].rectList[0].height = 11;
	stages[4].nodeList[1].rectList[0].x      = 6;
	stages[4].nodeList[1].rectList[0].y      = 9;
	stages[4].nodeList[1].rectList[0].weight = -1;

	stages[4].nodeList[1].rectList[1].width  = 20;
	stages[4].nodeList[1].rectList[1].height = 11;
	stages[4].nodeList[1].rectList[1].x      = 3;
	stages[4].nodeList[1].rectList[1].y      = 9;
	stages[4].nodeList[1].rectList[1].weight = 2;

	// stages[4].nodeList[1].rectList[2].width  = 0;
	// stages[4].nodeList[1].rectList[2].height = 0;
	// stages[4].nodeList[1].rectList[2].x      = 0;
	// stages[4].nodeList[1].rectList[2].y      = 0;
	// stages[4].nodeList[1].rectList[2].weight = 0;

	/// Node 23
	stages[2].nodeList[3].rectList[0].width  = 17;
	stages[2].nodeList[3].rectList[0].height = 13;
	stages[2].nodeList[3].rectList[0].x      = 6;
	stages[2].nodeList[3].rectList[0].y      = 3;
	stages[2].nodeList[3].rectList[0].weight = -1;

	stages[2].nodeList[3].rectList[1].width  = 20;
	stages[2].nodeList[3].rectList[1].height = 13;
	stages[2].nodeList[3].rectList[1].x      = 3;
	stages[2].nodeList[3].rectList[1].y      = 3;
	stages[2].nodeList[3].rectList[1].weight = 2;

	// stages[2].nodeList[3].rectList[2].width  = 0;
	// stages[2].nodeList[3].rectList[2].height = 0;
	// stages[2].nodeList[3].rectList[2].x      = 0;
	// stages[2].nodeList[3].rectList[2].y      = 0;
	// stages[2].nodeList[3].rectList[2].weight = 0;

	/// Node 24
	stages[0].nodeList[4].rectList[0].width  = 18;
	stages[0].nodeList[4].rectList[0].height = 3;
	stages[0].nodeList[4].rectList[0].x      = 4;
	stages[0].nodeList[4].rectList[0].y      = 14;
	stages[0].nodeList[4].rectList[0].weight = -1;

	stages[0].nodeList[4].rectList[1].width  = 18;
	stages[0].nodeList[4].rectList[1].height = 3;
	stages[0].nodeList[4].rectList[1].x      = 2;
	stages[0].nodeList[4].rectList[1].y      = 7;
	stages[0].nodeList[4].rectList[1].weight = 2;

	stages[0].nodeList[4].rectList[2].width  = 20;
	stages[0].nodeList[4].rectList[2].height = 10;
	stages[0].nodeList[4].rectList[2].x      = 2;
	stages[0].nodeList[4].rectList[2].y      = 7;
	stages[0].nodeList[4].rectList[2].weight = 2;

	return (Cascade*) &cas;
}