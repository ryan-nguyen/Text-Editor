# Project 1

Ryan Nguyen (811-221-139)

## Instructions

To clean my code (i.e., remove any compiled files), type:

```
$ make clean
```

To compile my code, type:

```
$ make
```

To run my code (after compiling it), type:

```
$ ./editor
```

OR

```
$ ./editor tester.txt
```

OR

```
$ ./editor [path of file to be read]
```

## Reflection

Compared to my initial brainstorm, the actual implementation of ncurses seemed very complex until I got the hang of it. It was very difficult to keep track of every command the user could input and to account for these inputs. I ended up having a lot of nested for loops and switch statements. Finding a way to store every input as well as the input already in the file that is opened into a buffer to read was also very difficult. The idea seems easy at first, but implementing arrow keys made it difficult to keep track of the current position in the buffer and how it would affect the buffer.

