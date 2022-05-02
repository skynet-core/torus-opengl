#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <memory>
#include <numbers>
#include <utility>

#include "ShaderProgram.h"
#include "Torus.h"

template <typename T> using UPtr = std::unique_ptr<T>;

int display_w = 1280, display_h = 720;
bool show_stats_window = true;
bool show_props_window = true;
float rotateSpeed = 2 * std::numbers::pi / 60;
float yRotate = 0.0f;
double lastFrameTime = 0.0;
double deltaFrameTime = 0.0;

const char *items[] = { "FILL", "LINE" };
static int currentPolygonMode = 0;
bool faceCull = false;
bool drawNormals = false;

unsigned int uboMatrices, uboDirLights, VAO, VBO, EBO;
UPtr<ShaderProgram> torusProgram;
UPtr<ShaderProgram> torusNormalsProgram;
UPtr<Torus> torusObject;

ImVec2 top_left = ImVec2 (0, 0);
ImVec2 top_left_pivot = ImVec2 (0.0f, 0.0f);
ImVec2 top_right_pivot = ImVec2 (1.0f, 0.0f);

glm::vec4 clear_color = glm::vec4 (0.0f, 0.0f, 0.0f, 1.0f);

float shininess = 32;
float metalness = 0.0f;

glm::vec4 torus_ambient_color = glm::vec4 (1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 torus_diffuse_color = glm::vec4 (1.0f, 0.0f, 0.0f, 1.0f);
glm::vec4 torus_specular_color = glm::vec4 (1.0f, 0.0f, 0.0f, 1.0f);

glm::vec4 dir_light_ambient_color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 dir_light_diffuse_color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);
glm::vec4 dir_light_specular_color = glm::vec4 (1.0f, 1.0f, 1.0f, 1.0f);

glm::vec4 normals_color = glm::vec4 (0.0f, 1.0f, 0.0f, 1.0f);
glm::vec3 rotate_axis = glm::vec3 (0.0f, 1.0f, 0.0f);

static void
glfw_error_callback (int error, const char *description)
{
  std::cerr << fmt::format ("Glfw Error {}: {}\n", error, description)
            << std::endl;
}

static void
key_callback (GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
    {
      switch (key)
        {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose (window, GLFW_TRUE);
          break;
        case GLFW_KEY_S:
          show_stats_window = ((mods & GLFW_MOD_CONTROL) != 0);
        case GLFW_KEY_P:
          show_props_window = ((mods & GLFW_MOD_CONTROL) != 0);
        default:
          break;
        }
    }
}

static void
cursor_pos_callback (GLFWwindow *window, double xpos, double ypos)
{
}

