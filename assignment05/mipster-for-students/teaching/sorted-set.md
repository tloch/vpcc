# Sorted Sets in C++

The following example can be compiled and executed using

```sh
gcc -lstdc++ main.cc
./a.out
```

The difference between [`std:set`][1] and [`std::multiset`][2] is that `multiset` allows storing objects with same keys.

```c++
#include <stdlib.h>

#include <set>

class MyObject {
 public:
  MyObject(const char* name, int priority) : name_(name), priority_(priority) {}
  ~MyObject() {}

  int priority() const { return priority_; }
  const char* name() const { return name_; }

 private:
  int priority_;
  const char* name_;
};


struct MyObjectCompare {
  bool operator() (const MyObject* lhs, const MyObject* rhs) const {
    return lhs->priority() < rhs->priority();
  }
};


// For the lazy typers.
typedef std::multiset<MyObject*, MyObjectCompare> SortedSet;


int main(int argc, char** argv) {
  SortedSet sorted_set;

  MyObject* a = new MyObject("a", 1);
  MyObject* b = new MyObject("b", 3);
  MyObject* c = new MyObject("c", 2);
  MyObject* d = new MyObject("d", 42);
  MyObject* e = new MyObject("e", 2);

  sorted_set.insert(a);
  sorted_set.insert(b);
  sorted_set.insert(c);
  sorted_set.insert(d);
  sorted_set.insert(e);

  // The easy part: Just print them.
  printf("Printing all objects in set:\n");
  for(SortedSet::iterator it = sorted_set.begin();
      it != sorted_set.end();
      ++it) {
    printf("  MyObject '%s' of priority: %d\n",
        (*it)->name(), (*it)->priority());
  }

  // Remove all MyObject objects with priority < 3.
  const int priority_limit = 3;
  printf("Removing all with priority < %d\n", priority_limit);
  for(SortedSet::iterator it = sorted_set.begin();
      it != sorted_set.end();
      /* iterator is handled in the body of the loop */) {
    if ((*it)->priority() < priority_limit) {
      // Erase returns the iterator following the erased one. 
      // Unfortunatelly it also destroys the old iterator.
      it = sorted_set.erase(it);
    } else {
      // Regular increment to next iterator.
      ++it;
    }
  }

  // One more print loop.
  printf("Printing all objects in set:\n");
  for(SortedSet::iterator it = sorted_set.begin();
      it != sorted_set.end();
      ++it) {
    printf("  MyObject '%s' of priority: %d\n",
        (*it)->name(), (*it)->priority());
  }
  
  // Properly destroy objects. (Not done here.)

  return EXIT_SUCCESS;
}
```

[1]: http://www.cplusplus.com/reference/set/set/
[2]: http://www.cplusplus.com/reference/set/multiset/
