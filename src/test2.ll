define i32 @main() {
entry:
  %var = alloca %foo, align 8
  call void @foo_0(ptr %var, i32 15)
  %f1 = load %foo, ptr %var, align 8
  store %foo %f1, ptr %var, align 8
  %var1 = alloca %foo, align 8
  call void @foo_0(ptr %var1, i32 15)
  %f2 = load %foo, ptr %var1, align 8
  store %foo %f2, ptr %var1, align 8
  call void @foo_destroy(ptr %var)
  ret i32 0
}