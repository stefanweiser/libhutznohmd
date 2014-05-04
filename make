#!/bin/bash -ue

function get_command_path()
{
    if command -v "$1"; then
        return 0
    fi
    echo "ERROR: Unable to find program $1." 1>&2
    return 1
}

function check_version()
{
    local found=( $(echo "$2" | tr '.' ' ') )
    local minimum=( $(echo "$3" | tr '.' ' ') )

    for i in "${!minimum[@]}"; do
        if [ ${found[$i]} -lt ${minimum[$i]} ]; then
            echo "ERROR: Need at least version $3 of program $1, but only $2 was found."
            return 1
        elif [ ${found[$i]} -gt ${minimum[$i]} ]; then
            echo "INFO: Fulfilling need for version $3 of program $1 with version $2."
            return 0
        fi
    done
    echo "INFO: Fulfilling need for version $3 of program $1 with version $2."
    return 0
}

binary_cmake="$(get_command_path cmake)"
binary_gpp="$(get_command_path g++)"
binary_gcov="$(get_command_path gcov)"
binary_cppcheck="$(get_command_path cppcheck)"
binary_astyle="$(get_command_path astyle)"
binary_doxygen="$(get_command_path doxygen)"
binary_dot="$(get_command_path dot)"
binary_java="$(get_command_path java)"
binary_lcov="$(get_command_path lcov)"
binary_genhtml="$(get_command_path genhtml)"
binary_rpmbuild="$(get_command_path rpmbuild)"
binary_valgrind="$(get_command_path valgrind)"

min_version_cmake="2.8"
min_version_gpp="4.8"
min_version_gcov="4.8"
min_version_cppcheck="1.60"
min_version_astyle="2.03"
min_version_doxygen="1.7.6"
min_version_dot="2.26"
min_version_java="1.7"
min_version_lcov="1.9"
min_version_rpmbuild="4.9"
min_version_valgrind="3.7.0"

script_path=$(dirname `readlink -f $0`)
script_name=$(basename `readlink -f $0`)

build_path="${script_path}/build"
install_path="${script_path}/install"

function check_cmake()
{
    local cmake_version=$(${binary_cmake} --version | tr ' ' '\n' | tail -n 1)
    check_version "${binary_cmake}" "${cmake_version}" "${min_version_cmake}"
}

function check_compiler()
{
    local gpp_version=$(${binary_gpp} --version | head -n 1 | tr ' ' '\n' | tail -n 1)
    check_version "${binary_gpp}" "${gpp_version}" "${min_version_gpp}"
}

function check_gcov()
{
    local gcov_version=$(${binary_gcov} --version | head -n 1 | tr ' ' '\n' | tail -n 1)
    check_version "${binary_gcov}" "${gcov_version}" "${min_version_gcov}"
}

function check_cppcheck()
{
    local cppcheck_version=$(${binary_cppcheck} --version | tr ' ' '\n' | tail -n 1)
    check_version "${binary_cppcheck}" "${cppcheck_version}" "${min_version_cppcheck}"
}

function check_astyle()
{
    local astyle_version=$(${binary_astyle} --version 2>&1 | tr ' ' '\n' | tail -n 1)
    check_version "${binary_astyle}" "${astyle_version}" "${min_version_astyle}"
}

function check_doxygen()
{
    local doxygen_version=$(${binary_doxygen} --version)
    check_version "${binary_doxygen}" "${doxygen_version}" "${min_version_doxygen}"
}

function check_dot()
{
    local dot_version=$(${binary_dot} -V 2>&1 | tr ' ' '\n' | tail -n2 | head -n1)
    check_version "${binary_dot}" "${dot_version}" "${min_version_dot}"
}

function check_java()
{
    local java_version=$(${binary_java} -version 2>&1 | tr ' ' '\n' | head -n3 | tail -n1 | tr -d '"')
    check_version "${binary_java}" "${java_version}" "${min_version_java}"
}

function check_lcov()
{
    local lcov_version=$(${binary_lcov} --version | tr ' ' '\n' | tail -n 1)
    check_version "${binary_lcov}" "${lcov_version}" "${min_version_lcov}"
}

function check_rpmbuild()
{
    local rpmbuild_version=$(${binary_rpmbuild} --version | tr ' ' '\n' | tail -n 1)
    check_version "${binary_rpmbuild}" "${rpmbuild_version}" "${min_version_rpmbuild}"
}
 
