
fun readln() {
  var str = "";
  var add = "";
  var end = false;
  while (end==false) {
    add = getc();
    if (add == "[Return]") { 
      end=true; 
    } else {
      if (add.chars==1) { 
        print(add); 
        str = str + add;
      }
    } // Echo characters
  }
  return str;
}

// Read lines from STDIN
fg(1); // white
print("FunOS interactive shell v1.0\n");
var str = "";
var running = true;
while (running == true) {
  fg(1); // white
  print("/> ");
  cursor(1);
  str = readln();
  print("\n");
  cursor(0);
  if (str != "") {
    fg(15); // light gray
    print(str + "\n");
  }
  if (str == "exit") { running = false; }
}
fg(7); // yellow
