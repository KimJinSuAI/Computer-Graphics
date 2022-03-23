#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/opencv.hpp>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <list>
#include "define.h"
#include <fmod.h>

#define _WINDOW_HEIGHT 1000
#define _WINDOW_WIDTH 2000

FMOD_SYSTEM* System;
FMOD_CHANNEL* Channel[3];
FMOD_SOUND* Sound[3];
GLfloat camx = 0, camy = 2, camz = 2;
GLfloat lx = 0.0f, ly = 0.0f, lz = -1.0f;
GLfloat cam_upx = 0, cam_upy = 1, cam_upz = 0;


using namespace std;
using namespace cv;

int ammo = 30;

float speed = 1;
float zombieSpeed = 0.7;
float bulletSpeed = 4;

float bulletBorder = 1.5;

int zombieCount = 0;

float eyeSensitivity = 0.01f;
int minMapX = -55;
int maxMapX = 55;
int minMapZ = -130;
int maxMapZ = 130;
float mapDistanceX = (maxMapX - minMapX) / 4;
float mapDistanceZ = (maxMapZ - minMapZ) / 4;

float lastMouseX = _WINDOW_WIDTH / 2; float lastMouseY = _WINDOW_HEIGHT / 2;
float yaw = 0;
float pitch = 0;

int moveMode = 0;

GLfloat lightPos0[4];
GLfloat lightPos0_tmpx = 0;
GLfloat lightPos0_tmpy = 0;
GLfloat lightPos0_tmpz = 0;

GLfloat IdleTime = 0;

CModel head;
CModel body;
CModel larm;
CModel rarm;
CModel lfoot;
CModel rfoot;
CModel uzi;
CModel pistol;
CModel shotgun;
CModel shotgunPath;
CModel bullet;
CModel hairband;
CModel floor_o;
CModel wall1, wall2, wall3, wall4;
CModel moon;
CModel sky;
CModel item;
CModel clear;

Mat image[10];
GLuint tex_ids[10] = { 1,2,3,4,5,6,7,8,9 };
int nTex = 10;


void setMaterialfv(GLfloat ambient[4], GLfloat diffuse[4], GLfloat specular[4], GLfloat shiness[4]) {
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shiness);
}

void draw_obj(CModel m, char ch) {
	GLfloat x, y, z, nx, ny, nz, tx, ty;
	int v_id, vt_id, vn_id;
	int idx;
	for (int o = 0; o < m.objs.size(); o++) {
		if (ch == 'w') {
			idx = 0;
		}
		else if (ch == 'f') {
			idx = 1;
		}
		else if (ch == 's') {
			idx = 2;
		}
		else if (ch == 'B') {
			idx = 5;
		}
		else if (ch == 'b') {
			idx = 6;
		}
		else if (ch == 'S') {
			idx = 7;
		}
		else if (ch == 'P') {
			idx = 8;
		}
		
		if (ch != ' ') {
			glBindTexture(GL_TEXTURE_2D, tex_ids[idx]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[idx].cols, image[idx].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[idx].data);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//안티에일리어싱
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//안티에일리어싱
		}
		int nFaces = m.objs[o].f.size();
		for (int k = 0; k < nFaces; k++) {
			int nPoints = m.objs[o].f[k].v_pairs.size();
			glBegin(GL_POLYGON);
			for (int i = 0; i < nPoints; i++) {
				v_id = m.objs[o].f[k].v_pairs[i].d[0];
				vt_id = m.objs[o].f[k].v_pairs[i].d[1];
				vn_id = m.objs[o].f[k].v_pairs[i].d[2];
				x = m.objs[o].v[v_id - 1].d[0];
				y = m.objs[o].v[v_id - 1].d[1];
				z = m.objs[o].v[v_id - 1].d[2];

				nx = m.objs[o].vn[vn_id - 1].d[0];
				ny = m.objs[o].vn[vn_id - 1].d[1];
				nz = m.objs[o].vn[vn_id - 1].d[2];

				glNormal3f(nx, ny, nz);
				if (ch != ' ') {
					tx = m.objs[o].vt[vt_id - 1].d[0];
					ty = m.objs[o].vt[vt_id - 1].d[1];
					glTexCoord2f(tx, ty);
				}

				glVertex3f(x, y, z);
			}
			glEnd();
		}
	}
}

