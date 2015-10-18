The libhutznohmd is a library, that helps the developers of a web service
implementing it according to the principles of the
[Representational State Transfer]
(http://www.ics.uci.edu/~fielding/pubs/dissertation/top.htm "Roy Thomas
Fieldings dissertation") architectural style by providing easy to use, reliable,
robust, well tested and (scalable) functionality.

Build
-----

Building is done by a shell script. It provides various build steps from
bootstrapping to packaging. It also provides several checking and code
improvement steps (e.g. clang-format, cppcheck, valgrind or code coverage).

    $ ./make --help

Examples
--------

Some examples are provided by another script:

    $ ./example --help

Documentation
-------------

For a complete doxygen documentation (including concepts), you can generate
it with:

    $ ./make doc

Then look at:

    $ ${YOUR_FAVORITE_BROWSER} ./build/html/index.html

Documentation is stored in doxygen comments directly in code. You need
doxygen and graphviz installed to build it. For UML support the project
utilizes PlantUML and therefore you will also need Java installed. The script
will download PlantUML automatically to the build folder before building the
documentation.

