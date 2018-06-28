// Defines the entry point for the console application.
//
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <cmath>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"

#include "vec.hpp"
#include "mat.hpp"
#include "transform.hpp"

#include "SOIL.h"

void init();
void display();
void reshape(int, int);
void idle();
void keyboard(unsigned char, int, int);
void special(int, int, int);
bool checkBallCollision(Object, Camera);

void Collocate_Object(Object obj, kmuvcl::math::mat4x4f mat_Model,
                                  kmuvcl::math::mat4x4f mat_Proj,
                                  kmuvcl::math::mat4x4f mat_View);



GLuint program;

GLint  loc_a_vertex;
GLint  loc_a_normal;
GLint  loc_a_texcoord;

GLint  loc_u_texid;

GLint  loc_u_pvm_matrix;
GLint  loc_u_view_matrix;
GLint  loc_u_view_inv_matrix;
GLint  loc_u_model_matrix;
GLint  loc_u_normal_matrix;

GLint  loc_u_light_vector;

GLint  loc_u_light_ambient;
GLint  loc_u_light_diffuse;
GLint  loc_u_light_specular;

GLint  loc_u_material_ambient;
GLint  loc_u_material_diffuse;
GLint  loc_u_material_specular;
GLint  loc_u_material_shininess;

kmuvcl::math::mat4x4f   mat_PVM;

kmuvcl::math::vec4f light_vector      = kmuvcl::math::vec4f(10.0f, 10.0f, 10.0f);
kmuvcl::math::vec4f light_ambient     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_diffuse     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_specular    = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

kmuvcl::math::vec4f material_ambient  = kmuvcl::math::vec4f(0.4f, 0.693f, 0.094f, 1.0f);
kmuvcl::math::vec4f material_diffuse  = kmuvcl::math::vec4f(0.4f, 0.693f, 0.094f, 1.0f);
kmuvcl::math::vec4f material_specular = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

float     material_shininess= 50.0f;


std::string g_filename;
std::string conflicted_object;    //충돌된 객체
bool gamemode = false;
int choice;

Object      g_model, g_tv;
Object      g_acro, g_anky, g_rham, g_trike, g_stego, g_oran, g_dilo;       // object
Object      g_museum, g_ground;
Camera      g_camera;

std::vector <Object> Object_collection;

std::string object_name[8] = {"acro", "anky",
                "rham", "trike", "raptor", "stego", "oran", "dilo"};

float model_angle[6] = { 0.0f, };
float move_x[6] = { 0.0f, };
float move_z[6] = { 0.0f, };
float input_angle[6] = { 0.0f, };

float dx = pow(-1, rand()) * ((float)(rand()%36)/100.0f);
float dz = pow(-1, rand()) * ((float)(rand()%36)/100.0f);
float s = dx;
float t = dz;

std::chrono::time_point<std::chrono::system_clock> prev, curr;

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    g_filename = argv[2];
  }
  else
  {
    g_filename = "./data/cube.obj";
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(1000, 1000);
  glutCreateWindow("Modeling & Navigating Your Studio");



  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutIdleFunc(idle);

  if (glewInit() != GLEW_OK)
  {
      std::cerr << "failed to initialize glew" << std::endl;
      return -1;
  }



  init();


  glutMainLoop();

  return 0;
}

void init()
{
  g_acro.load_simple_obj("./data/acro.obj", "acro");
  g_anky.load_simple_obj("./data/anky.obj", "anky");
  g_rham.load_simple_obj("./data/Rham-Phorynchus.obj", "rham");
  g_trike.load_simple_obj("./data/trike.obj", "trike");
  g_stego.load_simple_obj("./data/stegosaurus.obj", "stego");
  g_oran.load_simple_obj("./data/oran.obj", "oran");
  g_dilo.load_simple_obj("./data/Dilo.obj", "dilo");
  g_museum.load_simple_obj("./data/cube.obj","museum");
  g_ground.load_simple_obj("./data/ground.obj", "ground");


  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  program = Shader::create_program("./shader/phong_vert.glsl", "./shader/phong_frag.glsl");

  loc_u_pvm_matrix         = glGetUniformLocation(program, "u_pvm_matrix");
  loc_u_view_matrix        = glGetUniformLocation(program, "u_view_matrix");
  loc_u_view_inv_matrix    = glGetUniformLocation(program, "u_view_inv_matrix");

  loc_u_model_matrix       = glGetUniformLocation(program, "u_model_matrix");
  loc_u_normal_matrix      = glGetUniformLocation(program, "u_normal_matrix");

  loc_u_light_vector       = glGetUniformLocation(program, "u_light_vector");

  loc_u_light_ambient      = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse      = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular     = glGetUniformLocation(program, "u_light_specular");

  loc_u_material_ambient   = glGetUniformLocation(program, "u_material_ambient");
  loc_u_material_diffuse   = glGetUniformLocation(program, "u_material_diffuse");
  loc_u_material_specular  = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");

  loc_a_vertex             = glGetAttribLocation(program, "a_vertex");
  loc_a_normal             = glGetAttribLocation(program, "a_normal");
  loc_a_texcoord           = glGetAttribLocation(program, "a_texcoord");

  loc_u_texid              = glGetUniformLocation(program, "u_texid");

  prev = curr = std::chrono::system_clock::now();
}