static void
setup (GLFWwindow *window, const char *glsl_version)
{
  torusProgram
      = std::make_unique<ShaderProgram> ("./shaders/torus_vertex_base.glsl",
                                         "./shaders/torus_fragment_base.glsl");
  torusNormalsProgram = std::make_unique<ShaderProgram> (
      "./shaders/torus_vertex.glsl", "./shaders/torus_fragment.glsl",
      "./shaders/torus_geometry.glsl");

  float xscale, yscale;
  glfwGetWindowContentScale (window, &xscale, &yscale);
  glViewport (0, 0, display_w, display_h);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable (GL_BLEND);
  glEnable (GL_MULTISAMPLE);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION ();
  ImGui::CreateContext ();
  ImGuiIO &io = ImGui::GetIO ();
  io.FontGlobalScale = std::max (xscale, yscale);
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls
  // Setup Dear ImGui style
  ImGui::StyleColorsDark ();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL (window, true);
  ImGui_ImplOpenGL3_Init (glsl_version);

  unsigned int torusProgramId = torusProgram->get_id ();
  unsigned int torusNormalsProgramId = torusNormalsProgram->get_id ();
  // create torus model object ...
  torusObject = std::make_unique<Torus> (0.5f, 0.8f, 12);
  glGenVertexArrays (1, &VAO);
  glGenBuffers (1, &VBO);
  glGenBuffers (1, &EBO);
  glGenBuffers (1, &uboMatrices);
  glGenBuffers (1, &uboDirLights);

  glBindVertexArray (VAO);

  glBindBuffer (GL_ARRAY_BUFFER, VBO); // bind ...
  glBufferData (GL_ARRAY_BUFFER,
                (GLsizeiptr)torusObject->get_vertices ().size ()
                    * (GLsizeiptr)torusObject->get_normals ().size () * 3
                    * sizeof (float),
                nullptr, GL_STATIC_DRAW);
  glBufferSubData (GL_ARRAY_BUFFER, 0,
                   (GLsizeiptr)torusObject->get_vertices ().size () * 3
                       * sizeof (float),
                   &torusObject->get_vertices ().data ()[0]);
  glBufferSubData (
      GL_ARRAY_BUFFER,
      (GLsizeiptr)torusObject->get_vertices ().size () * 3 * sizeof (float),
      (GLsizeiptr)torusObject->get_normals ().size () * 3 * sizeof (float),
      &torusObject->get_normals ().data ()[0]);

  glEnableVertexAttribArray (0);
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
                         (void *)nullptr);

  glEnableVertexAttribArray (1);
  glVertexAttribPointer (
      1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (float),
      (void *)(torusObject->get_vertices ().size () * 3 * sizeof (float)));

  glBindBuffer (GL_ARRAY_BUFFER, 0); // unbind ...

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO); // bind ...
  glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                (GLsizeiptr)torusObject->get_indices ().size ()
                    * sizeof (unsigned int),
                torusObject->get_indices ().data (), GL_STATIC_DRAW);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0); // unbind ...

  unsigned int uboMatricesIndex
      = glGetUniformBlockIndex (torusProgramId, "Matrices");
  glUniformBlockBinding (torusProgramId, uboMatricesIndex, 0);

  unsigned int uboMatricesIndexNormals
      = glGetUniformBlockIndex (torusNormalsProgramId, "Matrices");
  glUniformBlockBinding (torusNormalsProgramId, uboMatricesIndexNormals, 0);

  unsigned int uboDirLightsIndex
      = glGetUniformBlockIndex (torusProgramId, "DirectionalLights");
  glUniformBlockBinding (torusProgramId, uboDirLightsIndex, 1);

  unsigned int uboDirLightsIndexNormals
      = glGetUniformBlockIndex (torusNormalsProgramId, "DirectionalLights");
  glUniformBlockBinding (torusNormalsProgramId, uboDirLightsIndexNormals, 1);

  glBindBuffer (GL_UNIFORM_BUFFER, uboMatrices);
  glBufferData (GL_UNIFORM_BUFFER, 4 * sizeof (glm::mat4), nullptr,
                GL_STATIC_DRAW);
  glBindBuffer (GL_UNIFORM_BUFFER, 0); // unbind

  glBindBuffer (GL_UNIFORM_BUFFER, uboDirLights);
  glBufferData (GL_UNIFORM_BUFFER, 4 * sizeof (glm::vec4), nullptr,
                GL_STATIC_DRAW);
  glBindBuffer (GL_UNIFORM_BUFFER, 0); // unbind

  glBindBufferRange (GL_UNIFORM_BUFFER, 0, uboMatrices, 0,
                     4 * sizeof (glm::mat4));

  glBindBufferRange (GL_UNIFORM_BUFFER, 1, uboDirLights, 0,
                     4 * sizeof (glm::vec4));
}

glm::mat4 projection, view, normal;
glm::mat4 model = glm::identity<glm::mat4> ();

