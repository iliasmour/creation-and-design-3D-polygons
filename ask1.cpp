#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <list> 
#include <iostream>
#include <assert.h>
#include "triangulate.h"


using namespace std; 
const int MAX_POINTS = 20; 
#define POLYGON 1
#define CLIPPING 2
#define EXTRUDE 3
#define MOVE 4
#define EXIT 5
//LINE_COLOR & FILL_COLOR
#define WHITE 1          //glColor3f(1, 1, 1);
#define BLACK 2         //glColor3f(0, 0, 0);
#define RED 3             //glColor3f(1, 0, 0);
#define DARK_GREEN 4  //glColor3f(0, 0.5, 0);
#define LIGHT_GREEN 5 //glColor3f(0, 1, 0);
#define BLUE 6     //glColor3f(0, 0, 1);
#define AQUA 7   //glColor3f(0, 1, 1);
#define PINK 8     //gcColor3f(1, 0, 1);
#define SOFT_PINK 9 //glColor3f(1, 0.8, 0.9);
#define PURPLE 10  //glColor3f(0.5, 0, 1);
#define BROWN 11   //glColor3f(0.5, 0.2, 0);
#define YELLOW 12  //glColor3f(1, 1, 0);
#define GRAY 13    //glColor3f(0.5, 0.5, 0.5);
#define ORANGE 14  //glColor3f(1, 0.5, 0);
#define GOLD 15  //glColor3f(1, 0.85, 0);
#define BEIGE 16 //glColor3f(0.95, 0.95, 0.85);
class Point{              
  int Xvalue, Yvalue;

  public:
  void xy(int x, int y){
    Xvalue = x;
    Yvalue = y;
  }
 
  //return individual x y value
  int x() { return Xvalue; }
  int y() { return Yvalue; }
  //float t() { return Tvalue; }
};


class Polygon{ 
	list <Point> points;
	int isFinished=0;
	
	public:
	void addPoint(Point point1){
		points.push_back(point1);
	}
	public:
	void addPointfront(Point point1){
		points.push_front(point1);
	}

	Point getPoint(){
		Point front;
		front=points.front();
		points.pop_front();
		return front;
	}

	Point getPointBack(){
		Point back;
		back=points.back();
		points.pop_back();
		return back;
	}

	int getSize(){
		points.size();
	}
	
	void setClose(){ //is polygon finished?
		isFinished=1;
	}

	int getClose(){
		return isFinished;
	}
	
};

//-------------------------------------------------------global metavlites--------------------------------------------------------
double w ,h; //width , height of the window
list <Polygon> polygons; //lista apo polygons
list <Vector2dVector> results;
int openPolygon=0; //0 dn iparxei,1 iparxei
int exitpolygonmode=0;
float colorLineArray[3]={0,0,0}; // stores r,g,b colors for line color.
float colorFillArray[3]={1,1,1};
float zi[20]={0};
int linecolorcheck=0;
int fillcolorcheck=0;
int tpressed=0;
int clipping_pressed=0;
int counterT=0;
int xmin = 0,ymin = 0,xmax = 0,ymax = 0;
int enter = 1,sz,st_flag=1;
int flagc=0;
int flag=0;
int extrude_flag=0;
int icheck[20]={-1};
int ry=750;
int movepressed=0;
int extrude_polygon=-1;
float z=0;
// angle of rotation for the camera direction
float angle=0.0;
// actual vector representing the camera's direction
float lx=0.0f,lz=-1.0f;
// XZ position of the camera
float x=0.0f,z2=5.0f;
//-----------------------------------------------------------kodikas -------------------------------------------------------------
void resize(int width, int height) {
	// we ignore the params and do:
	glutReshapeWindow( 600, 500);
}

void drawPoints(){

	int i,j;
	Polygon polygon1;	
	Point point1;
	glColor3f(colorLineArray[0],colorLineArray[1],colorLineArray[2]);
	glPointSize(1.0f);
	glBegin(GL_POINTS);
	for(i=0;i<polygons.size();i++){
		polygon1=polygons.front(); //pairno to poligono
		polygons.pop_front();

		for(j=0;j<polygon1.getSize();j++){
			point1=polygon1.getPoint();
			glVertex3f(point1.x(),ry-point1.y(),0);
			polygon1.addPoint(point1);
		}
		polygons.push_back(polygon1); //to ksanavazo mesa
	}
	glEnd();
	glFlush();	
}

