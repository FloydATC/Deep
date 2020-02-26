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

#include <ws2tcpip.h>

#include "EventHandler.h"
#include "GameState.h"
//#include "IOFile.h"
#include "Machine.h"
#include "Message.h"
#include "Scene3D.h"
#include "ShaderProgram.h"
#include "UTF8hack.h"
#include "hexdump.h" // For debugging only


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
/*
void check_gl(std::string when)
{
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    std::cerr << "OpenGL error " << when << ": " << gluErrorString(err) << "(" << err << ")" << std::endl;
    //running = false;
  }
}

*/

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




/*

void prepare_scene()
{
  //std::cout << "prepare_scene()" << std::endl;
  glClearColor(0.25f, 0.25f, 0.30f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  check_gl("prepare scene");
}

*/


void process_keydown(Message* msg, std::vector<Machine*> vms, GameState* gamestate)
{
  //std::cout << "process_keydown()" << std::endl;
  if (msg->key.scancode == 71 && (msg->key.mod & KMOD_CTRL)) {
    std::cout << "process_keydown() detected Ctrl+Break" << std::endl;
    vms[gamestate->current_vm]->push(new Message(Message::Type::Break));
  }
  if (msg->key.sym == SDLK_INSERT && (msg->key.mod & KMOD_CTRL)) {
    std::cout << "process_keydown() detected Ctrl+Insert" << std::endl;
    //if (focused!=nullptr) { focused->push(new Message(Message::Type::Paste)); }
  }
  if (msg->key.sym == SDLK_INSERT && (msg->key.mod & KMOD_SHIFT)) {
    //std::cout << "process_keydown() detected Shift+Insert" << std::endl;
    char* str = SDL_GetClipboardText();
    if (str != nullptr) {
      // Generate TextInput events for clipboard contents
      //std::vector<int> codepoints = UTF8hack::codepoints(str, strlen(str));
      for (auto & codepoint : UTF8hack::codepoints(str, strlen(str))) {
        //std::cout << "codepoint=" << codepoint << std::endl;
        switch (codepoint) {
          case 10: {
            // Simulate the user hitting Enter (Does not normally produce TextInput events)
            Message* keydn = new Message(Message::Type::KeyDown);
            keydn->key.mod = KMOD_NONE;
            keydn->key.scancode = SDL_SCANCODE_RETURN;
            keydn->key.sym = SDLK_RETURN;
            vms[gamestate->current_vm]->push(keydn);
            Message* keyup = new Message(Message::Type::KeyUp);
            keyup->key.mod = KMOD_NONE;
            keyup->key.scancode = SDL_SCANCODE_RETURN;
            keyup->key.sym = SDLK_RETURN;
            vms[gamestate->current_vm]->push(keyup);
            break;
          }
          default: {
            Message* clip = new Message(Message::Type::TextInput);
            char buf[5] = {0,0,0,0,0};
            char* err = nullptr;
            char* res = UTF8hack::append(codepoint, buf, err); // utf8.h
            clip->text = res;
            vms[gamestate->current_vm]->push(clip);
          }
        }
      }
    } else {
      std::cout << "Clipboard is empty" << std::endl;
    }
    //if (focused!=nullptr) { focused->push(new Message(Message::Type::Paste)); }
  }
  // Route event to the active Virtual Machine
  vms[gamestate->current_vm]->push(msg);
}


void process_mousemotion(Message* msg, Scene3D* scene, GameState* gamestate)
{
  // Mouse controls camera direction
  //std::cout << "processMsg() MouseMotion message x=" << msg->motion.x << " y=" << msg->motion.y << std::endl;
  int x = msg->motion.x;
  int y = msg->motion.y;
  int w = scene->camera()->getWidth();
  int h = scene->camera()->getHeight();
  float sensitivity = 0.2;
  scene->camera()->setPitch((float) (y - (h/2)) * sensitivity);
  scene->camera()->setYaw((float) (x - (w/2)) * sensitivity);

  // Focus on VM based on where the user is looking
  if (msg->motion.x < w/2) {
    if (msg->motion.y < h/2) {
      gamestate->current_vm = 0;
    } else {
      gamestate->current_vm = 2;
    }
  } else {
    if (msg->motion.y < h/2) {
      gamestate->current_vm = 1;
    } else {
      gamestate->current_vm = 3;
    }
  }

  delete msg;
}