static void render (GLFWwindow *window, double time)
{
  deltaFrameTime = time - lastFrameTime;
  lastFrameTime = time;
  glfwGetFramebufferSize (window, &display_w, &display_h);
  glViewport (0, 0, display_w, display_h);
  glClearColor (clear_color.r, clear_color.g, clear_color.b, clear_color.a);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame ();
  ImGui_ImplGlfw_NewFrame ();
  ImGui::NewFrame ();

  if (show_stats_window)
    {
      ImGui::SetNextWindowPos (top_left, ImGuiCond_Once, top_left_pivot);
      ImGui::Begin ("Stats", &show_stats_window);
      ImGui::ColorEdit4 ("clear color", (float *)glm::value_ptr (clear_color));
      ImGui::Text ("Application average %.3f ms/frame (%.1f FPS)",
                   1000.0f / ImGui::GetIO ().Framerate,
                   ImGui::GetIO ().Framerate);
      ImGui::End ();
    }

  if (show_props_window)
    {
      ImGui::SetNextWindowPos (ImVec2 (display_w, 0), ImGuiCond_Once,
                               top_right_pivot);
      ImGui::Begin ("Props", &show_props_window);
      ImGui::ColorEdit4 ("Ambient Color",
                         (float *)glm::value_ptr (torus_ambient_color));
      ImGui::ColorEdit4 ("Diffuse Color",
                         (float *)glm::value_ptr (torus_diffuse_color));
      ImGui::ColorEdit4 ("Specular Color",
                         (float *)glm::value_ptr (torus_specular_color));
      ImGui::ColorEdit4 ("DirL Ambient Color",
                         (float *)glm::value_ptr (dir_light_ambient_color));
      ImGui::ColorEdit4 ("DirL Diffuse Color",
                         (float *)glm::value_ptr (dir_light_diffuse_color));
      ImGui::ColorEdit4 ("DirL Specular Color",
                         (float *)glm::value_ptr (dir_light_specular_color));
      ImGui::Checkbox ("Show Normals", &drawNormals);
      ImGui::ColorEdit4 ("Normals Color",
                         (float *)glm::value_ptr (normals_color));
      ImGui::SliderFloat ("rotate x", &rotate_axis.x, 0.0f, 1.0f);
      ImGui::SliderFloat ("rotate y", &rotate_axis.y, 0.0f, 1.0f);
      ImGui::SliderFloat ("rotate z", &rotate_axis.z, 0.0f, 1.0f);
      if (ImGui::BeginCombo ("Polygon Mode", items[currentPolygonMode],
                             ImGuiComboFlags_None))
        {
          for (int n = 0; n < IM_ARRAYSIZE (items); n++)
            {
              const bool is_selected = (currentPolygonMode == n);
              if (ImGui::Selectable (items[n], is_selected))
                {
                  currentPolygonMode = n;
                }
              if (is_selected)
                {
                  ImGui::SetItemDefaultFocus ();
                }
            }
          ImGui::EndCombo ();
        }
      ImGui::Checkbox ("Face Culling", &faceCull);

      ImGui::End ();
    }

  // Rendering
  ImGui::Render ();

  // MATRICES START
  glBindBuffer (GL_UNIFORM_BUFFER, uboMatrices);
  // projection matrix ...
  projection
      = glm::perspective (glm::radians (45.0f),
                          (float)display_w / (float)display_h, 1.0f, 1000.0f);
  glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (glm::mat4),
                   glm::value_ptr (projection));
  // camera matrix ...
  view = glm::lookAt (glm::vec3 (0.0f, 0.0f, -3.0f),
                      glm::vec3 (0.0f, 0.0f, 0.0f),
                      glm::vec3 (0.0f, 1.0f, 0.0f));
  glBufferSubData (GL_UNIFORM_BUFFER, sizeof (glm::mat4), sizeof (glm::mat4),
                   glm::value_ptr (view));

  // model - transformation matrix ...
  if (glm::length (rotate_axis) != 0.0f)
    {
      yRotate += rotateSpeed * deltaFrameTime;
      model = glm::identity<glm::mat4> ();
      model = glm::rotate (model, (float)(yRotate), rotate_axis);
    }
  glBufferSubData (GL_UNIFORM_BUFFER, 2 * sizeof (glm::mat4),
                   sizeof (glm::mat4), glm::value_ptr (model));
  // normal matrix ...
  normal = glm::transpose (glm::inverse (view * model));
  glBufferSubData (GL_UNIFORM_BUFFER, 3 * sizeof (glm::mat4),
                   sizeof (glm::mat4), glm::value_ptr (normal));
  // unbind ubo ...
  glBindBuffer (GL_UNIFORM_BUFFER, 0);
  // MATRICES END

  // DIR LIGHT START
  glBindBuffer (GL_UNIFORM_BUFFER, uboDirLights);
  glBufferSubData (GL_UNIFORM_BUFFER, 0, sizeof (glm::vec4),
                   glm::value_ptr (glm::vec4 (-2.0f, 2.0f, 2.0f, 0.0f)));
  glBufferSubData (GL_UNIFORM_BUFFER, 1 * sizeof (glm::vec4),
                   sizeof (glm::vec4),
                   glm::value_ptr (dir_light_ambient_color));
  glBufferSubData (GL_UNIFORM_BUFFER, 2 * sizeof (glm::vec4),
                   sizeof (glm::vec4),
                   glm::value_ptr (dir_light_diffuse_color));
  glBufferSubData (GL_UNIFORM_BUFFER, 3 * sizeof (glm::vec4),
                   sizeof (glm::vec4),
                   glm::value_ptr (dir_light_specular_color));
  glBindBuffer (GL_UNIFORM_BUFFER, 0);
  // DIR LIGHT END

  if (faceCull)
    {
      glEnable (GL_CULL_FACE);
    }
  else
    {
      glDisable (GL_CULL_FACE);
    }
  glPolygonMode (GL_FRONT_AND_BACK,
                 currentPolygonMode == 0 ? GL_FILL : GL_LINE);
  glBindVertexArray (VAO);
  glBindBuffer (GL_ARRAY_BUFFER, VBO);
  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, EBO);

  auto indices = torusObject->get_indices ();
  torusProgram->activate ();
  torusProgram->set<glm::vec4> ("material.ambient", torus_ambient_color);
  torusProgram->set<glm::vec4> ("material.diffuse", torus_diffuse_color);
  torusProgram->set<glm::vec4> ("material.specular", torus_specular_color);
  torusProgram->set<float> ("material.shininess", shininess);

  glDrawElements (GL_TRIANGLE_STRIP, indices.size (), GL_UNSIGNED_INT,
                  nullptr);

  if (drawNormals)
    {

      torusNormalsProgram->activate ();
      torusNormalsProgram->set<glm::vec4> ("material.ambient", normals_color);
      glDrawElements (GL_TRIANGLE_STRIP, indices.size (), GL_UNSIGNED_INT,
                      nullptr);
    }

  // render 3D objects ...
  ImGui_ImplOpenGL3_RenderDrawData (ImGui::GetDrawData ());
}

