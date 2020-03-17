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
#include "Machine.h"
#include "Message.h"
#include "2D/Overlay2D.h"
#include "3D/Scene3D.h"
#include "ShaderProgram.h"
#include "UTF8hack.h"
#include "3D/Plane3D.h"
#include "hexdump.h" // For debugging only
#include "Texture.h"
#include "3D/Light3D.h"


//std::thread gamethread;
// https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue
//std::queue<Message*> m_queue;
//std::mutex m_mutex;
//std::condition_variable m_cv;
//std::atomic<bool> m_timerExit; // May be better than "running"

//bool main_running = true;

// https://www.turbonut.com/2019/03/18/opengl3-sdl2/
// http://lazyfoo.net/SDL_tutorials/lesson33/index.php









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
  if ((int)vms.size() >= gamestate->current_vm+1) vms[gamestate->current_vm]->push(msg);
}


void process_mousemotion(Message* msg, std::vector<Machine*> vms, Scene3D* scene, GameState* gamestate)
{
  // Mouse controls camera direction
  //std::cout << "processMsg() MouseMotion message x=" << msg->motion.x << " y=" << msg->motion.y << std::endl;
  int x = msg->motion.x;
  int y = msg->motion.y;
  int w = scene->getCamera()->getWidth();
  int h = scene->getCamera()->getHeight();
  float sensitivity = 0.2;
  scene->getCamera()->setPitch(-(float) (y - (h/2)) * sensitivity);
  scene->getCamera()->setYaw(-(float) (x - (w/2)) * sensitivity);

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

  // Detect Obj3D mouse intersection
  // WARNING! This temporary code assumes a 1:1 relationship between Obj3Ds and VMs
  Vector2 mouse = Vector2(msg->motion.x, msg->motion.y);
  bool mouse_virtual = false;

  for (int i=0; i<4; i++) {
    //std::cout << "VM " << i << std::endl;
    Prop3D* p = scene->getPropByName(std::string("vm").append(std::to_string(i)));
    if (p == nullptr) continue;
    if (!p->xy_plane_visible()) continue;
    if (!p->mouse_intersects(mouse)) continue;
    gamestate->current_vm = i;

    Vector2 v = p->relative_mouse_pos(mouse, scene->getCamera());
    // v 0,0 is center of screen so we must calibrate to VM's screen
    // Maybe we should do this inside Machine.cpp, I'm not 100% sure.
    //std::cout << "  raw mouse position " << v.x << "," << v.y << std::endl;
    Vector2 mv; // v mapped to 320x200 display of VM
    mv.x = (v.x + 0.38) * 320 / 0.76;
    mv.y = ((v.y * -1) + 0.3) * 200 / 0.6;
    if (mv.x >= 0 && mv.x < 320 && mv.y >= 0 && mv.y < 200) {
      p->setDecalPosition(Vector2(v.x, v.y));
      Message* relative = new Message(Message::Type::MouseMotion);
      relative->motion.x = int(mv.x);
      relative->motion.y = int(mv.y);
      //std::cout << "  mouse position " << relative->motion.x << "," << relative->motion.y << std::endl;
      vms[i]->push(relative);
      mouse_virtual = true;
    }

  }
  // Hide system mouse cursor if it is active on one of the VM screens
  SDL_ShowCursor( mouse_virtual ? SDL_DISABLE : SDL_ENABLE);
  delete msg;
}

void process_mousewheel(Message* msg, Scene3D* scene)
{
  // Mouse wheel controls camera zoom
  //std::cout << "processMsg() MouseWheel message" << std::endl;
  float fov = scene->getCamera()->getFOV();
  fov *= (msg->wheel.y > 0 ? 0.95f : 1.05f );
  scene->getCamera()->setFOV(fov);
  delete msg;
}

