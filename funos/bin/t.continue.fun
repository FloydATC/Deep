
// This tests the "continue" and "break" statements in a for() loop
for (var i=0; i<5; i++) {
  if (i==1) { print("i is one\n"); continue; }
  if (i==2) { print("i is two\n"); continue; }
  if (i==3) { print("i is three\n"); continue; }
  print("almost there...\n"); // Prints when i is 0 and 4
  if (i==4) { print("i is four\n"); break; }
}

// Same, written as a somewhat awkward while() loop
var i=-1;
while (i<5) {
  i++;
  if (i==1) { print("i is one\n"); continue; }
  if (i==2) { print("i is two\n"); continue; }
  if (i==3) { print("i is three\n"); continue; }
  print("almost there...\n"); // Prints when i is 0 and 4
  if (i==4) { print("i is four\n"); break; }
}

