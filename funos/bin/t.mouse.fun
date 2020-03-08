

// Array index constants for mouse()
var x = 0;
var y = 1;
var relx = 2;
var rely = 3;

var event;
var draw = false;

screen.clear();
print("Use the mouse to draw\n");
while (getkey() != "[Escape]") {
  if (event = mouse()) {
    switch (event.mouse) {
      case "down": draw = true; break;
      case "up": draw = false; break;
      case "move": {
        if (draw) {
          gl.rgb(rand(), rand(), rand());
          gl.line(
            event.x - event.relx, 
            event.y - event.rely, 
            event.x, 
            event.y
          );
        }
        break;
      }
    }
  }
  sleep(1);
}
screen.clear();


