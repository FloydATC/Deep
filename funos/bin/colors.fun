
screen.clear();
for (var i = 0; i < 16; i = i + 1) {
  screen.fg(3); // cyan
  screen.bg(0); // black
  screen.pos(1,4+i*2);  
  print(i);
  screen.pos(2+i,1);  
  print(i);
  for (var j = 0; j < 16; j = j + 1) {
    screen.fg(i);
    screen.bg(j);
    screen.pos(2+i,4+j*2);
    print("Aa");
  }
}
screen.fg(3); // cyan
screen.bg(0); // black
print("\n");


