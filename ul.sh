function aux() {
  set -xe
  objs=""
  for arg in "$@"; do
    ~/Documents/Unilang/bin/Unilang $arg.ul;
    as tests/output.s -o $arg.o;
    objs=$(echo "$objs $arg.o")
  done;
  ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib64/crt1.o /usr/lib64/crti.o -lc $objs /usr/lib64/crtn.o  ~/Documents/Unilang/stdlib/lib/ul_lib.a -o $1
  for obj in $objs; do
    rm $obj
  done;
}

aux $@
