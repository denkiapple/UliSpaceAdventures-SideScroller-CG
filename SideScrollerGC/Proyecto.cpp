/***************************************
 Proyecto Final Gráficas Computacionales
 
 Ulises Torner Campuzano - A01333456
 Adrian Paredes Chavez
 Hiram Araujo - A01336671
 
 - Balas: cuestan 1 de munición - 5 para matar enemigos, inútiles contra asteroides
 - Misil: sólo disparas uno a la vez, cuestan 10 de munición, matan asteroides
 - SuperLaser: cuesta 25 de munición y 2 de vida, destruye todo pero no daña a Kylo Ren
****************************************/

//Imports **************************************************************/
#ifdef _WIN32
    #include "glut.h"
#elif __APPLE__
    #include <GLUT/GLUT.h>
#endif

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include "targa.h"
#include "soil/SOIL.h"
#include "glm/glm.h"


//Definiciones de las Funciones ****************************************/
void Init();
void Display();
void Reshape(int w, int h);

void loadTextures();
void loadLights();

void SpecialKeys(int key, int x, int y);
void keyboard (unsigned char key, int x, int y);
void KeyboardUp(unsigned char key, int x, int y);

void mostrarTexto(float x, float y, void *font,const char *string);
void DrawFlechita();
void DrawButton(char *texto, GLfloat x, GLfloat y);
void drawTrinchera(bool paralax);
void drawHUD();

void menuPrincipal();
void pantallaControles();
void juego(bool intro);
void pantallaPausa();
void gameOver();

void colisionamos();
void generaMalos();


//Variables ************************************************************/
bool pausa = false;
bool intro = true;
GLint estadoJuego = 0;
GLint posicionFlecha = 0;
GLint dificultad = 1;
GLfloat momento = 0.04;
GLfloat momentoBala = 0.1;
int score = 0;

GLfloat maintextureSlide = 0.0;
GLfloat tiempoInicio = 0.0;
GLfloat introX = 5.0;
GLfloat slideIn = -3.0;
GLuint texturas[15];
GLfloat u = 0.0;
GLfloat v = 1.0;

bool showColiders = false;
bool superLaserReady = true;
GLuint superLaserCooldown = 0;
bool misilReady = true;
GLfloat posXmisil = 0.0;
GLfloat posYmisil = 0.0;


//Modelos
GLMmodel *xWing, *asteroide, *tieFighter, *kyloRen, *bb8UP, *bb8Down, *bb8FULL;
GLfloat xWid = 0.30; GLfloat aWid = 0.35; GLfloat tWid = 0.35; GLfloat kWid = 0.35;
GLfloat angRotaAsteroide = 0.0;
GLfloat tiltXWing = 0.0;
GLint municiones = 25;
GLint disparos = 0;
GLuint frames = 0;
GLint numEnemigos = 0;
GLint numAsteroides = 0;
GLfloat ang = 0.0;
GLfloat slideBB8 = 5.0;
bool catchBB8 = false;
GLfloat kyloLife = 10.0;
GLfloat posXkylo = -5.0;
GLfloat posYkylo = 0.0;


//Variables de movimiento
bool moveLeft  = false;
bool moveRight = false;
bool moveUp    = false;
bool moveDown  = false;


//Variables de la Nave
GLfloat posX = -0.5;
GLfloat posY = 0.0;
GLfloat posZ = 0.0;
GLfloat vida = 10.0;
GLint frameTextBoomXW = 0;


//Posición de los PowerUps
GLfloat xPowerUp = 0.0;
GLfloat yPowerUp = 0.0;
GLfloat xPowerHealth = 0.0;
GLfloat yPowerHealth = 0.0;
bool mostrarPowerUp = false;
bool mostrarPowerHealth = false;

//Variables del Texto
int *font = (int*)GLUT_BITMAP_HELVETICA_18;
int *font2 = (int*)GLUT_BITMAP_HELVETICA_12;


//Estructura Genérica
typedef struct Estructura {
    GLfloat xStruct;
    GLfloat yStruct;
    
    GLfloat xVector;
    GLfloat yVector;
    
    GLfloat xBala;
    GLfloat yBala;
    
    bool activo;
    GLuint vida;
} Estructura;

Estructura *nuevaEstructura(GLfloat x, GLfloat y) {
    Estructura *e = (Estructura *)malloc(sizeof(Estructura));
    
    e->xStruct = x;
    e->yStruct = y;
    
    e->xVector = 0.0;
    e->yVector = 0.0;
    e->xBala = 0.0;
    e->yBala = 0.0;
    
    e->activo = true;
    e->vida = 5;
    
    return e;
}

Estructura *nuevoAsteroide(GLfloat x, GLfloat y, GLfloat xVctr, GLfloat yVctr) {
    Estructura *e = nuevaEstructura(x, y);
    e->xVector = xVctr;
    e->yVector = yVctr;
    return e;
}

Estructura *nuevoEnemigo(GLfloat x, GLfloat y, GLfloat xVctr, GLfloat yVctr) {
    Estructura *e = nuevoAsteroide(x, y, xVctr, yVctr);
    e->xBala = x;
    e->yBala = y;
    return e;
}

using namespace std;
vector <Estructura> balas;
vector <Estructura> asteroides;
vector <Estructura> enemigos;
vector <Estructura> booms;
vector <Estructura> balasKylo;


//Inicio ***************************************************************/
void Init() {
	glClearColor(0,0.1,0.15,1);
	glEnable(GL_DEPTH_TEST);
    
    loadTextures();
    loadLights();
    
    //Cargar los modelos
    xWing = glmReadOBJ("./xWing/MASTER_AJIO_FINAL.obj");
    asteroide = glmReadOBJ("meteorito_normal.obj");
    tieFighter = glmReadOBJ("./TieFighter/THAILANDESE_FIGHTER_FINAL_FINAL.obj");
    kyloRen = glmReadOBJ("./KyloShuttle/ulichanFinal.obj");
    bb8UP = glmReadOBJ("./BB8/mema_bebocho.obj");
    bb8Down= glmReadOBJ("./BB8/esfera_bb8.obj");
    bb8FULL = glmReadOBJ("./BB8/bebocho_full.obj");
}

