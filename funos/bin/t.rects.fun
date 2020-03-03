
var x1;
var y1;
var x2;
var y2;

var width = gl.width();
var height = gl.height();

while (getkey() != "[Escape]") {

  x1 = rand() * width;
  y1 = rand() * height;
  x2 = rand() * width;
  y2 = rand() * height;
  gl.rgb(rand(), rand(), rand());
  gl.rect(x1, y1, x2-x1, y2-y1);
}
screen.clear();
