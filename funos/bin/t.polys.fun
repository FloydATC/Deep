
var x1;
var y1;
var x2;
var y2;
var x3;
var y3;

var width = gl.width();
var height = gl.height();

while (getkey() != "[Escape]") {

  x1 = rand() * width;
  y1 = rand() * height;
  x2 = rand() * width;
  y2 = rand() * height;
  x3 = rand() * width;
  y3 = rand() * height;
  gl.rgb(rand(), rand(), rand());
  gl.poly(x1, y1, x2, y2, x3, y3);
}
screen.clear();