function check_valgrind()
{
    local valgrind_version=$(${binary_valgrind} --version | tr '-' '\n' | tail -n 1)
    check_version "${binary_valgrind}" "${valgrind_version}" "${min_version_valgrind}"
}

function usage()
{
    echo ""
    echo "  This script assists you to build librestsrv."
    echo ""
    echo "  usage: ${script_name} [step] [target] [options]"
    echo ""
    echo "  Steps:"
    echo "   all             : Builds all steps to make a package."
    echo "   astyle          : Formats all sources."
    echo "   bootstrap       : Bootstraps the build."
    echo "   build           : Builds the targets."
    echo "   check           : Checks all sources."
    echo "   clean           : Removes all build output."
    echo "   coverage        : Generates lcov output."
    echo "   doc             : Builds documentation."
    echo "   install         : Installs the targets."
    echo "   integrationtest : Executes integration tests."
    echo "   package         : Builds packages."
    echo "   test            : Executes unit and integration tests."
    echo "   unittest        : Executes unit tests."
    echo "   valgrind        : Check all tests with valgrind."
    echo ""
    echo "  Targets (only needed for bootstraping):"
    echo "   coverage"
    echo "   debug"
    echo "   release"
    echo ""
    echo "  Options:"
    echo "   -h"
    echo "   --help        : Displays this help information"
    echo "   -s"
    echo "   --summary     : Prints only important informations and skips other"
    echo ""
    echo "  Example (clean, bootstrap, check, astyle, doc, build, test, install, package):"
    echo "    ${script_name} all debug"
    echo ""
    echo "  Example (do a cppcheck):"
    echo "    ${script_name} check"
    echo ""
}

function exec_clean()
{
    rm -rf "${build_path}" "${install_path}"
}

function exec_bootstrap()
{
    check_cmake

    mkdir -p "${build_path}"
    cd "${build_path}"
    ${binary_cmake} "${script_path}" -DCMAKE_INSTALL_PREFIX="${install_path}" -DCMAKE_BUILD_TYPE="$1"
}

function exec_check()
{
    check_cppcheck

    cd "${build_path}"
    make check
}

function exec_astyle()
{
    check_astyle

    cd "${script_path}"
    local files="$(find "${script_path}/src" "${script_path}/integrationtest" "${script_path}/unittest" "${script_path}/examples" -name *.cpp -o -name *.hpp -o -name *.h)"
    for file in $files; do
        ${binary_astyle} --options="${script_path}/astyle.rc" $file
    done
}

function exec_doc()
{
    check_doxygen
    check_dot
    check_java

    local plantuml_jar="/tmp/plantuml.jar"
    if [ ! -f "${plantuml_jar}" ]; then
        wget "http://sourceforge.net/projects/plantuml/files/plantuml.jar/download" -O "${plantuml_jar}"
    fi

    java -Djava.awt.headless=true -jar "${plantuml_jar}" -v -o "${build_path}/html" "./src/**.(c|cpp|h|hpp)"
    cd "${build_path}"
    make doc
}

function exec_build()
{
    check_compiler

    cd "${build_path}"
    make -j"$(grep processor /proc/cpuinfo | wc -l)" all
}

function exec_unittest()
{
    cd "${build_path}"
    if [ "${opts_summary}" -ne 0 ]; then
        LD_LIBRARY_PATH="${build_path}/src" "${build_path}/unittest/unittest_restsrv" | grep -E "\[==========\]|\[\ \ PASSED\ \ \]"
    else
        LD_LIBRARY_PATH="${build_path}/src" "${build_path}/unittest/unittest_restsrv"
    fi
}

function exec_integrationtest()
{
    cd "${build_path}"
    if [ "${opts_summary}" -ne 0 ]; then
        LD_LIBRARY_PATH="${build_path}/src" "${build_path}/integrationtest/integrationtest_restsrv" | grep -E "\[==========\]|\[\ \ PASSED\ \ \]"
    else
        LD_LIBRARY_PATH="${build_path}/src" "${build_path}/integrationtest/integrationtest_restsrv"
    fi
}

function exec_install()
{
    cd "${build_path}"
    make install
}

function exec_package()
{
    cd "${build_path}"
    make package
}

