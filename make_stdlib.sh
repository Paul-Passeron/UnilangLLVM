set -xe 
projman -b
for f in ./stdlib/std/*.ul; do
  ./bin/Unilang $f;
  mv tests/output.s $f.s;
  as $f.s -o $f.o;
  rm $f.s;
  ar rvs stdlib/lib/ul_lib.a $f.o;
  rm $f.o;
done