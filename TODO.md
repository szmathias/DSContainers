# TODOs for String View Library and Printing/IO Library

---

## String View Library

**Context:**  
A string view is a lightweight, non-owning reference to a sequence of characters, useful for efficient substring operations without copying data.

**Description:**  
Implement a `StringView` type that references a portion of a string (or char array) with a pointer and length.  
Provide utility functions for creating, slicing, comparing, and searching within string views.

**Examples:**
- Create a view of a substring:
  ```c
  StringView sv = strview_from_range(str_data(&s), 5, 10); // view of s[5..15]
  ```
- Compare two views:
  ```c
  bool eq = strview_equals(&sv1, &sv2);
  ```
- Find a character in a view:
  ```c
  ssize_t idx = strview_find(&sv, 'x');
  ```

**Tasks:**
- [ ] Define `StringView` struct (pointer + length)
- [ ] Add constructors: from string, from range, from C-string
- [ ] Add slicing and subview functions
- [ ] Add comparison and search functions
- [ ] Add documentation and tests

---

## Printing/IO Library for Strings and String Views

**Context:**  
Convenient and safe printing and file IO for both owned strings and string views.

**Description:**  
Implement functions for printing strings and string views to stdout, files, and buffers.  
Support formatted output and error handling.

**Examples:**
- Print a string view to stdout:
  ```c
  strview_print(&sv);
  ```
- Write a string to a file:
  ```c
  str_write_to_file(&s, "output.txt");
  ```
- Print with formatting:
  ```c
  str_printf("Value: %d, Text: %s\n", val, str_data(&s));
  ```

**Tasks:**
- [ ] Add `str_print`, `strview_print` for stdout
- [ ] Add `str_write_to_file`, `strview_write_to_file`
- [ ] Add formatted printing functions
- [ ] Add error handling and documentation
- [ ] Add tests for all IO functions

---

