

// ---- Linked list v1.0 ----

class List {}

// Initialize linked list
fun new_list() {
  var list = List();
  list.length = 0;
  list.head = null;
  list.tail = null;
  return list;
}

class ListItem {}

// Add item at the beginning
fun unshift_list(list, value) {
  var item = ListItem();
  item.value = value;
  item.prev = null;
  item.next = list.head;
  if (item.next==null) {
    // First
    list.tail = item;
  } else {
    // Attach self to beginning of list
    item.next.prev = item;
  }
  list.head = item;
  list.length = list.length + 1;
  return list.length;
}

// Add item at the end
fun push_list(list, value) {
  var item = ListItem();
  item.value = value;
  item.next = null;
  item.prev = list.tail;
  if (item.prev==null) {
    // First
    list.head = item;
  } else {
    // Attach self to end of list
    item.prev.next = item;
  }
  list.tail = item;
  list.length = list.length + 1;
  return list.length;
}

// Remove first item
fun shift_list(list) {
  if (list.length == 0) { return null; }
  var item = list.head;
  if (item.next == null) {
    list.tail = null;
  } else {
    item.next.prev = null;
  }
  list.head = item.next;
  list.length = list.length - 1;
  return item.value;
}

// Remove last item
fun pop_list(list) {
  if (list.length == 0) { return null; }
  var item = list.tail;
  if (item.prev == null) {
    list.head = null;
  } else {
    item.prev.next = null;
  }
  list.tail = item.prev;
  list.length = list.length - 1;
  return item.value;
}

// Loop through list
fun foreach_list(list, function) {
  var item = list.head;
  var i = 0;
  while (item!=null) {
    function(item.value, i++);
    item = item.next;
  }
}


fun join_list(list, delimiter) {
  var str = "";
  fun concat(value, i) {
    if (i>0) { str = str + delimiter; }
    str = str + value;
  }
  foreach_list(list, concat);
  return str;
}

fun get_list(list, n) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  var item = list.head;
  while (item!=null) {
    if (i==n) {
      //debug(str("get_list(",n,") =>", item.value));
      return item.value;
    }
    item = item.next;
    i++;
  }
  return null; // Unreachable
}

fun set_list(list, n, value) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  var item = list.head;
  while (item!=null) {
    if (i==n) {
      item.value = value;
      //debug(str("set_list(",n,",",value,") ok"));
      return item.value;
    }
    item = item.next;
    i++;
  }
  return null; // Unreachable
}

fun del_list(list, n) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  // First and last items are special cases
  if (n == 0) { return shift_list(list); }
  if (n == list.length-1) { return pop_list(list); }

  var item = list.head;
  while (item!=null) {
    if (i==n) {
      item.prev.next = item.next;
      item.next.prev = item.prev;
      list.length = list.length - 1;
      return item.value;
    }
    item = item.next;
    i++;
  }
  return null; // Unreachable
}

fun ins_list(list, n, value) {
  var new = ListItem();
  new.value = value;
  new.prev = null;
  new.next = null;
  var i=0;
  // First and last items are special cases
  if (n == 0) { return unshift_list(list, value); }
  if (n >= list.length) { return push_list(list, value); }

  var item = list.head;
  while (item!=null) {
    if (i==n) {
      item.prev.next = new; // link to new from previous
      new.prev = item.prev; // link from new to previous
      new.next = item;      // link from new to current
      item.prev = new;      // link from current to new
      list.length = list.length + 1;
      return new.value;
    }
    item = item.next;
    i = i + 1;
  }
  return null; // Unreachable
}

// =============================




// Prepare screen
bg(6); // blue
fg(3); // cyan
cls();

// Draw title bar
fun titlebar(title) {
  if (title="") { title="(no name)"; }
  bg(15); // light gray
  fg(6); // blue
  cls(0,0,9552); // double horizontal line
  pos(0,2);
  print("[ ", title, " ]");
}

// Draw status bar
fun statusbar(mode) {
  if (mode="") { mode="INSERT"; }
  bg(15); // light gray
  fg(6); // blue
  pos(24,0);
  fg(6); // blue
  print("line:",docL," col:",docC," ",mode);
  cls(24,col(),24,39); // space
}