void obj_read() {
	ifstream head_s("./Object/head.obj");
	ifstream body_s("./Object/body.obj");
	ifstream larm_s("./Object/larm.obj");
	ifstream rarm_s("./Object/rarm.obj");
	ifstream lfoot_s("./Object/lfoot.obj");
	ifstream rfoot_s("./Object/rfoot.obj");
	ifstream pistol_s("./Object/pistol.obj");
	ifstream uzi_s("./Object/uzi.obj");
	ifstream shotgun_s("./Object/shotgun.obj");
	ifstream shotgunPath_s("./Object/shotgunPath.obj");
	ifstream bullet_s("./Object/bullet.obj");
	ifstream hairband_s("./Object/hair_band.obj");
	ifstream floor_s("./Object/floor.obj");
	ifstream wall1_s("./Object/wall1.obj");
	ifstream wall2_s("./Object/wall2.obj");
	ifstream wall3_s("./Object/wall3.obj");
	ifstream wall4_s("./Object/wall4.obj");
	ifstream moon_s("./Object/moon.obj");
	ifstream sky_s("./Object/sky.obj");
	ifstream item_s("./Object/item.obj");
	ifstream clear_s("./Object/clear.obj");

	head.loadObj(head_s);
	body.loadObj(body_s);
	larm.loadObj(larm_s);
	rarm.loadObj(rarm_s);
	lfoot.loadObj(lfoot_s);
	rfoot.loadObj(rfoot_s);
	uzi.loadObj(uzi_s);
	pistol.loadObj(pistol_s);
	shotgun.loadObj(shotgun_s);
	shotgunPath.loadObj(shotgunPath_s);
	bullet.loadObj(bullet_s);
	hairband.loadObj(hairband_s);
	floor_o.loadObj(floor_s);
	wall1.loadObj(wall1_s);
	wall2.loadObj(wall2_s);
	wall3.loadObj(wall3_s);
	wall4.loadObj(wall4_s);
	moon.loadObj(moon_s);
	sky.loadObj(sky_s);
	item.loadObj(item_s);
	clear.loadObj(clear_s);

	head_s.close();
	body_s.close();
	larm_s.close();
	rarm_s.close();
	lfoot_s.close();
	rfoot_s.close();
	uzi_s.close();
	pistol_s.close();
	shotgun_s.close();
	shotgun_s.close();
	bullet_s.close();
	hairband_s.close();
	floor_s.close();
	wall1_s.close();
	wall2_s.close();
	wall3_s.close();
	wall4_s.close();
	moon_s.close();
	sky_s.close();
	item_s.close();
	clear_s.close();
}

class Object {
public:
	string name;
	GLfloat x, y, z;						//물체좌표
	GLfloat vector_x = 0;
	GLfloat vector_y = 0;
	GLfloat vector_z = 1;		//물체 회전각
	GLfloat angle = 0;
	GLfloat minX, maxX, minY, maxY, minZ, maxZ;			//min,max x,y,z좌표
	GLfloat hp;
	GLfloat armAngle = 0;
	GLfloat armDirection = -1;
	GLfloat footAngle = 0;
	GLfloat footDirection = 1;
	bool walking = false;
	float time = 0;

	Object() {}
	Object(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) {//물체 생성, 충돌구간 설정
		x = x1, y = y1, z = z1;
		setColiisionBorder(minX1, maxX1, minY1, maxY1, minZ1, maxZ1);
		name = name1;
	}
	Object(GLfloat hp1) { hp = hp1; }

	void setColiisionBorder(GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) {	//minmax x,z점설정
		minX = minX1;
		maxX = maxX1;
		minY = minY1;
		maxY = maxY1;
		minZ = minZ1;
		maxZ = maxZ1;
	}
};

class MovableObject : public Object {
public:
	int speed;
	MovableObject() {}
	MovableObject(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1)
		: Object(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
	}

	void setColiisionBorder(GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) {	//minmax x,z점설정
		Object::setColiisionBorder(minX1, maxX1, minY1, maxY1, minZ1, maxZ1);
	}
};

class Man : public MovableObject {
public:
	Man() {}
	Man(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) : MovableObject(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
	}

	void setColiisionBorder(GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) {	//minmax x,z점설정
		Object::setColiisionBorder(minX1, maxX1, minY1, maxY1, minZ1, maxZ1);
	}

	void die() {			//체력 0되면 사망

	}
};

class User : public Man {
public:
	bool isButtonClicked = false;
public:
	User(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) :Man(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1)
	{
		hp = 100;
	}

	void getItem() {		//아이템 획득

	}

	void swapItem() {		//아이템전환

	}
};

class Zombie :public Man {
public:
	Zombie() {}
	Zombie(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1)
		:Man(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
		Object:hp = 80;
	}

	void die() {
		Man::die();
		dropItem();
	}

	void dropItem() {

	}
};

class Projectile : public Object {
public:
	Projectile() {};
	Projectile(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1)
		: Object(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
	}
};

class SpecialZombie : public Zombie {
	Projectile projectile;

	SpecialZombie(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1)
		:Zombie(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
		hp = 120;
	}

	void fire() {			//재정의. 특수좀비는 공격 시 발사체 발사

	}

	bool approach() {		//재정의. 특수좀비는 멀리서 공격가능

	}
};

class Wall : public Object {
public:
	Wall(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) : Object(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
	}
};

class Floor : public Object {
public:
	Floor(string name1, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat minX1, GLfloat maxX1, GLfloat minY1, GLfloat maxY1, GLfloat minZ1, GLfloat maxZ1) : Object(name1, x1, y1, z1, minX1, maxX1, minY1, maxY1, minZ1, maxZ1) {
	}
};