void drawLines(){
	int i,j;
	Polygon polygon1;	
	Point point1,point2;
	glLineWidth(3);
	glColor3f(colorLineArray[0],colorLineArray[1],colorLineArray[2]); 
	
	if(polygons.size()!=0){
		if(polygons.front().getSize()>=2){
			glBegin(GL_LINES);
			for(i=0;i<polygons.size();i++){
				polygon1=polygons.front(); //pairno to poligono
				polygons.pop_front();
				
				if(polygon1.getSize()>=2){
					
					point1=polygon1.getPoint();
					for(j=0;j<polygon1.getSize();j++){
						point2=polygon1.getPoint();
						
						glVertex3f(point1.x(),ry-point1.y(),0);
						glVertex3f(point2.x(),ry-point2.y(),0);
						
						polygon1.addPoint(point1);
						point1=point2;			
					}
					if (polygon1.getClose()==1){
						point2=polygon1.getPoint();
						glVertex3f(point1.x(),ry-point1.y(),0);
						glVertex3f(point2.x(),ry-point2.y(),0);
						polygon1.addPoint(point1);
						polygon1.addPoint(point2);
					}else{
						polygon1.addPoint(point1);
					}
					
				}
				polygons.push_back(polygon1); //to ksanavazo mesa
				
			}
			glEnd();
		}
	}
	glFlush();
}

void triangulation();

void triangulation(){
	int i ,j;
	Polygon polygon1;	
	Point point1;	
	for(i=0;i<polygons.size();i++){
		polygon1=polygons.front(); //pairno to poligono
		polygons.pop_front();
		if(polygon1.getSize()>=4){
			Vector2dVector a;			
			for(j=0;j<polygon1.getSize();j++){
				point1=polygon1.getPoint();
				a.push_back( Vector2d(point1.x(),point1.y()));
				polygon1.addPoint(point1);	
			}
			Vector2dVector result;
			Triangulate::Process(a,result);
			results.push_back(result);
		}
		polygons.push_back(polygon1); //to ksanavazo mesa
	}
	
	glutPostRedisplay(); 
}

