function aux() {
  set -xe 
  projman -b
  ./bin/Unilang $1
  as tests/output.s -o tmp.o
  ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib64/crt1.o /usr/lib64/crti.o -lc tmp.o /usr/lib64/crtn.o ./stdlib/lib/ul_lib.a -o $2
  rm tmp.o
  # ./$2
}

# ./make_stdlib.sh
aux $1.ul $1