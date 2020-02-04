
cls(0,0,24,39,0x2592); // Checker pattern
cls(1,1,23,38);

rgb(255,0,255); // magenta
rect(0,0,320,200);

fg(7);
pos(2,2);
print("Hello world");


for (var x=0; x<320; x++) {
  rgb(rand(),rand(),rand());
  var h = rand()*20;
  var y = 150 - h/2;
  rect(x,y,1,h);
}

for (var y=0; y<200; y++) {
  rgb(rand(),rand(),rand());
  var w = rand()*20;
  var x = 240 - w/2;
  rect(x,y,w,1);
}
