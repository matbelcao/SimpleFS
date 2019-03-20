# SimpleFS


This is one of the project that I developed as a "final test" during my bachelor degree in computer engineering.
In particular, this one is a simple filesystem written in C that must respect some complexity time constraints.


## Description
The goal of the Algorithms and Data Structures course project is to implement a simple hierarchical file system with storage only in main memory.
The hierarchical filesystem organizes the resources according to a tree structure, and identifies each resource uniquely through the path that links it to the root. The resources contained in a hierarchical file system can be files or directories. Both have a name, represented as a string of characters. For the files it is only possible to be inserted as leaves of the tree, while the directories can appear as both leaves and intermediate nodes.
The root of the tree is conventionally a directory, called the root directory. Only file nodes can contain data, represented as a sequence of bytes, while directories have no associated data. All tree nodes can contain metadata, but for the purposes of this project, only the directories contain them. The metadata of the directory are the names of its direct descendants.
The program that implements the filesystem will receive a diary of the actions to be performed from the standard input and will print the result of the same on standard output. The program must be implemented in standard C, uniquely with the help of the standard library (libc) and the basic runtime. The runtime of the program consists on reading a line of the diary of the actions, and performing the corresponding action on the internal representation of the filesystem maintained by it, and writing the result to standard output before proceeding to the next action (execution of actions is completely sequential).


## Commands
The filesystem paths are represented with the usual **__UNIX syntax__**: a path is therefore the sequence of resource names that from the root directory reaches the resource identified by the path. The names are separated by the path separator character **__/__**).
For example, let's consider the following filesystem:

<br/>
<center><img src="/img/filesystem.jpg" height="200px"></img></center>
<br/>

The path that identifies the resource file0 is _/file0_, the one that identifies file3 is _/dir2/file3_.
The following __restrictions__ are applied:
- Resource names are alphanumeric and can be up to 255 characters long
- The maximum height of the tree is 255
- The maximum number of children of a node is 1024

The program receives one of the following __commands__ for each line of input, where _[path]_ indicates a generic path and _[name]_ an alphanumeric string of up to 255 characters.

- `create [path]` : The command has the effect of creating an empty file inside the filesystem. Output prints _"ok"_ if the file was created regularly, _"no"_ if the creation of the file was not successful (for example, if you try to create a file in a non-existent directory, or if you exceed the filesystem limits).
- `create_dir [path]` : Creates an empty directory within the filesystem. Prints _"ok"_ if the directory was created regularly, _"no"_ if the creation was not successful.
- `read [path]` : Reads the entire content of a file, printing in output _"content"_ followed by a space character and the content of the file, if the file doesn't exists prints _"no"_.
- `write [path>] [content>]` : Writes or overwrites the content of a file, which must already exist. Outputs _"ok"_ followed by the number of characters written if the writing is successful, _"no"_ otherwise. The _[content]_ parameter is an alphanumeric sequence of characters and spaces delimited by double quotes "".<br/>
__Example__: _write /poems /jabberwocky "It was brillig and the slithy toves"_
- `delete [path]` : Deletes a resource and prints the result (_"ok"_ - _"no"_). A resource can  be deleted only when it has no children.
- `delete_r [path]` : Deletes a resource and all its descendants, if they exists. Prints the outcome (_"ok"_ - _"no"_).
- `find [name]` : Finds the location of the resources in the filesystem. Outputs _"ok"_ followed by the path of the resource with the corrisponding _[name]_. Resource's paths must be printed in lexicographic order. The path separator characters must be considered in the ordering, therefore the sorting comparison have to be made between the entire strings representing paths. In case no resources are found, the command prints _"no"_.
- `exit` : Terminates the resource manager execution. Doesn't print anything in output.


## Complexity constraints

- **__l__** is the path length
- **__d__** is the number of resources in the system
- **__d_path__** the number of children's resources of the specified one
- **__f__** is the number of resources found during a search operation

<br/>
<center><img src="/img/complexity.jpg" height="250px" ></img><center>
