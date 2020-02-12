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

#include "GFX.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "EventHandler.h"
#include "IOFile.h"
#include "Matrices.h"
#include "Machine.h"
#include "Message.h"
#include "Obj3DLoader.h"
#include "Scene3D.h"
#include "ShaderProgram.h"
#include "utf8.h"

#define WIDTH 640
#define HEIGHT 480

//std::thread gamethread;
// https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue
//std::queue<Message*> m_queue;
//std::mutex m_mutex;
//std::condition_variable m_cv;
//std::atomic<bool> m_timerExit; // May be better than "running"

//bool main_running = true;

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


/*
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
*/





void prepare_scene()
{
  //std::cout << "prepare_scene()" << std::endl;
  glClearColor(0.25f, 0.25f, 0.30f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  check_gl("prepare scene");
}



/*
  ===================================================
  RENDER 3D MODEL
  ===================================================
*/
/*
//void render_scene(Matrix4 m_scene, Matrix4 m_model, GLuint vertexBufferID, GLuint shaderProgramID, GLuint TextureID, float pan_x, float pan_y)
void render_model(Matrix4 m_scene, Matrix4 m_model, GLuint shaderProgramID, GLuint TextureID, Obj3D* obj)
{
  //std::cout << "render_model() set shader ID = " << shaderProgramID << std::endl;
  glUseProgram(shaderProgramID);
  check_gl("set shader");

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
*/

void process_keydown(Message* msg, Machine* focused)
{
  std::cout << "process_keydown()" << std::endl;
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
    if (focused!=nullptr) {
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
    }
    //if (focused!=nullptr) { focused->push(new Message(Message::Type::Paste)); }
  }
  // Route event to the active Virtual Machine
  if (focused!=nullptr) { focused->push(msg); } else { delete msg; }
}


void process_mousemotion(Message* msg, Scene3D* scene)
{
  //std::cout << "processMsg() MouseMotion message x=" << msg->motion.x << " y=" << msg->motion.y << std::endl;
  int x = msg->motion.x;
  int y = msg->motion.y;
  int w = scene->camera()->getWidth();
  int h = scene->camera()->getHeight();
  float sensitivity = 0.2;
  scene->camera()->setPitch((float) (y - (h/2)) * sensitivity);
  scene->camera()->setYaw((float) (x - (w/2)) * sensitivity);

  //perspective_changed = true;
  //std::cout << m_scene << std::endl;
/*
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
*/
  delete msg;
}

void process_mousewheel(Message* msg, Scene3D* scene)
{
  //std::cout << "processMsg() MouseWheel message" << std::endl;
  float fov = scene->camera()->getFOV();
  fov *= (msg->wheel.y > 0 ? 0.95f : 1.05f );
  scene->camera()->setFOV(fov);
  delete msg;
}

void process_message(Message* msg, std::vector<Machine*> vms, int focused, Scene3D* scene)
{

  //std::cout << "processMsg()" << std::endl;
  //std::cout << "processMsg() msg received: " << msg << std::endl;
  switch (msg->type)
  {
    case Message::Type::Quit:
      //running = false;
      std::cout << "processMsg() Quit message" << std::endl;
      delete msg;
      break;
    case Message::Type::Resize:
      //width = msg->screen.width;
      //height = msg->screen.height;
      //perspective_changed = true;
      scene->camera()->setDimensions(msg->screen.width, msg->screen.height);
      std::cout << "processMsg() Resize message" << std::endl;
      delete msg;
      break;
    case Message::Type::TextInput:
      std::cout << "processMsg() TextInput message" << std::endl;
      //std::cout << "GameThread() Msg type is TextInput: " << msg->text << std::endl;
      // Route event to the active Virtual Machine
      vms[focused]->push(msg);
      break;
    case Message::Type::MouseMotion: {
      process_mousemotion(msg, scene);
      break;
    }
    case Message::Type::MouseButtonDown:
      std::cout << "processMsg() MouseButtonDown message" << std::endl;
      delete msg;
      break;
    case Message::Type::MouseButtonUp:
      std::cout << "processMsg() MouseButtonUp message" << std::endl;
      delete msg;
      break;
    case Message::Type::MouseWheel:
      process_mousewheel(msg, scene);
      break;
    case Message::Type::KeyDown:
      std::cout << "processMsg() KeyDown message" << std::endl;
      process_keydown(msg, vms[focused]);
      break;
    case Message::Type::KeyUp:
      std::cout << "processMsg() KeyUp message" << std::endl;
      // Route event to the active Virtual Machine
      vms[focused]->push(msg);
      break;
    default:
      //std::cout << "GameThread() Msg type is UNHANDLED: " << msg->type << std::endl;
      std::cout << "GameThread() UNHANDLED " << msg << std::endl;
      delete msg;
  }
  //std::cout << "processMsg() done" << std::endl;
}




