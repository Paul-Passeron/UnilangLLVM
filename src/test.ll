  %var = alloca %string, align 8
  %0 = alloca %string, align 8
  call void @string_1(ptr %0, ptr @2)
  %a = load %string, ptr %0, align 8
  store %string %a, ptr %var, align 8
  %1 = alloca %string, align 8
  call void @string_1(ptr %1, ptr @3)
  %2 = load %string, ptr %1, align 8
  %3 = call %string @string_op_add(ptr %var, %string %2)
  %4 = alloca %string, align 8
  store %string %3, ptr %4, align 8
  call void @string_println(ptr %4)
  call void @string_destroy(ptr %var)
  call void @string_destroy(ptr %4)
  call void @string_destroy(ptr %1)
  ret i32 0

%var = alloca %string, align 8
call void @string_1(ptr %var, ptr @2)
%a = load %string, ptr %var, align 8
store %string %a, ptr %var, align 8
%0 = alloca %string, align 8
call void @string_1(ptr %0, ptr @3)
%1 = load %string, ptr %0, align 8
%2 = call %string @string_op_add(ptr %var, %string %1)
%3 = alloca %string, align 8
store %string %2, ptr %3, align 8
call void @string_println(ptr %3)
call void @string_destroy(ptr %3)
call void @string_destroy(ptr %0)
call void @string_destroy(ptr %var)
ret i32 0