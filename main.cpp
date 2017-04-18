
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "MathHelper.h"

using glm::mat4;
using glm::mat3;
using glm::vec3;
using glm::vec4;
using namespace std;

typedef struct {
    GLuint Projection;
    GLuint View;
    GLuint Model;
    GLuint NormalMatrix;
    GLuint LightPosition;
    GLuint AmbientMaterial;
    GLuint DiffuseMaterial;
    GLuint TessLevelInner;
    GLuint TessLevelOuter;
} ShaderUniforms;

GLuint program;
GLuint vao;
GLuint vbo;

MathHelper mathhelper;
static mat4 ProjectionMatrix;
static mat4 ModelviewMatrix;
static mat3 NormalMatrix;
static ShaderUniforms Uniforms;
static mat4 modelMatrix, viewMatrix, projMatrix;
static vec3 camPos, lookat;

//file reader
char* readFile(const char* filename) {
    FILE * fp = fopen (filename, "r");
    fseek(fp, 0, SEEK_END);
    long length = ftell(fp);
    fseek(fp,0, SEEK_SET);
    char* contents = new char[length+1];
    for (int i=0;i<length+1;i++){
        contents[i]=0;
    }
    fread (contents, 1, length, fp);
    contents[length] = '\0';
    fclose(fp);
    return contents;
}

//shader program funcitons
static void CheckStatus( GLuint obj )
{
    GLint status = GL_FALSE;
    if( glIsShader(obj) ) glGetShaderiv( obj, GL_COMPILE_STATUS, &status );
    if( glIsProgram(obj) ) glGetProgramiv( obj, GL_LINK_STATUS, &status );
    if( status == GL_TRUE ) return;
    GLchar log[ 1 << 16 ] = { 0 };
    if( glIsShader(obj) ) glGetShaderInfoLog( obj, sizeof(log), NULL, log );
    if( glIsProgram(obj) ) glGetProgramInfoLog( obj, sizeof(log), NULL, log );
    std::cerr << log << std::endl;
    exit( -1 );
}

static void AttachShader( GLuint program, GLenum type, const char* src )
{
    GLuint shader = glCreateShader( type );
    glShaderSource( shader, 1, &src, NULL );
    glCompileShader( shader );
    CheckStatus( shader );
    glAttachShader( program, shader );
    glDeleteShader( shader );
}

static GLuint LoadProgram( const char* vert, const char* tcs, const char* tes,const char* geom, const char* frag )
{
    GLuint prog = glCreateProgram();
    if( vert ) AttachShader( prog, GL_VERTEX_SHADER, vert );
    if( tcs ) AttachShader( prog, GL_TESS_CONTROL_SHADER, tcs );
    if( tes ) AttachShader( prog, GL_TESS_EVALUATION_SHADER, tes );
    if( geom ) AttachShader( prog, GL_GEOMETRY_SHADER, geom );
    if( frag ) AttachShader( prog, GL_FRAGMENT_SHADER, frag );
    glLinkProgram( prog );
    CheckStatus( prog );
    return prog;
}
//init data
void initData()
{
    const char* verts = readFile("verShader.txt");
    const char* tcs = readFile("tcsShader.txt");
    const char* tes = readFile("tesShader.txt");
    const char* geoms = readFile("geoShader.txt");
    const char* frags = readFile("fragShader.txt");
    program = LoadProgram( verts, tcs, tes, geoms,frags );
    delete verts;
    delete tcs;
    delete tes;
    delete geoms;
    delete frags;
}
//build buffers
static void BuildBuffers(){
    
    float data[] ={
        0.000f,  0.000f,  1.000f,
        0.894f,  0.000f,  0.447f,
        0.276f,  0.851f,  0.447f,
        -0.724f,  0.526f,  0.447f,
        -0.724f, -0.526f,  0.447f,
        0.276f, -0.851f,  0.447f,
        0.724f,  0.526f, -0.447f,
        -0.276f,  0.851f, -0.447f,
        -0.894f,  0.000f, -0.447f,
        -0.276f, -0.851f, -0.447f,
        0.724f, -0.526f, -0.447f,
        0.000f,  0.000f, -1.000f};
    unsigned int indexes[] ={
        2, 1, 0,
        3, 2, 0,
        4, 3, 0,
        5, 4, 0,
        1, 5, 0,
        11, 6,  7,
        11, 7,  8,
        11, 8,  9,
        11, 9,  10,
        11, 10, 6,
        1, 2, 6,
        2, 3, 7,
        3, 4, 8,
        4, 5, 9,
        5, 1, 10,
        2,  7, 6,
        3,  8, 7,
        4,  9, 8,
        5, 10, 9,
        1, 6, 10};
    
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );
    
    GLuint vertex_buffer = 0;
    glGenBuffers( 1, &vertex_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW );
    glEnableVertexAttribArray( 0 );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    
    GLuint index_buffer = 0;
    glGenBuffers( 1, &index_buffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW );
    Uniforms.Projection = glGetUniformLocation(program,"Projection");
    Uniforms.View = glGetUniformLocation(program,"View");
    Uniforms.Model = glGetUniformLocation(program, "Model");
    Uniforms.NormalMatrix = glGetUniformLocation(program, "NormalMatrix");
    Uniforms.TessLevelInner = glGetUniformLocation(program, "TessLevelInner");
    Uniforms.TessLevelOuter = glGetUniformLocation(program, "TessLevelOuter");
}

