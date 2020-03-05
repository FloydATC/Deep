#include <iostream>
#include <fstream>
#include <functional>
#include <regex>
#include <random>

#include <dirent.h>
#include <sys/stat.h>


#include "IO/IODir.h"
#include "IO/IOFile.h"
#include "IO/IOHandle.h"
#include "IO/IOStream.h"
#include "IO/IOSocket.h"
#include "Machine.h"
#include "Fontcache.h"
#include "UTF8hack.h"
#include "hexdump.h" // For debugging only




// https://stackoverflow.com/questions/400257/how-can-i-pass-a-class-member-function-as-a-callback
// Only using high tier black magic
// Because we control when callbacks can occur, we can cheat:
// Each callback can refer to this variable to know which Machine instance to use
static Machine* running = nullptr;

// Initialize random number generator
std::random_device rand_dev;
std::default_random_engine rand_eng(rand_dev());
std::uniform_real_distribution<double> rand_double(0.0, 1.0);




//Machine::Machine(GLuint shaderID, Fontcache fontcache)
Machine::Machine(ShaderProgram* shader, Fontcache fontcache)
{
  //ctor
  initialize_vm();

  this->display = Display(shader, fontcache);
  std::cout << "Virtual Machine " << this << " initialized" << std::endl;

  // Run pre-designated FunC script
  CmdLine parser = CmdLine();
  parser.parse("funos/bin/init.fun");
  execute_file(parser.args_array()[0], &parser);
}

Machine::~Machine()
{
  //dtor
  shutdown_vm();

  std::cout << "Virtual Machine " << this << " destroyed" << std::endl;
}


void Machine::initialize_vm()
{
  if (running != nullptr) { return; }
  vm = FunC::initVM();
  set_callbacks();
  this->mouse_position_rel = Vector2(0, 0);
  add_iohandle(new IOStream()); // "stdin"
  add_iohandle(new IOStream()); // "stdout"
  add_iohandle(new IOStream()); // "stderr"
}

void Machine::shutdown_vm()
{
  if (running != nullptr) { return; }
  FunC::freeVM(vm);
  for (auto& handle: iohandles) {
    if (!handle->is_closed()) handle->close();
    delete handle;
  }
  iohandles.clear();
  flush_msg_queue();
}

void Machine::reset_vm()
{
  if (running != nullptr) { return; }
  shutdown_vm();
  initialize_vm();
}




void Machine::write_to_stdin(const std::string data)
{
  if (iohandles.size() >= 1 && !iohandles[0]->is_closed()) {
    iohandles[0]->write(data);
    std::cout << "Machine::write_to_stdin() wrote '" << data << "' (" << data.length() << " bytes)" << std::endl;
  }
}


void Machine::set_builtin_instance(std::string name, membermap functions)
{
  // The FunC API can create an "instance Value" using
  // - a char** array for field names, and
  // - a Value* array for values
  // From the std::unordered map 'functions', construct those arrays
  std::vector<std::string> keys;
  std::vector<const char*> keys_cstr;
  keys.reserve(functions.size());
  std::vector<FunC::Value> vals;
  vals.reserve(functions.size());

  for(auto key_value : functions) {
    std::string key = key_value.first;
    keys.push_back( key );
    // Create native function object in VM
    FunC::Value fn = to_nativeValue(vm, key.c_str(), (FunC::NativeFn) key_value.second );
    vals.push_back(fn);
  }

  // Convert std::vector of field names to char** array of pointers
  for (int i=0; i<(int)keys.size(); i++) keys_cstr.push_back(const_cast<char*>(keys[i].c_str()));

  // Create instance of an empty class named "*" in VM
  // The class has no methods and serves no purpose (yet)
  // We just want the instance's ability to serve as a namespace.
  FunC::Value instance = FunC::to_instanceValue(vm, keys_cstr.data(), vals.data(), keys.size());

  // Assign it to a name in the global namespace
  FunC::defineGlobal(vm, name.c_str(), instance);
}