void process_mousewheel(Message* msg, Scene3D* scene)
{
  // Mouse wheel controls camera zoom
  //std::cout << "processMsg() MouseWheel message" << std::endl;
  float fov = scene->camera()->getFOV();
  fov *= (msg->wheel.y > 0 ? 0.95f : 1.05f );
  scene->camera()->setFOV(fov);
  delete msg;
}

void process_message(Message* msg, std::vector<Machine*> vms, Scene3D* scene, GameState* gamestate)
{

  //std::cout << "processMsg()" << std::endl;
  //std::cout << "processMsg() msg received: " << msg << std::endl;
  switch (msg->type)
  {
    case Message::Type::Quit:
      //running = false;
      //std::cout << "processMsg() Quit message" << std::endl;
      delete msg;
      gamestate->shutdown = true;
      break;
    case Message::Type::Resize:
      //std::cout << "processMsg() Resize message" << std::endl;
      scene->camera()->setDimensions(msg->screen.width, msg->screen.height);
      gamestate->width = msg->screen.width;
      gamestate->height = msg->screen.height;
      delete msg;
      break;
    case Message::Type::TextInput:
      //std::cout << "processMsg() TextInput message" << std::endl;
      // Route event to the active Virtual Machine
      vms[gamestate->current_vm]->push(msg);
      break;
    case Message::Type::MouseMotion: {
      process_mousemotion(msg, scene, gamestate);
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
      //std::cout << "processMsg() KeyDown message" << std::endl;
      process_keydown(msg, vms, gamestate);
      break;
    case Message::Type::KeyUp:
      //std::cout << "processMsg() KeyUp message" << std::endl;
      // Route event to the active Virtual Machine
      vms[gamestate->current_vm]->push(msg);
      break;
    default:
      //std::cout << "GameThread() Msg type is UNHANDLED: " << msg->type << std::endl;
      std::cout << "processMsg() UNHANDLED message " << msg << std::endl;
      delete msg;
  }
  //std::cout << "processMsg() done" << std::endl;
}


std::string init_sdl()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) return SDL_GetError();
  if (IMG_Init(IMG_INIT_PNG) < 0) return "IMG_Init() failed";
  if (TTF_Init() < 0) return "TTF_Init() failed";
  //if (SDLNet_Init() < 0) return SDLNet_GetError();
  return "success";
}

void init_winsock(WSADATA* wsaData)
{
  int status = WSAStartup(MAKEWORD(2,2), wsaData);
  if (status != 0) std::cerr << "WARNING: WSAStartup error " << WSAGetLastError() << std::endl;
}

void shutdown_sdl(SDL_Window* window)
{
  std::cout<<"Shutting down SDL"<<std::endl;
  SDL_DestroyWindow(window);
  //SDLNet_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

void shutdown_winsock()
{
  int status = WSACleanup();
  if (status != 0) std::cerr << "WARNING: WSACleanup returned " << status << std::endl;
}


void OpenGL_debug_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg, const void *data )
{
  std::cout << "main:OpenGL_debug_callback() id=" << id << std::endl;
  switch (source)
  {
      case GL_DEBUG_SOURCE_API:             std::cout << "  Source: API"; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "  Source: Window System"; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "  Source: Shader Compiler"; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "  Source: Third Party"; break;
      case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "  Source: Application"; break;
      case GL_DEBUG_SOURCE_OTHER:           std::cout << "  Source: Other"; break;
  } std::cout << std::endl;

  switch (type)
  {
      case GL_DEBUG_TYPE_ERROR:               std::cout << "  Type: Error"; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "  Type: Deprecated Behaviour"; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "  Type: Undefined Behaviour"; break;
      case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "  Type: Portability"; break;
      case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "  Type: Performance"; break;
      case GL_DEBUG_TYPE_MARKER:              std::cout << "  Type: Marker"; break;
      case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "  Type: Push Group"; break;
      case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "  Type: Pop Group"; break;
      case GL_DEBUG_TYPE_OTHER:               std::cout << "  Type: Other"; break;
  } std::cout << std::endl;

  switch (severity)
  {
      case GL_DEBUG_SEVERITY_HIGH:         std::cout << "  Severity: high"; break;
      case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "  Severity: medium"; break;
      case GL_DEBUG_SEVERITY_LOW:          std::cout << "  Severity: low"; break;
      case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "  Severity: notification"; break;
  } std::cout << std::endl;

  std::cout << "  " << msg << std::endl;
}