static void initialuniform(){
    bool rotationaxis[]={false,false,false};
    float angles[]={0.0,0.0,0.0};
    mat4 translation = mathhelper.translate(vec3(0.0f, 0.0f, 0.0f));
    mat4 rotation = mathhelper.rotate(angles,rotationaxis);
    modelMatrix = translation*rotation;
    mat3 nm = mat3(glm::transpose(viewMatrix*modelMatrix));
    projMatrix = mathhelper.setPerspective(60.0f, 1.0f, 0.1f, 1000.0f);
    glUniformMatrix3fv(Uniforms.NormalMatrix, 1, GL_FALSE, &nm[0][0]);
    glUniformMatrix4fv(Uniforms.Model, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(Uniforms.View, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(Uniforms.Projection, 1, GL_FALSE, &projMatrix[0][0]);
    glUniform1f(Uniforms.TessLevelInner,4);
    glUniform1f(Uniforms.TessLevelOuter,4);
}

//render function
void render(GLFWwindow* window){
    glClear( GL_COLOR_BUFFER_BIT );
    viewMatrix = mathhelper.setCamera(camPos, lookat, vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(Uniforms.View, 1, GL_FALSE, &viewMatrix[0][0]);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    //glBindVertexArray( vao );
    glDrawElements( GL_PATCHES, 60, GL_UNSIGNED_BYTE, 0 );
    //glBindVertexArray( 0 );
    //glDrawElements(GL_LINE_STRIP, 60, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers( window );
    glfwPollEvents();
}

int main( int argc, char** argv )
{
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    
    GLFWwindow* window = glfwCreateWindow( 800, 800, "Hello window", NULL, NULL );
    if( !window )
    {
        cerr << "Error on window creating" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent( window );
    
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    // Dark blue background
    glEnable(GL_DEPTH_TEST);//enable depth comparisons and update the depth buffer
    glEnable(GL_CULL_FACE);
    //glClearColor(0.7f, 0.6f, 0.5f, 1.0f);
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    
    camPos=vec3(0.0f, 1.0f, 5.0f);
    lookat= vec3(0.0f, 0.0f, 0.0f);
    viewMatrix = mathhelper.setCamera(camPos, lookat, vec3(0.0f, 1.0f, 0.0f));
    
    initData();
    glUseProgram( program );
    BuildBuffers();
    initialuniform();
    glBindVertexArray( vao );
    do{
        render(window);
    }
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );

    glDeleteProgram(program);
    glfwTerminate();
    
    return 0;
}