// After the FunC VM has been initialized, add callbacks to native functions
// so they become available for the script engine
void Machine::set_callbacks()
{
  membermap members;

  // Callbacks to be usable by FunC script
  FunC::set_error_callback(vm, (FunC::ErrorCb) func_errorCallback);

  // Utility functions (temporary workarounds for language shortcomings)
  FunC::defineNative(vm, "reset",     (FunC::NativeFn) func_reset);
  FunC::defineNative(vm, "str",       (FunC::NativeFn) func_str); // Return argument(s) as string
  FunC::defineNative(vm, "getkey",    (FunC::NativeFn) func_getkey); // Read keyboard input
  FunC::defineNative(vm, "rand",      (FunC::NativeFn) func_rand); // Get random double between 0.0 and 1.0
  // TODO: mouse function should be replaced with a built-in object instance with named properties
  FunC::defineNative(vm, "mouse",     (FunC::NativeFn) func_mouse_pos); // Read mouse position, return array

  // This needs to be a top-level function
  FunC::defineNative(vm, "print",     (FunC::NativeFn) func_print); // Print arguments

  // Screen related functions available under built-in instance 'screen'
  members.clear();
  members.insert({ "clear",     (FunC::NativeFn) func_scr_clear }); // Clear screen or parts of it
  members.insert({ "cursor",    (FunC::NativeFn) func_scr_cursor }); // 0=Disable or 1=Enable cursor
  members.insert({ "autoscroll",(FunC::NativeFn) func_scr_autoscroll }); // 0=Disable or 1=Enable scrolling
  members.insert({ "up",        (FunC::NativeFn) func_scr_up }); // Scroll screen or parts of it
  members.insert({ "down",      (FunC::NativeFn) func_scr_down }); // Scroll screen or parts of it
  members.insert({ "left",      (FunC::NativeFn) func_scr_left }); // Scroll screen or parts of it
  members.insert({ "right",     (FunC::NativeFn) func_scr_right }); // Scroll screen or parts of it
  members.insert({ "fg",        (FunC::NativeFn) func_scr_fg }); // Set foreground color preset
  members.insert({ "bg",        (FunC::NativeFn) func_scr_bg }); // Set background color preset
  members.insert({ "pos",       (FunC::NativeFn) func_scr_pos }); // Set cursor row/col position
  members.insert({ "row",       (FunC::NativeFn) func_scr_row }); // Get cursor row
  members.insert({ "col",       (FunC::NativeFn) func_scr_col }); // Get cursor column
  members.insert({ "rows",      (FunC::NativeFn) func_scr_rows }); // Get number of screen rows
  members.insert({ "cols",      (FunC::NativeFn) func_scr_cols }); // Get number of screen columns
  set_builtin_instance("screen", members);

  // Drawing primitives available under built-in instance 'gl'
  members.clear();
  members.insert({ "width",     (FunC::NativeFn) func_gl_width }); // Return the display width in pixels
  members.insert({ "height",    (FunC::NativeFn) func_gl_height }); // Return the display height in pixels
  members.insert({ "rgb",       (FunC::NativeFn) func_gl_rgb }); // Set RGB color for drawing primitives
  members.insert({ "rect",      (FunC::NativeFn) func_gl_rect }); // Draw empty rectangle
  members.insert({ "area",      (FunC::NativeFn) func_gl_area }); // Draw filled rectangle
  members.insert({ "poly",      (FunC::NativeFn) func_gl_poly }); // Draw filled polygon
  members.insert({ "line",      (FunC::NativeFn) func_gl_line }); // Draw line
  members.insert({ "circle",    (FunC::NativeFn) func_gl_circle }); // Draw empty circle
  members.insert({ "disc",      (FunC::NativeFn) func_gl_disc }); // Draw filled circle
  members.insert({ "point",     (FunC::NativeFn) func_gl_point }); // Draw point (a single pixel)
  set_builtin_instance("gl", members);

  // Disk I/O functions
  FunC::defineNative(vm, "open",      (FunC::NativeFn) func_open); // Open a file
  FunC::defineNative(vm, "eof",       (FunC::NativeFn) func_eof); // Get end-of-file detection status for file
  FunC::defineNative(vm, "opendir",   (FunC::NativeFn) func_opendir); // Open a directory
  FunC::defineNative(vm, "readdir",   (FunC::NativeFn) func_readdir); // Read from an open directory
  // TODO: stat info should be properties of a built-in object instance returned from a single stat() function
  FunC::defineNative(vm, "file_owner",(FunC::NativeFn) func_file_owner); // Return UID for named file
  FunC::defineNative(vm, "file_group",(FunC::NativeFn) func_file_group); // Return UID for named file
  FunC::defineNative(vm, "file_type" ,(FunC::NativeFn) func_file_type); // Return textual type of named file ("file", "directory",...)
  FunC::defineNative(vm, "file_size", (FunC::NativeFn) func_file_size); // Return size (in bytes) of named file
  FunC::defineNative(vm, "file_atime",(FunC::NativeFn) func_file_atime); // Return last access time for named file
  FunC::defineNative(vm, "file_mtime",(FunC::NativeFn) func_file_mtime); // Return last modify time for named file
  FunC::defineNative(vm, "file_ctime",(FunC::NativeFn) func_file_ctime); // Return last create time for named file

  // Network I/O functions
  FunC::defineNative(vm, "connect",   (FunC::NativeFn) func_connect); // Read N bytes from an open file/socket/stream
  FunC::defineNative(vm, "listen",    (FunC::NativeFn) func_listen); // Read N bytes from an open file/socket/stream
  FunC::defineNative(vm, "accept",    (FunC::NativeFn) func_accept); // Read N bytes from an open file/socket/stream
  FunC::defineNative(vm, "send",      (FunC::NativeFn) func_send); // Read N bytes from an open file/socket/stream
  FunC::defineNative(vm, "recv",      (FunC::NativeFn) func_recv); // Read N bytes from an open file/socket/stream

  // Common I/O functions
  FunC::defineNative(vm, "read",      (FunC::NativeFn) func_read); // Read N bytes from an open file/socket/stream
  FunC::defineNative(vm, "readln",    (FunC::NativeFn) func_readln); // Read one line from an open file/socket/stream
  FunC::defineNative(vm, "write",     (FunC::NativeFn) func_write); // Write N bytes to an open file/socket/stream
  FunC::defineNative(vm, "writeln",   (FunC::NativeFn) func_writeln); // Read one line from an open file/socket/stream
  FunC::defineNative(vm, "close",     (FunC::NativeFn) func_close); // Close a file
  FunC::defineNative(vm, "error",     (FunC::NativeFn) func_error); // Get last error code for file (0=success)

}


bool Machine::execute_code(std::string code, std::string filename) {
  display.hide_cursor();
  running = this;
  fc_status = FunC::interpret(vm, code.c_str(), filename.c_str());
  running = nullptr;
  check_fc_status();
  return (fc_status == FunC::INTERPRET_COMPILED);
}

bool Machine::execute_file(std::string fname, CmdLine* parser) {
  IOFile* file = new IOFile();
  std::string buf = file->slurp(fname);
  delete file;

  // TODO: Need error handling

  reset_vm(); // Always start with a clean slate when running a file

  // Define global variable 'args' in VM
  FunC::Value args = FunC::to_stringValueArray(this->vm, parser->args_array(), parser->args_length());
  FunC::defineGlobal(this->vm, "args", args);

  return execute_code(buf, fname);
}