//Cargar las Texturas
void loadTextures() {
    //Texturas
    GLubyte *data;
    GLint xText,yText,dText;
    
    data=LoadTGA("space.tga",&xText,&yText,&dText);
    glBindTexture(GL_TEXTURE_2D,1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,xText,yText,0, GL_RGB,GL_UNSIGNED_BYTE,data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    data=LoadTGA("spaceShip.tga",&xText,&yText,&dText);
    glBindTexture(GL_TEXTURE_2D,2);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,xText,yText,0, GL_RGB,GL_UNSIGNED_BYTE,data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    data=LoadTGA("spaceShipSides.tga",&xText,&yText,&dText);
    glBindTexture(GL_TEXTURE_2D,3);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 3);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,xText,yText,0, GL_RGB,GL_UNSIGNED_BYTE,data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    delete data;
    
    
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    
    texturas[0] = SOIL_load_OGL_texture
    (
     "tuberias.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[1] = SOIL_load_OGL_texture
    (
     "space.jpg",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[2] = SOIL_load_OGL_texture
    (
     "transparent.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[3] = SOIL_load_OGL_texture
    (
     "./Sprites/explosionSprites.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[4] = SOIL_load_OGL_texture
    (
     "./Sprites/vida_powerUp.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[5] = SOIL_load_OGL_texture
    (
     "./Sprites/roces.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[6] = SOIL_load_OGL_texture
    (
     "./Sprites/laser_rojo.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[7] = SOIL_load_OGL_texture
    (
     "./Sprites/laser_verde.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[8] = SOIL_load_OGL_texture
    (
     "./Sprites/andromeda_shun_azul.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[9] = SOIL_load_OGL_texture
    (
     "./Sprites/superLaser.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    texturas[10] = SOIL_load_OGL_texture
    (
     "./Sprites/armas_powerUp.png",
     SOIL_LOAD_AUTO,
     SOIL_CREATE_NEW_ID,
     SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
     );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

//Inicializar las luces
void loadLights() {
    //Definir las Luces
    GLfloat posLuz[] = {0,0,4,1};
    glLightfv(GL_LIGHT0, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT1, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT2, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT3, GL_POSITION, posLuz);
    glLightfv(GL_LIGHT4, GL_POSITION, posLuz);
    
    //Para la luz 0:
    GLfloat ambien0[4] = {0.9,0.9,0.9,1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambien0);
    GLfloat diff0[4] = {0.5,0.5,0.5,1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
    GLfloat spec0[4] = {0.5,0.5,0.5,1.0};
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 51.2);
    
    //Para la luz 1:
    GLfloat ambien1[4] = {0.7,0.6,0.6,1.0};
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambien1);
    GLfloat diff1[4] = {0.5508,0.2118,0.066,1.0};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diff1);
    GLfloat spec1[4] = {0.580594,0.23257,0.0695701,1.0};
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 51.2);
    
    //Para la luz 2:
    GLfloat ambien2[4] = {0.7,0.6,0.6,1.0};
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambien2);
    GLfloat diff2[4] = {0.5508,0.2118,0.066,1.0};
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diff2);
    GLfloat spec2[4] = {0.580594,0.23257,0.0695701,1.0};
    glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 51.2);
    
    //Para la luz 3:
    GLfloat ambien3[4] = {0.8,0.2,0.2,1.0};
    glLightfv(GL_LIGHT3, GL_AMBIENT, ambien3);
    GLfloat diff3[4] = {0.5508,0.2118,0.066,1.0};
    glLightfv(GL_LIGHT3, GL_DIFFUSE, diff3);
    GLfloat spec3[4] = {0.580594,0.23257,0.0695701,1.0};
    glLightfv(GL_LIGHT3, GL_SPECULAR, spec3);
    glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 51.2);
    
    //Para la luz 4:
    GLfloat ambien4[4] = {1.0,0.1,0.1,1.0};
    glLightfv(GL_LIGHT4, GL_AMBIENT, ambien4);
    GLfloat diff4[4] = {0.2,0.2,0.2,1.0};
    glLightfv(GL_LIGHT4, GL_DIFFUSE, diff4);
    GLfloat spec4[4] = {0.2,0.2,0.2,1.0};
    glLightfv(GL_LIGHT4, GL_SPECULAR, spec4);
    glLightf(GL_LIGHT4, GL_SPOT_CUTOFF, 10);
}

//Display
void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    GLfloat tiempoActual = glutGet(GLUT_ELAPSED_TIME);
    intro = (tiempoActual - tiempoInicio >= 1000.0) ? false : true;
    if(fmod(tiempoActual - tiempoInicio, 2000.0) == 0.0) dificultad++;
    
    //Máquina de estados del juego
    if(estadoJuego == 0) menuPrincipal();
    else if(estadoJuego == 1) pantallaControles();
    else if(estadoJuego == 2) exit(0);
    else if(estadoJuego == 3 || estadoJuego == 4) gameOver();
    else juego(intro);
    
    //Textura de Fondo espacial
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[1]);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0,0.0+maintextureSlide); glVertex3f(-4.0,  2.3, -4.0);
    glTexCoord2f(1.0,1.0+maintextureSlide); glVertex3f( 4.0,  2.3, -4.0);
    glTexCoord2f(0.0,1.0+maintextureSlide); glVertex3f( 4.0, -2.3, -4.0);
    glTexCoord2f(0.0,0.0+maintextureSlide); glVertex3f(-4.0, -2.3, -4.0);
    glEnd(); glDisable(GL_TEXTURE_2D);
    
    if(introX > 0.0) introX-= 0.05;
    if(!pausa) maintextureSlide >= 1.0 ? maintextureSlide = 0.0 : maintextureSlide += 0.005;
    
    glutSwapBuffers();
    glutPostRedisplay();
}

//Menú Principal
void menuPrincipal() {
    //Dibujar Botones
    glColor3f(0.8, 0.0, 0.0);
    DrawButton("Nuevo Juego", 0.0, -0.5);
    DrawButton("Controles", 0.0, -1.0);
    DrawButton("Salir", 0.0, -1.5);
    
    glColor3f(1,1,1);
    mostrarTexto(-1.15, 1.6, (void*)font, "Uli Adventures: El Rescate de BB-8");
    char instruccionesMenu[] = "Use las flechitas para moverse en el menu principal y enter para seleccionar una opcion";
    mostrarTexto(-2.0, -2.0, (void*)font2, instruccionesMenu);
    
    glPushMatrix();
    switch(posicionFlecha) {
        case 0:
            glTranslatef(1.0, -0.5, 0.0);
            break;
        case 1:
            glTranslatef(1.0, -1.0, 0.0);
            break;
        case 2:
            glTranslatef(1.0, -1.5, 0.0);
            break;
    }
    DrawFlechita();
    glPopMatrix();
    
    //Dibujar a BB-8
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    
    glPushMatrix();
    glRotatef(250 + 50*sin(ang/100), 0, 1, 0);
    glScalef(0.3, 0.3, 0.3);
    glmDraw(bb8UP, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.0, 0.3, 0.0);
    glRotatef(-ang*20, 0, 0, 1);
    glRotatef(80, 0, 1, 0);
    glScalef(0.3, 0.3, 0.3);
    glmDraw(bb8Down, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
    glPopMatrix();
    
    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
    glColor3f(1, 1, 1);
    
    ang+=0.6;
}

//Menú Controles
void pantallaControles() {
    glPushMatrix();
    glColor3f(1, 1, 1);
    
    char controles[] = "Controles Basicos del Juego";
    mostrarTexto(-0.9, 1.6, font, controles);
    
    glBegin(GL_QUADS);
    glVertex3f(-0.25, 1.1, -1);
    glVertex3f(0.25, 1.1, -1);
    glVertex3f(0.25, 0.6, -1);
    glVertex3f(-0.25, 0.6, -1);
    
    glVertex3f(-0.25, 0.5, -1);
    glVertex3f(0.25, 0.5, -1);
    glVertex3f(0.25, 0.0, -1);
    glVertex3f(-0.25, 0.0, -1);
    
    glVertex3f(-0.85, 0.5, -1);
    glVertex3f(-0.35, 0.5, -1);
    glVertex3f(-0.35, 0.0, -1);
    glVertex3f(-0.85, 0.0, -1);
    
    glVertex3f(0.85, 0.5, -1);
    glVertex3f(0.35, 0.5, -1);
    glVertex3f(0.35, 0.0, -1);
    glVertex3f(0.85, 0.0, -1);
    glEnd();
    
    glBegin(GL_QUADS);
    glVertex3f(-1.0, -1.0, -1);
    glVertex3f(1.0, -1.0, -1);
    glVertex3f(1.0, -1.5, -1);
    glVertex3f(-1.0, -1.5, -1);
    glEnd();
    
    //Labels instructivos
    mostrarTexto(-0.16, 1.2, font, "Subir");
    mostrarTexto(-0.15, -0.2, font, "Bajar");
    mostrarTexto(-1.7, 0.2, font, "Retroceder");
    mostrarTexto(1.0, 0.2, font, "Avanzar");
    mostrarTexto(-0.3, -0.9, font, "Disparar");
    
    //Labels de las teclas
    glColor3f(0, 0, 0);
    mostrarTexto(-0.07, 0.8, font, "W");
    mostrarTexto(-0.07, 0.2, font, "S");
    mostrarTexto(-0.67, 0.2, font, "A");
    mostrarTexto(0.57, 0.2, font, "D");
    mostrarTexto(-0.57, -1.3, font, "Barra Espaciadora");
    
    
    //Botón de Volver al Menú principal
        //Luego hay que cambiar esto por el método DrawButton();
    glColor3f(0.8, 0.9, 0.9);
    glBegin(GL_QUADS);
    glVertex3f(-3.7, 1.7, -1);
    glVertex3f(-2.7, 1.7, -1);
    glVertex3f(-2.7, 2.2, -1);
    glVertex3f(-3.7, 2.2, -1);
    glEnd();
    
    glColor3f(0, 0, 0);
    mostrarTexto(-3.5, 1.9, font, "Regresar");
    
    glTranslatef(-2.6, 1.95, 1);
    DrawFlechita();
    
    glPopMatrix();
}

//Pantalla de Fin de Juego
void gameOver() {
    posX = 0.0;
    posY = 0.0;
    
    glColor3f(1,1,1);
    mostrarTexto(0, 0, (void*)font, estadoJuego == 3 ? "GAME-OVER" : "SUPER WIN!!!");
    
    //Botón para regresar al menú principal
    glColor3f(1, 1, 1);
    DrawButton("Regresar", 0, -1);
    
    glTranslatef(1, -1, 0);
    DrawFlechita();
}

//Pantalla de Pausa
void pantallaPausa() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texturas[2]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.8); glVertex3f(-4.0,  0.8, 2.0);
    glTexCoord2f(1.0,0.8); glVertex3f( 4.0,  0.8, 2.0);
    glTexCoord2f(1.0,0.2); glVertex3f( 4.0, -0.8, 2.0);
    glTexCoord2f(0.0,0.2); glVertex3f(-4.0, -0.8, 2.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//Método para reiniciar los valores cuando inicias un nuevo juego
void preparaJuego() {
    introX = 5.0;
    frameTextBoomXW = 0;
    
    score = 0;
    vida = 10.0;
    dificultad = 1;
    municiones = 25;
    misilReady = true;
    superLaserReady = true;
    posX = -0.5; posY = 0.0;
    frames = 0;
    
    slideBB8 = 5.0;
    catchBB8 = false;
    kyloLife = 10.0;
    posXkylo = -5.0;
    posYkylo = 0.0;
    
    mostrarPowerUp = false;
    mostrarPowerHealth = false;
    
    enemigos.clear(); asteroides.clear();
    numEnemigos = numAsteroides = 0;
    xPowerUp = yPowerUp = xPowerHealth = yPowerHealth = 0.0;
    
    tiempoInicio = glutGet(GLUT_ELAPSED_TIME);
    estadoJuego = -1;
}

//Pantala principal del Juego
void juego(bool intro) {
    glLoadIdentity();
    if(!intro) drawHUD();
    if(pausa) pantallaPausa();
    if(score < 25) generaMalos();
    
    //Definicion del Material:
    GLfloat diff3[] = {1,1,1,1};
    GLfloat amb3[] = {0.4,0.4,0.4,1};
    GLfloat spec3[] = {0.5,0.5,0.5,1};
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diff3);
    glMaterialfv(GL_FRONT, GL_AMBIENT, amb3);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec3);
    glMaterialf(GL_FRONT, GL_SHININESS, 10);
    
    //Dibuja los PowerUps
    if(!intro && vida > 0) {
        if(municiones <= 5) mostrarPowerUp = true;
        
        GLint tmp = rand()%100;
        if(tmp == 10 && mostrarPowerHealth==false && municiones<40) mostrarPowerUp = true;
        
        if(mostrarPowerUp) {
            glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[10]);
            glBegin(GL_QUADS);
            glTexCoord2f(1.0,0.0); glVertex3f(-0.15+xPowerUp, -0.15+yPowerUp, 0);
            glTexCoord2f(1.0,1.0); glVertex3f(-0.15+xPowerUp,  0.15+yPowerUp, 0);
            glTexCoord2f(0.0,1.0); glVertex3f( 0.15+xPowerUp,  0.15+yPowerUp, 0);
            glTexCoord2f(0.0,0.0); glVertex3f( 0.15+xPowerUp, -0.15+yPowerUp, 0);
            glEnd(); glDisable(GL_TEXTURE_2D);
        }
        
        if(vida < 6.0) {
            if(tmp == 35 && mostrarPowerUp==false) mostrarPowerHealth = true;
        }
        
        if(mostrarPowerHealth) {
            glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[4]);
            glBegin(GL_QUADS);
            glTexCoord2f(1.0,0.0); glVertex3f(-0.15+xPowerHealth, -0.15+yPowerHealth, 0);
            glTexCoord2f(1.0,1.0); glVertex3f(-0.15+xPowerHealth,  0.15+yPowerHealth, 0);
            glTexCoord2f(0.0,1.0); glVertex3f( 0.15+xPowerHealth,  0.15+yPowerHealth, 0);
            glTexCoord2f(0.0,0.0); glVertex3f( 0.15+xPowerHealth, -0.15+yPowerHealth, 0);
            glEnd(); glDisable(GL_TEXTURE_2D);
        }
    }
    
    //Movimientos y dibujo del objeto
    if(vida>0) {
        glPushMatrix();
        if(!intro) {
            if(!pausa) {
                if(moveUp && posY < 1.4) posY += momento;
                if(moveDown && posY > -1.4) posY -= momento;
                if(moveLeft && posX > -3.3) posX -= momento;
                if(moveRight && posX < 3.3) posX += momento;
            }
            
            glTranslatef(posX, posY, posZ);
            glRotatef(90, 0, 1, 0);
            if(moveUp) glRotatef(-tiltXWing, 0, 0, 1);
            if(moveDown) glRotatef(tiltXWing, 0, 0, 1);
            if(tiltXWing<45 && !pausa) tiltXWing += 0.5;
            
            //Dibuja los roces del objeto con las paredes
            if(posY > 1.2 || posY < -1.2) {
                glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[5]);
                glBegin(GL_QUADS);
                glTexCoord2f(1.0,0.0); glVertex3f(-1.0, -0.5, 1);
                glTexCoord2f(1.0,1.0); glVertex3f(-1.0,  0.5, 1);
                glTexCoord2f(0.0,1.0); glVertex3f( 1.0,  0.5, 1);
                glTexCoord2f(0.0,0.0); glVertex3f( 1.0, -0.5, 1);
                glEnd(); glDisable(GL_TEXTURE_2D);
            }
            
        } else {
            glTranslatef(slideIn, 0.0, 0.0);
            glRotatef(90, 0, 1, 0);
            if(slideIn<-0.5) slideIn += 0.05;
        }
        
        if(showColiders) glutWireSphere(xWid, 20, 20);
        glScalef(0.3, 0.3, 0.3);
        glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        glmDraw(xWing, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
        glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
        glPopMatrix();
        
    } else {
        //En caso de que explote la nave:
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,texturas[3]);
        glBegin(GL_QUADS);
        glTexCoord2f(u,v+0.2); glVertex3f(posX - (xWid), posY + (xWid), 3);
        glTexCoord2f(u+0.2,v+0.2); glVertex3f(posX + (xWid), posY + (xWid), 3);
        glTexCoord2f(u+0.2,v); glVertex3f(posX + (xWid), posY - (xWid), 3);
        glTexCoord2f(u,v); glVertex3f(posX - (xWid), posY - (xWid), 3);
        glEnd(); glDisable(GL_TEXTURE_2D);
        
        frameTextBoomXW++;
        if(frameTextBoomXW % 10 == 0) {
            if(v <= 0.0) {
                v = 1.0;
                u >= 1.0 ? u=0.0 : u+=0.2;
            } else {
                v -= 0.2;
            }
        }
        
        if(frameTextBoomXW >= 200) estadoJuego = 3;
    }
    
    //Disparos
    if(balas.size() > 0) if(balas[0].xStruct >= 4) balas.erase(balas.begin());
    if( municiones > 0 && vida > 0) {
        for(int i=0; i<balas.size(); i++) {
            if(balas[i].activo) {
                glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[6]);
                glBegin(GL_QUADS);
                glTexCoord2f(1.0,0.0); glVertex3f(balas[i].xStruct -0.2, balas[i].yStruct -0.1, 1);
                glTexCoord2f(1.0,1.0); glVertex3f(balas[i].xStruct -0.2, balas[i].yStruct +0.1, 1);
                glTexCoord2f(0.0,1.0); glVertex3f(balas[i].xStruct +0.2, balas[i].yStruct +0.1, 1);
                glTexCoord2f(0.0,0.0); glVertex3f(balas[i].xStruct +0.2, balas[i].yStruct -0.1, 1);
                glEnd(); glDisable(GL_TEXTURE_2D);
            }
            balas[i].xStruct += momentoBala;
        }
    }
    
    if(!misilReady && vida > 0) {
        glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[8]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0,0.0); glVertex3f(posXmisil +0.25, posYmisil -0.15, 1);
        glTexCoord2f(1.0,1.0); glVertex3f(posXmisil +0.25, posYmisil +0.15, 1);
        glTexCoord2f(0.0,1.0); glVertex3f(posXmisil -0.25, posYmisil +0.15, 1);
        glTexCoord2f(0.0,0.0); glVertex3f(posXmisil -0.25, posYmisil -0.15, 1);
        glEnd(); glDisable(GL_TEXTURE_2D);
        posXmisil += momentoBala;
        
        if(posXmisil > 4.0) {
            misilReady = true;
        }
    }
    
    if(!superLaserReady && vida > 0) {
        glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[9]);
        glBegin(GL_QUADS);
        glTexCoord2f(1.0,0.0); glVertex3f(posX +8.0, posY -0.15, 1);
        glTexCoord2f(1.0,1.0); glVertex3f(posX +8.0, posY +0.15, 1);
        glTexCoord2f(0.0,1.0); glVertex3f(posX +0.5, posY +0.15, 1);
        glTexCoord2f(0.0,0.0); glVertex3f(posX +0.5, posY -0.15, 1);
        glEnd(); glDisable(GL_TEXTURE_2D);
        
        if(superLaserCooldown > 200) {
            superLaserReady = true;
            superLaserCooldown = 0;
        }
        
        superLaserCooldown++;
    }
    
    //Otros elementos del juego
    if(!intro) {
        //Dibujar a los Asteroides
        if(asteroides.size()>0) {
            for(int i=0; i<asteroides.size(); i++) {
                if(asteroides[i].activo) {
                    glPushMatrix();
                    glTranslatef(asteroides[i].xStruct, asteroides[i].yStruct, 0.0);
                    
                    if(!pausa) {
                        glRotatef(angRotaAsteroide, 0, 1, 1);
                        
                        //Mover a los asteroides
                        asteroides[i].xStruct += asteroides[i].xVector;
                        asteroides[i].yStruct += asteroides[i].yVector;
                    }
                    
                    if(showColiders) glutWireSphere(aWid, 10, 10);
                    
                    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
                    glmDraw(asteroide, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
                    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
                    glPopMatrix();
                    
                    //Eliminar a los asteroides que se salen de la pantalla
                    if(asteroides[i].xStruct < -4) {
                        asteroides[i].activo = false;
                        asteroides[i].vida = 0;
                        numAsteroides--;
                    }
                }
            }
        }
        
        //Dibujar a las naves enemigas
        if(enemigos.size()>0) {
            for(int i=0; i<enemigos.size(); i++) {
                if(enemigos[i].activo) {
                    glPushMatrix();
                    //Vida de los malos
                    glColor3f(0.9, 0.6, 0.0); glBegin(GL_QUADS);
                    glVertex3f(enemigos[i].xStruct-(tWid/2),
                               enemigos[i].yStruct+(tWid/2) + 0.3, 2);
                    
                    glVertex3f(enemigos[i].xStruct+(tWid/2) - (0.4)*((5.0-(GLfloat)enemigos[i].vida)/5.0),
                               enemigos[i].yStruct+(tWid/2) + 0.3, 2);
                    
                    glVertex3f(enemigos[i].xStruct+(tWid/2) - (0.4)*((5.0-(GLfloat)enemigos[i].vida)/5.0),
                               enemigos[i].yStruct+(tWid/2) + 0.2, 2);
                    
                    glVertex3f(enemigos[i].xStruct-(tWid/2),
                               enemigos[i].yStruct+(tWid/2) + 0.2, 2);
                    glEnd(); glColor3f(1, 1, 1);
                    
                    //Mover la posición de los malos
                    glTranslatef(enemigos[i].xStruct, enemigos[i].yStruct, 0.0);
                    
                    
                    if(!pausa) {
                        glRotatef(-angRotaAsteroide*2, 0, 1, 0);
                        
                        //Mover a los malos
                        enemigos[i].xStruct += 0.2*enemigos[i].xVector;
                        enemigos[i].yStruct += 0.2*enemigos[i].yVector;
                        
                        //Rebotar a los enemigos de las paredes para que no choquen
                        if(enemigos[i].yStruct > 1.3 || enemigos[i].yStruct < -1.3) {
                            enemigos[i].yVector = -enemigos[i].yVector;
                        }
                    }
                    
                    //Eliminar a los malos que se salen de la pantalla
                    if(enemigos[i].xStruct < -4) {
                        enemigos[i].activo = false;
                        enemigos[i].vida = 0;
                        numEnemigos--;
                    }
                    
                    //Mostrar el bounding sphere de los malos
                    if(showColiders) glutWireSphere(tWid, 20, 20);
                    
                    glScalef(0.15, 0.15, 0.15);
                    glEnable(GL_LIGHTING);
                    
                    switch (enemigos[i].vida) {
                        case 5:
                            glEnable(GL_LIGHT0);
                            break;
                        case 4:
                            glEnable(GL_LIGHT1);
                            break;
                        case 3:
                            glEnable(GL_LIGHT2);
                            break;
                        case 2:
                            glEnable(GL_LIGHT3);
                            break;
                        case 1:
                            glEnable(GL_LIGHT4);
                            break;
                    }
                    
                    //glEnable(GL_LIGHT0);
                    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
                    glmDraw(tieFighter, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
                    glDisable(GL_LIGHTING); glDisable(GL_LIGHT0); glDisable(GL_LIGHT1);
                    glDisable(GL_LIGHT2); glDisable(GL_LIGHT3); glDisable(GL_LIGHT4);
                    glPopMatrix();
                    
                    //Disparos de los malos
                    enemigos[i].xBala -= momentoBala;
                    if(enemigos[i].xBala < -4) {
                        enemigos[i].xBala = enemigos[i].xStruct;
                        enemigos[i].yBala = enemigos[i].yStruct;
                    }
                    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[7]);
                    glBegin(GL_QUADS);
                    glTexCoord2f(1.0,0.0); glVertex3f(enemigos[i].xBala -0.2, enemigos[i].yBala -0.1, 1);
                    glTexCoord2f(1.0,1.0); glVertex3f(enemigos[i].xBala -0.2, enemigos[i].yBala +0.1, 1);
                    glTexCoord2f(0.0,1.0); glVertex3f(enemigos[i].xBala +0.2, enemigos[i].yBala +0.1, 1);
                    glTexCoord2f(0.0,0.0); glVertex3f(enemigos[i].xBala +0.2, enemigos[i].yBala -0.1, 1);
                    glEnd(); glDisable(GL_TEXTURE_2D);
                }
            }
        }
        
        //Dibuja los Booms:
        if(booms.size() > 0) {
            for(int i=0; i<booms.size(); i++) {
                if(booms[i].activo) {
                    glEnable(GL_TEXTURE_2D);
                    glBindTexture(GL_TEXTURE_2D,texturas[3]);
                    glBegin(GL_QUADS);
                        glTexCoord2f(u,v+0.2); glVertex3f(booms[i].xStruct - (tWid), booms[i].yStruct + (tWid), 3);
                        glTexCoord2f(u+0.2,v+0.2); glVertex3f(booms[i].xStruct + (tWid), booms[i].yStruct + (tWid), 3);
                        glTexCoord2f(u+0.2,v); glVertex3f(booms[i].xStruct + (tWid), booms[i].yStruct - (tWid), 3);
                        glTexCoord2f(u,v); glVertex3f(booms[i].xStruct - (tWid), booms[i].yStruct - (tWid), 3);
                    glEnd(); glDisable(GL_TEXTURE_2D);
                    
                    booms[i].vida++;
                    if(booms[i].vida % 10 == 0) {
                        if(v <= 0.0) {
                            v = 1.0;
                            u >= 1.0 ? u=0.0 : u+=0.2;
                        } else {
                            v -= 0.2;
                        }
                    }
                    
                    if(booms[i].vida >= 80) booms[i].activo = false;
                }
            }
        }
    }
    
    //Dibujo de BB-8 si matas más de 24 enemigos
    if(score >= 24 && !catchBB8) {
        if(slideBB8 > -4) {
            glPushMatrix();
            
            glTranslatef(slideBB8, 0, 0);
            glRotatef(ang, 1, 1, 1);
            glScalef(0.2, 0.2, 0.2);
            
            glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
            glmDraw(bb8FULL, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
            glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
            glPopMatrix();
            
        } else {
            Estructura explosion = *nuevaEstructura(slideBB8, 0.0);
            //En el caso de las explosiones la vida es el framerate
            explosion.vida = 0;
            booms.push_back(explosion);
            
            if(slideBB8 < -6) estadoJuego = 3;
        }
        
        slideBB8 -= 0.02;
        
        ang += 0.6;
        if(ang > 360) ang = 0.0;
    }
    
    //Si cachas a BB-8 aparece el Boss
    if(catchBB8) {
        if(kyloLife > 0) {
            
            //Barra de Vida de Kylo
            glColor3f(0.9, 0.6, 0.0); glBegin(GL_QUADS);
            glVertex3f(posXkylo-kWid, posYkylo+(kWid/2) + 0.6, 2);
            glVertex3f((posXkylo+kWid)-(kWid*2)*(1.0-((GLfloat)kyloLife/10.0)), posYkylo+(kWid/2) + 0.6, 2);
            glVertex3f((posXkylo+kWid)-(kWid*2)*(1.0-((GLfloat)kyloLife/10.0)), posYkylo+(kWid/2) + 0.5, 2);
            glVertex3f(posXkylo-kWid, posYkylo+(kWid/2) + 0.5, 2);
            glEnd(); glColor3f(1, 1, 1);
            
            //Dibujo de la nave de Kylo
            if(posXkylo < -3.5) posXkylo += 0.01;
            else posXkylo += 0.02*sinf(ang);
            
            if(posYkylo < posY) posYkylo += 0.005;
            if(posYkylo > posY) posYkylo -= 0.005;
            
            glPushMatrix();
            glTranslatef(posXkylo, posYkylo, 0);
            glRotatef(90, 0, 1, 0);
            if(showColiders) glutWireSphere(kWid, 10, 10);
            if(!superLaserReady) {
                glPushMatrix(); glColor3f(0.2, 0.5, 1);
                glRotatef(ang*100, 1, 1, 1);
                glutWireSphere(kWid+0.2, 10, 10);
                glColor3f(1, 1, 1); glPopMatrix();
            }
            glScalef(0.2, 0.2, 0.2);
            
            glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
            glmDraw(kyloRen, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
            glDisable(GL_LIGHTING); glDisable(GL_LIGHT0);
            glPopMatrix();
            
            //Dibujo de las balas de Kylo
            if(rand()%300 > 295) {
                balasKylo.push_back(*nuevaEstructura(posXkylo, posYkylo));
            }
            
            for(int i=0; i<balasKylo.size(); i++){
                
                if(balasKylo[i].xStruct > 4) balasKylo[i].activo = false;
                
                if(balasKylo[i].activo) {
                    glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[7]);
                    glBegin(GL_QUADS);
                    glTexCoord2f(1.0,0.0); glVertex3f(balasKylo[i].xStruct -0.2, balasKylo[i].yStruct -0.1, 1);
                    glTexCoord2f(1.0,1.0); glVertex3f(balasKylo[i].xStruct -0.2, balasKylo[i].yStruct +0.1, 1);
                    glTexCoord2f(0.0,1.0); glVertex3f(balasKylo[i].xStruct +0.2, balasKylo[i].yStruct +0.1, 1);
                    glTexCoord2f(0.0,0.0); glVertex3f(balasKylo[i].xStruct +0.2, balasKylo[i].yStruct -0.1, 1);
                    glEnd(); glDisable(GL_TEXTURE_2D);
                }
                
                balasKylo[i].xStruct += momentoBala;
            }
            
            ang += 0.01;
            if(ang > 360) ang = 0.0;
        
        } else {
            //pasar a superWin después de un cierto tiempo
            if(frames > 500) estadoJuego = 4;
        }
    }
    
    (frames > 1000000000) ? frames=0 : frames++;
    if(intro) glTranslatef(introX, 0.0, 0.0);
    drawTrinchera( intro ? false : (pausa ? false : true) );
    if(!pausa && !intro) colisionamos();
    if(!pausa) angRotaAsteroide += 2;
}

//Método que calcula las colisiones con los asteroides y enemigos
void colisionamos() {
    GLfloat distance,xDif,yDif;
    
    //Colisión con los PowerUps
    if(mostrarPowerUp) {
        xDif = (posX - xPowerUp);
        yDif = (posY - yPowerUp);
        distance = sqrt((xDif*xDif) + (yDif*yDif));
        
        if(distance < xWid + 0.5) {
            if(municiones < 50) municiones += 15;
            mostrarPowerUp = false;
            
            xPowerUp = -2 + (double)(rand()%40)/10.0;
            yPowerUp = -1 + (double)(rand()%30)/10.0;
        }
    }
    
    if(mostrarPowerHealth) {
        xDif = (posX - xPowerHealth);
        yDif = (posY - yPowerHealth);
        distance = sqrt((xDif*xDif) + (yDif*yDif));
        
        if(distance < xWid + 0.5) {
            if(vida < 10.0) vida += 1.0;
            mostrarPowerHealth = false;
            
            xPowerHealth = -2 + (double)(rand()%40)/10.0;
            yPowerHealth = -1 + (double)(rand()%30)/10.0;
        }
    }
    
    //Roces de la nave con las paredes
    if(posY > 1.3 || posY < -1.3) {
        if(frames%25 == 0) {
            if(vida > 0) vida -= 0.2;
        }
        
        //Sprites de roces:
        //GLint tmp = (posY>1.3)? 1 : -1;
        if(vida >  0){
            glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D,texturas[5]);
            glBegin(GL_QUADS);
            glTexCoord2f(0.5+(double)frames/6.0, 0.0); glVertex3f(posX +0.45, posY -0.25, 1);
            glTexCoord2f(0.5+(double)frames/6.0, 1.0); glVertex3f(posX +0.45, posY +0.5, 1);
            glTexCoord2f(0.0+(double)frames/6.0, 1.0); glVertex3f(posX -0.3, posY +0.5, 1);
            glTexCoord2f(0.0+(double)frames/6.0, 0.0); glVertex3f(posX -0.3, posY -0.25, 1);
            glEnd(); glDisable(GL_TEXTURE_2D);
        }
    }
    
    //Colisiones con la nave y los asteroides
    if(asteroides.size() > 0) {
        for(int i=0; i<asteroides.size(); i++) {
            if(asteroides[i].activo) {
                xDif = (asteroides[i].xStruct - posX);
                yDif = (asteroides[i].yStruct - posY);
                distance = sqrt((xDif*xDif) + (yDif*yDif));
                
                if(distance < (xWid+aWid)) {
                    asteroides[i].activo = false;
                    numAsteroides--;
                    vida = 0;
                }
                
                //Colisión con el superLaser:
                if(!superLaserReady) {
                    yDif = (asteroides[i].yStruct - posY);
                    distance = sqrt(yDif*yDif);
                    
                    if(distance < aWid+0.1) {
                        asteroides[i].activo = false;
                        numAsteroides--;
                        Estructura explosion = *nuevaEstructura(asteroides[i].xStruct, asteroides[i].yStruct);
                        //En el caso de las explosiones la vida es el framerate
                        explosion.vida = 0;
                        booms.push_back(explosion);
                    }
                }
            }
        }
    }
    
    //Colisiones con la nave y las naves enemigas
    if(enemigos.size() > 0) {
        for(int i=0; i<enemigos.size(); i++) {
            if(enemigos[i].activo) {
                
                //Colisión de la nave con el enemigo = muerte
                xDif = (enemigos[i].xStruct - posX);
                yDif = (enemigos[i].yStruct - posY);
                distance = sqrt((xDif*xDif) + (yDif*yDif));
                
                if(distance < (xWid+tWid)) {
                    enemigos[i].activo = false;
                    enemigos[i].vida = 0;
                    numEnemigos--;
                    vida = 0;
                }
                
                //Colisión de la nave con la bala del enemigo = bajar vida
                xDif = (enemigos[i].xBala - posX);
                yDif = (enemigos[i].yBala - posY);
                distance = sqrt((xDif*xDif) + (yDif*yDif));
                
                if(distance < xWid) {
                    vida -= 0.5;
                    enemigos[i].xBala = -5;
                }
                
                
                //Colisión con el superLaser:
                if(!superLaserReady) {
                    yDif = (enemigos[i].yStruct - posY);
                    distance = sqrt(yDif*yDif);
                    
                    if(distance < tWid+0.1) {
                        enemigos[i].activo = false;
                        enemigos[i].vida = 0;
                        numEnemigos--;
                        score++;
                        Estructura explosion = *nuevaEstructura(enemigos[i].xStruct, enemigos[i].yStruct);
                        //En el caso de las explosiones la vida es el framerate
                        explosion.vida = 0;
                        booms.push_back(explosion);
                    }
                }
                
            }
        }
    }
    
    //Colisiones de las balas con las naves enemigas
    if(balas.size() > 0) {
        for(int j=0; j<enemigos.size(); j++) {
            if(enemigos[j].activo) {
                for(int i=0; i<balas.size(); i++) {
                    if(balas[i].activo) {
                        xDif = (enemigos[j].xStruct - balas[i].xStruct);
                        yDif = (enemigos[j].yStruct - balas[i].yStruct);
                        distance = sqrt((xDif*xDif) + (yDif*yDif));
                        
                        if(distance < (tWid)) {
                            balas[i].activo = false;
                        
                            if(enemigos[j].vida > 1) {
                                enemigos[j].vida--;
                            
                            } else {
                                enemigos[j].activo = false;
                                score++;
                                numEnemigos--;
                                Estructura explosion = *nuevaEstructura(enemigos[j].xStruct, enemigos[j].yStruct);
                                //En el caso de las explosiones la vida es el framerate
                                explosion.vida = 0;
                                booms.push_back(explosion);
                            }
                        }
                    }
                }
            }
        }
    }
    
    //Colisiones del misil con los asteroides
    if(asteroides.size() > 0 && !misilReady) {
        for(int i=0; i<asteroides.size(); i++) {
            if(asteroides[i].activo) {
                xDif = (asteroides[i].xStruct - posXmisil);
                yDif = (asteroides[i].yStruct - posYmisil);
                distance = sqrt((xDif*xDif) + (yDif*yDif));
                
                if(distance < (xWid+aWid)) {
                    asteroides[i].activo = false;
                    numAsteroides--;
                    misilReady = true;
                }
            }
        }
    }
    
    //Colisiones del misil con los enemigos
    if(enemigos.size() > 0) {
        for(int j=0; j<enemigos.size(); j++) {
            if(enemigos[j].activo) {
                if(!misilReady) {
                    xDif = (enemigos[j].xStruct - posXmisil);
                    yDif = (enemigos[j].yStruct - posYmisil);
                    distance = sqrt((xDif*xDif) + (yDif*yDif));
                    
                    if(distance < (xWid+tWid)) {
                        enemigos[j].activo = false;
                        enemigos[j].vida = 0;
                        numEnemigos--;
                        misilReady = true;
                        score++;
                        Estructura explosion = *nuevaEstructura(enemigos[j].xStruct, enemigos[j].yStruct);
                        //En el caso de las explosiones la vida es el framerate
                        explosion.vida = 0;
                        booms.push_back(explosion);
                    }
                }
            }
        }
    }
    
    //Verifica que caches a BB-8
    if(score > 23) {
        xDif = (slideBB8 - posX);
        yDif = (0.0 - posY);
        distance = sqrt((xDif*xDif) + (yDif*yDif));
        
        if(distance < xWid) {
            catchBB8 = true;
            ang = 0.0;
        }
    }
    
    //Colisiones del Boss
    if(catchBB8) {
        
        //Colisiones con la nave de KyloRen
        if(kyloLife > 0) {
            xDif = (posXkylo - posX);
            yDif = (posYkylo - posY);
            distance = sqrt((xDif*xDif) + (yDif*yDif));
            
            if(distance < xWid+kWid) vida = 0;
        }
        
        //Colisiones con las balas de KyloRen
        if(balasKylo.size() > 0) {
            for(int i=0; i<balasKylo.size(); i++) {
                if(balasKylo[i].activo) {
                    xDif = (balasKylo[i].xStruct - posX);
                    yDif = (balasKylo[i].yStruct - posY);
                    distance = sqrt((xDif*xDif) + (yDif*yDif));
                            
                    if(distance < xWid) {
                        balasKylo[i].activo = false;
                        vida -= 2;
                    }
                }
            }
        }
        
        //Colisiones de mis balas con Kylo Ren
        if(balas.size() > 0) {
            for(int i=0; i<balas.size(); i++) {
                if(balas[i].activo) {
                    xDif = (posXkylo - balas[i].xStruct);
                    yDif = (posYkylo - balas[i].yStruct);
                    distance = sqrt((xDif*xDif) + (yDif*yDif));
                    
                    if(distance < (kWid)) {
                        if(kyloLife > 0) {
                            balas[i].activo = false;
                            kyloLife -= 0.1;
                        
                        } else {
                            frames = 0;
                            
                            Estructura explosion = *nuevaEstructura(posYkylo, posYkylo);
                            //En el caso de las explosiones la vida es el framerate
                            explosion.vida = 0;
                            booms.push_back(explosion);
                        }
                    }
                }
            }
        }
        
        //Colisiones con el misil
        if(!misilReady) {
            xDif = (posXkylo - posXmisil);
            yDif = (posYkylo - posYmisil);
            distance = sqrt((xDif*xDif) + (yDif*yDif));
            
            if(distance < (kWid)) {
                if(kyloLife > 0) {
                    misilReady = true;
                    kyloLife -= 0.0005;
                
                } else {
                    frames = 0;
                    
                    Estructura explosion = *nuevaEstructura(posYkylo, posYkylo);
                    //En el caso de las explosiones la vida es el framerate
                    explosion.vida = 0;
                    booms.push_back(explosion);
                }
            }
        }
    }
}

