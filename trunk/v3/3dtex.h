#ifndef TEX3D_H
#define TEX3D_H

#define APP_TITLE "Test"
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

//This is the ASCII code for the escape key
#define ESCAPE 27

#define WIDTH 128
#define HEIGHT 128
#define DEPTH 128
#define BYTES_PER_TEXEL 3
#define LAYER(r)	(WIDTH * HEIGHT * r * BYTES_PER_TEXEL)
// 2->1 dimension mapping function
#define TEXEL2(s, t)	(BYTES_PER_TEXEL * (s * WIDTH + t))
// 3->1 dimension mapping function
#define TEXEL3(s, t, r)	(TEXEL2(s, t) + LAYER(r))

unsigned char *build_texture(void);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion( int x, int y);
void display();
void reshape(int w, int h);
void glInit (int w, int h);
void idle();
void countFPS(int value);


#endif
