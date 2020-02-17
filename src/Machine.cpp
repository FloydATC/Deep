#include <iostream>
#include <fstream>
#include <functional>
#include <regex>
#include <random>

#include <dirent.h>
#include <sys/stat.h>

#include "IODir.h"
#include "IOFile.h"
#include "IOHandle.h"
#include "IOStream.h"
#include "IOSocket.h"
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




Machine::Machine(GLuint shaderID, Fontcache fontcache)
{
  //ctor
  initialize_vm();

  this->display = Display(shaderID, fontcache);
  std::cout << "Virtual Machine " << this << " initialized" << std::endl;

  // Run pre-designated FunC script
  execute_file("funos/bin/init.fun", "");
}

Machine::~Machine()
{
  //dtor
  shutdown_vm();

  std::cout << "Virtual Machine " << this << " destroyed" << std::endl;
}


// Split cmd into two separate strings;
// the first string should be the name of an executable
// the second string should be any parameters passed as arguments to that executable
// Note that the executable may contain spaces, in which case the string is "quoted"
void split_cmd( const std::string& cmd, std::string* executable, std::string* parameters )
{
  std::string c( cmd );
  size_t exec_end;
  if( c[ 0 ] == '\"' ) {
    // The name is quoted; look for the end quotes
    exec_end = c.find_first_of( '\"', 1 );
    if( std::string::npos != exec_end ) {
      *executable = c.substr( 1, exec_end - 1 );
      *parameters = c.substr( exec_end + 1 );
    } else {
      *executable = c.substr( 1, exec_end );
      std::string().swap( *parameters );
    }
  } else {
    // The name is not quoted so look for a space
    exec_end = c.find_first_of( ' ', 0 );
    if( std::string::npos != exec_end ) {
      *executable = c.substr( 0, exec_end );
      *parameters = c.substr( exec_end + 1 );
    } else {
      *executable = c.substr( 0, exec_end );
      std::string().swap( *parameters );
    }
  }
}

void Machine::initialize_vm()
{
  if (running != nullptr) { return; }
  //std::cout << "Machine::init_vm() initializing new vm" << std::endl;
  vm = FunC::initVM();
  //std::cout << "Machine::initialize_vm() setting callbacks" << std::endl;
  set_callbacks();
  //std::cout << "Machine::initialize_vm() create stdin" << std::endl;
  add_iohandle(new IOStream()); // "stdin"
  //std::cout << "Machine::initialize_vm() create stdout" << std::endl;
  add_iohandle(new IOStream()); // "stdout"
  //std::cout << "Machine::initialize_vm() create stderr" << std::endl;
  add_iohandle(new IOStream()); // "stderr"
  //std::cout << "Machine::initialize_vm() " << (void*)vm << " ready" << std::endl;
}

