#include <iostream>
#include <fstream>
#include <iomanip> // For cout::setprecision
#include <codecvt>
#include <unordered_map>
#include <string>
#include <chrono>
#include <math.h>
#include <string.h>

#include <thread>
#include <queue>
#include <mutex>
#include <atomic>

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "IOFile.h"
#include "Matrices.h"
#include "Machine.h"
#include "Message.h"
#include "Obj3DLoader.h"
#include "utf8.h"

#define WIDTH 640
#define HEIGHT 480

std::thread gamethread;
// https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue
std::queue<Message*> m_queue;
std::mutex m_mutex;
//std::condition_variable m_cv;
std::atomic<bool> m_timerExit; // May be better than "running"


// https://www.turbonut.com/2019/03/18/opengl3-sdl2/
// http://lazyfoo.net/SDL_tutorials/lesson33/index.php


double now() {
  auto time = std::chrono::system_clock::now().time_since_epoch();
  std::chrono::seconds seconds = std::chrono::duration_cast< std::chrono::seconds >(time);
  std::chrono::milliseconds ms = std::chrono::duration_cast< std::chrono::milliseconds >(time);
  return (double) seconds.count() + ((double) (ms.count() % 1000)/1000.0);
}



// check OpenGL errors

void check_gl(std::string when)
{
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error " << when << ": " << gluErrorString(err) << "(" << err << ")" << std::endl;
    //running = false;
  }
}


GLuint load_texture()
{
  GLuint TextureID = 0;

  // You should probably use CSurface::OnLoad ... ;)
  //-- and make sure the Surface pointer is good!
  //std::cout<<"load_texture() load png into surface"<<std::endl;
  SDL_Surface* Surface = IMG_Load("textures/test256.png");
  if(Surface) {
    //std::cout<<"load_texture() OK surface=" << Surface << " h=" << Surface->h << " w=" << Surface->w << std::endl;
  } else {
    std::cout<<"load_texture() FAILED "<<IMG_GetError()<<std::endl;
    // handle error
    //running = false;
    return 0;
  }

  glGenTextures(1, &TextureID);
  glBindTexture(GL_TEXTURE_2D, TextureID);
  check_gl("preparing texture");

  int Mode = GL_RGB;

  if(Surface->format->BytesPerPixel == 4) {
      Mode = GL_RGBA;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  check_gl("setting texture parameters");

  // Any other glTex* stuff here
  // ...

  return TextureID;
}


GLuint compile_shader(const char* vert_shader, const char* frag_shader)
{
  // Load GLSL vertex+fragment shaders
//  IOFile vert_glsl = IOFile(vert_shader);
//  IOFile frag_glsl = IOFile(frag_shader);
  IOFile file;
  std::string vert_glsl = file.slurp(vert_shader);
  std::string frag_glsl = file.slurp(frag_shader);


  // Compile shaders
  GLint success;
  int maxLength;
  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  const char *c_vert_glsl = vert_glsl.c_str();
  glShaderSource(vertexShaderID, 1, &c_vert_glsl, nullptr);
  glCompileShader(vertexShaderID);
  glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(vertexShaderID, sizeof(InfoLog), NULL, InfoLog);
    std::cerr << vert_glsl << std::endl;
    std::cerr << "Error compiling vertex shader: " << InfoLog << std::endl;
  }
  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  const char *c_frag_glsl = frag_glsl.c_str();
  glShaderSource(fragmentShaderID, 1, &c_frag_glsl, nullptr);
  glCompileShader(fragmentShaderID);
  glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(fragmentShaderID, sizeof(InfoLog), NULL, InfoLog);
    std::cerr << frag_glsl << std::endl;
    std::cerr << "Error compiling fragment shader: " << InfoLog << std::endl;
  }
  check_gl("compiling shaders");

  // Link compiled shaders into a shaderprogram
  GLuint shaderProgramID = glCreateProgram();
  glAttachShader(shaderProgramID, vertexShaderID);
  glAttachShader(shaderProgramID, fragmentShaderID);
  glLinkProgram(shaderProgramID);
  glGetProgramiv(shaderProgramID, GL_LINK_STATUS, (int *)&success);
  if(!success)
  {
    /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
    glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &maxLength);

    /* The maxLength includes the NULL character */
    GLchar* InfoLog = (char *)malloc(maxLength);

    /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
    glGetProgramInfoLog(shaderProgramID, maxLength, &maxLength, InfoLog);

    std::cerr << "Error linking shader program: " << InfoLog << std::endl;
    free(InfoLog);
  }

  check_gl("linking shaders");

  // Dispose of the shader objects (they have been compiled into shaderProgram now)
  glDetachShader(shaderProgramID, vertexShaderID);
  glDetachShader(shaderProgramID, fragmentShaderID);
  glDeleteShader(vertexShaderID);
  glDeleteShader(fragmentShaderID);
  check_gl("deleting shaders");

  return shaderProgramID;
}