//Método que genera a los enemigos
void generaMalos() {
    //Vieja implementación basada en no sé que se me ocurrió pero hacía muy dificil el juego
    /*
    if(vida == 0) return;
    if(numEnemigos > dificultad) return;
    if(numAsteroides > dificultad*2) return;
    
    GLint ySide = rand()%10;
    ySide = (ySide > 5) ? -1 : 1;
    
    for(int i=0; i<dificultad; i++) {
        asteroides.push_back(*nuevoEnemigo(3.5, -1.5 + 3*(double)(rand()%10)/10,
                                           -0.1 * ((double)rand())/RAND_MAX,
                                           ySide * 0.01 * ((double)rand())/RAND_MAX));
        numAsteroides++;
    }
    
    for(int i=0; i<dificultad; i++) {
        enemigos.push_back(*nuevoEnemigo(3.5, -1.5 + 3*(double)(rand()%10)/10,
                                         -0.1 * ((double)rand())/RAND_MAX,
                                         -ySide * 0.01 * ((double)rand())/RAND_MAX));
        numEnemigos++;
    }
    */
    
    //Nueva implementación basada en frames y el nivel de dificultad:
    if(vida > 0 && (frames%100 > 98)) {
        GLint randm = rand()%10;
        randm = (randm > 5) ? -1 : 1;
        
        if(rand()%100 < 40+(dificultad*2)) {
            asteroides.push_back(*nuevoEnemigo(3.5, -1.5 + 3*(double)(rand()%10)/10,
                                               -0.1 * ((double)rand())/RAND_MAX,
                                               randm * 0.01 * ((double)rand())/RAND_MAX));
        }
        
        if(rand()%100 < 40+(dificultad*2)) {
            enemigos.push_back(*nuevoEnemigo(3.5, -1.5 + 3*(double)(rand()%10)/10,
                                             -0.1 * ((double)rand())/RAND_MAX,
                                             -randm * 0.01 * ((double)rand())/RAND_MAX));
        }
    }
}