class Ceil : public Object {
public:
};

User* user;
list<Object*> objList;			//16개의 구간 (쿼드트리)
GLuint g_nCubeTex;

bool Collision(Object* i, list<Object*>::iterator j) {
	if ((*i).x < (*j)->x) {
		if ((*i).y < (*j)->y) {
			if ((*i).z < (*j)->z) {
				return (*i).maxX > (*j)->minX && (*i).maxY > (*j)->minY && (*i).maxZ > (*j)->minZ ? true : false;
			}
			else {
				return (*i).maxX > (*j)->minX && (*i).maxY > (*j)->minY && (*i).minZ < (*j)->maxZ ? true : false;
			}
		}
		else {
			if ((*i).z < (*j)->z) {
				return (*i).maxX > (*j)->minX && (*i).minY < (*j)->maxY && (*i).maxZ >(*j)->minZ ? true : false;
			}
			else {
				return (*i).maxX > (*j)->minX && (*i).minY < (*j)->maxY && (*i).minZ < (*j)->maxZ ? true : false;
			}
		}
	}
	else {
		if ((*i).y < (*j)->y) {
			if ((*i).z < (*j)->z) {
				return (*i).minX < (*j)->maxX && (*i).maxY >(*j)->minY && (*i).maxZ > (*j)->minZ ? true : false;
			}
			else {
				return (*i).minX < (*j)->maxX && (*i).maxY >(*j)->minY && (*i).minZ < (*j)->maxZ ? true : false;
			}
		}
		else {
			if ((*i).z < (*j)->z) {
				return (*i).minX < (*j)->maxX && (*i).minY < (*j)->maxY && (*i).maxZ >(*j)->minZ ? true : false;
			}
			else {
				return (*i).minX < (*j)->maxX && (*i).minY < (*j)->maxY && (*i).minZ < (*j)->maxZ ? true : false;
			}
		}
	}
}

Object* isCollide(Object* i) {						//구간내 개체와 충돌했나?
	for (list<Object*>::iterator j = objList.begin(); j != objList.end(); j++) {
		if (Collision(i,j) && (*j)->name != (*i).name) {
			return (*j);
		}
	}
	return NULL;
}

void init_texture() {
	glGenTextures(nTex, tex_ids);
	glEnable(GL_TEXTURE_2D);
}

void reshape(int width, int height) {
	GLfloat left = 0, bottom = 0;
	glViewport(left, bottom, (width - left), (height - bottom));		//뷰포트, 어디부터 그릴건지 결정
	glMatrixMode(GL_PROJECTION);										//모델을 투영행렬에 곱해 원근적인 표현을 하게함.
	glLoadIdentity();													//좌표계 초기화
	GLfloat ratio = (float)(width - left) / (height - bottom);
	gluPerspective(45, ratio, 0.1, 800);									//원근 투영을 설정한다. 매개변수는 시야각, 종횡비, 앞면, 뒷면이다.
}

void drawArrow() {
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(5, 0, 0);
	glVertex3f(5, 0, 0);
	glVertex3f(3, 1, 0);
	glVertex3f(5, 0, 0);
	glVertex3f(3, -1, 0);
	glEnd();
}

void drawFloor() {
	draw_obj(floor_o, 'f');
}

void drawCeil() {
	glBegin(GL_QUADS);				//천장
	glColor3d(0.07, 0.13, 0.2);
	glVertex3f(minMapX, 5, minMapZ);
	glVertex3f(minMapX, 5, maxMapZ);
	glVertex3f(maxMapX, 5, maxMapZ);
	glVertex3f(maxMapX, 5, minMapZ);
	glEnd();


	glBegin(GL_LINES);				//천장타일
	glColor3d(0, 0, 0);
	for (int i = 0; i < (maxMapX - minMapX) / 5 + 1; i++) {
		glVertex3f(minMapX + i * 5, 5, minMapZ);
		glVertex3f(minMapX + i * 5, 5, maxMapZ);
	}
	for (int i = 0; i < (maxMapZ - minMapZ) / 5 + 1; i++) {
		glVertex3f(minMapX, 5, minMapZ + i * 5);
		glVertex3f(maxMapX, 5, minMapZ + i * 5);
	}
	glEnd();
}

void drawWall() {				//벽
	draw_obj(wall1, 'w');
	draw_obj(wall2, 'w');
	draw_obj(wall3, 'w');
	draw_obj(wall4, 'w');
}

void drawWorld() {
	draw_obj(sky, 's');

	glPushMatrix();
	glTranslatef(lightPos0[0], lightPos0[1], lightPos0[2]);
	draw_obj(moon, 'b');
	glPopMatrix();

	drawFloor();
	//drawCeil();
	drawWall();
}

void drawShotgun() {
	draw_obj(shotgun, 'S');
	draw_obj(shotgunPath, 'P');
}