// Horizontal scroll bar
fun hscrollbar() {
  cursor(0);
  bg(11); // Dark gray
  fg(0);  // Black
  cls(23,1,37,39,32);    // Space
  cls(23,0,23,0,9664);   // Triangle pointing left
  cls(23,38,23,38,9654); // Triangle pointing right
  cursor(1);
}

// Vertical scroll bar 9650=up, 9660=down
fun vscrollbar() {
  cursor(0);
  bg(11); // Dark gray
  fg(0);  // Black
  cls(2,39,21,39,32);    // Space
  cls(1,39,1,39,9650);   // Triangle pointing up
  cls(22,39,22,39,9660); // Triangle pointing down
  cursor(1);
}


fun editor_scroll_up() {
  //debug("editor_scroll_up()");
  vscrollbar();
  bg(bgc);
  fg(fgc);
  scr_up(1, 0, 22, 38);
  // Patch bottom line
  var this_line = docL-scrR+22;
  var line = get_list(lines, this_line);
  if (line != null) {
    pos(22,0);
    var fc = docC-scrC;
    print(substr(line, fc, fc+39));
  }
}

fun editor_scroll_down() {
  //debug("editor_scroll_down()");
  vscrollbar();
  bg(bgc);
  fg(fgc);
  scr_down(1, 0, 22, 38);
  // Patch top line
  var this_line = docL-(scrR-1);
  var line = get_list(lines, this_line);
  if (line != null) {
    pos(1,0);
    var fc = docC-scrC;
    print(substr(line, fc, fc+39));
  }
}


fun editor_scroll_left() {
  cursor(0);
  set_list(lines, docL, line);
  fun patch_right_edge(line, i) {
    var first = docL-scrR+1;
    if (i >= first and i <= first+21) {
      pos(i+1-first, 38);
      var fc = docC-scrC;
      var c = substr(line, fc+39, 1);
      print(c ? c : " ");
    }
  }
  //debug("editor_scroll_left()");
  hscrollbar();
  bg(bgc);
  fg(fgc);
  scr_left(1, 0, 22, 38);
  var first_line = docL-scrR-1;
  var last_line = first_line + 21;
  foreach_list(lines, patch_right_edge);
  cursor(1);
}

fun editor_scroll_right() {
  cursor(0);
  set_list(lines, docL, line);
  fun patch_right_edge(line, i) {
    var first = docL-scrR+1;
    if (i >= first and i <= first+21) {
      pos(i+1-first, 0);
      var fc = docC-scrC;
      var c = substr(line, fc, 1);
      print(c ? c : " ");
    }
  }
  //debug("editor_scroll_right()");
  hscrollbar();
  bg(bgc);
  fg(fgc);
  scr_right(1, 0, 22, 38);
  var first_line = docL-scrR-1;
  var last_line = first_line + 21;
  foreach_list(lines, patch_right_edge);
  cursor(1);
}

fun redraw_screen() {
  cursor(0);
  if (scrR>docL+1) { scrR = docL+1; }
  //debug(str("redraw_screen() docL=",docL," docC=",docC," scrR=",scrR," scrC=",scrC));
  set_list(lines, docL, line);
  cls(1,0,22,38);
  fun print_line(line, i) {
    //debug(str("redraw_screen() loop i=", i, " begin"));
    var first = docL-scrR+1;
    if (i >= first and i <= first+21) {
      var fc = docC-scrC;
      //debug(str("redraw_screen() loop i=", i, " pos"));
      pos(i+1-first, 0);
      //debug(str("redraw_screen() loop i=", i, " substr"));
      //debug(str("redraw_screen() line=", line));
      //debug(str("redraw_screen() fc=", fc));
      print(substr(line, fc, 39));
    }
    //debug(str("redraw_screen() loop i=", i, " end"));
  }
  hscrollbar();
  vscrollbar();
  var first_line = docL-scrR-1;
  var last_line = first_line + 21;
  bg(bgc);
  fg(fgc);
  foreach_list(lines, print_line);
  pos(scrR, scrC);
  //debug(str("redraw_screen() done"));
  cursor(1);
}

