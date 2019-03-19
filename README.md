# SimpleFS

This is one of the project that I developed as a "final test" during my bachelor degree in computer engineering.
In particular, this one is a simple filesystem written in C that must respect some complexity time constraints.

## Context

The goal of the Algorithms and Data Structures course project is to implement a simple hierarchical file system with storage only in main memory.
The hierarchical filesystem organizes the resources according to a tree structure, and identifies each resource uniquely through the path that links it to the root. The resources contained in a hierarchical file system can be files or directories. Both have a name, represented as a string of characters. For the files it is only possible to be inserted as leaves of the tree, while the directories can appear as both leaves and intermediate nodes.
The root of the tree is conventionally a directory, called the root directory. Only file nodes can contain data, represented as a sequence of bytes, while directories have no associated data. All tree nodes can contain metadata, but for the purposes of this project, only the directories contain them. The metadata of the directory are the names of its direct descendants.
The program that implements the filesystem will receive a diary of the actions to be performed from the standard input and will print the result of the same on standard output. The program must be implemented in standard C, uniquely with the help of the standard library (libc) and the basic runtime. The runtime of the program consists on reading a line of the diary of the actions, and performing the corresponding action on the internal representation of the filesystem maintained by it, and writing the result to standard output before proceeding to the next action (execution of actions is completely sequential).

##Commands
to be continued...

##Complexity constraints

- **__l__** is the path length
- **__d__** is the number of resources in the system
- **__d_path__** the number of children's resources of the specified one
- **__f__** is the number of resources found during a search operation


<img src="/img/complexity.jpg" height="300px" ></img>