void drawArm(bool isArmed, bool isZombie, GLfloat armAngle) {
	glPushMatrix();
	glTranslatef(0.4, 2.5, -0.3);
	//GLfloat x = IdleTime;
	glPushMatrix();
	if (!isArmed) {
		glRotatef(-90, 1, 0, 0);
		if (isZombie) {
			glRotatef(armAngle,1,0,0);
		}
	}
	if (isZombie) {
		draw_obj(larm, 'B');
	}
	else {
		GLfloat ambient[] = { 0.85, 0.51, 0.60, 0 };	//살색
		GLfloat diffuse[] = { 0.85, 0.51, 0.60,0 };
		GLfloat specular[] = { 0.85, 0.51, 0.60,0 };
		draw_obj(larm, ' ');
	}
	glPopMatrix();
	glTranslatef(-1.9, 0, 0);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	if (isZombie) {
		glRotatef(armAngle, 1, 0, 0);
	}
	if (isZombie) {
		draw_obj(rarm, 'B');
	}
	else {
		draw_obj(rarm, ' ');
	}
	glPopMatrix();
	if (isArmed) {
		glEnable(GL_TEXTURE_2D);
		glPushMatrix();
		glTranslatef(0.5, 0.1, 3.5);
		drawShotgun();
		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	}
	glPopMatrix();
}

void drawUser() {
	glPushMatrix();
	glTranslatef(camx, camy - 4, camz);
	glRotatef(61.3 * yaw, 0, 1, 0);

	glDisable(GL_TEXTURE_2D);
	GLfloat ambient[] = { 0.85, 0.51, 0.60, 0 };	//살색
	GLfloat diffuse[] = { 0.85, 0.51, 0.60,0 };
	GLfloat specular[] = { 0.85, 0.51, 0.60,0 };
	GLfloat shininess[] = { 0 };
	GLfloat ambient1[] = { 0, 0, 0, 1.0 };		//검은색
	GLfloat diffuse1[] = { 0, 0, 0,1.0 };
	GLfloat specular1[] = { 0, 0, 0,1.0 };
	GLfloat shininess1[] = { 0 };
	GLfloat ambient2[] = { 0.97, 0.24, 0, 0 };	//주황색
	GLfloat diffuse2[] = { 0.97, 0.24, 0,0 };
	GLfloat specular2[] = { 0.97, 0.24, 0,0 };
	GLfloat shininess2[] = { 0 };
	setMaterialfv(ambient, diffuse, specular, shininess);
	draw_obj(head, ' ');

	setMaterialfv(ambient2, diffuse2, specular2, shininess2);
	draw_obj(hairband, ' ');
	setMaterialfv(ambient, diffuse, specular, shininess);
	draw_obj(body, ' ');
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	drawArm(true, false, 0);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	setMaterialfv(ambient1, diffuse1, specular1, shininess1);
	draw_obj(lfoot, ' ');
	draw_obj(rfoot, ' ');
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void drawZombie(Object* i) {
	glPushMatrix();
	GLfloat ambient1[] = { 0.3, 0.59, 0.32, 1.0 };
	GLfloat diffuse1[] = { 0.3, 0.59, 0.32,1.0 };
	GLfloat specular1[] = { 0.3, 0.59, 0.32,1.0 };
	GLfloat shininess1[] = { 25 };
	setMaterialfv(ambient1, diffuse1, specular1, shininess1);
	glTranslatef((*i).x, 0.2, (*i).z);

	if ((*i).x < user->x) {
		glRotatef((*i).angle, 0, 1, 0);
	}
	else {
		glRotatef(-(*i).angle, 0, 1, 0);
	}

	//draw_obj(head, ' ');

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_ids[3]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3].cols, image[3].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[3].data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//안티에일리어싱
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//안티에일리어싱
	glTranslatef(-0.3, 3.7, -0.3);

	GLfloat vF1[3] = { -0.7,-0.7,0.7 };
	GLfloat vF2[3] = { -0.7,0.7,0.7 };
	GLfloat vF3[3] = { 0.7,0.7,0.7 };
	GLfloat vF4[3] = { 0.7,-0.7,0.7 };
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(vF1[0], vF1[1], vF1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vF2[0], vF2[1], vF2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vF3[0], vF3[1], vF3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vF4[0], vF4[1], vF4[2]);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex_ids[4]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[4].cols, image[4].rows, 0, GL_RGB, GL_UNSIGNED_BYTE, image[4].data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//안티에일리어싱
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	//안티에일리어싱

	GLfloat vFB1[3] = { -0.7,-0.7,-0.7 };
	GLfloat vFB2[3] = { -0.7,0.7,-0.7 };
	GLfloat vFB3[3] = { 0.7,0.7,-0.7 };
	GLfloat vFB4[3] = { 0.7,-0.7,-0.7 };
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glNormal3f(0, 0, -1);
	glVertex3f(vFB1[0], vFB1[1], vFB1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vFB2[0], vFB2[1], vFB2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vFB3[0], vFB3[1], vFB3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vFB4[0], vFB4[1], vFB4[2]);
	glEnd();

	GLfloat vFBO1[3] = { -0.7,-0.7,-0.7 };
	GLfloat vFBO2[3] = { -0.7,-0.7,0.7 };
	GLfloat vFBO3[3] = { 0.7,-0.7,0.7 };
	GLfloat vFBO4[3] = { 0.7,-0.7,-0.7 };
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glNormal3f(0, -1, 0);
	glVertex3f(vFBO1[0], vFBO1[1], vFBO1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vFBO2[0], vFBO2[1], vFBO2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vFBO3[0], vFBO3[1], vFBO3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vFBO4[0], vFBO4[1], vFBO4[2]);
	glEnd();


	GLfloat vFT1[3] = { -0.7,0.7,-0.7 };
	GLfloat vFT2[3] = { -0.7,0.7,0.7 };
	GLfloat vFT3[3] = { 0.7,0.7,0.7 };
	GLfloat vFT4[3] = { 0.7,0.7,-0.7 };
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glNormal3f(0, 1, 0);
	glVertex3f(vFT1[0], vFT1[1], vFT1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vFT2[0], vFT2[1], vFT2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vFT3[0], vFT3[1], vFT3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vFT4[0], vFT4[1], vFT4[2]);
	glEnd();

	GLfloat vFL1[3] = { 0.7,-0.7,-0.7 };
	GLfloat vFL2[3] = { 0.7,0.7,-0.7 };
	GLfloat vFL3[3] = { 0.7,0.7,0.7 };
	GLfloat vFL4[3] = { 0.7,-0.7,0.7 };
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glNormal3f(1, 0, 0);
	glVertex3f(vFL1[0], vFL1[1], vFL1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vFL2[0], vFL2[1], vFL2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vFL3[0], vFL3[1], vFL3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vFL4[0], vFL4[1], vFL4[2]);
	glEnd();

	GLfloat vFR1[3] = { -0.7,-0.7,-0.7 };
	GLfloat vFR2[3] = { -0.7,0.7,-0.7 };
	GLfloat vFR3[3] = { -0.7,0.7,0.7 };
	GLfloat vFR4[3] = { -0.7,-0.7,0.7 };
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 1.0);
	glNormal3f(-1, 0, 0);
	glVertex3f(vFR1[0], vFR1[1], vFR1[2]);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(vFR2[0], vFR2[1], vFR2[2]);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(vFR3[0], vFR3[1], vFR3[2]);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(vFR4[0], vFR4[1], vFR4[2]);
	glEnd();
	glPopMatrix();

	draw_obj(body, 'B');

	glPushMatrix();
	drawArm(false, true, (*i).armAngle);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, 1, 0);
	glPushMatrix();
	glRotatef((*i).footAngle,1,0,0);
	glTranslatef(0, -1, 0);
	draw_obj(lfoot, 'B');
	glPopMatrix();
	glPushMatrix();
	glRotatef(-(*i).footAngle, 1, 0, 0);
	glTranslatef(0, -1, 0);
	draw_obj(rfoot, 'B');
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawBullet(Object* i) {
	glPushMatrix();
	GLfloat ambient[] = { 0.84, 0.84, 0, 0.0 };
	GLfloat diffuse[] = { 0.84, 0.84, 0,1.0 };
	GLfloat specular[] = { 0.84, 0.84, 0,1.0 };
	GLfloat shininess[] = { 25 };	//0~128f
	setMaterialfv(ambient, diffuse, specular, shininess);
	glTranslatef((*i).x, (*i).y, (*i).z);
	glRotatef((*i).angle, 0, 1, 0);
	draw_obj(bullet, 'b');
	glPopMatrix();
}

