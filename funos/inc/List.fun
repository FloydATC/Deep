

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
  var i = 0;
  while (item!=null) {
    function(item.value, i++);
    item = item.next;
  }
}


fun join_list(list, delimiter) {
  var str = "";
  fun concat(value, i) {
    if (i>0) { str = str + delimiter; }
    str = str + value;
  }
  foreach_list(list, concat);
  return str;
}

fun get_list(list, n) {
  var i=0;
  if (n >= list.length) { return null; } // Invalid
  var item = list.head;
  while (item!=null) {
    if (i==n) {
      //debug(str("get_list(",n,") =>", item.value));
      return item.value;
    }
    item = item.next;
    i++;
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
      //debug(str("set_list(",n,",",value,") ok"));
      return item.value;
    }
    item = item.next;
    i++;
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
      list.length = list.length - 1;
      return item.value;
    }
    item = item.next;
    i++;
  }
  return null; // Unreachable
}

fun ins_list(list, n, value) {
  var new = ListItem();
  new.value = value;
  new.prev = null;
  new.next = null;
  var i=0;
  // First and last items are special cases
  if (n == 0) { return unshift_list(list, value); }
  if (n >= list.length) { return push_list(list, value); }

  var item = list.head;
  while (item!=null) {
    if (i==n) {
      item.prev.next = new; // link to new from previous
      new.prev = item.prev; // link from new to previous
      new.next = item;      // link from new to current
      item.prev = new;      // link from current to new
      list.length = list.length + 1;
      return new.value;
    }
    item = item.next;
    i = i + 1;
  }
  return null; // Unreachable
}

// =============================