bool Machine::execute_line(std::string line)
{
  CmdLine parser = CmdLine();
  parser.parse(line);
  std::vector<std::string> args = parser.args_vector();
  if (args.size()==0) return true; // Empty line
  std::string cmd = args[0];

  // Try to execute as script
  std::string path = "funos/bin";
  struct dirent* entry;
  struct stat statbuf;
  bool is_file = false;
  DIR* dh = opendir(path.c_str());
  if (dh == NULL) {
    std::cerr << "Error reading directory '" << path << "'" << std::endl;
  } else {
    while ((entry = readdir(dh)) != NULL) {
      stat(entry->d_name, &statbuf);
      if (S_ISDIR(statbuf.st_mode)==false) {
        // Regular file
        //std::cout << "File: '" << entry->d_name << "'" << std::endl;
        if (strcmp(entry->d_name, (cmd+".fun").c_str())==0) { is_file = true; break; }
      }
    }
    closedir(dh);
  }
  if (is_file) {
    // Read, compile and run the script
    return execute_file(path+"/"+cmd+".fun", &parser);
  } else {
    return execute_code(line, "");
  }
}


void Machine::fullscreen_edit(Message* msg)
{
  switch (msg->type)
  {
    case Message::Type::TextInput:
      // Handle printable keys
      display.scroll_right(display.row, display.col, display.row, display.cols-1); // insert
      display.print(msg->text.c_str());
      break;
    case Message::Type::KeyDown: {
      // Printable keys are handled as TextInput so ignore them here
      if (msg->key.sym >= SDLK_a && msg->key.sym <= SDLK_z) { break; }
      if (msg->key.sym >= SDLK_0 && msg->key.sym <= SDLK_9) { break; }

      // Non-printable keys
      switch (msg->key.sym)
      {
        // Cursor control
        case SDLK_HOME:   display.cursor_home();   break;
        case SDLK_END:    display.cursor_end();    break;
        case SDLK_LEFT:   display.cursor_left();   break;
        case SDLK_RIGHT:  display.cursor_right();  break;
        case SDLK_UP:     display.cursor_up();     break;
        case SDLK_DOWN:   display.cursor_down();   break;

        // Editing
        case SDLK_DELETE:
          display.scroll_left(display.row, display.col, display.row, display.cols-1);
          break;
        case SDLK_BACKSPACE:
          display.pos(display.row, display.col-1);
          display.scroll_left(display.row, display.col, display.row, display.cols-1);
          break;

        // Execute command
        case SDLK_RETURN:
        case SDLK_KP_ENTER: {
          std::string line = "";
          for (int c=0; c<display.cols; c++) {
            line += display.char_at(display.row, c); // Note: utf8 encoded character
          }
          display.print("\n");
          execute_line(line);
          break;
        }

        // Other, unhandled
        default:
          std::cout << "Machine() builtin shell ignored " << msg << std::endl;
          break;
      }
      break;
    }
    case Message::Type::KeyUp:
      // Do nothing
      break;
    default:
      std::cerr << "Machine() UNHANDLED: " << msg << std::endl;
  }
}


void Machine::fullscreen_shell()
{
  // This is a very primitive test/rescue shell using a fullscreen editor
  // When no script is running we have to service the message queue
  while (m_queue.empty()==false) {
    Message* msg = m_queue.front();
    m_queue.pop();
    fullscreen_edit(msg);
    delete msg;
  }
}


void Machine::show_prompt()
{
  display.print("READY.\n");
  display.show_cursor(true);
}

void Machine::check_fc_status()
{
  switch (fc_status) {
    case FunC::INTERPRET_OK: // VM has finished executing code
      if (display.col>0) { display.print("\n"); }
      flush_msg_queue();
      show_prompt();
      break;
    case FunC::INTERPRET_COMPILED:
      // No action required
      break;
    case FunC::INTERPRET_RUNNING:
      // No action required
      break;
    case FunC::INTERPRET_COMPILE_ERROR:
      std::cerr << "VM returned INTERPRET_COMPILE_ERROR" << std::endl;
      reset_vm();
      show_prompt();
      break;
    case FunC::INTERPRET_RUNTIME_ERROR:
      std::cerr << "VM returned INTERPRET_RUNTIME_ERROR" << std::endl;
      reset_vm();
      show_prompt();
      break;
    default: {
      // Note sure if we still need this
      std::cerr << "VM returned UNHANDLED fc_status=" << fc_status << std::endl;
      reset_vm();
      show_prompt();
    }
  }
}

FunC::InterpretResult Machine::run()
{
  display.pre_render(); // bind OpenGL texture/framebuffer

  if (fc_status == FunC::INTERPRET_COMPILED || fc_status == FunC::INTERPRET_RUNNING) {
    if (fc_break==true) {
      // User pressed ctrl+c
      // Abort the script by "rebooting" the vm to guarantee a consistent state
      reset_vm();
      fc_status = FunC::INTERPRET_OK;
      display.print("^C\n");
      show_prompt();
    } else {
      running = this;
      fc_status = FunC::run(vm); // Run the VM for a few milliseconds (parameterize this?)
      running = nullptr;
      check_fc_status();
    }
  } else {
    // No script running so we need to handle events
    fullscreen_shell();
  }
  fc_break = false;

  display.post_render(); // unbind OpenGL texture/framebuffer
  return fc_status;
}

void Machine::flush_msg_queue()
{
  Message* msg = nullptr;
  while (m_queue.empty()==false) {
    msg = m_queue.front();
    m_queue.pop();
    delete msg;
  }
}


void Machine::handle_msg_quit(Message* msg)
{
  this->fc_break = true;
  delete msg;
}

void Machine::handle_msg_mousemotion(Message* msg)
{
  Vector2 curr = Vector2(msg->motion.x, msg->motion.y);
  Vector2 relative = curr - this->mouse_position_abs;
  this->mouse_position_abs = curr;  // Current position
  this->mouse_position_rel += relative; // Motion since last read
  delete msg;
}