void drawItem(Object* i) {
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	GLfloat ambient[] = { 0.85, 0.26, 0.33, 0.0 };
	GLfloat diffuse[] = { 1, 1, 1,1.0 };
	GLfloat specular[] = { 1, 1, 1,1.0 };
	GLfloat shininess[] = { 25 };	//0~128f
	setMaterialfv(ambient, diffuse, specular, shininess);
	glTranslatef((*i).x, (*i).y, (*i).z);
	draw_obj(item,' ');
	glPopMatrix();
	glEnable(GL_TEXTURE_2D);
}

void drawClear() {
	glPushMatrix();
	glTranslatef(camx + lx*2, camy + ly*2, camz + lz*2);
	draw_obj(clear, ' ');
	glPopMatrix();
}

void set_light() {
	GLfloat light_global_ambient[] = { 0.5,0.5,0.5,1.0 };
	GLfloat ambientLight0[] = { 1, 1, 1, 1.0f };   // 주변광
	GLfloat diffuseLight0[] = { 1.0f, 1.0f, 1.0f, 1.0f };   // 확산광
	GLfloat specular0[] = { 1.0f, 1.0f, 1.0f, 1.0f };       // 반사광
	lightPos0[0] = 20 - lightPos0_tmpx;
	lightPos0[1] = 94.0f - lightPos0_tmpy;
	lightPos0[2] = 168 - lightPos0_tmpz;
	lightPos0[3] = 1.0f;			// 위치

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_global_ambient);	//전역주변광 설정
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight0);            // <5>
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);            // <6>
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);				// <6>
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);               // <7>


	glDisable(GL_COLOR_MATERIAL);								//조명이 있어도 색상을 유지못하게
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_NORMALIZE);										//법선벡터의 크기가 1이되도록 정규화
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);		//행렬모드 모델뷰 설정
	glLoadIdentity();				//해당 모드의 행렬을 단위행렬로 초기화

	gluLookAt(camx, camy, camz,		//캠위치
		camx + lx, camy + ly, camz + lz,		//타겟위치
		cam_upx, cam_upy, cam_upz);

	set_light();												//조명설정
	GLfloat ambient[] = { 0.32, 0.33, 0.34, 0.0 };
	GLfloat diffuse[] = { 1, 1, 1,1.0 };
	GLfloat specular[] = { 1, 1, 1,1.0 };
	GLfloat shininess[] = { 25 };	//0~128f
	setMaterialfv(ambient, diffuse, specular, shininess);


	drawWorld();



	for (list<Object*>::iterator i = objList.begin(); i != objList.end(); i++) {
		if ((*i)->name[0] == 'u') {
			drawUser();
		}
		else if ((*i)->name[0] == 'z') {
			drawZombie((*i));
		}
		else if ((*i)->name[0] == 'b') {
			drawBullet((*i));
		}
		else if ((*i)->name[0] == 'i') {
			drawItem((*i));
		}
	}

	if (zombieCount == 0) {
		drawClear();
	}

	glutSwapBuffers();
}

