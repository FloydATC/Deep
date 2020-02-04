
// This script will generate random strings to stress both
// string internalization and the passing of strings between
// FunC and the host application via callback functions

var s;
var lo = 32;
var hi = 255;
var r = hi-lo;
while (true) {
  s = str(chr(rand()*r+lo), chr(rand()*r+lo), chr(rand()*r+lo), chr(rand()*r+lo));
  print(s, " ");
}
