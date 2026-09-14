#include <GL/glut.h>
#include <cmath>

#define PI 3.14159265



// ------------------------- animation state -------------------------

float wheelAngle  = 0;     // Obj012_Nagordola wheel rotation (degrees)
float chorkiAngle = 0;     // chorki spin (degrees)
float kiteTimer   = 0;     // common clock for swaying things
float birdX       = -50;   // Obj007_Birds fly left -> right
float sunY        = 330;   // sun rises from 330 up to 700

float kiteBoyX   = 350;    // kite boy stands still (no walking)
int   kiteBoyDir = 1;      // stays 1 - always faces right

float walkPos1 = 50;       // Obj023_Walker1: left -> right
float walkPos3 = 200;      // Obj025_Walker3: left -> right
float walkPos5 = 280;      // Obj027_Walker5: left -> right (balloon)
float walkPos6 = 480;      // Obj028_Walker6: left -> right (balloon)
float walkPos2 = 750;      // Obj024_Walker2: right -> left (balloon)
float walkPos4 = 600;      // Obj026_Walker4: right -> left (balloon)

float cloudX1 = 100;       // Obj003_Cloud1: left -> right
float cloudX3 = 350;       // Obj005_Cloud3: left -> right
float cloudX2 = 600;       // Obj004_Cloud2: right -> left
float cloudX4 = 780;       // Obj006_Cloud4: right -> left

bool  boatBoyActive = false;  // becomes true 5 seconds after start
float boatBoyX      = -60;    // boat boy walks in from the left



// ================== tiny "pen" helpers ==================

// filled circle
void circle(float cx, float cy, float r, int n = 24) {
    // plot n points around the center to fake a filled circle
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        float t = 2 * PI * i / n;
        glVertex2f(cx + r * cos(t), cy + r * sin(t));
    }
    glEnd();
}

// circle outline
void ring(float cx, float cy, float r, int n = 32) {
    // trace n points around the center to fake a circle outline
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++) {
        float t = 2 * PI * i / n;
        glVertex2f(cx + r * cos(t), cy + r * sin(t));
    }
    glEnd();
}

