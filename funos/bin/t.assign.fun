
var running = true;
while (running) {
  var key = getc();
  if (key!="") { 
    print(key, "\n");
    running = false; 
  }
}
