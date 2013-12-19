#!/bin/bash -ue

script_path=$(dirname `readlink -f $0`)
script_name=$(basename `readlink -f $0`)

build_path="${script_path}/build"
install_path="${script_path}/install"

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

	mkdir -p "${build_path}"
	cd "${build_path}"
	cmake "${script_path}" -DCMAKE_INSTALL_PREFIX="${install_path}" -DCMAKE_BUILD_TYPE="$1"
}

function exec_check()
{
	echo "Checking..."

	cd "${build_path}"
	make check
}

function exec_uncrustify()
{
	echo "Uncrustifying..."

	cd "${script_path}"
	local files="$(find "${script_path}/src" "${script_path}/unittest" -name *.cpp -o -name *.hpp)"
	for file in $files; do
		uncrustify --replace --no-backup -c "${script_path}/uncrustify.cfg" -l CPP $file
	done
}

function exec_doc()
{
	echo "Documenting..."

	cd "${build_path}"
	make doc
}

function exec_build()
{
	echo "Building..."

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
	target_path="${build_path}/src/"
	tracefile="${target_path}/coverage.info"
	lcov_output_path="${build_path}/lcov"

	echo "Generating lcov output..."
	cd "${script_path}"
	exec_clean
	exec_bootstrap coverage
	exec_build
	lcov --zerocounters --gcov-tool gcov-4.8 --directory "${target_path}" --output-file "${tracefile}"
	exec_test
	lcov --capture --gcov-tool gcov-4.8 --directory "${target_path}" --output-file "${tracefile}"
	lcov --gcov-tool gcov-4.8 --remove "${tracefile}" "/usr/include/*" --output-file "${tracefile}"
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
			echo "error: invalid word: ${word}"
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
