//#include "precompiled.h"
#include <windows.h>
#include <GL/glu.h>

#include "DebugDraw.h"

void drawWorldAxes(float scale)
{
	// Draw coord sys axes at origo
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
	float m = scale;
	glBegin(GL_LINES);
		glColor3f(1,0,0); glVertex3f(0,0,0); glVertex3f(m,0,0);
		glColor3f(0,1,0); glVertex3f(0,0,0); glVertex3f(0,m,0);
		glColor3f(0,0,1); glVertex3f(0,0,0); glVertex3f(0,0,m);
	glEnd();
	glPopAttrib();
}

void drawMatrix(const Matrix4 *mat, float scale, bool drawrings )
{
	GLUquadric *qobj = gluNewQuadric();

  glPushMatrix();
	if (mat != NULL) glMultMatrixf(&(*mat)[0][0]);
	glScalef(scale,scale,scale);

	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glLineWidth(2.0f);

	// Draw coord axes
	glBegin(GL_LINES);
		glColor3f(1, 0, 0);	glVertex3f(0,0,0); glVertex3f(2,0,0);
		glColor3f(0, 1, 0);	glVertex3f(0,0,0); glVertex3f(0,2,0);
		glColor3f(0, 0, 1);	glVertex3f(0,0,0); glVertex3f(0,0,2);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(-1.0f, -1.0f);
	
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
	//gluSphere(qobj, 0.8f, 30, 30);

	// Draw plane disks
  if( drawrings ) {
    glPushMatrix();
	  glRotatef(-90, 0, 1, 0);
	  glColor4f(1, 0, 0, 0.5f);
	  gluDisk(qobj, 0.8f, 1, 30, 1);
	  glPopMatrix();
	  
    glPushMatrix();
	  glRotatef(-90, 1, 0, 0);
	  glColor4f(0, 1, 0, 0.5f);
	  gluDisk(qobj, 0.8f, 1, 30, 1);
	  glPopMatrix();
	  
	  glColor4f(0, 0, 1, 0.5f);
	  gluDisk(qobj, 0.8f, 1, 30, 1);
  }
	
	glPopAttrib();

  glPopMatrix();

	gluDeleteQuadric(qobj);
}

void drawAAWireBox(float xside, float yside, float zside, const Vec3& origin)
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glLineWidth(1.0f);
	
	glBegin(GL_LINE_STRIP);
		glVertex3f(origin[0],				origin[1],			 origin[2]);
		glVertex3f(origin[0],				origin[1]+yside, origin[2]); 
		glVertex3f(origin[0]+xside,	origin[1]+yside, origin[2]); 
		glVertex3f(origin[0]+xside,	origin[1],			 origin[2]);
		glVertex3f(origin[0],				origin[1],			 origin[2]);
	glEnd();

	glBegin(GL_LINE_STRIP);
		glVertex3f(origin[0],				origin[1],			 origin[2]+zside);
		glVertex3f(origin[0],				origin[1]+yside, origin[2]+zside); 
		glVertex3f(origin[0]+xside,	origin[1]+yside, origin[2]+zside); 
		glVertex3f(origin[0]+xside,	origin[1],			 origin[2]+zside);
		glVertex3f(origin[0],				origin[1],			 origin[2]+zside);
		glEnd();

	glBegin(GL_LINES);
		glVertex3f(origin[0],				origin[1],			 origin[2]);
		glVertex3f(origin[0],				origin[1],			 origin[2]+zside);
		
		glVertex3f(origin[0],				origin[1]+yside, origin[2]); 
		glVertex3f(origin[0],				origin[1]+yside, origin[2]+zside); 
		
		glVertex3f(origin[0]+xside,	origin[1]+yside, origin[2]); 
		glVertex3f(origin[0]+xside,	origin[1]+yside, origin[2]+zside); 
		
		glVertex3f(origin[0]+xside,	origin[1],			 origin[2]);
		glVertex3f(origin[0]+xside,	origin[1],			 origin[2]+zside);
	glEnd();

	glPopAttrib();
}

void drawAAWireBox(const Vec3& size, const Vec3& origin)
{
	drawAAWireBox(size[0], size[1], size[2], origin);
}

void drawTransparentSphere(const Vec3& centre, float radius)
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	//glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(-1.0f, -1.0f);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
  glPushMatrix();
	glTranslatef(centre[0], centre[1], centre[2]);
	GLUquadric *qobj = gluNewQuadric();
	gluSphere(qobj, radius, 30, 30);
	gluDeleteQuadric(qobj);
  glPopMatrix();

	glPopAttrib();
}
