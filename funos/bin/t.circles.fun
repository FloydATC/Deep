
var x;
var y;
var r;

while (getkey() != "[Escape]") {

  x = rand()*320;
  y = rand()*200;
  r = rand()*100;
  gl.rgb(rand(), rand(), rand());
  gl.circle(x, y, r);
}
screen.clear();