void Machine::shutdown_vm()
{
  if (running != nullptr) { return; }
  //std::cout << "Machine::shutdown_vm() shutting down vm=" << (void*)vm << std::endl;
  FunC::freeVM(vm);
  for (auto& handle: iohandles) {
    //std::cout << "Machine::shutdown_vm() close IOHandle " << handle << std::endl;
    if (!handle->is_closed()) handle->close();
    //std::cout << "Machine::shutdown_vm() delete IOHandle " << handle << std::endl;
    delete handle;
  }
  iohandles.clear();
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

// After the FunC VM has been initialized, add callbacks to native functions
// so they become available for the script engine
void Machine::set_callbacks()
{
  // Callbacks to be usable by FunC script
  //std::cout << "Machine::execute_code() setting callbacks" << std::endl;
  FunC::set_error_callback(vm, (FunC::ErrorCb) func_errorCallback);

  // Utility functions (temporary workarounds for language shortcomings)
  FunC::defineNative(vm, "reset",     (FunC::NativeFn) func_reset);
  FunC::defineNative(vm, "str",       (FunC::NativeFn) func_str); // Return argument(s) as string
  FunC::defineNative(vm, "getkey",    (FunC::NativeFn) func_getkey); // Read keyboard input
  //FunC::defineNative(vm, "chr",       (FunC::NativeFn) func_chr); // Get UTF8 character for codepoint
  //FunC::defineNative(vm, "ord",       (FunC::NativeFn) func_ord); // Get codepoint for UTF8 character

  // Display functions
  FunC::defineNative(vm, "print",     (FunC::NativeFn) func_print); // Print arguments
  FunC::defineNative(vm, "cls",       (FunC::NativeFn) func_cls); // Clear screen or parts of it
  FunC::defineNative(vm, "cursor",    (FunC::NativeFn) func_cursor); // 0=Disable or 1=Enable cursor
  FunC::defineNative(vm, "autoscroll",(FunC::NativeFn) func_autoscroll); // 0=Disable or 1=Enable scrolling
  FunC::defineNative(vm, "scr_up",    (FunC::NativeFn) func_scr_up); // Scroll screen or parts of it
  FunC::defineNative(vm, "scr_down",  (FunC::NativeFn) func_scr_down); // Scroll screen or parts of it
  FunC::defineNative(vm, "scr_left",  (FunC::NativeFn) func_scr_left); // Scroll screen or parts of it
  FunC::defineNative(vm, "scr_right", (FunC::NativeFn) func_scr_right); // Scroll screen or parts of it
  FunC::defineNative(vm, "fg",        (FunC::NativeFn) func_fg); // Set foreground color preset
  FunC::defineNative(vm, "bg",        (FunC::NativeFn) func_bg); // Set background color preset
  FunC::defineNative(vm, "rgb",       (FunC::NativeFn) func_rgb); // Set RGB color for drawing primitives
  FunC::defineNative(vm, "pos",       (FunC::NativeFn) func_pos); // Set cursor row/col position
  FunC::defineNative(vm, "row",       (FunC::NativeFn) func_row); // Get cursor row
  FunC::defineNative(vm, "col",       (FunC::NativeFn) func_col); // Get cursor column
  FunC::defineNative(vm, "rows",      (FunC::NativeFn) func_rows); // Get number of screen rows
  FunC::defineNative(vm, "cols",      (FunC::NativeFn) func_cols); // Get number of screen columns
  FunC::defineNative(vm, "rect",      (FunC::NativeFn) func_rect); // Draw empty rectangle
  FunC::defineNative(vm, "area",      (FunC::NativeFn) func_area); // Draw filled rectangle
  FunC::defineNative(vm, "poly",      (FunC::NativeFn) func_poly); // Draw filled polygon
  FunC::defineNative(vm, "line",      (FunC::NativeFn) func_line); // Draw line
  FunC::defineNative(vm, "circle",    (FunC::NativeFn) func_circle); // Draw empty circle
  FunC::defineNative(vm, "disc",      (FunC::NativeFn) func_disc); // Draw filled circle
  FunC::defineNative(vm, "point",     (FunC::NativeFn) func_point); // Draw point (a single pixel)
  FunC::defineNative(vm, "rand",      (FunC::NativeFn) func_rand); // Get random double between 0.0 and 1.0

  // Disk I/O functions
  FunC::defineNative(vm, "open",      (FunC::NativeFn) func_open); // Open a file
  FunC::defineNative(vm, "read",      (FunC::NativeFn) func_read); // Read N bytes from an open file
  FunC::defineNative(vm, "readln",    (FunC::NativeFn) func_readln); // Read one line from an open file
  FunC::defineNative(vm, "eof",       (FunC::NativeFn) func_eof); // Get end-of-file detection status for file
  FunC::defineNative(vm, "opendir",   (FunC::NativeFn) func_opendir); // Open a directory
  FunC::defineNative(vm, "readdir",   (FunC::NativeFn) func_readdir); // Read from an open directory
  FunC::defineNative(vm, "file_owner",(FunC::NativeFn) func_file_owner); // Return UID for named file
  FunC::defineNative(vm, "file_group",(FunC::NativeFn) func_file_group); // Return UID for named file
  FunC::defineNative(vm, "file_type" ,(FunC::NativeFn) func_file_type); // Return textual type of named file ("file", "directory",...)
  FunC::defineNative(vm, "file_size", (FunC::NativeFn) func_file_size); // Return size (in bytes) of named file
  FunC::defineNative(vm, "file_atime",(FunC::NativeFn) func_file_atime); // Return last access time for named file
  FunC::defineNative(vm, "file_mtime",(FunC::NativeFn) func_file_mtime); // Return last modify time for named file
  FunC::defineNative(vm, "file_ctime",(FunC::NativeFn) func_file_ctime); // Return last create time for named file

  // Common I/O functions
  FunC::defineNative(vm, "close",     (FunC::NativeFn) func_close); // Close a file
  FunC::defineNative(vm, "error",     (FunC::NativeFn) func_error); // Get last error code for file (0=success)

}

bool Machine::execute_code(std::string code) {

  display.hide_cursor();
  //running = this;
  //fc_status = FunC::interpret(source_buffer + offset); // Point to the start of the most recent code appended
  char* err = NULL;
  std::cout << "Machine::execute_code() calling FunC::interpret()" << std::endl;
  fc_status = FunC::interpret(vm, code.c_str(), &err);
  std::cout << "FunC::interpret() returned " << fc_status << std::endl;
  //running = nullptr;

  if (err != NULL) {
    std::cout << "FunC::interpret() error message:" << std::endl;
    display.print(err);
    //std::cout << "FunC::interpret() free(" << &err << ") // err" << std::endl;
    //free(err);
    err = NULL;
  }

  if (fc_status != FunC::INTERPRET_COMPILED) {
    reset_vm();
    display.print("READY.\n");
    display.show_cursor(true);
  }
  return (fc_status == FunC::INTERPRET_COMPILED);
}

bool Machine::execute_file(std::string fname, std::string arguments) {
//  IOFile file = IOFile(fname);
//  std::string buf = file.contents;
  IOFile* file = new IOFile();
  std::string buf = file->slurp(fname);
  delete file;

  // TODO: Need error handling

  reset_vm(); // Always start with a clean slate when running a file
  return execute_code(buf);
}

bool Machine::execute_line(std::string line)
{
  //std::cout << "VM() execute=[" << line << "]" << std::endl;
  // Strip leading and trailing spaces
  std::regex re("^\\s*(.*?)\\s*$");
  line = regex_replace(line, re, "$1");
  //std::cout << "VM() trimmed=[" << line << "]" << std::endl;
  if (line=="") { return false; }

  std::string cmd;
  std::string arguments;
  split_cmd(line, &cmd, &arguments);
  //std::cout << "VM() cmd=[" << cmd << "] arguments=[" << arguments << "]" << std::endl;

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
    // cmd is a file, execute with arguments
    return execute_file(path+"/"+cmd+".fun", arguments);
  } else {

    return execute_code(line);
  }
}