// TEMP model: just two triangles forming a rectangular plane
GLuint opengl_prepare_vbo()
{
  GLfloat rect[] =
  {
    1.0f, 0.7f, 0.0f,  1.0f,  1.0f,
    1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    0.0f, 0.7f, 0.0f,  0.0f,  1.0f,
    0.0f, 0.0f, 0.0f,  0.0f,  0.0f,
    0.0f, 0.7f, 0.0f,  0.0f,  1.0f,
    1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
  };
  //   x      y     z      u      v

  GLuint vertexBufferID;
  glGenBuffers(1, &vertexBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
  check_gl("preparing bufferdata");

  return vertexBufferID;
}




void prepare_scene()
{
  //std::cout << "render() shader=" << shaderProgramID << " texture=" << TextureID << std::endl;
  glClearColor(0.25f, 0.25f, 0.30f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  check_gl("scene settings");
}



/*
  ===================================================
  RENDER 3D SCENE
  ===================================================
*/
//void render_scene(Matrix4 m_scene, Matrix4 m_model, GLuint vertexBufferID, GLuint shaderProgramID, GLuint TextureID, float pan_x, float pan_y)
void render_scene(Matrix4 m_scene, Matrix4 m_model, GLuint vertexBufferID, GLuint shaderProgramID, GLuint TextureID, Obj3D* obj)
{
  //std::cout << ">> set shader" << std::endl;
  glUseProgram(shaderProgramID);

  // Set active texture
  glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
  glBindTexture(GL_TEXTURE_2D, TextureID);
  check_gl("binding texture");

  GLuint uniform_scene = glGetUniformLocation(shaderProgramID, "scene");
  GLuint uniform_model = glGetUniformLocation(shaderProgramID, "model");
  glUniformMatrix4fv(uniform_scene, 1, GL_FALSE, m_scene.get());
  glUniformMatrix4fv(uniform_model, 1, GL_FALSE, m_model.get());
  check_gl("updating uniforms");

  // Draw experimental Obj3D
  m_model *= Matrix4().rotate(300, Vector3(0.98, 0.01, 0.0));
  m_model *= Matrix4().translate(0.5, 0.0, 0.0);
  m_model *= Matrix4().scale(2.5);
  glUniformMatrix4fv(uniform_model, 1, GL_FALSE, m_model.get());
  //for (int i=0; i<obj->subobjects; i++) {
  //  obj->render(i);
  //}
  obj->render(0);
  check_gl("obj3d render");

  glBindTexture(GL_TEXTURE_2D, 0);
  glDisableVertexAttribArray(0);
}




int GameThread(void* ptr)
{
  bool running = true;

  SDL_Window* window = (SDL_Window*) ptr;
  SDL_GLContext context;
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  float pan_x = 0.0;
  float pan_y = 0.0;
  float fov = 60.0f;
  int width = 0;
  int height = 0;
  SDL_GetWindowSize(window, &width, &height);
  bool perspective_changed = true; // true = need to initialize projection matrix

  // Create OpenGL context here
  std::cout<<"GameThread() creating OpenGL context"<<std::endl;
  context = SDL_GL_CreateContext(window);
  if(context == nullptr)
  {
    std::string msg = "OpenGL context could not be created: ";
    msg.append(SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
    running = false;
    return 0;
  }
  else
  {
    check_gl("creating context");
    glewInit();
    check_gl("initializing glew");
  }

  TTF_Font* font = TTF_OpenFont("fonts/unscii-8-thin.ttf", 16);
  if(!font) {
    std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
    running = false;
  }

  // Preload charmap cache
  //for (int i=32; i<128; i++) { glyph(font, i); }

  //std::cout << "Load new font object" << std::endl;
  Fontcache fontcache = Fontcache(font);

  GLuint VertexArrayObjectID;
  glGenVertexArrays(1, &VertexArrayObjectID);
  glBindVertexArray(VertexArrayObjectID);

  //std::cout << "Load shaders" << std::endl;
  GLuint scene_shader = compile_shader("glsl/scene_vert.glsl", "glsl/scene_frag.glsl");
  GLuint ortho_shader = compile_shader("glsl/ortho_vert.glsl", "glsl/ortho_frag.glsl");
  GLuint vertexBufferID = opengl_prepare_vbo();

  Obj3DLoader* obj_loader = new Obj3DLoader();
  Obj3D* test_object = obj_loader->load("C:\\Users\\floyd\\Documents\\Cpp\\Deep\\obj\\screen.obj");
//  Obj3D* test_object = obj_loader->load("C:\\Users\\floyd\\Documents\\Cpp\\Deep\\obj\\cube.obj");
  delete obj_loader;

  test_object->set_shader_v(glGetAttribLocation(scene_shader, "vertex"));
  test_object->set_shader_vt(glGetAttribLocation(scene_shader, "uv"));
  test_object->set_shader_vn(glGetAttribLocation(scene_shader, "normal"));

  std::cout << "Initialize VM A" << std::endl;
  Machine vm_A = Machine(ortho_shader, font);
  Machine* focused = &vm_A;

  std::cout << "Initialize VM B" << std::endl;
  Machine vm_B = Machine(ortho_shader, font);

  std::cout << "Initialize VM C" << std::endl;
  Machine vm_C = Machine(ortho_shader, font);

  std::cout << "Initialize VM D" << std::endl;
  Machine vm_D = Machine(ortho_shader, font);


  Matrix4 m_scene = Matrix4();
  Matrix4 m_model = Matrix4();

  try {
    std::cout<<"GameThread() entering its main loop"<<std::endl;
    while (running)
    {

      // Process any messages from main thread
      Message* msg = nullptr;
      std::unique_lock<std::mutex> lk(m_mutex);
      while (m_queue.empty()==false) {
        msg = m_queue.front();
        m_queue.pop();
        //std::cout << "GameThread() Msg received: " << msg << std::endl;
        switch (msg->type)
        {
          case Message::Type::Quit:
            running = false;
            //std::cout << "GameThread() Msg type is Quit" << std::endl;
            delete msg;
            break;
          case Message::Type::Resize:
            width = msg->screen.width;
            height = msg->screen.height;
            perspective_changed = true;
            //std::cout << "GameThread() Msg type is Resize" << std::endl;
            delete msg;
            break;
          case Message::Type::TextInput:
            //std::cout << "GameThread() Msg type is TextInput: " << msg->text << std::endl;
            // Route event to the active Virtual Machine
            if (focused!=nullptr) { focused->push(msg); } else { delete msg; }
            break;
          case Message::Type::MouseMotion: {
            //std::cout << "GameThread() Msg type is MouseMotion" << std::endl;
            pan_x = (msg->motion.x - width/2) / 10.0f;
            pan_y = (msg->motion.y - height/2) / 10.0f;

            perspective_changed = true;
            //std::cout << m_scene << std::endl;

            if (msg->motion.x < width/2) {
              if (msg->motion.y < height/2) {
                focused = &vm_A;
              } else {
                focused = &vm_C;
              }
            } else {
              if (msg->motion.y < height/2) {
                focused = &vm_B;
              } else {
                focused = &vm_D;
              }
            }

            delete msg;
            break;
          }
          case Message::Type::MouseButtonDown:
            std::cout << "GameThread() Msg type is MouseButtonDown" << std::endl;
            delete msg;
            break;
          case Message::Type::MouseButtonUp:
            std::cout << "GameThread() Msg type is MouseButtonUp" << std::endl;
            delete msg;
            break;
          case Message::Type::MouseWheel:
            //std::cout << "GameThread() Msg type is MouseWheel" << std::endl;
            //if (focused == &vm_A) { focused = &vm_B; } else { focused = &vm_A; }
            fov *= (msg->wheel.y > 0 ? 0.95f : 1.05f );
            perspective_changed = true;
            // Keep within sane limits
            if (fov < 25) fov = 25;
            if (fov > 120) fov = 120;
            std::cout << "FOV=" << fov << std::endl;
            delete msg;
            break;
          case Message::Type::KeyDown:
            //std::cout << "GameThread() Msg type is KeyDown" << std::endl;
            if (msg->key.scancode == 71 && (msg->key.mod & KMOD_CTRL)) {
              std::cout << "GameThread() detected Ctrl+Break" << std::endl;
              if (focused!=nullptr) { focused->push(new Message(Message::Type::Break)); }
            }
            if (msg->key.sym == SDLK_INSERT && (msg->key.mod & KMOD_CTRL)) {
              std::cout << "GameThread() detected Ctrl+Insert" << std::endl;
              //if (focused!=nullptr) { focused->push(new Message(Message::Type::Paste)); }
            }
            if (msg->key.sym == SDLK_INSERT && (msg->key.mod & KMOD_SHIFT)) {
              std::cout << "GameThread() detected Shift+Insert" << std::endl;
              if (focused==nullptr) { break; } // No paste target
              char* str = SDL_GetClipboardText();
              if (str != nullptr) {
                // Generate TextInput events for clipboard contents
                std::vector<int> codepoints = utf8::codepoints(str);
                for (auto & codepoint : codepoints) {
                  //std::cout << "codepoint=" << codepoint << std::endl;
                  switch (codepoint) {
                    case 10: {
                      // Simulate the user hitting Enter (Does not normally produce TextInput events)
                      Message* keydn = new Message(Message::Type::KeyDown);
                      keydn->key.mod = KMOD_NONE;
                      keydn->key.scancode = SDL_SCANCODE_RETURN;
                      keydn->key.sym = SDLK_RETURN;
                      focused->push(keydn);
                      Message* keyup = new Message(Message::Type::KeyUp);
                      keyup->key.mod = KMOD_NONE;
                      keyup->key.scancode = SDL_SCANCODE_RETURN;
                      keyup->key.sym = SDLK_RETURN;
                      focused->push(keyup);
                      break;
                    }
                    default: {
                      Message* clip = new Message(Message::Type::TextInput);
                      char buf[5] = {0,0,0,0,0};
                      char* err = nullptr;
                      char* res = utf8::append(codepoint, buf, err); // utf8.h
                      clip->text = res;
                      focused->push(clip);
                    }
                  }
                }
              } else {
                std::cout << "Clipboard is empty" << std::endl;
              }
              //if (focused!=nullptr) { focused->push(new Message(Message::Type::Paste)); }
            }
            // Route event to the active Virtual Machine
            if (focused!=nullptr) { focused->push(msg); } else { delete msg; }
            break;
          case Message::Type::KeyUp:
            //std::cout << "GameThread() Msg type is KeyUp" << std::endl;
            // Route event to the active Virtual Machine
            if (focused!=nullptr) { focused->push(msg); } else { delete msg; }
            break;
          default:
            //std::cout << "GameThread() Msg type is UNHANDLED: " << msg->type << std::endl;
            std::cout << "GameThread() UNHANDLED " << msg << std::endl;
            delete msg;
        }

      }


      if (perspective_changed) {
        // Window size, mouse position or FOV changed; recalculate scene matrix
        float aspect = (float) width/(float) height;
        //std::cout << "aspect ratio = " << aspect << std::endl;

        m_scene = Matrix4();
        m_scene *= Matrix4().Perspective(fov,aspect,0.1f,100.0f);
        m_scene *= Matrix4().translate(-0.0f, 0.3f, -0.8f);
        m_scene *= Matrix4().rotate(170, Vector3(0.98, 0.01, 0.0));

        m_scene *= Matrix4().translate(0.0f, 0.0f, -1.0f);
        m_scene *= Matrix4().rotate(pan_y, Vector3(1.0f, 0.0f, 0.0f));
        m_scene *= Matrix4().rotate(pan_x, Vector3(0.0f, -1.0f, 0.0f));
        m_scene *= Matrix4().translate(0.0f, 0.0f, 1.0f);

        perspective_changed = false;
      }



      // Render frame
      //render_texture(m_ortho, framebufferID, ortho_shader, textureID);
      vm_A.run();
      vm_B.run();
      vm_C.run();
      vm_D.run();

      glViewport(0, 0, width, height);
      prepare_scene();

      m_model = Matrix4();
      m_model *= Matrix4().translate(-0.5f, 0.0f, 0.0f); // Relative position
      m_model *= Matrix4().rotate(-15, Vector3(0.0f, 1.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().rotate(-15, Vector3(1.0f, 0.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().translate(-0.5f, 0.0f, 0.0f); // Rotation center
      render_scene(m_scene, m_model, vertexBufferID, scene_shader, vm_A.display.textureID, test_object);

      m_model = Matrix4();
      m_model *= Matrix4().translate(+0.5f, 0.0f, 0.0f); // Relative position
      m_model *= Matrix4().rotate(+15, Vector3(0.0f, 1.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().rotate(-15, Vector3(1.0f, 0.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().translate(-0.5f, 0.0f, 0.0f); // Rotation center
      render_scene(m_scene, m_model, vertexBufferID, scene_shader, vm_B.display.textureID, test_object);

      m_model = Matrix4();
      m_model *= Matrix4().translate(-0.5f, 0.8f, 0.0f); // Relative position
      m_model *= Matrix4().rotate(-15, Vector3(0.0f, 1.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().rotate(-30, Vector3(1.0f, 0.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().translate(-0.5f, 0.0f, 0.0f); // Rotation center
      render_scene(m_scene, m_model, vertexBufferID, scene_shader, vm_C.display.textureID, test_object);

      m_model = Matrix4();
      m_model *= Matrix4().translate(+0.5f, 0.8f, 0.0f); // Relative position
      m_model *= Matrix4().rotate(+15, Vector3(0.0f, 1.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().rotate(-30, Vector3(1.0f, 0.0f, 0.0f)); // Rotation angle and axis
      m_model *= Matrix4().translate(-0.5f, 0.0f, 0.0f); // Rotation center
      render_scene(m_scene, m_model, vertexBufferID, scene_shader, vm_D.display.textureID, test_object);

      SDL_GL_SwapWindow(window);
      //SDL_Delay(0);
      std::this_thread::yield();

    }
  }
  catch (...) {
    std::cerr << "Caught exception" << std::endl;
  }
  std::cout<<"GameThread() exited its main loop"<<std::endl;

  //FunC::freeVM();
  delete test_object;

  TTF_CloseFont(font);
  SDL_GL_DeleteContext(context);
  return 0;
}



int main(int argc, char* argv[])
{
  (void)(argc);
  (void)(argv);

  SDL_Window* window;
  SDL_Event event;
  //const Uint8* SDL_KeyboardState;
  Message* msg = nullptr;
  bool running = true;



  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    std::string msg = "SDL failed to initialize: ";
    msg.append(SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
    return 0;
  }
  else
  {
    IMG_Init(IMG_INIT_PNG); // TODO: Error checking here
    TTF_Init(); // TODO: Error checking here
    //SDL_KeyboardState = SDL_GetKeyboardState(NULL);

    // Create Window here
    window = SDL_CreateWindow("Deep", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window == nullptr)
    {
      std::string msg = "Window could not be created: ";
      msg.append(SDL_GetError());
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
      return 0;
    }
    else
    {

      // Start game thread
      std::cout << "main() Starting GameThread" << std::endl;
      //SDL_Thread* threadID = SDL_CreateThread( GameThread, "GameThread", (void*) window );
      gamethread = std::thread(GameThread, (void*) window);

      while (running)
      {
        //std::cout<<"main() Waiting..."<<std::endl;
        SDL_WaitEvent(&event); // Blocking call

        switch (event.type)
        {
          case SDL_QUIT:
            running = false;
            msg = new Message(Message::Type::Quit);
            break;
          case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
              case SDL_WINDOWEVENT_SHOWN:
                //std::cout << "main() Window shown" << std::endl;
                break;
              case SDL_WINDOWEVENT_HIDDEN:
                //std::cout << "main() Window hidden" << std::endl;
                break;
              case SDL_WINDOWEVENT_EXPOSED:
                //std::cout << "main() Window exposed" << std::endl;
                break;
              case SDL_WINDOWEVENT_RESIZED:
                //std::cout << "main() Window resized" << std::endl;
                break;
              case SDL_WINDOWEVENT_SIZE_CHANGED:
                //std::cout << "main() Window size changed" << std::endl;
                msg = new Message(Message::Type::Resize);
                msg->screen.width = event.window.data1;
                msg->screen.height = event.window.data2;
                break;
              case SDL_WINDOWEVENT_MOVED:
                //std::cout << "main() Window moved" << std::endl;
                break;
              case SDL_WINDOWEVENT_MINIMIZED:
                //std::cout << "main() Window minimized" << std::endl;
                break;
              case SDL_WINDOWEVENT_MAXIMIZED:
                //std::cout << "main() Window maximized" << std::endl;
                break;
              case SDL_WINDOWEVENT_RESTORED:
                //std::cout << "main() Window restored" << std::endl;
                break;
              case SDL_WINDOWEVENT_ENTER:
                //std::cout << "main() Window mouse enter" << std::endl;
                break;
              case SDL_WINDOWEVENT_LEAVE:
                //std::cout << "main() Window mouse leave" << std::endl;
                break;
              case SDL_WINDOWEVENT_FOCUS_GAINED:
                //std::cout << "main() Window focus gained" << std::endl;
                break;
              case SDL_WINDOWEVENT_FOCUS_LOST:
                //std::cout << "main() Window focus lost" << std::endl;
                break;
              case SDL_WINDOWEVENT_CLOSE:
                //std::cout << "main() Window close" << std::endl;
                break;
              default:
                std::cout << "main() unhandled WINDOW event=" << event.window.event << std::endl;
                break;
            }
            break;
          case SDL_TEXTINPUT:
            msg = new Message(Message::Type::TextInput);
            msg->text = event.text.text;
            break;
          case SDL_TEXTEDITING:
            //std::cerr << "main() SDL_TEXTEDITING ignored" << std::endl;
            break;
          case SDL_MOUSEMOTION:
            // Temporary stuff
            // New stuff
            msg = new Message(Message::Type::MouseMotion);
            msg->motion.x = event.motion.x;
            msg->motion.y = event.motion.y;
            msg->motion.xrel = event.motion.xrel;
            msg->motion.yrel = event.motion.yrel;
            break;
          case SDL_MOUSEBUTTONDOWN:
            msg = new Message(Message::Type::MouseButtonDown);
            std::cerr << "main() SDL_MOUSEBUTTONDOWN handling incomplete" << std::endl;
            break;
          case SDL_MOUSEBUTTONUP:
            msg = new Message(Message::Type::MouseButtonUp);
            std::cerr << "main() SDL_MOUSEBUTTONUP handling incomplete" << std::endl;
            break;
          case SDL_MOUSEWHEEL:
            msg = new Message(Message::Type::MouseWheel);
            msg->wheel.x = event.wheel.x;
            msg->wheel.y = event.wheel.y;
            //std::cerr << "main() SDL_MOUSEWHEEL handling incomplete" << std::endl;
            break;
          case SDL_KEYDOWN:
            msg = new Message(Message::Type::KeyDown);
            // https://wiki.libsdl.org/SDL_Keysym
            msg->key.mod      = event.key.keysym.mod;      // SDL_Keymod (alt, shift, ctrl etc.)
            msg->key.scancode = event.key.keysym.scancode; // SDL_Scancode (physical)
            msg->key.sym      = event.key.keysym.sym;      // SDL_Keycode (logical)
            break;
          case SDL_KEYUP:
            msg = new Message(Message::Type::KeyUp);
            break;
          case SDL_SYSWMEVENT:
            //std::cerr << "main() SDL_SYSWMEVENT ignored" << std::endl;
            break;
          case SDL_AUDIODEVICEADDED:
            //std::cerr << "main() SDL_AUDIODEVICEADDED ignored" << std::endl;
            break;
          case SDL_AUDIODEVICEREMOVED:
            //std::cerr << "main() SDL_AUDIODEVICEREMOVED ignored" << std::endl;
            break;
          default:
            std::cout << "main() unhandled SDL_Event type=" << event.type << std::endl;
        }


        // Send message to GameThread (if any)
        if (msg != nullptr) {
          std::unique_lock<std::mutex> lk(m_mutex);
          //std::cout << "main() dispatching message " << msg << std::endl;
          m_queue.push(msg);
          msg = nullptr;
        }

      }

      std::cout<<"main() Waiting for GameThread to finish..."<<std::endl;
      //SDL_WaitThread( threadID, NULL );
      gamethread.join();
      std::cout<<"main() Destroying window"<<std::endl;
      SDL_DestroyWindow(window);
      window = nullptr;


      std::cout<<"Shutting down SDL"<<std::endl;
      TTF_Quit();
      IMG_Quit();
      SDL_Quit();
    }
  }

  std::cout<<"Clean exit"<<std::endl;
  return 0;
}
