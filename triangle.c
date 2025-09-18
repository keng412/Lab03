/*
 * triangle.c
 *
 *  Created on: Aug 28, 2017
 *      Author: Thumrongsak Kosiyatrakul
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include "initShader.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))
int displayMode = 4;

typedef struct
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat w;
} vec4;

typedef struct
{
	vec4 x;
	vec4 y;
	vec4 z;
	vec4 w;
} mat4;

mat4 translate(GLfloat x, GLfloat y, GLfloat z)
{
	mat4 result = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
	result.w.x = x;
	result.w.y = y;
	result.w.z = z;
	return result;
}

GLuint ctm_location;
mat4 ctm = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
GLfloat x_value = 0;
int isGoingRight = 1;

vec4 vertices[63];
vec4 colors[63];
int num_vertices = 63;

void init(void)
{
    //rainbow triangle - 1
    vertices[0] = (vec4){ -0.5,  0.9, 0.0, 1.0};
    vertices[1] = (vec4){ -0.7,  0.5, 0.0, 1.0};
    vertices[2] = (vec4){ -0.3,  0.5, 0.0, 1.0};

    colors[0] = (vec4){1.0, 0.0, 0.0, 1.0};
    colors[1] = (vec4){0.0, 1.0, 0.0, 1.0};
    colors[2] = (vec4){0.0, 0.0, 1.0, 1.0};

    //green square - 2
    vertices[3] = (vec4){ 0.1, 0.5, 0.0, 1.0};
    vertices[4] = (vec4){ 0.5, 0.5, 0.0, 1.0};
    vertices[5] = (vec4){ 0.5, 0.9, 0.0, 1.0};

    vertices[6] = (vec4){ 0.1, 0.5, 0.0, 1.0};
    vertices[7] = (vec4){ 0.5, 0.9, 0.0, 1.0};
    vertices[8] = (vec4){ 0.1, 0.9, 0.0, 1.0};

    for (int i = 3; i <= 8; i++) {
        colors[i] = (vec4){0.0, 1.0, 0.0, 1.0};
    }

    //triangle loop - 3
    int idx = 9;
    int rows = 3;
    int cols = 6;

    //triangle width
    float triSize = 2.0f / cols;

    for (int r = 0; r < rows; r++) {
        float baseY = -0.9f + (rows - 1 - r) * (triSize + 0.05f);
        float topY = baseY + triSize;

        for (int c = 0; c < cols; c++) {
            float startX = -1.0f + c * triSize;
            float x0 = startX;
            float x1 = startX + triSize;
            float midX = (x0 + x1) / 2.0f;

            //single triangle
            vertices[idx]   = (vec4){ x0, baseY, 0.0, 1.0};
            vertices[idx+1] = (vec4){ x1, baseY, 0.0, 1.0};
            vertices[idx+2] = (vec4){ midX, topY, 0.0, 1.0};

            //c by row
            vec4 color;
            if (r == 0) color = (vec4){1.0, 0.0, 0.0, 1.0}; //red
            if (r == 1) color = (vec4){0.0, 1.0, 0.0, 1.0}; //green
            if (r == 2) color = (vec4){0.0, 0.0, 1.0, 1.0}; //blue

            colors[idx]   = color;
            colors[idx+1] = color;
            colors[idx+2] = color;

            idx += 3;
        }
    }

    // --- normal OpenGL init stuff ---
    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint vao;
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint vColor = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) sizeof(vertices));

	ctm_location = glGetUniformLocation(program, "ctm");

	printf("ctm_location:%i\n", ctm_location);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDepthRange(1,0);
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &ctm);

    if (displayMode == 1) {
        //only rainbow
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    else if (displayMode == 2) {
        //only square
        glDrawArrays(GL_TRIANGLES, 3, 6);
    }
    else if (displayMode == 3) {
        //only 3 rows of triangle loop
        glDrawArrays(GL_TRIANGLES, 9, 54);
    }
    else if (displayMode == 4) {
        glDrawArrays(GL_TRIANGLES, 0, 3);   //rainbow
        glDrawArrays(GL_TRIANGLES, 3, 6);   //square
        glDrawArrays(GL_TRIANGLES, 9, 54);  //triangle loop
    }

    glutSwapBuffers();
}


void keyboard(unsigned char key, int mousex, int mousey)
{
    if (key == 'q') exit(0);

    if (key == '1') displayMode = 1;  //rainbow
    if (key == '2') displayMode = 2;  //square
    if (key == '3') displayMode = 3;  //triangle loop
    if (key == '4') displayMode = 4;  //all

    glutPostRedisplay();
}

void idle(void)
{
	if(isGoingRight)
	{
		x_value += 0.005;
		if(x_value > 1.0)
		{
			x_value = 1.0;
			isGoingRight = 0;
		}
	}
	else
	{
		x_value -= 0.005;
		if(x_value < -1.0)
		{
			x_value = -1.0;
			isGoingRight = 1;
		}
	}

	ctm = translate(x_value, 0.0, 0.0);
	//glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Triangle");
    //glewInit();
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

    return 0;
}