#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// Native PI constant definition
const float PI = 3.14159265358979323846f;

// ==========================================
// SYSTEM & SCENE CONTROL VARIABLES
// ==========================================

// Scene State: 0 = Village Fair, 1 = River Boat Race
int currentScene = 0;

// Environment States
bool isNightMode = true;
bool isPaused = false;
float animSpeed = 1.0f;

// Scene Day/Night Transition Factors (0.0 = Day, 1.0 = Night)
float dayNightFactor = 1.0f;

// Motion & Animation Counters
float ferrisAngle = 0.0f;
float sunAngle = 0.0f;
float moonGlow = 0.0f;
bool moonGrowing = true;
float waveOffset = 0.0f;

// Cloud Positions
float cloud1X = 40.0f;
float cloud2X = 320.0f;
float cloud3X = 620.0f;

// Character Traversal Positions & Directions (Kept x >= 340.0f to prevent clipping)
float person1X = 380.0f; bool person1Right = true;
float person2X = 740.0f; bool person2Right = false;
float person3X = 420.0f; bool person3Right = true;
float person4X = 350.0f; bool person4Right = true;
float person5X = 580.0f; bool person5Right = false;

float vendorX = 480.0f;  bool vendorRight = true;

float legAngle = 0.0f;
bool legSwingForward = true;

// Boat Positions
float boat1X = 40.0f;
float boat2X = 10.0f;
float boat3X = 70.0f;

// Star Intensity Engine Data
float starAlpha[60];
float starCoords[60][2];

// ==========================================
// MATHEMATICAL GEOMETRY PRIMITIVES
// ==========================================

