#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265


// ======================= ANIMATION VALUES (these move) ======================
float cloudX1   = 420;    // Obj003_Cloud1 : drifts left  -> right (y 700)
float cloudX2   = 180;    // Obj004_Cloud2 : drifts right -> left  (y 620)
float cloudX3   = 120;    // Obj005_Cloud3 : drifts left  -> right (y 752)
float cloudX4   = 300;    // Obj006_Cloud4 : drifts right -> left  (y 585)

float birdX     = -140;   // Obj007_Birds : flock flies left -> right
float birdTimer = 0;      // Obj007_Birds : wing flap
float dholAngle  = 0;     // Obj019_DholPlayer : drumstick swing (degrees)
float stageTimer = 0;     // lanterns, lamp flames, steam, balloons
bool  paused     = false; // p = freeze everything


// ============================ tiny "pen" helpers ============================

// filled circle
void circle(float cx, float cy, float r, int n = 24) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < n; i++) {
        float t = 2 * PI * i / n;
        glVertex2f(cx + r * cos(t), cy + r * sin(t));
    }
    glEnd();
}

// circle outline
void ring(float cx, float cy, float r, int n = 32) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++) {
        float t = 2 * PI * i / n;
        glVertex2f(cx + r * cos(t), cy + r * sin(t));
    }
    glEnd();
}

// centred text
void text(float cx, float cy, const char* s) {
    int w = 0;
    for (const char* c = s; *c; c++)
        w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *c);
    glRasterPos2f(cx - w * 0.5f, cy);
    for (const char* c = s; *c; c++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}


// ================================ Obj001_Sky ================================
// Evening sky - ONE flat warm colour, no gradient.
// POSITION : one quad from y = 300 (horizon) up to y = 800.
void Obj001_Sky() {
    glColor3f(0.98, 0.74, 0.50);
    glBegin(GL_QUADS);
    glVertex2f(0, 300);    glVertex2f(800, 300);
    glVertex2f(800, 800);  glVertex2f(0, 800);
    glEnd();
}


// ================================ Obj002_Sun ================================
// Evening sun - the whole sun uses ONE solid warm-yellow colour :
// glow rings, the disc and the 12 rays are all glColor3f(1.0, 0.82, 0.20).
// POSITION : centre (690, 560) ; disc radius 38, glow 54, rays out to 68.
//            Change the 690 to move it left / right, the 560 to move it up/down.
void Obj002_Sun() {
    // glow + disc (all the same colour)
    glColor3f(1.0, 0.82, 0.20);
    circle(690, 560, 54);
    circle(690, 560, 46);
    circle(690, 560, 38);
    circle(690, 560, 29);

    // 12 rays around the sun
    glLineWidth(3);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        float a = i * PI / 6;
        glVertex2f(690 + cos(a) * 50, 560 + sin(a) * 50);
        glVertex2f(690 + cos(a) * 70, 560 + sin(a) * 70);
    }
    glEnd();
    glLineWidth(1);
}


// =============================== Obj003_Cloud1 ==============================
// Big white cloud, drifts left -> right.
// POSITION : centre (cloudX1, 700).
void Obj003_Cloud1() {
    float x = cloudX1;
    glColor3f(1, 1, 1);
    circle(x - 34, 700, 19);
    circle(x, 712, 27);
    circle(x + 34, 700, 20);
    circle(x + 12, 694, 17);
    circle(x - 14, 694, 16);
}


// =============================== Obj004_Cloud2 ==============================
// Medium cloud, drifts right -> left.
// POSITION : centre (cloudX2, 620).
void Obj004_Cloud2() {
    float x = cloudX2;
    glColor3f(1, 1, 1);
    circle(x - 26, 620, 16);
    circle(x, 630, 23);
    circle(x + 26, 620, 16);
    circle(x + 10, 614, 14);
}


// =============================== Obj005_Cloud3 ==============================
// Small high cloud, drifts left -> right.
// POSITION : centre (cloudX3, 752).
void Obj005_Cloud3() {
    float x = cloudX3;
    glColor3f(1, 1, 1);
    circle(x - 18, 752, 13);
    circle(x, 759, 18);
    circle(x + 18, 752, 13);
}


// =============================== Obj006_Cloud4 ==============================
// Tiny cloud, drifts right -> left.
// POSITION : centre (cloudX4, 585).
void Obj006_Cloud4() {
    float x = cloudX4;
    glColor3f(1, 1, 1);
    circle(x - 13, 585, 9);
    circle(x, 591, 13);
    circle(x + 13, 585, 9);
}


// ================================ Obj007_Birds ==============================
// Birds going home across the sky; wings flap with birdTimer.
// POSITION : the flock starts at birdX (global), it flies at y 626..698.
void Obj007_Birds() {
    float x = birdX;
    float f = 5 * sin(birdTimer * 7);
    glColor3f(0.30, 0.26, 0.32);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(x, 660);       glVertex2f(x - 11, 667 + f);
    glVertex2f(x, 660);       glVertex2f(x + 11, 667 + f);
    glVertex2f(x - 36, 690);  glVertex2f(x - 47, 697 + f);
    glVertex2f(x - 36, 690);  glVertex2f(x - 25, 697 + f);
    glVertex2f(x - 64, 646);  glVertex2f(x - 75, 653 + f);
    glVertex2f(x - 64, 646);  glVertex2f(x - 53, 653 + f);
    glVertex2f(x - 98, 676);  glVertex2f(x - 109, 683 + f);
    glVertex2f(x - 98, 676);  glVertex2f(x - 87, 683 + f);
    glVertex2f(x - 130, 630); glVertex2f(x - 141, 637 + f);
    glVertex2f(x - 130, 630); glVertex2f(x - 119, 637 + f);
    glEnd();
    glLineWidth(1);
}


// =============================== Obj008_Ground ==============================
// The village field - ONE flat colour, plus one flat shadow strip under the
// stage and the two stalls (never black).
// POSITION : from y = 0 (front) up to y = 300 (horizon).
void Obj008_Ground() {
    glColor3f(0.42, 0.58, 0.32);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);      glVertex2f(800, 0);
    glVertex2f(800, 300);  glVertex2f(0, 300);
    glEnd();

    glColor3f(0.28, 0.42, 0.24);
    glBegin(GL_QUADS);
    glVertex2f(185, 238); glVertex2f(615, 238);
    glVertex2f(620, 250); glVertex2f(180, 250);
    glVertex2f(8, 232);   glVertex2f(184, 232);
    glVertex2f(188, 244); glVertex2f(4, 244);
    glVertex2f(612, 232); glVertex2f(798, 232);
    glVertex2f(802, 244); glVertex2f(608, 244);
    glEnd();
}