void drawTriangle(){
	int i,j;
	Vector2dVector result1;	
	glLineWidth(3);
	glColor3f(0,1,0); 
	glBegin(GL_LINES);//TRIANGLES
	for(i=0;i<results.size();i++){
		result1=results.front(); 
		results.pop_front();		
		int tcount = result1.size()/3;
		for (int i=0; i<tcount; i++){		
			const Vector2d &p1 = result1[i*3+0];
			const Vector2d &p2 = result1[i*3+1];
			const Vector2d &p3 = result1[i*3+2];
			glVertex3f(p1.GetX(),ry-p1.GetY(),0);
			glVertex3f(p2.GetX(),ry-p2.GetY(),0);
			glVertex3f(p3.GetX(),ry-p3.GetY(),0);
			glVertex3f(p1.GetX(),ry-p1.GetY(),0);
		}
		results.push_back(result1);	
	}					
	glEnd();
	glFlush();
}
void draw3DTriangle(){
	int i,j;
	Vector2dVector result1;	
	glLineWidth(1);
	glColor3f(0,1,0); 
	glBegin(GL_LINES);//TRIANGLES
	for(i=0;i<results.size();i++){
		result1=results.front(); 
		results.pop_front();		
		int tcount = result1.size()/3;
		for (int i=0; i<tcount; i++){		
			const Vector2d &p1 = result1[i*3+0];
			const Vector2d &p2 = result1[i*3+1];
			const Vector2d &p3 = result1[i*3+2];
			glVertex3f(p1.GetX(),ry-p1.GetY(),zi[i]);
			glVertex3f(p2.GetX(),ry-p2.GetY(),zi[i]);
			glVertex3f(p3.GetX(),ry-p3.GetY(),zi[i]);
			glVertex3f(p1.GetX(),ry-p1.GetY(),zi[i]);
		}
		results.push_back(result1);	
	}					
	glEnd();
	glFlush();
}
 void fillPolygon(){
 	int i,j;
 	Vector2dVector result1;	
 	glColor3f(colorFillArray[0],colorFillArray[1],colorFillArray[2]); 
 	glPolygonMode(GL_FRONT,GL_FILL);
 	glBegin(GL_TRIANGLES);
 	for(i=0;i<results.size();i++){
 		result1=results.front(); 
 		results.pop_front();		
 		int tcount = result1.size()/3;
 		for (int i=0; i<tcount; i++){		
 			const Vector2d &p1 = result1[i*3+0];
 			const Vector2d &p2 = result1[i*3+1];
 			const Vector2d &p3 = result1[i*3+2];
 			glVertex3f(p1.GetX(),ry-p1.GetY(),0);
 			glVertex3f(p2.GetX(),ry-p2.GetY(),0);
 			glVertex3f(p3.GetX(),ry-p3.GetY(),0);
 		}
 		results.push_back(result1);	
 	}					
 	glEnd();
 	glFlush();
 }
 void fill3DPolygon(){
	glColor3f(colorFillArray[0],colorFillArray[1],colorFillArray[2]); 
	int i,j;
	Polygon polygon1;	
	Point point1;			
	for(i=0;i<polygons.size();i++){
		polygon1=polygons.front(); //pairno to poligono
		polygons.pop_front();	
		if(icheck[i]==i){
		glPolygonMode(GL_FRONT,GL_FILL);
		glBegin(GL_POLYGON);	
		if (polygon1.getClose()==1){	
			for(j=0;j<polygon1.getSize();j++){			
				point1=polygon1.getPoint();	
				glVertex3f(point1.x(),ry-point1.y(),zi[i]);
				polygon1.addPoint(point1);
			}	    		
		}

		glEnd();
		}
		polygons.push_back(polygon1); //to ksanavazo mesa	
	}	
	glFlush();		
}
// void fill3DPolygon(){
// 	int i,j;
//  	Vector2dVector result1;	
//  	glLineWidth(1);
//  	glColor3f(colorFillArray[0],colorFillArray[1],colorFillArray[2]); 
//  	glPolygonMode(GL_FRONT,GL_FILL);
//  	glBegin(GL_TRIANGLES);
//  	for(i=0;i<results.size();i++){
//  		result1=results.front(); 
//  		results.pop_front();	
// 		if(icheck[i]==i){	
// 			int tcount = result1.size()/3;
// 			for (int i=0; i<tcount; i++){		
// 				const Vector2d &p1 = result1[i*3+0];
// 				const Vector2d &p2 = result1[i*3+1];
// 				const Vector2d &p3 = result1[i*3+2];
// 				glVertex3f(p1.GetX(),ry-p1.GetY(),zi[i]);
// 				glVertex3f(p2.GetX(),ry-p2.GetY(),zi[i]);
// 				glVertex3f(p3.GetX(),ry-p3.GetY(),zi[i]);
// 			}
// 		}
//  		results.push_back(result1);	
//  	}					
//  	glEnd();
//  	glFlush();
//  }
void fill3DSides(){
	glColor3f(colorLineArray[0],colorLineArray[1],colorLineArray[2]); 
	int i,j;
	Polygon polygon1;	
	Point point1,point2;			
	for(i=0;i<polygons.size();i++){
		polygon1=polygons.front(); //pairno to poligono
		polygons.pop_front();	
		if(icheck[i]==i){
			
		if (polygon1.getClose()==1){
			glPolygonMode(GL_FRONT,GL_FILL);
			glBegin(GL_QUADS);
			for(j=0;j<polygon1.getSize();j++){
			
				point1=polygon1.getPoint();
		    	glVertex3f(point1.x(),ry-point1.y(),0);
				glVertex3f(point1.x(),ry-point1.y(),zi[i]);
				polygon1.addPoint(point1);
			}	 
			for(j=0;j<polygon1.getSize();j++){
			
				point1=polygon1.getPoint();
		    	glVertex3f(point1.x(),ry-point1.y(),zi[i]);
				glVertex3f(point1.x(),ry-point1.y(),0);
				polygon1.addPoint(point1);
			}	 
		glEnd();

		}		
		}
		polygons.push_back(polygon1); //to ksanavazo mesa	
	}	
	glFlush();		
}
void processLineColorMenuEvents(int option){
  
  if(linecolorcheck==0){
    linecolorcheck=1;
    switch(option){
 
      case WHITE:
        colorLineArray[0]=1;
        colorLineArray[1]=1;
        colorLineArray[2]=1;
        break;
 
      case BLACK:
        colorLineArray[0]=0;
        colorLineArray[1]=0;
        colorLineArray[2]=0;
        break;
 
      case RED:
        colorLineArray[0]=1;
        colorLineArray[1]=0;
        colorLineArray[2]=0;
        break;
 
      case DARK_GREEN:
        colorLineArray[0]=0;
        colorLineArray[1]=0.5;
        colorLineArray[2]=0;
        break;
 
      case LIGHT_GREEN:
        colorLineArray[0]=0;
        colorLineArray[1]=1;
        colorLineArray[2]=0;
        break;
 
      case BLUE:
        colorLineArray[0]=0;
        colorLineArray[1]=0;
        colorLineArray[2]=1;
        break;
 
      case AQUA:
        colorLineArray[0]=0;
        colorLineArray[1]=1;
        colorLineArray[2]=1;
        break;
 
      case PINK:
        colorLineArray[0]=1;
        colorLineArray[1]=0;
        colorLineArray[2]=1;
        break;
      
      case SOFT_PINK:
        colorLineArray[0]=1;
        colorLineArray[1]=0.8;
        colorLineArray[2]=0.9;
        break;
 
      case PURPLE:
        colorLineArray[0]=0.5;
        colorLineArray[1]=0;
        colorLineArray[2]=1;
        break;

      case BROWN:
        colorLineArray[0]=0.5;
        colorLineArray[1]=0.2;
        colorLineArray[2]=0;
        break;
 
      case YELLOW:
        colorLineArray[0]=1;
        colorLineArray[1]=1;
        colorLineArray[2]=0;
        break;
 
      case GRAY:
        colorLineArray[0]=0.5;
        colorLineArray[1]=0.5;
        colorLineArray[2]=0.5;
        break;
 
      case ORANGE:
        colorLineArray[0]=1;
        colorLineArray[1]=0.5;
        colorLineArray[2]=0;
        break;

      case GOLD:
        colorLineArray[0]=1;
        colorLineArray[1]=0.85;
        colorLineArray[2]=0;
        break;
 
      case BEIGE:
        colorLineArray[0]=0.95;
        colorLineArray[1]=0.95;
        colorLineArray[2]=0.85;
        break;
    }
  }
}
void processFillColorMenuEvents(int option){
 
  if(fillcolorcheck==0){
    fillcolorcheck=1;
    
    switch(option){
    
    case WHITE:
      colorFillArray[0]=1;
      colorFillArray[1]=1;
      colorFillArray[2]=1;
      break;
 
    case BLACK:
      colorFillArray[0]=0;
      colorFillArray[1]=0;
      colorFillArray[2]=0;
      break;
 
    case RED:
      colorFillArray[0]=1;
      colorFillArray[1]=0;
      colorFillArray[2]=0;
      break;
 
    case DARK_GREEN:
      colorFillArray[0]=0;
      colorFillArray[1]=0.5;
      colorFillArray[2]=0;
      break;
 
    case LIGHT_GREEN:
      colorFillArray[0]=0;
      colorFillArray[1]=1;
      colorFillArray[2]=0;
      break;
 
    case BLUE:
      colorFillArray[0]=0;
      colorFillArray[1]=0;
      colorFillArray[2]=1;
      break;
 
    case AQUA:
      colorFillArray[0]=0;
      colorFillArray[1]=1;
      colorFillArray[2]=1;
      break;

    case PINK:
      colorFillArray[0]=1;
      colorFillArray[1]=0;
      colorFillArray[2]=1;
      break;
 
    case SOFT_PINK:
      colorFillArray[0]=1;
      colorFillArray[1]=0.8;
      colorFillArray[2]=0.9;
      break;
 
    case PURPLE:
      colorFillArray[0]=0.5;
      colorFillArray[1]=0;
      colorFillArray[2]=1;
      break;

    case BROWN:
      colorFillArray[0]=0.5;
      colorFillArray[1]=0.2;
      colorFillArray[2]=0;
      break;
 
    case YELLOW:
      colorFillArray[0]=1;
      colorFillArray[1]=1;
      colorFillArray[2]=0;
      break;

    case GRAY:
      colorFillArray[0]=0.5;
      colorFillArray[1]=0.5;
      colorFillArray[2]=0.5;
      break;
 
    case ORANGE:
      colorFillArray[0]=1;
      colorFillArray[1]=0.5;
      colorFillArray[2]=0;
      break;
 
    case GOLD:
      colorFillArray[0]=1;
      colorFillArray[1]=0.85;
      colorFillArray[2]=0;
      break;
 
    case BEIGE:
      colorFillArray[0]=0.95;
      colorFillArray[1]=0.95;
      colorFillArray[2]=0.85;
      break;
    }
  }
}

