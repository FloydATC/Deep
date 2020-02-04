
var list_a = ["ok\n","error\n","error\n"];
print(list_a[0]);

var list_b = ["error\n","ok\n","error\n"];
print(list_b[1]);

var list_c = ["error\n","error\n","ok\n"];
print(list_c[2]);

var lol = [list_c, list_b, list_a];
print(lol[2][0]);
print(lol[1][1]);
print(lol[0][2]);

var deep = [[[["ok\n"]]]];
print(deep[0][0][0][0]);

var l1 = [0];
var l2 = [0];
var l3 = [0];
var l4 = ["ok\n"];
print(l4[l3[l2[l1[0]]]]);

print(["error\n", "error\n", "ok\n"][2]);
print(["error\n", "ok\n", "error\n"][1]);
print(["ok\n", "error\n", "error\n"][0]);

var list_one = ["error\n", "ok\n", "error\n"];
var list_two = list_one;
print(list_two[1]);

var list_three = list_one + list_two;
print(list_three[1]);
print(list_three[4]);


var list = ["error\n","error\n","error\n"];
list[0] = "ok\n";
print(list[0]); 

list[0] = "error\n";
list[1:] = ["error\n", "error\n", "error\n", "ok\n"];
print(list[4]); 
list[0:4] = [];
print(list[0]); 

list = ["error\n", "error\n", "error\n", "ok\n"][3:1];
print(list[0]); 

list = ["error\n", "error\n", "error\n"];
list.push("ok\n");
print(list[3]);

list.unshift("ok\n");
print(list[0]);

print(list.pop());
print(list.shift());




