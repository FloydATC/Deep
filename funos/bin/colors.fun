
cls();
for (var i = 0; i < 16; i = i + 1) {
  fg(3); // cyan
  bg(0); // black
  pos(1,4+i*2);  
  print(i);
  pos(2+i,1);  
  print(i);
  for (var j = 0; j < 16; j = j + 1) {
    fg(i);
    bg(j);
    pos(2+i,4+j*2);
    print("Aa");
  }
}
fg(3); // cyan
bg(0); // black
print("\n");

