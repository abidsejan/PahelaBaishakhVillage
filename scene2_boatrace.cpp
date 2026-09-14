#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265


// ======================= ANIMATION VALUES (these move) ======================
// change the value on the right to change where a thing starts,
// change the number inside update() to change how fast it moves.

float sunY      = 560;    // Obj002_Sun : rises from 560 up to 705
float cloudX1   = 110;    // Obj003_Cloud1 : drifts left  -> right
float cloudX2   = 640;    // Obj004_Cloud2 : drifts right -> left
float cloudX3   = 320;    // Obj005_Cloud3 : drifts left  -> right (small)
float cloudX4   = 760;    // Obj006_Cloud4 : drifts right -> left  (tiny)

float birdX     = -120;   // Obj007_Birds : the flock flies left -> right
float birdTimer = 0;      // Obj007_Birds : wing flap clock

float boat1X = 90;        // Obj026_Boat1 (far lane)
float boat2X = 90;        // Obj027_Boat2
float boat3X = 90;        // Obj028_Boat3
float boat4X = 90;        // Obj029_Boat4

float boat1Speed = 1.15;  // how fast each boat rows (pixels per frame)
float boat2Speed = 0.95;
float boat3Speed = 1.45;
float boat4Speed = 1.25;

bool  boatsPaused = false; // p = true (boats stop), s = false (boats row)
float raceTimer  = 0;      // makes the sails / wakes / oars pulse

float kiteTimer  = 0;      // Obj052_Kite : the kite sways


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
// Bright morning sky over the village.
// POSITION : band from y = 470 (bottom) up to y = 800 (top of the window).
void Obj001_Sky() {
    glBegin(GL_QUADS);
    glColor3f(0.75, 0.91, 0.99);
    glVertex2f(0, 470);  glVertex2f(800, 470);
    glColor3f(0.29, 0.62, 0.90);
    glVertex2f(800, 800); glVertex2f(0, 800);
    glEnd();
}


// ================================ Obj002_Sun ================================
// Proper warm sun : deep yellow core, golden middle, soft orange glow + rays.
// POSITION : centre (660, sunY) - sunY is a global, it rises while the
//            program runs.  Change 660 to move it left / right.
void Obj002_Sun() {
    // soft outer glow
    glColor3f(1.0, 0.82, 0.20);  circle(660, sunY, 54);
    glColor3f(1.0, 0.82, 0.20);  circle(660, sunY, 46);
    // body of the sun
    // The entire sun uses one solid warm-yellow color.
    glColor3f(1.0, 0.82, 0.20);  circle(660, sunY, 38);
    glColor3f(1.0, 0.82, 0.20);  circle(660, sunY, 29);

    // rays around the sun
    glColor3f(1.0, 0.82, 0.20);
    glLineWidth(3);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        float a = i * PI / 6;
        glVertex2f(660 + cos(a) * 50, sunY + sin(a) * 50);
        glVertex2f(660 + cos(a) * 68, sunY + sin(a) * 68);
    }
    glEnd();
    glLineWidth(1);
}


// =============================== Obj003_Cloud1 ==============================
// Big white cloud, drifts left -> right.
// POSITION : centre (cloudX1, 700) - cloudX1 is a global.
void Obj003_Cloud1() {
    float x = cloudX1;
    glColor3f(1, 1, 1);
    circle(x - 34, 700, 20);
    circle(x, 712, 28);
    circle(x + 34, 700, 21);
    circle(x + 12, 694, 18);
    circle(x - 14, 694, 17);
}


// =============================== Obj004_Cloud2 ==============================
// Medium cloud, drifts right -> left.
// POSITION : centre (cloudX2, 640).
void Obj004_Cloud2() {
    float x = cloudX2;
    glColor3f(1, 1, 1);
    circle(x - 26, 640, 16);
    circle(x, 650, 23);
    circle(x + 26, 640, 16);
    circle(x + 10, 634, 14);
}


// =============================== Obj005_Cloud3 ==============================
// Small cloud, drifts left -> right.
// POSITION : centre (cloudX3, 745).
void Obj005_Cloud3() {
    float x = cloudX3;
    glColor3f(0.98, 0.99, 1.0);
    circle(x - 18, 745, 13);
    circle(x, 752, 18);
    circle(x + 18, 745, 13);
    circle(x + 7, 740, 11);
}


// =============================== Obj006_Cloud4 ==============================
// Tiny cloud near the top, drifts right -> left.
// POSITION : centre (cloudX4, 775).
void Obj006_Cloud4() {
    float x = cloudX4;
    glColor3f(1, 1, 1);
    circle(x - 13, 775, 9);
    circle(x, 781, 13);
    circle(x + 13, 775, 9);
}


// ================================ Obj007_Birds ==============================
// A flock of 5 birds gliding over the river, wings flap with birdTimer.
// POSITION : the flock starts at birdX (a global) and the 5 birds are placed
//            at birdX - 0, -34, -62, -95, -128 (see the numbers below).
void Obj007_Birds() {
    float x = birdX;
    float f = 5 * sin(birdTimer * 7);      // wing flap
    glColor3f(0.15, 0.15, 0.18);
    glLineWidth(2);
    glBegin(GL_LINES);
    // bird 1
    glVertex2f(x, 660);        glVertex2f(x - 11, 667 + f);
    glVertex2f(x, 660);        glVertex2f(x + 11, 667 + f);
    // bird 2
    glVertex2f(x - 34, 686);   glVertex2f(x - 45, 693 + f);
    glVertex2f(x - 34, 686);   glVertex2f(x - 23, 693 + f);
    // bird 3
    glVertex2f(x - 62, 646);   glVertex2f(x - 73, 653 + f);
    glVertex2f(x - 62, 646);   glVertex2f(x - 51, 653 + f);
    // bird 4
    glVertex2f(x - 95, 676);   glVertex2f(x - 106, 683 + f);
    glVertex2f(x - 95, 676);   glVertex2f(x - 84, 683 + f);
    // bird 5
    glVertex2f(x - 128, 630);  glVertex2f(x - 139, 637 + f);
    glVertex2f(x - 128, 630);  glVertex2f(x - 117, 637 + f);
    glEnd();
    glLineWidth(1);
}


// ============================ Obj008_DistantHills ===========================
// Soft blue-green hills far away behind the village.
// POSITION : the base line sits at y = 558, the peaks go up to y = 612.
void Obj008_DistantHills() {
    glColor3f(0.52, 0.72, 0.55);
    glBegin(GL_POLYGON);
    glVertex2f(0, 558);    glVertex2f(70, 578);
    glVertex2f(140, 566);  glVertex2f(215, 596);
    glVertex2f(290, 570);  glVertex2f(370, 606);
    glVertex2f(450, 572);  glVertex2f(530, 600);
    glVertex2f(610, 568);  glVertex2f(690, 592);
    glVertex2f(760, 566);  glVertex2f(800, 580);
    glVertex2f(800, 558);
    glEnd();
}


// ============================== Obj009_FarField =============================
// The upper (far) bank : green field + a small paddy plot.
// POSITION : band from y = 520 (water edge) up to y = 566.
void Obj009_FarField() {
    glBegin(GL_QUADS);
    glColor3f(0.44, 0.72, 0.34);
    glVertex2f(0, 520);    glVertex2f(800, 520);
    glColor3f(0.56, 0.82, 0.40);
    glVertex2f(800, 566);  glVertex2f(0, 566);
    glEnd();

    // paddy plot (young green paddy in rows)
    glColor3f(0.62, 0.80, 0.36);
    glBegin(GL_QUADS);
    glVertex2f(286, 524); glVertex2f(452, 524);
    glVertex2f(462, 558); glVertex2f(276, 558);
    glEnd();
    glColor3f(0.42, 0.64, 0.26);
    glLineWidth(1.2);
    glBegin(GL_LINES);
    glVertex2f(300, 526); glVertex2f(292, 556);
    glVertex2f(322, 526); glVertex2f(316, 556);
    glVertex2f(344, 526); glVertex2f(340, 556);
    glVertex2f(366, 526); glVertex2f(364, 556);
    glVertex2f(388, 526); glVertex2f(388, 556);
    glVertex2f(410, 526); glVertex2f(412, 556);
    glVertex2f(432, 526); glVertex2f(436, 556);
    glEnd();
    glLineWidth(1);
}


