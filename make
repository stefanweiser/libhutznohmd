#!/bin/bash -ue

binary_cmake="cmake"
binary_gpp="g++"
binary_gcov="gcov"
binary_cppcheck="cppcheck"
binary_uncrustify="uncrustify"
binary_doxygen="doxygen"
binary_dot="dot"
binary_lcov="lcov"
binary_rpmbuild="rpmbuild"

min_version_cmake="2.8"
min_version_gpp="4.8"
min_version_gcov="4.8"
min_version_cppcheck="1.60"
min_version_uncrustify="0.60"
min_version_doxygen="1.7.6"
min_version_dot="2.26"
min_version_lcov="1.9"
min_version_rpmbuild="4.9"

script_path=$(dirname `readlink -f $0`)
script_name=$(basename `readlink -f $0`)

build_path="${script_path}/build"
install_path="${script_path}/install"

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

function check_existing()
{
	if command -v "$1" > /dev/null; then
		return 0
	fi

	echo "ERROR: Unable to find program $1."
	return 1
}

function check_cmake()
{
	check_existing "${binary_cmake}"
	local cmake_version=$(${binary_cmake} --version | tr ' ' '\n' | tail -n 1)
	check_version "${binary_cmake}" "${cmake_version}" "${min_version_cmake}"
}

function check_compiler()
{
	check_existing "${binary_gpp}"
	local gpp_version=$(${binary_gpp} --version | head -n 1 | tr ' ' '\n' | tail -n 1)
	check_version "${binary_gpp}" "${gpp_version}" "${min_version_gpp}"
}

function check_gcov()
{
	check_existing "${binary_gcov}"
	local gcov_version=$(${binary_gcov} --version | head -n 1 | tr ' ' '\n' | tail -n 1)
	check_version "${binary_gcov}" "${gcov_version}" "${min_version_gcov}"
}

function check_cppcheck()
{
	check_existing "${binary_cppcheck}"
	local cppcheck_version=$(${binary_cppcheck} --version | tr ' ' '\n' | tail -n 1)
	check_version "${binary_cppcheck}" "${cppcheck_version}" "${min_version_cppcheck}"
}

function check_uncrustify()
{
	check_existing "${binary_uncrustify}"
	local uncrustify_version=$(${binary_uncrustify} --version | tr ' ' '\n' | tail -n 1)
	check_version "${binary_uncrustify}" "${uncrustify_version}" "${min_version_uncrustify}"
}

function check_doxygen()
{
	check_existing "${binary_doxygen}"
	local doxygen_version=$(${binary_doxygen} --version)
	check_version "${binary_doxygen}" "${doxygen_version}" "${min_version_doxygen}"
}

function check_dot()
{
	check_existing "${binary_dot}"
	local dot_version=$(${binary_dot} -V 2>&1 | tr ' ' '\n' | tail -n2 | head -n1)
	check_version "${binary_dot}" "${dot_version}" "${min_version_dot}"
}

function check_lcov()
{
	check_existing "${binary_lcov}"
	local lcov_version=$(${binary_lcov} --version | tr ' ' '\n' | tail -n 1)
	check_version "${binary_lcov}" "${lcov_version}" "${min_version_lcov}"
}

function check_rpmbuild()
{
	check_existing "${binary_rpmbuild}"
	local rpmbuild_version=$(${binary_rpmbuild} --version | tr ' ' '\n' | tail -n 1)
	check_version "${binary_rpmbuild}" "${rpmbuild_version}" "${min_version_rpmbuild}"
}

