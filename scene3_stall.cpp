#include <GL/glut.h>
#include <cmath>
#include <cstring>
#define M_PI 3.14159265358979323846

// ==================== ANIMATION GLOBAL VARIABLES ====================
// These variables control continuous animation in the scene
float ferrisWheelAngle = 0.0f;  // Rotation angle of Ferris Wheel (nagordola)
float balloonWaveTime = 0.0f;   // Time variable for balloon floating + chorki spinning + dancer
float kitePhase = 0.0f;         // Phase angle (0 to 2*PI) to move kite in oval path

// ==================== STRUCT FOR WALKING CROWD ====================
struct Person {
    float x, y;          // Current position in world coordinates (0,800,0,800)
    float scale;         // Size scale: <1.0 = far away, >1.0 = close to camera
    float speed;         // Walking speed per frame
    int direction;       // 1 = moves right, -1 = moves left
    float r, g, b;       // Shirt color (RGB 0-1)
};

// 24 people divided into 3 depth layers
Person people[24] = {
    // Background layer (y ~ 212-226, small scale 0.65-0.75) - farthest, slowest
    { 680.0f, 220.0f, 0.7f,  0.60f, -1, 0.1f, 0.2f, 0.8f},
    { 280.0f, 214.0f, 0.75f, 0.80f,  1, 0.9f, 0.7f, 0.1f},
    { 560.0f, 226.0f, 0.65f, 0.72f, -1, 0.1f, 0.6f, 0.2f},
    { 160.0f, 218.0f, 0.7f,  0.88f,  1, 0.6f, 0.1f, 0.7f},
    { 760.0f, 224.0f, 0.68f, 0.64f, -1, 0.9f, 0.4f, 0.1f},
    { 200.0f, 212.0f, 0.72f, 0.76f,  1, 0.2f, 0.8f, 0.8f},

    // Mid-ground layer (y ~ 178-192, scale 0.85-0.95)
    { 440.0f, 185.0f, 0.9f,  1.00f, -1, 0.5f, 0.3f, 0.2f},
    {  40.0f, 178.0f, 0.95f, 1.20f,  1, 0.9f, 0.2f, 0.5f},
    { 640.0f, 192.0f, 0.85f, 0.88f, -1, 0.3f, 0.3f, 0.3f},
    { 320.0f, 182.0f, 0.9f,  1.12f,  1, 0.1f, 0.8f, 0.4f},
    { 420.0f, 188.0f, 0.88f, 1.00f,  1, 0.8f, 0.2f, 0.3f},
    { 100.0f, 180.0f, 0.92f, 1.08f, -1, 0.4f, 0.9f, 0.2f},

    // Foreground layer (y ~ 154-172, scale 1.05-1.2) - closest, fastest, biggest
    { 540.0f, 158.0f, 1.15f, 1.40f,  1, 0.2f, 0.4f, 0.9f},
    { 220.0f, 166.0f, 1.1f,  1.00f, -1, 0.9f, 0.8f, 0.2f},
    { 620.0f, 154.0f, 1.2f,  1.52f,  1, 0.7f, 0.1f, 0.9f},
    { 340.0f, 162.0f, 1.1f,  0.80f, -1, 0.3f, 0.7f, 0.6f},
    { 700.0f, 156.0f, 1.18f, 1.28f,  1, 0.9f, 0.5f, 0.3f},
    {  60.0f, 168.0f, 1.12f, 1.04f,  1, 0.5f, 0.2f, 0.7f},
    { 500.0f, 160.0f, 1.14f, 1.12f, -1, 0.1f, 0.9f, 0.5f},
    { 180.0f, 165.0f, 1.19f, 1.36f,  1, 0.8f, 0.3f, 0.6f},
    { 728.0f, 172.0f, 1.08f, 0.96f, -1, 0.6f, 0.6f, 0.2f},
    { 260.0f, 158.0f, 1.15f, 1.20f,  1, 0.2f, 0.7f, 0.5f},
    { 580.0f, 170.0f, 1.05f, 0.88f, -1, 0.8f, 0.4f, 0.8f},
    { 140.0f, 164.0f, 1.16f, 1.24f,  1, 0.4f, 0.5f, 0.9f}
};

// ==================== STRUCT FOR DRIFTING CLOUDS ====================
struct Cloud {
    float x, y;      // Center position of cloud
    float scale;     // Size multiplier
    float speed;     // Horizontal drift speed
};
Cloud clouds[3] = {
    { 220.0f, 712.0f, 1.1f, 0.18f},
    { 460.0f, 680.0f, 0.9f, 0.12f},
    { 672.0f, 720.0f, 1.2f, 0.22f}
};

// ==================== STRUCT FOR FLYING BIRDS ====================
struct Bird {
    float x, y;      // Position
    float scale;     // Bird size
    float speed;     // Flying speed
    int direction;   // 1 = right, -1 = left
    float phase;     // Wing flapping phase (for sin wave)
};
Bird birds[8] = {
    {  40.0f, 740.0f, 1.0f,  0.72f,  1, 0.0f},
    { 200.0f, 760.0f, 0.8f,  0.88f,  1, 1.0f},
    { 480.0f, 728.0f, 1.1f,  0.64f, -1, 2.0f},
    { 640.0f, 752.0f, 0.9f,  0.80f,  1, 0.5f},
    { 320.0f, 640.0f, 0.7f,  1.00f, -1, 1.5f},
    { 720.0f, 620.0f, 0.85f, 0.76f,  1, 2.5f},
    { 120.0f, 600.0f, 0.75f, 0.92f, -1, 0.8f},
    { 540.0f, 660.0f, 0.95f, 0.68f,  1, 1.8f}
};

