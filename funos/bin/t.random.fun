
autoscroll(0);
var running = true;
while (running == true) {
  fg(rand()*16);
  bg(rand()*16);
  pos(rand()*rows(), rand()*cols());
  var cp = rand()*(255-32);
  print(chr(32+cp));
  if (getc() != "") { running = false; }
}
autoscroll(1);
bg(0);
fg(3);
cls();

