#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#include <iostream>
#include "../lib/GL/glew.h"
#include "../lib/GL/glut.h"
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/matrix_transform.hpp"

GLuint vertexBuffer, vertexShader, fragmentShader, programID, texID;
GLint uniformPVM, uniformModel, imageWidth, imageHeight, nrChannels;
GLubyte *imageData;

glm::mat4 transformMatrix, perspectiveMatrix, viewMatrix, modelMatrix = glm::mat4(1.0f);

GLfloat cubeVertex[] = {
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,      0.5f, -0.5f, -0.5f,     1.0f, 0.0f,     0.0f, 0.0f, -1.0f,      0.5f, 0.5f, -0.5f,      1.0f, 1.0f,     0.0f, 0.0f, -1.0f,
    0.5f, 0.5f, -0.5f,      1.0f, 1.0f,     0.0f, 0.0f, -1.0f,      -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,     0.0f, 0.0f, -1.0f,      -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, 0.0f, -1.0f,
    
    -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,       0.5f, -0.5f, 0.5f,      1.0f, 0.0f,     0.0f, 0.0f, 1.0f,       0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     0.0f, 0.0f, 1.0f,       -0.5f, 0.5f, 0.5f,      0.0f, 1.0f,     0.0f, 0.0f, 1.0f,       -0.5f, -0.5f, 0.5f,     0.0f, 0.0f,     0.0f, 0.0f, 1.0f,
    
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, -1.0f, 0.0f,      0.5f, -0.5f, -0.5f,     1.0f, 0.0f,     0.0f, -1.0f, 0.0f,      0.5f, -0.5f, 0.5f,      1.0f, 1.0f,     0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f,      1.0f, 1.0f,     0.0f, -1.0f, 0.0f,      -0.5f, -0.5, 0.5f,      0.0f, 1.0f,     0.0f, -1.0f, 0.0f,      -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
    
    -0.5f, 0.5f, -0.5f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,       0.5f, 0.5f, -0.5f,      1.0f, 0.0f,     0.0f, 1.0f, 0.0f,       0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     0.0f, 1.0f, 0.0f,       -0.5f, 0.5, 0.5f,       0.0f, 1.0f,     0.0f, 1.0f, 0.0f,       -0.5f, 0.5f, -0.5f,     0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
    
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,      -0.5f, -0.5f, 0.5f,     1.0f, 0.0f,     -1.0f, 0.0f, 0.0f,      -0.5f, 0.5f, 0.5f,      1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f,      1.0f, 1.0f,     -1.0f, 0.0f, 0.0f,      -0.5f, 0.5f, -0.5f,     0.0f, 1.0f,     -1.0f, 0.0f, 0.0f,      -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,     -1.0f, 0.0f, 0.0f,
    
    0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,      0.5f, -0.5f, 0.5f,      1.0f, 0.0f,     1.0f, 0.0f, 0.0f,      0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f,       1.0f, 1.0f,     1.0f, 0.0f, 0.0f,      0.5f, 0.5f, -0.5f,      0.0f, 1.0f,     1.0f, 0.0f, 0.0f,      0.5f, -0.5f, -0.5f,     0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
};

const GLchar* const vertexSource = 
    "#version 130\r\n"
    "in vec3 inpPosition;"
    "in vec2 inpTexCoord;"
    "in vec3 inpNormal;"
    "out vec2 fragTexCoord;"
    "out vec3 fragNormal;"
    "uniform mat4 PVM;"
    "void main(){"
    "fragTexCoord = inpTexCoord;"
    "fragNormal = inpNormal;"
    "gl_Position = PVM * vec4(inpPosition, 1.0f);"
    "}";
    
const GLchar* const fragmentSource = 
    "#version 130\r\n"
    "in vec2 fragTexCoord;"
    "in vec3 fragNormal;"
    "uniform sampler2D ourTex;"
    "uniform mat4 modelMatrix;"
    "uniform vec3 lightDirection = vec3(0.0f, 0.8f, 0.6f);"
    "out vec4 fragColor;"
    "void main(){"
    "vec4 originalColor, ambientColor, diffuseColor;"
    "vec3 normal = (modelMatrix * vec4(fragNormal, 0.0f)).xyz;"
    "originalColor = texture(ourTex, fragTexCoord);"
    "ambientColor = originalColor;"
    "diffuseColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);"
    "float diffuse = dot(normal, -lightDirection);"
    "if(diffuse > 0)"
    "diffuseColor = diffuse * originalColor;"
    "fragColor = 0.1 * ambientColor + 0.9 * diffuseColor;"
    "}";

void updateMatrices(){
    float w = (float)GLUT_SCREEN_WIDTH;
    float h = (float)GLUT_SCREEN_HEIGHT;
    
    viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); 
    perspectiveMatrix = glm::perspective(80.0f, w / h, 0.1f, 10.0f);
    transformMatrix = perspectiveMatrix * viewMatrix * modelMatrix;
    
    glUniformMatrix4fv(uniformPVM, 1, GL_FALSE, &transformMatrix[0][0]);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, &modelMatrix[0][0]);
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glFlush();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'w' :
        modelMatrix = glm::rotate(modelMatrix, 0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
        break;
        case 'a' :
        modelMatrix = glm::rotate(modelMatrix, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
        case 's' :
        modelMatrix = glm::rotate(modelMatrix, -0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
        break;
        case 'd' :
        modelMatrix = glm::rotate(modelMatrix, 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
        case 'q' :
        modelMatrix = glm::rotate(modelMatrix, 0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
        break;
        case 'e' :
        modelMatrix = glm::rotate(modelMatrix, -0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
        break;
    }
    
    updateMatrices();
    glutPostRedisplay();
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutCreateWindow("opengl project");
    
    if(glewInit() != GLEW_OK){
        std::cout << "error : " << glewGetErrorString(0);
    }
    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertex), cubeVertex, GL_STATIC_DRAW);
    
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    imageData = stbi_load("../resource/box.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(imageData);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
    
    glBindAttribLocation(programID, 0, "position");
    glBindAttribLocation(programID, 1, "texCoord");
    glBindAttribLocation(programID, 2, "normal");
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, 0);
    glCompileShader(vertexShader);
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, 0);
    glCompileShader(fragmentShader);
    
    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);
    
    uniformPVM = glGetUniformLocation(programID, "PVM");
    uniformModel = glGetUniformLocation(programID, "modelMatrix");
    glUseProgram(programID);
    updateMatrices();
    glEnable(GL_DEPTH_TEST);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
}
