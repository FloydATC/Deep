

fun run_tests(a, b) {
  debug("Testing binary operators:\n");
  print("NOT a  = ", (~a), "\n");
  print("NOT b  = ", (~b), "\n");
  print("a << 1 = ", (a << 1), "\n");
  print("b << 1 = ", (b << 1), "\n");
  print("a >> 1 = ", (a >> 1), "\n");
  print("b >> 1 = ", (b >> 1), "\n");
  print("a & b  = ", (a & b), "\n");
  print("a | b  = ", (a | b), "\n");
  print("a ^ b  = ", (a ^ b), "\n");
}

var a = 0b11100001;
var b = 0b10000111;

print("a = 0b11100001 = ", a, "\n");
print("b = 0b10000111 = ", b, "\n");
run_tests(a, b);

a = 0755;
b = 0644;

print("a = 0755 = ", a, "\n");
print("b = 0644 = ", b, "\n");
run_tests(a, b);

a = 0xdeadbeef;
b = 0x00C0FFEE;

print("a = 0xdeadbeef = ", a, "\n");
print("b = 0x00C0FFEE = ", b, "\n");
run_tests(a, b);

