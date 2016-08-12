; RUN: opt < %s -memcpyopt -S | FileCheck %s
; Make sure memcpy-memcpy dependence is optimized across
; basic blocks.

target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.11.0"

%struct.s = type { i32, i32 }

@foo.s = private unnamed_addr constant %struct.s { i32 1, i32 2 }, align 4

; Function Attrs: nounwind ssp uwtable
define void @foo() #0 {
  %s = alloca %struct.s, align 4
  %t = alloca %struct.s, align 4
  %1 = bitcast %struct.s* %s to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %1, i8* bitcast (%struct.s* @foo.s to i8*), i64 8, i32 4, i1 false)
  br label %2

; <label>:2                                       ; preds = %0
  %3 = bitcast %struct.s* %t to i8*
  %4 = bitcast %struct.s* %s to i8*
  call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* %4, i64 8, i32 4, i1 false)
; CHECK: call void @llvm.memcpy.p0i8.p0i8.i64(i8* %3, i8* bitcast (%struct.s* @foo.s to i8*), i64 8, i32 4, i1 false)
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.memcpy.p0i8.p0i8.i64(i8* nocapture, i8* nocapture readonly, i64, i32, i1) #1