// ==================== UTILITY: DRAW TEXT ====================
// x,y : screen position where text starts
// font: GLUT bitmap font
// string: text to render
void drawBitmapText(float x, float y, void* font, const char* string) {
    glRasterPos2f(x, y); // Set text cursor position
    int len = (int)strlen(string);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]); // Draw each character
    }
}

// ==================== UTILITY: DRAW FILLED CIRCLE ====================
// cx,cy : center of circle
// r : radius
// num_segments : how many triangles to approximate circle (more = smoother)
void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN); // Fan from center to edge
    glVertex2f(cx, cy); // Center point
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments); // Angle 0 to 2PI
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y); // Perimeter point
    }
    glEnd();
}

// ==================== OBJECT: CLOUD ====================
// cx,cy : cloud center
// scale : size of cloud (1.0 = normal)
void drawCloud(float cx, float cy, float scale) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    // 4 overlapping circles to make fluffy shape
    drawCircle(0.0f, 0.0f, 20.0f, 12);
    drawCircle(16.0f, 10.0f, 26.0f, 12);
    drawCircle(32.0f, 0.0f, 20.0f, 12);
    drawCircle(16.0f, -6.0f, 18.0f, 12);
    glPopMatrix();
}

// ==================== OBJECT: SUN RAYS ====================
// cx,cy : sun center
// Draws 12 rays around sun using GL_LINES
void drawSunRays(float cx, float cy) {
    glColor3f(1.0f, 0.90f, 0.30f); // Golden yellow
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        float a = i * (2.0f * (float)M_PI / 12.0f); // 30 degree gap per ray
        float x1 = cx + cosf(a) * 46.0f; // Inner end (near sun surface)
        float y1 = cy + sinf(a) * 46.0f;
        float x2 = cx + cosf(a) * 74.0f; // Outer end
        float y2 = cy + sinf(a) * 74.0f;
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    }
    glEnd();
}

// ==================== OBJECT: FLYING BIRD ====================
// cx,cy : bird center position
// scale : bird size
// phase : wing flap phase (driven by sin wave)
void drawBird(float cx, float cy, float scale, float phase) {
    float flap = sinf(phase) * 8.0f; // Up-down flap offset
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray/black
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    // Draw M-shaped bird silhouette with flapping wings
    glVertex2f(cx - 14.0f * scale, cy + flap * scale);
    glVertex2f(cx -  4.0f * scale, cy + 6.0f * scale);
    glVertex2f(cx,cy);
    glVertex2f(cx +  4.0f * scale, cy + 6.0f * scale);
    glVertex2f(cx + 14.0f * scale, cy + flap * scale);
    glEnd();
}

// ==================== OBJECT: ALPONA ====================
// cx,cy : center of alpona
// scale : size multiplier
void drawAlpona(float cx, float cy, float scale) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, 1.0f);
    // 8 dots in circle, alternating red/yellow
    for (int i = 0; i < 8; i++) {
        float a = i * (2.0f * (float)M_PI / 8.0f);
        float px = cosf(a) * 20.0f;
        float py = sinf(a) * 20.0f;
        if (i % 2 == 0)
        glColor3f(0.85f, 0.1f, 0.1f); // Red
        else glColor3f(1.0f, 0.85f, 0.2f); // Yellow
        drawCircle(px, py, 8.8f, 12);
    }
    glColor3f(1.0f, 1.0f, 1.0f); // White center
    drawCircle(0.0f, 0.0f, 8.0f, 16);
    glColor3f(0.85f, 0.1f, 0.1f); // Small red dot inside
    drawCircle(0.0f, 0.0f, 4.0f, 12);
    glPopMatrix();
}

// ==================== OBJECT: FESTIVAL BANNER / FLAGS ====================
// Draws top poles, rope, and triangular flags
void drawFestivalBanner() {
    // Two vertical poles
    glColor3f(0.30f, 0.20f, 0.10f); // Brown wooden pole color
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(20.0f, 460.0f);
    glVertex2f(20.0f, 776.0f);
    glVertex2f(780.0f, 460.0f);
    glVertex2f(780.0f, 776.0f);
    glEnd();

    // Wavy rope/string for flags (sine wave)
    glColor3f(0.30f, 0.20f, 0.10f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(20.0f, 772.0f);
    glVertex2f(200.0f, 788.0f);
    glVertex2f(400.0f, 772.0f);
    glVertex2f(600.0f, 788.0f);
    glVertex2f(780.0f, 772.0f);
    glEnd();

    // Triangular flags - red, yellow, green alternating
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 14; i++) {
        float t = i / 13.0f; // 0 to 1 interpolation
        float fx = 20.0f + t * 760.0f; // X along top
        float fy = 772.0f + sinf(t * (float)M_PI) * 16.0f; // Y with sine wave offset
        if (i % 3 == 0) glColor3f(1.0f, 0.0f, 0.0f); // Red
        else if (i % 3 == 1) glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        else glColor3f(0.0f, 0.6f, 0.2f); // Green
        glVertex2f(fx - 10.0f, fy);
        glVertex2f(fx + 10.0f, fy);
        glVertex2f(fx, fy - 20.0f);
    }
    glEnd();

}