void drawFilledCircle(float cx, float cy, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * float(i) / float(segments);
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

void drawOutlineCircle(float cx, float cy, float r, int segments, float lineWidth) {
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * PI * float(i) / float(segments);
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

void drawQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glEnd();
}

void drawRect(float xmin, float ymin, float xmax, float ymax) {
    drawQuad(xmin, ymin, xmax, ymin, xmax, ymax, xmin, ymax);
}

void drawGradientRect(float xmin, float ymin, float xmax, float ymax,
                      float r1, float g1, float b1,
                      float r2, float g2, float b2) {
    glBegin(GL_QUADS);
    glColor3f(r1, g1, b1);
    glVertex2f(xmin, ymax);
    glVertex2f(xmax, ymax);
    glColor3f(r2, g2, b2);
    glVertex2f(xmax, ymin);
    glVertex2f(xmin, ymin);
    glEnd();
}

void drawTriangleShape(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawSegmentLine(float x1, float y1, float x2, float y2, float width) {
    glLineWidth(width);
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

float lerpColor(float dayVal, float nightVal, float factor) {
    return dayVal + factor * (nightVal - dayVal);
}

// ==========================================
// ENVIRONMENT & ATMOSPHERE RENDERERS
// ==========================================

void renderSkyBackground() {
    float topR = lerpColor(0.40f, 0.01f, dayNightFactor);
    float topG = lerpColor(0.70f, 0.01f, dayNightFactor);
    float topB = lerpColor(0.95f, 0.08f, dayNightFactor);

    float botR = lerpColor(0.75f, 0.05f, dayNightFactor);
    float botG = lerpColor(0.88f, 0.10f, dayNightFactor);
    float botB = lerpColor(0.98f, 0.25f, dayNightFactor);

    drawGradientRect(0.0f, 320.0f, 800.0f, 800.0f, topR, topG, topB, botR, botG, botB);
}

void renderCelestialBodies() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (dayNightFactor > 0.5f) {
        glColor4f(1.0f, 0.98f, 0.8f, 0.10f + moonGlow * 0.08f);
        drawFilledCircle(110.0f, 690.0f, 68.0f, 40);

        glColor3f(0.98f, 0.96f, 0.85f);
        drawFilledCircle(110.0f, 690.0f, 42.0f, 40);

        float skyBotR = lerpColor(0.75f, 0.05f, dayNightFactor);
        float skyBotG = lerpColor(0.88f, 0.10f, dayNightFactor);
        float skyBotB = lerpColor(0.98f, 0.25f, dayNightFactor);
        glColor3f(skyBotR, skyBotG, skyBotB);
        drawFilledCircle(126.0f, 702.0f, 38.0f, 40);
    }

    if (dayNightFactor < 0.5f) {
        glColor4f(1.0f, 0.9f, 0.2f, 0.30f);
        drawFilledCircle(700.0f, 700.0f, 68.0f, 40);

        glColor3f(1.0f, 0.85f, 0.1f);
        drawFilledCircle(700.0f, 700.0f, 42.0f, 40);

        glColor3f(1.0f, 0.8f, 0.1f);
        for (int i = 0; i < 12; i++) {
            float angle = i * (2.0f * PI / 12.0f) + sunAngle;
            float x1 = 700.0f + 48.0f * cosf(angle);
            float y1 = 700.0f + 48.0f * sinf(angle);
            float x2 = 700.0f + 72.0f * cosf(angle);
            float y2 = 700.0f + 72.0f * sinf(angle);
            drawSegmentLine(x1, y1, x2, y2, 2.5f);
        }
    }
    glDisable(GL_BLEND);
}

void renderStarfield() {
    if (dayNightFactor < 0.3f) return;

    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 50; i++) {
        glColor4f(1.0f, 1.0f, 1.0f, starAlpha[i] * dayNightFactor);
        glVertex2f(starCoords[i][0], starCoords[i][1]);
    }
    glEnd();
}

void renderCloudCover() {
    float cloudBase = lerpColor(0.98f, 0.22f, dayNightFactor);
    glColor3f(cloudBase, cloudBase, cloudBase + 0.05f);

    auto drawCloudUnit = [](float cx, float cy) {
        drawFilledCircle(cx, cy, 22.0f, 20);
        drawFilledCircle(cx + 18.0f, cy + 10.0f, 26.0f, 20);
        drawFilledCircle(cx + 40.0f, cy, 20.0f, 20);
        drawFilledCircle(cx + 20.0f, cy - 8.0f, 22.0f, 20);
    };

    drawCloudUnit(cloud1X, 720.0f);
    drawCloudUnit(cloud2X, 660.0f);
    drawCloudUnit(cloud3X, 740.0f);
}

void renderBuntingGarland() {
    glColor3f(0.5f, 0.5f, 0.5f);
    drawSegmentLine(0.0f, 740.0f, 800.0f, 740.0f, 1.5f);
    drawSegmentLine(0.0f, 710.0f, 800.0f, 710.0f, 1.5f);

    for (int x = 15; x < 800; x += 30) {
        int index = (x / 30) % 5;
        if (index == 0) glColor3f(0.95f, 0.15f, 0.15f);
        else if (index == 1) glColor3f(0.95f, 0.85f, 0.10f);
        else if (index == 2) glColor3f(0.15f, 0.65f, 0.95f);
        else if (index == 3) glColor3f(0.15f, 0.85f, 0.25f);
        else glColor3f(0.85f, 0.25f, 0.85f);

        drawTriangleShape((float)x - 10, 740.0f, (float)x + 10, 740.0f, (float)x, 710.0f);
        drawTriangleShape((float)x - 10, 710.0f, (float)x + 10, 710.0f, (float)x, 680.0f);
    }
}

// ==========================================
// GROUND & STALLS RENDERERS
// ==========================================

void renderGroundTerrain() {
    float gTopR = lerpColor(0.35f, 0.06f, dayNightFactor);
    float gTopG = lerpColor(0.78f, 0.28f, dayNightFactor);
    float gTopB = lerpColor(0.28f, 0.10f, dayNightFactor);

    float gBotR = lerpColor(0.25f, 0.04f, dayNightFactor);
    float gBotG = lerpColor(0.65f, 0.20f, dayNightFactor);
    float gBotB = lerpColor(0.20f, 0.08f, dayNightFactor);

    drawGradientRect(0.0f, 0.0f, 800.0f, 320.0f, gTopR, gTopG, gTopB, gBotR, gBotG, gBotB);

    float pTopR = lerpColor(0.78f, 0.32f, dayNightFactor);
    float pTopG = lerpColor(0.60f, 0.22f, dayNightFactor);
    float pTopB = lerpColor(0.42f, 0.14f, dayNightFactor);

    float pBotR = lerpColor(0.65f, 0.22f, dayNightFactor);
    float pBotG = lerpColor(0.48f, 0.15f, dayNightFactor);
    float pBotB = lerpColor(0.32f, 0.09f, dayNightFactor);

    drawGradientRect(0.0f, 0.0f, 800.0f, 140.0f, pTopR, pTopG, pTopB, pBotR, pBotG, pBotB);
}

void renderStallStaff(float x, float y, float r, float g, float b, int stallType) {
    glColor3f(0.92f, 0.76f, 0.62f);
    drawFilledCircle(x, y + 20.0f, 9.0f, 16);

    glColor3f(r, g, b);
    drawRect(x - 9.0f, y - 5.0f, x + 9.0f, y + 11.0f);

    glColor3f(0.2f, 0.2f, 0.2f);
    drawRect(x - 10.0f, y + 25.0f, x + 10.0f, y + 29.0f);
    drawTriangleShape(x - 8.0f, y + 29.0f, x + 8.0f, y + 29.0f, x, y + 36.0f);

    if (stallType == 1) {
        glColor3f(0.9f, 0.9f, 0.9f);
        drawSegmentLine(x + 5.0f, y + 5.0f, x - 15.0f, y + 45.0f, 1.0f);
        drawSegmentLine(x + 5.0f, y + 5.0f, x + 5.0f, y + 50.0f, 1.0f);
        drawSegmentLine(x + 5.0f, y + 5.0f, x + 20.0f, y + 42.0f, 1.0f);

        glColor3f(0.95f, 0.15f, 0.15f); drawFilledCircle(x - 15.0f, y + 53.0f, 9.0f, 16);
        glColor3f(0.15f, 0.85f, 0.25f); drawFilledCircle(x + 5.0f, y + 58.0f, 9.0f, 16);
        glColor3f(0.95f, 0.85f, 0.10f); drawFilledCircle(x + 20.0f, y + 50.0f, 9.0f, 16);
    }
    else if (stallType == 2) {
        for (int i = -2; i <= 2; i += 2) {
            float tx = x + i * 14.0f;
            float ty = y - 2.0f;

            glColor3f(0.3f, 0.2f, 0.1f);
            drawSegmentLine(tx, ty, tx, ty + 16.0f, 2.0f);

            if (i % 4 == 0) glColor3f(0.9f, 0.2f, 0.2f);
            else glColor3f(0.1f, 0.6f, 0.9f);
            drawTriangleShape(tx - 6.0f, ty + 16.0f, tx + 6.0f, ty + 16.0f, tx, ty + 24.0f);
            drawTriangleShape(tx - 6.0f, ty + 16.0f, tx + 6.0f, ty + 16.0f, tx, ty + 8.0f);
        }
    }
}

void renderStall(float x, float y, float w, float h, float r, float g, float b,
                float staffR, float staffG, float staffB, int stallType) {
    glColor3f(r, g, b);
    drawTriangleShape(x - 15.0f, y + h, x + w / 2.0f, y + h + 42.0f, x + w + 15.0f, y + h);

    glColor3f(0.98f, 0.95f, 0.88f);
    for (int i = 0; i < 5; i++) {
        float sx = x + (w / 5.0f) * i;
        if (i % 2 == 0) {
            drawTriangleShape(sx, y + h, sx + w / 5.0f, y + h, x + w / 2.0f, y + h + 42.0f);
        }
    }

    glColor3f(0.52f, 0.35f, 0.20f);
    drawRect(x, y, x + w, y + h);

    renderStallStaff(x + w / 2.0f, y + h * 0.45f, staffR, staffG, staffB, stallType);

    glColor3f(0.35f, 0.22f, 0.10f);
    drawRect(x - 5.0f, y + h * 0.35f, x + w + 5.0f, y + h * 0.55f);

    glColor3f(0.68f, 0.52f, 0.35f);
    drawRect(x, y, x + w, y + h * 0.35f);

    if (dayNightFactor > 0.4f) {
        glColor3f(1.0f, 0.90f, 0.20f);
        drawFilledCircle(x + 15.0f, y + h - 10.0f, 6.0f, 12);
        drawFilledCircle(x + w - 15.0f, y + h - 10.0f, 6.0f, 12);
    }
}

// ==========================================
// REALISTIC YET SIMPLE FERRIS WHEEL ENGINE
// ==========================================

void renderRealisticFerrisWheel(float cx, float cy, float outerRadius) {
    float innerRadius = outerRadius - 20.0f;
    float groundY = 145.0f; // Base standing in the green grass

    // 1. Dual-Truss A-Frame Stand
    glColor3f(0.20f, 0.22f, 0.26f);
    drawSegmentLine(cx, cy, cx - 80.0f, groundY, 6.0f);
    drawSegmentLine(cx, cy, cx + 80.0f, groundY, 6.0f);
    drawSegmentLine(cx, cy, cx - 60.0f, groundY, 4.0f);
    drawSegmentLine(cx, cy, cx + 60.0f, groundY, 4.0f);

    // Horizontal Cross Bars
    drawSegmentLine(cx - 50.0f, groundY + 60.0f, cx + 50.0f, groundY + 60.0f, 4.0f);
    drawSegmentLine(cx - 30.0f, groundY + 110.0f, cx + 30.0f, groundY + 110.0f, 4.0f);

    // 2. Double Wheel Rims
    glColor3f(0.80f, 0.15f, 0.15f);
    drawOutlineCircle(cx, cy, outerRadius, 60, 4.0f);
    glColor3f(0.30f, 0.35f, 0.40f);
    drawOutlineCircle(cx, cy, innerRadius, 60, 3.0f);

    // 3. Structural Cross-Bracing between Rims
    int numSections = 16;
    for (int i = 0; i < numSections; i++) {
        float a1 = ferrisAngle + (i * 2.0f * PI / numSections);
        float a2 = ferrisAngle + ((i + 1) * 2.0f * PI / numSections);

        float ox1 = cx + outerRadius * cosf(a1);
        float oy1 = cy + outerRadius * sinf(a1);
        float ix2 = cx + innerRadius * cosf(a2);
        float iy2 = cy + innerRadius * sinf(a2);

        glColor3f(0.55f, 0.58f, 0.62f);
        drawSegmentLine(ox1, oy1, ix2, iy2, 1.5f);
    }

    // 4. Main Spokes and Upright Cabins
    int numCabins = 8;
    for (int i = 0; i < numCabins; i++) {
        float angle = ferrisAngle + (i * 2.0f * PI / numCabins);
        float px = cx + outerRadius * cosf(angle);
        float py = cy + outerRadius * sinf(angle);

        // Spokes connecting center to outer rim
        glColor3f(0.70f, 0.72f, 0.75f);
        drawSegmentLine(cx, cy, px, py, 2.5f);

        // Upright Gondola / Cabin Rendering
        float cw = 22.0f; // Cabin width
        float ch = 18.0f; // Cabin height
        float topY = py - 4.0f;

        // Hanging Mount Pin
        glColor3f(0.2f, 0.2f, 0.2f);
        drawSegmentLine(px, py, px, topY, 2.5f);

        // Color variation for cabins
        if (i % 4 == 0) glColor3f(0.90f, 0.20f, 0.20f);
        else if (i % 4 == 1) glColor3f(0.20f, 0.75f, 0.30f);
        else if (i % 4 == 2) glColor3f(0.15f, 0.50f, 0.90f);
        else glColor3f(0.95f, 0.75f, 0.10f);

        // Cabin Body & Roof
        drawRect(px - cw / 2.0f, topY - ch, px + cw / 2.0f, topY);
        drawTriangleShape(px - cw / 2.0f - 2.0f, topY, px + cw / 2.0f + 2.0f, topY, px, topY + 5.0f);

        // Cabin Window
        glColor3f(0.95f, 0.95f, 0.98f);
        drawRect(px - cw / 4.0f, topY - ch * 0.65f, px + cw / 4.0f, topY - ch * 0.25f);
    }

    // 5. Heavy Central Axle Hub
    glColor3f(0.15f, 0.15f, 0.18f);
    drawFilledCircle(cx, cy, 14.0f, 24);
    glColor3f(0.85f, 0.85f, 0.85f);
    drawOutlineCircle(cx, cy, 14.0f, 24, 2.0f);
    drawFilledCircle(cx, cy, 5.0f, 16);
}

void renderGreatBanyanTree() {
    glColor3f(0.32f, 0.20f, 0.10f);
    drawRect(370.0f, 280.0f, 430.0f, 490.0f);

    glColor3f(0.20f, 0.12f, 0.05f);
    drawSegmentLine(385.0f, 280.0f, 385.0f, 490.0f, 2.0f);
    drawSegmentLine(400.0f, 280.0f, 400.0f, 490.0f, 2.0f);
    drawSegmentLine(415.0f, 280.0f, 415.0f, 490.0f, 2.0f);

    float fR = lerpColor(0.15f, 0.03f, dayNightFactor);
    float fG = lerpColor(0.60f, 0.22f, dayNightFactor);
    float fB = lerpColor(0.20f, 0.08f, dayNightFactor);

    glColor3f(fR, fG, fB);
    drawFilledCircle(400.0f, 590.0f, 110.0f, 40);
    drawFilledCircle(310.0f, 540.0f, 90.0f, 40);
    drawFilledCircle(490.0f, 540.0f, 90.0f, 40);
    drawFilledCircle(400.0f, 490.0f, 80.0f, 40);

    if (dayNightFactor > 0.4f) {
        glColor3f(1.0f, 0.2f, 0.2f); drawFilledCircle(330.0f, 510.0f, 8.0f, 12);
        glColor3f(0.2f, 1.0f, 0.3f); drawFilledCircle(470.0f, 520.0f, 8.0f, 12);
        glColor3f(1.0f, 0.9f, 0.1f); drawFilledCircle(400.0f, 560.0f, 9.0f, 12);
        glColor3f(0.1f, 0.7f, 1.0f); drawFilledCircle(360.0f, 600.0f, 8.0f, 12);
        glColor3f(1.0f, 0.4f, 0.8f); drawFilledCircle(440.0f, 590.0f, 8.0f, 12);
    }
}

void renderFloorAlpona(float cx, float cy, float radius) {
    glColor3f(0.95f, 0.95f, 0.95f);
    drawOutlineCircle(cx, cy, radius, 36, 1.5f);
    drawOutlineCircle(cx, cy, radius * 0.6f, 28, 1.5f);

    for (int i = 0; i < 12; i++) {
        float a = i * (2.0f * PI / 12.0f);
        float px = cx + radius * 0.8f * cosf(a);
        float py = cy + radius * 0.8f * sinf(a);
        drawFilledCircle(px, py, radius * 0.15f, 12);
    }
}

// ==========================================
// SCENE 2: RIVER LANDSCAPE
// ==========================================

void renderRiverLandscape() {
    renderSkyBackground();

    float fsR = lerpColor(0.22f, 0.04f, dayNightFactor);
    float fsG = lerpColor(0.58f, 0.22f, dayNightFactor);
    float fsB = lerpColor(0.20f, 0.08f, dayNightFactor);
    glColor3f(fsR, fsG, fsB);
    drawRect(0.0f, 420.0f, 800.0f, 480.0f);

    for (int x = 40; x < 800; x += 160) {
        glColor3f(0.65f, 0.25f, 0.15f);
        drawRect((float)x, 480.0f, (float)x + 45.0f, 510.0f);
        glColor3f(0.85f, 0.15f, 0.1f);
        drawTriangleShape((float)x - 5.0f, 510.0f, (float)x + 22.5f, 530.0f, (float)x + 50.0f, 510.0f);
    }

    float wTopR = lerpColor(0.20f, 0.08f, dayNightFactor);
    float wTopG = lerpColor(0.50f, 0.22f, dayNightFactor);
    float wTopB = lerpColor(0.85f, 0.50f, dayNightFactor);

    float wBotR = lerpColor(0.35f, 0.12f, dayNightFactor);
    float wBotG = lerpColor(0.65f, 0.32f, dayNightFactor);
    float wBotB = lerpColor(0.95f, 0.62f, dayNightFactor);

    drawGradientRect(0.0f, 180.0f, 800.0f, 420.0f, wTopR, wTopG, wTopB, wBotR, wBotG, wBotB);

    glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
    for (int y = 200; y < 410; y += 25) {
        float xStart = fmodf((float)y * 12.0f + waveOffset, 800.0f);
        drawSegmentLine(xStart, (float)y, xStart + 40.0f, (float)y, 1.5f);
        drawSegmentLine(fmodf(xStart + 300.0f, 800.0f), (float)y, fmodf(xStart + 340.0f, 800.0f), (float)y, 1.5f);
    }

    float nsR = lerpColor(0.25f, 0.05f, dayNightFactor);
    float nsG = lerpColor(0.68f, 0.25f, dayNightFactor);
    float nsB = lerpColor(0.22f, 0.08f, dayNightFactor);
    glColor3f(nsR, nsG, nsB);
    drawRect(0.0f, 0.0f, 800.0f, 180.0f);
}

void renderRacingBoat(float x, float y, float r, float g, float b, float oarPhase) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
    glVertex2f(x - 60.0f, y);
    glVertex2f(x + 75.0f, y);
    glVertex2f(x + 95.0f, y + 20.0f);
    glVertex2f(x - 75.0f, y + 20.0f);
    glEnd();

    glColor3f(0.95f, 0.85f, 0.2f);
    drawSegmentLine(x - 72.0f, y + 16.0f, x + 90.0f, y + 16.0f, 2.5f);

    for (int i = 0; i < 5; i++) {
        float rowerX = x - 45.0f + i * 26.0f;

        glColor3f(0.92f, 0.76f, 0.62f);
        drawFilledCircle(rowerX, y + 30.0f, 5.5f, 12);

        glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
        drawTriangleShape(rowerX - 7.0f, y + 20.0f, rowerX + 7.0f, y + 20.0f, rowerX, y + 30.0f);

        float oarAngle = sinf(oarPhase + rowerX * 0.05f) * 14.0f;
        glColor3f(0.40f, 0.25f, 0.12f);
        drawSegmentLine(rowerX, y + 24.0f, rowerX + oarAngle, y - 10.0f, 2.5f);
    }
}