// ============================== Obj009_HouseLeft ============================
// Village house behind, left side.  Flat colours, lit windows, no silhouette.
// POSITION : wall x 12..80, base y = 296, roof from y 358 up to y 392.
void Obj009_HouseLeft() {
    glColor3f(0.88, 0.74, 0.54);
    glBegin(GL_QUADS);
    glVertex2f(12, 296); glVertex2f(80, 296);
    glVertex2f(80, 358); glVertex2f(12, 358);
    glEnd();

    glColor3f(0.62, 0.30, 0.20);
    glBegin(GL_POLYGON);
    glVertex2f(4, 358);  glVertex2f(88, 358);
    glVertex2f(72, 392); glVertex2f(20, 392);
    glEnd();

    glColor3f(0.44, 0.28, 0.14);
    glBegin(GL_QUADS);
    glVertex2f(16, 296); glVertex2f(32, 296);
    glVertex2f(32, 330); glVertex2f(16, 330);
    glEnd();

    glColor3f(1.0, 0.84, 0.42);
    glBegin(GL_QUADS);
    glVertex2f(40, 310); glVertex2f(56, 310);
    glVertex2f(56, 330); glVertex2f(40, 330);
    glVertex2f(62, 310); glVertex2f(76, 310);
    glVertex2f(76, 330); glVertex2f(62, 330);
    glEnd();

    glColor3f(0.55, 0.38, 0.20);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(48, 310); glVertex2f(48, 330);
    glVertex2f(40, 320); glVertex2f(56, 320);
    glVertex2f(69, 310); glVertex2f(69, 330);
    glVertex2f(62, 320); glVertex2f(76, 320);
    glEnd();
    glLineWidth(1);
}


// ============================= Obj010_HouseRight ============================
// Village house behind, right side.
// POSITION : wall x 642..716, base y = 296, roof from y 358 up to y 392.
void Obj010_HouseRight() {
    glColor3f(0.88, 0.74, 0.54);
    glBegin(GL_QUADS);
    glVertex2f(642, 296); glVertex2f(716, 296);
    glVertex2f(716, 358); glVertex2f(642, 358);
    glEnd();

    glColor3f(0.62, 0.30, 0.20);
    glBegin(GL_POLYGON);
    glVertex2f(634, 358); glVertex2f(724, 358);
    glVertex2f(708, 392); glVertex2f(650, 392);
    glEnd();

    glColor3f(0.44, 0.28, 0.14);
    glBegin(GL_QUADS);
    glVertex2f(690, 296); glVertex2f(706, 296);
    glVertex2f(706, 330); glVertex2f(690, 330);
    glEnd();

    glColor3f(1.0, 0.84, 0.42);
    glBegin(GL_QUADS);
    glVertex2f(648, 310); glVertex2f(662, 310);
    glVertex2f(662, 330); glVertex2f(648, 330);
    glVertex2f(694, 310); glVertex2f(708, 310);
    glVertex2f(708, 330); glVertex2f(694, 330);
    glEnd();

    glColor3f(0.55, 0.38, 0.20);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(655, 310); glVertex2f(655, 330);
    glVertex2f(648, 320); glVertex2f(662, 320);
    glVertex2f(701, 310); glVertex2f(701, 330);
    glVertex2f(694, 320); glVertex2f(708, 320);
    glEnd();
    glLineWidth(1);
}


// ============================== Obj011_TreeLeft =============================
// Round village tree behind, left side (clear of the house and the stage).
// POSITION : trunk base (135, 294), crown centre (135, 368), spread x 90..180.
void Obj011_TreeLeft() {
    glColor3f(0.44, 0.30, 0.18);
    glBegin(GL_POLYGON);
    glVertex2f(129, 294); glVertex2f(141, 294);
    glVertex2f(138, 342); glVertex2f(132, 342);
    glEnd();

    glColor3f(0.16, 0.42, 0.20);
    circle(135, 368, 36);
    circle(112, 350, 22);
    circle(158, 352, 22);
    circle(118, 386, 20);
    circle(152, 384, 20);

    glColor3f(0.26, 0.56, 0.26);
    circle(135, 374, 26);
    circle(116, 356, 15);
    circle(154, 358, 15);
}


// ============================= Obj012_TreeRight =============================
// Round village tree behind, right side.
// POSITION : trunk base (772, 294), crown centre (772, 366), spread x 725..800.
void Obj012_TreeRight() {
    glColor3f(0.44, 0.30, 0.18);
    glBegin(GL_POLYGON);
    glVertex2f(766, 294); glVertex2f(778, 294);
    glVertex2f(775, 340); glVertex2f(769, 340);
    glEnd();

    glColor3f(0.16, 0.42, 0.20);
    circle(772, 366, 34);
    circle(746, 350, 21);
    circle(792, 350, 21);

    glColor3f(0.26, 0.56, 0.26);
    circle(772, 372, 24);
    circle(750, 356, 14);
    circle(790, 356, 14);
}


