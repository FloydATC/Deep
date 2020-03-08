
screen.autoscroll(0);

while (getkey() != "[Escape]") {

  screen.fg(rand()*16);
  screen.bg(rand()*16);
  screen.pos(
    rand() * screen.rows(), 
    rand() * screen.cols()
  );
  var cp = rand()*(255-32);
  print((32+cp).char);

}
screen.autoscroll(1);
screen.bg(0);
screen.fg(3);
screen.clear();