void shutdown_sdl(SDL_Window* window)
{
  std::cout<<"Shutting down SDL"<<std::endl;
  SDL_DestroyWindow(window);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}




int main(int argc, char* argv[])
{
  (void)(argc);
  (void)(argv);

  SDL_Window* window;
  SDL_Event event;
  //const Uint8* SDL_KeyboardState;


  EventHandler event_handler = EventHandler();

  //SDL_Window* window = (SDL_Window*) ptr;
  SDL_GLContext context;
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  //float pan_x = 0.0;
  //float pan_y = 0.0;
  //float fov = 60.0f;
  int width = 0;
  int height = 0;
  //bool perspective_changed = true; // true = need to initialize projection matrix
  bool failure = false;


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
      SDL_GetWindowSize(window, &width, &height);
      // Create OpenGL context here
      std::cout<<"GameThread() creating OpenGL context"<<std::endl;
      context = SDL_GL_CreateContext(window);

      if(context == nullptr)
      {
        std::string msg = "OpenGL context could not be created: ";
        msg.append(SDL_GetError());
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
        return 0;
      } else {
        check_gl("creating context");
        glewInit();
        check_gl("initializing glew");
      }

      TTF_Font* font = TTF_OpenFont("fonts/unscii-8-thin.ttf", 16);
      if(!font) {
        std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
        return 0;
      }

      std::cout << "Load new font object" << std::endl;
      Fontcache fontcache = Fontcache(font);



      Scene3D* scene = new Scene3D();
      scene->camera()->setDimensions(width, height);

      std::cout << "Load shaders" << std::endl;
      ShaderProgram* scene_shader = scene->getShader("glsl/scene_vert.glsl", "glsl/scene_frag.glsl");
      ShaderProgram* ortho_shader = scene->getShader("glsl/ortho_vert.glsl", "glsl/ortho_frag.glsl");


      std::cout << "main() Creating virtual machines" << std::endl;
      std::vector<Machine*> vms;
      vms.push_back(new Machine(ortho_shader->id(), font));
      vms.push_back(new Machine(ortho_shader->id(), font));
      vms.push_back(new Machine(ortho_shader->id(), font));
      vms.push_back(new Machine(ortho_shader->id(), font));
      std::cout << "main() Virtual machines ready" << std::endl;

      Obj3D* test_object = scene->getObj3D("obj/screen.obj");

      Prop3D* p1 = scene->addProp(test_object);
      Prop3D* p2 = scene->addProp(test_object);
      Prop3D* p3 = scene->addProp(test_object);
      Prop3D* p4 = scene->addProp(test_object);

      p1->setScale(2.5);
      p2->setScale(2.5);
      p3->setScale(2.5);
      p4->setScale(2.5);

      p1->setPosition(Vector3(-0.60,  0.50,  0.0));
      p2->setPosition(Vector3( 0.60,  0.50,  0.0));
      p3->setPosition(Vector3(-0.60, -0.50,  0.0));
      p4->setPosition(Vector3( 0.60, -0.50,  0.0));

      p1->setDirection(Vector3(100, 20,  0));
      p2->setDirection(Vector3(100,-20,  0));
      p3->setDirection(Vector3( 80, 20,  0));
      p4->setDirection(Vector3( 80,-20,  0));

      p1->setShader(scene_shader, "vertex", "uv", "normal");
      p2->setShader(scene_shader, "vertex", "uv", "normal");
      p3->setShader(scene_shader, "vertex", "uv", "normal");
      p4->setShader(scene_shader, "vertex", "uv", "normal");

      p1->setTexture(vms[0]->display.textureID);
      p2->setTexture(vms[1]->display.textureID);
      p3->setTexture(vms[2]->display.textureID);
      p4->setTexture(vms[3]->display.textureID);


      std::cout << "main() ----------- Entering main loop" << std::endl;
      while (failure == false && event_handler.shutdown == false)
      {
        //SDL_WaitEvent(&event); // Blocking call
        if (SDL_PollEvent(&event)) {
          event_handler.handle_event(event);
        }


        while (event_handler.has_message()) {
          // Note: processMsg is a placeholder
          process_message(event_handler.get_message(), vms, 0, scene);
        }

        for (auto& vm: vms) {
          vm->run();
        }

        //render_frame(width, height, scene.camera()->matrix(), scene_shader->id(), vms, test_object);
        scene->render();

        SDL_GL_SwapWindow(window);
      }
      std::cout << "main() ----------- Exiting main loop" << std::endl;

      for (auto& vm: vms) {
        delete vm;
      }

      delete scene;

      TTF_CloseFont(font);
      SDL_GL_DeleteContext(context);

      std::cout<<"main() Destroying window"<<std::endl;
      shutdown_sdl(window);
    }
  }

  std::cout<<"Clean exit"<<std::endl;
  return 0;
}