void process_mousebutton(Message* msg, std::vector<Machine*> vms, GameState* gamestate)
{
  vms[gamestate->current_vm]->push(msg);
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
      scene->setDimensions(msg->screen.width, msg->screen.height);
      gamestate->width = msg->screen.width;
      gamestate->height = msg->screen.height;
      delete msg;
      break;
    case Message::Type::TextInput:
      //std::cout << "processMsg() TextInput message" << std::endl;
      // Route event to the active Virtual Machine
      if ((int)vms.size() >= gamestate->current_vm+1) vms[gamestate->current_vm]->push(msg);
      break;
    case Message::Type::MouseMotion: {
      process_mousemotion(msg, vms, scene, gamestate);
      break;
    }
    case Message::Type::MouseButton:
      process_mousebutton(msg, vms, gamestate);
      break;
    case Message::Type::MouseWheel:
      process_mousewheel(msg, scene);
      break;
    case Message::Type::KeyDown:
      //std::cout << "processMsg() KeyDown message" << std::endl;
      if (msg->key.sym == SDLK_KP_4) scene->getCamera()->strafeLeft(0.1);
      if (msg->key.sym == SDLK_KP_6) scene->getCamera()->strafeRight(0.1);
      if (msg->key.sym == SDLK_KP_8) scene->getCamera()->strafeUp(0.1);
      if (msg->key.sym == SDLK_KP_2) scene->getCamera()->strafeDown(0.1);
      if (msg->key.sym == SDLK_KP_PLUS) scene->getCamera()->strafeForward(0.1);
      if (msg->key.sym == SDLK_KP_MINUS) scene->getCamera()->strafeBackward(0.1);
      process_keydown(msg, vms, gamestate);
      break;
    case Message::Type::KeyUp:
      //std::cout << "processMsg() KeyUp message" << std::endl;
      // Route event to the active Virtual Machine
      if ((int)vms.size() >= gamestate->current_vm+1) vms[gamestate->current_vm]->push(msg);
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


#define DEBUG_NO_VIRTUAL_MACHINES
#define DEBUG_NO_SCREENS
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
      SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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

        GLuint id = 131185; // Static draw buffer will use video memory. Yes? Where else would I want it?
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 1, &id, GL_FALSE);
      }

      TTF_Font* font = TTF_OpenFont("fonts/unscii-8-thin.ttf", 16);
      if(!font) {
        std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
        return 0;
      }

      std::cout << "Load new font object" << std::endl;
      Fontcache fontcache = Fontcache(font);


      Scene3D scene = Scene3D();
      scene.setDimensions(gamestate.width, gamestate.height);
      scene.getCamera()->setPosition(Vector3(0.0, 0.0, 1.0));

      std::cout << "Load shaders" << std::endl;
#ifndef DEBUG_NO_SCREENS
      ShaderProgram* screen_shader = scene.getShader("glsl/screen_vert.glsl", "glsl/screen_frag.glsl");
#endif

      ShaderProgram* scene_shader = scene.getShader("glsl/scene_vert.glsl", "glsl/scene_frag.glsl");

#ifndef DEBUG_NO_VIRTUAL_MACHINES
      ShaderProgram* ortho_shader = scene.getShader("glsl/ortho_vert.glsl", "glsl/ortho_frag.glsl");
#endif

      //Overlay2D* ui = new Overlay2D();
      //ui->setShader(ortho_shader);

      std::vector<Machine*> vms;
#ifndef DEBUG_NO_VIRTUAL_MACHINES
      std::cout << "main() Creating virtual machines" << std::endl;
      vms.push_back(new Machine(ortho_shader, font));
      vms.push_back(new Machine(ortho_shader, font));
      vms.push_back(new Machine(ortho_shader, font));
      vms.push_back(new Machine(ortho_shader, font));
      std::cout << "main() Virtual machines ready" << std::endl;
#endif