bool isIntersecting(int x1,int y1,int x2, int y2,int x3,int y3,int x4, int y4){
	
	return (((x3-x1)*(y2-y1)-(y3-y1)*(x2-x1))*((x4-x1)*(y2-y1)-(y4-y1)*(x2-x1))<0)&&
		(((x1-x3)*(y4-y3)-(y1-y3)*(x4-x3))*((x2-x3)*(y4-y3)-(y2-y3)*(x4-x3))<0);
	
}
void draw3dPolygon(){
	int i,j;
	Polygon polygon1;	
	Point point1,point2;
	glLineWidth(1);
	glColor3f(colorLineArray[0],colorLineArray[1],colorLineArray[2]); 
	
	if(polygons.size()!=0){
		if(polygons.front().getSize()>=2){
			glBegin(GL_LINES);
			for(i=0;i<polygons.size();i++){
				polygon1=polygons.front(); //pairno to poligono
				polygons.pop_front();
				if(icheck[i]==i){
				if(polygon1.getSize()>=2){
					
					point1=polygon1.getPoint();
					for(j=0;j<polygon1.getSize();j++){
						point2=polygon1.getPoint();
						
						glVertex3f(point1.x(),ry-point1.y(),zi[i]);
						glVertex3f(point1.x(),ry-point1.y(),0);
						glVertex3f(point2.x(),ry-point2.y(),zi[i]);
						glVertex3f(point2.x(),ry-point2.y(),0);
						glVertex3f(point1.x(),ry-point1.y(),zi[i]);
						glVertex3f(point2.x(),ry-point2.y(),zi[i]);
						polygon1.addPoint(point1);
						point1=point2;			
					}
						polygon1.addPoint(point1);
					
					
				}
				}
				polygons.push_back(polygon1); //to ksanavazo mesa
				
			}
			glEnd();
		}
	}
	glFlush();	
}
void drawQuad(){
	glColor3f(0,0,0);
	glLineWidth(1.0);

	glBegin(GL_LINE_LOOP);
        glVertex3f(xmin,ymin,1);
        glVertex3f(xmin,ymax,1);
        glVertex3f(xmax,ymax,1);
        glVertex3f(xmax,ymin,1);
	glEnd();
		
	glFlush();
   
}
void drag_start(int x, int y){
    y = ry-y;
    if(enter==1&&st_flag){
        xmin = x;
        ymin = y;
        st_flag = 0;
    }
    else{
        xmax = x;
        ymax = y;
    }
    drawQuad();
}