// =========================== Obj014_StageBackdrop ===========================
// Decorated cloth at the back of the stage : flat burgundy cloth, gold border,
// a big alpona emblem in the middle and four hanging tassels.
// POSITION : cloth x 215..585, y 296..470 ; emblem centre (400, 410).
void Obj014_StageBackdrop() {
    glColor3f(0.55, 0.12, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(215, 296); glVertex2f(585, 296);
    glVertex2f(585, 470); glVertex2f(215, 470);
    glEnd();

    glColor3f(0.44, 0.09, 0.14);
    glBegin(GL_QUADS);
    glVertex2f(215, 296); glVertex2f(262, 296);
    glVertex2f(262, 470); glVertex2f(215, 470);
    glVertex2f(538, 296); glVertex2f(585, 296);
    glVertex2f(585, 470); glVertex2f(538, 470);
    glEnd();

    glColor3f(0.92, 0.76, 0.22);
    glLineWidth(4);
    glBegin(GL_LINE_LOOP);
    glVertex2f(215, 296); glVertex2f(585, 296);
    glVertex2f(585, 470); glVertex2f(215, 470);
    glEnd();
    glLineWidth(1);

    // ---------- alpona emblem (x 363..437) ----------
    glColor3f(0.96, 0.92, 0.80);
    circle(400, 410, 30);
    glColor3f(0.85, 0.15, 0.10);
    circle(400, 410, 25);
    glColor3f(0.96, 0.92, 0.80);
    circle(400, 410, 18);
    glColor3f(0.96, 0.92, 0.80);
    circle(400, 440, 7);  circle(400, 380, 7);
    circle(370, 410, 7);  circle(430, 410, 7);
    circle(379, 431, 7);  circle(421, 431, 7);
    circle(379, 389, 7);  circle(421, 389, 7);
    glColor3f(0.85, 0.15, 0.10);
    circle(400, 410, 10);
    glColor3f(0.10, 0.55, 0.25);
    circle(400, 410, 5);
    
}


// =========================== Obj015_StagePlatform ===========================
// The wooden stage : front face, floor, short steps, marigold garland.
// POSITION : front face x 185..615, y 245..285 ; floor y 285..296 ;
//            steps x 374..426, y 224..245.
void Obj015_StagePlatform() {
    glColor3f(0.58, 0.34, 0.16);
    glBegin(GL_QUADS);
    glVertex2f(185, 245); glVertex2f(615, 245);
    glVertex2f(615, 285); glVertex2f(185, 285);
    glEnd();

    glColor3f(0.72, 0.46, 0.22);
    glBegin(GL_QUADS);
    glVertex2f(195, 285); glVertex2f(605, 285);
    glVertex2f(615, 296); glVertex2f(185, 296);
    glEnd();

    // alpona panels painted on the front face
    glColor3f(0.92, 0.88, 0.78);
    circle(245, 265, 12);
    circle(325, 265, 12);
    circle(475, 265, 12);
    circle(555, 265, 12);
    glColor3f(0.85, 0.15, 0.10);
    circle(245, 265, 6);
    circle(325, 265, 6);
    circle(475, 265, 6);
    circle(555, 265, 6);

    // steps
    glColor3f(0.62, 0.38, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(380, 234); glVertex2f(420, 234);
    glVertex2f(420, 245); glVertex2f(380, 245);
    glEnd();
    glColor3f(0.50, 0.30, 0.14);
    glBegin(GL_QUADS);
    glVertex2f(374, 224); glVertex2f(426, 224);
    glVertex2f(426, 234); glVertex2f(374, 234);
    glEnd();
    
}


// ============================ Obj016_StagePillars ===========================
// Two decorated pillars, the red top beam with the greeting, the valance,
// the bunting and two plain golden lamps on the pillar tops.
// POSITION : pillars x 185..215 and 585..615, y 245..500 ;
//            beam x 185..615, y 470..496 ; greeting sits on the beam y 476.
void Obj016_StagePillars() {
    glColor3f(0.85, 0.72, 0.35);
    glBegin(GL_QUADS);
    glVertex2f(185, 245); glVertex2f(215, 245);
    glVertex2f(215, 500); glVertex2f(185, 500);
    glVertex2f(585, 245); glVertex2f(615, 245);
    glVertex2f(615, 500); glVertex2f(585, 500);
    glEnd();

    glColor3f(0.80, 0.12, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(185, 300); glVertex2f(215, 300);
    glVertex2f(215, 316); glVertex2f(185, 316);
    glVertex2f(185, 380); glVertex2f(215, 380);
    glVertex2f(215, 396); glVertex2f(185, 396);
    glVertex2f(585, 300); glVertex2f(615, 300);
    glVertex2f(615, 316); glVertex2f(585, 316);
    glVertex2f(585, 380); glVertex2f(615, 380);
    glVertex2f(615, 396); glVertex2f(585, 396);
    glEnd();

    // top beam
    glColor3f(0.72, 0.14, 0.16);
    glBegin(GL_QUADS);
    glVertex2f(185, 470); glVertex2f(615, 470);
    glVertex2f(615, 496); glVertex2f(185, 496);
    glEnd();
    glColor3f(0.92, 0.76, 0.22);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(185, 470); glVertex2f(615, 470);
    glVertex2f(185, 496); glVertex2f(615, 496);
    glEnd();
    glLineWidth(1);

    // the greeting written on the beam
    glColor3f(0.99, 0.94, 0.78);
    text(400, 476, "SHUBHO NOBO BORSHO  1433");

    // scalloped valance (centre left open)
    glColor3f(0.92, 0.76, 0.22);
    glBegin(GL_TRIANGLES);
    glVertex2f(220, 470); glVertex2f(256, 470); glVertex2f(238, 446);
    glVertex2f(280, 470); glVertex2f(316, 470); glVertex2f(298, 446);
    glVertex2f(340, 470); glVertex2f(376, 470); glVertex2f(358, 446);
    glVertex2f(424, 470); glVertex2f(460, 470); glVertex2f(442, 446);
    glVertex2f(484, 470); glVertex2f(520, 470); glVertex2f(502, 446);
    glVertex2f(544, 470); glVertex2f(580, 470); glVertex2f(562, 446);
    glEnd();
    glColor3f(0.90, 0.15, 0.10);
    circle(238, 443, 4, 8); circle(298, 443, 4, 8); circle(358, 443, 4, 8);
    circle(442, 443, 4, 8); circle(502, 443, 4, 8); circle(562, 443, 4, 8);
}


// ============================== Obj018_Speakers =============================
// Two speaker boxes at the sides of the stage floor.
// POSITION : left x 210..244, right x 556..590, both y 296..344.
void Obj018_Speakers() {
    glColor3f(0.26, 0.24, 0.26);
    glBegin(GL_QUADS);
    glVertex2f(210, 296); glVertex2f(244, 296);
    glVertex2f(244, 344); glVertex2f(210, 344);
    glVertex2f(556, 296); glVertex2f(590, 296);
    glVertex2f(590, 344); glVertex2f(556, 344);
    glEnd();

    glColor3f(0.16, 0.15, 0.16);
    circle(227, 328, 12, 16);
    circle(573, 328, 12, 16);
    glColor3f(0.40, 0.38, 0.40);
    circle(227, 328, 5, 12);
    circle(573, 328, 5, 12);
}


// ============================= Obj019_DholPlayer ============================
// Dhol player - simple traditional Bengali performer.
// POSITION : centre x = 285, feet y = 296.
void Obj019_DholPlayer() {

    float s = sin(dholAngle * PI / 180);

    // legs
    glColor3f(0.20, 0.18, 0.16);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(279, 330); glVertex2f(276, 296);
    glVertex2f(291, 330); glVertex2f(296, 296);
    glEnd();
    glLineWidth(1);

    // white panjabi
    glColor3f(0.95, 0.94, 0.88);
    glBegin(GL_POLYGON);
    glVertex2f(272, 330);
    glVertex2f(298, 330);
    glVertex2f(302, 358);
    glVertex2f(268, 358);
    glEnd();

    // red scarf
    glColor3f(0.85, 0.10, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(270, 352);
    glVertex2f(300, 352);
    glVertex2f(299, 359);
    glVertex2f(271, 359);
    glEnd();

    // neck
    glColor3f(0.95, 0.80, 0.65);
    glBegin(GL_QUADS);
    glVertex2f(281, 357); glVertex2f(289, 357);
    glVertex2f(289, 365); glVertex2f(281, 365);
    glEnd();

    // hair
    glColor3f(0.10, 0.08, 0.07);
    circle(285, 372, 13);

    // face
    glColor3f(0.95, 0.80, 0.65);
    circle(285, 370, 10);

    // eyes
    glColor3f(0.10, 0.08, 0.07);
    circle(281, 370, 1.5, 8);
    circle(289, 370, 1.5, 8);

    // arms
    glColor3f(0.95, 0.80, 0.65);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(272, 350); glVertex2f(261, 340 - 7*s);
    glVertex2f(298, 350); glVertex2f(309, 340 + 7*s);
    glEnd();
    glLineWidth(1);

    // dhol strap
    glColor3f(0.70, 0.48, 0.20);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(274, 357); glVertex2f(302, 333);
    glEnd();
    glLineWidth(1);

    // dhol body
    glColor3f(0.68, 0.28, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(261, 309);
    glVertex2f(309, 309);
    glVertex2f(309, 335);
    glVertex2f(261, 335);
    glEnd();

    // drum heads
    glColor3f(0.95, 0.90, 0.78);
    circle(261, 322, 13, 16);
    circle(309, 322, 13, 16);

    // drum rings
    glColor3f(0.82, 0.12, 0.08);
    ring(261, 322, 12, 16);
    ring(309, 322, 12, 16);

    // drum center
    glColor3f(0.95, 0.80, 0.35);
    circle(261, 322, 5, 12);
    circle(309, 322, 5, 12);

    // sticks
    glColor3f(0.55, 0.35, 0.12);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(261, 340 - 7*s); glVertex2f(255, 318 - 10*s);
    glVertex2f(309, 340 + 7*s); glVertex2f(315, 318 + 10*s);
    glEnd();
    glLineWidth(1);
}

// =============================== Obj020_Singer1 =============================
// Female singer - simple saree and microphone.
// POSITION : centre x = 345, feet y = 296.
void Obj020_Singer1() {

    // legs
    glColor3f(0.20, 0.18, 0.16);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(341, 320); glVertex2f(339, 296);
    glVertex2f(350, 320); glVertex2f(351, 296);
    glEnd();
    glLineWidth(1);

    // saree
    glColor3f(0.85, 0.12, 0.28);
    glBegin(GL_POLYGON);
    glVertex2f(333, 335);
    glVertex2f(357, 335);
    glVertex2f(362, 296);
    glVertex2f(328, 296);
    glEnd();

    // saree border
    glColor3f(0.95, 0.78, 0.20);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(330, 300); glVertex2f(360, 300);
    glVertex2f(334, 300); glVertex2f(355, 334);
    glEnd();
    glLineWidth(1);

    // blouse
    glColor3f(0.70, 0.10, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(334, 335);
    glVertex2f(356, 335);
    glVertex2f(358, 358);
    glVertex2f(332, 358);
    glEnd();

    // neck
    glColor3f(0.95, 0.80, 0.65);
    glBegin(GL_QUADS);
    glVertex2f(341, 356); glVertex2f(349, 356);
    glVertex2f(349, 364); glVertex2f(341, 364);
    glEnd();

    // hair
    glColor3f(0.10, 0.08, 0.07);
    circle(345, 373, 13);

    // face
    glColor3f(0.95, 0.80, 0.65);
    circle(345, 370, 10);

    // eyes
    glColor3f(0.10, 0.08, 0.07);
    circle(341, 370, 1.5, 8);
    circle(349, 370, 1.5, 8);

    // bindi
    glColor3f(0.80, 0.05, 0.05);
    circle(345, 366, 1.5, 8);

    // flower
    glColor3f(1.0, 0.80, 0.15);
    circle(356, 378, 4, 8);

    // arms
    glColor3f(0.95, 0.80, 0.65);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(334, 347); glVertex2f(330, 337);
    glVertex2f(356, 347); glVertex2f(360, 356);
    glEnd();
    glLineWidth(1);

    // microphone
    glColor3f(0.25, 0.25, 0.28);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(360, 356); glVertex2f(360, 364);
    glEnd();
    glLineWidth(1);

    circle(360, 367, 4, 10);
}

// =============================== Obj021_Singer2 =============================
// SECOND SINGER (man) - cream panjabi, singing into a hand microphone.
// POSITION : centre x = 465, feet y = 296, everything inside x 449..481.
void Obj021_Singer2() {
    // pyjama
    glColor3f(0.35, 0.32, 0.42);
    glBegin(GL_QUADS);
    glVertex2f(452, 296); glVertex2f(478, 296);
    glVertex2f(476, 336); glVertex2f(454, 336);
    glEnd();

    // cream panjabi
    glColor3f(0.96, 0.94, 0.86);
    glBegin(GL_QUADS);
    glVertex2f(451, 336); glVertex2f(479, 336);
    glVertex2f(481, 358); glVertex2f(449, 358);
    glEnd();

    // head
    glColor3f(0.12, 0.10, 0.10);
    circle(465, 372, 12);
    glColor3f(0.95, 0.80, 0.65);
    circle(465, 368, 10);
    glColor3f(0.1, 0.1, 0.1);
    circle(461, 369, 1.5, 8); circle(469, 369, 1.5, 8);

    // arms - one hand holds the microphone
    glColor3f(0.95, 0.80, 0.65);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(451, 348); glVertex2f(448, 340);
    glVertex2f(479, 348); glVertex2f(482, 356);
    glEnd();
    glLineWidth(1);

    // hand microphone
    glColor3f(0.25, 0.25, 0.28);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(482, 356); glVertex2f(482, 362);
    glEnd();
    glLineWidth(1);
    glColor3f(0.35, 0.35, 0.38);
    circle(482, 365, 4, 10);
}


// ============================ Obj022_EktaraPlayer ===========================
// THE EKTARA PLAYER - one string folk instrument.
//   the gourd (round sound box) is held under one arm, a long bamboo neck
//   goes up from it, and one white string runs along the neck.
// POSITION : centre x = 535, feet y = 296, everything inside x 519..551.
void Obj022_EktaraPlayer() {
    // dhoti
    glColor3f(0.95, 0.93, 0.84);
    glBegin(GL_POLYGON);
    glVertex2f(522, 336); glVertex2f(548, 336);
    glVertex2f(553, 296); glVertex2f(517, 296);
    glEnd();

    // orange kurta
    glColor3f(0.95, 0.55, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(523, 336); glVertex2f(547, 336);
    glVertex2f(549, 358); glVertex2f(521, 358);
    glEnd();

    // head with a green tupi
    glColor3f(0.12, 0.10, 0.10);
    circle(535, 372, 12);
    glColor3f(0.95, 0.80, 0.65);
    circle(535, 368, 10);
    glColor3f(0.1, 0.1, 0.1);
    circle(531, 369, 1.5, 8); circle(539, 369, 1.5, 8);
    //glColor3f(0.10, 0.55, 0.30);
    //circle(535, 376, 10, 16);

    // arms : left arm holds the gourd, right arm plucks the string
    glColor3f(0.95, 0.80, 0.65);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(523, 348); glVertex2f(521, 336);
    glVertex2f(547, 348); glVertex2f(543, 352);
    glEnd();
    glLineWidth(1);

    // EKTARA : bamboo neck + gourd + one string
    glColor3f(0.55, 0.38, 0.18);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(521, 336); glVertex2f(514, 374);
    glEnd();
    glLineWidth(1);

    glColor3f(0.85, 0.62, 0.25);
    circle(524, 330, 11, 16);
    glColor3f(0.62, 0.42, 0.16);
    circle(524, 330, 7, 12);

    glColor3f(0.98, 0.98, 0.98);
    glLineWidth(1.2);
    glBegin(GL_LINES);
    glVertex2f(519, 340); glVertex2f(514, 372);
    glEnd();
    glLineWidth(1);
}

// ============================== Obj024_FoodStall ============================
// THE FOOD STALL (left, x 6..186, y 28..250) :
//   two bamboo posts hold the striped canopy, the counter carries a clay stove
//   with a steaming cauldron, a tray of jilapi and a plate of panta-ilish,
//   the vendor stands behind the counter with a ladle, two customers eat.
// POSITION : posts x 18 and x 178 (y 152..216) ; canopy apex (95, 250) ;
//            counter y 120..152 ; customers stand with their feet at y = 28.
void Obj024_FoodStall() {
    // ---------- two bamboo posts that hold up the canopy ----------
    glColor3f(0.58, 0.44, 0.22);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex2f(18, 152); glVertex2f(18, 216);
    glVertex2f(178, 152); glVertex2f(178, 216);
    glEnd();
    glLineWidth(1);

    // ---------- striped canopy ----------
    glColor3f(0.88, 0.14, 0.14);
    glBegin(GL_TRIANGLES);
    glVertex2f(95, 250); glVertex2f(6, 216);   glVertex2f(36, 216);
    glVertex2f(95, 250); glVertex2f(66, 216);  glVertex2f(96, 216);
    glVertex2f(95, 250); glVertex2f(126, 216); glVertex2f(156, 216);
    glEnd();
    glColor3f(0.98, 0.92, 0.72);
    glBegin(GL_TRIANGLES);
    glVertex2f(95, 250); glVertex2f(36, 216);  glVertex2f(66, 216);
    glVertex2f(95, 250); glVertex2f(96, 216);  glVertex2f(126, 216);
    glVertex2f(95, 250); glVertex2f(156, 216); glVertex2f(186, 216);
    glEnd();
    glColor3f(0.85, 0.12, 0.12);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(6, 216); glVertex2f(186, 216);
    glEnd();
    glLineWidth(1);

    // ---------- clay stove with a steaming cauldron (x 20..62) ----------
    glColor3f(0.68, 0.40, 0.22);
    glBegin(GL_QUADS);
    glVertex2f(20, 152); glVertex2f(62, 152);
    glVertex2f(56, 166); glVertex2f(26, 166);
    glEnd();
    glColor3f(0.30, 0.26, 0.24);
    glBegin(GL_QUADS);
    glVertex2f(26, 152); glVertex2f(56, 152);
    glVertex2f(56, 158); glVertex2f(26, 158);
    glEnd();
    glColor3f(0.55, 0.55, 0.60);
    glBegin(GL_POLYGON);
    glVertex2f(22, 166); glVertex2f(60, 166);
    glVertex2f(54, 186); glVertex2f(28, 186);
    glEnd();
    glColor3f(0.75, 0.75, 0.80);
    glBegin(GL_QUADS);
    glVertex2f(24, 184); glVertex2f(58, 184);
    glVertex2f(58, 190); glVertex2f(24, 190);
    glEnd();
    glColor3f(0.95, 0.85, 0.35);
    circle(41, 188, 6, 12);
    glLineWidth(1);

    // ---------- counter ----------
    glColor3f(0.64, 0.40, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(16, 120); glVertex2f(180, 120);
    glVertex2f(180, 152); glVertex2f(16, 152);
    glEnd();
    glColor3f(0.46, 0.28, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(16, 142); glVertex2f(180, 142);
    glVertex2f(180, 152); glVertex2f(16, 152);
    glEnd();

    // ---------- tray of jilapi (x 70..116) ----------
    glColor3f(0.90, 0.72, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(70, 152); glVertex2f(116, 152);
    glVertex2f(112, 166); glVertex2f(74, 166);
    glEnd();
    glColor3f(0.95, 0.55, 0.10);
    circle(82, 161, 5, 12);
    circle(94, 162, 5, 12);
    circle(106, 161, 5, 12);
    glColor3f(0.75, 0.35, 0.05);
    ring(82, 161, 3, 10);
    ring(94, 162, 3, 10);
    ring(106, 161, 3, 10);

    // ---------- vendor behind the counter (x 120..144) ----------
    glColor3f(0.14, 0.10, 0.10);  circle(132, 198, 10);
    glColor3f(0.95, 0.80, 0.65);  circle(132, 195, 8);
    glColor3f(0.0, 0.0, 0.0);     circle(129, 196, 1.3); circle(135, 196, 1.3);
    glColor3f(0.97, 0.96, 0.88);
    glBegin(GL_QUADS);
    glVertex2f(122, 152); glVertex2f(142, 152);
    glVertex2f(144, 186); glVertex2f(120, 186);
    glEnd();
    glColor3f(0.55, 0.18, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(121, 152); glVertex2f(143, 152);
    glVertex2f(144, 164); glVertex2f(120, 164);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(122, 178); glVertex2f(114, 186);
    glVertex2f(142, 178); glVertex2f(150, 186);
    glEnd();
    glLineWidth(1);
    // ladle in his hand
    glColor3f(0.72, 0.72, 0.76);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(150, 186); glVertex2f(152, 190);
    glEnd();
    glLineWidth(1);
    glColor3f(0.80, 0.80, 0.84);
    circle(153, 188, 5, 12);

    

    // ---------- two customers eating in front (feet y = 28) ----------
    // customer 1 (x = 50)
    glColor3f(0.14, 0.10, 0.10);  circle(50, 88, 10);
    glColor3f(0.95, 0.80, 0.65);  circle(50, 85, 8);
    glColor3f(0.20, 0.45, 0.85);
    glBegin(GL_QUADS);
    glVertex2f(40, 52); glVertex2f(60, 52);
    glVertex2f(62, 70); glVertex2f(38, 70);
    glEnd();
    glColor3f(0.30, 0.30, 0.38);
    glBegin(GL_TRIANGLES);
    glVertex2f(50, 56); glVertex2f(37, 40); glVertex2f(63, 40);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(45, 40); glVertex2f(45, 28);
    glVertex2f(55, 40); glVertex2f(55, 28);
    glVertex2f(40, 66); glVertex2f(32, 60);
    glVertex2f(60, 66); glVertex2f(66, 74);
    glEnd();
    glLineWidth(1);
    glColor3f(0.96, 0.95, 0.90);
    circle(68, 76, 6, 12);

    // customer 2 (x = 140)
    glColor3f(0.14, 0.10, 0.10);  circle(140, 88, 10);
    glColor3f(0.95, 0.80, 0.65);  circle(140, 85, 8);
    glColor3f(0.95, 0.55, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(130, 52); glVertex2f(150, 52);
    glVertex2f(152, 70); glVertex2f(128, 70);
    glEnd();
    glColor3f(0.35, 0.32, 0.30);
    glBegin(GL_TRIANGLES);
    glVertex2f(140, 56); glVertex2f(127, 40); glVertex2f(153, 40);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(135, 40); glVertex2f(135, 28);
    glVertex2f(145, 40); glVertex2f(145, 28);
    glVertex2f(130, 66); glVertex2f(122, 60);
    glVertex2f(150, 66); glVertex2f(158, 74);
    glEnd();
    glLineWidth(1);
    glColor3f(0.96, 0.95, 0.90);
    circle(160, 76, 6, 12);
}


// ============================= Obj025_DrinkStall ============================
// Drink stall (right, x 610..800, y 28..250) : two posts hold the canopy, the
// counter carries a big jug of sherbet, two glasses and a tray of sweets, the
// vendor serves, and two customers drink in front.
// POSITION : posts x 620 and x 788 (y 120..216) ; canopy apex (705, 250) ;
//            counter y 120..152 ; customers stand with feet at y = 28.
void Obj025_DrinkStall() {
    // ---------- canopy ----------
    glColor3f(0.10, 0.45, 0.72);
    glBegin(GL_TRIANGLES);
    glVertex2f(705, 250); glVertex2f(610, 216); glVertex2f(641, 216);
    glVertex2f(705, 250); glVertex2f(672, 216); glVertex2f(703, 216);
    glVertex2f(705, 250); glVertex2f(734, 216); glVertex2f(765, 216);
    glEnd();
    glColor3f(0.98, 0.92, 0.72);
    glBegin(GL_TRIANGLES);
    glVertex2f(705, 250); glVertex2f(641, 216); glVertex2f(672, 216);
    glVertex2f(705, 250); glVertex2f(703, 216); glVertex2f(734, 216);
    glVertex2f(705, 250); glVertex2f(765, 216); glVertex2f(796, 216);
    glEnd();
    glColor3f(0.10, 0.35, 0.60);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(610, 216); glVertex2f(796, 216);
    glEnd();
    glLineWidth(1);

    // ---------- counter ----------
    glColor3f(0.64, 0.40, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(618, 120); glVertex2f(792, 120);
    glVertex2f(792, 152); glVertex2f(618, 152);
    glEnd();
    glColor3f(0.46, 0.28, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(618, 142); glVertex2f(792, 142);
    glVertex2f(792, 152); glVertex2f(618, 152);
    glEnd();

    // ---------- big jug of sherbet ----------
    glColor3f(0.92, 0.55, 0.15);
    circle(690, 166, 13, 16);
    glColor3f(0.70, 0.40, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(684, 178); glVertex2f(696, 178);
    glVertex2f(696, 184); glVertex2f(684, 184);
    glEnd();
    glColor3f(0.95, 0.75, 0.30);
    circle(690, 164, 8, 12);

    // ---------- two glasses ----------
    glColor3f(0.93, 0.97, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(710, 152); glVertex2f(724, 152);
    glVertex2f(722, 170); glVertex2f(712, 170);
    glVertex2f(728, 152); glVertex2f(742, 152);
    glVertex2f(740, 170); glVertex2f(730, 170);
    glEnd();
    glColor3f(0.95, 0.45, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(712, 154); glVertex2f(722, 154);
    glVertex2f(721, 166); glVertex2f(713, 166);
    glVertex2f(730, 154); glVertex2f(740, 154);
    glVertex2f(739, 166); glVertex2f(731, 166);
    glEnd();

    // ---------- tray of sweets ----------
    glColor3f(0.90, 0.72, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(624, 152); glVertex2f(668, 152);
    glVertex2f(664, 166); glVertex2f(628, 166);
    glEnd();
    glColor3f(0.95, 0.55, 0.10);
    circle(636, 161, 4.5, 10);
    circle(648, 162, 4.5, 10);
    circle(660, 161, 4.5, 10);

    // ---------- vendor behind the counter (x 748..778) ----------
    glColor3f(0.14, 0.10, 0.10);  circle(763, 198, 10);
    glColor3f(0.95, 0.80, 0.65);  circle(763, 195, 8);
    glColor3f(0.0, 0.0, 0.0);     circle(760, 196, 1.3); circle(766, 196, 1.3);
    glColor3f(0.20, 0.55, 0.35);
    glBegin(GL_QUADS);
    glVertex2f(752, 152); glVertex2f(774, 152);
    glVertex2f(776, 186); glVertex2f(750, 186);
    glEnd();
    glColor3f(0.85, 0.80, 0.40);
    glBegin(GL_QUADS);
    glVertex2f(751, 152); glVertex2f(775, 152);
    glVertex2f(776, 164); glVertex2f(750, 164);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(752, 178); glVertex2f(744, 186);
    glVertex2f(774, 178); glVertex2f(784, 188);
    glEnd();
    glLineWidth(1);
    glColor3f(0.93, 0.97, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(779, 188); glVertex2f(789, 188);
    glVertex2f(788, 200); glVertex2f(780, 200);
    glEnd();

    // ---------- two bamboo posts that hold up the canopy ----------
    glColor3f(0.58, 0.44, 0.22);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex2f(620, 120); glVertex2f(620, 216);
    glVertex2f(788, 120); glVertex2f(788, 216);
    glEnd();
    glLineWidth(1);

    // ---------- customer drinking in front (feet y = 28) ----------
    // customer 1 (x = 690)
    glColor3f(0.14, 0.10, 0.10);  circle(690, 88, 10);
    glColor3f(0.95, 0.80, 0.65);  circle(690, 85, 8);
    glColor3f(0.85, 0.20, 0.55);
    glBegin(GL_QUADS);
    glVertex2f(680, 52); glVertex2f(700, 52);
    glVertex2f(702, 70); glVertex2f(678, 70);
    glEnd();
    glColor3f(0.32, 0.30, 0.35);
    glBegin(GL_TRIANGLES);
    glVertex2f(690, 56); glVertex2f(677, 40); glVertex2f(703, 40);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(685, 40); glVertex2f(685, 28);
    glVertex2f(695, 40); glVertex2f(695, 28);
    glVertex2f(680, 66); glVertex2f(672, 60);
    glVertex2f(700, 66); glVertex2f(708, 74);
    glEnd();
    glLineWidth(1);
    glColor3f(0.93, 0.97, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(704, 74); glVertex2f(714, 74);
    glVertex2f(713, 88); glVertex2f(705, 88);
    glEnd();

    // customer 2 (x = 752)
    glColor3f(0.14, 0.10, 0.10);  circle(752, 88, 10);
    glColor3f(0.95, 0.80, 0.65);  circle(752, 85, 8);
    glColor3f(0.15, 0.60, 0.85);
    glBegin(GL_QUADS);
    glVertex2f(742, 52); glVertex2f(762, 52);
    glVertex2f(764, 70); glVertex2f(740, 70);
    glEnd();
    glColor3f(0.35, 0.32, 0.30);
    glBegin(GL_TRIANGLES);
    glVertex2f(752, 56); glVertex2f(739, 40); glVertex2f(765, 40);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(747, 40); glVertex2f(747, 28);
    glVertex2f(757, 40); glVertex2f(757, 28);
    glVertex2f(742, 66); glVertex2f(734, 60);
    glVertex2f(762, 66); glVertex2f(770, 74);
    glEnd();
    glLineWidth(1);
    glColor3f(0.93, 0.97, 1.0);
    glBegin(GL_QUADS);
    glVertex2f(766, 74); glVertex2f(776, 74);
    glVertex2f(775, 88); glVertex2f(767, 88);
    glEnd();
}


// ========================== AUDIENCE & CHILDREN =============================
// =========================== Obj026_AudienceRow1 ============================
// Back row of the audience - people sitting on mats (seen from behind).
// POSITION : mats y 180..186, heads reach y 210 ; x from 195 to 595.
void Obj026_AudienceRow1() {
    glColor3f(0.72, 0.62, 0.42);
    glBegin(GL_QUADS);
    glVertex2f(195, 180); glVertex2f(235, 180);
    glVertex2f(239, 188); glVertex2f(191, 188);
    glVertex2f(240, 180); glVertex2f(280, 180);
    glVertex2f(284, 188); glVertex2f(236, 188);
    glVertex2f(285, 180); glVertex2f(325, 180);
    glVertex2f(329, 188); glVertex2f(281, 188);
    glVertex2f(330, 180); glVertex2f(370, 180);
    glVertex2f(374, 188); glVertex2f(326, 188);
    glVertex2f(375, 180); glVertex2f(415, 180);
    glVertex2f(419, 188); glVertex2f(371, 188);
    glVertex2f(420, 180); glVertex2f(460, 180);
    glVertex2f(464, 188); glVertex2f(416, 188);
    glVertex2f(465, 180); glVertex2f(505, 180);
    glVertex2f(509, 188); glVertex2f(461, 188);
    glVertex2f(510, 180); glVertex2f(550, 180);
    glVertex2f(554, 188); glVertex2f(506, 188);
    glVertex2f(555, 180); glVertex2f(595, 180);
    glVertex2f(599, 188); glVertex2f(551, 188);
    glEnd();

    // person 1 (x = 215)
    glColor3f(0.20, 0.16, 0.14);  circle(215, 204, 7);
    glColor3f(0.85, 0.20, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(204, 186); glVertex2f(226, 186);
    glVertex2f(224, 199); glVertex2f(206, 199);
    glEnd();
    // person 2 (x = 260)
    glColor3f(0.20, 0.16, 0.14);  circle(260, 204, 7);
    glColor3f(0.15, 0.55, 0.75);
    glBegin(GL_QUADS);
    glVertex2f(249, 186); glVertex2f(271, 186);
    glVertex2f(269, 199); glVertex2f(251, 199);
    glEnd();
    // person 3 (x = 305)
    glColor3f(0.20, 0.16, 0.14);  circle(305, 204, 7);
    glColor3f(0.95, 0.78, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(294, 186); glVertex2f(316, 186);
    glVertex2f(314, 199); glVertex2f(296, 199);
    glEnd();
    // person 4 (x = 350)
    glColor3f(0.20, 0.16, 0.14);  circle(350, 204, 7);
    glColor3f(0.80, 0.25, 0.60);
    glBegin(GL_QUADS);
    glVertex2f(339, 186); glVertex2f(361, 186);
    glVertex2f(359, 199); glVertex2f(341, 199);
    glEnd();
    // person 5 (x = 395)
    glColor3f(0.20, 0.16, 0.14);  circle(395, 204, 7);
    glColor3f(0.15, 0.60, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(384, 186); glVertex2f(406, 186);
    glVertex2f(404, 199); glVertex2f(386, 199);
    glEnd();
    // person 6 (x = 440)
    glColor3f(0.20, 0.16, 0.14);  circle(440, 204, 7);
    glColor3f(0.95, 0.55, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(429, 186); glVertex2f(451, 186);
    glVertex2f(449, 199); glVertex2f(431, 199);
    glEnd();
    // person 7 (x = 485)
    glColor3f(0.20, 0.16, 0.14);  circle(485, 204, 7);
    glColor3f(0.30, 0.35, 0.75);
    glBegin(GL_QUADS);
    glVertex2f(474, 186); glVertex2f(496, 186);
    glVertex2f(494, 199); glVertex2f(476, 199);
    glEnd();
    // person 8 (x = 530)
    glColor3f(0.20, 0.16, 0.14);  circle(530, 204, 7);
    glColor3f(0.90, 0.20, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(519, 186); glVertex2f(541, 186);
    glVertex2f(539, 199); glVertex2f(521, 199);
    glEnd();
    // person 9 (x = 575)
    glColor3f(0.20, 0.16, 0.14);  circle(575, 204, 7);
    glColor3f(0.20, 0.50, 0.80);
    glBegin(GL_QUADS);
    glVertex2f(564, 186); glVertex2f(586, 186);
    glVertex2f(584, 199); glVertex2f(566, 199);
    glEnd();
}


// =========================== Obj027_AudienceRow2 ============================
// Second row of the audience, sitting on mats (a little bigger = closer).
// POSITION : mats y 130..138, heads reach y 166 ; x from 207 to 543.
void Obj027_AudienceRow2() {
    glColor3f(0.70, 0.60, 0.40);
    glBegin(GL_QUADS);
    glVertex2f(211, 130); glVertex2f(239, 130);
    glVertex2f(243, 138); glVertex2f(207, 138);
    glVertex2f(261, 130); glVertex2f(289, 130);
    glVertex2f(293, 138); glVertex2f(257, 138);
    glVertex2f(311, 130); glVertex2f(339, 130);
    glVertex2f(343, 138); glVertex2f(307, 138);
    glVertex2f(361, 130); glVertex2f(389, 130);
    glVertex2f(393, 138); glVertex2f(357, 138);
    glVertex2f(411, 130); glVertex2f(439, 130);
    glVertex2f(443, 138); glVertex2f(407, 138);
    glVertex2f(461, 130); glVertex2f(489, 130);
    glVertex2f(493, 138); glVertex2f(457, 138);
    glVertex2f(511, 130); glVertex2f(539, 130);
    glVertex2f(543, 138); glVertex2f(507, 138);
    glEnd();

    // person 1 (x = 225)
    glColor3f(0.18, 0.14, 0.12);  circle(225, 160, 8);
    glColor3f(0.90, 0.85, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(213, 138); glVertex2f(237, 138);
    glVertex2f(235, 153); glVertex2f(215, 153);
    glEnd();
    // person 2 (x = 275)
    glColor3f(0.18, 0.14, 0.12);  circle(275, 160, 8);
    glColor3f(0.85, 0.30, 0.55);
    glBegin(GL_QUADS);
    glVertex2f(263, 138); glVertex2f(287, 138);
    glVertex2f(285, 153); glVertex2f(265, 153);
    glEnd();
    // person 3 (x = 325)
    glColor3f(0.18, 0.14, 0.12);  circle(325, 160, 8);
    glColor3f(0.20, 0.50, 0.80);
    glBegin(GL_QUADS);
    glVertex2f(313, 138); glVertex2f(337, 138);
    glVertex2f(335, 153); glVertex2f(315, 153);
    glEnd();
    // person 4 (x = 375)
    glColor3f(0.18, 0.14, 0.12);  circle(375, 160, 8);
    glColor3f(0.90, 0.15, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(363, 138); glVertex2f(387, 138);
    glVertex2f(385, 153); glVertex2f(365, 153);
    glEnd();
    // person 5 (x = 425)
    glColor3f(0.18, 0.14, 0.12);  circle(425, 160, 8);
    glColor3f(0.10, 0.62, 0.35);
    glBegin(GL_QUADS);
    glVertex2f(413, 138); glVertex2f(437, 138);
    glVertex2f(435, 153); glVertex2f(415, 153);
    glEnd();
    // person 6 (x = 475)
    glColor3f(0.18, 0.14, 0.12);  circle(475, 160, 8);
    glColor3f(0.95, 0.75, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(463, 138); glVertex2f(487, 138);
    glVertex2f(485, 153); glVertex2f(465, 153);
    glEnd();
    // person 7 (x = 525)
    glColor3f(0.18, 0.14, 0.12);  circle(525, 160, 8);
    glColor3f(0.55, 0.25, 0.75);
    glBegin(GL_QUADS);
    glVertex2f(513, 138); glVertex2f(537, 138);
    glVertex2f(535, 153); glVertex2f(515, 153);
    glEnd();
}


// =========================== Obj028_AudienceRow3 ============================
// Third row - four standing people watching the programme.
// POSITION : feet y = 70, heads reach y 116 ; x from 220 to 445.
void Obj028_AudienceRow3() {
    // person 1 (x = 235)
    glColor3f(0.18, 0.14, 0.12);  circle(235, 110, 8);
    glColor3f(0.95, 0.25, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(226, 82); glVertex2f(244, 82);
    glVertex2f(246, 98); glVertex2f(224, 98);
    glEnd();
    glColor3f(0.30, 0.30, 0.38);
    glBegin(GL_TRIANGLES);
    glVertex2f(235, 86); glVertex2f(224, 70); glVertex2f(246, 70);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(231, 70); glVertex2f(231, 60);
    glVertex2f(239, 70); glVertex2f(239, 60);
    glVertex2f(226, 94); glVertex2f(219, 86);
    glVertex2f(244, 94); glVertex2f(251, 86);
    glEnd();
    glLineWidth(1);

    // person 2 (x = 300)
    glColor3f(0.18, 0.14, 0.12);  circle(300, 110, 8);
    glColor3f(0.15, 0.55, 0.80);
    glBegin(GL_QUADS);
    glVertex2f(291, 82); glVertex2f(309, 82);
    glVertex2f(311, 98); glVertex2f(289, 98);
    glEnd();
    glColor3f(0.35, 0.32, 0.30);
    glBegin(GL_TRIANGLES);
    glVertex2f(300, 86); glVertex2f(289, 70); glVertex2f(311, 70);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(296, 70); glVertex2f(296, 60);
    glVertex2f(304, 70); glVertex2f(304, 60);
    glVertex2f(291, 94); glVertex2f(284, 86);
    glVertex2f(309, 94); glVertex2f(316, 104);
    glEnd();
    glLineWidth(1);

    // person 3 (x = 365)
    glColor3f(0.18, 0.14, 0.12);  circle(365, 110, 8);
    glColor3f(0.95, 0.80, 0.25);
    glBegin(GL_QUADS);
    glVertex2f(356, 82); glVertex2f(374, 82);
    glVertex2f(376, 98); glVertex2f(354, 98);
    glEnd();
    glColor3f(0.40, 0.36, 0.32);
    glBegin(GL_TRIANGLES);
    glVertex2f(365, 86); glVertex2f(354, 70); glVertex2f(376, 70);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(361, 70); glVertex2f(361, 60);
    glVertex2f(369, 70); glVertex2f(369, 60);
    glVertex2f(356, 94); glVertex2f(349, 102);
    glVertex2f(374, 94); glVertex2f(381, 102);
    glEnd();
    glLineWidth(1);

    // person 4 (x = 430)
    glColor3f(0.18, 0.14, 0.12);  circle(430, 110, 8);
    glColor3f(0.85, 0.20, 0.55);
    glBegin(GL_QUADS);
    glVertex2f(421, 82); glVertex2f(439, 82);
    glVertex2f(441, 98); glVertex2f(419, 98);
    glEnd();
    glColor3f(0.32, 0.30, 0.35);
    glBegin(GL_TRIANGLES);
    glVertex2f(430, 86); glVertex2f(419, 70); glVertex2f(441, 70);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(426, 70); glVertex2f(426, 60);
    glVertex2f(434, 70); glVertex2f(434, 60);
    glVertex2f(421, 94); glVertex2f(414, 86);
    glVertex2f(439, 94); glVertex2f(446, 86);
    glEnd();
    glLineWidth(1);
}

// ============================ Obj030_ChildrenPlay ===========================
// Children playing in the front right corner, x 478..667, y 20..114 :
//   two of them with balloons, one with a spinning chorki, and a ball.
// POSITION : all feet on the line y = 20.
void Obj030_ChildrenPlay() {
    // ---- child 1 : red balloon (x = 495) ----
    glColor3f(0.14, 0.10, 0.10);  circle(495, 68, 9);
    glColor3f(0.95, 0.80, 0.65);  circle(495, 65, 7);
    glColor3f(0.0, 0.0, 0.0);     circle(492, 66, 1.2); circle(498, 66, 1.2);
    glColor3f(0.95, 0.85, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(486, 32); glVertex2f(504, 32);
    glVertex2f(506, 50); glVertex2f(484, 50);
    glEnd();
    glColor3f(0.20, 0.50, 0.85);
    glBegin(GL_TRIANGLES);
    glVertex2f(495, 36); glVertex2f(483, 20); glVertex2f(507, 20);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(490, 20); glVertex2f(490, 10);
    glVertex2f(500, 20); glVertex2f(500, 10);
    glVertex2f(486, 44); glVertex2f(478, 36);
    glVertex2f(504, 44); glVertex2f(512, 72);
    glEnd();
    glLineWidth(1);
    float b1 = 4 * sin(stageTimer * 2);
    glColor3f(0.35, 0.32, 0.30);
    glBegin(GL_LINES);
    glVertex2f(512, 72); glVertex2f(520 + b1, 94);
    glEnd();
    glColor3f(0.90, 0.12, 0.12);
    circle(520 + b1, 102, 10);
    glBegin(GL_TRIANGLES);
    glVertex2f(520 + b1, 92); glVertex2f(517 + b1, 88); glVertex2f(523 + b1, 88);
    glEnd();

    // ---- child 2 : blue balloon (x = 580) ----
    glColor3f(0.14, 0.10, 0.10);  circle(580, 68, 9);
    glColor3f(0.95, 0.80, 0.65);  circle(580, 65, 7);
    glColor3f(0.0, 0.0, 0.0);     circle(577, 66, 1.2); circle(583, 66, 1.2);
    glColor3f(0.85, 0.25, 0.60);
    glBegin(GL_QUADS);
    glVertex2f(571, 32); glVertex2f(589, 32);
    glVertex2f(591, 50); glVertex2f(569, 50);
    glEnd();
    glColor3f(0.95, 0.75, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(580, 36); glVertex2f(568, 20); glVertex2f(592, 20);
    glEnd();
    glColor3f(0.25, 0.22, 0.20);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(575, 20); glVertex2f(575, 10);
    glVertex2f(585, 20); glVertex2f(585, 10);
    glVertex2f(571, 44); glVertex2f(563, 36);
    glVertex2f(589, 44); glVertex2f(597, 72);
    glEnd();
    glLineWidth(1);
    float b2 = 4 * sin(stageTimer * 2 + 2);
    glColor3f(0.35, 0.32, 0.30);
    glBegin(GL_LINES);
    glVertex2f(597, 72); glVertex2f(605 + b2, 92);
    glEnd();
    glColor3f(0.15, 0.45, 0.90);
    circle(605 + b2, 100, 10);
    glBegin(GL_TRIANGLES);
    glVertex2f(605 + b2, 90); glVertex2f(602 + b2, 86); glVertex2f(608 + b2, 86);
    glEnd();
    glLineWidth(1);
}


// =========================== display + animation ============================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // ---------------- evening sky ----------------
    Obj001_Sky();
    Obj002_Sun();

    // clouds stay inside the sky (clipping with glScissor)
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 300, 800, 500);
    Obj003_Cloud1();
    Obj004_Cloud2();
    Obj005_Cloud3();
    Obj006_Cloud4();
    glDisable(GL_SCISSOR_TEST);

    Obj007_Birds();

    // ---------------- village behind ----------------
    Obj008_Ground();
    Obj009_HouseLeft();
    Obj010_HouseRight();
    Obj011_TreeLeft();
    Obj012_TreeRight();

    // ---------------- the cultural stage ----------------
    Obj014_StageBackdrop();
    Obj015_StagePlatform();
    Obj016_StagePillars();
    Obj018_Speakers();
    Obj019_DholPlayer();
    Obj020_Singer1();
    Obj021_Singer2();
    Obj022_EktaraPlayer();

    // ---------------- stalls on both sides ----------------
    Obj024_FoodStall();
    Obj025_DrinkStall();

    // ---------------- audience and children ----------------
    Obj026_AudienceRow1();
    Obj027_AudienceRow2();
    Obj028_AudienceRow3();
    Obj030_ChildrenPlay();

    glColor3f(0.20, 0.18, 0.24);
    text(400, 770, "p = pause / resume      Esc = quit");

    glutSwapBuffers();
}

// ================================ update ================================
// Runs every 16 ms.  This is the animation controller: it changes only
// the global animation values; the Obj... functions do the actual drawing.
void update(int value) {
    if (!paused) {
        // clouds
        cloudX1 += 0.14; if (cloudX1 > 900)  cloudX1 = -100;
        cloudX3 += 0.10; if (cloudX3 > 900)  cloudX3 = -100;
        cloudX2 -= 0.11; if (cloudX2 < -100) cloudX2 = 900;
        cloudX4 -= 0.07; if (cloudX4 < -100) cloudX4 = 900;

        // birds fly home
        birdX += 2.0;
        if (birdX > 940) birdX = -140;
        birdTimer += 0.05;
        // dhol sticks swing
        dholAngle += 6;
        if (dholAngle >= 360) dholAngle -= 360;
        // animated balloons
        stageTimer += 0.04;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ================================ keyboard ================================
// p / P pauses or resumes the animation; Esc closes the program.
void keyboard(unsigned char key, int x, int y) {
    if (key == 'p' || key == 'P') paused = !paused;
    else if (key == 27) exit(0);
    glutPostRedisplay();
}

// ================================ init ===================================
// Sets the background, 2-D coordinate system and projection.
void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// ================================= main ==================================
// Creates the GLUT window, registers callbacks and starts the event loop.
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pohela Boishakh - Village Cultural Program (Scene 3)");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);       // p / Esc
    glutTimerFunc(0, update, 0);      // animation loop
    glutMainLoop();

    return 0;
}
