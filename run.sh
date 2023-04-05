#!/bin/bash

build_dir="build"
if [ "${CMAKE_BUILD_TYPE,,}" = "release" ]; then
  build_dir="build_rel"
fi

launch_html="$build_dir/client.html"

if [ ! -f "$launch_html" ]; then
  echo "Project not yet built, building..."
  ./build.sh
fi

port=6939

echo "Launching $launch_html with emrun..."
emrun --port "$port" "$launch_html"
echo "Finished."
