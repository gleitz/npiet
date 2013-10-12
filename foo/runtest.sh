#!/bin/bash

maxe=10000000

if [ "$1" = "-g" ] ; then
  gen=1
  shift
fi

if [ "$1" = "-rnd" ] ; then
  rndopts="-rnd"
  shift
else
  rndopts=''
fi


if [ "$1" != "" ] ; then
  ../npiet-foogol -q $1
  if [ "$gen" = 1 ] ; then
    ../npiet -e $maxe npiet-foogol.png >& `basename $1 .foo`.out
    ls -l `basename $1 .foo`.out
  else
    ../npiet -e $maxe -tpic -te 1000 npiet-foogol.png
  fi

  exit 0
fi


for f in *.foo ; do
  b=`basename $f .foo`

  echo running test $f
  fail=0

  for wrap in 3173 2048 1024 222 128 66 48 24 23 22 16 15 14 13 12 11 10 ; do

   for rnd in '' $rndopts ; do

    echo -n "."

    if [ $fail = 0 ] ; then
      targ="-w $wrap"

      ../npiet-foogol $rnd -q $targ $f > /tmp/$b-tmp-$wrap.out
      ../npiet -e $maxe npiet-foogol.png </dev/null >> /tmp/$b-tmp-$wrap.out
      if diff -q $b.out /tmp/$b-tmp-$wrap.out >/dev/null ; then
	rm -f ./a.out 2>/dev/null
      else
        echo ==== file=$f wrap=$wrap rnd=$rnd ====
        diff -u $b.out /tmp/$b-tmp-$wrapout
        fail=1
      fi
      rm -f /tmp/$b-tmp-$wrap.out
    fi

    done
  done

  echo
done

exit 0
