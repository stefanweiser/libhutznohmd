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

The newest documentation is available at:

    https://doc.stefanweiser.de/libhutznohmd/

You can also generate a complete doxygen documentation (including concepts) by:

    $ ./make doc

Then look at:

    $ ${YOUR_FAVORITE_BROWSER} ./build/html/index.html

Documentation is stored in doxygen comments directly in code. You need
doxygen and graphviz installed to build it. For UML support the project
utilizes PlantUML and therefore you will also need Java installed. The script
will download PlantUML automatically to the build folder before building the
documentation.

Software Quality
----------------

Continuous inspection of source code quality with a very strict rule set is done
on:

    https://sonar.stefanweiser.de/overview?id=libhutznohmd

Continuous Integration
----------------------

Completed builds, build stability information and nightly builds are available
at:

    https://ci.stefanweiser.de/job/libhutznohmd/