void Machine::push(Message* msg)
{
  // Some message types are handled in "hardware"
  switch (msg->type) {
    case Message::Type::Break: handle_msg_quit(msg); break;
    case Message::Type::MouseMotion: handle_msg_mousemotion(msg); break;
    default: m_queue.push(msg); // Handled in "software"
  }
}


uint16_t Machine::add_iohandle(IOHandle* ptr)
{
  // Find an available handle (one that has been closed, or a new one)
  //std::cout << "Machine::add_iohandle() ptr=" << ptr << std::endl;
  for (uint16_t i=0; i<iohandles.size(); i++) {
    if (iohandles[i]->is_closed()) {
      delete iohandles[i];
      iohandles[i] = ptr;
      return i;
    }
  }
  // No re-usable handles so append one
  iohandles.push_back(ptr);
  return iohandles.size()-1;
}







// STATIC callbacks depend on the static variable "current" instead of "this"
// because FunC is written in C and doesn't have the foggest faintest idea what "this" is.
// And it doesn't have to, because only one FunC VM will ever be running at any point in time
// as long as we don't do anything colossaly stupid like try to run them in different threads.

// FunC calls this to print runtime errors
void Machine::func_errorCallback(const char* errormsg) {
  if (running==nullptr) {
    std::cerr << "Machine::func_errorCallback() called while running==nullptr" << std::endl;
    return;
  }
  running->display.print(errormsg);
}


// Implement the very latest in looping and rasterizing technology
// to facilitate the visual rendition of literal constructs for the
// cognitive consumption of information via visual means.
// Print stuff. On the screen.
// Synopsis: print("Hello world\n", 3.14, foo); // Any number of arguments
// Note: Only the LAST 4000 characters in a string get printed, based on the
// assumption that any characters before that would have scrolled off the screen anyway
// Also note: Strings are assumed to contain valid UTF8 sequences
// (extended ASCII -> bad stuff may happen)
bool Machine::func_print(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  for (int i=0; i<argc; i++) {
    running->display.print(FunC::to_cstring(argv[i]));
  }
  *result = FunC::to_numberValue(1);
  return true;
}


// Return and reset the current mouse position as an array
bool Machine::func_mouse_pos(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  double numbers[4];
  numbers[0] = running->mouse_position_abs.x;
  numbers[1] = running->mouse_position_abs.y;
  numbers[2] = running->mouse_position_rel.x;
  numbers[3] = running->mouse_position_rel.y;
  *result = FunC::to_numberValueArray(running->vm, numbers, 4);
  running->mouse_position_rel = Vector2(0, 0);
  return true;
}





// Concatenate multiple arguments into a single string
// Currently supports only numbers and strings, and you can't even control
// how numbers are formatted. This function is likely to change in the future so YMMV
bool Machine::func_str(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  std::string buf = "";
  for (int i=0; i<argc; i++) {
    buf += FunC::to_cstring(argv[i]);
  }
  *result = FunC::to_stringValue(running->vm, buf.c_str());
  return true;
}

// Clear screen, or part of it as indicated as follows
// Synopsis: screen.clear();      // Clear entire screen (0 arguments)
// Synopsis: ...(row);            // Clear only one row (1 argument)
// Synopsis: ...(r1,r2);          // Clear rows r1 thru r2 (2 arguments)
// Synopsis: ...(r1,r2,cp);       // Clear rows r1 thru r2 using the specified codepoint (3 arguments)
// Synopsis: ...(r1,c1,r2,c2);    // Clear a rectangular area of the screen (4 arguments)
// Synopsis: ...(r1,c1,r2,c2,cp); // ...using the specified codepoint (5 arguments)
// Obviously, where no codepoint is specified, the default is 32 (blank space)
bool Machine::func_scr_clear(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  int r1;
  int r2;
  int c1 = 0;
  int c2 = running->display.cols-1;
  int codepoint;
  switch (argc) {
    case 0:
      running->display.cls();
      break;
    case 1:
      r1 = (int) FunC::to_double(argv[0]);
      running->display.cls(r1, 0, r1, c2);
      break;
    case 2:
      r1 = (int) FunC::to_double(argv[0]);
      r2 = (int) FunC::to_double(argv[1]);
      running->display.cls(r1, 0, r2, c2);
      break;
    case 3:
      r1 = (int) FunC::to_double(argv[0]);
      r2 = (int) FunC::to_double(argv[1]);
      codepoint = (int) FunC::to_double(argv[2]);
      running->display.cls(r1, 0, r2, c2, codepoint);
      break;
    case 4:
      r1 = (int) FunC::to_double(argv[0]);
      c1 = (int) FunC::to_double(argv[1]);
      r2 = (int) FunC::to_double(argv[2]);
      c2 = (int) FunC::to_double(argv[3]);
      running->display.cls(r1, c1, r2, c2);
      break;
    case 5:
      r1 = (int) FunC::to_double(argv[0]);
      c1 = (int) FunC::to_double(argv[1]);
      r2 = (int) FunC::to_double(argv[2]);
      c2 = (int) FunC::to_double(argv[3]);
      codepoint = (int) FunC::to_double(argv[4]);
      running->display.cls(r1, c1, r2, c2, codepoint);
      break;
    default:
      // There's no satisfying some people.
      *result = FunC::to_numberValue(-1);
      return false;
  }
  *result = FunC::to_numberValue(1);
  return true;
}


// Reset the foreground and background color, cursor state and position,
// clear the screen and then finally execute "init.fun"
bool Machine::func_reset(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  running->display.reset();
  CmdLine parser = CmdLine();
  parser.parse("funos/bin/init.fun");
  running->execute_file(parser.args_array()[0], &parser);
  *result = FunC::to_numberValue(1);
  return true;
}