void display()
{

  Object_collection.clear();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  // Camera setting
  kmuvcl::math::mat4x4f   mat_Proj, mat_View, mat_Model;

  // camera intrinsic param
  mat_Proj = kmuvcl::math::perspective(g_camera.fovy(), 1.0f, 0.001f, 10000.0f);

  // camera extrinsic param
  mat_View = kmuvcl::math::lookAt(
		g_camera.position()(0), g_camera.position()(1), g_camera.position()(2),				// eye position
		g_camera.center_position()(0), g_camera.center_position()(1), g_camera.center_position()(2), // center position
		g_camera.up_direction()(0), g_camera.up_direction()(1), g_camera.up_direction()(2)			// up direction
		);
  mat_Model = kmuvcl::math::mat4x4f(1.0f);


  mat_PVM = mat_Proj * mat_View * mat_Model;

  glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, mat_PVM);

  // TODO: properly handle keyboard event

  kmuvcl::math::mat3x3f mat_Normal;

  mat_Normal(0, 0) = mat_Model(0, 0);
  mat_Normal(0, 1) = mat_Model(0, 1);
  mat_Normal(0, 2) = mat_Model(0, 2);
  mat_Normal(1, 0) = mat_Model(1, 0);
  mat_Normal(1, 1) = mat_Model(1, 1);
  mat_Normal(1, 2) = mat_Model(1, 2);
  mat_Normal(2, 0) = mat_Model(2, 0);
  mat_Normal(2, 1) = mat_Model(2, 1);
  mat_Normal(2, 2) = mat_Model(2, 2);

  kmuvcl::math::mat4x4f mat_View_inv = kmuvcl::math::inverse(mat_View);

	glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, mat_PVM);
  glUniformMatrix4fv(loc_u_model_matrix, 1, false, mat_Model);
  glUniformMatrix4fv(loc_u_view_matrix, 1, false, mat_View);
  glUniformMatrix4fv(loc_u_view_inv_matrix, 1, false, mat_View_inv);
  glUniformMatrix3fv(loc_u_normal_matrix, 1, false, mat_Normal);

  glUniform3fv(loc_u_light_vector, 1, light_vector);
  glUniform4fv(loc_u_light_ambient, 1, light_ambient);
  glUniform4fv(loc_u_light_diffuse, 1, light_diffuse);
  glUniform4fv(loc_u_light_specular, 1, light_specular);

  glUniform4fv(loc_u_material_ambient, 1, material_ambient);
  glUniform4fv(loc_u_material_diffuse, 1, material_diffuse);
  glUniform4fv(loc_u_material_specular, 1, material_specular);
  glUniform1f(loc_u_material_shininess, material_shininess);

  Shader::check_gl_error("glUniform4fv");