//Método que dibuja la HUD
void drawHUD() {
    glPushMatrix();
    
    glColor3f(0, 0, 0);
    mostrarTexto(-3.8, 2, font, "Municiones");
    char s[14] = "UliPoints    ";
    s[12] = '0' + (score%10);
    s[11] = '0' + ((score/10)%10);
    s[10] = '0' + ((score/100)%10);
    mostrarTexto(3, 2, font, s);
    char t[14] = "Dificultad   ";
    t[12] = '0' + (dificultad%10);
    t[11] = '0' + ((dificultad/10)%10);
    mostrarTexto(3, -2.1, font, t);
    mostrarTexto(-3.8, -2.1, font, "Health");
    
    //Bara de Municiones
    glColor3f(0.0, 0.7, 0.8);
    glBegin(GL_QUADS);
    glVertex3f(-3, 2.15, 4);
    glVertex3f(-3 + ((GLfloat)municiones/20), 2.15, 4);
    glVertex3f(-3 + ((GLfloat)municiones/20), 1.95, 4);
    glVertex3f(-3, 1.95, 4);
    glEnd();
    
    //Barra de Vida
    if(vida > 0) {
        glColor3f(1-vida/10, vida/10, 0.2);
        glBegin(GL_QUADS);
        glVertex3f(-3, -2.15, 4);
        glVertex3f(-3 + ((GLfloat)vida/2), -2.15, 4);
        glVertex3f(-3 + ((GLfloat)vida/2), -1.95, 4);
        glVertex3f(-3, -1.95, 4);
        glEnd();
    }
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

//Método que dibuja la trinchera
void drawTrinchera(bool paralax) {
    glPushMatrix();
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,2);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0,  2.3, -3.0);
    glTexCoord2f(1.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0,  2.3, -3.0);
    glTexCoord2f(0.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0, 1.8, -3.0);
    glTexCoord2f(0.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0, 1.8, -3.0);
    
    glTexCoord2f(1.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0,  -1.8, -3.0);
    glTexCoord2f(1.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0,  -1.8, -3.0);
    glTexCoord2f(0.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0, -2.3, -3.0);
    glTexCoord2f(0.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0, -2.3, -3.0);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D,3);
    glBegin(GL_QUADS);
    glTexCoord2f(1.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0,  1.8, -3.0);
    glTexCoord2f(1.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0,  1.8, -3.0);
    glTexCoord2f(0.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0, 1.3, -3.0);
    glTexCoord2f(0.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0, 1.3, -3.0);
    
    glTexCoord2f(1.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0,  -1.3, -3.0);
    glTexCoord2f(1.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0,  -1.3, -2.0);
    glTexCoord2f(0.0,paralax ? 1.0+maintextureSlide*2 : 1.0); glVertex3f( 4.0, -1.8, -3.0);
    glTexCoord2f(0.0,paralax ? 0.0+maintextureSlide*2 : 0.0); glVertex3f(-4.0, -1.8, -3.0);
    glEnd();
    
    glBindTexture(GL_TEXTURE_2D,texturas[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(!paralax ? 1.0 : 1.0-maintextureSlide*4, 1.0); glVertex3f(-4.0,  -1.0, -2.0);
    glTexCoord2f(!paralax ? 0.0 : 0.0-maintextureSlide*4, 1.0); glVertex3f( 4.0,  -1.0, -2.0);
    glTexCoord2f(!paralax ? 0.0 : 0.0-maintextureSlide*4, 0.0); glVertex3f( 4.0, -2.5, -2.0);
    glTexCoord2f(!paralax ? 1.0 : 1.0-maintextureSlide*4, 0.0); glVertex3f(-4.0, -2.5, -2.0);
        
    glTexCoord2f(!paralax ? 1.0 : 1.0-maintextureSlide*4, 0.0); glVertex3f(-4.0, 2.5, -2.0);
    glTexCoord2f(!paralax ? 0.0 : 0.0-maintextureSlide*4, 0.0); glVertex3f( 4.0, 2.5, -2.0);
    glTexCoord2f(!paralax ? 0.0 : 0.0-maintextureSlide*4, 1.0); glVertex3f( 4.0, 1.0, -2.0);
    glTexCoord2f(!paralax ? 1.0 : 1.0-maintextureSlide*4, 1.0); glVertex3f(-4.0, 1.0, -2.0);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

//Método que dibuja la flechita de selección
void DrawFlechita() {
    glColor3f(0.29, 0.50, 0.81);
    
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0, 0.0, 0.5);
    glVertex3f(0.4, 0.3, 0.5);
    glVertex3f(0.4, -0.3, 0.5);
    glEnd();
    glColor3f(1, 1, 1);
}

//Método que dibuja un botón en la pantalla
void DrawButton(char *texto, GLfloat x, GLfloat y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    
    glColor3f(0.95,0.95,0.95);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-0.8, 0.2, 0.0);
    glNormal3f(0, 0, 1);
    glVertex3f(0.8, 0.2, 0.0);
    glNormal3f(0, 0, 1);
    glVertex3f(0.8, -0.2, 0.0);
    glNormal3f(0, 0, 1);
    glVertex3f(-0.8, -0.2, 0.0);
    glEnd();
    
    glTranslatef(0, 0, 0.5);
    glColor3f(0.0,0.10,0.15);
    mostrarTexto(-0.6, -0.05, (void *) font, texto);
    glPopMatrix();
    
    glColor3f(1, 1, 1);
}