FunC::InterpretResult Machine::run()
{
  display.pre_render();

  if (fc_status == FunC::INTERPRET_COMPILED || fc_status == FunC::INTERPRET_RUNNING) {
    if (fc_break==true) {
      std::cout << "Machine::run() fc_break == true" << std::endl;
      reset_vm();
      fc_status = FunC::INTERPRET_OK;
      display.print("^C\nREADY.\n");
      display.show_cursor(true);
      flush_msg_queue();
    } else {
      running = this;
      fc_status = FunC::run(vm);
      running = nullptr;
      switch (fc_status) {
        case FunC::INTERPRET_OK: // VM is ready to receive (more) code
          std::cout << "FunC::run() returned INTERPRET_OK" << std::endl;
          if (display.col>0) { display.print("\n"); }
          display.print("READY.\n");
          display.show_cursor(true);
          flush_msg_queue();
          break;
        case FunC::INTERPRET_COMPILED:
          // run() has already been called so this is should be impossible
          // See FunC::vm.c
          std::cerr << "Machine.run() returned INTERPRET_COMPILED unexpectedly" << std::endl;
          display.print("?INTERNAL ERROR\n");
        case FunC::INTERPRET_RUNNING:
          break;
        case FunC::INTERPRET_COMPILE_ERROR:
          // run() should never get called if there was a compile error
          // See Machine::execute_code()
          std::cerr << "Machine.run() returned INTERPRET_COMPILE_ERROR unexpectedly" << std::endl;
          display.print("?INTERNAL ERROR\n");
        case FunC::INTERPRET_RUNTIME_ERROR:
          // Error messages have already been printed via func_errorCallback()
        default:
          //reset_vm();
          display.print("\rREADY.\n");
          display.show_cursor(true);
          flush_msg_queue();
      }
    }
  } else {
    // No script running so we need to handle events
    // This is a very primitive test/rescue shell
    //std::cout << "Machine::run() calling getc() on STDIN" << std::endl;
    Message* msg = nullptr;
    std::string line = "";
    while (m_queue.empty()==false) {
      msg = m_queue.front();
      m_queue.pop();
      //std::cout << "Machine() Msg received: " << msg << std::endl;
      switch (msg->type)
      {
        case Message::Type::TextInput:
          display.scroll_right(display.row, display.col, display.row, display.cols-1); // insert
          display.print(msg->text.c_str());
          break;
        case Message::Type::KeyDown: {
          //std::cout << "Machine() received KeyDown code=" << msg->key.sym << std::endl;
          if (msg->key.sym >= SDLK_a && msg->key.sym <= SDLK_z) { break; }
          if (msg->key.sym >= SDLK_0 && msg->key.sym <= SDLK_9) { break; }
          switch (msg->key.sym)
          {
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
              for (int c=0; c<display.cols; c++) {
                line += display.char_at(display.row, c);
              }
              display.print("\n");
              execute_line(line);
              break;
            case SDLK_LEFT:
              display.cursor_left();
              break;
            case SDLK_RIGHT:
              display.cursor_right();
              break;
            case SDLK_UP:
              display.cursor_up();
              break;
            case SDLK_DOWN:
              display.cursor_down();
              break;
            case SDLK_DELETE: // test
              display.scroll_left(display.row, display.col, display.row, display.cols-1);
              break;
            case SDLK_BACKSPACE: // test
              display.pos(display.row, display.col-1);
              display.scroll_left(display.row, display.col, display.row, display.cols-1);
              break;
            case SDLK_PAGEUP: // test
              display.scroll_left();
              break;
            case SDLK_PAGEDOWN: // test
              display.scroll_right();
              break;
            default:
              std::cout << "Machine() builtin shell ignored " << msg << std::endl;
              break;
          }
          break;
        }
        case Message::Type::KeyUp:
          //std::cout << "Machine() received KeyUp" << std::endl;
          break;
        default:
          std::cerr << "Machine() UNHANDLED: " << msg << std::endl;
          //std::cerr << "Machine() Msg type is UNHANDLED: " << msg->type << std::endl;
      }
      delete msg;
    }
  }
  fc_break = false;

  display.post_render();
  return fc_status;
}

