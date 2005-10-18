//===-- llvm/Instructions.h - Instruction subclass definitions --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file was developed by the LLVM research group and is distributed under
// the University of Illinois Open Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file exposes the class definitions of all of the subclasses of the
// Instruction class.  This is meant to be an easy way to get access to all
// instruction subclasses.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_INSTRUCTIONS_H
#define LLVM_INSTRUCTIONS_H

#include "llvm/Instruction.h"
#include "llvm/InstrTypes.h"

namespace llvm {

class BasicBlock;
class ConstantInt;
class PointerType;
class VectorType;


//===----------------------------------------------------------------------===//
//                             AllocationInst Class
//===----------------------------------------------------------------------===//

/// AllocationInst - This class is the common base class of MallocInst and
/// AllocaInst.
///
class AllocationInst : public UnaryInstruction {
protected:
  AllocationInst(const Type *Ty, Value *ArraySize, unsigned iTy,
                 const std::string &Name = "", Instruction *InsertBefore = 0);
  AllocationInst(const Type *Ty, Value *ArraySize, unsigned iTy,
                 const std::string &Name, BasicBlock *InsertAtEnd);

public:

  /// isArrayAllocation - Return true if there is an allocation size parameter
  /// to the allocation instruction that is not 1.
  ///
  bool isArrayAllocation() const;

  /// getArraySize - Get the number of element allocated, for a simple
  /// allocation of a single element, this will return a constant 1 value.
  ///
  inline const Value *getArraySize() const { return getOperand(0); }
  inline Value *getArraySize() { return getOperand(0); }

  /// getType - Overload to return most specific pointer type
  ///
  inline const PointerType *getType() const {
    return reinterpret_cast<const PointerType*>(Instruction::getType());
  }

  /// getAllocatedType - Return the type that is being allocated by the
  /// instruction.
  ///
  const Type *getAllocatedType() const;

  virtual Instruction *clone() const = 0;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const AllocationInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Alloca ||
           I->getOpcode() == Instruction::Malloc;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                MallocInst Class
//===----------------------------------------------------------------------===//

/// MallocInst - an instruction to allocated memory on the heap
///
class MallocInst : public AllocationInst {
  MallocInst(const MallocInst &MI);
public:
  explicit MallocInst(const Type *Ty, Value *ArraySize = 0,
                      const std::string &Name = "",
                      Instruction *InsertBefore = 0)
    : AllocationInst(Ty, ArraySize, Malloc, Name, InsertBefore) {}
  MallocInst(const Type *Ty, Value *ArraySize, const std::string &Name,
             BasicBlock *InsertAtEnd)
    : AllocationInst(Ty, ArraySize, Malloc, Name, InsertAtEnd) {}

  virtual MallocInst *clone() const;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const MallocInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Malloc);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                AllocaInst Class
//===----------------------------------------------------------------------===//

/// AllocaInst - an instruction to allocate memory on the stack
///
class AllocaInst : public AllocationInst {
  AllocaInst(const AllocaInst &);
public:
  explicit AllocaInst(const Type *Ty, Value *ArraySize = 0,
                      const std::string &Name = "",
                      Instruction *InsertBefore = 0)
    : AllocationInst(Ty, ArraySize, Alloca, Name, InsertBefore) {}
  AllocaInst(const Type *Ty, Value *ArraySize, const std::string &Name,
             BasicBlock *InsertAtEnd)
    : AllocationInst(Ty, ArraySize, Alloca, Name, InsertAtEnd) {}

  virtual AllocaInst *clone() const;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const AllocaInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Alloca);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                 FreeInst Class
//===----------------------------------------------------------------------===//

/// FreeInst - an instruction to deallocate memory
///
class FreeInst : public UnaryInstruction {
  void AssertOK();
public:
  explicit FreeInst(Value *Ptr, Instruction *InsertBefore = 0);
  FreeInst(Value *Ptr, BasicBlock *InsertAfter);

  virtual FreeInst *clone() const;

  virtual bool mayWriteToMemory() const { return true; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const FreeInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Free);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                LoadInst Class
//===----------------------------------------------------------------------===//

/// LoadInst - an instruction for reading from memory.  This uses the
/// SubclassData field in Value to store whether or not the load is volatile.
///
class LoadInst : public UnaryInstruction {
  LoadInst(const LoadInst &LI)
    : UnaryInstruction(LI.getType(), Load, LI.getOperand(0)) {
    setVolatile(LI.isVolatile());

#ifndef NDEBUG
    AssertOK();
#endif
  }
  void AssertOK();
public:
  LoadInst(Value *Ptr, const std::string &Name, Instruction *InsertBefore);
  LoadInst(Value *Ptr, const std::string &Name, BasicBlock *InsertAtEnd);
  LoadInst(Value *Ptr, const std::string &Name = "", bool isVolatile = false,
           Instruction *InsertBefore = 0);
  LoadInst(Value *Ptr, const std::string &Name, bool isVolatile,
           BasicBlock *InsertAtEnd);

  /// isVolatile - Return true if this is a load from a volatile memory
  /// location.
  ///
  bool isVolatile() const { return SubclassData; }

  /// setVolatile - Specify whether this is a volatile load or not.
  ///
  void setVolatile(bool V) { SubclassData = V; }

  virtual LoadInst *clone() const;

  virtual bool mayWriteToMemory() const { return isVolatile(); }