//Método que maneja el despliegue de Texto
void mostrarTexto(float x, float y, void *font,const char *string) {
    const char *c;
    glRasterPos2f(x,y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

//Teclas para la interacción con los menús
void SpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            if(estadoJuego==0) if(posicionFlecha > 0) posicionFlecha--;
            break;
            
        case GLUT_KEY_DOWN:
            if(estadoJuego==0) if(posicionFlecha < 2) posicionFlecha++;
            break;
    }
}

//Teclas para la interacción con el juego
void Keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            moveUp = true;
            break;
        case 'a':
            moveLeft = true;
            break;
        case 's':
            moveDown = true;
            break;
        case 'd':
            moveRight = true;
            break;
            
        case 'm':
            if(misilReady && municiones>=10) {
                posXmisil = posX;
                posYmisil = posY;
                municiones -= 10;
                misilReady = false;
            }
            break;
            
        case 'l':
            if(municiones >= 25){
                superLaserReady = false;
                municiones -= 25;
                if(vida > 2) vida -= 2.0;
            }
            break;
            
        case 'p':
            if(!intro) pausa = !pausa;
            break;
        
        case ' ':
            if(municiones > 0) {
                municiones--;
                balas.push_back(*nuevaEstructura(posX, posY));
            }
            break;
        
        case 13: //Enter
            if(estadoJuego == -1) {
                //no hacer nada?
            } else if(estadoJuego == 0) {
                if(posicionFlecha == 0) preparaJuego();   //nuevo Juego
                if(posicionFlecha == 1) estadoJuego = 1;  //pantallaControles
                if(posicionFlecha == 2) estadoJuego = 2;  //salir del juego
            } else {
                estadoJuego = 0;
            }
            break;
            
            
        //Casos para Debug:
        case 'o':
            score += 10;
            break;
        case 'q':
            exit(0);
            break;
        case 'c':
            showColiders = !showColiders;
            break;
    }
}

//Eventos de múltiples teclas a la vez
void KeyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
            tiltXWing = 0.0;
            moveUp = false;
            break;
        case 'a':
            moveLeft = false;
            break;
        case 's':
            tiltXWing = 0.0;
            moveDown = false;
            break;
        case 'd':
            moveRight = false;
            break;
    }
}

void Reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-4, 4, -2.3, 2.3, -10, 10);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1024,576);
	glutCreateWindow("Uli Space Adventures");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
    
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(Keyboard);
    glutKeyboardUpFunc(KeyboardUp);
	
    Init();
	glutMainLoop();
	return 0;
}