void drag_end(int x, int y){
    y = ry-y;
    if(enter==1&&st_flag==0){
        xmax = x;
        ymax = y;
        st_flag = 1;
        drawQuad();
    }
}
int searchXY(int x,int y){// returns the position of a point with coordinates near x,y
	Polygon polygon1;	
	Point point1;
	int i,j;
	int flag=-1;
	int flag1=-1;
    for(i=0;i<polygons.size();i++){
		polygon1=polygons.front(); //pairno to poligono
		polygons.pop_front();
		for(j=0;j<polygon1.getSize();j++){
			point1=polygon1.getPoint();
			if( (point1.x()>x-10 && point1.x()<x+10)  &&  (point1.y()>y-10 && point1.y()<y+10) && flag1==-1){
				icheck[i]=i;
				flag=j;
				flag1=1;
				printf("Please give a number between 0.0 - 1.0  for the length of extruction \n");
		        scanf("%f" , &zi[i]);
			}
			polygon1.addPoint(point1);	
		}
		polygons.push_back(polygon1);	  
	
	}
	return flag;  // if there isn't a point nearby return -1
}
int movepoint; //the position of the point in the array that will be moved
int valid;
void dragPoint(int x,int y){
	Polygon polygon1;	
	Point point1;
	int flag=0;
  if(movepoint!=-1 && valid==1){ // prevent the user from dragging a point outside the window
    if(x<0 ){
      x=0;
    }
    if(x>600){
       x=600;
    }
    if(y<0){
      y=0;
    }
    if(y>500){
      y=500;
    }
	for(int i=0;i<polygons.size();i++){
		polygon1=polygons.front(); //pairno to poligono
		polygons.pop_front();
		if(icheck[i]==i){
			for(int j=0;j<polygon1.getSize();j++){
				point1=polygon1.getPoint();
				if(movepoint==j){
					point1.xy(x,y); //update x,y after mov
					polygon1.addPoint(point1);
					flag=1;
					movepoint=-1;
					icheck[i]=-1;
				}else{
					polygon1.addPoint(point1);				
				}
				
			} 
			//polygons.push_back(polygon1);
		}
		polygons.push_back(polygon1);
		if(flag==1){break;}
		
	}    
    glutPostRedisplay();
  }
}

void mouseMove(int button, int state, int x, int y){

  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
    
	if(movepressed==1){
		valid=1;
		movepoint=searchXY(x,y);   //finds a point near user's click
		printf("%d\n", movepoint);
		glutMotionFunc(dragPoint);
		
	}
	else if(clipping_pressed==1){
		glutMotionFunc(drag_start);
		glutPassiveMotionFunc(drag_end);
    
  }
	else if(extrude_flag==1){
		extrude_polygon=searchXY(x,y);
	}
	else{
		valid=0;
	}
  }
}
void drawQuad();
void display ( void )
{		
	glViewport(0, 0, w, h);
	glLoadIdentity();
	// Set the camera
	 gluLookAt(	x, 1.0f, z2,
	 		x+lx, 1.0f,  z2+lz,
			0.0f, 1.0f,  0.0f); 

	glClearColor(1.0, 1.0, 1.0, 0.0); // sets the backgraound color to white
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //reset window

	glMatrixMode(GL_PROJECTION);
	
	gluOrtho2D(0.0, w, 0.0, h);
	
	drawPoints();
	triangulation();
	fillPolygon();
	drawLines();
	if(tpressed==1){
		drawTriangle();

	}
	if(movepressed==0){
		drawQuad();
	}
	
	 if(extrude_flag==1){
		fill3DPolygon();
		fill3DSides();
	 	draw3dPolygon();
		//draw3DTriangle();
	 }
	
	glFlush ();
	glutSwapBuffers();
}