// ============================== Obj010_FarHouse1 ============================
// Far bank house number 1 - kept SMALL and HIGH so it sits far behind.
// POSITION : centre x = 70, base y = 552.  (raise base / shrink size = further)
void Obj010_FarHouse1() {
    // wall
    glColor3f(0.86, 0.82, 0.70);
    glBegin(GL_QUADS);
    glVertex2f(48, 552); glVertex2f(92, 552);
    glVertex2f(92, 576); glVertex2f(48, 576);
    glEnd();
    // thatched roof
    glColor3f(0.66, 0.34, 0.24);
    glBegin(GL_POLYGON);
    glVertex2f(42, 576); glVertex2f(98, 576);
    glVertex2f(88, 592); glVertex2f(52, 592);
    glEnd();
    // door + window
    glColor3f(0.45, 0.30, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(64, 552); glVertex2f(76, 552);
    glVertex2f(76, 568); glVertex2f(64, 568);
    glEnd();
    glColor3f(0.62, 0.48, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(80, 558); glVertex2f(88, 558);
    glVertex2f(88, 568); glVertex2f(80, 568);
    glEnd();
}


// ============================== Obj011_FarHouse2 ============================
// Far bank house number 2 (with a small veranda).
// POSITION : centre x = 225, base y = 552.
void Obj011_FarHouse2() {
    glColor3f(0.88, 0.83, 0.68);
    glBegin(GL_QUADS);
    glVertex2f(200, 552); glVertex2f(250, 552);
    glVertex2f(250, 578); glVertex2f(200, 578);
    glEnd();
    // veranda roof in front
    glColor3f(0.70, 0.38, 0.26);
    glBegin(GL_QUADS);
    glVertex2f(196, 570); glVertex2f(254, 570);
    glVertex2f(254, 576); glVertex2f(196, 576);
    glEnd();
    glColor3f(0.62, 0.32, 0.22);
    glBegin(GL_POLYGON);
    glVertex2f(194, 578); glVertex2f(256, 578);
    glVertex2f(244, 596); glVertex2f(206, 596);
    glEnd();
    glColor3f(0.45, 0.30, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(216, 552); glVertex2f(230, 552);
    glVertex2f(230, 570); glVertex2f(216, 570);
    glEnd();
    glColor3f(0.62, 0.48, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(236, 560); glVertex2f(246, 560);
    glVertex2f(246, 570); glVertex2f(236, 570);
    glEnd();
}


// ============================== Obj012_FarHouse3 ============================
// Far bank house number 3.
// POSITION : centre x = 610, base y = 552.
void Obj012_FarHouse3() {
    glColor3f(0.85, 0.80, 0.70);
    glBegin(GL_QUADS);
    glVertex2f(586, 552); glVertex2f(634, 552);
    glVertex2f(634, 574); glVertex2f(586, 574);
    glEnd();
    glColor3f(0.66, 0.32, 0.22);
    glBegin(GL_POLYGON);
    glVertex2f(580, 574); glVertex2f(640, 574);
    glVertex2f(628, 590); glVertex2f(592, 590);
    glEnd();
    glColor3f(0.45, 0.30, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(602, 552); glVertex2f(616, 552);
    glVertex2f(616, 566); glVertex2f(602, 566);
    glEnd();
    glColor3f(0.62, 0.48, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(620, 558); glVertex2f(630, 558);
    glVertex2f(630, 568); glVertex2f(620, 568);
    glEnd();
}


// ============================== Obj013_FarHouse4 ============================
// Far bank house number 4 (tin roof).
// POSITION : centre x = 755, base y = 552.
void Obj013_FarHouse4() {
    glColor3f(0.87, 0.81, 0.68);
    glBegin(GL_QUADS);
    glVertex2f(730, 552); glVertex2f(782, 552);
    glVertex2f(782, 576); glVertex2f(730, 576);
    glEnd();
    glColor3f(0.55, 0.58, 0.62);
    glBegin(GL_POLYGON);
    glVertex2f(724, 576); glVertex2f(788, 576);
    glVertex2f(776, 590); glVertex2f(736, 590);
    glEnd();
    glColor3f(0.45, 0.30, 0.18);
    glBegin(GL_QUADS);
    glVertex2f(748, 552); glVertex2f(762, 552);
    glVertex2f(762, 568); glVertex2f(748, 568);
    glEnd();
    glColor3f(0.62, 0.48, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(766, 558); glVertex2f(776, 558);
    glVertex2f(776, 570); glVertex2f(766, 570);
    glEnd();
}


// ============================ Obj014_VillageTemple ==========================
// Small village temple with three domes - it makes the far bank look like a
// real Bengali village.  (no flag anywhere in this scene)
// POSITION : centre x = 470, base y = 548.
void Obj014_VillageTemple() {
    // base platform
    glColor3f(0.80, 0.74, 0.62);
    glBegin(GL_QUADS);
    glVertex2f(444, 548); glVertex2f(496, 548);
    glVertex2f(496, 556); glVertex2f(444, 556);
    glEnd();
    // main room
    glColor3f(0.92, 0.86, 0.72);
    glBegin(GL_QUADS);
    glVertex2f(450, 556); glVertex2f(490, 556);
    glVertex2f(490, 586); glVertex2f(450, 586);
    glEnd();
    // middle dome + spire
    glColor3f(0.90, 0.42, 0.24);
    glBegin(GL_POLYGON);
    glVertex2f(450, 586); glVertex2f(490, 586);
    glVertex2f(482, 602); glVertex2f(470, 608);
    glVertex2f(458, 602);
    glEnd();
    glColor3f(0.85, 0.25, 0.18);
    glBegin(GL_TRIANGLES);
    glVertex2f(470, 608); glVertex2f(474, 622); glVertex2f(466, 622);
    glEnd();
    // two small side domes
    glColor3f(0.90, 0.42, 0.24);
    glBegin(GL_POLYGON);
    glVertex2f(444, 574); glVertex2f(458, 574);
    glVertex2f(454, 588); glVertex2f(448, 592);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2f(482, 574); glVertex2f(496, 574);
    glVertex2f(492, 592); glVertex2f(486, 588);
    glEnd();
    // doorway
    glColor3f(0.35, 0.22, 0.14);
    glBegin(GL_QUADS);
    glVertex2f(463, 556); glVertex2f(477, 556);
    glVertex2f(477, 576); glVertex2f(463, 576);
    glEnd();
}


// ============================== Obj015_FarTree1 =============================
// Round village tree on the far bank.
// POSITION : trunk base (140, 546), crown centre (140, 578).
void Obj015_FarTree1() {
    glColor3f(0.42, 0.28, 0.16);
    glBegin(GL_POLYGON);
    glVertex2f(136, 546); glVertex2f(145, 546);
    glVertex2f(143, 572); glVertex2f(138, 572);
    glEnd();
    glColor3f(0.16, 0.46, 0.20);
    circle(140, 578, 20);
    circle(122, 570, 14);
    circle(158, 570, 14);
    glColor3f(0.24, 0.58, 0.26);
    circle(140, 582, 15);
    circle(126, 574, 10);
    circle(154, 574, 10);
}


// ============================== Obj016_FarTree2 =============================
// Bigger round tree on the far bank.
// POSITION : trunk base (340, 544), crown centre (340, 582).
void Obj016_FarTree2() {
    glColor3f(0.42, 0.28, 0.16);
    glBegin(GL_POLYGON);
    glVertex2f(335, 544); glVertex2f(346, 544);
    glVertex2f(344, 574); glVertex2f(337, 574);
    glEnd();
    glColor3f(0.14, 0.44, 0.19);
    circle(340, 582, 24);
    circle(318, 572, 16);
    circle(362, 572, 16);
    glColor3f(0.22, 0.56, 0.25);
    circle(340, 586, 18);
    circle(322, 576, 12);
    circle(358, 576, 12);
}


// ============================== Obj017_FarTree3 =============================
// Round tree on the far bank, right side.
// POSITION : trunk base (700, 544), crown centre (700, 578).
void Obj017_FarTree3() {
    glColor3f(0.42, 0.28, 0.16);
    glBegin(GL_POLYGON);
    glVertex2f(696, 544); glVertex2f(705, 544);
    glVertex2f(703, 570); glVertex2f(698, 570);
    glEnd();
    glColor3f(0.15, 0.45, 0.20);
    circle(700, 578, 21);
    circle(682, 570, 14);
    circle(718, 570, 14);
    glColor3f(0.23, 0.57, 0.25);
    circle(700, 582, 16);
    circle(686, 574, 11);
    circle(714, 574, 11);
}


// ============================== Obj018_FarPalm1 =============================
// Tall palm tree - very typical of a Bengal village.
// POSITION : trunk base (270, 548), top of the trunk (262, 622).
void Obj018_FarPalm1() {
    glColor3f(0.45, 0.32, 0.18);
    glBegin(GL_POLYGON);
    glVertex2f(266, 548); glVertex2f(276, 548);
    glVertex2f(268, 622); glVertex2f(262, 622);
    glEnd();
    glColor3f(0.18, 0.48, 0.20);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(265, 622); glVertex2f(243, 636);
    glVertex2f(265, 622); glVertex2f(252, 646);
    glVertex2f(265, 622); glVertex2f(266, 650);
    glVertex2f(265, 622); glVertex2f(282, 644);
    glVertex2f(265, 622); glVertex2f(290, 632);
    glVertex2f(265, 622); glVertex2f(240, 620);
    glEnd();
    glLineWidth(1);
    glColor3f(0.35, 0.25, 0.12);
    circle(262, 618, 3, 8);
    circle(269, 618, 3, 8);
    circle(266, 614, 3, 8);
}


// ============================== Obj019_FarPalm2 =============================
// Second palm, leaning the other way.
// POSITION : trunk base (640, 548), top of the trunk (648, 616).
void Obj019_FarPalm2() {
    glColor3f(0.45, 0.32, 0.18);
    glBegin(GL_POLYGON);
    glVertex2f(636, 548); glVertex2f(645, 548);
    glVertex2f(651, 616); glVertex2f(645, 616);
    glEnd();
    glColor3f(0.18, 0.48, 0.20);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(648, 616); glVertex2f(628, 630);
    glVertex2f(648, 616); glVertex2f(636, 640);
    glVertex2f(648, 616); glVertex2f(650, 644);
    glVertex2f(648, 616); glVertex2f(666, 636);
    glVertex2f(648, 616); glVertex2f(672, 624);
    glVertex2f(648, 616); glVertex2f(624, 612);
    glEnd();
    glLineWidth(1);
    glColor3f(0.35, 0.25, 0.12);
    circle(645, 612, 3, 8);
    circle(652, 612, 3, 8);
    circle(649, 608, 3, 8);
}


// ============================= Obj020_FarCrowd1 =============================
// Villagers cheering on the far bank (left group).  Small = far away.
// POSITION : they stand on the line y = 524 (just above the water edge).
void Obj020_FarCrowd1() {
    // villager 1 (x = 95)
    glColor3f(0.1, 0.1, 0.1);        circle(95, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(95, 551, 5);
    glColor3f(0.9, 0.15, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(90, 536); glVertex2f(100, 536);
    glVertex2f(101, 544); glVertex2f(89, 544);
    glEnd();
    glColor3f(0.2, 0.5, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex2f(95, 538); glVertex2f(88, 524); glVertex2f(102, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(1.6);
    glBegin(GL_LINES);
    glVertex2f(90, 542); glVertex2f(86, 554);
    glVertex2f(100, 542); glVertex2f(104, 554);
    glEnd();
    glLineWidth(1);

    // villager 2 (x = 133)
    glColor3f(0.1, 0.1, 0.1);        circle(133, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(133, 551, 5);
    glColor3f(0.95, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(128, 536); glVertex2f(138, 536);
    glVertex2f(139, 544); glVertex2f(127, 544);
    glEnd();
    glColor3f(0.85, 0.35, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(133, 538); glVertex2f(126, 524); glVertex2f(140, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(1.6);
    glBegin(GL_LINES);
    glVertex2f(128, 542); glVertex2f(124, 554);
    glVertex2f(138, 542); glVertex2f(142, 554);
    glEnd();
    glLineWidth(1);

    // villager 3 (x = 171)
    glColor3f(0.1, 0.1, 0.1);        circle(171, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(171, 551, 5);
    glColor3f(0.1, 0.6, 0.8);
    glBegin(GL_QUADS);
    glVertex2f(166, 536); glVertex2f(176, 536);
    glVertex2f(177, 544); glVertex2f(165, 544);
    glEnd();
    glColor3f(0.6, 0.2, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex2f(171, 538); glVertex2f(164, 524); glVertex2f(178, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(1.6);
    glBegin(GL_LINES);
    glVertex2f(166, 542); glVertex2f(162, 554);
    glVertex2f(176, 542); glVertex2f(180, 554);
    glEnd();
    glLineWidth(1);
}


// ============================= Obj021_FarCrowd2 =============================
// Villagers cheering on the far bank (right group).
// POSITION : they stand on the line y = 524.
void Obj021_FarCrowd2() {
    // villager 4 (x = 596)
    glColor3f(0.1, 0.1, 0.1);        circle(596, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(596, 551, 5);
    glColor3f(0.85, 0.2, 0.5);
    glBegin(GL_QUADS);
    glVertex2f(591, 536); glVertex2f(601, 536);
    glVertex2f(602, 544); glVertex2f(590, 544);
    glEnd();
    glColor3f(0.1, 0.65, 0.3);
    glBegin(GL_TRIANGLES);
    glVertex2f(596, 538); glVertex2f(589, 524); glVertex2f(603, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(1.6);
    glBegin(GL_LINES);
    glVertex2f(591, 542); glVertex2f(587, 554);
    glVertex2f(601, 542); glVertex2f(605, 554);
    glEnd();

    // villager 5 (x = 634)
    glColor3f(0.1, 0.1, 0.1);        circle(634, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(634, 551, 5);
    glColor3f(0.95, 0.5, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(629, 536); glVertex2f(639, 536);
    glVertex2f(640, 544); glVertex2f(628, 544);
    glEnd();
    glColor3f(0.2, 0.35, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex2f(634, 538); glVertex2f(627, 524); glVertex2f(641, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2f(629, 542); glVertex2f(625, 554);
    glVertex2f(639, 542); glVertex2f(643, 554);
    glEnd();

    // villager 6 (x = 672)
    glColor3f(0.1, 0.1, 0.1);        circle(672, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(672, 551, 5);
    glColor3f(0.1, 0.7, 0.35);
    glBegin(GL_QUADS);
    glVertex2f(667, 536); glVertex2f(677, 536);
    glVertex2f(678, 544); glVertex2f(666, 544);
    glEnd();
    glColor3f(0.9, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(672, 538); glVertex2f(665, 524); glVertex2f(679, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2f(667, 542); glVertex2f(663, 554);
    glVertex2f(677, 542); glVertex2f(681, 554);
    glEnd();

    // villager 7 (x = 710)
    glColor3f(0.1, 0.1, 0.1);        circle(710, 553, 6);
    glColor3f(0.95, 0.8, 0.65);      circle(710, 551, 5);
    glColor3f(0.98, 0.85, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(705, 536); glVertex2f(715, 536);
    glVertex2f(716, 544); glVertex2f(704, 544);
    glEnd();
    glColor3f(0.8, 0.2, 0.6);
    glBegin(GL_TRIANGLES);
    glVertex2f(710, 538); glVertex2f(703, 524); glVertex2f(717, 524);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glBegin(GL_LINES);
    glVertex2f(705, 542); glVertex2f(701, 554);
    glVertex2f(715, 542); glVertex2f(719, 554);
    glEnd();
    glLineWidth(1);
}


// =============================== Obj022_River ===============================
// The river.  The water is STATIC (no waves) - only soft coloured bands and
// a few long streaks so it still looks like water.
// POSITION : band from y = 300 (near edge) to y = 520 (far edge).
void Obj022_River() {
    glBegin(GL_QUADS);
    glColor3f(0.30, 0.56, 0.80);
    glVertex2f(0, 300);    glVertex2f(800, 300);
    glColor3f(0.62, 0.83, 0.94);
    glVertex2f(800, 520);  glVertex2f(0, 520);
    glEnd();

    // long still streaks on the water (they never move)
    glColor3f(0.78, 0.90, 0.97);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(40, 318);   glVertex2f(150, 318);
    glVertex2f(300, 330);  glVertex2f(410, 330);
    glVertex2f(560, 316);  glVertex2f(680, 316);
    glVertex2f(90, 352);   glVertex2f(220, 352);
    glVertex2f(430, 356);  glVertex2f(540, 356);
    glVertex2f(650, 350);  glVertex2f(760, 350);
    glVertex2f(180, 396);  glVertex2f(310, 396);
    glVertex2f(490, 400);  glVertex2f(610, 400);
    glVertex2f(60, 440);   glVertex2f(190, 440);
    glVertex2f(320, 452);  glVertex2f(450, 452);
    glVertex2f(590, 446);  glVertex2f(720, 446);
    glVertex2f(120, 490);  glVertex2f(260, 490);
    glVertex2f(400, 496);  glVertex2f(530, 496);
    glVertex2f(620, 486);  glVertex2f(740, 486);
    glEnd();

}


// =============================== Obj023_Ducks ===============================
// Three ducks swimming near the near bank - a little village life on the water.
// POSITION : (250, 314), (276, 310), (300, 318).
void Obj023_Ducks() {
    // duck 1
    glColor3f(0.98, 0.98, 0.98);
    circle(250, 314, 8);
    glColor3f(0.96, 0.78, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(257, 315); glVertex2f(266, 313); glVertex2f(257, 318);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    circle(253, 317, 1.2, 8);

    // duck 2
    glColor3f(0.96, 0.96, 0.92);
    circle(276, 310, 7);
    glColor3f(0.96, 0.78, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(282, 311); glVertex2f(290, 309); glVertex2f(282, 314);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    circle(279, 313, 1.2, 8);

    // duck 3 (baby duck)
    glColor3f(1.0, 0.96, 0.72);
    circle(300, 318, 5);
    glColor3f(0.96, 0.78, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(304, 319); glVertex2f(310, 317); glVertex2f(304, 321);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    circle(302, 320, 1, 8);
}


// ============================= Obj024_StartLine =============================
// Start line of the race : a dashed rope across the river.  (no flag here)
// POSITION : x = RACE_START_X (90).  Change the #define to move it.
void Obj024_StartLine() {
    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(90, 312); glVertex2f(90, 322);
    glVertex2f(90, 330); glVertex2f(90, 340);
    glVertex2f(90, 348); glVertex2f(90, 358);
    glVertex2f(90, 366); glVertex2f(90, 376);
    glVertex2f(90, 384); glVertex2f(90, 394);
    glVertex2f(90, 402); glVertex2f(90, 412);
    glVertex2f(90, 420); glVertex2f(90, 430);
    glVertex2f(90, 438); glVertex2f(90, 448);
    glVertex2f(90, 456); glVertex2f(90, 466);
    glVertex2f(90, 474); glVertex2f(90, 484);
    glVertex2f(90, 492); glVertex2f(90, 502);
    glEnd();
    glLineWidth(1);
}


// ============================ Obj025_FinishLine =============================
// Finish line : black & white checkered band across the river + two posts.
// POSITION : x = RACE_FINISH_X (745).
void Obj025_FinishLine() {
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(740, 306); glVertex2f(750, 306);
    glVertex2f(750, 327); glVertex2f(740, 327);
    glVertex2f(740, 348); glVertex2f(750, 348);
    glVertex2f(750, 369); glVertex2f(740, 369);
    glVertex2f(740, 390); glVertex2f(750, 390);
    glVertex2f(750, 411); glVertex2f(740, 411);
    glVertex2f(740, 432); glVertex2f(750, 432);
    glVertex2f(750, 453); glVertex2f(740, 453);
    glVertex2f(740, 474); glVertex2f(750, 474);
    glVertex2f(750, 495); glVertex2f(740, 495);
    glEnd();

    glColor3f(0.08, 0.08, 0.08);
    glBegin(GL_QUADS);
    glVertex2f(740, 327); glVertex2f(750, 327);
    glVertex2f(750, 348); glVertex2f(740, 348);
    glVertex2f(740, 369); glVertex2f(750, 369);
    glVertex2f(750, 390); glVertex2f(740, 390);
    glVertex2f(740, 411); glVertex2f(750, 411);
    glVertex2f(750, 432); glVertex2f(740, 432);
    glVertex2f(740, 453); glVertex2f(750, 453);
    glVertex2f(750, 474); glVertex2f(740, 474);
    glVertex2f(740, 495); glVertex2f(750, 495);
    glVertex2f(750, 514); glVertex2f(740, 514);
    glEnd();

    // two posts holding the line
    glColor3f(0.25, 0.22, 0.18);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(745, 306); glVertex2f(745, 292);
    glVertex2f(745, 514); glVertex2f(745, 528);
    glEnd();
    glLineWidth(1);
}


// =============================== Obj026_Boat1 ===============================
// BOAT 1 - far lane, brown hull.
// POSITION : x = boat1X (global, rows from the start line to the finish line)
//            y = 478  (the lane)      size = 0.62 (small because it is far)
//            To move it to another lane change the 478 inside glTranslatef.
void Obj026_Boat1() {
    glPushMatrix();
    glTranslatef(boat1X, 478, 0);
    glScalef(0.62, 0.62, 1);

    float w = 2 * sin(raceTimer * 3);      // wake / sail pulse

    // soft reflection under the hull
    // foaming wake behind the stern
    // Boat reflection geometry is intentionally removed so no sail/hull reflection appears on the water.

    // hull : deep keel, raised bow and stern
    glColor3f(0.52, 0.30, 0.13);
    glBegin(GL_POLYGON);
    glVertex2f(-38, 7);  glVertex2f(-34, 1);
    glVertex2f(-18, -4); glVertex2f(0, -6);
    glVertex2f(18, -4);  glVertex2f(34, 1);
    glVertex2f(41, 7);   glVertex2f(36, 11);
    glVertex2f(33, 12);  glVertex2f(-30, 12);
    glEnd();

    // deck stripes
    glColor3f(0.35, 0.20, 0.09);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-30, 12); glVertex2f(33, 12);
    glVertex2f(-34, 1);  glVertex2f(34, 1);
    glEnd();

    // raised bow + stern end pieces
    glColor3f(0.42, 0.24, 0.11);
    glBegin(GL_TRIANGLES);
    glVertex2f(33, 12);  glVertex2f(41, 7);  glVertex2f(36, 7);
    glVertex2f(-38, 7);  glVertex2f(-30, 12); glVertex2f(-34, 7);
    glEnd();

    // mast
    glColor3f(0.35, 0.22, 0.10);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(0, 10); glVertex2f(0, 60);
    glEnd();

    // billowing sail
    glColor3f(0.97, 0.97, 0.95);
    glBegin(GL_POLYGON);
    glVertex2f(-1, 10); glVertex2f(-1, 58);
    glVertex2f(-14, 44); glVertex2f(-28, 26); glVertex2f(-22, 14);
    glEnd();
    glColor3f(0.65, 0.65, 0.65);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(-1, 58); glVertex2f(-28, 26);
    glEnd();

    // little flag on the mast top
    glColor3f(0.85, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 60); glVertex2f(17 + w, 60); glVertex2f(8, 51);
    glEnd();

    // oars
    glColor3f(0.40, 0.26, 0.12);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-20, 2); glVertex2f(-34, -10);
    glVertex2f(20, 2);  glVertex2f(34, -10);
    glEnd();

    // rower leaning forward
    glColor3f(0.1, 0.1, 0.1);        circle(-3, 16, 5);
    glColor3f(0.95, 0.8, 0.65);      circle(-3, 15, 4);
    glColor3f(0.90, 0.15, 0.15);
    glBegin(GL_QUADS);
    glVertex2f(-7, 9); glVertex2f(1, 9); glVertex2f(1, 16); glVertex2f(-7, 16);
    glEnd();
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-6, 16); glVertex2f(-12, 12);
    glVertex2f(0, 16);  glVertex2f(6, 12);
    glEnd();
    glLineWidth(1);

    glPopMatrix();
}


// =============================== Obj027_Boat2 ===============================
// BOAT 2 - second lane, deep red hull.
// POSITION : x = boat2X, y = 436 (lane), size = 0.70.
void Obj027_Boat2() {
    glPushMatrix();
    glTranslatef(boat2X, 436, 0);
    glScalef(0.70, 0.70, 1);

    float w = 2 * sin(raceTimer * 3 + 1);

    // Boat reflection and wake geometry removed so only the actual boat remains above the water.

    // Boat 2 hull - deep red racing boat body.
    glColor3f(0.62, 0.14, 0.12);
    glBegin(GL_POLYGON);
    glVertex2f(-38, 7);  glVertex2f(-34, 1);
    glVertex2f(-18, -4); glVertex2f(0, -6);
    glVertex2f(18, -4);  glVertex2f(34, 1);
    glVertex2f(41, 7);   glVertex2f(36, 11);
    glVertex2f(33, 12);  glVertex2f(-30, 12);
    glEnd();

    // Boat 2 deck stripes - dark red top and side guide lines.
    glColor3f(0.42, 0.08, 0.08);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-30, 12); glVertex2f(33, 12);
    glVertex2f(-34, 1);  glVertex2f(34, 1);
    glEnd();

    // Boat 2 raised bow and stern end pieces.
    glColor3f(0.52, 0.12, 0.10);
    glBegin(GL_TRIANGLES);
    glVertex2f(33, 12);  glVertex2f(41, 7);  glVertex2f(36, 7);
    glVertex2f(-38, 7);  glVertex2f(-30, 12); glVertex2f(-34, 7);
    glEnd();

    // Boat 2 mast - vertical wooden pole supporting the sail.
    glColor3f(0.35, 0.22, 0.10);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(0, 10); glVertex2f(0, 60);
    glEnd();

    // Boat 2 billowing sail - cream colored triangular sail.
    glColor3f(0.98, 0.96, 0.88);
    glBegin(GL_POLYGON);
    glVertex2f(-1, 10); glVertex2f(-1, 58);
    glVertex2f(-14, 44); glVertex2f(-28, 26); glVertex2f(-22, 14);
    glEnd();
    // Boat 2 sail edge seam - thin gray diagonal line.
    glColor3f(0.65, 0.65, 0.65);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(-1, 58); glVertex2f(-28, 26);
    glEnd();

    // Boat 2 mast-top flag - small red triangular flag.
    glColor3f(0.85, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 60); glVertex2f(17 + w, 60); glVertex2f(8, 51);
    glEnd();

    // Boat 2 oars - two wooden paddles extending from the hull.
    glColor3f(0.40, 0.26, 0.12);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-20, 2); glVertex2f(-34, -10);
    glVertex2f(20, 2);  glVertex2f(34, -10);
    glEnd();

    // Boat 2 rower - head and face.
    glColor3f(0.1, 0.1, 0.1);        circle(-3, 16, 5);
    glColor3f(0.95, 0.8, 0.65);      circle(-3, 15, 4);
    // Boat 2 rower clothing - green shirt.
    glColor3f(0.10, 0.65, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(-7, 9); glVertex2f(1, 9); glVertex2f(1, 16); glVertex2f(-7, 16);
    glEnd();
    // Boat 2 rower arms - simple dark strokes showing the rowing pose.
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-6, 16); glVertex2f(-12, 12);
    glVertex2f(0, 16);  glVertex2f(6, 12);
    glEnd();
    glLineWidth(1);

    glPopMatrix();
}


// =============================== Obj028_Boat3 ===============================
// BOAT 3 - middle lane, dark green hull.
// POSITION : x = boat3X, y = 392 (lane), size = 0.78.
void Obj028_Boat3() {
    glPushMatrix();
    glTranslatef(boat3X, 392, 0);
    glScalef(0.78, 0.78, 1);

    float w = 2 * sin(raceTimer * 3 + 2);

    // Boat reflection and wake geometry removed so only the actual boat remains above the water.

    // Boat 3 hull - dark green racing boat body.
    glColor3f(0.20, 0.42, 0.22);
    glBegin(GL_POLYGON);
    glVertex2f(-38, 7);  glVertex2f(-34, 1);
    glVertex2f(-18, -4); glVertex2f(0, -6);
    glVertex2f(18, -4);  glVertex2f(34, 1);
    glVertex2f(41, 7);   glVertex2f(36, 11);
    glVertex2f(33, 12);  glVertex2f(-30, 12);
    glEnd();

    // Boat 3 deck stripes - dark green guide lines on the hull.
    glColor3f(0.13, 0.30, 0.16);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-30, 12); glVertex2f(33, 12);
    glVertex2f(-34, 1);  glVertex2f(34, 1);
    glEnd();

    // Boat 3 raised bow and stern end pieces.
    glColor3f(0.18, 0.36, 0.19);
    glBegin(GL_TRIANGLES);
    glVertex2f(33, 12);  glVertex2f(41, 7);  glVertex2f(36, 7);
    glVertex2f(-38, 7);  glVertex2f(-30, 12); glVertex2f(-34, 7);
    glEnd();

    // Boat 3 mast - vertical wooden pole supporting the sail.
    glColor3f(0.35, 0.22, 0.10);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(0, 10); glVertex2f(0, 60);
    glEnd();

    // Boat 3 billowing sail - light cream triangular sail.
    glColor3f(0.96, 0.97, 0.93);
    glBegin(GL_POLYGON);
    glVertex2f(-1, 10); glVertex2f(-1, 58);
    glVertex2f(-14, 44); glVertex2f(-28, 26); glVertex2f(-22, 14);
    glEnd();
    // Boat 3 sail edge seam - thin gray diagonal line.
    glColor3f(0.65, 0.65, 0.65);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(-1, 58); glVertex2f(-28, 26);
    glEnd();

    // Boat 3 mast-top flag - small red triangular flag.
    glColor3f(0.85, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 60); glVertex2f(17 + w, 60); glVertex2f(8, 51);
    glEnd();

    // Boat 3 oars - two wooden paddles extending from the hull.
    glColor3f(0.40, 0.26, 0.12);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-20, 2); glVertex2f(-34, -10);
    glVertex2f(20, 2);  glVertex2f(34, -10);
    glEnd();

    // Boat 3 rower - head, face, clothing and rowing arms.
    glColor3f(0.1, 0.1, 0.1);        circle(-3, 16, 5);
    glColor3f(0.95, 0.8, 0.65);      circle(-3, 15, 4);
    glColor3f(0.95, 0.78, 0.10);
    glBegin(GL_QUADS);
    glVertex2f(-7, 9); glVertex2f(1, 9); glVertex2f(1, 16); glVertex2f(-7, 16);
    glEnd();
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-6, 16); glVertex2f(-12, 12);
    glVertex2f(0, 16);  glVertex2f(6, 12);
    glEnd();
    glLineWidth(1);

    glPopMatrix();
}


// =============================== Obj029_Boat4 ===============================
// BOAT 4 - second nearest lane, mustard hull.
// POSITION : x = boat4X, y = 348 (lane), size = 0.86.
void Obj029_Boat4() {
    glPushMatrix();
    glTranslatef(boat4X, 348, 0);
    glScalef(0.86, 0.86, 1);

    float w = 2 * sin(raceTimer * 3 + 3);

    // Boat reflection and wake geometry removed so only the actual boat remains above the water.

    // Boat 4 hull - mustard yellow racing boat body.
    glColor3f(0.72, 0.48, 0.10);
    glBegin(GL_POLYGON);
    glVertex2f(-38, 7);  glVertex2f(-34, 1);
    glVertex2f(-18, -4); glVertex2f(0, -6);
    glVertex2f(18, -4);  glVertex2f(34, 1);
    glVertex2f(41, 7);   glVertex2f(36, 11);
    glVertex2f(33, 12);  glVertex2f(-30, 12);
    glEnd();

    // Boat 4 deck stripes - brown-gold guide lines on the hull.
    glColor3f(0.52, 0.34, 0.07);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-30, 12); glVertex2f(33, 12);
    glVertex2f(-34, 1);  glVertex2f(34, 1);
    glEnd();

    // Boat 4 raised bow and stern end pieces.
    glColor3f(0.62, 0.42, 0.09);
    glBegin(GL_TRIANGLES);
    glVertex2f(33, 12);  glVertex2f(41, 7);  glVertex2f(36, 7);
    glVertex2f(-38, 7);  glVertex2f(-30, 12); glVertex2f(-34, 7);
    glEnd();

    // Boat 4 mast - vertical wooden pole supporting the sail.
    glColor3f(0.35, 0.22, 0.10);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(0, 10); glVertex2f(0, 60);
    glEnd();

    // Boat 4 billowing sail - light cream triangular sail.
    glColor3f(0.98, 0.98, 0.96);
    glBegin(GL_POLYGON);
    glVertex2f(-1, 10); glVertex2f(-1, 58);
    glVertex2f(-14, 44); glVertex2f(-28, 26); glVertex2f(-22, 14);
    glEnd();
    // Boat 4 sail edge seam - thin gray diagonal line.
    glColor3f(0.65, 0.65, 0.65);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(-1, 58); glVertex2f(-28, 26);
    glEnd();

    // Boat 4 mast-top flag - small red triangular flag.
    glColor3f(0.85, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 60); glVertex2f(17 + w, 60); glVertex2f(8, 51);
    glEnd();

    // Boat 4 oars - two wooden paddles extending from the hull.
    glColor3f(0.40, 0.26, 0.12);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-20, 2); glVertex2f(-34, -10);
    glVertex2f(20, 2);  glVertex2f(34, -10);
    glEnd();

    // Boat 4 rower - head, face, clothing and rowing arms.
    glColor3f(0.1, 0.1, 0.1);        circle(-3, 16, 5);
    glColor3f(0.95, 0.8, 0.65);      circle(-3, 15, 4);
    glColor3f(0.15, 0.35, 0.75);
    glBegin(GL_QUADS);
    glVertex2f(-7, 9); glVertex2f(1, 9); glVertex2f(1, 16); glVertex2f(-7, 16);
    glEnd();
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(-6, 16); glVertex2f(-12, 12);
    glVertex2f(0, 16);  glVertex2f(6, 12);
    glEnd();
    glLineWidth(1);

    glPopMatrix();
}

// =========================== Obj031_NearBank ================================
// Near (front) bank - the green village ground where the crowd stands.
// POSITION : band from y = 0 (bottom of the window) up to y = 300 (water).
void Obj031_NearBank() {
    glBegin(GL_QUADS);
    glColor3f(0.38, 0.70, 0.30);
    glVertex2f(0, 0);      glVertex2f(800, 0);
    glColor3f(0.58, 0.85, 0.42);
    glVertex2f(800, 300);  glVertex2f(0, 300);
    glEnd();
}


// =========================== Obj032_VillagePath =============================
// Sandy village path that runs from the front of the picture up to the ghat.
// POSITION : starts at the bottom (x 40..130) and ends at the water (x 450..520)
void Obj032_VillagePath() {
    glColor3f(0.82, 0.71, 0.48);
    glBegin(GL_POLYGON);
    glVertex2f(90, 0);    glVertex2f(140, 120);
    glVertex2f(300, 226); glVertex2f(450, 300);
    glVertex2f(520, 300); glVertex2f(360, 222);
    glVertex2f(215, 112); glVertex2f(180, 0);
    glEnd();
    
}
// ============================ Obj034_NearHouse2 =============================
// Near bank house (right) : bigger, tin roof, with a courtyard.
// POSITION : wall from x = 622 to x = 760, base y = 70, wall top y = 142.
void Obj034_NearHouse2() {
    glColor3f(0.88, 0.78, 0.58);
    glBegin(GL_QUADS);
    glVertex2f(622, 70);  glVertex2f(760, 70);
    glVertex2f(760, 142); glVertex2f(622, 142);
    glEnd();
    glColor3f(0.72, 0.58, 0.40);
    glBegin(GL_QUADS);
    glVertex2f(618, 58);  glVertex2f(764, 58);
    glVertex2f(764, 72);  glVertex2f(618, 72);
    glEnd();
    // thatched roof
glColor3f(0.64, 0.28, 0.14);

glBegin(GL_POLYGON);

glVertex2f(610, 142);
glVertex2f(772, 142);

glVertex2f(758, 180);
glVertex2f(628, 180);

glEnd();


// roof ridge + thatch lines
glColor3f(0.86, 0.46, 0.16);

glLineWidth(2);

glBegin(GL_LINES);

// top/ridge line
glVertex2f(628, 180);
glVertex2f(758, 180);

// thatch line
glVertex2f(618, 150);
glVertex2f(766, 150);

glEnd();

glLineWidth(1);
    // door + two windows
    glColor3f(0.40, 0.24, 0.11);
    glBegin(GL_QUADS);
    glVertex2f(676, 72);  glVertex2f(706, 72);
    glVertex2f(706, 118); glVertex2f(676, 118);
    glEnd();
    glColor3f(0.95, 0.85, 0.30);
    circle(686, 96, 2.2, 8);
    circle(697, 96, 2.2, 8);
    glColor3f(0.55, 0.38, 0.20);
    glBegin(GL_QUADS);
    glVertex2f(636, 96);  glVertex2f(660, 96);
    glVertex2f(660, 120); glVertex2f(636, 120);
    glVertex2f(722, 96);  glVertex2f(746, 96);
    glVertex2f(746, 120); glVertex2f(722, 120);
    glEnd();
    glColor3f(0.96, 0.94, 0.86);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(648, 96);  glVertex2f(648, 120);
    glVertex2f(636, 108); glVertex2f(660, 108);
    glVertex2f(734, 96);  glVertex2f(734, 120);
    glVertex2f(722, 108); glVertex2f(746, 108);
    glEnd();
    glLineWidth(1);
    // marigold garland over the door
    glColor3f(0.95, 0.55, 0.05);
    circle(681, 122, 3, 8);  circle(691, 124, 3, 8);
    circle(701, 122, 3, 8);
    glColor3f(0.90, 0.15, 0.10);
    circle(686, 126, 2.4, 8); circle(696, 126, 2.4, 8);
    // water pots beside the house
    glColor3f(0.78, 0.42, 0.16);
    circle(600, 82, 10, 16);
    glColor3f(0.30, 0.22, 0.14);
    glBegin(GL_QUADS);
    glVertex2f(594, 84); glVertex2f(606, 84);
    glVertex2f(606, 88); glVertex2f(594, 88);
    glEnd();
}

// ============================= Obj037_NearTree2 =============================
// Big tree on the near bank (right side).
// POSITION : trunk base (580, 96), crown centre (580, 188).
void Obj037_NearTree2() {
    glColor3f(0.42, 0.26, 0.14);
    glBegin(GL_POLYGON);
    glVertex2f(573, 96);  glVertex2f(587, 96);
    glVertex2f(583, 174); glVertex2f(577, 174);
    glEnd();
    glColor3f(0.13, 0.48, 0.17);
    circle(580, 190, 40);
    circle(544, 170, 26);
    circle(616, 172, 26);
    glColor3f(0.22, 0.62, 0.24);
    circle(580, 196, 30);
    circle(550, 176, 19);
    circle(610, 178, 19);
    glColor3f(0.9, 0.1, 0.1);     circle(562, 204, 3.5);
    glColor3f(0.98, 0.85, 0.12);  circle(598, 200, 3.5);
    glColor3f(1.0, 1.0, 1.0);     circle(580, 172, 3);
}


// ============================== Obj038_NearPalm =============================
// Tall palm at the right edge of the near bank.
// POSITION : trunk base (762, 58), top of the trunk (768, 186).
void Obj038_NearPalm() {
    glColor3f(0.46, 0.32, 0.18);
    glBegin(GL_POLYGON);
    glVertex2f(757, 58);  glVertex2f(769, 58);
    glVertex2f(772, 186); glVertex2f(764, 186);
    glEnd();
    glColor3f(0.18, 0.50, 0.20);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(768, 186); glVertex2f(736, 202);
    glVertex2f(768, 186); glVertex2f(748, 216);
    glVertex2f(768, 186); glVertex2f(766, 222);
    glVertex2f(768, 186); glVertex2f(790, 214);
    glVertex2f(768, 186); glVertex2f(798, 198);
    glVertex2f(768, 186); glVertex2f(734, 180);
    glEnd();
    glLineWidth(1);
    glColor3f(0.36, 0.26, 0.12);
    circle(764, 182, 4, 8);
    circle(772, 182, 4, 8);
    circle(768, 176, 4, 8);
}

// ================================ Obj040_Cow ================================
// A cow grazing on the near bank - every village has one.
// POSITION : body from x = 382 to x = 452, base y = 118, head at (462, 158).
void Obj040_Cow() {
    // body
    glColor3f(0.97, 0.96, 0.92);
    glBegin(GL_POLYGON);
    glVertex2f(382, 140); glVertex2f(448, 140);
    glVertex2f(456, 152); glVertex2f(450, 170);
    glVertex2f(396, 172); glVertex2f(380, 158);
    glEnd();
    // brown patches
    glColor3f(0.42, 0.32, 0.22);
    circle(408, 160, 9, 12);
    circle(432, 150, 7, 12);
    // head
    glColor3f(0.97, 0.96, 0.92);
    circle(464, 162, 13, 16);
    glColor3f(0.86, 0.72, 0.66);
    circle(474, 156, 6, 12);
    glColor3f(0.2, 0.2, 0.2);
    circle(468, 168, 1.6, 8);
    // horns
    glColor3f(0.85, 0.78, 0.62);
    glBegin(GL_TRIANGLES);
    glVertex2f(456, 174); glVertex2f(450, 186); glVertex2f(462, 178);
    glVertex2f(472, 174); glVertex2f(478, 186); glVertex2f(468, 178);
    glEnd();
    // legs
    glColor3f(0.94, 0.93, 0.88);
    glLineWidth(5);
    glBegin(GL_LINES);
    glVertex2f(390, 142); glVertex2f(390, 118);
    glVertex2f(404, 142); glVertex2f(404, 118);
    glVertex2f(434, 142); glVertex2f(434, 118);
    glVertex2f(446, 142); glVertex2f(446, 118);
    glEnd();
    glLineWidth(1);
    // tail
    glColor3f(0.94, 0.93, 0.88);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(381, 166); glVertex2f(372, 146);
    glEnd();
    glColor3f(0.35, 0.28, 0.20);
    circle(371, 142, 4, 10);
    glLineWidth(1);
}
// =========================== Obj042_WomanPitcher2 ===========================
// Second woman carrying a pitcher, walking the other way.
// NEW POSITION : feet around (500, 80), pitcher around (500, 142).

void Obj042_WomanPitcher2() {

    // hair
    glColor3f(0.1, 0.1, 0.1);
    circle(500, 128, 10);

    // face
    glColor3f(0.95, 0.8, 0.65);
    circle(500, 125, 8);

    // eyes
    glColor3f(0, 0, 0);
    circle(497, 126, 1.3);
    circle(503, 126, 1.3);

    // blouse
    glColor3f(0.9, 0.35, 0.55);

    glBegin(GL_QUADS);

    glVertex2f(490, 100);
    glVertex2f(510, 100);

    glVertex2f(512, 116);
    glVertex2f(488, 116);

    glEnd();

    // saree
    glColor3f(0.95, 0.75, 0.20);

    glBegin(GL_POLYGON);

    glVertex2f(488, 104);
    glVertex2f(512, 104);

    glVertex2f(518, 80);
    glVertex2f(482, 80);

    glEnd();

    // saree border
    glColor3f(0.75, 0.15, 0.35);

    glLineWidth(2);

    glBegin(GL_LINES);

    glVertex2f(483, 84);
    glVertex2f(513, 98);

    glEnd();

    // legs + arms
    glColor3f(0.2, 0.2, 0.2);

    glLineWidth(2.5);

    glBegin(GL_LINES);

    // legs
    glVertex2f(494, 80);
    glVertex2f(494, 70);

    glVertex2f(506, 80);
    glVertex2f(506, 70);

    // arms
    glVertex2f(510, 110);
    glVertex2f(518, 98);

    glVertex2f(490, 110);
    glVertex2f(484, 128);

    glEnd();

    glLineWidth(1);

    // pitcher on the head
    glColor3f(0.80, 0.44, 0.16);
    circle(500, 142, 10, 16);

    // pitcher top
    glColor3f(0.55, 0.30, 0.12);

    glBegin(GL_QUADS);

    glVertex2f(494, 147);
    glVertex2f(506, 147);

    glVertex2f(506, 153);
    glVertex2f(494, 153);

    glEnd();
}


// ========================== Obj047_FishingPlatform ==========================
// Bamboo fishing platform (machan) standing in the water at the edge of the
// near bank, with a fisherman and his rod.
// POSITION : deck from x = 462 to x = 556, from y = 300 to y = 318.
void Obj047_FishingPlatform() {
    // bamboo poles going down into the water
    glColor3f(0.58, 0.44, 0.20);
    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex2f(470, 300); glVertex2f(468, 286);
    glVertex2f(500, 300); glVertex2f(500, 286);
    glVertex2f(530, 300); glVertex2f(532, 286);
    glEnd();
    glLineWidth(1);

    // deck
    glColor3f(0.72, 0.58, 0.30);
    glBegin(GL_QUADS);
    glVertex2f(462, 302); glVertex2f(556, 302);
    glVertex2f(556, 316); glVertex2f(462, 316);
    glEnd();
    glColor3f(0.55, 0.42, 0.20);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(462, 309); glVertex2f(556, 309);
    glVertex2f(486, 302); glVertex2f(486, 316);
    glVertex2f(510, 302); glVertex2f(510, 316);
    glVertex2f(534, 302); glVertex2f(534, 316);
    glEnd();

    // fisherman sitting on the deck
    glColor3f(0.1, 0.1, 0.1);        circle(490, 344, 8);
    glColor3f(0.95, 0.8, 0.65);      circle(490, 342, 6.5);
    glColor3f(0.9, 0.85, 0.55);
    glBegin(GL_QUADS);
    glVertex2f(483, 320); glVertex2f(497, 320);
    glVertex2f(499, 336); glVertex2f(481, 336);
    glEnd();
    glColor3f(0.45, 0.35, 0.25);
    glBegin(GL_QUADS);
    glVertex2f(481, 316); glVertex2f(503, 316);
    glVertex2f(499, 322); glVertex2f(483, 322);
    glEnd();
    // bamboo fishing rod + line
    glColor3f(0.55, 0.42, 0.18);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(497, 332); glVertex2f(546, 366);
    glEnd();
    glColor3f(0.85, 0.85, 0.85);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(546, 366); glVertex2f(546, 340);
    glEnd();
    glColor3f(0.95, 0.15, 0.15);
    circle(546, 338, 2.5, 8);
    // catch basket
    glColor3f(0.72, 0.52, 0.24);
    glBegin(GL_POLYGON);
    glVertex2f(464, 316); glVertex2f(482, 316);
    glVertex2f(478, 330); glVertex2f(468, 330);
    glEnd();
}


// ============================== Obj048_MooredBoat ===========================
// Small fishing boat tied up beside the platform.
// POSITION : centre (600, 312).
void Obj048_MooredBoat() {
    // Moored fishing boat hull - small brown boat beside the platform.
    glColor3f(0.58, 0.38, 0.18);
    glBegin(GL_POLYGON);
    glVertex2f(566, 314); glVertex2f(570, 308);
    glVertex2f(584, 305); glVertex2f(616, 305);
    glVertex2f(630, 308); glVertex2f(634, 314);
    glVertex2f(614, 318); glVertex2f(586, 318);
    glEnd();
    // Moored boat upper rim - dark wooden edge.
    glColor3f(0.42, 0.26, 0.12);
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(570, 308); glVertex2f(630, 308);
    glEnd();
    // pole stuck in the water + rope
    glColor3f(0.5, 0.38, 0.18);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(640, 300); glVertex2f(640, 330);
    glEnd();
    glColor3f(0.85, 0.78, 0.6);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(634, 312); glVertex2f(640, 318);
    glEnd();
    // oar resting inside
    // Moored boat oar - wooden paddle resting across the boat.
    glColor3f(0.45, 0.32, 0.15);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(578, 310); glVertex2f(624, 316);
    glEnd();
    glLineWidth(1);
}


// ============================ Obj049_CheerCrowdLeft =========================
// Villagers cheering on the near bank (left group) - both arms in the air.
// POSITION : they stand on the line y = 238 (see the y numbers below).
void Obj049_CheerCrowdLeft() {
    // villager 1 (x = 45)
    glColor3f(0.1, 0.1, 0.1);        circle(45, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(45, 283, 8);
    glColor3f(0, 0, 0);              circle(42, 284, 1.3); circle(48, 284, 1.3);
    glColor3f(0.9, 0.12, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(35, 260); glVertex2f(55, 260);
    glVertex2f(57, 275); glVertex2f(33, 275);
    glEnd();
    glColor3f(0.2, 0.45, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex2f(45, 263); glVertex2f(31, 238); glVertex2f(59, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(41, 246); glVertex2f(41, 238);
    glVertex2f(49, 246); glVertex2f(49, 238);
    glVertex2f(35, 270); glVertex2f(27, 288);
    glVertex2f(55, 270); glVertex2f(63, 288);
    glEnd();
    glLineWidth(1);

    // villager 2 (x = 100)
    glColor3f(0.1, 0.1, 0.1);        circle(100, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(100, 283, 8);
    glColor3f(0, 0, 0);              circle(97, 284, 1.3); circle(103, 284, 1.3);
    glColor3f(0.95, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(90, 260); glVertex2f(110, 260);
    glVertex2f(112, 275); glVertex2f(88, 275);
    glEnd();
    glColor3f(0.85, 0.35, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(100, 263); glVertex2f(86, 238); glVertex2f(114, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(96, 246); glVertex2f(96, 238);
    glVertex2f(104, 246); glVertex2f(104, 238);
    glVertex2f(90, 270); glVertex2f(82, 288);
    glVertex2f(110, 270); glVertex2f(118, 288);
    glEnd();
    glLineWidth(1);

    // villager 3 (x = 155)
    glColor3f(0.1, 0.1, 0.1);        circle(155, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(155, 283, 8);
    glColor3f(0, 0, 0);              circle(152, 284, 1.3); circle(158, 284, 1.3);
    glColor3f(0.1, 0.62, 0.82);
    glBegin(GL_QUADS);
    glVertex2f(145, 260); glVertex2f(165, 260);
    glVertex2f(167, 275); glVertex2f(143, 275);
    glEnd();
    glColor3f(0.55, 0.18, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex2f(155, 263); glVertex2f(141, 238); glVertex2f(169, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(151, 246); glVertex2f(151, 238);
    glVertex2f(159, 246); glVertex2f(159, 238);
    glVertex2f(145, 270); glVertex2f(137, 288);
    glVertex2f(165, 270); glVertex2f(173, 288);
    glEnd();
    glLineWidth(1);
}


// =========================== Obj050_CheerCrowdRight =========================
// Villagers cheering on the near bank (right group).
// POSITION : they stand on the line y = 238.
void Obj050_CheerCrowdRight() {
    // villager 6 (x = 400)
    glColor3f(0.1, 0.1, 0.1);        circle(400, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(400, 283, 8);
    glColor3f(0, 0, 0);              circle(397, 284, 1.3); circle(403, 284, 1.3);
    glColor3f(0.95, 0.5, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(390, 260); glVertex2f(410, 260);
    glVertex2f(412, 275); glVertex2f(388, 275);
    glEnd();
    glColor3f(0.3, 0.3, 0.35);
    glBegin(GL_TRIANGLES);
    glVertex2f(400, 263); glVertex2f(386, 238); glVertex2f(414, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(396, 246); glVertex2f(396, 238);
    glVertex2f(404, 246); glVertex2f(404, 238);
    glVertex2f(390, 270); glVertex2f(382, 288);
    glVertex2f(410, 270); glVertex2f(418, 288);
    glEnd();
    glLineWidth(1);

    // villager 7 (x = 585)
    glColor3f(0.1, 0.1, 0.1);        circle(585, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(585, 283, 8);
    glColor3f(0, 0, 0);              circle(582, 284, 1.3); circle(588, 284, 1.3);
    glColor3f(0.9, 0.12, 0.12);
    glBegin(GL_QUADS);
    glVertex2f(575, 260); glVertex2f(595, 260);
    glVertex2f(597, 275); glVertex2f(573, 275);
    glEnd();
    glColor3f(0.2, 0.45, 0.8);
    glBegin(GL_TRIANGLES);
    glVertex2f(585, 263); glVertex2f(571, 238); glVertex2f(599, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(581, 246); glVertex2f(581, 238);
    glVertex2f(589, 246); glVertex2f(589, 238);
    glVertex2f(575, 270); glVertex2f(567, 288);
    glVertex2f(595, 270); glVertex2f(603, 288);
    glEnd();
    glLineWidth(1);

    // villager 8 (x = 640)
    glColor3f(0.1, 0.1, 0.1);        circle(640, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(640, 283, 8);
    glColor3f(0, 0, 0);              circle(637, 284, 1.3); circle(643, 284, 1.3);
    glColor3f(0.1, 0.7, 0.35);
    glBegin(GL_QUADS);
    glVertex2f(630, 260); glVertex2f(650, 260);
    glVertex2f(652, 275); glVertex2f(628, 275);
    glEnd();
    glColor3f(0.9, 0.15, 0.15);
    glBegin(GL_TRIANGLES);
    glVertex2f(640, 263); glVertex2f(626, 238); glVertex2f(654, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(636, 246); glVertex2f(636, 238);
    glVertex2f(644, 246); glVertex2f(644, 238);
    glVertex2f(630, 270); glVertex2f(622, 288);
    glVertex2f(650, 270); glVertex2f(658, 288);
    glEnd();
    glLineWidth(1);

    // villager 9 (x = 695)
    glColor3f(0.1, 0.1, 0.1);        circle(695, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(695, 283, 8);
    glColor3f(0, 0, 0);              circle(692, 284, 1.3); circle(698, 284, 1.3);
    glColor3f(0.95, 0.8, 0.1);
    glBegin(GL_QUADS);
    glVertex2f(685, 260); glVertex2f(705, 260);
    glVertex2f(707, 275); glVertex2f(683, 275);
    glEnd();
    glColor3f(0.6, 0.2, 0.75);
    glBegin(GL_TRIANGLES);
    glVertex2f(695, 263); glVertex2f(681, 238); glVertex2f(709, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(691, 246); glVertex2f(691, 238);
    glVertex2f(699, 246); glVertex2f(699, 238);
    glVertex2f(685, 270); glVertex2f(677, 288);
    glVertex2f(705, 270); glVertex2f(713, 288);
    glEnd();
    glLineWidth(1);

    // villager 10 (x = 750)
    glColor3f(0.1, 0.1, 0.1);        circle(750, 286, 10);
    glColor3f(0.95, 0.8, 0.65);      circle(750, 283, 8);
    glColor3f(0, 0, 0);              circle(747, 284, 1.3); circle(753, 284, 1.3);
    glColor3f(0.1, 0.62, 0.82);
    glBegin(GL_QUADS);
    glVertex2f(740, 260); glVertex2f(760, 260);
    glVertex2f(762, 275); glVertex2f(738, 275);
    glEnd();
    glColor3f(0.95, 0.95, 0.92);
    glBegin(GL_TRIANGLES);
    glVertex2f(750, 263); glVertex2f(736, 238); glVertex2f(764, 238);
    glEnd();
    glColor3f(0.2, 0.2, 0.2);
    glLineWidth(2.5);
    glBegin(GL_LINES);
    glVertex2f(746, 246); glVertex2f(746, 238);
    glVertex2f(754, 246); glVertex2f(754, 238);
    glVertex2f(740, 270); glVertex2f(732, 288);
    glVertex2f(760, 270); glVertex2f(768, 288);
    glEnd();
    glLineWidth(1);
}


// ============================== Obj051_Bunting ==============================
// Pohela Boishakh bunting (red / yellow / green / orange flags) across the sky.
// POSITION : rope from (0, 700) to (800, 700), 10 flags hanging 24 down.
void Obj051_Bunting() {
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
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(20, 702); glVertex2f(56, 698); glVertex2f(38, 676);
    glEnd();
    glColor3f(0.98, 0.85, 0.12);
    glBegin(GL_TRIANGLES);
    glVertex2f(96, 687); glVertex2f(132, 684); glVertex2f(114, 662);
    glEnd();
    glColor3f(0.1, 0.7, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(176, 681); glVertex2f(212, 679); glVertex2f(194, 657);
    glEnd();
    glColor3f(0.98, 0.5, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(256, 677); glVertex2f(292, 676); glVertex2f(274, 654);
    glEnd();
    glColor3f(0.2, 0.4, 0.9);
    glBegin(GL_TRIANGLES);
    glVertex2f(336, 675); glVertex2f(372, 675); glVertex2f(354, 653);
    glEnd();
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(416, 675); glVertex2f(452, 676); glVertex2f(434, 654);
    glEnd();
    glColor3f(0.98, 0.85, 0.12);
    glBegin(GL_TRIANGLES);
    glVertex2f(496, 677); glVertex2f(532, 679); glVertex2f(514, 657);
    glEnd();
    glColor3f(0.1, 0.7, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(576, 681); glVertex2f(612, 684); glVertex2f(594, 662);
    glEnd();
    glColor3f(0.98, 0.5, 0.1);
    glBegin(GL_TRIANGLES);
    glVertex2f(656, 687); glVertex2f(692, 690); glVertex2f(674, 668);
    glEnd();
    glColor3f(0.8, 0.2, 0.7);
    glBegin(GL_TRIANGLES);
    glVertex2f(736, 698); glVertex2f(772, 702); glVertex2f(754, 680);
    glEnd();
}


// =============================== Obj052_Kite ================================
// A kite flying high above the village, it sways with kiteTimer.
// POSITION : centre (400 + 70 * sin(kiteTimer), 640).
void Obj052_Kite() {
    float kx = 400 + 70 * sin(kiteTimer);
    float ky = 640;
    // string down to the boy on the bank
    glColor3f(0.3, 0.3, 0.3);
    glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(kx, ky - 26); glVertex2f(75, 252);
    glEnd();
    // kite : red left half, green right half
    glColor3f(0.95, 0.1, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(kx, ky + 24); glVertex2f(kx - 22, ky); glVertex2f(kx, ky - 24);
    glEnd();
    glColor3f(0.1, 0.8, 0.2);
    glBegin(GL_TRIANGLES);
    glVertex2f(kx, ky + 24); glVertex2f(kx + 22, ky); glVertex2f(kx, ky - 24);
    glEnd();
    // cross + tail
    glColor3f(0.1, 0.1, 0.1);
    glLineWidth(1.5);
    glBegin(GL_LINES);
    glVertex2f(kx, ky + 24); glVertex2f(kx, ky - 24);
    glVertex2f(kx - 22, ky); glVertex2f(kx + 22, ky);
    glVertex2f(kx, ky - 24); glVertex2f(kx - 10, ky - 40);
    glVertex2f(kx - 10, ky - 40); glVertex2f(kx + 6, ky - 54);
    //glVertex2f(kx + 6, ky - 54); glVertex2f(kx - 8, ky - 68);
    glEnd();
    
    glLineWidth(1);
}

// ================================ RACE LINES ================================
// (these two numbers are used by Obj024 / Obj025 and by update())
#define RACE_START_X   90      // x of the start line  (boats start here)
#define RACE_FINISH_X  745     // x of the finish line (boats wrap back after)


// =========================== display + animation ============================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // ---------------- sky, sun, clouds, birds, kite ----------------
    Obj001_Sky();
    Obj002_Sun();

    // clouds are cut off at the sky line (clipping with glScissor)
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 545, 800, 255);
    Obj003_Cloud1();
    Obj004_Cloud2();
    Obj005_Cloud3();
    Obj006_Cloud4();
    glDisable(GL_SCISSOR_TEST);

    Obj007_Birds();

    // kite and its string above the village
    Obj052_Kite();

    // ---------------- far bank (behind the river) ------------------
    Obj008_DistantHills();
    Obj009_FarField();
    Obj010_FarHouse1();      // these houses sit high = far behind
    Obj011_FarHouse2();
    Obj012_FarHouse3();
    Obj013_FarHouse4();
    Obj014_VillageTemple();
    Obj015_FarTree1();
    Obj016_FarTree2();
    Obj017_FarTree3();
    Obj018_FarPalm1();
    Obj019_FarPalm2();
    Obj020_FarCrowd1();
    Obj021_FarCrowd2();

    // ---------------- river ----------------------------------------
    Obj022_River();
    Obj023_Ducks();
    Obj024_StartLine();
    Obj025_FinishLine();

    // ---------------- the four racing boats (far -> near) ----------
    Obj026_Boat1();
    Obj027_Boat2();
    Obj028_Boat3();
    Obj029_Boat4();

    // ---------------- near bank ------------------------------------
    Obj031_NearBank();
    Obj032_VillagePath();
    Obj034_NearHouse2();
    Obj037_NearTree2();
    Obj038_NearPalm();
    Obj040_Cow();
    Obj042_WomanPitcher2();

    // ---------------- things standing in / on the water ------------
    Obj047_FishingPlatform();
    Obj048_MooredBoat();

    // near-bank cheering villagers on the left
    Obj049_CheerCrowdLeft();
    // near-bank cheering villagers on the right
    Obj050_CheerCrowdRight();
    // Pohela Boishakh flags across the sky
    Obj051_Bunting();
    // help line at the top of the window
    glColor3f(0.15, 0.15, 0.15);
    text(400, 772, "s = start   p = pause   r = restart   Esc = quit");

    glutSwapBuffers();
}

// runs every 16 ms : move the sun, the clouds, the birds and the boats
void update(int value) {
    // sun rises and stops at 705
    if (sunY < 705) { sunY += 0.5; if (sunY > 705) sunY = 705; }

    // clouds drift (change these numbers for faster / slower clouds)
    cloudX1 += 0.16;  if (cloudX1 > 900)  cloudX1 = -100;
    cloudX3 += 0.11;  if (cloudX3 > 900)  cloudX3 = -100;
    cloudX2 -= 0.13;  if (cloudX2 < -100) cloudX2 = 900;
    cloudX4 -= 0.08;  if (cloudX4 < -100) cloudX4 = 900;

    // birds fly across the sky
    birdX += 2.2;
    if (birdX > 940) birdX = -140;
    birdTimer += 0.05;

    // kite sways
    kiteTimer += 0.02;

    // sails, wakes and oars pulse (always, even when paused)
    raceTimer += 0.05;

    // the race : each boat rows to the finish line and starts again
    if (!boatsPaused) {
        boat1X += boat1Speed; if (boat1X > RACE_FINISH_X + 60) boat1X = RACE_START_X - 60;
        boat2X += boat2Speed; if (boat2X > RACE_FINISH_X + 60) boat2X = RACE_START_X - 60;
        boat3X += boat3Speed; if (boat3X > RACE_FINISH_X + 60) boat3X = RACE_START_X - 60;
        boat4X += boat4Speed; if (boat4X > RACE_FINISH_X + 60) boat4X = RACE_START_X - 60;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// s = start   p = pause   r = restart   Esc = quit
void keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') boatsPaused = false;         // start / resume
    else if (key == 'p' || key == 'P') boatsPaused = true;     // pause
    else if (key == 'r' || key == 'R') {                       // restart race
        boatsPaused = false;
        boat1X = RACE_START_X;
        boat2X = RACE_START_X;
        boat3X = RACE_START_X;
        boat4X = RACE_START_X;
    }
    else if (key == 27) exit(0);                               // Esc = quit
    glutPostRedisplay();
}

void init() {
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 800);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Pohela Boishakh - Village Boat Race (Scene 2)");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);       // s / p / r / Esc
    glutTimerFunc(0, update, 0);      // animation loop
    glutMainLoop();

    return 0;
}
