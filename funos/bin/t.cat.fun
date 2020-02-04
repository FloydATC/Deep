
var filename = "funos/bin/t.cat.fun"; // Quine :-P
var fh = open(filename, "r");
var buf;

print("fh=",fh,"\n");
while (!eof(fh)) {
  print(readln(fh));
}
close(fh);