fun check_cursor_pos() {
  // Check cursor column against line length
  var linelen = line.chars;
  //debug(str("check_cursor_pos() docC=",docC," linelen=",linelen));
  var redraw = false;

  if (docC > linelen) {
    var delta = docC - linelen;
    //debug(str("cursor is ",delta," too far right"));
    if (delta > scrC) {
      scrC = 0;
      redraw = true;
    } else {
      scrC -= delta;
    }
    //debug(str("cursor is now at column ",scrC));
    docC = linelen;
  }
  if (scrC > 38) {
    scrC = 38;
    redraw = true;
  }

  if (redraw==true) {
    // We need to jump and redraw
    redraw_screen();
    pos(scrR, scrC);
  }
  //debug("check_cursor_pos() done");
}

fun handle_return() {
  // Break current line, insert a new one
  var tail = substr(line, docC);
  line = substr(line, 0, docC);
  set_list(lines, docL, line);
  line = tail;
  ins_list(lines, docL+1, line);
  if (docC-scrC == 0) {
    // We are scrolled all the way left
    cls(scrR, scrC, scrR, 38); // Clear rest of current line
    docL++;
    docC=0;
    scrC=0;
    scrR++;
    if (scrR<=22) {
      // Push lines down
      scr_down(scrR, 0, 22, 38);
      pos(scrR, 0);
      print(substr(tail, 0, 39));
    } else {
      // We need to scroll up instead
      scrR=22;
      scr_up(1, 0, 22, 38);
      pos(scrR, 0);
      print(substr(tail, 0, 39));
      pos(scrR, 0);
    }
  } else {
    // Need to redraw entire screen
    docL++;
    docC=0;
    scrC=0;
    scrR++;
    if (scrR>22) { scrR=22; }
    redraw_screen();
  }
}

fun handle_up() {
  set_list(lines, docL, line);
  if (docL > 0) {
    docL--;
    line = get_list(lines, docL);
    //debug("line"+str(docL)+" '"+line+"'");
    if (scrR == 1) {
      // Need to scroll down
      editor_scroll_down();
    } else {
      scrR--;
    }

    check_cursor_pos();
  }
}

fun handle_down() {
  set_list(lines, docL, line);
  var peek = get_list(lines, docL+1);
  if (peek == null) { return; } // End of file
  scrR++;
  docL++;
  line = peek;
  if (scrR>22) {
    // We need to scroll up
    scrR=22;
    editor_scroll_up();
  }
  check_cursor_pos();
}

fun handle_left() {
  if (docC == 0) {
    // Go to end of previous line
    if (docL > 0) {
      set_list(lines, docL, line);
      docL--;
      line = get_list(lines, docL);
      if (scrR==1) {
        editor_scroll_down();
      } else {
        scrR--;
      }
      // Do we need to jump or move cursor?
      var linelen = line.chars;
      //var fc = docC - scrC;

      docC = linelen;
      scrC = linelen;
      check_cursor_pos();

    }
  } else {
    if (scrC==0) {
      docC--;
      editor_scroll_right();
    } else {
      scrC--;
      docC--;
      pos(scrR, scrC);
    }
  }
}

fun handle_right() {
  var linelen = line.chars;
  if (docC<linelen) {
    scrC++;
    if (scrC>38) {
     scrC--;
     editor_scroll_left();
    }
    docC++;
  } else {
    // Jump to beginning of next line
    var peek = get_list(lines, docL+1);
    if (peek == null) { return; }
    set_list(lines, docL, line);
    line = peek;
    docL++;
    scrR++;
    if (scrR>22) {
      scrR = 22;
      editor_scroll_up();
    }
    var fc = docC-scrC;
    docC = 0;
    scrC = 0;
    if (fc > 0) { redraw_screen(); }
  }
}

fun handle_home() {
  var fc = docC-scrC;
  docC=0;
  scrC=0;
  if (fc > 0) { redraw_screen(); }
}

fun handle_end() {
  var linelen = line.chars;
  if (docC<linelen) {
    var delta = linelen - docC;
    docC+=delta;
    scrC+=delta;
  }
  check_cursor_pos();
}

