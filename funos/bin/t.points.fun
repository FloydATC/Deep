
var x;
var y;

while (getkey() != "[Escape]") {

  x = rand() * gl.width();
  y = rand() * gl.height();
  gl.rgb(rand(), rand(), rand());
  gl.point(x, y);
}
screen.clear();

