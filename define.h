//diffuse가 가장 물체의 색상과 연관있음 // 근데 젤 상관없어보임. ambient가 젤 큰영향같음

#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <gl/glut.h>
#include <stdio.h>
#include <stdlib.h>

#define GL_TEXTURE_CUBE_MAP               0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X    0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X    0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y    0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y    0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z    0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z    0x851A
#define GL_CLAMP_TO_EDGE                  0x812F
#define GL_TEXTURE_WRAP_R                 0x8072
#define GL_REFLECTION_MAP                 0x8512
typedef unsigned char uchar;

int index = 1 , color = 0;

bool antialiase = true;

int gamemode = 0;

int drag;
double tempX, tempY;


/*inline void draw_obj(ObjParser *objParser) {
	glBegin(GL_TRIANGLES);
	for (unsigned int n = 0; n < objParser->getFaceSize(); n += 3) {
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n] - 1].x,
			objParser->vertices[objParser->vertexIdx[n] - 1].y,
			objParser->vertices[objParser->vertexIdx[n] - 1].z);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 1] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 1] - 1].z);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(objParser->vertices[objParser->vertexIdx[n + 2] - 1].x,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].y,
			objParser->vertices[objParser->vertexIdx[n + 2] - 1].z);
	}
	glEnd();
}
*/

using namespace std;

class CPoint2f {
public:
	vector <float> d{ 0,0 };
};

class CPoint3f {
public:
	vector <float> d{ 0,0,0 };
};

class CPoint3i {
public:
	vector <int> d{ 0,0,0 };
};

class CFace {
public:
	vector <CPoint3i> v_pairs;
};

class CObj {
public:
	string name;
	vector <CPoint3f> v;
	vector <CPoint2f> vt;
	vector <CPoint3f> vn;
	vector <CFace> f;
};

class CModel {
public:
	vector <CObj> objs;

	vector<float> my_strtok_f(char* str, char* deliameter) {
		vector <float> v;
		char* context;
		char* tok = strtok_s(str, deliameter, &context);

		while (tok != NULL) {
			v.push_back(atof(tok));
			tok = strtok_s(NULL, deliameter, &context);
		}
		return v;
	}

	vector<string> my_strtok_s(char* str, char* deliameter) {
		vector <string> v;
		char* context;
		char* tok = strtok_s(str, deliameter, &context);

		while (tok != NULL) {
			v.push_back(tok);
			tok = strtok_s(context, deliameter, &context);
		}
		return v;
	}



	vector<int> my_strtok_i(char* str, char* deliameter) {
		vector <int> v;
		char* context;
		char* tok = strtok_s(str, deliameter, &context);

		while (tok != NULL) {
			v.push_back(atof(tok));
			tok = strtok_s(context, deliameter, &context);
		}
		return v;
	}

	void loadObj(ifstream& fin) {
		string line;
		CObj obj_tmp;
		int cnt = 0;
		//이전 obj까지의 누적 벡터수
		int cnt_prev_vertex = 0;
		int cnt_prev_texture = 0;
		int cnt_prev_normal = 0;
		while (getline(fin, line)) {
			int len = line.length();
			vector<float> vf;
			vector<string> s;
			vector<int> vi;
			CPoint3f p3;
			CPoint2f p2;
			CPoint3i p3i;
			if (line[0] == 'o' && line[1] == ' ') {
				obj_tmp.name = line.substr(2, len - 2);
				objs.push_back(obj_tmp);
				if (cnt > 0) {
					cnt_prev_vertex += objs[cnt - 1].v.size();
					cnt_prev_texture += objs[cnt - 1].vt.size();
					cnt_prev_normal += objs[cnt - 1].vn.size();
				}
				cnt += 1;
			}

			if (line[0] == 'v' && line[1] == ' ') {
				vf = my_strtok_f((char*)line.substr(2, len - 2).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1], vf[2] };
				objs[cnt - 1].v.push_back(p3);
			}
			else if (line[0] == 'v' && line[1] == 't') {
				vf = my_strtok_f((char*)line.substr(3, len - 3).c_str(), (char*)" ");
				p2.d = { vf[0], vf[1] };
				objs[cnt - 1].vt.push_back(p2);
			}
			else if (line[0] == 'v' && line[1] == 'n') {
				vf = my_strtok_f((char*)line.substr(3, len - 3).c_str(), (char*)" ");
				p3.d = { vf[0], vf[1], vf[2] };
				objs[cnt - 1].vn.push_back(p3);
			}
			else if (line[0] == 'f' && line[1] == ' ') {
				s = my_strtok_s((char*)line.substr(2, len - 2).c_str(), (char*)" ");
				int nVertexes = s.size();
				CFace face_tmp;
				for (int i = 0; i < nVertexes; i++) {
					vi = my_strtok_i((char*)s[i].c_str(), (char*)"/");
					p3i.d = { vi[0] - cnt_prev_vertex, vi[1] - cnt_prev_texture,vi[2] - cnt_prev_normal };
					face_tmp.v_pairs.push_back(p3i);
				}
				objs[cnt - 1].f.push_back(face_tmp);
			}
		}
	}
};

uchar* readImageData(const char* filename, int* width, int* height, int* channels)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	int dataPos;     // Position in the file where the actual data begins
	int imageSize;   // = width*height*3 or width*height
	// Actual RGB data
	uchar* imageData;

	// Open the file
	FILE* file = fopen(filename, "rb");
	if (!file) { printf("bmpfunc.cpp : Image could not be opened\n"); return 0; }


	if (header[0] != 'B' || header[1] != 'M') {
		printf("bmpfunc.cpp : Not a correct BMP file\n");
		return 0;
	}

	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);

	if (imageSize == 0)
	{
		printf("bmpfunc.cpp : There is no channel info, assume input image channel as 3\n");
		*channels = 3;
		imageSize = (*width) * (*height) * 3; // Assume 3 channels
	}
	else {
		if (imageSize == (*width) * (*height)) {
			printf("bmpfunc.cpp : Input img : 8bit-img\n");
			*channels = 1;
		}
		else if (imageSize == (*width) * (*height) * 3) {
			printf("bmpfunc.cpp : input img : 24bit-img\n");
			*channels = 3;
		}
		else { // Assume 3 channels
			*channels = 3;
		}
	}
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

#if VERBOSE
	printf("bmpfunc.cpp : width = %d\n", *width);
	printf("bmpfunc.cpp : height = %d\n", *height);
	printf("bmpfunc.cpp : imgSize = %d\n", imageSize);
#endif

	imageData = (uchar*)malloc(imageSize);

	fread(imageData, 1, imageSize, file);
	fclose(file);

	// flip R & B // BGR order -> RGB order
	if (*channels == 3) {
		for (int i = 0; i < (*height); i++)
		{
			for (int j = 0; j < (*width); j++)
			{
				uchar tmp = imageData[3 * (i * (*width) + j)]; // R
				imageData[3 * (i * (*width) + j)] = imageData[3 * (i * (*width) + j) + 2];
				imageData[3 * (i * (*width) + j) + 2] = tmp;
			}
		}
	}

	return imageData;
}


#pragma once