void renderFinishBanner() {
    drawSegmentLine(720.0f, 180.0f, 720.0f, 420.0f, 4.0f);

    for (int y = 180; y < 420; y += 16) {
        if ((y / 16) % 2 == 0) glColor3f(0.1f, 0.1f, 0.1f);
        else glColor3f(0.95f, 0.95f, 0.95f);

        drawRect(714.0f, (float)y, 726.0f, (float)y + 16.0f);
    }
}

void renderGrazingCow(float x, float y) {
    glColor3f(0.95f, 0.95f, 0.95f);
    drawRect(x, y, x + 42.0f, y + 24.0f);

    glColor3f(0.15f, 0.15f, 0.15f);
    drawFilledCircle(x + 12.0f, y + 14.0f, 6.0f, 12);
    drawFilledCircle(x + 30.0f, y + 9.0f, 7.0f, 12);

    drawSegmentLine(x + 6.0f, y, x + 6.0f, y - 14.0f, 3.5f);
    drawSegmentLine(x + 14.0f, y, x + 14.0f, y - 14.0f, 3.5f);
    drawSegmentLine(x + 28.0f, y, x + 28.0f, y - 14.0f, 3.5f);
    drawSegmentLine(x + 36.0f, y, x + 36.0f, y - 14.0f, 3.5f);

    glColor3f(0.95f, 0.95f, 0.95f);
    drawRect(x + 42.0f, y + 10.0f, x + 55.0f, y + 26.0f);
}

