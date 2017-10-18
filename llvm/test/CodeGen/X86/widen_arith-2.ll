; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc < %s -mtriple=i686-unknown-unknown -mattr=+sse4.2 | FileCheck %s

; widen v8i8 to v16i8 (checks even power of 2 widening with add & and)

define void @update(i64* %dst_i, i64* %src_i, i32 %n) nounwind {
; CHECK-LABEL: update:
; CHECK:       # BB#0: # %entry
; CHECK-NEXT:    subl $12, %esp
; CHECK-NEXT:    movl $0, (%esp)
; CHECK-NEXT:    pcmpeqd %xmm0, %xmm0
; CHECK-NEXT:    movdqa {{.*#+}} xmm1 = [4,4,4,4,4,4,4,4]
; CHECK-NEXT:    jmp .LBB0_1
; CHECK-NEXT:    .p2align 4, 0x90
; CHECK-NEXT:  .LBB0_2: # %forbody
; CHECK-NEXT:    # in Loop: Header=BB0_1 Depth=1
; CHECK-NEXT:    movl (%esp), %eax
; CHECK-NEXT:    leal (,%eax,8), %ecx
; CHECK-NEXT:    movl {{[0-9]+}}(%esp), %edx
; CHECK-NEXT:    addl %ecx, %edx
; CHECK-NEXT:    movl %edx, {{[0-9]+}}(%esp)
; CHECK-NEXT:    addl {{[0-9]+}}(%esp), %ecx
; CHECK-NEXT:    movl %ecx, {{[0-9]+}}(%esp)
; CHECK-NEXT:    pmovzxbw {{.*#+}} xmm2 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero
; CHECK-NEXT:    psubw %xmm0, %xmm2
; CHECK-NEXT:    pand %xmm1, %xmm2
; CHECK-NEXT:    packsswb %xmm0, %xmm2
; CHECK-NEXT:    movq %xmm2, (%edx,%eax,8)
; CHECK-NEXT:    incl (%esp)
; CHECK-NEXT:  .LBB0_1: # %forcond
; CHECK-NEXT:    # =>This Inner Loop Header: Depth=1
; CHECK-NEXT:    movl (%esp), %eax
; CHECK-NEXT:    cmpl {{[0-9]+}}(%esp), %eax
; CHECK-NEXT:    jl .LBB0_2
; CHECK-NEXT:  # BB#3: # %afterfor
; CHECK-NEXT:    addl $12, %esp
; CHECK-NEXT:    retl
entry:
	%dst_i.addr = alloca i64*
	%src_i.addr = alloca i64*
	%n.addr = alloca i32
	%i = alloca i32, align 4
	%dst = alloca <8 x i8>*, align 4
	%src = alloca <8 x i8>*, align 4
	store i64* %dst_i, i64** %dst_i.addr
	store i64* %src_i, i64** %src_i.addr
	store i32 %n, i32* %n.addr
	store i32 0, i32* %i
	br label %forcond

forcond:
	%tmp = load i32, i32* %i
	%tmp1 = load i32, i32* %n.addr
	%cmp = icmp slt i32 %tmp, %tmp1
	br i1 %cmp, label %forbody, label %afterfor

forbody:
	%tmp2 = load i32, i32* %i
	%tmp3 = load i64*, i64** %dst_i.addr
	%arrayidx = getelementptr i64, i64* %tmp3, i32 %tmp2
	%conv = bitcast i64* %arrayidx to <8 x i8>*
	store <8 x i8>* %conv, <8 x i8>** %dst
	%tmp4 = load i32, i32* %i
	%tmp5 = load i64*, i64** %src_i.addr
	%arrayidx6 = getelementptr i64, i64* %tmp5, i32 %tmp4
	%conv7 = bitcast i64* %arrayidx6 to <8 x i8>*
	store <8 x i8>* %conv7, <8 x i8>** %src
	%tmp8 = load i32, i32* %i
	%tmp9 = load <8 x i8>*, <8 x i8>** %dst
	%arrayidx10 = getelementptr <8 x i8>, <8 x i8>* %tmp9, i32 %tmp8
	%tmp11 = load i32, i32* %i
	%tmp12 = load <8 x i8>*, <8 x i8>** %src
	%arrayidx13 = getelementptr <8 x i8>, <8 x i8>* %tmp12, i32 %tmp11
	%tmp14 = load <8 x i8>, <8 x i8>* %arrayidx13
	%add = add <8 x i8> %tmp14, < i8 1, i8 1, i8 1, i8 1, i8 1, i8 1, i8 1, i8 1 >
	%and = and <8 x i8> %add, < i8 4, i8 4, i8 4, i8 4, i8 4, i8 4, i8 4, i8 4 >
	store <8 x i8> %and, <8 x i8>* %arrayidx10
	br label %forinc

forinc:
	%tmp15 = load i32, i32* %i
	%inc = add i32 %tmp15, 1
	store i32 %inc, i32* %i
	br label %forcond

afterfor:
	ret void
}