// Return a random double between 0.0 and 1.0
// Typical use is to multiply this number with the desired range
// Synopsis: percent = rand()*100
bool Machine::func_rand(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue( rand_double(rand_eng) );
  return true;
}

// Get the next single character (or unprintable key) from the VM event queue,
// this means reading keyboard input from the user (if any)
// Synopsis: key = getkey()
// Non-printable keys are translated into a multi-character string with "[Key name]"
// The user script may differentiate between printable and
// non-printable keys using the chars() function:
// length 0 = No input
// length 1 = Single printable character such as "a", "1" or "?"
// length >1 = Unprintable key or combination like [Return], [Backspace] or [Ctrl Insert]
// Note: This function will never block
bool Machine::func_getkey(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // string = getc()
  // Return 1 UTF8 character (or "" if none are waiting)
  std::string buf = "";
  Message* msg;
  while (running->m_queue.empty()==false) {
    //std::cout << "Machine.func_getc() queue not empty" << std::endl;
    msg = running->m_queue.front();
    running->m_queue.pop();
    switch (msg->type)
    {
      case Message::Type::TextInput:
        buf = msg->text;
        break;
      case Message::Type::KeyDown: {
        //std::cout << "VM.read() KeyDown sym=" << msg->key.sym << std::endl;
        switch (msg->key.sym) {
          default:
            std::string name = SDL_GetKeyName(msg->key.sym);
            if (name != "Left Alt" && name != "Right Alt") {
              if (msg->key.mod & KMOD_ALT) { name = "Alt " + name; }
            }
            if (name != "Left Ctrl" && name != "Right Ctrl") {
              if (msg->key.mod & KMOD_CTRL) { name = "Ctrl " + name; }
            }
            if (UTF8hack::u8_strlen((char*)name.c_str())>1) {
              if (name != "Left Shift" && name != "Right Shift") {
                if (msg->key.mod & KMOD_SHIFT) { name = "Shift " + name; }
              }
              //std::cout << "VM.read() KeyDown name=" << name << std::endl;
              buf = "["+name+"]";
            }
            break;
        }
        break;
      }
      case Message::Type::KeyUp:
        break;
      default:
        std::cout<<"VM.read() discarding " << msg <<std::endl;
    }
    delete msg;
    if (buf!="") { break; }
  }
  //std::cout << "Machine.func_getc() converting '" << buf << "' as " << (void *) buf.c_str() << " to Value" << std::endl;
  *result = FunC::to_stringValue(running->vm, buf.c_str());
  return true;
}



// Hide or show blinking cursor
// Synopsis: cursor(0) // Hide
// Synopsis: cursor(1) // Show
// Note: Repeated calls "stack" so two calls to "hide"
// must be followed by two calls to "show"
// for the cursor to re-appear
bool Machine::func_scr_cursor(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  if (FunC::to_double(argv[0]) == 0) {
    running->display.hide_cursor();
  } else {
    running->display.show_cursor();
  }
  *result = FunC::to_numberValue(1);
  return true;
}

// Enable or disable autoscrolling when printing past the bottom line
// If autoscrolling is disabled, the cursor instead wraps around to column 0
// Synopsis: autoscroll(0) // Disable
// Synopsis: autoscroll(1) // Enable
bool Machine::func_scr_autoscroll(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  if (FunC::to_double(argv[0]) == 0) {
    running->display.disable_autoscroll();
  } else {
    running->display.enable_autoscroll();
  }
  *result = FunC::to_numberValue(1);
  return true;
}

// Scroll up by one row
// With 0 arguments, scroll the entire screen
// Synopsis: scr_up()
// With 4 arguments, scroll only the area r1,c1,r2,c2
// Synopsis: scr_up(r1,c1,r2,c2)
bool Machine::func_scr_up(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 0) {
    running->display.scroll_up();
    *result = FunC::to_numberValue(1);
    return true;
  }
  if (argc == 4) {
    int r1 = FunC::to_double(argv[0]);
    int c1 = FunC::to_double(argv[1]);
    int r2 = FunC::to_double(argv[2]);
    int c2 = FunC::to_double(argv[3]);
    running->display.scroll_up(r1, c1, r2, c2);
    *result = FunC::to_numberValue(1);
    return true;
  }
  *result = FunC::to_numberValue(-1);
  return false;
}

// Scroll down by one row
// Otherwise same as scr_up()
bool Machine::func_scr_down(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 0) {
    running->display.scroll_down();
    *result = FunC::to_numberValue(1);
    return true;
  }
  if (argc == 4) {
    int r1 = FunC::to_double(argv[0]);
    int c1 = FunC::to_double(argv[1]);
    int r2 = FunC::to_double(argv[2]);
    int c2 = FunC::to_double(argv[3]);
    running->display.scroll_down(r1, c1, r2, c2);
    *result = FunC::to_numberValue(1);
    return true;
  }
  *result = FunC::to_numberValue(-1);
  return false;
}

// Scroll left by one column
// Otherwise same as scr_up()
bool Machine::func_scr_left(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 0) {
    running->display.scroll_left();
    *result = FunC::to_numberValue(1);
    return true;
  }
  if (argc == 4) {
    int r1 = FunC::to_double(argv[0]);
    int c1 = FunC::to_double(argv[1]);
    int r2 = FunC::to_double(argv[2]);
    int c2 = FunC::to_double(argv[3]);
    running->display.scroll_left(r1, c1, r2, c2);
    *result = FunC::to_numberValue(1);
    return true;
  }
  *result = FunC::to_numberValue(-1);
  return false;
}

