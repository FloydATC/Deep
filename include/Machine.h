#ifndef MACHINE_H
#define MACHINE_H

namespace FunC {
  extern "C" {
    #include <vm.h>
  }
}

#include <queue>

#include <SDL_ttf.h>

#include "Display.h"
#include "GFX.h"
#include "IO/IOHandle.h"
#include "Message.h"
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

class Machine
{
  public:

//    Machine(GLuint shaderID, Fontcache fontcache);
    Machine(ShaderProgram* shader, Fontcache fontcache);
    ~Machine();

    void handle_msg_quit(Message* msg);
    void handle_msg_mousemotion(Message* msg);
    void push(Message* msg);
    bool execute_code(std::string code);
    bool execute_file(std::string fname, std::string arguments);
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
    static bool func_reset     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_str       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_getkey    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_rand      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_mouse_x   (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_mouse_y   (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_mouse_relx(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_mouse_rely(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

    // Display functions
    static bool func_print     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_cls       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_cursor    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_autoscroll(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_up    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_down  (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_left  (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_scr_right (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_fg        (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_bg        (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_rgb       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_pos       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_row       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_col       (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_rows      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_cols      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_rect      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_area      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_poly      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_line      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_circle    (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_disc      (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);
    static bool func_point     (FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result);

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

    Vector2 mouse_position_abs;
    Vector2 mouse_position_rel;

    FunC::VM* vm = nullptr;

    FunC::InterpretResult fc_status;
    std::queue<Message*> m_queue;
    std::vector<IOHandle*> iohandles;
    uint16_t add_iohandle(IOHandle* ptr);

    void flush_msg_queue();
    void initialize_vm();
    void shutdown_vm();
    void reset_vm();
    void set_callbacks();
};

#endif // MACHINE_H
