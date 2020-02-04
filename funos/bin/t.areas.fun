
var x1;
var y1;
var x2;
var y2;

while (true) {

  x1 = rand()*320;
  y1 = rand()*200;
  x2 = rand()*320;
  y2 = rand()*200;
  rgb(rand(), rand(), rand());
  area(x1, y1, x2-x1, y2-y1);
}