#ifndef DEBUG_NO_SCREENS
      Texture* mouse = scene.getTexture("textures/mouse.png");
      Obj3D* screen = scene.getObj3D("obj/screen.obj");
      //screen->getPart(0)->setColor(1.0, 1.0, 1.0, 1.0); // Display
      screen->getPart(0)->setShader(screen_shader);
      screen->getPart(0)->setDecalTexture(mouse);
      //screen->getPart(1)->setColor(0.2, 0.2, 0.3, 1.0); // Enclosure

      scene.addProp("vm0", screen);
      scene.addProp("vm1", screen);
      scene.addProp("vm2", screen);
      scene.addProp("vm3", screen);

      scene.getPropByName("vm0")->setPosition(Vector3(-0.60,  0.50, -0.5));
      scene.getPropByName("vm1")->setPosition(Vector3( 0.60,  0.50, -0.5));
      scene.getPropByName("vm2")->setPosition(Vector3(-0.60, -0.50, -0.5));
      scene.getPropByName("vm3")->setPosition(Vector3( 0.60, -0.50, -0.5));

      scene.getPropByName("vm0")->setDirection( 15, 15,  0);
      scene.getPropByName("vm1")->setDirection( 15,-15,  0);
      scene.getPropByName("vm2")->setDirection(-15, 15,  0);
      scene.getPropByName("vm3")->setDirection(-15,-15,  0);

      // Hide mouse cursors by default
      scene.getPropByName("vm0")->setDecalPosition(Vector2(-1, -1));
      scene.getPropByName("vm1")->setDecalPosition(Vector2(-1, -1));
      scene.getPropByName("vm2")->setDecalPosition(Vector2(-1, -1));
      scene.getPropByName("vm3")->setDecalPosition(Vector2(-1, -1));
#endif

      ShaderProgram* plane_shader = scene.getShader("glsl/plane_vert.glsl", "glsl/plane_frag.glsl");
      Material* plane_material = scene.getMaterial();
      plane_material->setAmbientColor(0.10, 0.10, 0.11);
      plane_material->setDiffuseColor(0.75, 0.75, 0.80);
      plane_material->setSpecularColor(0.25, 0.25, 0.30);
      plane_material->setEmissiveColor(0.00, 0.00, 0.00);
      plane_material->setName("plane");
      Plane3D* plane = new Plane3D();
      plane->setShader(plane_shader);
      plane->setMaterial(plane_material);
      scene.addProp("plane", plane);
      scene.getPropByName("plane")->setPosition(Vector3(0.0, -1.0, 0.0));
      scene.getPropByName("plane")->setDirection(Vector3(0.0, 1.0, 0.0));

      Obj3D* cube = scene.getObj3D("obj/cube.obj");
      scene.addProp("cube", cube);
      Texture* test = scene.getTexture("textures/test256.png");
      scene.getPropByName("cube")->setTexture(test);
      scene.getPropByName("cube")->setPosition(Vector3(0.0, -0.25, 0.0));

      scene.setShader(scene_shader); // Set default shader

      Light3D* overhead = new Light3D(Vector3(0.7, 0.7, 0.6));
      overhead->setPosition(Vector3( -0.10, 1.00, 0.5));
      scene.addLight("overhead", overhead);


#ifndef DEBUG_NO_VIRTUAL_MACHINES
      // Bind VM textures to props (NOT to the underlying, shared mesh objects)
      for (int i=0; i<(int)vms.size(); i++) {
        std::cout << "main() applying VM texture " << i << std::endl;
        scene.getPropByName(std::string("vm").append(std::to_string(i)))->setTexture(vms[i]->display.textureId());
      }
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

        //ui->setDimensions(scene.getWidth(), scene.getHeight());
        //ui->pre_render();

        // For debugging, visualize the xy_plane of each Prop3D
        /*
        for (int i=0; i<4; i++) {
          Prop3D* p = scene.getProp(i);
          if (p->xy_plane_visible())
            ui->draw_quad(
              Vector2(p->xy_plane[0].x, p->xy_plane[0].y),
              Vector2(p->xy_plane[1].x, p->xy_plane[1].y),
              Vector2(p->xy_plane[2].x, p->xy_plane[2].y),
              Vector2(p->xy_plane[3].x, p->xy_plane[3].y)
            );
        }
        */

        //ui->post_render();

        SDL_GL_SwapWindow(window);
      }
      std::cout << "main() ----------- Exiting main loop" << std::endl;

      for (auto& vm: vms) {
        delete vm;
      }

      //delete ui;


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

