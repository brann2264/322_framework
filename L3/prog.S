(@main
(@main
0
mem rsp -8 <- :entrance_global_1_ret
call @h 0
:entrance_global_1_retmem rsp -8 <- :entrance_global_2_ret
call @e 0
:entrance_global_2_retmem rsp -8 <- :entrance_global_3_ret
call @l 0
:entrance_global_3_retmem rsp -8 <- :entrance_global_4_ret
call @l 0
:entrance_global_4_retmem rsp -8 <- :entrance_global_5_ret
call @o 0
:entrance_global_5_retmem rsp -8 <- :entrance_global_6_ret
call @space 0
:entrance_global_6_retmem rsp -8 <- :entrance_global_7_ret
call @w 0
:entrance_global_7_retmem rsp -8 <- :entrance_global_8_ret
call @o 0
:entrance_global_8_retmem rsp -8 <- :entrance_global_9_ret
call @r 0
:entrance_global_9_retmem rsp -8 <- :entrance_global_10_ret
call @l 0
:entrance_global_10_retmem rsp -8 <- :entrance_global_11_ret
call @d 0
:entrance_global_11_retreturn
)
(@h
0
%var <- 209
mem rsp -8 <- :entrance_global_12_ret
rdi <- %var
call print 1
:entrance_global_12_retreturn
)
(@e
0
%var <- 203
mem rsp -8 <- :entrance_global_13_ret
rdi <- %var
call print 1
:entrance_global_13_retreturn
)
(@l
0
%var <- 217
mem rsp -8 <- :entrance_global_14_ret
rdi <- %var
call print 1
:entrance_global_14_retreturn
)
(@o
0
%var <- 223
mem rsp -8 <- :entrance_global_15_ret
rdi <- %var
call print 1
:entrance_global_15_retreturn
)
(@w
0
%var <- 239
mem rsp -8 <- :entrance_global_16_ret
rdi <- %var
call print 1
:entrance_global_16_retreturn
)
(@space
0
%var <- 65
mem rsp -8 <- :entrance_global_17_ret
rdi <- %var
call print 1
:entrance_global_17_retreturn
)
(@r
0
%var <- 229
mem rsp -8 <- :entrance_global_18_ret
rdi <- %var
call print 1
:entrance_global_18_retreturn
)
(@d
0
%var <- 201
mem rsp -8 <- :entrance_global_19_ret
rdi <- %var
call print 1
:entrance_global_19_retreturn
)
)
