
// Initialize
var bgc = 6; // blue
var fgc = 3; // cyan
var docL = 0;
var docC = 0;
var scrR = 1;
var scrC = 0;
var mode = "INSERT";

var lines = [""]; // Empty document
var line = "";

fun foreach_list(list, fn) {
  var i = 0;
  while (i<list.length) {
    fn(list[i], i++);
  }
}


// Prepare screen
bg(bgc); // blue
fg(fgc); // cyan
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
  bg(bgc);
  fg(bgc);
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
  var line = lines[this_line];
  if (line != null) {
    pos(22,0);
    var fc = docC-scrC;
    print(line.substr(fc, fc+39));
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
  var line = lines[this_line];
  if (line != null) {
    pos(1,0);
    var fc = docC-scrC;
    print(line.substr(fc, fc+39));
  }
}


fun editor_scroll_left() {
  cursor(0);
  lines[docL] = line;
  fun patch_right_edge(line, i) {
    var first = docL-scrR+1;
    if (i >= first and i <= first+21) {
      pos(i+1-first, 38);
      var fc = docC-scrC;
      print(line.chars>=fc+39 ? line.substr(fc+39, 1) : " ");
    }
  }
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
  lines[docL] = line;
  fun patch_right_edge(line, i) {
    var first = docL-scrR+1;
    if (i >= first and i <= first+21) {
      pos(i+1-first, 0);
      var fc = docC-scrC;
      var c = line.substr(fc, 1);
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
  lines[docL] = line;
  bg(bgc);
  fg(fgc);
  cls(1,0,22,38);
  fun print_line(line, i) {
    //debug(str("redraw_screen() loop i=", i, " begin"));
    var first = docL-scrR+1;
    if (i >= first and i <= first+21) {
      var fc = docC-scrC;
      pos(i+1-first, 0);
      if (fc<line.chars) print(line.substr(fc, 39));
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
  //var tail = substr(line, docC);
  var tail = line.substr(docC, 39);
  //line = substr(line, 0, docC);
  line = line.substr(0, docC);
  lines[docL] = line;
  line = tail;
  //debug("lines="+lines.length.base(10)+" docL="+docL.base(10));
  lines[docL+1:0] = [line]; // Insert below
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
      print(tail.substr(0, 39));
    } else {
      // We need to scroll up instead
      scrR=22;
      scr_up(1, 0, 22, 38);
      pos(scrR, 0);
      print(tail.substr(0, 39));
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
  lines[docL] = line;
  if (docL > 0) {
    docL--;
    line = lines[docL];
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
  lines[docL] = line;


  if (docL+1 == lines.length) { return; } // End of file

  scrR++;
  docL++;
  line = lines[docL];
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
      lines[docL] = line;
      docL--;
      line = lines[docL];
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
    if (docL+1 == lines.length) return; // End of file
    lines[docL] = line;
    docL++;
    line = lines[docL];
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
      lines[docL:1] = []; // Delete line
      scr_up(scrR, 0, 22, 38);
      docL--;
      scrR--;
      var prev = lines[docL];
      var linelen = prev.chars;
      line = prev + line;
      docC = linelen;
      scrC = linelen;
      check_cursor_pos();
      var fc = docC-scrC;
      pos(scrR,scrC);
      print(line.substr(docC, fc-docC+39));
    }
  } else {
    line = line.substr(0, docC-1) + line.substr(docC); // HMMM
    if (scrC ==0) {
      editor_scroll_right();
    } else {
      scrC--;
      docC--;
      bg(bgc);
      fg(fgc);
      pos(scrR, scrC);
      // Redraw line from cursor
      print((line+" ").substr(docC, cols()-1-scrC));
    }
  }
}

fun handle_delete() {
  var linelen = line.chars;
  if (docC == linelen) {
    // Need to merge previous line with this one
    if (lines.length > docL+1) {
      line = line + lines[docL+1];
      lines[docL+1:1] = []; // Delete line below current
      if (scrR<22) {
        scr_up(scrR+1, 0, 22, 38);
      }
      pos(scrR,scrC);
      print((line+" ").substr(docC, cols()-1-scrC));
    }
  } else {
    // Eat the next character
    line = line.substr(0, docC) + line.substr(docC+1); // HMMM
    print((line+" ").substr(docC, cols()-1-scrC));
  }
}

fun handle_pgup() {
  lines[docL] = line;
  if (docL>0) {
    docL-=22;
    if (docL<0) {
      scrR=1;
      docL=0;
    }
    line = lines[docL];
    var linelen = line.chars;
    if (docC > linelen) { check_cursor_pos(); }
    redraw_screen();
  }
}

fun handle_pgdn() {
  lines[docL] = line;
  if (docL<lines.length-1) {
    docL+=22;
    if (docL>lines.length-23) {
      scrR=22;
      if (scrR>lines.length) { scrR=lines.length; }
      docL=lines.length-1;
    }
    line = lines[docL];
    var linelen = line.chars;
    if (docC > linelen) { check_cursor_pos(); }
    redraw_screen();
  }
}

fun handle_character(key) {
  // Single, regular character key

  // Insert character into current line
  var temp = line;
  line = temp.substr(0, docC)
       + key
       + temp.substr(docC); // HMMM

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

fun handle_string(string) {
  // Multiple regular character keys at the same time (for efficiency)

  // Insert string into current line
  var temp = line;
  line = temp.substr(0, docC)
       + string
       + temp.substr(docC); // HMMM

  // Push current line
  scrC+=string.chars;
  docC+=string.chars;
  if (scrC > 38) scrC = 38;
  redraw_screen();
}


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
  var key = "";
  var string = "";
  while((key = getkey()) and (key.chars==1) and (key.code>=32)) string += key;
  if (string != "") {
    if (string.chars == 1) {
      handle_character(string);
    } else {
      handle_string(string);
    }
    statusbar(mode);
  }

  if (key=="") {
    sleep(0); // Yield
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

    statusbar(mode);
  }
}

// Clean exit
bg(0);
fg(3);
cls();