int
main (int, char **)
{
  glfwSetErrorCallback (glfw_error_callback);
  if (!glfwInit ())
    {
      std::cerr << "GLFW: init failed" << std::endl;
      return 1;
    }

  const char *glsl_version = "#version 440";
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint (GLFW_SAMPLES, 4);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  GLFWmonitor *primary = glfwGetPrimaryMonitor ();
  GLFWwindow *window
      = glfwCreateWindow (display_w, display_h,
                          "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
  if (window == nullptr)
    {
      std::cerr << "GLFW: failed to create Window" << std::endl;
      return 1;
    }
  glfwMakeContextCurrent (window);
  glfwSwapInterval (1);

  if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress))
    {
      std::cerr << "GLAD: failed to load opengl context" << std::endl;
      return -1;
    }

  glfwSetKeyCallback (window, key_callback);
  glfwSetCursorPosCallback (window, cursor_pos_callback);

  setup (window, glsl_version);
  // Main loop
  while (!glfwWindowShouldClose (window))
    {
      render (window, glfwGetTime ());
      glfwSwapBuffers (window);
      glfwPollEvents ();
    }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown ();
  ImGui_ImplGlfw_Shutdown ();
  ImGui::DestroyContext ();

  glfwDestroyWindow (window);
  glfwTerminate ();

  return 0;
}