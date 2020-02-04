
// Read characters from STDIN
var str = "";
var add = "";
while (true) {
  add = getc();
  if (add != "") {
    str = str + add;
    print(str + "\n");
  }
}