// ==================== OBJECT: GENERIC HUMAN FIGURE ====================
// x,y : base position (feet)
// scale : size
// r,g,b : shirt color
void drawHuman(float x, float y, float scale, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    glColor3f(0.95f, 0.80f, 0.65f); // Skin color - face
    drawCircle(0.0f, 64.0f, 10.0f, 12);

    glColor3f(0.1f, 0.1f, 0.1f); // Black hair
    drawCircle(0.0f, 70.0f, 7.2f, 8);

    glColor3f(r, g, b); // Shirt body (QUAD)
    glBegin(GL_QUADS);
    glVertex2f(-8.0f, 52.0f);
    glVertex2f(8.0f, 52.0f);
    glVertex2f(8.0f, 8.0f);
    glVertex2f(-8.0f, 8.0f);
    glEnd();

    // Arms and legs using thick lines
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 40.0f); glVertex2f(-16.0f, 20.0f); // Left arm
    glVertex2f(0.0f, 40.0f); glVertex2f(16.0f, 20.0f);  // Right arm
    glVertex2f(-4.0f, 8.0f); glVertex2f(-10.0f, -24.0f); // Left leg
    glVertex2f(4.0f, 8.0f); glVertex2f(10.0f, -24.0f);   // Right leg
    glEnd();
    glPopMatrix();
}

// ==================== OBJECT: BALLOON SELLER + CUSTOMER ====================
// x,y : seller base position
// Uses global balloonWaveTime for floating animation
void drawBalloonSellerWithCustomer(float x, float y) {
    drawHuman(x, y, 1.0f, 0.8f, 0.2f, 0.6f); // Seller

    float floatOffset = sinf(balloonWaveTime) * 8.0f; // Floating up-down

    // Balloon strings
    glLineWidth(1.2f);
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_LINES);
    for (int i = 0; i < 7; i++) {
        float bx = x - 28.0f + i * 8.8f;
        float by = y + 128.0f + (i % 2) * 16.0f + floatOffset;
        glVertex2f(x + 12.0f, y + 40.0f); // Hand
        glVertex2f(bx, by); // Balloon
    }
    glEnd();

    // 7 colorful balloons
    float bColors[7][3] = {
        {1.0f, 0.0f, 0.0f}, {0.0f, 0.4f, 1.0f}, {1.0f, 0.6f, 0.0f},
        {0.0f, 0.8f, 0.2f}, {0.8f, 0.0f, 0.8f}, {1.0f, 1.0f, 0.0f},
        {0.0f, 0.8f, 0.8f}
    };
    for (int i = 0; i < 7; i++) {
        float bx = x - 28.0f + i * 8.8f;
        float by = y + 128.0f + (i % 2) * 16.0f + floatOffset;
        glColor3f(bColors[i][0], bColors[i][1], bColors[i][2]);
        drawCircle(bx, by, 11.2f, 12);
    }

    // Customer buying balloon (blue dress)
    float custX = x + 48.0f;
    float custY = y;
    drawHuman(custX, custY, 1.0f, 0.15f, 0.35f, 0.85f);
    // Arm reaching to seller
    glLineWidth(3.0f);
    glColor3f(0.95f, 0.80f, 0.65f);
    glBegin(GL_LINES);
    glVertex2f(custX - 8.0f, custY + 40.0f);
    glVertex2f(x + 8.0f, y + 72.0f);
    glEnd();
}

// ==================== OBJECT: CHORKI HEAD (SPINNING FLOWER) ====================
// cx,cy : center of flower
// scale : size
// phase : rotation angle (makes petals spin)
void drawChorkiHead(float cx, float cy, float scale, float phase) {
    float petals = 6;
    for (int i = 0; i < (int)petals; i++) {
        float a = phase + i * (2.0f * (float)M_PI / petals); // Each petal 60 deg apart
        float px = cx + cosf(a) * 9.0f * scale;
        float py = cy + sinf(a) * 9.0f * scale;
        if (i % 2 == 0) glColor3f(1.0f, 0.20f, 0.30f); // Red petal
        else glColor3f(1.0f, 0.70f, 0.10f); // Orange petal
        drawCircle(px, py, 5.0f * scale, 10);
    }
    glColor3f(1.0f, 0.95f, 0.20f); // Yellow center
    drawCircle(cx, cy, 4.0f * scale, 10);
}


