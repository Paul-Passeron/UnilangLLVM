define i32 @main() {
entry:
  %var = alloca %string, align 8
  call void @string_1(ptr %var, ptr @2)
  %1 = load %string, ptr %var, align 8
  %2 = alloca %string, align 8
  store %string %1, ptr %2, align 8
 
  %3 = load %string, ptr %2, align 8
  %4 = alloca %string, align 8
  call void @string_0(ptr %4, %string %3)
  %5 = load %string, ptr %4, align 8
  %6 = alloca %string, align 8
  store %string %5, ptr %6, align 8
  call void @string_println(ptr %6)
  %7 = alloca %string, align 8
  call void @string_1(ptr %7, ptr @3)
  %8 = load %string, ptr %7, align 8
  %9 = alloca %string, align 8
  store %string %8, ptr %9, align 8
  %10 = load %string, ptr %9, align 8
  %11 = alloca %string, align 8
  call void @string_0(ptr %11, %string %10)
  %12 = load %string, ptr %11, align 8
  %13 = alloca %string, align 8
  store %string %12, ptr %13, align 8
  call void @string_println(ptr %13)
  call void @string_destroy(ptr %11)
  call void @string_destroy(ptr %7)
  call void @string_destroy(ptr %4)
  call void @string_destroy(ptr %var)
  ret i32 0
}