// centered text
void text(float cx, float cy, const char* s) {
    // measure the total width so the text can be centered
    int w = 0;
    for (const char* c = s; *c; c++)
        w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    // start half a width left of center, then stamp each letter
    glRasterPos2f(cx - w * 0.5f, cy);
    for (const char* c = s; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}


// ============================ Obj001_Background ============================

// sky + grass
void Obj001_Background() {
    // sky: light blue at the bottom, darker blue at the top
    glBegin(GL_QUADS);
    glColor3f(0.53, 0.81, 0.98);
    glVertex2f(0, 350); glVertex2f(800, 350);
    glColor3f(0.3, 0.65, 0.9);
    glVertex2f(800, 800); glVertex2f(0, 800);
    glEnd();

    // grass: green gradient below the sky
    glBegin(GL_QUADS);
    glColor3f(0.35, 0.72, 0.28);
    glVertex2f(0, 0); glVertex2f(800, 0);
    glColor3f(0.48, 0.82, 0.35);
    glVertex2f(800, 350); glVertex2f(0, 350);
    glEnd();
}

// shining sun with 12 line rays (Obj-002)
void Obj002_Sun() {
    // main disc
    glColor3f(1.0, 0.85, 0.1);
    circle(700, sunY, 40);

    // 12 rays as plain lines - no loop, no sin/cos
    float d1 = 45, q1 = 39.0, h1 = 22.5;   // inner-end steps
    float d2 = 60, q2 = 52.0, h2 = 30.0;   // outer-end steps
    glColor3f(1.0, 0.9, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(700 + d1, sunY); glVertex2f(700 + d2, sunY);  // ray 1 (right)
    glVertex2f(700 + q1, sunY + h1); glVertex2f(700 + q2, sunY + h2);  // ray 2
    glVertex2f(700 + h1, sunY + q1); glVertex2f(700 + h2, sunY + q2);  // ray 3
    glVertex2f(700, sunY + d1); glVertex2f(700, sunY + d2);  // ray 4 (top)
    glVertex2f(700 - h1, sunY + q1); glVertex2f(700 - h2, sunY + q2);  // ray 5
    glVertex2f(700 - q1, sunY + h1); glVertex2f(700 - q2, sunY + h2);  // ray 6
    glVertex2f(700 - d1, sunY); glVertex2f(700 - d2, sunY);  // ray 7 (left)
    glVertex2f(700 - q1, sunY - h1); glVertex2f(700 - q2, sunY - h2);  // ray 8
    glVertex2f(700 - h1, sunY - q1); glVertex2f(700 - h2, sunY - q2);  // ray 9
    glVertex2f(700, sunY - d1); glVertex2f(700, sunY - d2);  // ray 10 (bottom)
    glVertex2f(700 + h1, sunY - q1); glVertex2f(700 + h2, sunY - q2);  // ray 11
    glVertex2f(700 + q1, sunY - h1); glVertex2f(700 + q2, sunY - h2);  // ray 12
    glEnd();
    glLineWidth(1);
}


// ==================== clouds (3 circles each) ====================

// big cloud, drifts left -> right across the Obj002_Sun
void Obj003_Cloud1() {
    float x = cloudX1;
    // left puff, right puff, then the big top puff
    glColor3f(1, 1, 1);
    circle(x - 30, 700, 18);
    circle(x + 30, 700, 18);
    circle(x, 708, 26);
}

// medium cloud, drifts right -> left over the Obj002_Sun
void Obj004_Cloud2() {
    float x = cloudX2;
    // left puff, right puff, then the big top puff
    glColor3f(1, 1, 1);
    circle(x - 22, 745, 14);
    circle(x + 22, 745, 14);
    circle(x, 751, 20);
}

// small cloud, drifts left -> right under the Obj002_Sun
void Obj005_Cloud3() {
    float x = cloudX3;
    // left puff, right puff, then the big top puff
    glColor3f(1, 1, 1);
    circle(x - 18, 688, 11);
    circle(x + 18, 688, 11);
    circle(x, 693, 16);
}

// tiny cloud near the top, drifts right -> left
void Obj006_Cloud4() {
    float x = cloudX4;
    // left puff, right puff, then the big top puff
    glColor3f(1, 1, 1);
    circle(x - 15, 770, 9);
    circle(x + 15, 770, 9);
    circle(x, 774, 13);
}


// ================================ Obj007_Birds ================================

// flock of 5 gliding birds (Obj-007) (fixed wings, they never flap)
void Obj007_Birds() {
    float x = birdX;
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    // bird 1
    glVertex2f(x - 12, 686); glVertex2f(x, 680);
    glVertex2f(x, 680);      glVertex2f(x + 12, 686);
    // bird 2
    glVertex2f(x - 42, 706); glVertex2f(x - 30, 700);
    glVertex2f(x - 30, 700); glVertex2f(x - 18, 706);
    // bird 3
    glVertex2f(x - 57, 666); glVertex2f(x - 45, 660);
    glVertex2f(x - 45, 660); glVertex2f(x - 33, 666);
    // bird 4
    glVertex2f(x - 82, 696); glVertex2f(x - 70, 690);
    glVertex2f(x - 70, 690); glVertex2f(x - 58, 696);
    // bird 5
    glVertex2f(x - 102, 676); glVertex2f(x - 90, 670);
    glVertex2f(x - 90, 670);  glVertex2f(x - 78, 676);
    glEnd();
    glLineWidth(1);
}

// ============================== Obj051_Bunting ==============================
// Pohela Boishakh bunting (red / yellow / green / orange flags) across the sky.
// POSITION : rope from (0, 700) to (800, 700), 10 flags hanging 24 down.
void Obj032_Bunting() {
    // rope (drawn as short straight pieces so it sags in the middle)
    glColor3f(0.25, 0.22, 0.18);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(0, 706);   glVertex2f(80, 690);
    glVertex2f(80, 690);  glVertex2f(160, 682);
    glVertex2f(160, 682); glVertex2f(240, 678);
    glVertex2f(240, 678); glVertex2f(320, 676);
    glVertex2f(320, 676); glVertex2f(400, 675);
    glVertex2f(400, 675); glVertex2f(480, 676);
    glVertex2f(480, 676); glVertex2f(560, 678);
    glVertex2f(560, 678); glVertex2f(640, 682);
    glVertex2f(640, 682); glVertex2f(720, 690);
    glVertex2f(720, 690); glVertex2f(800, 706);
    glEnd();
    glLineWidth(1);

    // 10 flags
    glColor3f(0.9, 0.1, 0.1);   // flag 1 - red
    glBegin(GL_TRIANGLES);
    glVertex2f(20, 702); glVertex2f(56, 698); glVertex2f(38, 676);
    glEnd();
    glColor3f(0.98, 0.85, 0.12);   // flag 2 - yellow
    glBegin(GL_TRIANGLES);
    glVertex2f(96, 687); glVertex2f(132, 684); glVertex2f(114, 662);
    glEnd();
    glColor3f(0.1, 0.7, 0.2);   // flag 3 - green
    glBegin(GL_TRIANGLES);
    glVertex2f(176, 681); glVertex2f(212, 679); glVertex2f(194, 657);
    glEnd();
    glColor3f(0.98, 0.5, 0.1);   // flag 4 - orange
    glBegin(GL_TRIANGLES);
    glVertex2f(256, 677); glVertex2f(292, 676); glVertex2f(274, 654);
    glEnd();
    glColor3f(0.2, 0.4, 0.9);   // flag 5 - blue
    glBegin(GL_TRIANGLES);
    glVertex2f(336, 675); glVertex2f(372, 675); glVertex2f(354, 653);
    glEnd();
    glColor3f(0.9, 0.1, 0.1);   // flag 6 - red
    glBegin(GL_TRIANGLES);
    glVertex2f(416, 675); glVertex2f(452, 676); glVertex2f(434, 654);
    glEnd();
    glColor3f(0.98, 0.85, 0.12);   // flag 7 - yellow
    glBegin(GL_TRIANGLES);
    glVertex2f(496, 677); glVertex2f(532, 679); glVertex2f(514, 657);
    glEnd();
    glColor3f(0.1, 0.7, 0.2);   // flag 8 - green
    glBegin(GL_TRIANGLES);
    glVertex2f(576, 681); glVertex2f(612, 684); glVertex2f(594, 662);
    glEnd();
    glColor3f(0.98, 0.5, 0.1);   // flag 9 - orange
    glBegin(GL_TRIANGLES);
    glVertex2f(656, 687); glVertex2f(692, 690); glVertex2f(674, 668);
    glEnd();
    glColor3f(0.8, 0.2, 0.7);   // flag 10 - magenta
    glBegin(GL_TRIANGLES);
    glVertex2f(736, 698); glVertex2f(772, 702); glVertex2f(754, 680);
    glEnd();
}

// ============================ alpona floor art ============================

// red + white alpona near the nagordola
void Obj009_Alpona1() {
    float x = 150, y = 160, r = 35;
    // base: red disc, white ring, small red center
    glColor3f(0.85, 0.15, 0.1);
    circle(x, y, r);
    glColor3f(0.98, 0.98, 0.98);
    circle(x, y, r - 4);
    glColor3f(0.85, 0.15, 0.1);
    circle(x, y, r * 0.35);

    // ring dots - plain circle() calls, no loop, no sin/cos
    float d = r * 0.65;    // straight step
    float e = d * 0.7071;  // diagonal step

    glColor3f(0.85, 0.15, 0.1); circle(x + d, y, r * 0.2);       // right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + d, y, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x + e, y + e, r * 0.2);   // top-right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + e, y + e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x, y + d, r * 0.2);       // top dot
    glColor3f(0.98, 0.98, 0.98); circle(x, y + d, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - e, y + e, r * 0.2);   // top-left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - e, y + e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - d, y, r * 0.2);       // left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - d, y, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - e, y - e, r * 0.2);   // bottom-left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - e, y - e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x, y - d, r * 0.2);       // bottom dot
    glColor3f(0.98, 0.98, 0.98); circle(x, y - d, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x + e, y - e, r * 0.2);   // bottom-right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + e, y - e, r * 0.1);
}

// red + white alpona in the middle
void Obj010_Alpona2() {
    float x = 400, y = 140, r = 40;
    // base: red disc, white ring, small red center
    glColor3f(0.85, 0.15, 0.1);
    circle(x, y, r);
    glColor3f(0.98, 0.98, 0.98);
    circle(x, y, r - 4);
    glColor3f(0.85, 0.15, 0.1);
    circle(x, y, r * 0.35);

    // ring dots - plain circle() calls, no loop, no sin/cos
    float d = r * 0.65;    // straight step
    float e = d * 0.7071;  // diagonal step

    glColor3f(0.85, 0.15, 0.1); circle(x + d, y, r * 0.2);       // right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + d, y, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x + e, y + e, r * 0.2);   // top-right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + e, y + e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x, y + d, r * 0.2);       // top dot
    glColor3f(0.98, 0.98, 0.98); circle(x, y + d, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - e, y + e, r * 0.2);   // top-left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - e, y + e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - d, y, r * 0.2);       // left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - d, y, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - e, y - e, r * 0.2);   // bottom-left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - e, y - e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x, y - d, r * 0.2);       // bottom dot
    glColor3f(0.98, 0.98, 0.98); circle(x, y - d, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x + e, y - e, r * 0.2);   // bottom-right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + e, y - e, r * 0.1);
}

// red + white alpona near the house
void Obj011_Alpona3() {
    float x = 650, y = 150, r = 30;
    // base: red disc, white ring, small red center
    glColor3f(0.85, 0.15, 0.1);
    circle(x, y, r);
    glColor3f(0.98, 0.98, 0.98);
    circle(x, y, r - 4);
    glColor3f(0.85, 0.15, 0.1);
    circle(x, y, r * 0.35);

    // ring dots - plain circle() calls, no loop, no sin/cos
    float d = r * 0.65;    // straight step
    float e = d * 0.7071;  // diagonal step

    glColor3f(0.85, 0.15, 0.1); circle(x + d, y, r * 0.2);       // right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + d, y, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x + e, y + e, r * 0.2);   // top-right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + e, y + e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x, y + d, r * 0.2);       // top dot
    glColor3f(0.98, 0.98, 0.98); circle(x, y + d, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - e, y + e, r * 0.2);   // top-left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - e, y + e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - d, y, r * 0.2);       // left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - d, y, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x - e, y - e, r * 0.2);   // bottom-left dot
    glColor3f(0.98, 0.98, 0.98); circle(x - e, y - e, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x, y - d, r * 0.2);       // bottom dot
    glColor3f(0.98, 0.98, 0.98); circle(x, y - d, r * 0.1);
    glColor3f(0.85, 0.15, 0.1); circle(x + e, y - e, r * 0.2);   // bottom-right dot
    glColor3f(0.98, 0.98, 0.98); circle(x + e, y - e, r * 0.1);
}


// ============================== Obj012_Nagordola ==============================

// ferris wheel - spokes turn with wheelAngle
void Obj012_Nagordola() {
    float cx = 150, cy = 430, R = 115;

    // support legs
    glColor3f(0.25, 0.25, 0.25);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex2f(cx, cy); glVertex2f(cx - 75, 200);
    glVertex2f(cx, cy); glVertex2f(cx + 75, 200);
    glEnd();

    // wheel rim (outer circle only)
    glColor3f(0.85, 0.2, 0.1);
    glLineWidth(4);
    ring(cx, cy, R);

    // spokes - each one turns with the wheel
    float w = wheelAngle * PI / 180;
    glColor3f(0.9, 0.7, 0.1);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    for (int i = 0; i < 6; i++) {
        float a = i * PI / 6 + w;
        glVertex2f(cx + cos(a) * R, cy + sin(a) * R);
        glVertex2f(cx - cos(a) * R, cy - sin(a) * R);
    }
    glEnd();

    // hub
    glColor3f(0.2, 0.2, 0.2);
    circle(cx, cy, 14);

    // 6 hanging seats with triangle roofs (unrolled, no loop)

    // seat 1
    // hanging point on the rim (travels with the wheel)
    float a0 = w;
    float sx0 = cx + cos(a0) * R;
    float sy0 = cy + sin(a0) * R;
    // short bar hanging down from the rim
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(sx0, sy0); glVertex2f(sx0, sy0 - 18);
    glEnd();
    // square seat box
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(sx0 - 14, sy0 - 34); glVertex2f(sx0 + 14, sy0 - 34);
    glVertex2f(sx0 + 14, sy0 - 18); glVertex2f(sx0 - 14, sy0 - 18);
    glEnd();
    // triangle roof on top of the box
    glColor3f(0.2, 0.4, 0.9);
    glBegin(GL_TRIANGLES);
    glVertex2f(sx0, sy0 - 6);
    glVertex2f(sx0 - 18, sy0 - 18);
    glVertex2f(sx0 + 18, sy0 - 18);
    glEnd();

    // seat 2
    // hanging point on the rim (travels with the wheel)
    float a1 = PI / 3 + w;
    float sx1 = cx + cos(a1) * R;
    float sy1 = cy + sin(a1) * R;
    // short bar hanging down from the rim
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(sx1, sy1); glVertex2f(sx1, sy1 - 18);
    glEnd();
    // square seat box
    glColor3f(0.1, 0.7, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(sx1 - 14, sy1 - 34); glVertex2f(sx1 + 14, sy1 - 34);
    glVertex2f(sx1 + 14, sy1 - 18); glVertex2f(sx1 - 14, sy1 - 18);
    glEnd();
    // triangle roof on top of the box
    glColor3f(0.95, 0.5, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(sx1, sy1 - 6);
    glVertex2f(sx1 - 18, sy1 - 18);
    glVertex2f(sx1 + 18, sy1 - 18);
    glEnd();

    // seat 3
    // hanging point on the rim (travels with the wheel)
    float a2 = 2 * PI / 3 + w;
    float sx2 = cx + cos(a2) * R;
    float sy2 = cy + sin(a2) * R;
    // short bar hanging down from the rim
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(sx2, sy2); glVertex2f(sx2, sy2 - 18);
    glEnd();
    // square seat box
    glColor3f(0.9, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(sx2 - 14, sy2 - 34); glVertex2f(sx2 + 14, sy2 - 34);
    glVertex2f(sx2 + 14, sy2 - 18); glVertex2f(sx2 - 14, sy2 - 18);
    glEnd();
    // triangle roof on top of the box
    glColor3f(0.8, 0.2, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex2f(sx2, sy2 - 6);
    glVertex2f(sx2 - 18, sy2 - 18);
    glVertex2f(sx2 + 18, sy2 - 18);
    glEnd();

    // seat 4
    // hanging point on the rim (travels with the wheel)
    float a3 = PI + w;
    float sx3 = cx + cos(a3) * R;
    float sy3 = cy + sin(a3) * R;
    // short bar hanging down from the rim
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(sx3, sy3); glVertex2f(sx3, sy3 - 18);
    glEnd();
    // square seat box
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(sx3 - 14, sy3 - 34); glVertex2f(sx3 + 14, sy3 - 34);
    glVertex2f(sx3 + 14, sy3 - 18); glVertex2f(sx3 - 14, sy3 - 18);
    glEnd();
    // triangle roof on top of the box
    glColor3f(0.1, 0.7, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex2f(sx3, sy3 - 6);
    glVertex2f(sx3 - 18, sy3 - 18);
    glVertex2f(sx3 + 18, sy3 - 18);
    glEnd();

    // seat 5
    // hanging point on the rim (travels with the wheel)
    float a4 = 4 * PI / 3 + w;
    float sx4 = cx + cos(a4) * R;
    float sy4 = cy + sin(a4) * R;
    // short bar hanging down from the rim
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(sx4, sy4); glVertex2f(sx4, sy4 - 18);
    glEnd();
    // square seat box
    glColor3f(0.1, 0.7, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(sx4 - 14, sy4 - 34); glVertex2f(sx4 + 14, sy4 - 34);
    glVertex2f(sx4 + 14, sy4 - 18); glVertex2f(sx4 - 14, sy4 - 18);
    glEnd();
    // triangle roof on top of the box
    glColor3f(0.6, 0.2, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex2f(sx4, sy4 - 6);
    glVertex2f(sx4 - 18, sy4 - 18);
    glVertex2f(sx4 + 18, sy4 - 18);
    glEnd();

    // seat 6
    // hanging point on the rim (travels with the wheel)
    float a5 = 5 * PI / 3 + w;
    float sx5 = cx + cos(a5) * R;
    float sy5 = cy + sin(a5) * R;
    // short bar hanging down from the rim
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(sx5, sy5); glVertex2f(sx5, sy5 - 18);
    glEnd();
    // square seat box
    glColor3f(0.9, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(sx5 - 14, sy5 - 34); glVertex2f(sx5 + 14, sy5 - 34);
    glVertex2f(sx5 + 14, sy5 - 18); glVertex2f(sx5 - 14, sy5 - 18);
    glEnd();
    // triangle roof on top of the box
    glColor3f(0.9, 0.75, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(sx5, sy5 - 6);
    glVertex2f(sx5 - 18, sy5 - 18);
    glVertex2f(sx5 + 18, sy5 - 18);
    glEnd();
    glLineWidth(1);
}


// ============================= village house =============================

// village house with double door + two windows
void Obj013_House() {
    // wall
    glColor3f(0.82, 0.7, 0.53);
    glBegin(GL_QUADS);
    glVertex2f(630, 220); glVertex2f(770, 220);
    glVertex2f(770, 320); glVertex2f(630, 320);
    glEnd();

    // roof
    glColor3f(0.65, 0.25, 0.12);
    glBegin(GL_POLYGON);
    glVertex2f(610, 315); glVertex2f(790, 315);
    glVertex2f(750, 380); glVertex2f(650, 380);
    glEnd();

    // dark doorway between the two open panels
    glColor3f(0.15, 0.08, 0.04);
    glBegin(GL_QUADS);
    glVertex2f(680, 220); glVertex2f(715, 220);
    glVertex2f(715, 275); glVertex2f(680, 275);
    glEnd();

    // left door panel, slightly open
    glColor3f(0.5, 0.3, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(678, 220); glVertex2f(694, 224);
    glVertex2f(694, 271); glVertex2f(678, 275);
    glEnd();

    // right door panel, slightly open
    glBegin(GL_QUADS);
    glVertex2f(717, 220); glVertex2f(701, 224);
    glVertex2f(701, 271); glVertex2f(717, 275);
    glEnd();

    // plank lines on the panels
    glColor3f(0.35, 0.2, 0.1);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(686, 223); glVertex2f(686, 273);
    glVertex2f(709, 223); glVertex2f(709, 273);
    glEnd();

    // door handles
    glColor3f(0.95, 0.85, 0.3);
    circle(691, 248, 1.8, 8);
    circle(704, 248, 1.8, 8);

    // left window
    glColor3f(0.35, 0.2, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(640, 255); glVertex2f(665, 255);
    glVertex2f(665, 280); glVertex2f(640, 280);
    glEnd();

    // right window
    glBegin(GL_QUADS);
    glVertex2f(730, 255); glVertex2f(755, 255);
    glVertex2f(755, 280); glVertex2f(730, 280);
    glEnd();

    // window crosses
    glColor3f(0.95, 0.93, 0.85);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(652.5, 255); glVertex2f(652.5, 280);
    glVertex2f(640, 267.5); glVertex2f(665, 267.5);
    glVertex2f(742.5, 255); glVertex2f(742.5, 280);
    glVertex2f(730, 267.5); glVertex2f(755, 267.5);
    glEnd();
    glLineWidth(1);
}


// ====================== decorated tree (festival Obj014_Tree) ======================

// big tree with balloons and lights
void Obj014_Tree() {
    // trunk
    glColor3f(0.4, 0.25, 0.15);
    glBegin(GL_POLYGON);
    glVertex2f(370, 220); glVertex2f(430, 220);
    glVertex2f(420, 480); glVertex2f(380, 480);
    glEnd();

    // leaves: dark layer, then light layer
    glColor3f(0.12, 0.5, 0.18);
    circle(400, 580, 90);
    circle(330, 530, 75);
    circle(470, 530, 75);
    circle(360, 480, 65);
    circle(440, 480, 65);
    glColor3f(0.18, 0.62, 0.24);
    circle(400, 570, 75);
    circle(340, 520, 60);
    circle(460, 520, 60);

    // two long hanging balloons
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(310, 500); glVertex2f(290, 440);
    glVertex2f(490, 510); glVertex2f(510, 450);
    glEnd();
    glColor3f(0.9, 0.1, 0.1);
    circle(290, 440, 12);
    glColor3f(0.95, 0.85, 0.1);
    circle(510, 450, 12);

    // 4 branch balloons (static - no swaying)
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(330, 570); glVertex2f(310, 590);
    glVertex2f(470, 570); glVertex2f(490, 595);
    glVertex2f(365, 625); glVertex2f(355, 645);
    glVertex2f(440, 625); glVertex2f(450, 645);
    glEnd();
    glColor3f(0.8, 0.2, 0.7);
    circle(310, 600, 9);
    glColor3f(0.2, 0.4, 0.9);
    circle(490, 605, 9);
    glColor3f(0.95, 0.5, 0.1);
    circle(355, 655, 9);
    glColor3f(0.1, 0.7, 0.3);
    circle(450, 655, 9);

    // shiny dots on the leaves
    glColor3f(0.9, 0.1, 0.1);      circle(372, 548, 4);
    glColor3f(0.98, 0.85, 0.12);   circle(432, 545, 4);
    glColor3f(0.2, 0.4, 0.9);      circle(400, 500, 4);
    glColor3f(0.8, 0.2, 0.7);      circle(348, 500, 4);
    glColor3f(0.1, 0.7, 0.3);      circle(455, 500, 4);
    glColor3f(0.98, 0.98, 0.98);   circle(400, 590, 3.5);
    glColor3f(0.95, 0.5, 0.1);     circle(368, 625, 3.5);
    glColor3f(0.2, 0.6, 0.9);      circle(435, 628, 3.5);

    // fair lights zigzag on the trunk
    float lx[9] = { 378, 420, 381, 417, 383, 415, 385, 413, 387 };
    float ly[9] = { 445, 420, 395, 370, 345, 320, 295, 270, 245 };
    glColor3f(0.15, 0.35, 0.12);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    for (int i = 0; i < 8; i++) {
        glVertex2f(lx[i], ly[i]);
        glVertex2f(lx[i + 1], ly[i + 1]);
    }
    glEnd();
    // glowing bulbs at each zigzag corner
    glColor3f(1, 0.95, 0.3);
    for (int i = 0; i < 9; i++)
        circle(lx[i], ly[i], 2.5);
    glLineWidth(1);
}


// ==================== kite boy (boy + kite in one) ====================

// boy walking left <-> right, flying a swaying kite
void Obj015_KiteBoy() {
    float x = kiteBoyX;   // stands still (no walking)
    float y = 150;
    int d = kiteBoyDir;   // always faces right (no turning)

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // yellow shirt
    glColor3f(0.95, 0.85, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // yellow panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (one arm up holding the string)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10 * d, y + 32); glVertex2f(x - 16 * d, y + 20);
    glVertex2f(x + 10 * d, y + 32); glVertex2f(x + 16 * d, y + 45);
    glEnd();
    glLineWidth(1);

    // kite sways left <-> right, height fixed
    float kx = 400 + 80 * sin(kiteTimer);
    float ky = 650;

    // string from the raised hand to the kite
    float hx = x + 16 * d;
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2f(hx, y + 45); glVertex2f(kx, ky - 20);
    glEnd();

    // kite diamond: red left half, green right half
    float s = 22;
    glColor3f(0.95, 0.1, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(kx, ky + s); glVertex2f(kx - s, ky); glVertex2f(kx, ky - s);
    glEnd();
    glColor3f(0.1, 0.8, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(kx, ky + s); glVertex2f(kx + s, ky); glVertex2f(kx, ky - s);
    glEnd();

    // kite cross + tail
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(kx, ky + s); glVertex2f(kx, ky - s);
    glVertex2f(kx - s, ky); glVertex2f(kx + s, ky);
    glVertex2f(kx, ky - s);           glVertex2f(kx - 10, ky - s - 15);
    glVertex2f(kx - 10, ky - s - 15); glVertex2f(kx + 5, ky - s - 30);
    glVertex2f(kx + 5, ky - s - 30);  glVertex2f(kx - 8, ky - s - 45);
    glEnd();
    glLineWidth(1);
}


// ================== chorki boy (boy + chorki in one) ==================

// boy holding a spinning chorki (pinwheel)
void Obj016_ChorkiBoy() {
    float x = 300;
    float y = 35;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // red shirt
    glColor3f(0.85, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // red panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (one arm up holding the stick)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 45);
    glEnd();
    glLineWidth(1);

    // bamboo stick in the raised hand
    glColor3f(0.5, 0.35, 0.15);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(316, 80); glVertex2f(316, 125);
    glEnd();

    // 4 spinning triangle blades around (316, 133)
    // each corner is turned with rotation math:
    // turned_x = px * cos(a) - py * sin(a)
    // turned_y = px * sin(a) + py * cos(a)
    for (int i = 0; i < 4; i++) {
        float a = (chorkiAngle + i * 90) * PI / 180;
        float c = cos(a), s = sin(a);
        float ax = 316 + 5 * s,  ay = 133 - 5 * c;
        float bx = 316 - 5 * s,  by = 133 + 5 * c;
        float tx = 316 + 22 * c, ty = 133 + 22 * s;

        if (i == 0)      glColor3f(0.9, 0.1, 0.1);
        else if (i == 1) glColor3f(0.95, 0.5, 0.1);
        else if (i == 2) glColor3f(0.98, 0.85, 0.12);
        else             glColor3f(0.2, 0.4, 0.9);
        glBegin(GL_TRIANGLES);
        glVertex2f(ax, ay);
        glVertex2f(bx, by);
        glVertex2f(tx, ty);
        glEnd();
    }

    // hub pin triangle
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(316, 129.5);
    glVertex2f(312.5, 135.5);
    glVertex2f(319.5, 135.5);
    glEnd();
    glLineWidth(1);
}


// ==================== mangal shobhajatra masks ====================

// owl mask on a pole
void Obj017_OwlMask() {
    // pole
    glColor3f(0.5, 0.35, 0.15);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(80, 180); glVertex2f(80, 260);
    glEnd();

    // face
    glColor3f(0.95, 0.9, 0.1);
    circle(80, 285, 28);

    // ears
    glColor3f(0.9, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(58, 300); glVertex2f(72, 323); glVertex2f(78, 305);
    glVertex2f(102, 300); glVertex2f(88, 323); glVertex2f(82, 305);
    glEnd();

    // eyes
    glColor3f(1, 1, 1);
    circle(70, 289, 10);
    circle(90, 289, 10);
    glColor3f(0.1, 0.1, 0.1);
    circle(70, 289, 5);
    circle(90, 289, 5);

    // beak
    glColor3f(0.9, 0.2, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(75, 280);
    glVertex2f(85, 280);
    glVertex2f(80, 267);
    glEnd();
    glLineWidth(1);
}

// tiger mask on a pole
void Obj018_TigerMask() {
    // pole
    glColor3f(0.5, 0.35, 0.15);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(595, 150); glVertex2f(595, 240);
    glEnd();

    // face
    glColor3f(0.98, 0.55, 0.05);
    circle(595, 265, 30);

    // ears
    glColor3f(0.1, 0.1, 0.1);
    circle(573, 287, 9);
    circle(617, 287, 9);

    // stripes
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(573, 275); glVertex2f(585, 273);
    glVertex2f(617, 275); glVertex2f(605, 273);
    glVertex2f(595, 293); glVertex2f(595, 281);
    glEnd();

    // eyes
    glColor3f(1, 1, 1);
    circle(584, 267, 8);
    circle(606, 267, 8);
    glColor3f(0.1, 0.1, 0.1);
    circle(584, 267, 4);
    circle(606, 267, 4);

    // muzzle + nose
    glColor3f(0.95, 0.95, 0.95);
    circle(595, 252, 10);
    glColor3f(0.9, 0.1, 0.1);
    circle(595, 250, 4.5);
    glLineWidth(1);
}


// ==================== Obj012_Nagordola Obj019_Owner ====================

// Obj019_Owner in lungi, waving at the visitors
void Obj019_Owner() {
    // head
    glColor3f(0, 0, 0);
    circle(60, 203, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(60, 200, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(57, 201, 1.3);
    circle(63, 201, 1.3);

    // cream panjabi
    glColor3f(0.95, 0.92, 0.82);
    glBegin(GL_QUADS);
    glVertex2f(50, 177); glVertex2f(70, 177);
    glVertex2f(72, 192); glVertex2f(48, 192);
    glEnd();

    // lungi cloth
    glColor3f(0.55, 0.15, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(49, 155); glVertex2f(71, 155);
    glVertex2f(72, 177); glVertex2f(48, 177);
    glEnd();

    // lungi stripes
    glColor3f(0.85, 0.45, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(54, 156); glVertex2f(54, 176);
    glVertex2f(60, 156); glVertex2f(60, 176);
    glVertex2f(66, 156); glVertex2f(66, 176);
    glEnd();

    // feet
    glColor3f(0.95, 0.8, 0.65);
    circle(56, 156, 2, 8);
    circle(64, 156, 2, 8);

    // arms: left down, right raised waving
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(50, 187); glVertex2f(44, 175);
    glVertex2f(70, 187); glVertex2f(76, 200);
    glEnd();
    glLineWidth(1);
}


// =========================== shops and stalls ===========================

// Obj020_Tent shop with its own vendor, pots and ektara
void Obj020_Tent() {
    // ---- vendor behind the counter ----
    float x = 400, y = 250;

    // head: black hair circle with a skin face on top
    glColor3f(0, 0, 0);
    circle(x, y + 43, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 40, 8);

    // two black dot eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 41, 1.3);
    circle(x + 3, y + 41, 1.3);

    // red shirt (wider at the shoulders)
    glColor3f(0.85, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 15); glVertex2f(x + 10, y + 15);
    glVertex2f(x + 12, y + 34); glVertex2f(x - 12, y + 34);
    glEnd();

    // white pants below the shirt
    glColor3f(0.95, 0.95, 0.95);
    glBegin(GL_QUADS);
    glVertex2f(x - 11, y);      glVertex2f(x + 11, y);
    glVertex2f(x + 10, y + 18); glVertex2f(x - 10, y + 18);
    glEnd();

    // red sleeves reaching down to the counter
    glColor3f(0.85, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x - 11, y + 31); glVertex2f(x - 9, y + 33);
    glVertex2f(x - 18, y + 21); glVertex2f(x - 20, y + 19);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x + 9, y + 33);  glVertex2f(x + 11, y + 31);
    glVertex2f(x + 20, y + 19); glVertex2f(x + 18, y + 21);
    glEnd();

    // skin hands resting at the counter edge
    glColor3f(0.95, 0.8, 0.65);
    glBegin(GL_QUADS);
    glVertex2f(x - 20, y + 19); glVertex2f(x - 18, y + 21);
    glVertex2f(x - 24, y + 17); glVertex2f(x - 25, y + 14);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x + 18, y + 21); glVertex2f(x + 20, y + 19);
    glVertex2f(x + 25, y + 14); glVertex2f(x + 24, y + 17);
    glEnd();

    // ---- poles ----
    glColor3f(0.3, 0.18, 0.08);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(330, 220); glVertex2f(330, 310);
    glVertex2f(470, 220); glVertex2f(470, 310);
    glEnd();

    // ---- striped canopy: 8 triangles, no loop ----

    glColor3f(0.9, 0.15, 0.15);   // stripe 1
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(310, 310);
    glVertex2f(332.5, 310);
    glEnd();

    glColor3f(0.98, 0.85, 0.12);   // stripe 2
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(332.5, 310);
    glVertex2f(355, 310);
    glEnd();

    glColor3f(0.9, 0.15, 0.15);   // stripe 3
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(355, 310);
    glVertex2f(377.5, 310);
    glEnd();

    glColor3f(0.98, 0.85, 0.12);   // stripe 4
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(377.5, 310);
    glVertex2f(400, 310);
    glEnd();

    glColor3f(0.9, 0.15, 0.15);   // stripe 5
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(400, 310);
    glVertex2f(422.5, 310);
    glEnd();

    glColor3f(0.98, 0.85, 0.12);   // stripe 6
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(422.5, 310);
    glVertex2f(445, 310);
    glEnd();

    glColor3f(0.9, 0.15, 0.15);   // stripe 7
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(445, 310);
    glVertex2f(467.5, 310);
    glEnd();

    glColor3f(0.98, 0.85, 0.12);   // stripe 8
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 360);
    glVertex2f(467.5, 310);
    glVertex2f(490, 310);
    glEnd();

    // ---- counter ----
    glColor3f(0.6, 0.35, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(325, 220); glVertex2f(475, 220);
    glVertex2f(475, 250); glVertex2f(325, 250);
    glEnd();

    // ---- clay pots on the counter ----
    glColor3f(0.8, 0.4, 0.15);
    circle(350, 258, 7);
    circle(375, 260, 9);
    circle(425, 260, 9);

    // ---- ektara on the counter ----
    // round drum: brown shell with a light skin top
    glColor3f(0.6, 0.3, 0.1);
    circle(455, 252, 10);
    glColor3f(0.95, 0.85, 0.7);
    circle(455, 252, 7);
    // bamboo neck: two slanted poles meeting at the top
    glColor3f(0.4, 0.2, 0.08);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(449, 252); glVertex2f(455, 292);
    glVertex2f(461, 252); glVertex2f(455, 292);
    glEnd();
    // single silver string up the middle
    glColor3f(0.9, 0.9, 0.9);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(455, 252); glVertex2f(455, 290);
    glEnd();
}

// street food Obj021_Cart with its own vendor + panta-ilish plate
void Obj021_Cart() {
    // ---- body ----
    glColor3f(0.7, 0.2, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(160, 205); glVertex2f(270, 205);
    glVertex2f(270, 235); glVertex2f(160, 235);
    glEnd();

    // ---- umbrella pole + umbrella ----
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(210, 235); glVertex2f(210, 275);
    glEnd();
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(210, 275);
    glVertex2f(165, 255);
    glVertex2f(255, 255);
    glEnd();

    // ---- wheels ----
    glColor3f(0.2, 0.2, 0.2);
    circle(180, 200, 15);
    circle(250, 200, 15);
    glColor3f(0.8, 0.8, 0.8);
    circle(180, 200, 10);
    circle(250, 200, 10);

    // ---- panta-ilish plate ----
    glColor3f(0.8, 0.4, 0.15);
    circle(190, 240, 10);
    glColor3f(0.95, 0.95, 0.9);
    circle(190, 240, 7);
    // small hilsha fish on the plate
    glColor3f(0.6, 0.65, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex2f(186, 240);
    glVertex2f(194, 243);
    glVertex2f(194, 237);
    glEnd();

    // ---- vendor in front of the Obj021_Cart ----
    float x = 240, y = 155;

    // head: black hair circle with a skin face on top
    glColor3f(0, 0, 0);
    circle(x, y + 43, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 40, 8);

    // two black dot eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 41, 1.3);
    circle(x + 3, y + 41, 1.3);

    // red shirt (wider at the shoulders)
    glColor3f(0.85, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 15); glVertex2f(x + 10, y + 15);
    glVertex2f(x + 12, y + 34); glVertex2f(x - 12, y + 34);
    glEnd();

    // white pants below the shirt
    glColor3f(0.95, 0.95, 0.95);
    glBegin(GL_QUADS);
    glVertex2f(x - 11, y);      glVertex2f(x + 11, y);
    glVertex2f(x + 10, y + 18); glVertex2f(x - 10, y + 18);
    glEnd();

    // red sleeves reaching down to the counter
    glColor3f(0.85, 0.2, 0.2);
    glBegin(GL_QUADS);
    glVertex2f(x - 11, y + 31); glVertex2f(x - 9, y + 33);
    glVertex2f(x - 18, y + 21); glVertex2f(x - 20, y + 19);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x + 9, y + 33);  glVertex2f(x + 11, y + 31);
    glVertex2f(x + 20, y + 19); glVertex2f(x + 18, y + 21);
    glEnd();

    // skin hands resting at the counter edge
    glColor3f(0.95, 0.8, 0.65);
    glBegin(GL_QUADS);
    glVertex2f(x - 20, y + 19); glVertex2f(x - 18, y + 21);
    glVertex2f(x - 24, y + 17); glVertex2f(x - 25, y + 14);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x + 18, y + 21); glVertex2f(x + 20, y + 19);
    glVertex2f(x + 25, y + 14); glVertex2f(x + 24, y + 17);
    glEnd();
    glLineWidth(1);
}

// balloon stall with 6 balloons (no seller)
void Obj022_BalloonStall() {
    // ---- bamboo poles ----
    glColor3f(0.5, 0.35, 0.15);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(493, 165); glVertex2f(493, 220);
    glVertex2f(557, 165); glVertex2f(557, 220);
    glEnd();

    // ---- counter ----
    glColor3f(0.6, 0.35, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(487, 205); glVertex2f(563, 205);
    glVertex2f(563, 220); glVertex2f(487, 220);
    glEnd();

    // ---- bunch of 6 balloons (static - no swaying, no loop) ----
    // 6 strings running from the counter top
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(525, 220); glVertex2f(501, 256);
    glVertex2f(525, 220); glVertex2f(517, 268);
    glVertex2f(525, 220); glVertex2f(533, 260);
    glVertex2f(525, 220); glVertex2f(549, 252);
    glVertex2f(525, 220); glVertex2f(511, 279);
    glVertex2f(525, 220); glVertex2f(537, 281);
    glEnd();
    // the 6 balloons: red green yellow blue magenta orange
    glColor3f(0.9, 0.1, 0.1);
    circle(501, 265, 9);
    glColor3f(0.1, 0.7, 0.2);
    circle(517, 277, 9);
    glColor3f(0.98, 0.85, 0.12);
    circle(533, 269, 9);
    glColor3f(0.2, 0.4, 0.9);
    circle(549, 261, 9);
    glColor3f(0.8, 0.2, 0.7);
    circle(511, 287, 8);
    glColor3f(0.95, 0.5, 0.1);
    circle(537, 289, 8);
}


// ==================== walkers (each boy = own function) ====================

// Obj023_Walker1: red shirt, orange balloon, walks left -> right
void Obj023_Walker1() {
    float x = walkPos1;
    float y = 112;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // red shirt
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // red panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (right arm up holding the balloon)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 45);
    glEnd();
    glLineWidth(1);

    // orange balloon in the raised hand
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(x + 16, y + 45); glVertex2f(x + 22, y + 85);
    glEnd();
    glColor3f(0.95, 0.5, 0.1);
    circle(x + 22, y + 95, 10);
}

// Obj024_Walker2: yellow shirt, purple balloon, walks right -> left
void Obj024_Walker2() {
    float x = walkPos2;
    float y = 110;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // yellow shirt
    glColor3f(0.95, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // yellow panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (facing left, left arm up holding the balloon)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 20);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 45);
    glEnd();
    glLineWidth(1);

    // purple balloon in the raised hand
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(x - 16, y + 45); glVertex2f(x - 22, y + 85);
    glEnd();
    glColor3f(0.6, 0.2, 0.8);
    circle(x - 22, y + 95, 10);
}

// Obj025_Walker3: blue shirt, no balloon, walks left -> right
void Obj025_Walker3() {
    float x = walkPos3;
    float y = 90;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // blue shirt
    glColor3f(0.1, 0.6, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // blue panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (both arms down)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 20);
    glEnd();
    glLineWidth(1);
}

// Obj026_Walker4: pink shirt, red balloon, walks right -> left
void Obj026_Walker4() {
    float x = walkPos4;
    float y = 70;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // pink shirt
    glColor3f(0.8, 0.2, 0.6);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // pink panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (facing left, left arm up holding the balloon)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 20);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 45);
    glEnd();
    glLineWidth(1);

    // red balloon in the raised hand
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(x - 16, y + 45); glVertex2f(x - 22, y + 85);
    glEnd();
    glColor3f(0.9, 0.1, 0.1);
    circle(x - 22, y + 95, 10);
}

// Obj027_Walker5: red shirt, red balloon, walks left -> right
void Obj027_Walker5() {
    float x = walkPos5;
    float y = 148;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // red shirt
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // red panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (right arm up holding the balloon)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 45);
    glEnd();
    glLineWidth(1);

    // red balloon in the raised hand
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(x + 16, y + 45); glVertex2f(x + 22, y + 85);
    glEnd();
    glColor3f(0.9, 0.1, 0.1);
    circle(x + 22, y + 95, 10);
}

// Obj028_Walker6: yellow shirt, cyan balloon, walks left -> right
void Obj028_Walker6() {
    float x = walkPos6;
    float y = 158;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // yellow shirt
    glColor3f(0.95, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // yellow panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (right arm up holding the balloon)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 45);
    glEnd();
    glLineWidth(1);

    // cyan balloon in the raised hand
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(x + 16, y + 45); glVertex2f(x + 22, y + 85);
    glEnd();
    glColor3f(0.2, 0.8, 0.9);
    circle(x + 22, y + 95, 10);
}

// standing visitor near the house, facing left
void Obj029_Visitor() {
    float x = 720;
    float y = 165;

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // pink shirt
    glColor3f(0.8, 0.2, 0.6);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // pink panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms (both arms down)
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 20);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glEnd();
    glLineWidth(1);
}


// ====================== boat boy + speech bubble ======================

// round speech bubble above the boat boy
void Obj030_SpeechBubble() {
    float x = boatBoyX;
    float y = 252;

    // white round bubble
    glColor3f(1, 1, 1);
    circle(x, y, 80, 64);
    // dark outline ring around the bubble
    glColor3f(0.15, 0.15, 0.15);
    glLineWidth(2.5);
    ring(x, y, 80, 64);
    glLineWidth(1);

    // two lines of text inside the bubble
    glColor3f(0.1, 0.1, 0.1);
    text(x, y + 4, "Let's go watch");
    text(x, y - 18, "the boat race!");
}

// boat-race boy: walks in, stops, then speaks
void Obj031_BoatBoy() {
    if (!boatBoyActive) return;   // hidden during the first 5 seconds
    float x = boatBoyX;
    float y = 100;

    bool speaking = (x >= 250);   // speaks after arriving

    // head
    glColor3f(0, 0, 0);
    circle(x, y + 48, 10);
    glColor3f(0.95, 0.8, 0.65);
    circle(x, y + 45, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(x - 3, y + 46, 1.3);
    circle(x + 3, y + 46, 1.3);

    // blue shirt
    glColor3f(0.2, 0.4, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(x - 10, y + 22);
    glVertex2f(x + 10, y + 22);
    glVertex2f(x + 12, y + 37);
    glVertex2f(x - 12, y + 37);
    glEnd();

    // blue panjabi
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 25);
    glVertex2f(x - 14, y + 8);
    glVertex2f(x + 14, y + 8);
    glEnd();

    // legs + arms
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y + 8); glVertex2f(x - 4, y);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 4, y);
    glVertex2f(x - 10, y + 32); glVertex2f(x - 16, y + 20);
    glVertex2f(x + 10, y + 32); glVertex2f(x + 16, y + 20);   // hand always down
    glEnd();
    glLineWidth(1);

    // show the speech bubble after arrival
    if (speaking)
        Obj030_SpeechBubble();
}


// =========================== display + animation ===========================

void display() {
    // clear the screen and reset the drawing position
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // sky, sun and ground first (everything else paints on top)
    Obj001_Background();
    Obj002_Sun();

    // clouds stay inside the sky (clipping)
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 350, 800, 450);   // clip rectangle = sky area only
    Obj003_Cloud1();
    Obj004_Cloud2();
    Obj005_Cloud3();
    Obj006_Cloud4();
    glDisable(GL_SCISSOR_TEST);

    // birds, bunting and the big rides
    Obj007_Birds();
    Obj032_Bunting();
    Obj012_Nagordola();
    Obj013_House();
    Obj014_Tree();

    // red + white floor art
    Obj009_Alpona1();
    Obj010_Alpona2();
    Obj011_Alpona3();

    // masks, people and shops in the middle ground
    Obj017_OwlMask();
    Obj019_Owner();
    Obj020_Tent();
    Obj021_Cart();
    Obj022_BalloonStall();
    Obj015_KiteBoy();
    Obj018_TigerMask();

    // walking visitors crossing the fair
    Obj027_Walker5();
    Obj028_Walker6();
    Obj029_Visitor();
    Obj023_Walker1();
    Obj025_Walker3();
    Obj024_Walker2();
    Obj026_Walker4();

    // chorki boy and boat boy in front
    Obj016_ChorkiBoy();
    Obj031_BoatBoy();

    // show the finished frame on the screen
    glutSwapBuffers();
}

// moves everything, runs every 16 ms
void update(int value) {
    // spin the ferris wheel, wrap back after a full turn
    wheelAngle += 1.2;
    if (wheelAngle >= 360) wheelAngle -= 360;

    // spin the chorki much faster
    chorkiAngle += 14;
    if (chorkiAngle >= 360) chorkiAngle -= 360;

    // fly the birds right, jump back to the left edge
    birdX += 2;
    if (birdX > 900) birdX = -100;

    // sun rises from behind the house and stops at 700
    if (sunY < 700) {
        sunY += 0.6;
        if (sunY > 700) sunY = 700;
    }

    // advance the common clock that sways the kite
    kiteTimer += 0.04;

    // kite boy stands still (no walking)

    // walkers
    walkPos1 += 0.75; if (walkPos1 > 850) walkPos1 = -50;
    walkPos3 += 0.6;  if (walkPos3 > 850) walkPos3 = -50;
    walkPos2 -= 0.7;  if (walkPos2 < -50) walkPos2 = 850;
    walkPos4 -= 0.55; if (walkPos4 < -50) walkPos4 = 850;

    // balloon walkers
    walkPos5 += 0.65; if (walkPos5 > 850) walkPos5 = -50;
    walkPos6 += 0.5;  if (walkPos6 > 850) walkPos6 = -50;

    // clouds
    cloudX1 += 0.15; if (cloudX1 > 900) cloudX1 = -100;
    cloudX3 += 0.10; if (cloudX3 > 900) cloudX3 = -100;
    cloudX2 -= 0.12; if (cloudX2 < -100) cloudX2 = 900;
    cloudX4 -= 0.08; if (cloudX4 < -100) cloudX4 = 900;

    // boat boy walks in from the left and stops at 250
    if (boatBoyActive && boatBoyX < 250) boatBoyX += 0.8;

    // boat boy's hand never moves (always down)

    // ask for a redraw, then run update() again in 16 ms
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// fires exactly ONCE, 5 seconds after the program starts
void startBoatBoy(int value) {
    boatBoyActive = true;
}

// one-time setup: background color + flat 2D view
void init() {
    // white background color
    glClearColor(1, 1, 1, 1);
    // flat 2D view: screen maps to 0..800 on both axes
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800);
    // back to normal drawing mode
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    // start GLUT and open the window
    glutInit(&argc, argv);
    // smooth double-buffered color window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Shubho Nabo Barsho - Festive Pohela Boishakh Fair");

    init();                  // set up the 2D view
    glutDisplayFunc(display);   // display() draws every frame
    glutTimerFunc(0, update, 0);           // repeating animation loop
    glutTimerFunc(5000, startBoatBoy, 0);  // boat boy enters after 5 seconds
    glutMainLoop();   // hand control to GLUT (runs forever)

    return 0;
}
