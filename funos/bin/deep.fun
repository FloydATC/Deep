
// This script tests nested for loops

// Intended use:
// 1. Enable execution tracing in FunC
// 2. Execute script
// 3. Observe stack

var i=1;
for (var a=0; a<10; a++) {
  for (var b=0; b<10; b++) {
    for (var c=0; c<10; c++) {
      for (var d=0; d<10; d++) {
        print("\r", i++, ": ", a+b+c+d, "  ");
      }
    }
  }
}