// Scroll right by one column
// Otherwise same as scr_up()
bool Machine::func_scr_right(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 0) {
    running->display.scroll_right();
    *result = FunC::to_numberValue(1);
    return true;
  }
  if (argc == 4) {
    int r1 = FunC::to_double(argv[0]);
    int c1 = FunC::to_double(argv[1]);
    int r2 = FunC::to_double(argv[2]);
    int c2 = FunC::to_double(argv[3]);
    running->display.scroll_right(r1, c1, r2, c2);
    *result = FunC::to_numberValue(1);
    return true;
  }
  *result = FunC::to_numberValue(-1);
  return false;
}

// Set the current foreground (text) color to one of the 16 color presets (0-15)
bool Machine::func_scr_fg(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 1) {
    running->display.set_fgcolor((int) FunC::to_double(argv[0]));
  }
  *result = FunC::to_numberValue(running->display.fgcolor);
  return true;
}


// Set the current background (text) color to one of the 16 color presets (0-15)
bool Machine::func_scr_bg(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 1) {
    running->display.set_bgcolor((int) FunC::to_double(argv[0]));
  }
  *result = FunC::to_numberValue(running->display.bgcolor);
  return true;
}

// Place the text cursor (and thereby control where the next print() text will appear
// at the specified row and column
// Synopsis: pos(row, column)
// Note: The upper left cursor position is 0,0
bool Machine::func_scr_pos(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  int r = (int) FunC::to_double(argv[0]);
  int c = (int) FunC::to_double(argv[1]);
  //std::cout << "invoking display.pos() with r=" << r << " c=" << c << std::endl;
  running->display.pos(r, c);
  *result = FunC::to_numberValue(1);
  return true;
}

// Return the current row position of the text cursor
// Synopsis: current = row()
// Note: The top row is number 0
bool Machine::func_scr_row(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.row);
  return true;
}

// Return the current column position of the text cursor
// Synopsis: current = col()
// Note: The leftmost column is number 0
bool Machine::func_scr_col(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.col);
  return true;
}

// Return the number of visible rows on the screen
// Synopsis: bottom = rows()-1;
bool Machine::func_scr_rows(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.rows);
  return true;
}

// Return the number of visible columns on the screen
// Synopsis: right = cols()-1;
bool Machine::func_scr_cols(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.cols);
  return true;
}


// Return width of the display in pixels
bool Machine::func_gl_width(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.width);
  return true;
}

// Return height of the display in pixels
bool Machine::func_gl_height(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.height);
  return true;
}


// Set RGB color for drawing primitives
bool Machine::func_gl_rgb(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 3) {
    float r = (float) FunC::to_double(argv[0]);
    float g = (float) FunC::to_double(argv[1]);
    float b = (float) FunC::to_double(argv[2]);
    running->display.set_rgbcolor(r, g, b);
  }
  *result = FunC::to_numberValue(running->display.fgcolor);
  return true;
}



// Render a line rectangle using the current foreground color
// starting at coordinates x,y with width w and height h
// Compare and contrast with area()
// Synopsis: rect(x,y,w,h);
// Note: With 40x25 characters at 8x8 pixels, the display resolution is 320x200
// Warning: By moronic design, OpenGL may OMIT the first and/or last pixel
bool Machine::func_gl_rect(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // rect(x, y, w, h);
  if (argc != 4) { *result = FunC::to_numberValue(-1); return false; }
  int x = (int) FunC::to_double(argv[0]);
  int y = (int) FunC::to_double(argv[1]);
  int w = (int) FunC::to_double(argv[2]);
  int h = (int) FunC::to_double(argv[3]);
  running->display.draw_rect(x, y, w, h);
  *result = FunC::to_numberValue(1);
  return true;
}

// Render a filled rectangle using the current foreground color
// starting at coordinates x,y with width w and height h
// Compare and contrast with rect()
// Synopsis: area(x,y,w,h);
// Note: With 40x25 characters at 8x8 pixels, the display resolution is 320x200
bool Machine::func_gl_area(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // area(x, y, w, h); // filled rect
  if (argc != 4) { *result = FunC::to_numberValue(-1); return false; }
  int x = (int) FunC::to_double(argv[0]);
  int y = (int) FunC::to_double(argv[1]);
  int w = (int) FunC::to_double(argv[2]);
  int h = (int) FunC::to_double(argv[3]);
  running->display.draw_area(x, y, w, h);
  *result = FunC::to_numberValue(1);
  return true;
}


// Render a filled polygon using the current foreground color
// between three points x1,y1 - x2,y2 - x3,y3
// Synopsis: poly(x1,y1,x2,y2,x3,y3);
// Note: With 40x25 characters at 8x8 pixels, the display resolution is 320x200
bool Machine::func_gl_poly(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // poly(x1,y1,x2,y2,x3,y3); // filled polygon
  if (argc != 6) { *result = FunC::to_numberValue(-1); return false; }
  int x1 = (int) FunC::to_double(argv[0]);
  int y1 = (int) FunC::to_double(argv[1]);
  int x2 = (int) FunC::to_double(argv[2]);
  int y2 = (int) FunC::to_double(argv[3]);
  int x3 = (int) FunC::to_double(argv[4]);
  int y3 = (int) FunC::to_double(argv[5]);
  running->display.draw_poly(x1, y1, x2, y2, x3, y3);
  *result = FunC::to_numberValue(1);
  return true;
}


// Render a single line using the current foreground color
// from coordinate x1,y1 to x2,y2
// Synopsis: area(x1,y1,x2,y2);
// Warning: By moronic design, OpenGL may OMIT the first and/or last pixel
bool Machine::func_gl_line(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // line(x1, y1, x2, y2);
  if (argc != 4) { *result = FunC::to_numberValue(-1); return false; }
  int x1 = (int) FunC::to_double(argv[0]);
  int y1 = (int) FunC::to_double(argv[1]);
  int x2 = (int) FunC::to_double(argv[2]);
  int y2 = (int) FunC::to_double(argv[3]);
  //std::cout << "Machine::func_line() x1=" << x1 << " y1=" << y1 << " x2=" << x2 << " y2=" << y2 << std::endl;
  running->display.draw_line(x1, y1, x2, y2);
  *result = FunC::to_numberValue(1);
  return true;
}