void createPolygon(int button, int state, int x, int y){
 		
	Polygon polygon1;
	Point point1,point2,point3,point4;

	int isIntersect =0;
	glutDetachMenu(GLUT_RIGHT_BUTTON);
	if(exitpolygonmode==1){ //polygon is finished return
		glutPostRedisplay();
		glutAttachMenu(GLUT_RIGHT_BUTTON);
		return;
	}
	

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){ // left click to draw new points
		if (openPolygon==0){
			
			point1.xy(x, y);
			polygon1.addPoint(point1);
			polygons.push_back(polygon1);
			openPolygon=1;
		}else{
			
			point1.xy(x, y);
			polygon1=polygons.back();
 			polygons.pop_back();
			if(polygon1.getSize()>=3){
				point2=polygon1.getPointBack();
				point3=polygon1.getPoint();
				isIntersect =0;
				
				for(int i=0;i<polygon1.getSize();i++){
					point4=polygon1.getPoint();
					if(isIntersecting(point1.x(),point1.y(),point2.x(),point2.y(),
					point3.x(),point3.y(),point4.x(),point4.y())){
						isIntersect=1;
					}
				
					polygon1.addPoint(point3);
					point3=point4;
				}
				
				
				polygon1.addPoint(point4);
				polygon1.addPoint(point2);
				

				if(isIntersect==0){
					polygon1.addPoint(point1);
				}
			}
			else{polygon1.addPoint(point1);}
			polygons.push_back(polygon1);
		}  
		
	}
 
	if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ){ //right click to finish drawing a polygon
		
		polygon1=polygons.back();
		polygons.pop_back();
		point1=polygon1.getPointBack();
		point2=polygon1.getPoint();
		point3=polygon1.getPoint();
		isIntersect =0;		
		for(int i=0;i<polygon1.getSize();i++){
			point4=polygon1.getPoint();
			if(isIntersecting(point1.x(),point1.y(),point2.x(),point2.y(),
				point3.x(),point3.y(),point4.x(),point4.y())){
				isIntersect=1;

			}	
			polygon1.addPoint(point3);
			point3=point4;
		}
			
		polygon1.addPointfront(point2);				
		polygon1.addPoint(point4);
		polygon1.addPoint(point1);
			

		if(isIntersect==0){
			polygon1.setClose();
			openPolygon=0;
			exitpolygonmode=1;
		}
		polygons.push_back(polygon1);
		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}

	glutPostRedisplay(); 
	
}

void processActionMenuEvents(int option) {
 
	switch(option){
		case EXIT:
		exit(0);

		case POLYGON:
		exitpolygonmode=0;
		glutMouseFunc(createPolygon);
     		break;
		case CLIPPING:
		//triangulation();
		clipping_pressed=1;
		movepressed=0; 
		glutMouseFunc(mouseMove);       
      		break;
		case EXTRUDE:
		clipping_pressed=0;	
		extrude_flag=1;
		glutMouseFunc(mouseMove);
			break;
		case MOVE:
		movepressed=1;
		clipping_pressed=0;	
		extrude_flag=1;
      	glutMouseFunc(mouseMove);       
      		break;
	}
}

void sendClippingPoints();

void key(unsigned char key, int x, int y){
    
	if(key=='C'){
		sendClippingPoints();		
		flagc=1;		
	}
	else if (key == 'T'&&counterT % 2 == 0){
		tpressed=1;
		counterT++;
		glutPostRedisplay();
	}
	else if(key == 'T'&&counterT % 2 == 1){
		tpressed=0;
		results.clear();
		counterT++;
	}	
}

int x_intersect(int x1, int y1, int x2, int y2, 
                int x3, int y3, int x4, int y4) 
{ 
    int num = (x1*y2 - y1*x2) * (x3-x4) - 
              (x1-x2) * (x3*y4 - y3*x4); 
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
    return num/den; 
} 
 
// Returns y-value of point of intersectipn of 
// two lines 
int y_intersect(int x1, int y1, int x2, int y2, 
                int x3, int y3, int x4, int y4) 
{ 
    int num = (x1*y2 - y1*x2) * (y3-y4) - 
              (y1-y2) * (x3*y4 - y3*x4); 
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4); 
    return num/den; 
} 
  
// This functions clips all the edges w.r.t one clip 
// edge of clipping area 
void clip(int poly_points[][2], int &poly_size, 
          int x1, int y1, int x2, int y2) 
{ 
    int new_points[MAX_POINTS][2], new_poly_size = 0; 
  
    // (ix,iy),(kx,ky) are the co-ordinate values of 
    // the points 
    for (int i = 0; i < poly_size; i++) 
    { 
        // i and k form a line in polygon 
        int k = (i+1) % poly_size; 
        int ix = poly_points[i][0], iy = poly_points[i][1]; 
        int kx = poly_points[k][0], ky = poly_points[k][1]; 
  
        // Calculating position of first point 
        // w.r.t. clipper line 
        int i_pos = (x2-x1) * (iy-y1) - (y2-y1) * (ix-x1); 
  
        // Calculating position of second point 
        // w.r.t. clipper line 
        int k_pos = (x2-x1) * (ky-y1) - (y2-y1) * (kx-x1); 
  
        // Case 1 : When both points are inside 
        if (i_pos < 0  && k_pos < 0) 
        { 
            //Only second point is added 
            new_points[new_poly_size][0] = kx; 
            new_points[new_poly_size][1] = ky; 
            new_poly_size++; 
        } 
  
        // Case 2: When only first point is outside 
        else if (i_pos >= 0  && k_pos < 0) 
        { 
            // Point of intersection with edge 
            // and the second point is added 
            new_points[new_poly_size][0] = x_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_points[new_poly_size][1] = y_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_poly_size++; 
  
            new_points[new_poly_size][0] = kx; 
            new_points[new_poly_size][1] = ky; 
            new_poly_size++; 
        } 
  
        // Case 3: When only second point is outside 
        else if (i_pos < 0  && k_pos >= 0) 
        { 
            //Only point of intersection with edge is added 
            new_points[new_poly_size][0] = x_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_points[new_poly_size][1] = y_intersect(x1, 
                              y1, x2, y2, ix, iy, kx, ky); 
            new_poly_size++; 
        } 
  
        // Case 4: When both points are outside 
        else
        { 
            //No points are added 
        } 
    } 
  
    // Copying new points into original list 
   
    poly_size = new_poly_size; 
    for (int i = 0; i < poly_size; i++) 
    { 
        poly_points[i][0] = new_points[i][0]; 
        poly_points[i][1] = new_points[i][1]; 
    } 
} 
  
