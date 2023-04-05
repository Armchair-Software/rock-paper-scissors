#!/bin/bash

infile="$1"
if [ -z "$infile" ]; then
  echo "Usage: $0 <filename> [namespace]" 2>&1
  exit 1
fi

if [ ! -f "$infile" ]; then
  echo "Resource compiler: File $infile not found." 2>&1
  exit 1
fi

outfile="$1.h"

# don't update if outfile is newer than infile
if [ ! "$infile" -nt "$outfile" ]; then
  echo "Resource compiler: $outfile up to date"
  exit
fi

# generate a fairly unique hash to act as a rawstring prefix/suffix
shorthash=$(md5sum "$infile" | cut -c 1-16)

if [ -z "$2" ]; then
  namespace=$(dirname "$infile" | sed 's/\//::/g')
else
  namespace="$2"
fi
resourcename=$(basename "$infile" | sed 's/\./_/g')

function string_head() {
  # raw string head
  echo -n "R\"${shorthash}("
}
function string_tail() {
  # raw string tail
  echo -n ")${shorthash}\""
}


# truncate the destination file
> "$outfile"

# optional namespace head
if [ ! -z "$namespace" ]; then
  echo "namespace $namespace {" >> "$outfile"
fi

echo -n "inline constexpr const char *${resourcename}{$(string_head)" >> "$outfile"

while IFS= read -r line; do
  # remove blank lines
  if grep -q '^\s*$' <<< "$line"; then continue; fi

  # preprocess special comments - these are uncommented and inserted between string sections
  if grep -q '^\s*//#ifdef \|^\s*//#ifndef \|^\s*//#endif ' <<< "$line"; then
    echo "$(string_tail)" >> "$outfile"
    sed 's/\s*\/\///' <<< "$line" >> "$outfile"
    echo "$(string_head)" >> "$outfile"
    continue
  fi

  # remove commented lines entirely
  if grep -q '^\s*//' <<< "$line"; then continue; fi

  # strip other trailing line comments
  line=$(sed 's/\s*\/\/.*//' <<< "$line")

  echo "$line" >> "$outfile"
done < "$infile"

echo "$(string_tail)};" >> "$outfile"

# optional namespace tail
if [ ! -z "$namespace" ]; then
  echo "}" >> "$outfile"
fi

echo "Resource compiler: $infile compiled to $outfile: $namespace::$resourcename"