//Museum
  mat_Model = kmuvcl::math::scale(1.0f, 1.0f, 1.0f);
  mat_Model = kmuvcl::math::rotate(0.0f, 1.0f, 0.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(0.0f, 0.0f, -50.0f) * mat_Model;

  Collocate_Object(g_museum, mat_Model, mat_Proj, mat_View);

  mat_Model = kmuvcl::math::scale(1.0f, 1.0f, 1.0f);
  mat_Model = kmuvcl::math::rotate(-90.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(50.0f, 0.0f, 0.0f) * mat_Model;
  Collocate_Object(g_museum, mat_Model, mat_Proj, mat_View);

  mat_Model = kmuvcl::math::scale(1.0f, 1.0f, 1.0f);
  mat_Model = kmuvcl::math::rotate(90.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(-50.0f, 0.0f, 0.0f) * mat_Model;
  Collocate_Object(g_museum, mat_Model, mat_Proj, mat_View);

  mat_Model = kmuvcl::math::scale(1.0f, 1.0f, 1.0f);
  mat_Model = kmuvcl::math::rotate(180.0f, 1.0f, 0.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(0.0f, 0.0f, 0.0f) * mat_Model;
  Collocate_Object(g_ground, mat_Model, mat_Proj, mat_View);


//Acrocanthosaurus
  mat_Model = kmuvcl::math::scale(3.0f, 3.0f, 3.0f);
  mat_Model = kmuvcl::math::rotate(270.0f, 1.0f, 0.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(input_angle[0],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(model_angle[0],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(0.0f + move_x[0], 0.0f, -38.0f + move_z[0]) * mat_Model;

  Collocate_Object(g_acro, mat_Model, mat_Proj, mat_View);

//Ankylosaurus

  mat_Model = kmuvcl::math::scale(4.0f, 4.0f, 4.0f);
  mat_Model = kmuvcl::math::rotate(50.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(input_angle[1],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(model_angle[1], 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(-27.0f + move_x[1], 0.0f, -20.0f + move_z[1]) * mat_Model;

  Collocate_Object(g_anky, mat_Model, mat_Proj, mat_View);


//Rham-Phorynchis
  mat_Model = kmuvcl::math::scale(0.01f, 0.01f, 0.01f);
  mat_Model = kmuvcl::math::rotate((float)atan(t/s)*180.0f/3.14f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(dx, 50.0f, dz) * mat_Model;

  Collocate_Object(g_rham, mat_Model, mat_Proj, mat_View);


//Trike

  mat_Model = kmuvcl::math::scale(4.0f, 4.0f, 4.0f);
  mat_Model = kmuvcl::math::rotate(-90.0f, 1.0f, 0.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(-30.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(input_angle[2],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(model_angle[2],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(30.0f + move_x[2], 0.0f, -30.0f + move_z[2]) * mat_Model;

  Collocate_Object(g_trike, mat_Model, mat_Proj, mat_View);


//Stego

  mat_Model = kmuvcl::math::scale(1.0f, 1.0f, 1.0f);
  mat_Model = kmuvcl::math::rotate(270.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(input_angle[3],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(model_angle[3],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(30.0f + move_x[3], 0.0f, 20.0f + move_z[3]) * mat_Model;

  Collocate_Object(g_stego, mat_Model, mat_Proj, mat_View);

//Oran

  mat_Model = kmuvcl::math::scale(3.0f, 3.0f, 3.0f);
  mat_Model = kmuvcl::math::rotate(-90.0f, 0.0f, 0.0f, 1.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(-90.0f, 1.0f, 0.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(-180.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(input_angle[4],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(model_angle[4],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(-35.0f + move_x[4], 0.0f, 14.0f + move_z[4]) * mat_Model;

  Collocate_Object(g_oran, mat_Model, mat_Proj, mat_View);

//Dilo

  mat_Model = kmuvcl::math::scale(0.6f, 0.6f, 0.6f);
  mat_Model = kmuvcl::math::rotate(90.0f, 0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(input_angle[5],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::rotate(model_angle[5],        0.0f, 1.0f, 0.0f) * mat_Model;
  mat_Model = kmuvcl::math::translate(-35.0f + move_x[5], 0.0f, 42.0f + move_z[5]) * mat_Model;

  Collocate_Object(g_dilo, mat_Model, mat_Proj, mat_View);

    glUseProgram(0);

    glutSwapBuffers();
  }


void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

}

void keyboard(unsigned char key, int x, int y)
{
  switch(key){
    case 'a': case 'A':
      if(gamemode){
        input_angle[choice-1] -= 1.0f;
      }
      else{
        g_camera.rotate_left(0.1f);
      }
      break;
    case 'd': case 'D':
      if(gamemode){
        input_angle[choice-1] += 1.0f;
      }
      else{
        g_camera.rotate_right(0.1f);
      }
      break;
    case 'g': case 'G':
      if(gamemode == false){
        gamemode = true;
      }
      else gamemode = false;
      break;
    case '1' :
      choice = 1;
      break;
    case '2' :
      choice = 2;
      break;
    case '3' :
      choice = 3;
      break;
    case '4' :
      choice = 4;
      break;
    case '5' :
      choice = 5;
      break;
    case '6' :
      choice = 6;
      break;
  }

	glutPostRedisplay();
}

void special(int key, int x, int y)
{
  switch(key){
    case GLUT_KEY_UP:
        if(gamemode){
          move_z[choice-1] += 1.0f;
        }
        else{
          g_camera.move_forward(2.0f);
          for(int i = 0; i < Object_collection.size(); i++){
            if(checkBallCollision(Object_collection.at(i), g_camera)){
              g_camera.move_backward(2.0f);
              break;

            }
          }
        }

      break;
    case GLUT_KEY_DOWN:
      if(gamemode){
        move_z[choice-1] -= 1.0f;
      }
      else{
        g_camera.move_backward(2.0f);
        for(int i = 0; i < Object_collection.size(); i++){
          if(checkBallCollision(Object_collection.at(i), g_camera)){
            g_camera.move_forward(2.0f);
            break;
          }
        }
      }

      break;
    case GLUT_KEY_LEFT:
      if(gamemode){
        move_x[choice-1] -= 1.0f;
      }
      else{
        g_camera.move_left(2.0f);
        for(int i = 0; i < Object_collection.size(); i++){
          if(checkBallCollision(Object_collection.at(i), g_camera)){
            g_camera.move_right(2.0f);
            break;

          }
        }
      }
      break;
    case GLUT_KEY_RIGHT:
      if(gamemode){
        move_x[choice-1] += 1.0f;
      }
      else{
        g_camera.move_right(2.0f);
        for(int i = 0; i < Object_collection.size(); i++){
          if(checkBallCollision(Object_collection.at(i), g_camera)){
            g_camera.move_left(2.0f);
            break;
          }
        }
      }

      break;
  }

	glutPostRedisplay();
}

void idle()
{

  if(dx >= 50.0f)
  {
    s = -1 * ((float)(rand()%50));
    t = pow(-1, rand()) * ((float)(rand()%50));
  }
  if(dx <= -50.0f)
  {
    s = ((float)(rand()%50));
    t = pow(-1, rand()) * ((float)(rand()%50));
  }
  if(dz <= 0.0f)
  {
    s = pow(-1, rand()) * ((float)(rand()%50));
    t = ((float)(rand()%50));
  }
  if(dz >= 50.0f)
  {
    s = pow(-1, rand()) * ((float)(rand()%50));
    t = -1 * ((float)(rand()%50));
  }
  dx += s/100.0f;
  dz += t/100.0f;

  if(conflicted_object == "acro" || model_angle[0] != 1.0f){
    model_angle[0] += 1.0f;
    if(model_angle[0] == 360.0f){
      model_angle[0] = 0.0f;
      conflicted_object = "";
    }
  }
  if(conflicted_object == "anky" || model_angle[1] != 1.0f){
    model_angle[1] += 1.0f;
    if(model_angle[1] == 360.0f){
      model_angle[1] = 0.0f;
      conflicted_object = "";
    }
  }
  if(conflicted_object == "trike" || model_angle[2] != 1.0f){
    model_angle[2] += 1.0f;
    if(model_angle[2] == 360.0f){
      model_angle[2] = 0.0f;
      conflicted_object = "";
    }
  }
  if(conflicted_object == "stego" || model_angle[3] != 1.0f){
    model_angle[3] += 1.0f;
    if(model_angle[3] == 360.0f){
      model_angle[3] = 0.0f;
      conflicted_object = "";
    }
  }
  if(conflicted_object == "oran" || model_angle[4] != 1.0f){
    model_angle[4] += 1.0f;
    if(model_angle[4] == 360.0f){
      model_angle[4] = 0.0f;
      conflicted_object = "";
    }
  }
  if(conflicted_object == "dilo" || model_angle[5] != 1.0f){
    model_angle[5] += 1.0f;
    if(model_angle[5] == 360.0f){
      model_angle[5] = 0.0f;
      conflicted_object = "";
    }
  }


  glutPostRedisplay();
}

bool checkBallCollision(Object obj, Camera c) {

    if(obj.get_objName() == "museum") return false;
    if(obj.get_objName() == "ground") return false;

    kmuvcl::math::vec3f obj_position, c_position;


    obj_position = obj.get_position();
    c_position = c.position();

    float deltaX = obj_position(0) - c_position(0);
    float deltaY = obj_position(1) - c_position(1);
    float deltaZ = obj_position(2) - c_position(2);


    float length = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

    std::cout << length << std::endl;

    std::cout << obj.get_max_length() << std::endl;

    if(length > obj.get_max_length() / 2.0f + 20.0f)
        return false;
    else
        conflicted_object = obj.get_objName();
        std::cout << conflicted_object << std::endl;
        return true;

}

void Collocate_Object(Object obj, kmuvcl::math::mat4x4f mat_Model,
                                  kmuvcl::math::mat4x4f mat_Proj,
                                  kmuvcl::math::mat4x4f mat_View){

  mat_PVM = mat_Proj * mat_View * mat_Model;

  glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, mat_PVM);

  kmuvcl::math::vec4f checkBall_position4f;
  kmuvcl::math::vec3f object_centerpoint;

  object_centerpoint = obj.get_center_position();

  checkBall_position4f = kmuvcl::math::vec4f(object_centerpoint(0),
                          object_centerpoint(1),
                          object_centerpoint(2), 1.0f);

  checkBall_position4f = mat_Model * checkBall_position4f;

  obj.set_value(kmuvcl::math::vec3f(checkBall_position4f(0),
                    checkBall_position4f(1),
                    checkBall_position4f(2)));

  glActiveTexture(GL_TEXTURE0);

  glBindTexture(GL_TEXTURE_2D, obj.textureid);
  glUniform1i(loc_u_texid, 0);

  obj.draw(loc_a_vertex, loc_a_normal, loc_a_texcoord,
    loc_u_material_ambient, loc_u_material_diffuse,
    loc_u_material_specular, loc_u_material_shininess);
  Shader::check_gl_error("draw");

  obj.set_minmax();

  Object_collection.push_back(obj);

}
