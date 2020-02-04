
// This script tests nested if statements

// Intended use:
// 1. Enable execution tracing in FunC
// 2. Execute script
// 3. Observe stack

/* Expected results:
0146   66 OP_LOOP          -> 000b
          [ <script> ]
000b    6 OP_GET_GLOBAL       5 'running'
          [ <script> ][ false ]
000d    | OP_JUMP_IF_FALSE -> 0149
          [ <script> ][ false ]
0149    | OP_POP
          [ <script> ]
014a   68 OP_NULL
          [ <script> ][ null ]
014b    | OP_RETURN
*/

var i=0;
var j=0;
var running = true;
while (running) {
  if (j>=1) {
    var a = j;
    j=j/2;
    print(">");
    if (j>=1) {
      var b = j;
      j=j/2;
      print(">");
      if (j>=1) {
        var c = j;
        j=j/2;
        print(">");
        if (j>=1) {
         var d = j;
          j=j/2;
          print(">");
          if (j>=1) {
            var e = j;
            j=j/2;
            print(">");
            if (j>=1) {
              var f = j;
              j=j/2;
              print(">");
              if (j>=1) {
                var g = j;
                j=j/2;
                print(">");
                if (j>=1) {
                  var h = j;
                  print(">\n");
                  running = false;
                } else {
                  print("\n");
                }
              } else {
                print("\n");
              }
            } else {
              print("\n");
            }
          } else {
            print("\n");
          }
        } else {
          print("\n");
        }
      } else {
        print("\n");
      }
    } else {
      print("\n");
    }
  } else {
    print("\n");
  }


  j = i++;
}