void Machine::flush_msg_queue()
{
  Message* msg = nullptr;
  while (m_queue.empty()==false) {
    msg = m_queue.front();
    m_queue.pop();
    //std::cout << "Machine::flush() discarding " << msg << std::endl;
    delete msg;
  }
  //std::cout << "Machine() flushed message queue" << std::endl;
}

void Machine::push(Message* msg)
{
  //std::cout << "Message " << msg << " queued on VM " << this << std::endl;
  if (msg->type == Message::Type::Break) { fc_break = true; }
  m_queue.push(msg);
}

int chars(const char* str) {
  int c=0;
  int bytes = strlen(str);
  for (int i=0; i<bytes; i++) {
    if (((unsigned char) str[i] & 192) != 128) {
      c++;
      //std::cout << "Machine::chars() [count] byte=" << (unsigned char) str[i] << std::endl;
    } else {
      //std::cout << "Machine::chars() [     ] byte=" << (unsigned char) str[i] << std::endl;
    }
  }
  return c;
}


uint16_t Machine::add_iohandle(IOHandle* ptr)
{
  // Find an available handle
  //std::cout << "Machine::add_iohandle() ptr=" << ptr << std::endl;
  for (uint16_t i=0; i<iohandles.size(); i++) {
    //std::cout << "Machine::add_iohandle() check handle=" << i+1 << std::endl;
    if (iohandles[i]->is_closed()) {
      //std::cout << "Machine::add_iohandle() delete handle=" << i+1 << std::endl;
      delete iohandles[i];
      iohandles[i] = ptr;
      //std::cout << "Machine::add_iohandle() reused handle=" << i+1 << std::endl;
      return i;
    }
  }
  // No available handles so append it
  iohandles.push_back(ptr);
  //std::cout << "Machine::add_iohandle() appended handle=" << iohandles.size() << std::endl;
  return iohandles.size()-1;
}


