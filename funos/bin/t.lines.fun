
var x1;
var y1;
var x2;
var y2;

while (getkey() != "[Escape]") {
  x1 = rand() * gl.width();
  y1 = rand() * gl.height();
  x2 = rand() * gl.width();
  y2 = rand() * gl.height();
  gl.rgb(rand(), rand(), rand());
  gl.line(x1, y1, x2, y2);
}
screen.clear();