// Render an empty circle using the current foreground color
// centered at coordinate x,y and with radius r
// Synopsis: circle(x,y,r);
bool Machine::func_gl_circle(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // circle(x, y, r);
  if (argc != 3) { *result = FunC::to_numberValue(-1); return false; }
  int x = (int) FunC::to_double(argv[0]);
  int y = (int) FunC::to_double(argv[1]);
  int r = (int) FunC::to_double(argv[2]);
  //std::cout << "Machine::func_circle() x=" << x << " y=" << y << " r << std::endl;
  running->display.draw_circle(x, y, r);
  *result = FunC::to_numberValue(1);
  return true;
}


// Render an filled circle using the current foreground color
// centered at coordinate x,y and with radius r
// Synopsis: disc(x,y,r);
bool Machine::func_gl_disc(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // disc(x, y, r);
  if (argc != 3) { *result = FunC::to_numberValue(-1); return false; }
  int x = (int) FunC::to_double(argv[0]);
  int y = (int) FunC::to_double(argv[1]);
  int r = (int) FunC::to_double(argv[2]);
  //std::cout << "Machine::func_disc() x=" << x << " y=" << y << " r << std::endl;
  running->display.draw_disc(x, y, r);
  *result = FunC::to_numberValue(1);
  return true;
}




// Render a single pixel using the current foreground color
// at coordinate x,y
// Synopsis: point(x,y);
// Note: This is obviously the least efficient way possible to draw something
// but sometimes you just need that one pixel in the right spot, right?
bool Machine::func_gl_point(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // point(x, y);
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  int x = (int) FunC::to_double(argv[0]);
  int y = (int) FunC::to_double(argv[1]);
  //std::cout << "Machine::func_point() x=" << x << " y=" << y << std::endl;
  running->display.draw_point(x, y);
  *result = FunC::to_numberValue(1);
  return true;
}




// Open a file for reading or writing
// Synopsis: var fh = open("path/to/filename", "r");
// Synopsis: var fh = open("path/to/filename", "w");
// Synopsis: var fh = open("path/to/filename", "a");
// File modes match those of <cstdio> fopen()
// Note: This function will ALWAYS return a handle, detect errors with error(fh)!=0
bool Machine::func_open(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  std::string filename = FunC::to_cstring(argv[0]);
  std::string mode = FunC::to_cstring(argv[1]);
  //std::cout << "Machine::func_open() filename=" << filename << " mode=" << mode << std::endl;
  IOFile* file = IOFile::open(filename, mode);
  int handle = running->add_iohandle(file);
  *result = FunC::to_numberValue(handle);
  //std::cout << "Machine::func_open() handle=" << handle << std::endl;
  return true;
}

// Attempt to read from an open file handle
// Synopsis: var buffer = read(fh, 1024);
bool Machine::func_read(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  int bytes = (int) FunC::to_double(argv[1]);
  //std::cout << "Machine::func_read() handle=" << handle << " bytes=" << bytes << std::endl;
  std::string res = running->iohandles[handle]->read(bytes);
  *result = FunC::to_stringValue(running->vm, res.c_str());
  return true;
}

// Attempt to read from an open handle until '\n' is found or
// end-of-file is detected.
// Note: This operation MAY return emptystring "" for very long lines
// (>4096 bytes) which may require multiple readln() // calls.
// If this is the case, error(fh) will return EAGAIN (code 3406)
// Synopsis: while (!eof(fh)) { var line = readln(fh); }
// WARNING! The following code is UNSAFE:
//   while (line = readln()) { ... } // May end prematurely for very long lines!
bool Machine::func_readln(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  //std::cout << "Machine::func_readln() handle=" << handle << std::endl;
  std::string res = running->iohandles[handle]->readln();
  *result = FunC::to_stringValue(running->vm, res.c_str());
  return true;
}

// Attempt to write to an open file handle
// Synopsis: var bytes_written = write(fh, buffer);
bool Machine::func_write(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  std::string buffer = FunC::to_cstring(argv[1]);
  //std::cout << "Machine::func_write() handle=" << handle << " bytes=" << buffer.length() << std::endl;
  int bytes_written = running->iohandles[handle]->write(buffer);
  *result = FunC::to_numberValue(bytes_written);
  return true;
}

// Attempt to write to an open file handle,
// adding a platform dependent newline character sequence
// ("\r\n" on Windows, "\n" elsewhere)
// Synopsis: var bytes_written = writeln(fh, buffer);
// Note: bytes_written INCLUDES the newline character sequence
bool Machine::func_writeln(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  std::string buffer = FunC::to_cstring(argv[1]);
  //std::cout << "Machine::func_writeln() handle=" << handle << " bytes=" << buffer.length() << std::endl;
  int bytes_written = running->iohandles[handle]->writeln(buffer);
  *result = FunC::to_numberValue(bytes_written);
  return true;
}

// Return true if end-of-file has been detected
// Synopsis: while (!eof(fh)) { var line = readln(fh); }
bool Machine::func_eof(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  //std::cout << "Machine::func_eof() handle=" << handle << std::endl;
  int res = running->iohandles[handle]->is_eof() ? 1 : 0;
  *result = FunC::to_numberValue(res); return true;
}

// Return the last error code detected
// Multiple reads do not reset the code
// Synopsis: if (error(fh)) { print(error(fh)); }
bool Machine::func_error(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  //std::cout << "Machine::func_close() handle=" << handle << std::endl;
  int res = running->iohandles[handle]->last_error();
  *result = FunC::to_numberValue(res); return true;
}