function usage()
{
	echo ""
	echo "  This script assists you to build librest."
	echo ""
	echo "  usage: ${script_name} [step] [target] [options]"
	echo ""
	echo "  Steps:"
	echo "   clean       : Removes all build output."
	echo "   check       : Checks all sources."
	echo "   uncrustify  : Uncrustifying all sources."
	echo "   doc         : Builds documentation."
	echo "   bootstrap   : Bootstraps the build."
	echo "   build       : Builds the targets."
	echo "   test        : Tests the targets."
	echo "   install     : Installs the targets."
	echo "   package     : Builds packages."
	echo "   all         : Builds all steps."
	echo "   coverage    : Generates lcov output."
	echo ""
	echo "  Targets (only needed for bootstraping):"
	echo "   debug"
	echo "   release"
	echo "   coverage"
	echo ""
	echo "  Options:"
	echo "   -h"
	echo "   --help        : Displays this help information"
	echo ""
	echo "  Example (complete debug):"
	echo "    ${script_name} complete debug"
	echo ""
}

function exec_clean()
{
	echo "Cleaning..."

	rm -rf "${build_path}" "${install_path}"
}

function exec_bootstrap()
{
	echo "Bootstrapping..."

	check_cmake

	mkdir -p "${build_path}"
	cd "${build_path}"
	cmake "${script_path}" -DCMAKE_INSTALL_PREFIX="${install_path}" -DCMAKE_BUILD_TYPE="$1"
}

function exec_check()
{
	echo "Checking..."

	check_cppcheck

	cd "${build_path}"
	make check
}

function exec_uncrustify()
{
	echo "Uncrustifying..."

	check_uncrustify

	cd "${script_path}"
	local files="$(find "${script_path}/src" "${script_path}/unittest" -name *.cpp -o -name *.hpp)"
	for file in $files; do
		uncrustify --replace --no-backup -c "${script_path}/uncrustify.cfg" -l CPP $file
	done
}

function exec_doc()
{
	echo "Documenting..."

	check_doxygen
	check_dot

	cd "${build_path}"
	make doc
}

function exec_build()
{
	echo "Building..."

	check_compiler

	cd "${build_path}"
	make -j"$(grep processor /proc/cpuinfo | wc -l)" all
}

function exec_test()
{
	echo "Testing..."

	cd "${build_path}"
	LD_LIBRARY_PATH="${build_path}/src" "${build_path}/unittest/unittest_rest"
	LD_LIBRARY_PATH="${build_path}/src" "${build_path}/integrationtest/integrationtest_rest"
}

function exec_install()
{
	echo "Installing..."

	cd "${build_path}"
	make install
}

function exec_package()
{
	echo "Packaging..."

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

	echo "Generating lcov output..."
	cd "${script_path}"
	exec_clean
	exec_bootstrap coverage
	exec_build
	lcov --zerocounters --directory "${target_path}" --output-file "${tracefile}"
	exec_test
	lcov --capture --directory "${target_path}" --output-file "${tracefile}"
	lcov --remove "${tracefile}" "/usr/include/*" --output-file "${tracefile}"
	rm -rf "${lcov_output_path}"
	genhtml "${tracefile}" --output-directory "${lcov_output_path}"
}

opts_words=()
opts_clean=0
opts_bootstrap=0
opts_check=0
opts_uncrustify=0
opts_doc=0
opts_build=0
opts_test=0
opts_install=0
opts_package=0
opts_coverage=0
opts_build_type="debug"

args=$(getopt \
	--options "h" \
	--longopt "help" \
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
		uncrustify)
			opts_uncrustify=1
			;;
		doc)
			opts_doc=1
			;;
		build)
			opts_build=1
			;;
		test)
			opts_test=1
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
			opts_uncrustify=1
			opts_doc=1
			opts_build=1
			opts_test=1
			opts_install=1
			opts_package=1
			;;
		coverage)
			opts_coverage=1
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

if [ ${opts_uncrustify} -ne 0 ]; then
	exec_uncrustify
fi

if [ ${opts_doc} -ne 0 ]; then
	exec_doc
fi

if [ ${opts_build} -ne 0 ]; then
	exec_build
fi

if [ ${opts_test} -ne 0 ]; then
	exec_test
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

exit 0
