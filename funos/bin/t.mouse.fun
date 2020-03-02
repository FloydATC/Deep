
var prev_x = -1;
var prev_y = -1;
var curr_x = -1;
var curr_y = -1;
cls();
print("Use the mouse to draw\n");
while (getkey() == "") {
  curr_x = mouse_x();
  curr_y = mouse_y();
  if (prev_x != -1 and prev_y != -1) {
    if (curr_x != prev_x or curr_y != prev_y) {
      // Mouse has moved
      rgb(rand(), rand(), rand());
      line(prev_x, prev_y, curr_x, curr_y);
    }
  }
  prev_x = curr_x;
  prev_y = curr_y;
  sleep(1);
}
cls();