  Value *getPointerOperand() { return getOperand(0); }
  const Value *getPointerOperand() const { return getOperand(0); }
  static unsigned getPointerOperandIndex() { return 0U; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const LoadInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Load;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                StoreInst Class
//===----------------------------------------------------------------------===//

/// StoreInst - an instruction for storing to memory
///
class StoreInst : public Instruction {
  Use Ops[2];
  StoreInst(const StoreInst &SI) : Instruction(SI.getType(), Store, Ops, 2) {
    Ops[0].init(SI.Ops[0], this);
    Ops[1].init(SI.Ops[1], this);
    setVolatile(SI.isVolatile());
#ifndef NDEBUG
    AssertOK();
#endif
  }
  void AssertOK();
public:
  StoreInst(Value *Val, Value *Ptr, Instruction *InsertBefore);
  StoreInst(Value *Val, Value *Ptr, BasicBlock *InsertAtEnd);
  StoreInst(Value *Val, Value *Ptr, bool isVolatile = false,
            Instruction *InsertBefore = 0);
  StoreInst(Value *Val, Value *Ptr, bool isVolatile, BasicBlock *InsertAtEnd);


  /// isVolatile - Return true if this is a load from a volatile memory
  /// location.
  ///
  bool isVolatile() const { return SubclassData; }

  /// setVolatile - Specify whether this is a volatile load or not.
  ///
  void setVolatile(bool V) { SubclassData = V; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 2 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 2 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 2; }


  virtual StoreInst *clone() const;

  virtual bool mayWriteToMemory() const { return true; }

  Value *getPointerOperand() { return getOperand(1); }
  const Value *getPointerOperand() const { return getOperand(1); }
  static unsigned getPointerOperandIndex() { return 1U; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const StoreInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Store;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                             GetElementPtrInst Class
//===----------------------------------------------------------------------===//

/// GetElementPtrInst - an instruction for type-safe pointer arithmetic to
/// access elements of arrays and structs
///
class GetElementPtrInst : public Instruction {
  GetElementPtrInst(const GetElementPtrInst &GEPI)
    : Instruction(reinterpret_cast<const Type*>(GEPI.getType()), GetElementPtr,
                  0, GEPI.getNumOperands()) {
    Use *OL = OperandList = new Use[NumOperands];
    Use *GEPIOL = GEPI.OperandList;
    for (unsigned i = 0, E = NumOperands; i != E; ++i)
      OL[i].init(GEPIOL[i], this);
  }
  void init(Value *Ptr, const std::vector<Value*> &Idx);
  void init(Value *Ptr, Value *Idx0, Value *Idx1);
  void init(Value *Ptr, Value *Idx);
public:
  /// Constructors - Create a getelementptr instruction with a base pointer an
  /// list of indices.  The first ctor can optionally insert before an existing
  /// instruction, the second appends the new instruction to the specified
  /// BasicBlock.
  GetElementPtrInst(Value *Ptr, const std::vector<Value*> &Idx,
                    const std::string &Name = "", Instruction *InsertBefore =0);
  GetElementPtrInst(Value *Ptr, const std::vector<Value*> &Idx,
                    const std::string &Name, BasicBlock *InsertAtEnd);

  /// Constructors - These two constructors are convenience methods because one
  /// and two index getelementptr instructions are so common.
  GetElementPtrInst(Value *Ptr, Value *Idx,
                    const std::string &Name = "", Instruction *InsertBefore =0);
  GetElementPtrInst(Value *Ptr, Value *Idx,
                    const std::string &Name, BasicBlock *InsertAtEnd);
  GetElementPtrInst(Value *Ptr, Value *Idx0, Value *Idx1,
                    const std::string &Name = "", Instruction *InsertBefore =0);
  GetElementPtrInst(Value *Ptr, Value *Idx0, Value *Idx1,
                    const std::string &Name, BasicBlock *InsertAtEnd);
  ~GetElementPtrInst();

  virtual GetElementPtrInst *clone() const;

  // getType - Overload to return most specific pointer type...
  inline const PointerType *getType() const {
    return reinterpret_cast<const PointerType*>(Instruction::getType());
  }

  /// getIndexedType - Returns the type of the element that would be loaded with
  /// a load instruction with the specified parameters.
  ///
  /// A null type is returned if the indices are invalid for the specified
  /// pointer type.
  ///
  static const Type *getIndexedType(const Type *Ptr,
                                    const std::vector<Value*> &Indices,
                                    bool AllowStructLeaf = false);
  static const Type *getIndexedType(const Type *Ptr, Value *Idx0, Value *Idx1,
                                    bool AllowStructLeaf = false);
  static const Type *getIndexedType(const Type *Ptr, Value *Idx);

  inline op_iterator       idx_begin()       { return op_begin()+1; }
  inline const_op_iterator idx_begin() const { return op_begin()+1; }
  inline op_iterator       idx_end()         { return op_end(); }
  inline const_op_iterator idx_end()   const { return op_end(); }

  Value *getPointerOperand() {
    return getOperand(0);
  }
  const Value *getPointerOperand() const {
    return getOperand(0);
  }
  static unsigned getPointerOperandIndex() {
    return 0U;                      // get index for modifying correct operand
  }

  inline unsigned getNumIndices() const {  // Note: always non-negative
    return getNumOperands() - 1;
  }

  inline bool hasIndices() const {
    return getNumOperands() > 1;
  }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const GetElementPtrInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::GetElementPtr);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                            SetCondInst Class
//===----------------------------------------------------------------------===//

/// SetCondInst class - Represent a SetCC or VSetCC operator, where CC
/// is eq, ne, lt, gt, le, or ge.
///
class SetCondInst : public BinaryOperator {
public:
  SetCondInst(BinaryOps Opcode, Value *LHS, Value *RHS,
              const std::string &Name = "", Instruction *InsertBefore = 0);
  SetCondInst(BinaryOps Opcode, Value *LHS, Value *RHS,
              const std::string &Name, BasicBlock *InsertAtEnd);

  /// getInverseCondition - Return the inverse of the current condition opcode.
  /// For example seteq -> setne, setgt -> setle, setlt -> setge, etc...
  ///
  BinaryOps getInverseCondition() const {
    return getInverseCondition(getOpcode());
  }

  /// getInverseCondition - Static version that you can use without an
  /// instruction available.
  ///
  static BinaryOps getInverseCondition(BinaryOps Opcode);

  /// getSwappedCondition - Return the condition opcode that would be the result
  /// of exchanging the two operands of the setcc instruction without changing
  /// the result produced.  Thus, seteq->seteq, setle->setge, setlt->setgt, etc.
  ///
  BinaryOps getSwappedCondition() const {
    return getSwappedCondition(getOpcode());
  }

  /// getSwappedCondition - Static version that you can use without an
  /// instruction available.
  ///
  static BinaryOps getSwappedCondition(BinaryOps Opcode);

  // getScalarOpcode - Return the scalar version of this opcode.
  // For example seteq -> seteq, vseteq -> seteq, etc.
  //
  BinaryOps getScalarOpcode() const {
    return getScalarOpcode(getOpcode());
  }

  /// getScalarOpcode - Static version that you can use without an
  /// instruction available.
  ///
  static BinaryOps getScalarOpcode(BinaryOps Opcode);

  // getVectorOpcode - Return the scalar version of this opcode.
  // For example seteq -> seteq, vseteq -> seteq, etc.
  //
  BinaryOps getVectorOpcode() const {
    return getVectorOpcode(getOpcode());
  }

  /// getVectorOpcode - Static version that you can use without an
  /// instruction available.
  ///
  static BinaryOps getVectorOpcode(BinaryOps Opcode);

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const SetCondInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() >= SetEQ &&
      I->getOpcode() <= VSetGT;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                                 CastInst Class
//===----------------------------------------------------------------------===//

/// CastInst - This class represents a cast from Operand[0] to the type of
/// the instruction (i->getType()).
///
class CastInst : public UnaryInstruction {
  CastInst(const CastInst &CI)
    : UnaryInstruction(CI.getType(), Cast, CI.getOperand(0)) {
  }
public:
  CastInst(Value *S, const Type *Ty, const std::string &Name = "",
           Instruction *InsertBefore = 0)
    : UnaryInstruction(Ty, Cast, S, Name, InsertBefore) {
  }
  CastInst(Value *S, const Type *Ty, const std::string &Name,
           BasicBlock *InsertAtEnd)
    : UnaryInstruction(Ty, Cast, S, Name, InsertAtEnd) {
  }

  virtual CastInst *clone() const;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const CastInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Cast;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                 CallInst Class
//===----------------------------------------------------------------------===//

/// CallInst - This class represents a function call, abstracting a target
/// machine's calling convention.  This class uses low bit of the SubClassData
/// field to indicate whether or not this is a tail call.  The rest of the bits
/// hold the calling convention of the call.
///
class CallInst : public Instruction {
  CallInst(const CallInst &CI);
  void init(Value *Func, const std::vector<Value*> &Params);
  void init(Value *Func, Value *Actual1, Value *Actual2);
  void init(Value *Func, Value *Actual);
  void init(Value *Func);

public:
  CallInst(Value *F, const std::vector<Value*> &Par,
           const std::string &Name = "", Instruction *InsertBefore = 0);
  CallInst(Value *F, const std::vector<Value*> &Par,
           const std::string &Name, BasicBlock *InsertAtEnd);

  // Alternate CallInst ctors w/ two actuals, w/ one actual and no
  // actuals, respectively.
  CallInst(Value *F, Value *Actual1, Value *Actual2,
           const std::string& Name = "", Instruction *InsertBefore = 0);
  CallInst(Value *F, Value *Actual1, Value *Actual2,
           const std::string& Name, BasicBlock *InsertAtEnd);
  CallInst(Value *F, Value *Actual, const std::string& Name = "",
           Instruction *InsertBefore = 0);
  CallInst(Value *F, Value *Actual, const std::string& Name,
           BasicBlock *InsertAtEnd);
  explicit CallInst(Value *F, const std::string &Name = "",
                    Instruction *InsertBefore = 0);
  explicit CallInst(Value *F, const std::string &Name,
                    BasicBlock *InsertAtEnd);
  ~CallInst();

  virtual CallInst *clone() const;
  bool mayWriteToMemory() const { return true; }

  bool isTailCall() const           { return SubclassData & 1; }
  void setTailCall(bool isTailCall = true) {
    SubclassData = (SubclassData & ~1) | unsigned(isTailCall);
  }

  /// getCallingConv/setCallingConv - Get or set the calling convention of this
  /// function call.
  unsigned getCallingConv() const { return SubclassData >> 1; }
  void setCallingConv(unsigned CC) {
    SubclassData = (SubclassData & 1) | (CC << 1);
  }

  /// getCalledFunction - Return the function being called by this instruction
  /// if it is a direct call.  If it is a call through a function pointer,
  /// return null.
  Function *getCalledFunction() const {
    return static_cast<Function*>(dyn_cast<Function>(getOperand(0)));
  }

  // getCalledValue - Get a pointer to a method that is invoked by this inst.
  inline const Value *getCalledValue() const { return getOperand(0); }
  inline       Value *getCalledValue()       { return getOperand(0); }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const CallInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Call;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                 ShiftInst Class
//===----------------------------------------------------------------------===//

/// ShiftInst - This class represents left and right shift instructions.
///
class ShiftInst : public Instruction {
  Use Ops[2];
  ShiftInst(const ShiftInst &SI)
    : Instruction(SI.getType(), SI.getOpcode(), Ops, 2) {
    Ops[0].init(SI.Ops[0], this);
    Ops[1].init(SI.Ops[1], this);
  }
  void init(OtherOps Opcode, Value *S, Value *SA) {
    assert((Opcode == Shl || Opcode == Shr) && "ShiftInst Opcode invalid!");
    Ops[0].init(S, this);
    Ops[1].init(SA, this);
  }

public:
  ShiftInst(OtherOps Opcode, Value *S, Value *SA, const std::string &Name = "",
            Instruction *InsertBefore = 0)
    : Instruction(S->getType(), Opcode, Ops, 2, Name, InsertBefore) {
    init(Opcode, S, SA);
  }
  ShiftInst(OtherOps Opcode, Value *S, Value *SA, const std::string &Name,
            BasicBlock *InsertAtEnd)
    : Instruction(S->getType(), Opcode, Ops, 2, Name, InsertAtEnd) {
    init(Opcode, S, SA);
  }

  OtherOps getOpcode() const {
    return static_cast<OtherOps>(Instruction::getOpcode());
  }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 2 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 2 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 2; }

  virtual ShiftInst *clone() const;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const ShiftInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Shr) |
           (I->getOpcode() == Instruction::Shl);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                               SelectInst Class
//===----------------------------------------------------------------------===//

/// SelectInst - This class represents the LLVM 'select' instruction.
///
class SelectInst : public Instruction {
  Use Ops[3];

  void init(Value *C, Value *S1, Value *S2) {
    Ops[0].init(C, this);
    Ops[1].init(S1, this);
    Ops[2].init(S2, this);
  }

  SelectInst(const SelectInst &SI)
    : Instruction(SI.getType(), SI.getOpcode(), Ops, 3) {
    init(SI.Ops[0], SI.Ops[1], SI.Ops[2]);
  }
public:
  SelectInst(Value *C, Value *S1, Value *S2, const std::string &Name = "",
             Instruction *InsertBefore = 0)
    : Instruction(S1->getType(), Instruction::Select, Ops, 3,
                  Name, InsertBefore) {
    init(C, S1, S2);
  }
  SelectInst(Value *C, Value *S1, Value *S2, const std::string &Name,
             BasicBlock *InsertAtEnd)
    : Instruction(S1->getType(), Instruction::Select, Ops, 3,
                  Name, InsertAtEnd) {
    init(C, S1, S2);
  }

  Value *getCondition() const { return Ops[0]; }
  Value *getTrueValue() const { return Ops[1]; }
  Value *getFalseValue() const { return Ops[2]; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 3 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 3 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 3; }

  OtherOps getOpcode() const {
    return static_cast<OtherOps>(Instruction::getOpcode());
  }

  virtual SelectInst *clone() const;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const SelectInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Select;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                               VSelectInst Class
//===----------------------------------------------------------------------===//

/// VSelectInst - This class represents the vector version of the LLVM
/// 'select' instruction.
///
class VSelectInst : public Instruction {
  Use Ops[3];

  void init(Value *C, Value *S1, Value *S2) {
    Ops[0].init(C, this);
    Ops[1].init(S1, this);
    Ops[2].init(S2, this);
  }

  VSelectInst(const VSelectInst &SI)
    : Instruction(SI.getType(), SI.getOpcode(), Ops, 3) {
    init(SI.Ops[0], SI.Ops[1], SI.Ops[2]);
  }
public:
  VSelectInst(Value *C, Value *S1, Value *S2, const std::string &Name = "",
             Instruction *InsertBefore = 0)
    : Instruction(S1->getType(), Instruction::VSelect, Ops, 3,
                  Name, InsertBefore) {
    init(C, S1, S2);
  }
  VSelectInst(Value *C, Value *S1, Value *S2, const std::string &Name,
             BasicBlock *InsertAtEnd)
    : Instruction(S1->getType(), Instruction::VSelect, Ops, 3,
                  Name, InsertAtEnd) {
    init(C, S1, S2);
  }

  Value *getCondition() const { return Ops[0]; }
  Value *getTrueValue() const { return Ops[1]; }
  Value *getFalseValue() const { return Ops[2]; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 3 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 3 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 3; }

  OtherOps getOpcode() const {
    return static_cast<OtherOps>(Instruction::getOpcode());
  }

  virtual VSelectInst *clone() const;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const VSelectInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::VSelect;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                VAArgInst Class
//===----------------------------------------------------------------------===//

/// VAArgInst - This class represents the va_arg llvm instruction, which returns
/// an argument of the specified type given a va_list and increments that list
///
class VAArgInst : public UnaryInstruction {
  VAArgInst(const VAArgInst &VAA)
    : UnaryInstruction(VAA.getType(), VAArg, VAA.getOperand(0)) {}
public:
  VAArgInst(Value *List, const Type *Ty, const std::string &Name = "",
             Instruction *InsertBefore = 0)
    : UnaryInstruction(Ty, VAArg, List, Name, InsertBefore) {
  }
  VAArgInst(Value *List, const Type *Ty, const std::string &Name,
            BasicBlock *InsertAtEnd)
    : UnaryInstruction(Ty, VAArg, List, Name, InsertAtEnd) {
  }

  virtual VAArgInst *clone() const;
  bool mayWriteToMemory() const { return true; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const VAArgInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == VAArg;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                               PHINode Class
//===----------------------------------------------------------------------===//

// PHINode - The PHINode class is used to represent the magical mystical PHI
// node, that can not exist in nature, but can be synthesized in a computer
// scientist's overactive imagination.
//
class PHINode : public Instruction {
  /// ReservedSpace - The number of operands actually allocated.  NumOperands is
  /// the number actually in use.
  unsigned ReservedSpace;
  PHINode(const PHINode &PN);
public:
  PHINode(const Type *Ty, const std::string &Name = "",
          Instruction *InsertBefore = 0)
    : Instruction(Ty, Instruction::PHI, 0, 0, Name, InsertBefore),
      ReservedSpace(0) {
  }

  PHINode(const Type *Ty, const std::string &Name, BasicBlock *InsertAtEnd)
    : Instruction(Ty, Instruction::PHI, 0, 0, Name, InsertAtEnd),
      ReservedSpace(0) {
  }

  ~PHINode();

  /// reserveOperandSpace - This method can be used to avoid repeated
  /// reallocation of PHI operand lists by reserving space for the correct
  /// number of operands before adding them.  Unlike normal vector reserves,
  /// this method can also be used to trim the operand space.
  void reserveOperandSpace(unsigned NumValues) {
    resizeOperands(NumValues*2);
  }

  virtual PHINode *clone() const;

  /// getNumIncomingValues - Return the number of incoming edges
  ///
  unsigned getNumIncomingValues() const { return getNumOperands()/2; }

  /// getIncomingValue - Return incoming value #x
  ///
  Value *getIncomingValue(unsigned i) const {
    assert(i*2 < getNumOperands() && "Invalid value number!");
    return getOperand(i*2);
  }
  void setIncomingValue(unsigned i, Value *V) {
    assert(i*2 < getNumOperands() && "Invalid value number!");
    setOperand(i*2, V);
  }
  unsigned getOperandNumForIncomingValue(unsigned i) {
    return i*2;
  }

  /// getIncomingBlock - Return incoming basic block #x
  ///
  BasicBlock *getIncomingBlock(unsigned i) const {
    return reinterpret_cast<BasicBlock*>(getOperand(i*2+1));
  }
  void setIncomingBlock(unsigned i, BasicBlock *BB) {
    setOperand(i*2+1, reinterpret_cast<Value*>(BB));
  }
  unsigned getOperandNumForIncomingBlock(unsigned i) {
    return i*2+1;
  }

  /// addIncoming - Add an incoming value to the end of the PHI list
  ///
  void addIncoming(Value *V, BasicBlock *BB) {
    assert(getType() == V->getType() &&
           "All operands to PHI node must be the same type as the PHI node!");
    unsigned OpNo = NumOperands;
    if (OpNo+2 > ReservedSpace)
      resizeOperands(0);  // Get more space!
    // Initialize some new operands.
    NumOperands = OpNo+2;
    OperandList[OpNo].init(V, this);
    OperandList[OpNo+1].init(reinterpret_cast<Value*>(BB), this);
  }

  /// removeIncomingValue - Remove an incoming value.  This is useful if a
  /// predecessor basic block is deleted.  The value removed is returned.
  ///
  /// If the last incoming value for a PHI node is removed (and DeletePHIIfEmpty
  /// is true), the PHI node is destroyed and any uses of it are replaced with
  /// dummy values.  The only time there should be zero incoming values to a PHI
  /// node is when the block is dead, so this strategy is sound.
  ///
  Value *removeIncomingValue(unsigned Idx, bool DeletePHIIfEmpty = true);

  Value *removeIncomingValue(const BasicBlock *BB, bool DeletePHIIfEmpty =true){
    int Idx = getBasicBlockIndex(BB);
    assert(Idx >= 0 && "Invalid basic block argument to remove!");
    return removeIncomingValue(Idx, DeletePHIIfEmpty);
  }

  /// getBasicBlockIndex - Return the first index of the specified basic
  /// block in the value list for this PHI.  Returns -1 if no instance.
  ///
  int getBasicBlockIndex(const BasicBlock *BB) const {
    Use *OL = OperandList;
    for (unsigned i = 0, e = getNumOperands(); i != e; i += 2)
      if (OL[i+1] == reinterpret_cast<const Value*>(BB)) return i/2;
    return -1;
  }

  Value *getIncomingValueForBlock(const BasicBlock *BB) const {
    return getIncomingValue(getBasicBlockIndex(BB));
  }

  /// hasConstantValue - If the specified PHI node always merges together the 
  /// same value, return the value, otherwise return null.
  ///
  Value *hasConstantValue(bool AllowNonDominatingInstruction = false) const;
  
  /// Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const PHINode *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::PHI;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
 private:
  void resizeOperands(unsigned NumOperands);
};

//===----------------------------------------------------------------------===//
//                               ReturnInst Class
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------------
/// ReturnInst - Return a value (possibly void), from a function.  Execution
/// does not continue in this function any longer.
///
class ReturnInst : public TerminatorInst {
  Use RetVal;  // Possibly null retval.
  ReturnInst(const ReturnInst &RI) : TerminatorInst(Instruction::Ret, &RetVal,
                                                    RI.getNumOperands()) {
    if (RI.getNumOperands())
      RetVal.init(RI.RetVal, this);
  }

  void init(Value *RetVal);

public:
  // ReturnInst constructors:
  // ReturnInst()                  - 'ret void' instruction
  // ReturnInst(    null)          - 'ret void' instruction
  // ReturnInst(Value* X)          - 'ret X'    instruction
  // ReturnInst(    null, Inst *)  - 'ret void' instruction, insert before I
  // ReturnInst(Value* X, Inst *I) - 'ret X'    instruction, insert before I
  // ReturnInst(    null, BB *B)   - 'ret void' instruction, insert @ end of BB
  // ReturnInst(Value* X, BB *B)   - 'ret X'    instruction, insert @ end of BB
  //
  // NOTE: If the Value* passed is of type void then the constructor behaves as
  // if it was passed NULL.
  ReturnInst(Value *retVal = 0, Instruction *InsertBefore = 0)
    : TerminatorInst(Instruction::Ret, &RetVal, 0, InsertBefore) {
    init(retVal);
  }
  ReturnInst(Value *retVal, BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Ret, &RetVal, 0, InsertAtEnd) {
    init(retVal);
  }
  ReturnInst(BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Ret, &RetVal, 0, InsertAtEnd) {
  }

  virtual ReturnInst *clone() const;

  // Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < getNumOperands() && "getOperand() out of range!");
    return RetVal;
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < getNumOperands() && "setOperand() out of range!");
    RetVal = Val;
  }

  Value *getReturnValue() const { return RetVal; }

  unsigned getNumSuccessors() const { return 0; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const ReturnInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Ret);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
 private:
  virtual BasicBlock *getSuccessorV(unsigned idx) const;
  virtual unsigned getNumSuccessorsV() const;
  virtual void setSuccessorV(unsigned idx, BasicBlock *B);
};

//===----------------------------------------------------------------------===//
//                               BranchInst Class
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------------
/// BranchInst - Conditional or Unconditional Branch instruction.
///
class BranchInst : public TerminatorInst {
  /// Ops list - Branches are strange.  The operands are ordered:
  ///  TrueDest, FalseDest, Cond.  This makes some accessors faster because
  /// they don't have to check for cond/uncond branchness.
  Use Ops[3];
  BranchInst(const BranchInst &BI);
  void AssertOK();
public:
  // BranchInst constructors (where {B, T, F} are blocks, and C is a condition):
  // BranchInst(BB *B)                           - 'br B'
  // BranchInst(BB* T, BB *F, Value *C)          - 'br C, T, F'
  // BranchInst(BB* B, Inst *I)                  - 'br B'        insert before I
  // BranchInst(BB* T, BB *F, Value *C, Inst *I) - 'br C, T, F', insert before I
  // BranchInst(BB* B, BB *I)                    - 'br B'        insert at end
  // BranchInst(BB* T, BB *F, Value *C, BB *I)   - 'br C, T, F', insert at end
  BranchInst(BasicBlock *IfTrue, Instruction *InsertBefore = 0)
    : TerminatorInst(Instruction::Br, Ops, 1, InsertBefore) {
    assert(IfTrue != 0 && "Branch destination may not be null!");
    Ops[0].init(reinterpret_cast<Value*>(IfTrue), this);
  }
  BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond,
             Instruction *InsertBefore = 0)
    : TerminatorInst(Instruction::Br, Ops, 3, InsertBefore) {
    Ops[0].init(reinterpret_cast<Value*>(IfTrue), this);
    Ops[1].init(reinterpret_cast<Value*>(IfFalse), this);
    Ops[2].init(Cond, this);
#ifndef NDEBUG
    AssertOK();
#endif
  }

  BranchInst(BasicBlock *IfTrue, BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Br, Ops, 1, InsertAtEnd) {
    assert(IfTrue != 0 && "Branch destination may not be null!");
    Ops[0].init(reinterpret_cast<Value*>(IfTrue), this);
  }

  BranchInst(BasicBlock *IfTrue, BasicBlock *IfFalse, Value *Cond,
             BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Br, Ops, 3, InsertAtEnd) {
    Ops[0].init(reinterpret_cast<Value*>(IfTrue), this);
    Ops[1].init(reinterpret_cast<Value*>(IfFalse), this);
    Ops[2].init(Cond, this);
#ifndef NDEBUG
    AssertOK();
#endif
  }


  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < getNumOperands() && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < getNumOperands() && "setOperand() out of range!");
    Ops[i] = Val;
  }

  virtual BranchInst *clone() const;

  inline bool isUnconditional() const { return getNumOperands() == 1; }
  inline bool isConditional()   const { return getNumOperands() == 3; }

  inline Value *getCondition() const {
    assert(isConditional() && "Cannot get condition of an uncond branch!");
    return getOperand(2);
  }

  void setCondition(Value *V) {
    assert(isConditional() && "Cannot set condition of unconditional branch!");
    setOperand(2, V);
  }

  // setUnconditionalDest - Change the current branch to an unconditional branch
  // targeting the specified block.
  // FIXME: Eliminate this ugly method.
  void setUnconditionalDest(BasicBlock *Dest) {
    if (isConditional()) {  // Convert this to an uncond branch.
      NumOperands = 1;
      Ops[1].set(0);
      Ops[2].set(0);
    }
    setOperand(0, reinterpret_cast<Value*>(Dest));
  }

  unsigned getNumSuccessors() const { return 1+isConditional(); }

  BasicBlock *getSuccessor(unsigned i) const {
    assert(i < getNumSuccessors() && "Successor # out of range for Branch!");
    return (i == 0) ? cast<BasicBlock>(getOperand(0)) :
                      cast<BasicBlock>(getOperand(1));
  }

  void setSuccessor(unsigned idx, BasicBlock *NewSucc) {
    assert(idx < getNumSuccessors() && "Successor # out of range for Branch!");
    setOperand(idx, reinterpret_cast<Value*>(NewSucc));
  }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const BranchInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Br);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
private:
  virtual BasicBlock *getSuccessorV(unsigned idx) const;
  virtual unsigned getNumSuccessorsV() const;
  virtual void setSuccessorV(unsigned idx, BasicBlock *B);
};

//===----------------------------------------------------------------------===//
//                               SwitchInst Class
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------------
/// SwitchInst - Multiway switch
///
class SwitchInst : public TerminatorInst {
  unsigned ReservedSpace;
  // Operand[0]    = Value to switch on
  // Operand[1]    = Default basic block destination
  // Operand[2n  ] = Value to match
  // Operand[2n+1] = BasicBlock to go to on match
  SwitchInst(const SwitchInst &RI);
  void init(Value *Value, BasicBlock *Default, unsigned NumCases);
  void resizeOperands(unsigned No);
public:
  /// SwitchInst ctor - Create a new switch instruction, specifying a value to
  /// switch on and a default destination.  The number of additional cases can
  /// be specified here to make memory allocation more efficient.  This
  /// constructor can also autoinsert before another instruction.
  SwitchInst(Value *Value, BasicBlock *Default, unsigned NumCases,
             Instruction *InsertBefore = 0)
    : TerminatorInst(Instruction::Switch, 0, 0, InsertBefore) {
    init(Value, Default, NumCases);
  }

  /// SwitchInst ctor - Create a new switch instruction, specifying a value to
  /// switch on and a default destination.  The number of additional cases can
  /// be specified here to make memory allocation more efficient.  This
  /// constructor also autoinserts at the end of the specified BasicBlock.
  SwitchInst(Value *Value, BasicBlock *Default, unsigned NumCases,
             BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Switch, 0, 0, InsertAtEnd) {
    init(Value, Default, NumCases);
  }
  ~SwitchInst();


  // Accessor Methods for Switch stmt
  inline Value *getCondition() const { return getOperand(0); }
  void setCondition(Value *V) { setOperand(0, V); }

  inline BasicBlock *getDefaultDest() const {
    return cast<BasicBlock>(getOperand(1));
  }

  /// getNumCases - return the number of 'cases' in this switch instruction.
  /// Note that case #0 is always the default case.
  unsigned getNumCases() const {
    return getNumOperands()/2;
  }

  /// getCaseValue - Return the specified case value.  Note that case #0, the
  /// default destination, does not have a case value.
  ConstantInt *getCaseValue(unsigned i) {
    assert(i && i < getNumCases() && "Illegal case value to get!");
    return getSuccessorValue(i);
  }

  /// getCaseValue - Return the specified case value.  Note that case #0, the
  /// default destination, does not have a case value.
  const ConstantInt *getCaseValue(unsigned i) const {
    assert(i && i < getNumCases() && "Illegal case value to get!");
    return getSuccessorValue(i);
  }

  /// findCaseValue - Search all of the case values for the specified constant.
  /// If it is explicitly handled, return the case number of it, otherwise
  /// return 0 to indicate that it is handled by the default handler.
  unsigned findCaseValue(const ConstantInt *C) const {
    for (unsigned i = 1, e = getNumCases(); i != e; ++i)
      if (getCaseValue(i) == C)
        return i;
    return 0;
  }

  /// addCase - Add an entry to the switch instruction...
  ///
  void addCase(ConstantInt *OnVal, BasicBlock *Dest);

  /// removeCase - This method removes the specified successor from the switch
  /// instruction.  Note that this cannot be used to remove the default
  /// destination (successor #0).
  ///
  void removeCase(unsigned idx);

  virtual SwitchInst *clone() const;

  unsigned getNumSuccessors() const { return getNumOperands()/2; }
  BasicBlock *getSuccessor(unsigned idx) const {
    assert(idx < getNumSuccessors() &&"Successor idx out of range for switch!");
    return cast<BasicBlock>(getOperand(idx*2+1));
  }
  void setSuccessor(unsigned idx, BasicBlock *NewSucc) {
    assert(idx < getNumSuccessors() && "Successor # out of range for switch!");
    setOperand(idx*2+1, reinterpret_cast<Value*>(NewSucc));
  }

  // getSuccessorValue - Return the value associated with the specified
  // successor.
  inline ConstantInt *getSuccessorValue(unsigned idx) const {
    assert(idx < getNumSuccessors() && "Successor # out of range!");
    return reinterpret_cast<ConstantInt*>(getOperand(idx*2));
  }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const SwitchInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Switch;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
private:
  virtual BasicBlock *getSuccessorV(unsigned idx) const;
  virtual unsigned getNumSuccessorsV() const;
  virtual void setSuccessorV(unsigned idx, BasicBlock *B);
};

//===----------------------------------------------------------------------===//
//                               InvokeInst Class
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------------

/// InvokeInst - Invoke instruction.  The SubclassData field is used to hold the
/// calling convention of the call.
///
class InvokeInst : public TerminatorInst {
  InvokeInst(const InvokeInst &BI);
  void init(Value *Fn, BasicBlock *IfNormal, BasicBlock *IfException,
            const std::vector<Value*> &Params);
public:
  InvokeInst(Value *Fn, BasicBlock *IfNormal, BasicBlock *IfException,
             const std::vector<Value*> &Params, const std::string &Name = "",
             Instruction *InsertBefore = 0);
  InvokeInst(Value *Fn, BasicBlock *IfNormal, BasicBlock *IfException,
             const std::vector<Value*> &Params, const std::string &Name,
             BasicBlock *InsertAtEnd);
  ~InvokeInst();

  virtual InvokeInst *clone() const;

  bool mayWriteToMemory() const { return true; }

  /// getCallingConv/setCallingConv - Get or set the calling convention of this
  /// function call.
  unsigned getCallingConv() const { return SubclassData; }
  void setCallingConv(unsigned CC) {
    SubclassData = CC;
  }

  /// getCalledFunction - Return the function called, or null if this is an
  /// indirect function invocation.
  ///
  Function *getCalledFunction() const {
    return dyn_cast<Function>(getOperand(0));
  }

  // getCalledValue - Get a pointer to a function that is invoked by this inst.
  inline Value *getCalledValue() const { return getOperand(0); }

  // get*Dest - Return the destination basic blocks...
  BasicBlock *getNormalDest() const {
    return cast<BasicBlock>(getOperand(1));
  }
  BasicBlock *getUnwindDest() const {
    return cast<BasicBlock>(getOperand(2));
  }
  void setNormalDest(BasicBlock *B) {
    setOperand(1, reinterpret_cast<Value*>(B));
  }

  void setUnwindDest(BasicBlock *B) {
    setOperand(2, reinterpret_cast<Value*>(B));
  }

  inline BasicBlock *getSuccessor(unsigned i) const {
    assert(i < 2 && "Successor # out of range for invoke!");
    return i == 0 ? getNormalDest() : getUnwindDest();
  }

  void setSuccessor(unsigned idx, BasicBlock *NewSucc) {
    assert(idx < 2 && "Successor # out of range for invoke!");
    setOperand(idx+1, reinterpret_cast<Value*>(NewSucc));
  }

  unsigned getNumSuccessors() const { return 2; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const InvokeInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return (I->getOpcode() == Instruction::Invoke);
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
private:
  virtual BasicBlock *getSuccessorV(unsigned idx) const;
  virtual unsigned getNumSuccessorsV() const;
  virtual void setSuccessorV(unsigned idx, BasicBlock *B);
};


//===----------------------------------------------------------------------===//
//                              UnwindInst Class
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------------
/// UnwindInst - Immediately exit the current function, unwinding the stack
/// until an invoke instruction is found.
///
class UnwindInst : public TerminatorInst {
public:
  UnwindInst(Instruction *InsertBefore = 0)
    : TerminatorInst(Instruction::Unwind, 0, 0, InsertBefore) {
  }
  UnwindInst(BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Unwind, 0, 0, InsertAtEnd) {
  }

  virtual UnwindInst *clone() const;

  unsigned getNumSuccessors() const { return 0; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const UnwindInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Unwind;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
private:
  virtual BasicBlock *getSuccessorV(unsigned idx) const;
  virtual unsigned getNumSuccessorsV() const;
  virtual void setSuccessorV(unsigned idx, BasicBlock *B);
};

//===----------------------------------------------------------------------===//
//                           UnreachableInst Class
//===----------------------------------------------------------------------===//

//===---------------------------------------------------------------------------
/// UnreachableInst - This function has undefined behavior.  In particular, the
/// presence of this instruction indicates some higher level knowledge that the
/// end of the block cannot be reached.
///
class UnreachableInst : public TerminatorInst {
public:
  UnreachableInst(Instruction *InsertBefore = 0)
    : TerminatorInst(Instruction::Unreachable, 0, 0, InsertBefore) {
  }
  UnreachableInst(BasicBlock *InsertAtEnd)
    : TerminatorInst(Instruction::Unreachable, 0, 0, InsertAtEnd) {
  }

  virtual UnreachableInst *clone() const;

  unsigned getNumSuccessors() const { return 0; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const UnreachableInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Unreachable;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
private:
  virtual BasicBlock *getSuccessorV(unsigned idx) const;
  virtual unsigned getNumSuccessorsV() const;
  virtual void setSuccessorV(unsigned idx, BasicBlock *B);
};

//===----------------------------------------------------------------------===//
//                             VMemoryInst Class
//===----------------------------------------------------------------------===//

/// VMemoryInst - This class is the common base class of VGatherInst and
/// VScatterInst.
///
class VMemoryInst : public Instruction {
protected:
  VMemoryInst(const Type *Ty, unsigned iType,
	      Use *Ops, unsigned NumOps,
	      const std::string &Name = "", Instruction *InsertBef = 0);
  VMemoryInst(const Type* Ty, unsigned iType,
	      Use *Ops, unsigned NumOps,
	      const std::string &Name, BasicBlock *InsertAE);
  ~VMemoryInst();

public:

  // Check for correct number of indices
  //
  static bool VMemoryInst::checkNumIndices(const std::vector<Value*> &Idx) {
    return (Idx.size() >= 4 && Idx.size() % 4 == 0);
  }

  // Check for correct types of indices
  //
  static bool VMemoryInst::checkIndexType(const std::vector<Value*> &);

  virtual Instruction *clone() const = 0;

  const Type *getElementType() const;
  virtual Value *getPointerOperand() = 0;
  virtual const Value *getPointerOperand() const = 0;

  virtual unsigned getNumIndices() const = 0;
  virtual Value *getIndex(unsigned) = 0;
  virtual const Value *getIndex(unsigned) const = 0;

  Value *getLowerBound(unsigned level) { return getIndex(4*level); }
  const Value *getLowerBound(unsigned level) const { return getIndex(4*level); }

  Value *getUpperBound(unsigned level) { return getIndex(4*level+1); }
  const Value *getUpperBound(unsigned level) const { return getIndex(4*level+1); }

  Value *getStride(unsigned level) { return getIndex(4*level+2); }
  const Value *getStride(unsigned level) const { return getIndex(4*level+2); }
  
  Value *getMultiplier(unsigned level) {return getIndex(4*level+3); }
  const Value *getMultiplier(unsigned level) const {return getIndex(4*level+3); }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const VMemoryInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::VGather ||
           I->getOpcode() == Instruction::VScatter;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                VGatherInst Class
//===----------------------------------------------------------------------===//

/// VGatherInst - an instruction for reading a vector into a register
/// from memory
///
class VGatherInst : public VMemoryInst {
  VGatherInst(const VGatherInst &LI) : VMemoryInst(LI.getType(), VGather, 0, 0) {
    NumOperands = LI.getNumOperands();
    Use *OL = OperandList = new Use[NumOperands];
    for (unsigned i = 0; i < NumOperands; ++i)
      OL[i].init(LI.getOperand(i), this);
  }
  void init(Value *Ptr, const std::vector<Value*> &Idx);

public:
  VGatherInst(Value *Ptr, const std::vector<Value*> &Idx,
	    const std::string &Name = "", Instruction *InsertBefore = 0);
  VGatherInst(Value *Ptr, const std::vector<Value*> &Idx,
	    const std::string &Name, BasicBlock *InsertAtEnd);

  virtual VGatherInst *clone() const;

  virtual bool mayWriteToMemory() const { return false; }

  Value *getPointerOperand() { return getOperand(0); }
  const Value *getPointerOperand() const { return getOperand(0); }
  static unsigned getPointerOperandIndex() { return 0U; }

  unsigned getNumIndices() const {  // Note: always non-negative
    return getNumOperands() - 1;
  }

  Value *getIndex(unsigned i) { return getOperand(i + 1); }
  const Value *getIndex(unsigned i) const { return getOperand(i + 1); }
  
  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const VGatherInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::VGather;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};



//===----------------------------------------------------------------------===//
//                                VScatterInst Class
//===----------------------------------------------------------------------===//

/// VScatterInst - an instruction for storing a vector register to
/// memory
///
class VScatterInst : public VMemoryInst {
  VScatterInst(const VScatterInst &SI) : VMemoryInst(SI.getType(), VScatter, 0, 0) {
    NumOperands = SI.getNumOperands();
    Use *OL = OperandList = new Use[NumOperands];
    for (unsigned i = 0; i < NumOperands; ++i)
      OL[i].init(SI.getOperand(i), this);
  }
  void init(Value *Val, Value *Ptr, const std::vector<Value*> &Idx);

public:
  VScatterInst(Value *Val, Value *Ptr, const std::vector<Value*> &Idx,
	       Instruction *InsertBefore = 0);
  VScatterInst(Value *Val, Value *Ptr, const std::vector<Value*> &Idx,
	       BasicBlock *InsertAtEnd);

  virtual VScatterInst *clone() const;

  virtual bool mayWriteToMemory() const { return true; }

  Value *getPointerOperand() { return getOperand(1); }
  const Value *getPointerOperand() const { return getOperand(1); }
  static unsigned getPointerOperandIndex() { return 1U; }

  unsigned getNumIndices() const {  // Note: always non-negative
    return getNumOperands() - 2;
  }

  Value *getIndex(unsigned i) { return getOperand(i + 2); }
  const Value *getIndex(unsigned i) const { return getOperand(i + 2); }
  
  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const VScatterInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::VScatter;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};


//===----------------------------------------------------------------------===//
//                                VImmInst Class
//===----------------------------------------------------------------------===//

/// VImmInst - an instruction for creating a vector from a replicated
/// scalar value
///
class VImmInst : public Instruction {
  Use Ops[2];
  VImmInst(const VImmInst &VI) : Instruction(VI.getType(), VImm, Ops, 2) {
    Ops[0].init(VI.Ops[0], this);
    Ops[1].init(VI.Ops[1], this);
  }

public:
  VImmInst(Value *Ptr, Value *Len, bool isFixed,
	   const std::string &Name = "", Instruction *InsertBefore = 0);
  VImmInst(Value *Ptr, Value *Len, bool isFixed,
	   const std::string &Name, BasicBlock *InsertAtEnd);

  virtual VImmInst *clone() const;

  virtual bool mayWriteToMemory() const { return false; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 2 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 2 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 2; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const VImmInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::VImm;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                                ExtractInst Class
//===----------------------------------------------------------------------===//

/// ExtractInst - an instruction for extracting a subvector from a
/// vector
///
class ExtractInst : public Instruction {
  Use Ops[4];
  ExtractInst(const ExtractInst &EI) : Instruction(EI.getType(), Extract, Ops, 4) {
    Ops[0].init(EI.Ops[0], this);
    Ops[1].init(EI.Ops[1], this);
    Ops[2].init(EI.Ops[2], this);
    Ops[3].init(EI.Ops[3], this);
  }

public:
  ExtractInst(Value *Val, Value *Start, Value *Stride, Value *Len,
	       const std::string &Name = "", Instruction *InsertBefore = 0);
  ExtractInst(Value *Val, Value *Start, Value *Stride, Value *Len,
	       const std::string &Name, BasicBlock *InsertAtEnd);

  virtual ExtractInst *clone() const;

  virtual bool mayWriteToMemory() const { return false; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 4 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 4 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 4; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const ExtractInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Extract;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                                ExtractElementInst Class
//===----------------------------------------------------------------------===//

/// ExtractElementInst - an instruction for extracting a single
/// element from a vector
///
class ExtractElementInst : public Instruction {
  Use Ops[2];
  ExtractElementInst(const ExtractElementInst &EI) : 
    Instruction(EI.getType(), ExtractElement, Ops, 2) {
    Ops[0].init(EI.Ops[0], this);
    Ops[1].init(EI.Ops[1], this);
  }

public:
  ExtractElementInst(Value *Val, Value *Index,
	       const std::string &Name = "", Instruction *InsertBefore = 0);
  ExtractElementInst(Value *Val, Value *Index,
	       const std::string &Name, BasicBlock *InsertAtEnd);

  virtual ExtractElementInst *clone() const;

  virtual bool mayWriteToMemory() const { return false; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 2 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 2 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 2; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const ExtractElementInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::ExtractElement;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                                CombineInst Class
//===----------------------------------------------------------------------===//

/// CombineInst - an instruction for combining two vectors by
/// overlaying the second on the first
///
class CombineInst : public Instruction {
  Use Ops[4];
  CombineInst(const CombineInst &EI) : Instruction(EI.getType(), Combine, Ops, 4) {
    Ops[0].init(EI.Ops[0], this);
    Ops[1].init(EI.Ops[1], this);
    Ops[2].init(EI.Ops[2], this);
    Ops[3].init(EI.Ops[3], this);
  }

public:
  CombineInst(Value *V1, Value *V2, Value *Start, Value *Stride,
	      const std::string &Name = "", Instruction *InsertBefore = 0);
  CombineInst(Value *V1, Value *V2, Value *Start, Value *Stride,
	      const std::string &Name, BasicBlock *InsertAtEnd);

  virtual CombineInst *clone() const;

  virtual bool mayWriteToMemory() const { return false; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 4 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 4 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 4; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const CombineInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::Combine;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

//===----------------------------------------------------------------------===//
//                                CombineElementInst Class
//===----------------------------------------------------------------------===//

/// CombineElementInst - an instruction for combining two vectors by
/// overlaying the second on the first
///
class CombineElementInst : public Instruction {
  Use Ops[3];
  CombineElementInst(const CombineElementInst &EI) : 
    Instruction(EI.getType(), CombineElement, Ops, 3) {
    Ops[0].init(EI.Ops[0], this);
    Ops[1].init(EI.Ops[1], this);
    Ops[2].init(EI.Ops[2], this);
  }

public:
  CombineElementInst(Value *Vector, Value *Element, Value *Index, 
	      const std::string &Name = "", Instruction *InsertBefore = 0);
  CombineElementInst(Value *Vector, Value *Element, Value *Index,
	      const std::string &Name, BasicBlock *InsertAtEnd);

  virtual CombineElementInst *clone() const;

  virtual bool mayWriteToMemory() const { return false; }

  /// Transparently provide more efficient getOperand methods.
  Value *getOperand(unsigned i) const {
    assert(i < 3 && "getOperand() out of range!");
    return Ops[i];
  }
  void setOperand(unsigned i, Value *Val) {
    assert(i < 3 && "setOperand() out of range!");
    Ops[i] = Val;
  }
  unsigned getNumOperands() const { return 3; }

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const CombineElementInst *) { return true; }
  static inline bool classof(const Instruction *I) {
    return I->getOpcode() == Instruction::CombineElement;
  }
  static inline bool classof(const Value *V) {
    return isa<Instruction>(V) && classof(cast<Instruction>(V));
  }
};

} // End llvm namespace

#endif