// Close a file, directory or socket handle
bool Machine::func_close(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  //std::cout << "Machine::func_close() handle=" << handle << std::endl;
  running->iohandles[handle]->close();
  *result = FunC::to_numberValue(1); return true;
}

// Open a directory for reading
// Synopsis: var dh=opendir("/"); var fname; while (fname=readdir(dh)) { print(fname,"\n"); }
bool Machine::func_opendir(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  //std::cout << "Machine::func_opendir() argc=" << argc << std::endl;
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* dirname = FunC::to_cstring(argv[0]);
  //std::cout << "Machine::func_opendir() name=" << dirname << std::endl;
  IODir* dir = IODir::open(dirname);
  *result = FunC::to_numberValue(running->add_iohandle(dir));
  //std::cout << "Machine::func_opendir() handle=" << running->iohandles.size() << std::endl;
  return true;
}

// Read one filename from an open directory
// Returns emptystring "" if there are no more files
// Synopsis: var dh=opendir("/"); var fname; while (fname=readdir(dh)) { print(fname,"\n"); }
bool Machine::func_readdir(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  //std::cout << "Machine::func_readdir() handle=" << handle << std::endl;
  if (running->iohandles[handle]->last_error() == 0 && running->iohandles[handle]->is_closed() == false) {
    //std::cout << "Machine::func_readdir(" << handle << ")...";
    std::string res = running->iohandles[handle]->read();
    //std::cout << "done" << std::endl;
    *result = FunC::to_stringValue(running->vm, res.c_str());
  } else {
    *result = FunC::to_stringValue(running->vm, "");
  }
  return true;
}


bool Machine::func_file_owner(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  IOFile* file = IOFile::stat(filename);
  if (file->last_error() == 0) {
    *result = FunC::to_numberValue(file->uid());
  } else {
    *result = FunC::to_numberValue(-1);
  }
  delete file;
  return true;
}


bool Machine::func_file_group(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  IOFile* file = IOFile::stat(filename);
  if (file->last_error() == 0) {
    *result = FunC::to_numberValue(file->gid());
  } else {
    *result = FunC::to_numberValue(-1);
  }
  delete file;
  return true;
}


bool Machine::func_file_type(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  //std::cout << "Machine::func_file_type(" << filename << ")...";
  IOFile* file = IOFile::stat(filename);
  //std::cout << "ok" << std::endl;
  std::string res = "unknown";
  if (file->last_error() == 0) {
    int filemode = file->mode();
    if (S_ISREG(filemode)) res = "file";
    if (S_ISDIR(filemode)) res = "directory";
    *result = FunC::to_stringValue(vm, res.c_str());
    //std::cout << "Machine::func_file_type() type=" << res << std::endl;
  } else {
    *result = FunC::to_numberValue(-1);
    //std::cout << "Machine::func_file_type() stat failed" << std::endl;
  }
  delete file;
  //std::cout << "Machine::func_file_type() returning" << std::endl;
  return true;
}


bool Machine::func_file_size(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  IOFile* file = IOFile::stat(filename);
  if (file->last_error() == 0) {
    *result = FunC::to_numberValue(file->size());
  } else {
    *result = FunC::to_numberValue(-1);
  }
  delete file;
  return true;
}


bool Machine::func_file_atime(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  IOFile* file = IOFile::stat(filename);
  if (file->last_error() == 0) {
    *result = FunC::to_numberValue(file->atime());
  } else {
    *result = FunC::to_numberValue(-1);
  }
  delete file;
  return true;
}


bool Machine::func_file_mtime(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  IOFile* file = IOFile::stat(filename);
  if (file->last_error() == 0) {
    *result = FunC::to_numberValue(file->mtime());
  } else {
    *result = FunC::to_numberValue(-1);
  }
  delete file;
  return true;
}


bool Machine::func_file_ctime(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  char* filename = FunC::to_cstring(argv[0]);
  IOFile* file = IOFile::stat(filename);
  if (file->last_error() == 0) {
    *result = FunC::to_numberValue(file->ctime());
  } else {
    *result = FunC::to_numberValue(-1);
  }
  delete file;
  return true;
}


bool Machine::func_connect(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 3) { *result = FunC::to_numberValue(-1); return false; }
  std::string host = FunC::to_cstring(argv[0]);
  std::string port = FunC::to_cstring(argv[1]);
  std::string protocol = FunC::to_cstring(argv[2]);

  IOSocket* socket = IOSocket::connect(host, port, protocol);

  *result = FunC::to_numberValue(running->add_iohandle(socket));
  return true;
}

bool Machine::func_listen(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  std::string port = FunC::to_cstring(argv[0]);
  std::string protocol = FunC::to_cstring(argv[1]);

  IOSocket* socket = IOSocket::listen(port, protocol);

  *result = FunC::to_numberValue(running->add_iohandle(socket));
  return true;
}

bool Machine::func_accept(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);

  IOSocket* listener = (IOSocket*) running->iohandles[handle];
  IOSocket* socket = listener->accept();

  if (socket == nullptr) {
    // If accept() failed, it usually just means no client was waiting
    // If an error has occurred, it is flagged on the server socket
    *result = FunC::to_numberValue(0);
  } else {
    *result = FunC::to_numberValue(running->add_iohandle(socket));
  }
  return true;
}

bool Machine::func_send(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 2) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  std::string data = FunC::to_cstring(argv[1]);

  int bytes_sent = running->iohandles[handle]->send(data);

  *result = FunC::to_numberValue(bytes_sent);
  return true;
}

bool Machine::func_recv(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);

  std::string data = running->iohandles[handle]->recv();

  *result = FunC::to_stringValue(running->vm, data.c_str());
  return true;
}

