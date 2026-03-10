(@main
(@main
0
%v1 <- 1
%v2 <- 2
%v4 <- %v2 < %v1
cjump %v4 = 1:false_global_1
goto :false_global_2
rax <- 0
return
rax <- %v4
return
)
)
