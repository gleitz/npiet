#!/bin/bash


for f in hi.??? nfib.??? nprime.??? loop.??? nhello.??? ; do
  echo "**"
  echo "** running: npiet $f:"
  echo "**"
  echo -- output start --
  if [ $f = nprime.gif ] ; then
      echo 991 | ../npiet $f
  elif [ $f = nprime.ppm ] ; then
      echo 1000 | ../npiet $f
  else
      ../npiet $f
  fi
  echo -- output end --
  echo
done

echo "**"
echo "** running: npiet -te 0 check - expect two Hi:"
echo "**"
rm -f npiet-trace.png 2>/dev/null
../npiet -te 0 hi.png
../npiet npiet-trace.png
rm -f npiet-trace.png 2>/dev/null