void MyTimer(int value) {
	glutTimerFunc(20, MyTimer, 1);
	glutPostRedisplay();
}

void MyKeyBoard(unsigned char KeyPressed, int x, int y) {
GLfloat tmpx = camx, tmpy = camy, tmpz = camz;
switch (KeyPressed) {
case 'a': tmpx += speed * sin(3.14 / 2 - yaw); tmpz -= speed * cos(3.14 / 2 - yaw); break;	//횡이동은 90 - 각도
case 'w': tmpx += speed * lx; tmpz += speed * lz; break;
case 'd': tmpx -= speed * sin(3.14 / 2 - yaw); tmpz += speed * cos(3.14 / 2 - yaw); break;
case 's': tmpx -= speed * lx; tmpz -= speed * lz; break;
case 'z': tmpy = camy - 0.3; break;
case 'x': tmpy = camy + 0.3; break;
case 'o': lightPos0_tmpy -= 1; break;
case 'l': lightPos0_tmpy += 1; break;
case 'i': lightPos0_tmpx -= 1; break;
case 'p': lightPos0_tmpx += 1; break;
case ',': lightPos0_tmpz -= 1; break;
case '.': lightPos0_tmpz += 1; break;
}
user->x = tmpx;
user->y = tmpy;
user->z = tmpz;
user->setColiisionBorder(tmpx - 1, tmpx + 1, tmpy - 1, tmpy + 1, tmpz - 1, tmpz + 1);
Object* result = isCollide(user);
if (result != NULL) {
	if (result->name[0] == 'z' || result->name[0] == 'b'|| result->name[0] == 'i') {
		camx = tmpx;
		camy = tmpy;
		camz = tmpz;
		if (result->name[0] == 'i') {
			objList.remove(result);
			ammo += 10;
		}
	}
	else {
		user->x = camx;
		user->y = camy;
		user->z = camz;
		user->setColiisionBorder(camx - 1, camx + 1, camy - 1, camy + 1, camz - 1, camz + 1);
	}
}
else {
	camx = tmpx;
	camy = tmpy;
	camz = tmpz;
}

glutPostRedisplay();
}

void MyMouse(int xpos, int ypos) {
	int state = glutGetModifiers();				//오일러각 사용. opengl은 카메라가 글로벌 좌표이므로 힘들었음. 
	float xOffset = lastMouseX - xpos;			//타겟좌표 = gluLookat의 방향벡터를 카메라위치에 더해줘야함
	float yOffset = lastMouseY - ypos;
	lastMouseX = xpos;
	lastMouseY = ypos;

	xOffset *= eyeSensitivity;
	yOffset *= eyeSensitivity;

	yaw += xOffset;
	pitch += yOffset;


	lx = sin(yaw);
	ly = pitch;
	lz = cos(yaw);

	glutPostRedisplay();
}