fun handle_backspace() {
  if (docC == 0) {
    // Need to merge this line with previous line
    if (docL > 0) {
      del_list(lines, docL);
      scr_up(scrR, 0, 22, 38);
      docL--;
      scrR--;
      var prev = get_list(lines, docL);
      var linelen = prev.chars;
      line = prev + line;
      docC = linelen;
      scrC = linelen;
      check_cursor_pos();
      var fc = docC-scrC;
      pos(scrR,scrC);
      print(substr(line, docC, fc-docC+39));
    }
  } else {
    line = substr(line, 0, docC-1) + substr(line, docC);
    if (scrC ==0) {
      editor_scroll_right();
    } else {
      scrC--;
      docC--;
      bg(bgc);
      fg(fgc);
      pos(scrR, scrC);
      // Redraw line from cursor
      print(substr(line+" ", docC, cols()-1-scrC));
    }
  }
}

fun handle_delete() {
  var linelen = line.chars;
  if (docC == linelen) {
    // Need to merge previous line with this one
    var next = get_list(lines, docL+1);
    if (next != null) {
      line = line + next;
      del_list(lines, docL+1);
      if (scrR<22) {
        scr_up(scrR+1, 0, 22, 38);
      }
      pos(scrR,scrC);
      print(substr(line+" ", docC, cols()-1-scrC));
    }
  } else {
    // Eat the next character
    line = substr(line, 0, docC) + substr(line, docC+1);
    print(substr(line+" ", docC, cols()-1-scrC));
  }
}

fun handle_pgup() {
  set_list(lines, docL, line);
  if (docL>0) {
    docL-=22;
    if (docL<0) {
      scrR=1;
      docL=0;
    }
    line = get_list(lines, docL);
    var linelen = line.chars;
    if (docC > linelen) { check_cursor_pos(); }
    redraw_screen();
  }
}

fun handle_pgdn() {
  set_list(lines, docL, line);
  if (docL<lines.length-1) {
    docL+=22;
    if (docL>lines.length-23) {
      scrR=22;
      if (scrR>lines.length) { scrR=lines.length; }
      docL=lines.length-1;
    }
    line = get_list(lines, docL);
    var linelen = line.chars;
    if (docC > linelen) { check_cursor_pos(); }
    redraw_screen();
  }
}

fun handle_character(key) {
  // Single, regular character key

  // Insert character into current line
  line = substr(line, 0, docC) + key + substr(line, docC);

  // Push current line
  scr_right(scrR, scrC, scrR, cols()-2);
  scrC++;
  docC++;
  pos(scrR, scrC-1);
  bg(bgc);
  fg(fgc);
  print(key);

  // Are we at the right edge?
  if (scrC > 38) {
    // At right edge, scroll entire editor
    scrC=38;
    editor_scroll_left();
  }
}

// Initialize
var bgc = 6; // blue
var fgc = 3; // cyan
var docL = 0;
var docC = 0;
var scrR = 1;
var scrC = 0;
var mode = "INSERT";

var lines = new_list();
var line = "";
push_list(lines, line);

var running = true;
titlebar("");
hscrollbar();
vscrollbar();
statusbar(mode);
pos(scrR,scrC);
bg(bgc);
fg(fgc);
cursor(1);

// Main loop
while (running==true) {
  var key = getc();

  if (key=="") {
    sleep(0); // Yield
  } else {
    if (key.chars==1) {
      handle_character(key);
    } else {
      // Any other keystrokes:

      switch (key) {

        // -- Navigation
        case "[Up]":         handle_up();        break;
        case "[Down]":       handle_down();      break;
        case "[Left]":       handle_left();      break;
        case "[Right]":      handle_right();     break;
        case "[Home]":       handle_home();      break;
        case "[End]":        handle_end();       break;
        case "[PageUp]":     handle_pgup();      break;
        case "[PageDown]":   handle_pgdn();      break;

        // -- Editing
        case "[Return]":     handle_return();    break;
        case "[Backspace]":  handle_backspace(); break;
        case "[Delete]":     handle_delete();    break;

        // -- Application
        case "[Ctrl C]":     running = false;    break;

        // -- Unknown / ignored
        default: {
          pos(0,0);
          print(key);
        }
      }
    }


    statusbar(mode);
    pos(scrR,scrC);
    bg(6); // blue
    fg(3); // cyan
  }
}

// Clean exit
bg(0);
fg(3);
cls();