// STATIC callbacks depend on the static variable "current" instead of "this"
// because FunC is written in C and doesn't have the foggest faintest idea what "this" is.
// And it doesn't have to, because only one FunC VM will ever be running at any point in time
// as long as we don't do anything colossaly stupid like try to run them in different threads.

// FunC calls this to print runtime errors
void Machine::func_errorCallback(const char* errormsg) {
  if (running==nullptr) { return; }
  running->display.print(errormsg);
}


// Implement the very latest in looping and rasterizing technology
// to facilitate the visual rendition of literal constructs for the
// cognitive consumption of information via visual means.
// Print stuff. On the screen.
// Synopsis: print("Hello world\n", 3.14, foo); // Any number of arguments
bool Machine::func_print(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  for (int i=0; i<argc; i++) {
    running->display.print(FunC::to_cstring(argv[i]));
  }
  *result = FunC::to_numberValue(1);
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
// Synopsis: cls();               // Clear entire screen (0 arguments)
// Synopsis: cls(row);            // Clear only one row (1 argument)
// Synopsis: cls(r1,r2);          // Clear rows r1 thru r2 (2 arguments)
// Synopsis: cls(r1,r2,cp);       // Clear rows r1 thru r2 using the specified codepoint (3 arguments)
// Synopsis: cls(r1,c1,r2,c2);    // Clear a rectangular area of the screen (4 arguments)
// Synopsis: cls(r1,c1,r2,c2,cp); // ...using the specified codepoint (5 arguments)
// Obviously, where no codepoint is specified, the default is 32 (blank space)
bool Machine::func_cls(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
  running->execute_file("funos/bin/init.fun", "");
  *result = FunC::to_numberValue(1);
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
            if (chars(name.c_str())>1) {
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


// Given a codepoint, return a string containing the UTF-8 character
// For codepoints 0 through 127, this is identical to the ASCII character
// Synopsis: string = chr(65); // = "A"
/*
bool Machine::func_chr(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // string = chr(codepoint)
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int codepoint = (int) FunC::to_double(argv[0]);
  char buf[5] = {0,0,0,0,0};
  char* err = nullptr;
  char* res = UTF8hack::append(codepoint, buf, err); // utf8.h
  //std::cout << "utf8::append returned \"" << res << "\" for cp " << codepoint << std::endl;
  *result = FunC::to_stringValue(running->vm, (const char*) res);
  return true;
//  return FunC::to_stringValue(Fontcache::UnicodeToUTF8(FunC::to_double(argv[0])).c_str());
}
*/

// Given a character, return the UTF-8 codepoint
// If the argument is a string with multiple characters,
// this function works on the first character in the string
// Synopsis: codepoint = ord("Alphabet"); // = 65
/*
bool Machine::func_ord(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  // int = ord(string)
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  // We expect only a single char but utf8::codepoints accepts a longer string
  std::vector<int> results = UTF8hack::codepoints(FunC::to_cstring(argv[0]));
  // Return the first integer, if any
  if (results.empty()) { *result = FunC::to_numberValue(-1); return false; }
  *result = FunC::to_numberValue(results[0]);
  return true;
}
*/



// Return a substring of BYTES from a string
// Synopsis: str = substr(string, offset, length)
// negative offset = count from right hand side
// negative length = remaining length minus this
/*
bool Machine::func_sub(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result) {
  if (argc < 2 || argc > 3) { *result = FunC::to_numberValue(-1); return false; }
  char* str = FunC::to_cstring(argv[0]);
  int strlength = strlen(str); // Number of bytes in the original string
  // Index of first byte to return
  int offset = (int) FunC::to_double(argv[1]);
  if (offset < 0) { offset = strlength + offset; } // negative = count from the tail end
  if (offset < 0) { *result = FunC::to_stringValue(running->vm, ""); return true; } // Negative offset past entire string
  if (offset >= strlength) { *result = FunC::to_stringValue(running->vm, ""); return true; } // Offset past entire string
  // Desired (max) number of UTF8 chars to return
  int maxlen = strlength;
  if (argc == 3) { maxlen = (int) FunC::to_double(argv[2]); }
  if (maxlen < 0) { maxlen = strlength + maxlen - offset; } // negative = remaining length minus this
  if (maxlen > (strlength - offset)) { maxlen = strlength - offset; } // Limit offset to maximum possible
  if (maxlen <= 0) { *result = FunC::to_stringValue(running->vm, ""); return true; } // Zero length

  std::string src = str;
  std::string res = src.substr(offset, maxlen);
  *result = FunC::to_stringValue(running->vm, res.c_str());
  return true;
}
*/



// Return a substring of UTF-8 CHARACTERS from a string
// Synopsis: str = substr(string, offset, length)
// negative offset = count from right hand side
// negative length = remaining length minus this
/*
bool Machine::func_substr(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result) {
  if (argc < 2 || argc > 3) { *result = FunC::to_numberValue(-1); return false; }
  char* str = FunC::to_cstring(argv[0]);
  int strlength = chars(str); // Number of UTF8 chars in the original string
  // Index of first UTF8 char to return
  int offset = (int) FunC::to_double(argv[1]);
  if (offset < 0) { offset = strlength + offset; } // negative = count from the tail end
  if (offset < 0) { *result = FunC::to_stringValue(running->vm, ""); return true; } // Negative offset past entire string
  if (offset >= strlength) { *result = FunC::to_stringValue(running->vm, ""); return true; } // Offset past entire string
  // Desired (max) number of UTF8 chars to return
  int maxlen = strlength;
  if (argc == 3) { maxlen = (int) FunC::to_double(argv[2]); }
  if (maxlen < 0) { maxlen = strlength + maxlen - offset; } // negative = remaining length minus this
  if (maxlen > (strlength - offset)) { maxlen = strlength - offset; } // Limit offset to maximum possible
  if (maxlen <= 0) { *result = FunC::to_stringValue(running->vm, ""); return true; } // Zero length

  //std::cout << "Machine::func_substr() str=\"" << str << "\" strlen=" << strlen << " offset=" << offset << " maxlen=" << maxlen << std::endl;
  // Split the char* into array of codepoints
  std::vector<int> codepoints = utf8::codepoints(str);
  //std::cout << "Machine::func_substr() begin copying" << std::endl;
  // Copy the codepoints we want
  std::vector<int> keep = std::vector<int>(codepoints.begin() + offset, codepoints.begin() + offset + maxlen);
  //std::cout << "Machine::func_substr() finished copying" << std::endl;

  // Reconstruct a string of UTF8 characters
  std::string res;
  //std::cout << "Machine::func_substr() begin reassembly" << std::endl;
  for (const auto &codepoint : keep) {
    //std::cout << "  codepoint " << codepoint << std::endl;
    char buf[5] = {0,0,0,0,0};
    char* err = nullptr;
    char* c_res = utf8::append(codepoint, buf, err);
    res += c_res;
  }

  //std::cout << "Machine::func_substr() done, result=\"" << result << "\"" << std::endl;
  *result = FunC::to_stringValue(running->vm, res.c_str());
  return true;
}
*/

// Hide or show blinking cursor
// Synopsis: cursor(0) // Hide
// Synopsis: cursor(1) // Show
// Note: Repeated calls "stack" so two calls to "hide"
// must be followed by two calls to "show"
// for the cursor to re-appear
bool Machine::func_cursor(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_autoscroll(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_fg(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 1) {
    running->display.set_fgcolor((int) FunC::to_double(argv[0]));
  }
  *result = FunC::to_numberValue(running->display.fgcolor);
  return true;
}


// Set the current background (text) color to one of the 16 color presets (0-15)
bool Machine::func_bg(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc == 1) {
    running->display.set_bgcolor((int) FunC::to_double(argv[0]));
  }
  *result = FunC::to_numberValue(running->display.bgcolor);
  return true;
}

// Set RGB color for drawing primitives
bool Machine::func_rgb(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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

// Place the text cursor (and thereby control where the next print() text will appear
// at the specified row and column
// Synopsis: pos(row, column)
// Note: The upper left cursor position is 0,0
bool Machine::func_pos(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_row(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.row);
  return true;
}

// Return the current column position of the text cursor
// Synopsis: current = col()
// Note: The leftmost column is number 0
bool Machine::func_col(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.col);
  return true;
}

// Return the number of visible rows on the screen
// Synopsis: bottom = rows()-1;
bool Machine::func_rows(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.rows);
  return true;
}

// Return the number of visible columns on the screen
// Synopsis: right = cols()-1;
bool Machine::func_cols(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue(running->display.cols);
  return true;
}

// Render a line rectangle using the current foreground color
// starting at coordinates x,y with width w and height h
// Compare and contrast with area()
// Synopsis: rect(x,y,w,h);
// Note: With 40x25 characters at 8x8 pixels, the display resolution is 320x200
// Warning: By moronic design, OpenGL may OMIT the first and/or last pixel
bool Machine::func_rect(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_area(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_poly(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_line(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_circle(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_disc(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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
bool Machine::func_point(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
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


// Return a random double between 0.0 and 1.0
// Typical use is to multiply this number with the desired range
// Synopsis: percent = rand()*100
bool Machine::func_rand(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  *result = FunC::to_numberValue( rand_double(rand_eng) );
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
  char* filename = FunC::to_cstring(argv[0]);
  char* mode = FunC::to_cstring(argv[1]);
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
// Note: This operation may return emptystring "" for very long lines
// (>4096 bytes) which may require multiple readln() // calls.
// If this is the case, error(fh) will return EAGAIN (code 3406)
// Synopsis: while (!eof(fh)) { var line = readln(fh); }
bool Machine::func_readln(FunC::VM* vm, int argc, FunC::Value argv[], FunC::Value* result)
{
  if (argc != 1) { *result = FunC::to_numberValue(-1); return false; }
  int handle = (int) FunC::to_double(argv[0]);
  //std::cout << "Machine::func_readln() handle=" << handle << std::endl;
  std::string res = running->iohandles[handle]->readln();
  *result = FunC::to_stringValue(running->vm, res.c_str());
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
  *result = FunC::to_numberValue(running->add_iohandle(dir)); // Handles are 1 indexed
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