void MyMouse2(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {

			if (ammo > 0) {
				ammo -= 1;
				FMOD_System_PlaySound(System, Sound[1], NULL, false, &Channel[1]);

				GLfloat tmpx = 0;
				GLfloat tmpy = camy - 0.3;
				GLfloat tmpz = 0;
				tmpx = camx + lx;
				tmpz = camz + lz * 1.2;
				Projectile* bullet1 = new Projectile("b1", tmpx, tmpy, tmpz, tmpx - bulletBorder, tmpx + bulletBorder, tmpy - bulletBorder, tmpy + bulletBorder, tmpz - bulletBorder, tmpz + bulletBorder);
				bullet1->angle = 61.3 * yaw;
				bullet1->time = IdleTime;
				objList.push_back(bullet1);
				user->isButtonClicked = false;
			}
			else {
				FMOD_System_PlaySound(System, Sound[2], NULL, false, &Channel[2]);
			}
		}
		else if (state == GLUT_UP) {
		}
	}
}

void stage1() {
	zombieCount = 15;

	Zombie* zombie = new Zombie[zombieCount];
	for (int i = 0; i < zombieCount; i++) {
		int x = rand() % 35;
		int z = rand() % 100;
		zombie[i] = Zombie("z1", x, 0, z, 0, 1, x, x + 1, z, z + 1);
		zombie[i].walking = true;
		objList.push_back(&zombie[i]);
	}
}

inline void MyIdle(int value) {
	IdleTime += 1;
	for (list<Object*>::iterator i = objList.begin(); i != objList.end(); i++) {
		if ((*i)->name[0] == 'z') {
			if(!(*i)->walking){
				(*i)->armAngle += 20 * (*i)->armDirection;
				if ((*i)->armAngle < -70 || (*i)->armAngle>=0) {
					(*i)->armDirection *= -1;
				}
				if ((*i)->armAngle == 0) {
					(*i)->walking = true;
					user->hp -= 5;
					if (user->hp < 0) {
						cout << "죽었습니다.\n";
						exit(0);
					}
				}
			}
			else {
				if (sqrt(pow(user->x - (*i)->x, 2) + pow(user->z - (*i)->z, 2)) < 14) {
					(*i)->walking = false;
					(*i)->footAngle = 0;
				}
				else {
					(*i)->footAngle += 20 * (*i)->footDirection;
					if ((*i)->footAngle < -50 || (*i)->footAngle>= 50) {
						(*i)->footDirection *= -1;
					}
					GLfloat tmp_vector[2];	//좀비의 벡터
					GLfloat target_vector[2];	//유저로의 벡터
							//각 = acos((axbx+ayby+azbz)/sqrt(ax^+ay^+az^)/sqrt(bx^+by^+bz^))
					tmp_vector[0] = (*i)->vector_x, tmp_vector[1] = (*i)->vector_y, tmp_vector[1] = (*i)->vector_z;
					target_vector[0] = (*user).x - (*i)->x; target_vector[1] = (*user).z - (*i)->z;

					double rVal = 0;
					//유저방향으로 회전
					for (int i = 0; i < 2; i++) {
						rVal += tmp_vector[i] * target_vector[i];
					}
					double tmp = 0;
					for (int i = 0; i < 2; i++) {
						tmp += pow(tmp_vector[i], 2);
					}
					rVal /= sqrt(tmp);

					tmp = 0;
					for (int i = 0; i < 2; i++) {
						tmp += pow(target_vector[i], 2);
					}
					rVal /= sqrt(tmp);

					double a = acos(rVal); //라디안을 도로 변환
					a = (int(a * 180 / 3.14) + 360) % 360;
					(*i)->angle = a;
									if ((*i)->x < user->x) {
						(*i)->x = (*i)->x + (sin(a)) * zombieSpeed;
						(*i)->z = (*i)->z + (cos(a)) * zombieSpeed;
					}
					else {
						(*i)->x = (*i)->x - (sin(a)) * zombieSpeed;
						(*i)->z = (*i)->z + (cos(a)) * zombieSpeed;
					}
					(*i)->setColiisionBorder((*i)->x - 1, (*i)->x + 1, (*i)->minY, (*i)->maxY, (*i)->z - 1, (*i)->z + 1);
				}
			}
		}
		else if ((*i)->name[0] == 'b') {
			(*i)->x = (*i)->x + sin((*i)->angle / 180 * 3.14) * bulletSpeed;
			(*i)->z = (*i)->z + cos((*i)->angle / 180 * 3.14) * bulletSpeed;
			(*i)->setColiisionBorder((*i)->x - bulletBorder, (*i)->x + bulletBorder, (*i)->y - 1, (*i)->y + 1, (*i)->z - bulletBorder, (*i)->z + bulletBorder);
			Object* result = isCollide(*i);
			if (result != NULL) {
				if (result->name[0] == 'w') {
					objList.erase(i--);
				}
				else if (result->name[0] == 'z') {
					objList.erase(i--);
					result->hp -= 30;
					if (result->hp < 0) {
						--i;
						objList.remove(result);
						int random = rand() % 100;
						if (random < 40) {
							Object* item = new Object("item", result->x, 0, result->z, result->minX, result->maxX, 1, 2, result->minZ, result->maxZ);//아이템생성
							objList.push_back(item);
						}
						zombieCount -= 1;
					}
				}
			}
		}
	}
	glutTimerFunc(10, MyIdle, 1); // 10msec 
	glutPostRedisplay();
}