// ==========================================
// CHARACTER RENDERING ENGINE
// ==========================================

void renderHumanCharacter(float x, float y, float scale,
                          float r, float g, float b, float swing, bool facingRight) {
    float headRadius = 11.0f * scale;
    float bodyHeight = 48.0f * scale;
    float legHeight = 42.0f * scale;

    float headY = y + legHeight + bodyHeight + headRadius;
    float neckY = y + legHeight + bodyHeight;
    float hipY = y + legHeight;

    glColor3f(0.92f, 0.76f, 0.62f);
    drawFilledCircle(x, headY, headRadius, 18);

    glColor3f(0.1f, 0.1f, 0.1f);
    float eyePos = facingRight ? 4.0f * scale : -4.0f * scale;
    drawFilledCircle(x + eyePos, headY + 2.0f, 2.0f * scale, 8);

    glColor3f(r, g, b);
    drawRect(x - 11.0f * scale, hipY, x + 11.0f * scale, neckY);

    glColor3f(0.12f, 0.15f, 0.28f);
    float leftLegX = x - 5.0f * scale + swing * 9.0f;
    float rightLegX = x + 5.0f * scale - swing * 9.0f;

    drawSegmentLine(x - 5.0f * scale, hipY, leftLegX, y, 4.0f * scale);
    drawSegmentLine(x + 5.0f * scale, hipY, rightLegX, y, 4.0f * scale);

    glColor3f(0.08f, 0.08f, 0.08f);
    drawFilledCircle(leftLegX, y, 3.5f * scale, 10);
    drawFilledCircle(rightLegX, y, 3.5f * scale, 10);

    glColor3f(r * 0.82f, g * 0.82f, b * 0.82f);
    drawSegmentLine(x - 11.0f * scale, neckY - 6.0f, x - 17.0f * scale - swing * 6.0f, hipY + 12.0f, 3.5f * scale);
    drawSegmentLine(x + 11.0f * scale, neckY - 6.0f, x + 17.0f * scale + swing * 6.0f, hipY + 12.0f, 3.5f * scale);
}

