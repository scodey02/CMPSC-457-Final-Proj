#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <string>

// Camera and object settings
float angle = 0.0f;
float lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};  // Light position
float lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};  // White light
bool useLighting = true;
bool useTexture = false;
GLuint textureID;
std::string description = "Use 'l' to toggle lighting, 't' to toggle texture, 'r' to rotate, 'c' to toggle curve, 'rgb' to change color.";

// Bézier curve settings
bool drawCurve = false;         // Toggle the curve on/off
float t = 0.0f;                 // Parameter for curve animation
float tIncrement = 0.01f;       // Speed of animation
float controlPoints[3][3] = {   // Control points for the Bézier curve
    {0.0f, 0.0f, 0.0f},        // Start at the cube's center
    {0.5f, 1.0f, 0.0f},        // Control point
    {1.0f, 0.0f, 0.0f}         // End point
};

// Color and material properties
GLfloat mat_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat mat_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Default color (white)
GLfloat mat_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat mat_shininess[] = {50.0f};  // Shiny material for specular highlights

// Current cube color (initialized as white)
float cubeColor[3] = {1.0f, 1.0f, 1.0f};

// Function for drawing a text box background
void drawTextBox(float x, float y, float width, float height) {
    glBegin(GL_QUADS);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);  // Semi-transparent black background
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Function for drawing text inside the text box
void drawText(float x, float y, const std::string &text) {
    glColor3f(1.0f, 1.0f, 1.0f);  // White color for text
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Function for drawing a cube (basic model)
void drawCube() {
    glBegin(GL_QUADS);
    glColor3fv(cubeColor);  // Set the color based on the current cube color
    // Front face
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    // Back face
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    // Left face
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    // Right face
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    // Top face
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    // Bottom face
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glEnd();
}

// Bézier curve function
void bezierCurve(float t, float &x, float &y, float &z) {
    x = (1 - t) * (1 - t) * controlPoints[0][0] +
        2 * (1 - t) * t * controlPoints[1][0] +
        t * t * controlPoints[2][0];
    y = (1 - t) * (1 - t) * controlPoints[0][1] +
        2 * (1 - t) * t * controlPoints[1][1] +
        t * t * controlPoints[2][1];
    z = (1 - t) * (1 - t) * controlPoints[0][2] +
        2 * (1 - t) * t * controlPoints[2][2];
}

// Function for drawing the Bézier curve
void drawBezierCurve() {
    if (!drawCurve) return; // Only draw if enabled

    glPointSize(5.0f); // Make the dots visible
    glBegin(GL_POINTS);
    float x, y, z;
    bezierCurve(t, x, y, z); // Calculate point along the curve
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the dots
    glVertex3f(x, y, z); // Draw the current dot
    glEnd();

    t += tIncrement; // Update the animation
    if (t > 1.0f) t = 0.0f; // Loop the animation
}

// Function to apply transformations and update the scene
void applyTransformations() {
    glRotatef(angle, 1.0, 1.0, 0.0); // Rotate object
    glTranslatef(0.5f, 0.0f, 0.0f);  // Translate object
}

// Function for drawing the scene
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the buffer
    glLoadIdentity();

    // Set up the camera
    glTranslatef(0.0f, 0.0f, -5.0f);

    if (useLighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor); // Set the diffuse light color
        glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor); // Set the specular light color
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor); // Set the ambient light color
    } else {
        glDisable(GL_LIGHTING);
    }

    // Apply material properties (shiny or dull)
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    if (useTexture) {
        glEnable(GL_TEXTURE_2D); // Enable texture
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glDisable(GL_TEXTURE_2D); // Disable texture
    }

    applyTransformations();
    drawCube();

    drawBezierCurve(); // Draw the curve if enabled

    drawTextBox(-1.0f, -0.95f, 2.0f, 0.1f);
    drawText(-0.95f, -0.9f, description);

    glutSwapBuffers();
}

// Keyboard interaction to toggle between different effects
void keyboard(unsigned char key, int x, int y) {
    if (key == 'c') { // Toggle the curve
        drawCurve = !drawCurve;
        description = "Curve toggled: " + std::string(drawCurve ? "ON" : "OFF");
    }
    if (key == 'r') { // Rotate the object
        angle += 5.0f;
        description = "Object rotated. Angle: " + std::to_string(angle);
    }
    if (key == 'l') { // Toggle lighting
        useLighting = !useLighting;
        description = "Lighting toggled: " + std::string(useLighting ? "ON" : "OFF");
    }
    if (key == 't') { // Toggle texture
        useTexture = !useTexture;
        description = "Texture toggled: " + std::string(useTexture ? "ON" : "OFF");
    }
    if (key == 'r') { // Change cube color to red
        cubeColor[0] = 1.0f; cubeColor[1] = 0.0f; cubeColor[2] = 0.0f;
    }
    if (key == 'g') { // Change cube color to green
        cubeColor[0] = 0.0f; cubeColor[1] = 1.0f; cubeColor[2] = 0.0f;
    }
    if (key == 'b') { // Change cube color to blue
        cubeColor[0] = 0.0f; cubeColor[1] = 0.0f; cubeColor[2] = 1.0f;
    }
    if (key == 'w') { // Reset color to white
        cubeColor[0] = 1.0f; cubeColor[1] = 1.0f; cubeColor[2] = 1.0f;
    }
}

// Function to set up the projection
void setProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 100.0);  // Perspective projection
    glMatrixMode(GL_MODELVIEW);
}

// Initialization function
void init() {
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Set background color
    setProjection();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D OpenGL Simulation with Text Box");

    init();
    glutDisplayFunc(drawScene);
    glutIdleFunc(drawScene);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}