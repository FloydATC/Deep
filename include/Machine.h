#ifndef MACHINE_H
#define MACHINE_H

namespace FunC {
  extern "C" {
#include <vm.h>
  }
}

#pragma comment(lib, "FunCx64.lib")

#include <queue>

#include <SDL_ttf.h>

#include "CmdLine.h"
#include "Display.h"
#include "GFX.h"
#include "IO/IOHandle.h"
#include "Message.h"
#include "Dev/Mouse.h"
#include "ShaderProgram.h"
#include "Vectors.h"


/*

  This class provides a runtime environment for the FunC script VM
  (bytecode compiler + interpreter) using the following emulated "hardware":
  - a "display": Display.h provides an OpenGL texture and function calls for
    drawing primitives, printing text and other things display related.
  - an "operating system": IOHandles.h (and subclasses) provide streams for
    file/directory, socket communication and simulated standard input/output.

*/

typedef std::unordered_map<std::string,FunC::NativeFn> MemberMap;
typedef std::unordered_map<std::string,FunC::Value> ValueMap;


class Machine
{
  public:

//    Machine(GLuint shaderID, Fontcache fontcache);
    Machine(ShaderProgram* shader, Fontcache fontcache);
    ~Machine();

    void push(Message* msg);
    bool execute_code(std::string code, std::string filename);
    bool execute_file(std::string fname, CmdLine* parser);
    bool execute_line(std::string line);
    void write_to_stdin(const std::string data);

    Display display;

    void fullscreen_edit(Message* msg);
    void fullscreen_shell();
    void show_prompt();
    void check_fc_status();
    FunC::InterpretResult run();

    static void func_errorCallback(const char* errormsg);

    // Utility functions (temporary workarounds for language shortcomings)
    static bool func_reset         (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_str           (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_getkey        (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_rand          (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_mouse         (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);


    static bool func_print         (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

    // Display functions
    static bool func_scr_clear     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_cursor    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_autoscroll(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_up        (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_down      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_left      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_right     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_fg        (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_bg        (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_pos       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_row       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_col       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_rows      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_cols      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

    static bool func_gl_width     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_height    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_rgb       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_rect      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_area      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_poly      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_line      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_circle    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_disc      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_gl_point     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

    // Disk I/O functions
    static bool func_open      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_eof       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_opendir   (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_readdir   (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_owner(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_group(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_type (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_size (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_atime(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_mtime(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_file_ctime(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

    // Network I/O functions
    static bool func_connect   (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_listen    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_accept    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_send      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_recv      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

    // Common I/O functions
    static bool func_read      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_readln    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_write     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_writeln   (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_error     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_close     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

  protected:

  private:
    bool fc_break = false; // Ctrl+C detected

    Mouse* mouse;
    //Vector2 mouse_position_abs;
    //Vector2 mouse_position_rel;

    FunC::VM* vm = nullptr;

    FunC::InterpretResult fc_status;
    std::queue<Message*> m_queue;
    std::vector<IOHandle*> iohandles;
    uint16_t add_iohandle(IOHandle* ptr);

    void flush_msg_queue();
    void initialize_vm();
    void shutdown_vm();
    void reset_vm();
    FunC::Value make_builtin_instance(ValueMap values);
    void set_builtin_instance(std::string name, MemberMap functions);
    void set_callbacks();
    void handle_msg_quit(Message* msg);
    void handle_msg_mousemotion(Message* msg);
    void handle_msg_mousebutton(Message* msg);

};

#endif // MACHINE_H