void renderBalloonVendorCharacter(float x, float y, float swing, bool facingRight) {
    renderHumanCharacter(x, y, 1.30f, 0.22f, 0.48f, 0.78f, swing, facingRight);

    float handX = facingRight ? x + 24.0f : x - 24.0f;
    float handY = y + 72.0f;

    glColor3f(0.85f, 0.85f, 0.85f);
    drawSegmentLine(handX, handY, x - 32.0f, handY + 115.0f, 1.2f);
    drawSegmentLine(handX, handY, x - 12.0f, handY + 130.0f, 1.2f);
    drawSegmentLine(handX, handY, x + 12.0f, handY + 125.0f, 1.2f);
    drawSegmentLine(handX, handY, x + 32.0f, handY + 110.0f, 1.2f);

    glColor3f(0.95f, 0.15f, 0.15f); drawFilledCircle(x - 32.0f, handY + 125.0f, 15.0f, 20);
    glColor3f(0.15f, 0.85f, 0.25f); drawFilledCircle(x - 12.0f, handY + 142.0f, 15.0f, 20);
    glColor3f(0.95f, 0.85f, 0.10f); drawFilledCircle(x + 12.0f, handY + 137.0f, 15.0f, 20);
    glColor3f(0.80f, 0.15f, 0.85f); drawFilledCircle(x + 32.0f, handY + 120.0f, 15.0f, 20);
}