void init_light() {					//가시성 제어

	glEnable(GL_LIGHTING);                                     // <4>
	glShadeModel(GL_SMOOTH);		//폴리곤을 따라서 정점색을 보간해나가며 채운다.
	glEnable(GL_DEPTH_TEST);		//은면제거(보이지않는면 표시안함), 깊이버퍼 활성화

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	//glEnable(GL_CLIP_PLANE0);
	//glEnable(GL_CLIP_PLANE1);
}

int main(int argc, char** argv) {
	FMOD_System_Create(&System);
	FMOD_System_Init(System, 32, FMOD_INIT_NORMAL, NULL);
	FMOD_Channel_SetVolume(Channel[0], (float)0.8);

	FMOD_System_CreateSound(System, "./sound/bgm.wav", FMOD_LOOP_NORMAL, 0, &Sound[0]);
	FMOD_System_CreateSound(System, "./sound/gun.wav", FMOD_DEFAULT, 0, &Sound[1]);
	FMOD_System_CreateSound(System, "./sound/empty.mp3", FMOD_DEFAULT, 0, &Sound[2]);
	FMOD_System_PlaySound(System, Sound[0], NULL, false, &Channel[0]);


	image[0] = imread("./img/brick.jpg", IMREAD_COLOR);
	cvtColor(image[0], image[0], COLOR_BGR2RGB);

	image[1] = imread("./img/metal2.jpg", IMREAD_COLOR);
	cvtColor(image[1], image[1], COLOR_BGR2RGB);

	image[2] = imread("./img/night.jpg", IMREAD_COLOR);
	cvtColor(image[2], image[2], COLOR_BGR2RGB);

	image[3] = imread("./img/zombieFF.jpg", IMREAD_COLOR);
	cvtColor(image[3], image[3], COLOR_BGR2RGB);

	image[4] = imread("./img/zombieFLR.jpg", IMREAD_COLOR);
	cvtColor(image[4], image[4], COLOR_BGR2RGB);

	image[5] = imread("./img/zombieBody.jpg", IMREAD_COLOR);
	cvtColor(image[5], image[5], COLOR_BGR2RGB);

	image[6] = imread("./img/bullet.jpg", IMREAD_COLOR);
	cvtColor(image[6], image[6], COLOR_BGR2RGB);

	image[7] = imread("./img/wood.jpg", IMREAD_COLOR);
	cvtColor(image[7], image[7], COLOR_BGR2RGB);

	image[8] = imread("./img/metal3.jpg", IMREAD_COLOR);
	cvtColor(image[8], image[8], COLOR_BGR2RGB);

	Floor* floor = new Floor("floor", 0, 0, 0, minMapX, maxMapX, 0, 0, minMapZ, maxMapZ);
	//Ceil* ceil = new Ceil();
	Wall* wall1 = new Wall("wall1", minMapX, 2.5, minMapZ + 2 * mapDistanceZ, minMapX, minMapX, 0, 5, minMapZ, maxMapZ);	//좌측벽
	Wall* wall2 = new Wall("wall2", maxMapX, 2.5, minMapZ + 2 * mapDistanceZ, maxMapX, maxMapX, 0, 5, minMapZ, maxMapZ);	//우
	Wall* wall3 = new Wall("wall3", minMapX + 2 * mapDistanceX, 2.5, maxMapZ, minMapX, maxMapX, 0, 5, maxMapZ, maxMapZ);	//하
	Wall* wall4 = new Wall("wall4", minMapX + 2 * mapDistanceX, 2.5, minMapZ, minMapX, maxMapX, 0, 5, minMapZ, minMapZ);	//상

	user = new User("u", camx, camy, camz, camx - 1, camx + 1, camy - 1, camy + 1, camz - 1, camz + 1);

	obj_read();

	objList.push_back(user);
	objList.push_back(wall1);
	objList.push_back(wall2);
	objList.push_back(wall3);
	objList.push_back(wall4);
	objList.push_back(floor);

	stage1();




	glutInit(&argc, argv);										//glut 초기화
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);				//디스플레이 설정 기본색상과 원할한 애니메이션을위해 더블버퍼사용
	glutInitWindowSize(_WINDOW_WIDTH, _WINDOW_HEIGHT);			//창 크기 결정
	glutCreateWindow("BoxHead 3D");								//창 생성
	init_light();
	init_texture();

	glutDisplayFunc(display);									//그리기함수
	glutReshapeFunc(reshape);									//reshape함수
	glutKeyboardFunc(MyKeyBoard);								//키보드 이벤트리스너
	glutPassiveMotionFunc(MyMouse);								//마우스 움직임이벤트
	glutMouseFunc(MyMouse2);									//마우스 클릭이벤트
	
	glutTimerFunc(19, MyIdle, 1);								//타이머
	//init_texture(image.data, image.cols, image.rows);

	glutMainLoop();												//메인루프 진입
	return 0;
}