Mission Statement
-----------------

To help the architects and developers of a webservice implementing it according to the principles of the [Representational State Transfer](http://www.ics.uci.edu/~fielding/pubs/dissertation/top.htm "Roy Thomas Fieldings dissertation") architectural style by supporting an easy to use, reliable, robust, scalable and well tested library.

Build
-----

Build is done by the make script. It provides various build steps from bootstrapping to packaging. It also provides several checking and code improvement steps (e.g. astyle, cppcheck, valgrind or code coverage).

    $ ./make --help

Examples
--------

Some examples are provided by an example script:

    $ ./example --help

Documentation
-------------

For a complete doxygen documentation (including concepts), you can generate it with:

    $ ./make bootstrap doc

Then look at:

    $ firefox ./build/html/index.html

Documentation is stored in the code through doxygen comments. You need doxygen and graphviz installed to build it.
For UML support the project utilizes PlantUML and therefore you will also need Java installed. The script will download PlantUML, if not exists, to the temporary folder before building the documentation.