void renderSpectatorsOnBank() {
    renderHumanCharacter(320.0f, 60.0f, 1.15f, 0.85f, 0.15f, 0.2f, 0.0f, true);
    renderHumanCharacter(370.0f, 55.0f, 1.15f, 0.15f, 0.65f, 0.8f, 0.0f, false);
    renderHumanCharacter(420.0f, 65.0f, 1.15f, 0.25f, 0.82f, 0.3f, 0.0f, true);
    renderHumanCharacter(470.0f, 50.0f, 1.15f, 0.90f, 0.42f, 0.1f, 0.0f, true);
    renderHumanCharacter(520.0f, 60.0f, 1.15f, 0.65f, 0.20f, 0.7f, 0.0f, false);
}

// ==========================================
// CORE DISPLAY & UPDATE LOOPS
// ==========================================

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (currentScene == 0) {
        // --- VILLAGE FAIR SCENE ---
        renderSkyBackground();
        renderCelestialBodies();
        renderStarfield();
        renderCloudCover();

        renderBuntingGarland();
        renderGroundTerrain();

        // 1. Background Stalls
        renderStall(40.0f, 270.0f, 110.0f, 95.0f, 0.85f, 0.20f, 0.10f, 0.20f, 0.60f, 0.80f, 1);
        renderStall(160.0f, 275.0f, 115.0f, 100.0f, 0.10f, 0.55f, 0.75f, 0.90f, 0.40f, 0.20f, 2);
        renderStall(490.0f, 270.0f, 120.0f, 100.0f, 0.85f, 0.45f, 0.10f, 0.15f, 0.75f, 0.30f, 1);
        renderStall(630.0f, 275.0f, 125.0f, 105.0f, 0.20f, 0.65f, 0.30f, 0.80f, 0.20f, 0.70f, 2);

        // 2. Realistic & Simple Ferris Wheel (Centered at x: 160, y: 260 with outer radius: 150)
        renderRealisticFerrisWheel(160.0f, 260.0f, 150.0f);

        renderGreatBanyanTree();

        // Ground Alpona Patterns
        renderFloorAlpona(450.0f, 65.0f, 40.0f);
        renderFloorAlpona(580.0f, 50.0f, 30.0f);

        // Pedestrians on front path (Restricted to x >= 340.0f)
        renderHumanCharacter(person1X, 35.0f, 1.25f, 0.9f, 0.15f, 0.2f, legAngle, person1Right);
        renderHumanCharacter(person2X, 25.0f, 1.30f, 0.1f, 0.7f, 0.8f, -legAngle, person2Right);
        renderHumanCharacter(person3X, 15.0f, 1.20f, 0.8f, 0.2f, 0.7f, legAngle, person3Right);
        renderHumanCharacter(person4X, 30.0f, 1.15f, 0.2f, 0.8f, 0.3f, -legAngle, person4Right);
        renderHumanCharacter(person5X, 20.0f, 1.25f, 0.9f, 0.5f, 0.1f, legAngle, person5Right);

        renderBalloonVendorCharacter(vendorX, 40.0f, legAngle, vendorRight);
    }
    else if (currentScene == 1) {
        // --- BOAT RACE SCENE ---
        renderRiverLandscape();
        renderCelestialBodies();
        renderStarfield();
        renderCloudCover();

        renderFinishBanner();

        // Racing Boats
        renderRacingBoat(boat1X, 350.0f, 0.90f, 0.15f, 0.15f, waveOffset * 0.1f);
        renderRacingBoat(boat2X, 280.0f, 0.15f, 0.75f, 0.25f, waveOffset * 0.12f);
        renderRacingBoat(boat3X, 210.0f, 0.20f, 0.45f, 0.92f, waveOffset * 0.09f);

        renderGrazingCow(140.0f, 110.0f);
        renderGrazingCow(220.0f, 100.0f);

        renderSpectatorsOnBank();
    }

    glutSwapBuffers();
}