// Implements Sutherland–Hodgman algorithm 
void suthHodgClip(int poly_points[][2], int poly_size, 
                  int clipper_points[][2], int clipper_size) 
{ 	
	
	Polygon polygon1;
	Point point1;
    //i and k are two consecutive indexes 
    for (int i=0; i<clipper_size; i++) 
    { 
        int k = (i+1) % clipper_size; 
  
        // We pass the current array of vertices, it's size 
        // and the end points of the selected clipper line 
        clip(poly_points, poly_size, clipper_points[i][0], 
             clipper_points[i][1], clipper_points[k][0], 
             clipper_points[k][1]); 
    }  
	polygon1=polygons.front(); //pairno to poligono
	polygons.pop_front();	
				
	for(int i=0;i<polygon1.getSize();i++){			
		point1=polygon1.getPoint();	
		
		point1.xy(poly_points[i][0],ry-poly_points[i][1]);
		polygon1.addPoint(point1);
	}	    		
			
	polygons.push_back(polygon1); //to ksanavazo mesa
       
} 
void sendClippingPoints(){
	int poly_size ; 
	int poly_points[20][2];
  	flag=0;
	int isIntersectClip =0; 
	int clipper_size = 4; 
	int clipper_points[][2] = {{xmin,ymin}, {xmin,ymax}, 
                              {xmax,ymax}, {xmax,ymin} }; //drag the mouse from down left to up right	
	Polygon polygon1;
	Point point1,point3,point4;
	if(polygons.size()!=0){	
		for(int i=0;i<polygons.size();i++){
			polygon1=polygons.front(); //pairno to poligono
			polygons.pop_front();
		
				for(int k=0;k<polygon1.getSize();k++){			
					point1=polygon1.getPoint();	
					poly_points[k][0]=point1.x();
					poly_points[k][1]=ry-point1.y();
					polygon1.addPoint(point1);
				}
				poly_size=polygon1.getSize();
				polygons.push_back(polygon1);//to ksanavazo mesa
				suthHodgClip(poly_points, poly_size, clipper_points, 
                 		clipper_size);
			
		}	
	}
}
// STHN PARAKATW METHODO YLOPOIOYME TO CLIPPING EPILEGONTAS POIO POLYGVNO 
//THA  APOKOPSOYME ANALOGA ME TO AN TO TETRAGVNO APOKOPHS TEMNEI TO POLYGVNO  ALLA DEN DOYLEYEI ENTELVWS SWSTA
// void sendClippingPoints(){
// 	int poly_size ; 
// 	int poly_points[20][2];
//   	flag=0;
// 	int isIntersectClip =0;
//         // Defining clipper polygon vertices in clockwise order 
//         // 1st Example with square clipper 
// 	int clipper_size = 4; 
// 	int clipper_points[][2] = {{xmin,ymin}, {xmin,ymax}, 
//                               {xmax,ymax}, {xmax,ymin} }; //drag the mouse from down left to up right	
// 	Polygon polygon1;
// 	Point point1,point3,point4;
// 	if(polygons.size()!=0){	
// 		for(int i=0;i<polygons.size();i++){
// 			polygon1=polygons.front(); //pairno to poligono
// 			polygons.pop_front();
// 			point3=polygon1.getPoint();
// 			isIntersectClip =0;	
// 			for(int j=0;j<polygon1.getSize();j++){
// 				point4=polygon1.getPoint();
// 				if(isIntersecting(xmax,ymin,xmax,ymax,
// 				point3.x(),point3.y(),point4.x(),point4.y())){
// 					isIntersectClip=1;
// 					printf("katheta \n");
// 				}
// 				polygon1.addPoint(point3);
// 				point3=point4;
// 			}
// 			polygon1.addPoint(point4);
// 			point3=polygon1.getPoint();
// 			for(int l=0;l<polygon1.getSize();l++){
// 				point4=polygon1.getPoint();
// 				if(isIntersecting(xmin,ymax,xmax,ymax,
// 				point3.x(),point3.y(),point4.x(),point4.y())){
// 					isIntersectClip=1;
// 					printf("parallila \n");
// 				}
// 				polygon1.addPoint(point3);
// 				point3=point4;
// 			}
// 			polygon1.addPoint(point4);
// 			if(isIntersectClip==1){
// 				for(int k=0;k<polygon1.getSize();k++){			
// 					point1=polygon1.getPoint();	
// 					poly_points[k][0]=point1.x();
// 					poly_points[k][1]=ry-point1.y();
// 					polygon1.addPoint(point1);
// 				}
// 				poly_size=polygon1.getSize();
// 				polygons.push_back(polygon1);//to ksanavazo mesa
// 				suthHodgClip(poly_points, poly_size, clipper_points, 
//                  		clipper_size);
// 			}
// 			else{
// 				polygons.push_back(polygon1);
// 			}		
// 		}	
// 	}
// }
void createMenus(){
	int mainMenu,actionMenu,lineColorMenu,fillColorMenu;
	lineColorMenu=glutCreateMenu(processLineColorMenuEvents); // creates LINE_COLOR menu
	glutAddMenuEntry("White", 1);     // Adds LINE_COLOR choices
	glutAddMenuEntry("Black", 2);
	glutAddMenuEntry("Red", 3);
	glutAddMenuEntry("Dark Green", 4);
	glutAddMenuEntry("Light Green", 5);
	glutAddMenuEntry("Blue", 6);
	glutAddMenuEntry("Aqua", 7);
	glutAddMenuEntry("Pink", 8);
	glutAddMenuEntry("Soft Pink", 9);
	glutAddMenuEntry("Purple", 10);
	glutAddMenuEntry("Brown", 11);
	glutAddMenuEntry("Yellow", 12);
	glutAddMenuEntry("Gray", 13);
	glutAddMenuEntry("Orange", 14);
	glutAddMenuEntry("Gold", 15);
	glutAddMenuEntry("Beige", 16);

 	fillColorMenu=glutCreateMenu(processFillColorMenuEvents); //creates FILL_Color submenu FILL_COLOR choices
	glutAddMenuEntry("White", 1);    // Adds FILL_COLOR choices
	glutAddMenuEntry("Black", 2);
	glutAddMenuEntry("Red", 3);
	glutAddMenuEntry("Dark Green", 4);
	glutAddMenuEntry("Light Green", 5);
	glutAddMenuEntry("Blue", 6);
	glutAddMenuEntry("Aqua", 7);
	glutAddMenuEntry("Pink", 8);
	glutAddMenuEntry("Soft Pink", 9);
	glutAddMenuEntry("Purple", 10);
	glutAddMenuEntry("Brown", 11);
	glutAddMenuEntry("Yellow", 12);
	glutAddMenuEntry("Gray", 13);
	glutAddMenuEntry("Orange", 14);
	glutAddMenuEntry("Gold", 15);
	glutAddMenuEntry("Beige", 16);

	actionMenu=glutCreateMenu(processActionMenuEvents);
	glutAddMenuEntry("Polygon", 1);  
	glutAddMenuEntry("Clipping", 2);       //Add "Polygon" choice in the "ACTION" submenu in the 1st place
	glutAddMenuEntry("Extrude", 3); 
	glutAddMenuEntry("Move", 4);
	glutAddMenuEntry("Exit", 5);
	mainMenu=glutCreateMenu(processActionMenuEvents);
	glutAddSubMenu("ACTION", actionMenu); // Create Submenu named "ACTION" which contains the choiceMenu choices
  	glutAddSubMenu("LINE_COLOR",lineColorMenu); // Create Submenu named "LINE_COLOR" which contains the lineColorMenu choices
	glutAddSubMenu("FILL_COLOR",fillColorMenu); // Create Submenu named "FILL_COLOR" which contains the fillColorMenu choices

	glutAttachMenu(GLUT_RIGHT_BUTTON);// set menu to open with middle click
	
}

void processSpecialKeys(int key, int xx, int yy) {

	float fraction = 0.1f;

	switch (key) {
		case GLUT_KEY_LEFT :
			angle -= 0.01f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_RIGHT :
			angle += 0.01f;
			lx = sin(angle);
			lz = -cos(angle);
			break;
		case GLUT_KEY_UP :
			x += lx * fraction;
			z2 += lz * fraction;
			break;
		case GLUT_KEY_DOWN :
			x -= lx * fraction;
			z2 -= lz * fraction;
			break;
	}	
}
int main(int argc, char ** argv){
 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(600,500);
	glutCreateWindow("ASKISI 1 ");
	w = glutGet( GLUT_WINDOW_WIDTH );
	h = glutGet( GLUT_WINDOW_HEIGHT );
	glutReshapeFunc(resize); 
	createMenus();
	glutKeyboardFunc(key);
	glutSpecialFunc(processSpecialKeys);
	glutDisplayFunc(display); 
	glutMainLoop();
	
	return 1;

}
