
var x = 0;
var x_inc = false;
var y = 0;
var y_inc = false;

while (getc()=="") {
  rgb(255,255,255);
  rect(0,0,320,200);
  rgb(0,255,0);
  line(x,0,x,199);
  line(0,y,319,y);

  sleep(); // Yield, updates frame

  rgb(0,0,0);
  line(x,0,x,199);
  line(0,y,319,y);
  if (x == 0 or x == 319) { x_inc = !x_inc; }
  if (y == 0 or y == 199) { y_inc = !y_inc; }
  x += x_inc ? 1 : -1;
  y += y_inc ? 1 : -1;
}
