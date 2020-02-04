
// ---- Linked list v1.0 ----

class List {}

// Initialize linked list
fun new_list() {
  var list = List();
  list.length = 0;
  list.head = null;
  list.tail = null;
  return list;
}

class ListItem {}

// Add item at the beginning
fun unshift_list(list, value) {
  var item = ListItem();
  item.value = value;
  item.prev = null;
  item.next = list.head;
  if (item.next==null) {
    // First
    list.tail = item;
  } else {
    // Attach self to beginning of list
    item.next.prev = item;
  }
  list.head = item;
  list.length = list.length + 1;
  return list.length;
}

// Add item at the end
fun push_list(list, value) {
  var item = ListItem();
  item.value = value;
  item.next = null;
  item.prev = list.tail;
  if (item.prev==null) {
    // First
    list.head = item;
  } else {
    // Attach self to end of list
    item.prev.next = item;
  }
  list.tail = item;
  list.length = list.length + 1;
  return list.length;
}

// Remove first item
fun shift_list(list) {
  if (list.length == 0) { return null; }
  var item = list.head;
  if (item.next == null) {
    list.tail = null;
  } else {
    item.next.prev = null;
  }
  list.head = item.next;
  list.length = list.length - 1;
  return item.value;
}

// Remove last item
fun pop_list(list) {
  if (list.length == 0) { return null; }
  var item = list.tail;
  if (item.prev == null) {
    list.head = null;
  } else {
    item.prev.next = null;
  }
  list.tail = item.prev;
  list.length = list.length - 1;
  return item.value;
}

// Loop through list
fun foreach_list(list, function) {
  var item = list.head;
  while (item!=null) {
    function(item.value);
    item = item.next;
  }
}


fun join_list(list, delimiter) {
  var str = "";
  var i=0;
  fun concat(value) {
    if (i>0) { str = str + delimiter; }
    str = str + value;
    i++;
  }
  foreach_list(list, concat);
  return str;
}

fun get_list(list, n) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  var item = list.head;
  while (item!=null) {
    if (i==n) { return item.value; }
    item = item.next;
    i = i + 1;
  }
  return null; // Unreachable
}

fun set_list(list, n, value) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  var item = list.head;
  while (item!=null) {
    if (i==n) { 
      item.value = value;
      return item.value; 
    }
    item = item.next;
    i = i + 1;
  }
  return null; // Unreachable
}

fun del_list(list, n) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  // First and last items are special cases
  if (n == 0) { return shift_list(list); } 
  if (n == list.length-1) { return pop_list(list); } 

  var item = list.head;
  while (item!=null) {
    if (i==n) { 
      item.prev.next = item.next;
      item.next.prev = item.prev;
      return item.value; 
    }
    item = item.next;
    i = i + 1;
  }
  return null; // Unreachable
}


// ---- tests below this line


// Make list
print("* Create:\n");
var items = new_list();
print(items, "\n");

// Populate it with strings
print("* Push:\n");
for (var i=0; i<8; i=i+1) {
  push_list(items, "_"+chr(97+i)+"_");
}
print("length=", items.length, "\n");

// Join
print("* Join:\n");
var str = join_list(items, ", ");
print(str + "\n");

// Set value at position
print("* Set number 4 (first=0):\n");
print(set_list(items, 4, "_E_") + "\n");

// Get value at position
print("* Get number 4 (first=0):\n");
print(get_list(items, 4) + "\n");

// Pop last value
print("* Pop:\n");
print(pop_list(items) + "\n");

// Shift first value
print("* Shift:\n");
print(shift_list(items) + "\n");

// Unshift replacement value
print("* Unshift:\n");
print(unshift_list(items, "TOP"), "\n");

// Delete specific value
print("* Delete 2:\n");
print(del_list(items, 2), "\n");

// Iterate over the list
print("* Foreach:\n");
fun show_item(value) { print(value, "\n"); }
foreach_list(items, show_item);