// ==================== OBJECT: FULL CHORKI (FLOWER ON STICK) ====================
// cx,cy : base of stick (where seller holds)
// scale : size
// phase : spinning phase
void drawChorki(float cx, float cy, float scale, float phase) {
    // Wooden stick
    glColor3f(0.45f, 0.28f, 0.10f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(cx, cy);
    glVertex2f(cx, cy + 20.0f * scale);
    glEnd();
    // Flower on top
    drawChorkiHead(cx, cy + 24.0f * scale, scale, phase);
}

// ==================== OBJECT: CHORKI SELLER ====================
// x,y : seller base position
// Holds chorkis in both hands, one hand has bundle
void drawChorkiSeller(float x, float y) {
    drawHuman(x, y, 1.0f, 0.30f, 0.50f, 0.20f); // Seller body

    // Both arms raised
    glLineWidth(3.0f);
    glColor3f(0.95f, 0.80f, 0.65f);
    glBegin(GL_LINES);
    glVertex2f(x + 4.0f, y + 40.0f); glVertex2f(x + 20.0f, y + 64.0f); // Right arm up
    glVertex2f(x - 4.0f, y + 40.0f); glVertex2f(x - 20.0f, y + 64.0f); // Left arm up
    glEnd();

    // RIGHT HAND: 2 chorkis
    drawChorki(x + 20.0f, y + 64.0f, 1.35f, balloonWaveTime);
    drawChorki(x + 30.0f, y + 54.0f, 1.0f,  balloonWaveTime + 1.0f);

    // LEFT HAND: fist holding bundle
    glColor3f(0.85f, 0.55f, 0.30f);
    drawCircle(x - 20.0f, y + 64.0f, 4.5f, 8); // Fist knot
    glColor3f(0.60f, 0.40f, 0.15f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 3; i++) {
        float a = 1.15f + 0.34f * (float)i; // Fan angle 66 deg to 105 deg
        float dx = cosf(a) * 40.0f;
        float dy = sinf(a) * 40.0f;
        glVertex2f(x - 20.0f, y + 64.0f);
        glVertex2f(x - 20.0f + dx, y + 64.0f + dy);
    }
    glEnd();

    // Flower heads on bundle sticks
    for (int i = 0; i < 3; i++) {
        float a = 1.15f + 0.34f * (float)i;
        float bx = x - 20.0f + cosf(a) * 40.0f;
        float by = y + 64.0f + sinf(a) * 40.0f;
        drawChorkiHead(bx, by + 3.0f, 0.85f, balloonWaveTime + i * 0.8f);
    }
}

// ==================== OBJECT: GENERIC STALL ITEMS (POTS) ====================
// startX,startY : stall base
// w : stall width (to spread items)
void drawBaishakhiItems(float startX, float startY, float w) {
    glColor3f(0.8f, 0.3f, 0.1f); // Clay pot color
    drawCircle(startX + 32.0f, startY + 36.0f, 8.0f, 8);
    drawCircle(startX + w * 0.5f, startY + 36.0f, 10.0f, 8);
    drawCircle(startX + w - 32.0f, startY + 36.0f, 8.0f, 8);
}

// ==================== OBJECT: HILSA FISH FOR PANTA ILISH STALL ====================
// startX,startY : stall base
// w : stall width
void drawHilsaFishItems(float startX, float startY, float w) {
    float fishPositionsX[3] = { startX + 32.0f, startX + w * 0.5f, startX + w - 32.0f };
    for (int i = 0; i < 3; i++) {
        float fx = fishPositionsX[i];
        float fy = startY + 36.0f;
        glColor3f(0.85f, 0.88f, 0.92f); // Silver body
        drawCircle(fx, fy, 8.0f, 10);
        glColor3f(0.7f, 0.75f, 0.8f); // Tail
        glBegin(GL_TRIANGLES);
        glVertex2f(fx - 6.0f, fy);
        glVertex2f(fx - 14.0f, fy + 6.0f);
        glVertex2f(fx - 14.0f, fy - 6.0f);
        glEnd();
    }
}

// ==================== OBJECT: VILLAGE STALL (MAIN BUILDING) ====================
// startX,startY : bottom-left corner of stall
// w,h : width and height of main wall
// r,g,b : wall color
// roofR,roofG,roofB : roof color
// title : text written on roof
// isPandaDelicious : if true draws Hilsa fish, else draws clay pots
void drawStall(float startX, float startY, float w, float h, float r, float g, float b, float roofR, float roofG, float roofB, const char* title, bool isPandaDelicious = false) {
    // Main wall (QUAD)
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(startX, startY);
    glVertex2f(startX + w, startY);
    glVertex2f(startX + w, startY + h);
    glVertex2f(startX, startY + h);
    glEnd();

    // Darker side pillars for depth
    glColor3f(r * 0.6f, g * 0.6f, b * 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(startX, startY);
    glVertex2f(startX + 8.0f, startY);
    glVertex2f(startX + 8.0f, startY + h);
    glVertex2f(startX, startY + h);
    glVertex2f(startX + w - 8.0f, startY);
    glVertex2f(startX + w, startY);
    glVertex2f(startX + w, startY + h);
    glVertex2f(startX + w - 8.0f, startY + h);
    glEnd();

    // White counter/table inside stall
    glColor3f(0.92f, 0.90f, 0.82f);
    glBegin(GL_QUADS);
    glVertex2f(startX + 12.0f, startY);
    glVertex2f(startX + w - 12.0f, startY);
    glVertex2f(startX + w - 12.0f, startY + 56.0f);
    glVertex2f(startX + 12.0f, startY + 56.0f);
    glEnd();

    // Items on counter
    if (isPandaDelicious) {
        drawHilsaFishItems(startX, startY, w);
    } else {
        drawBaishakhiItems(startX, startY, w);
    }

    // Roof (trapezoid using QUAD)
    glColor3f(roofR, roofG, roofB);
    glBegin(GL_QUADS);
    glVertex2f(startX - 12.0f, startY + h);
    glVertex2f(startX + w + 12.0f, startY + h);
    glVertex2f(startX + w - 8.0f, startY + h + 56.0f);
    glVertex2f(startX + 8.0f, startY + h + 56.0f);
    glEnd();

    // Title text on roof
    glColor3f(1.0f, 1.0f, 1.0f);
    drawBitmapText(startX + 16.0f, startY + h + 20.0f, GLUT_BITMAP_HELVETICA_12, title);

    // Small triangular flags hanging from stall roof
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 4; i++) {
        float fx = startX + 16.0f + i * 28.0f;
        if (i % 3 == 0) glColor3f(1.0f, 0.0f, 0.0f); // Red
        else if (i % 3 == 1) glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        else glColor3f(0.0f, 1.0f, 0.0f); // Green
        glVertex2f(fx, startY + h);
        glVertex2f(fx + 20.0f, startY + h);
        glVertex2f(fx + 10.0f, startY + h - 20.0f);
    }
    glEnd();
}

// ==================== OBJECT: BUYERS IN FRONT OF STALL ====================
// startX : stall left X
// w : stall width
// baseY : ground Y in front of stall
void drawBuyersAtStall(float startX, float w, float baseY) {
    float xs[3] = { startX + 18.0f, startX + w * 0.5f, startX + w - 18.0f };
    float cols[3][3] = {
        {0.20f, 0.40f, 0.85f},   // Blue shirt
        {0.85f, 0.20f, 0.30f},   // Red shirt
        {0.20f, 0.70f, 0.30f}    // Green shirt
    };
    for (int i = 0; i < 3; i++) {
        drawHuman(xs[i], baseY, 0.85f, cols[i][0], cols[i][1], cols[i][2]);
    }
}

// ==================== OBJECT: FERRIS WHEEL (NAGORDOLA) ====================
// No parameters - position fixed at (648,448)
// Uses global ferrisWheelAngle for rotation
void drawFerrisWheel() {
    // A-frame support poles
    glColor3f(0.35f, 0.20f, 0.10f); // Dark brown
    glLineWidth(5.0f);
    glBegin(GL_LINES);
    glVertex2f(688.0f, 180.0f); glVertex2f(648.0f, 448.0f);
    glVertex2f(608.0f, 180.0f); glVertex2f(648.0f, 448.0f);
    glEnd();

    glPushMatrix();
    glTranslatef(648.0f, 448.0f, 0.0f); // Move origin to wheel center
    glRotatef(ferrisWheelAngle, 0.0f, 0.0f, 1.0f); // Rotate whole wheel

    // Outer rim (circle outline)
    glColor3f(0.95f, 0.65f, 0.05f); // Golden
    glLineWidth(4.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 36; i++) {
        float theta = 2.0f * M_PI * float(i) / 36.0f;
        glVertex2f(104.0f * cosf(theta), 104.0f * sinf(theta));
    }
    glEnd();

    // 6 cabins + spokes
    for (int i = 0; i < 6; i++) {
        float angle = i * (2.0f * M_PI / 6.0f); // 60 deg gap
        float cx = 104.0f * cosf(angle);
        float cy = 104.0f * sinf(angle);

        // Spoke from center to cabin
        glColor3f(0.25f, 0.25f, 0.25f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(cx, cy);
        glEnd();

        glPushMatrix();
        glTranslatef(cx, cy, 0.0f);
        glRotatef(-ferrisWheelAngle, 0.0f, 0.0f, 1.0f); // Counter-rotate to keep cabin upright

        // Cabin box
        glColor3f(0.15f, 0.45f, 0.85f); // Blue cabin
        glBegin(GL_QUADS);
        glVertex2f(-12.0f, -20.0f);
        glVertex2f(12.0f, -20.0f);
        glVertex2f(12.0f, 0.0f);
        glVertex2f(-12.0f, 0.0f);
        glEnd();

        // Person head inside cabin
        glColor3f(0.95f, 0.80f, 0.65f);
        drawCircle(0.0f, -8.0f, 4.8f, 8);

        // Person shirt inside cabin
        glColor3f(0.9f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex2f(-4.8f, -18.0f);
        glVertex2f(4.8f, -18.0f);
        glVertex2f(4.8f, -10.0f);
        glVertex2f(-4.8f, -10.0f);
        glEnd();

        glPopMatrix();
    }
    glPopMatrix();
}

// ==================== OBJECT: MASKED FOLK DANCER ====================
// x,y : base position
// scale : size
// phase : sway phase for dancing animation
void drawMaskedDancer(float x, float y, float scale, float phase) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);
    float sway = sinf(phase); // -1 to 1 sway
    glRotatef(sway * 8.0f, 0.0f, 0.0f, 1.0f); // Tilt body left-right

    // Body
    glColor3f(0.9f, 0.4f, 0.05f); // Orange
    glBegin(GL_QUADS);
    glVertex2f(-8.8f, 52.0f);
    glVertex2f(8.8f, 52.0f);
    glVertex2f(8.8f, 8.0f);
    glVertex2f(-8.8f, 8.0f);
    glEnd();

    // Mask face (bigger circle)
    glColor3f(0.95f, 0.6f, 0.1f);
    drawCircle(0.0f, 64.0f, 12.0f, 16);

    // Mask eyes
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle(-4.8f, 66.0f, 2.8f, 8);
    drawCircle(4.8f, 66.0f, 2.8f, 8);

    // Man on mask
    glColor3f(1.0f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(-12.0f, 68.0f); glVertex2f(-6.0f, 76.0f);
    glVertex2f(12.0f, 68.0f); glVertex2f(6.0f, 76.0f);
    glEnd();

    // Dancing arms (sway with phase)
    glLineWidth(3.0f);
    glColor3f(0.95f, 0.80f, 0.65f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 44.0f); glVertex2f(-20.0f, 60.0f + sway * 8.0f);
    glVertex2f(0.0f, 44.0f); glVertex2f(20.0f, 60.0f - sway * 8.0f);
    glEnd();

    // Legs
    glBegin(GL_LINES);
    glVertex2f(-4.0f, 8.0f); glVertex2f(-14.0f, -20.0f);
    glVertex2f(4.0f, 8.0f); glVertex2f(8.0f, -24.0f);
    glEnd();

    glPopMatrix();
}

// ==================== OBJECT: TRADITIONAL DRESS PERSON ====================
// x,y : base position
// scale : size
// isFemale : true = saree, false = panjabi
void drawTraditionalPerson(float x, float y, float scale, bool isFemale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    glColor3f(0.95f, 0.80f, 0.65f); // Face skin
    drawCircle(0.0f, 64.0f, 10.0f, 12);
    glColor3f(0.1f, 0.1f, 0.1f); // Hair
    drawCircle(0.0f, 70.0f, 7.2f, 8);

    if (isFemale) {
        // Saree - white with red border (traditional Boishakhi)
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(-10.0f, 52.0f); glVertex2f(10.0f, 52.0f);
        glVertex2f(12.0f, -8.0f); glVertex2f(-12.0f, -8.0f);
        glEnd();
        glColor3f(0.85f, 0.1f, 0.1f); // Red border at bottom
        glBegin(GL_QUADS);
        glVertex2f(-12.0f, -2.0f); glVertex2f(12.0f, -2.0f);
        glVertex2f(12.0f, -8.0f); glVertex2f(-12.0f, -8.0f);
        glEnd();
        // Saree pallu
        glBegin(GL_QUADS);
        glVertex2f(-8.0f, 52.0f); glVertex2f(0.0f, 52.0f);
        glVertex2f(6.0f, 12.0f); glVertex2f(-2.0f, 12.0f);
        glEnd();

        // Arms and legs
        glLineWidth(3.0f);
        glColor3f(0.95f, 0.80f, 0.65f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 40.0f); glVertex2f(-16.0f, 20.0f);
        glVertex2f(0.0f, 40.0f); glVertex2f(16.0f, 20.0f);
        glVertex2f(-6.0f, -8.0f); glVertex2f(-8.0f, -24.0f);
        glVertex2f(6.0f, -8.0f); glVertex2f(8.0f, -24.0f);
        glEnd();
    } else {
        // Male: white panjabi with red vertical line
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glVertex2f(-8.0f, 52.0f); glVertex2f(8.0f, 52.0f);
        glVertex2f(8.0f, 8.0f); glVertex2f(-8.0f, 8.0f);
        glEnd();
        glColor3f(0.85f, 0.1f, 0.1f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 52.0f); glVertex2f(0.0f, 12.0f);
        glEnd();

        glLineWidth(3.0f);
        glColor3f(0.95f, 0.80f, 0.65f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 40.0f); glVertex2f(-16.0f, 20.0f);
        glVertex2f(0.0f, 40.0f); glVertex2f(16.0f, 20.0f);
        glVertex2f(-4.0f, 8.0f); glVertex2f(-10.0f, -24.0f);
        glVertex2f(4.0f, 8.0f); glVertex2f(10.0f, -24.0f);
        glEnd();
    }
    glPopMatrix();
}

// ==================== OBJECT: KITE FLYING CHILD ====================
// x,y : child base position
// scale : child size
// kiteX,kiteY : kite position in sky (calculated from kitePhase)
void drawKiteFlyingChild(float x, float y, float scale, float kiteX, float kiteY) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale * 0.75f, scale * 0.75f, 1.0f); // Child is smaller

    glColor3f(0.95f, 0.80f, 0.65f); // Face
    drawCircle(0.0f, 64.0f, 10.0f, 12);
    glColor3f(0.15f, 0.1f, 0.05f); // Hair
    drawCircle(0.0f, 70.0f, 7.2f, 8);

    glColor3f(0.2f, 0.6f, 0.9f); // Blue shirt
    glBegin(GL_QUADS);
    glVertex2f(-8.0f, 52.0f); glVertex2f(8.0f, 52.0f);
    glVertex2f(8.0f, 8.0f); glVertex2f(-8.0f, 8.0f);
    glEnd();

    glLineWidth(3.0f);
    glColor3f(0.95f, 0.80f, 0.65f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 40.0f); glVertex2f(16.0f, 80.0f); // Right arm raised holding string
    glVertex2f(0.0f, 40.0f); glVertex2f(-16.0f, 20.0f); // Left arm
    glVertex2f(-4.0f, 8.0f); glVertex2f(-10.0f, -24.0f); // Legs
    glVertex2f(4.0f, 8.0f); glVertex2f(10.0f, -24.0f);
    glEnd();
    glPopMatrix();

    // Kite string from child's hand to kite
    float handX = x + 16.0f * scale * 0.75f;
    float handY = y + 80.0f * scale * 0.75f;
    glColor3f(0.3f, 0.3f, 0.3f); // Gray string
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2f(handX, handY);
    glVertex2f(kiteX, kiteY);
    glEnd();

    // Kite body at kiteX,kiteY
    glPushMatrix();
    glTranslatef(kiteX, kiteY, 0.0f);
    glColor3f(1.0f, 0.3f, 0.1f); // Red diamond kite
    glBegin(GL_QUADS);
    glVertex2f(0.0f, 14.0f);
    glVertex2f(10.0f, 0.0f);
    glVertex2f(0.0f, -14.0f);
    glVertex2f(-10.0f, 0.0f);
    glEnd();
    // Cross lines on kite
    glColor3f(1.0f, 1.0f, 0.2f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(-10.0f, 0.0f); glVertex2f(10.0f, 0.0f);
    glVertex2f(0.0f, 14.0f); glVertex2f(0.0f, -14.0f);
    glEnd();
    // Kite tail (zigzag)
    glColor3f(0.2f, 0.7f, 0.2f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(0.0f, -14.0f);
    glVertex2f(-4.0f, -24.0f);
    glVertex2f(4.0f, -32.0f);
    glVertex2f(-4.0f, -40.0f);
    glEnd();
    glPopMatrix();
}

// ==================== OBJECT: PARENT WITH CHILD ON SHOULDERS ====================
// x,y : parent base
// scale : size
// rP,gP,bP : parent shirt color
// rC,gC,bC : child shirt color
void drawChildOnShoulders(float x, float y, float scale, float rP, float gP, float bP, float rC, float gC, float bC) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Parent face and hair
    glColor3f(0.95f, 0.80f, 0.65f);
    drawCircle(0.0f, 64.0f, 10.0f, 12);
    glColor3f(0.1f, 0.1f, 0.1f);
    drawCircle(0.0f, 70.0f, 7.2f, 8);

    // Parent body
    glColor3f(rP, gP, bP);
    glBegin(GL_QUADS);
    glVertex2f(-8.8f, 52.0f); glVertex2f(8.8f, 52.0f);
    glVertex2f(8.8f, 8.0f); glVertex2f(-8.8f, 8.0f);
    glEnd();

    // Parent arms holding child's legs
    glLineWidth(3.0f);
    glColor3f(0.95f, 0.80f, 0.65f);
    glBegin(GL_LINES);
    glVertex2f(-6.0f, 52.0f); glVertex2f(-12.0f, 80.0f);
    glVertex2f(6.0f, 52.0f); glVertex2f(12.0f, 80.0f);
    glVertex2f(-4.0f, 8.0f); glVertex2f(-10.0f, -24.0f);
    glVertex2f(4.0f, 8.0f); glVertex2f(10.0f, -24.0f);
    glEnd();

    // Child face on top of parent
    glColor3f(0.95f, 0.80f, 0.65f);
    drawCircle(0.0f, 104.0f, 7.2f, 12);
    glColor3f(0.15f, 0.1f, 0.05f);
    drawCircle(0.0f, 108.8f, 5.2f, 8);

    // Child body sitting on shoulders
    glColor3f(rC, gC, bC);
    glBegin(GL_QUADS);
    glVertex2f(-5.6f, 96.0f); glVertex2f(5.6f, 96.0f);
    glVertex2f(5.6f, 76.0f); glVertex2f(-5.6f, 76.0f);
    glEnd();

    // Child arms and legs dangling
    glLineWidth(2.0f);
    glColor3f(0.95f, 0.80f, 0.65f);
    glBegin(GL_LINES);
    glVertex2f(-5.6f, 92.0f); glVertex2f(-10.0f, 72.0f);
    glVertex2f(5.6f, 92.0f); glVertex2f(10.0f, 112.0f);
    glVertex2f(-3.2f, 76.0f); glVertex2f(-8.0f, 56.0f);
    glVertex2f(3.2f, 76.0f); glVertex2f(8.0f, 56.0f);
    glEnd();
    glPopMatrix();
}

// ==================== MAIN DISPLAY FUNCTION ====================
// This is called every frame by GLUT to draw everything in correct order (back to front)
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // 1. SKY BACKGROUND (gradient from light blue to deeper blue)
    glBegin(GL_QUADS);
    glColor3f(0.53f, 0.81f, 0.98f); // Top sky color
    glVertex2f(0.0f, 800.0f); glVertex2f(800.0f, 800.0f);
    glColor3f(0.88f, 0.96f, 1.0f); // Horizon color (lighter)
    glVertex2f(800.0f, 460.0f); glVertex2f(0.0f, 460.0f);
    glEnd();

    // 2. SUN + RAYS
    drawSunRays(112.0f, 688.0f); // Rays around sun
    glColor3f(1.0f, 0.92f, 0.2f); // Sun body yellow
    drawCircle(112.0f, 688.0f, 36.0f, 24);

    // 3. CLOUDS (animated drift)
    for (int i = 0; i < 3; i++) {
        drawCloud(clouds[i].x, clouds[i].y, clouds[i].scale);
    }

    // 4. BIRDS (animated flying + wing flapping)
    for (int i = 0; i < 8; i++) {
        drawBird(birds[i].x, birds[i].y, birds[i].scale, birds[i].phase);
    }

    // 5. TOP BANNER WITH FLAGS
    drawFestivalBanner();

    // 6. GROUND / GRASS FIELD
    glBegin(GL_QUADS);
    glColor3f(0.22f, 0.68f, 0.32f); // Green grass
    glVertex2f(0.0f, 460.0f); glVertex2f(800.0f, 460.0f);
    glVertex2f(800.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glEnd();

    // 7. ALPONA ART ON GROUND
    drawAlpona(40.0f, 40.0f, 1.0f);
    drawAlpona(400.0f, 20.0f, 1.2f);
    drawAlpona(752.0f, 48.0f, 0.9f);

    // 8. BACKGROUND STALLS (farther, smaller)
    drawStall(60.0f,  380.0f, 112.0f, 128.0f, 0.75f, 0.65f, 0.35f, 0.80f, 0.2f, 0.2f, "Handicraft");
    drawBuyersAtStall(60.0f, 112.0f, 352.0f);
    drawStall(232.0f, 392.0f, 120.0f, 136.0f, 0.80f, 0.45f, 0.40f, 0.20f, 0.5f, 0.7f, "Clay Toys");
    drawBuyersAtStall(232.0f, 120.0f, 364.0f);
    drawStall(408.0f, 384.0f, 112.0f, 128.0f, 0.65f, 0.75f, 0.50f, 0.90f, 0.3f, 0.1f, "Muri Murki");
    drawBuyersAtStall(408.0f, 112.0f, 356.0f);
    drawStall(580.0f, 388.0f, 128.0f, 140.0f, 0.85f, 0.55f, 0.45f, 0.20f, 0.7f, 0.3f, "Bangle Store");
    drawBuyersAtStall(580.0f, 128.0f, 360.0f);

    // 9. FOREGROUND STALLS (closer, bigger)
    drawStall(20.0f,  272.0f, 128.0f, 152.0f, 0.85f, 0.75f, 0.40f, 0.85f, 0.2f, 0.15f, "Bioscope");
    drawStall(180.0f, 280.0f, 120.0f, 152.0f, 0.90f, 0.50f, 0.45f, 0.20f, 0.6f, 0.20f, "NakshiKatha");
    drawBuyersAtStall(180.0f, 120.0f, 252.0f);
    drawStall(340.0f, 268.0f, 128.0f, 156.0f, 0.90f, 0.30f, 0.30f, 0.80f, 0.1f, 0.1f, "Panta Ilish", true); // Hilsa fish
    drawStall(500.0f, 276.0f, 120.0f, 152.0f, 0.75f, 0.40f, 0.70f, 0.80f, 0.1f, 0.5f, "Bioscope");
    drawStall(660.0f, 272.0f, 132.0f, 156.0f, 0.70f, 0.80f, 0.60f, 0.20f, 0.4f, 0.80f, "Jilapi Ghar");

    // 10. FERRIS WHEEL
    drawFerrisWheel();

    // 11. STATIC CROWD BEHIND STALLS (buyers near stalls)
    drawHuman(80.0f, 248.0f, 1.0f, 0.5f, 0.2f, 0.8f);
    drawHuman(112.0f, 244.0f, 1.0f, 0.2f, 0.5f, 0.9f);
    drawHuman(392.0f, 248.0f, 1.0f, 0.9f, 0.8f, 0.1f);
    drawHuman(448.0f, 244.0f, 1.0f, 0.6f, 0.1f, 0.8f);
    drawHuman(552.0f, 248.0f, 1.0f, 0.4f, 0.9f, 0.8f);
    drawHuman(584.0f, 244.0f, 1.0f, 0.9f, 0.3f, 0.2f);
    drawHuman(712.0f, 248.0f, 1.0f, 0.9f, 0.4f, 0.3f);
    drawHuman(744.0f, 244.0f, 1.0f, 0.3f, 0.8f, 0.6f);

    // Traditional dressed people
    drawTraditionalPerson(32.0f, 244.0f, 1.0f, true);  // Female in saree
    drawTraditionalPerson(772.0f, 244.0f, 1.0f, false); // Male in panjabi

    // Parent with child on shoulders
    drawChildOnShoulders(320.0f, 232.0f, 1.0f, 0.15f, 0.45f, 0.25f, 0.95f, 0.15f, 0.15f);

    // 12. WALKING CROWD (moves left/right continuously)
    for (int i = 0; i < 24; i++) {
        drawHuman(people[i].x, people[i].y, people[i].scale, people[i].r, people[i].g, people[i].b);
    }

    // 13. FRONT ATTRACTION ROW (drawn last = on top, so not covered)
    // Spacing ~90px so no overlap, y=80-92 below walking crowd (y>=150)

    drawChorkiSeller(150.0f, 92.0f); // Left chorki seller
    drawBalloonSellerWithCustomer(270.0f, 90.0f); // Balloon seller

    // Kite flying - calculate kite position from phase
    float kiteX = 400.0f + 70.0f * sinf(kitePhase); // Left-right movement 70px
    float kiteY = 580.0f + 45.0f * cosf(kitePhase); // Up-down movement 45px
    drawKiteFlyingChild(400.0f, 80.0f, 1.0f, kiteX, kiteY);

    drawChorkiSeller(590.0f, 90.0f); // Right chorki seller
    drawMaskedDancer(690.0f, 90.0f, 1.0f, balloonWaveTime + 0.8f); // Dancer right

    drawMaskedDancer(495.0f, 90.0f, 1.0f, balloonWaveTime + 2.0f); // Dancer  2

    glutSwapBuffers(); // Double buffering
}

// ==================== TIMER / ANIMATION UPDATE ====================
// value : timer ID
// Called every ~16ms (~60 FPS)
void update(int value) {
    // Ferris wheel rotation
    ferrisWheelAngle += 1.2f;
    if (ferrisWheelAngle > 360.0f) ferrisWheelAngle -= 360.0f;

    // Balloon floating + chorki spinning + dancer sway
    balloonWaveTime += 0.06f;

    // Kite oval motion phase
    kitePhase += 0.03f;
    if (kitePhase > 2.0f * (float)M_PI) kitePhase -= 2.0f * (float)M_PI;

    // Cloud drifting - wrap around when out of screen
    for (int i = 0; i < 3; i++) {
        clouds[i].x += clouds[i].speed;
        if (clouds[i].x > 860.0f) clouds[i].x = -60.0f;
    }

    // People walking - wrap around screen edges
    for (int i = 0; i < 24; i++) {
        people[i].x += people[i].direction * people[i].speed;
        if (people[i].direction == 1 && people[i].x > 860.0f) people[i].x = -60.0f;
        else if (people[i].direction == -1 && people[i].x < -60.0f) people[i].x = 860.0f;
    }

    // Birds flying + wing flapping phase update
    for (int i = 0; i < 8; i++) {
        birds[i].x += birds[i].direction * birds[i].speed;
        birds[i].phase += 0.15f; // Wing flap speed
        if (birds[i].direction == 1 && birds[i].x > 860.0f) birds[i].x = -60.0f;
        else if (birds[i].direction == -1 && birds[i].x < -60.0f) birds[i].x = 860.0f;
    }

    glutPostRedisplay(); // Request redraw
    glutTimerFunc(16, update, 16); // Call again after 16ms
}

// ==================== OPENGL INITIALIZATION ====================
void initGL() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White clear color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, 800.0f, 0.0f, 800.0f); // 2D coordinate system 800x800
    glMatrixMode(GL_MODELVIEW);
}

// ==================== MAIN FUNCTION ====================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800); // Window size
    glutCreateWindow("Village Fair - Baishakhi Mela"); // Title
    initGL();
    glutDisplayFunc(display); // Set display callback
    glutTimerFunc(25, update, 0); // Start animation timer
    glutMainLoop(); // Enter event loop
    return 0;
}