int main(int argc, char* argv[])
{
  (void)(argc);
  (void)(argv);

  GameState gamestate = GameState();
  gamestate.width = 640;
  gamestate.height = 480;

  EventHandler event_handler = EventHandler();

  SDL_Window* window;
  SDL_Event event;
  SDL_GLContext context;

  WSADATA wsaData;
  init_winsock(&wsaData);

  std::string msg = init_sdl();
  if(msg != "success")
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
    return 0;
  } else {

    // Create Window here
    window = SDL_CreateWindow("Deep",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              gamestate.width,
                              gamestate.height,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(window == nullptr)
    {
      std::string msg = "Window could not be created: ";
      msg.append(SDL_GetError());
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
      return 0;
    }
    else
    {
      SDL_GetWindowSize(window, &gamestate.width, &gamestate.height);

      //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

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
        glewInit();

        glDebugMessageCallback(OpenGL_debug_callback, NULL);
        glEnable(GL_DEBUG_OUTPUT);
        std::vector<GLuint> ignored = {}; // Debug messages to suppress
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, ignored.size(), ignored.data(), true);
      }

      TTF_Font* font = TTF_OpenFont("fonts/unscii-8-thin.ttf", 16);
      if(!font) {
        std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
        return 0;
      }

      std::cout << "Load new font object" << std::endl;
      Fontcache fontcache = Fontcache(font);



      Scene3D scene = Scene3D();
      scene.camera()->setDimensions(gamestate.width, gamestate.height);

      std::cout << "Load shaders" << std::endl;
      ShaderProgram* scene_shader = scene.getShader("glsl/scene_vert.glsl", "glsl/scene_frag.glsl");
      scene_shader->setAttributeV("vertex");
      scene_shader->setAttributeVT("uv");
      scene_shader->setAttributeVN("normal");
      scene_shader->setUniformCameraMatrix("scene");
      scene_shader->setUniformModelMatrix("model");
      scene_shader->setUniformDebugFlag("is_debug");



      std::vector<Machine*> vms;
#ifndef DEBUG_NO_VIRTUAL_MACHINES
      ShaderProgram* ortho_shader = scene.getShader("glsl/ortho_vert.glsl", "glsl/ortho_frag.glsl");
      std::cout << "main() Creating virtual machines" << std::endl;
      vms.push_back(new Machine(ortho_shader->id(), font));
      vms.push_back(new Machine(ortho_shader->id(), font));
      vms.push_back(new Machine(ortho_shader->id(), font));
      vms.push_back(new Machine(ortho_shader->id(), font));
      std::cout << "main() Virtual machines ready" << std::endl;
#endif

      Obj3D* test_object = scene.getObj3D("obj/screen.obj");

      Prop3D* p1 = scene.addProp(test_object);
      Prop3D* p2 = scene.addProp(test_object);
      Prop3D* p3 = scene.addProp(test_object);
      Prop3D* p4 = scene.addProp(test_object);

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

      p1->setShader(scene_shader);
      p2->setShader(scene_shader);
      p3->setShader(scene_shader);
      p4->setShader(scene_shader);

#ifndef DEBUG_NO_VIRTUAL_MACHINES
      p1->setTexture(vms[0]->display.textureID);
      p2->setTexture(vms[1]->display.textureID);
      p3->setTexture(vms[2]->display.textureID);
      p4->setTexture(vms[3]->display.textureID);
#endif


      std::cout << "main() ----------- Entering main loop" << std::endl;
      while (gamestate.shutdown == false)
      {
        //SDL_WaitEvent(&event); // Blocking call
        while (SDL_PollEvent(&event)) {
          event_handler.handle_event(event);
        }


        while (event_handler.has_message()) {
          process_message(event_handler.get_message(), vms, &scene, &gamestate);
        }

        for (auto& vm: vms) {
          vm->run();
        }

        scene.render();

        SDL_GL_SwapWindow(window);
      }
      std::cout << "main() ----------- Exiting main loop" << std::endl;

      for (auto& vm: vms) {
        delete vm;
      }



      TTF_CloseFont(font);
      SDL_GL_DeleteContext(context);

      std::cout<<"main() Destroying window"<<std::endl;
      shutdown_sdl(window);
      shutdown_winsock();
    }
  }

  std::cout<<"Clean exit"<<std::endl;
  return 0;
}

