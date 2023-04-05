#!/bin/bash

# redirect all stdout to stderr
exec 1>&2

build_dir="build"
if [ "${CMAKE_BUILD_TYPE,,}" = "release" ]; then
  build_dir="build_rel"
fi

target="$1"
if [ -z "$target" ]; then target="client"; fi

if [ "$target" = "clean" ]; then
  echo "Cleaning build directory \"$build_dir\"..."
  rm -r "$build_dir"
  target="client"
fi

procs=$(nproc)
./generate_git_version.sh

if [ ! -d "$build_dir" ]; then
  echo "Creating build directory \"$build_dir\"..."
  mkdir "$build_dir"
fi

cd "$build_dir"

if [ ! -z "$(which ccache)" ]; then
  echo "CCache enabled"
  export EM_COMPILER_WRAPPER=ccache
fi

if [ ! -f "Makefile" ] || [ "../CMakeLists.txt" -nt "Makefile" ]; then
  echo "Running CMAKE to generate Makefile..."
  emcmake cmake .. || exit 1
fi

echo "Running make..."
#emmake make -j"$procs" VERBOSE=1 "$target" || exit 1
emmake make -j"$procs" "$target" || exit 1

echo "Assembling resources..."
cd ..
rsync -ar --progress "resources/"* "$build_dir/"
echo "Done."
