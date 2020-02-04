
var x1;
var y1;
var x2;
var y2;
var x3;
var y3;

while (true) {

  x1 = rand()*320;
  y1 = rand()*200;
  x2 = rand()*320;
  y2 = rand()*200;
  x3 = rand()*320;
  y3 = rand()*200;
  rgb(rand(), rand(), rand());
  poly(x1, y1, x2, y2, x3, y3);
}