void update(int value) {
    if (!isPaused) {
        if (isNightMode && dayNightFactor < 1.0f) {
            dayNightFactor += 0.02f;
        } else if (!isNightMode && dayNightFactor > 0.0f) {
            dayNightFactor -= 0.02f;
        }

        // Ferris Wheel Rotation & Waves
        ferrisAngle += 0.018f * animSpeed;
        if (ferrisAngle > 2.0f * PI) ferrisAngle -= 2.0f * PI;

        sunAngle += 0.01f * animSpeed;
        waveOffset += 2.0f * animSpeed;

        if (moonGrowing) {
            moonGlow += 0.01f * animSpeed;
            if (moonGlow >= 1.0f) moonGrowing = false;
        } else {
            moonGlow -= 0.01f * animSpeed;
            if (moonGlow <= 0.0f) moonGrowing = true;
        }

        for (int i = 0; i < 50; i++) {
            starAlpha[i] = 0.2f + (float)(rand() % 80) / 100.0f;
        }

        // Cloud Motion
        cloud1X += 0.4f * animSpeed;
        if (cloud1X > 850.0f) cloud1X = -50.0f;
        cloud2X += 0.3f * animSpeed;
        if (cloud2X > 850.0f) cloud2X = -50.0f;
        cloud3X += 0.5f * animSpeed;
        if (cloud3X > 850.0f) cloud3X = -50.0f;

        // Leg Swing Motion
        if (legSwingForward) {
            legAngle += 0.03f * animSpeed;
            if (legAngle >= 0.8f) legSwingForward = false;
        } else {
            legAngle -= 0.03f * animSpeed;
            if (legAngle <= -0.8f) legSwingForward = true;
        }

        // Pedestrian Movement Engine (Restricted to x >= 340.0f)
        float step = 0.6f * animSpeed;

        if (person1Right) {
            person1X += step;
            if (person1X > 750.0f) person1Right = false;
        } else {
            person1X -= step;
            if (person1X < 350.0f) person1Right = true;
        }

        if (person2Right) {
            person2X += step * 0.85f;
            if (person2X > 750.0f) person2Right = false;
        } else {
            person2X -= step * 0.85f;
            if (person2X < 340.0f) person2Right = true;
        }

        if (person3Right) {
            person3X += step * 1.1f;
            if (person3X > 750.0f) person3Right = false;
        } else {
            person3X -= step * 1.1f;
            if (person3X < 360.0f) person3Right = true;
        }

        if (person4Right) {
            person4X += step * 0.75f;
            if (person4X > 750.0f) person4Right = false;
        } else {
            person4X -= step * 0.75f;
            if (person4X < 340.0f) person4Right = true;
        }

        if (person5Right) {
            person5X += step * 0.95f;
            if (person5X > 750.0f) person5Right = false;
        } else {
            person5X -= step * 0.95f;
            if (person5X < 370.0f) person5Right = true;
        }

        if (vendorRight) {
            vendorX += step * 0.5f;
            if (vendorX > 720.0f) vendorRight = false;
        } else {
            vendorX -= step * 0.5f;
            if (vendorX < 380.0f) vendorRight = true;
        }

        // Boat Race Movements
        boat1X += 2.2f * animSpeed;
        boat2X += 2.5f * animSpeed;
        boat3X += 2.0f * animSpeed;

        if (boat1X > 800.0f) boat1X = -80.0f;
        if (boat2X > 800.0f) boat2X = -80.0f;
        if (boat3X > 800.0f) boat3X = -80.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ==========================================
// SYSTEM INITIALIZATION & INPUT HANDLING
// ==========================================

void keyboard(unsigned char key, int x, int y) {
    if (key == 's' || key == 'S') {
        currentScene = 1;
    } else if (key == 'f' || key == 'F') {
        currentScene = 0;
    } else if (key == 'n' || key == 'N') {
        isNightMode = !isNightMode;
    } else if (key == 'p' || key == 'P') {
        isPaused = !isPaused;
    } else if (key == '+' || key == '=') {
        animSpeed += 0.2f;
    } else if (key == '-' || key == '_') {
        if (animSpeed > 0.2f) animSpeed -= 0.2f;
    } else if (key == 27) {
        exit(0);
    }
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 800.0, 0.0, 800.0);

    for (int i = 0; i < 50; i++) {
        starCoords[i][0] = (float)(rand() % 780 + 10);
        starCoords[i][1] = (float)(rand() % 420 + 360);
        starAlpha[i] = 0.4f + (float)(rand() % 60) / 100.0f;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Extended Village Fair & Boat Race Simulation - OpenGL GLUT");

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
    return 0;
}