function exec_coverage()
{
    check_gcov
    check_lcov

    local target_path="${build_path}/src/"
    local tracefile="${target_path}/coverage.info"
    local lcov_output_path="${build_path}/lcov"

    cd "${script_path}"
    exec_clean
    exec_bootstrap coverage
    exec_build
    ${binary_lcov} -c -i -d "${target_path}" -o "${tracefile}.base"
    exec_unittest
    exec_integrationtest
    ${binary_lcov} -c -d "${target_path}" -o "${tracefile}.test"
    ${binary_lcov} -a "${tracefile}.base" -a "${tracefile}.test" -o "${tracefile}"
    ${binary_lcov} -r "${tracefile}" "/usr/include/*" -o "${tracefile}"
    ${binary_genhtml} "${tracefile}" --output-directory "${lcov_output_path}"
}

function exec_valgrind()
{
    cd "${build_path}"
    LD_LIBRARY_PATH="${build_path}/src" ${binary_valgrind} "${build_path}/unittest/unittest_restsrv"
    LD_LIBRARY_PATH="${build_path}/src" ${binary_valgrind} "${build_path}/integrationtest/integrationtest_restsrv"
}

opts_words=()
opts_clean=0
opts_bootstrap=0
opts_check=0
opts_astyle=0
opts_doc=0
opts_build=0
opts_unittest=0
opts_integrationtest=0
opts_install=0
opts_package=0
opts_coverage=0
opts_valgrind=0
opts_build_type="debug"
opts_summary=0

args=$(getopt \
    --options "h" \
    --longopt "help" \
    --longopt "summary" \
    -- "$@")
if [ $? != 0 ]; then
    usage
    exit 1
fi

eval set -- ${args}
while [ $# -ne 0 ]; do
    case "$1" in
        -h)
            usage
            exit 1
            ;;
        --help)
            usage
            exit 1
            ;;
        --summary)
                        opts_summary=1
                        ;;
        *)
            if [ $# -gt 1 ] ; then
                opts_words[${#opts_words[*]}]=$2
            fi
            ;;
    esac
    shift
done

if [ ${#opts_words[*]} -eq 0 ] ; then
    opts_words[0]="build"
    opts_words[1]="debug"
fi

for word in ${opts_words[*]} ; do
    case "${word}" in
        clean)
            opts_clean=1
            ;;
        bootstrap)
            opts_bootstrap=1
            ;;
        check)
            opts_check=1
            ;;
        astyle)
            opts_astyle=1
            ;;
        doc)
            opts_doc=1
            ;;
        build)
            opts_build=1
            ;;
        test)
            opts_unittest=1
            opts_integrationtest=1
            ;;
        unittest)
            opts_unittest=1
            ;;
        integrationtest)
            opts_integrationtest=1
            ;;
        install)
            opts_install=1
            ;;
        package)
            opts_package=1
            ;;
        all)
            opts_clean=1
            opts_bootstrap=1
            opts_check=1
            opts_astyle=1
            opts_build=1
            opts_unittest=1
            opts_integrationtest=1
            opts_install=1
            opts_package=1
            ;;
        coverage)
            opts_coverage=1
            ;;
        valgrind)
            opts_valgrind=1
            ;;

        debug)
            opts_build_type="debug"
            ;;
        release)
            opts_build_type="release"
            ;;
        coverage)
            opts_build_type="coverage"
            ;;

        *)
            echo "ERROR: Found invalid argument. (${word})"
            exit 1
            ;;
    esac
done

if [ ${opts_clean} -ne 0 ]; then
    exec_clean
fi

if [ ${opts_bootstrap} -ne 0 ]; then
    exec_bootstrap ${opts_build_type}
fi

if [ ${opts_check} -ne 0 ]; then
    exec_check
fi

if [ ${opts_astyle} -ne 0 ]; then
    exec_astyle
fi

if [ ${opts_doc} -ne 0 ]; then
    exec_doc
fi

if [ ${opts_build} -ne 0 ]; then
    exec_build
fi

if [ ${opts_unittest} -ne 0 ]; then
    exec_unittest
fi

if [ ${opts_integrationtest} -ne 0 ]; then
    exec_integrationtest
fi

if [ ${opts_install} -ne 0 ]; then
    exec_install
fi

if [ ${opts_package} -ne 0 ]; then
    exec_package
fi

if [ ${opts_coverage} -ne 0 ]; then
    exec_coverage
fi

if [ ${opts_valgrind} -ne 0 ]; then
    exec_valgrind
fi

exit 0

