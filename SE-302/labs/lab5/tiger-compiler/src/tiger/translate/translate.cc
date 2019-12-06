#include "tiger/translate/translate.h"

#include "tiger/errormsg/errormsg.h"
#include "tiger/frame/temp.h"
#include "tiger/semant/semant.h"
#include "tiger/semant/types.h"
#include "tiger/util/util.h"
#include <cstdio>
#include <iostream>
#include <queue>
#include <set>
#include <string>

using VEnvType = S::Table< E::EnvEntry >*;
using TEnvType = S::Table< TY::Ty >*;

namespace FRM {
F::Frame* newFrame( TEMP::Label name, U::BoolList* formals, F::Frame* lastFrame ) {
    std::cout << "called newFrame" << std::endl;
    F::AccessList* formalAcList = nullptr;
    int            argCount     = 0;
    while ( formals ) {
        ++argCount;
        auto access = new F::InFrameAccess( /* offset: */ argCount * F::wordSize );
        access->sym = formals->symbol;
        std::cout << "created new InFrameAccess. sym: " << access->sym << std::endl;
        formalAcList = new F::AccessList( access, formalAcList );
        formals      = formals->tail;
    }

    F::Frame* f = new F::Frame( name );
    f->putInfo( F::Frame::Kind::ARGUMENT, argCount, formalAcList );
    f->putInfo( F::Frame::Kind::VARIABLE, 0, nullptr );
    f->lastFrame = lastFrame;
    return f;
}
}  // namespace FRM

namespace TR {

class Access {
public:
    Level* level;

    F::Access* access;

    Access( Level* level, F::Access* access ) : level( level ), access( access ) {}
};

class AccessList {
public:
    Access*     head;
    AccessList* tail;

    AccessList( Access* head, AccessList* tail ) : head( head ), tail( tail ) {}
};

class Level {
public:
    F::Frame* frame;
    Level*    parent;

    Level( F::Frame* frame, Level* parent ) : frame( frame ), parent( parent ) {}
    AccessList* Formals( Level* level ) {
        return nullptr;
    }

    Level* NewLevel( Level* parent, TEMP::Label* name, U::BoolList* formals ) {
        std::cout << "called new level. label: " << name->Name() << std::endl;
        auto frame = FRM::newFrame( *name, formals, this->frame );
        return new Level( frame, this );
    }
};

class PatchList {
public:
    TEMP::Label** head;
    PatchList*    tail;

    PatchList( TEMP::Label** head, PatchList* tail ) : head( head ), tail( tail ) {}
};

void do_patch( PatchList* tList, TEMP::Label* label ) {
    for ( ; tList; tList = tList->tail )
        *( tList->head ) = label;
}

class Cx {
public:
    PatchList* trues;
    PatchList* falses;
    T::Stm*    stm;

    Cx( PatchList* trues, PatchList* falses, T::Stm* stm ) : trues( trues ), falses( falses ), stm( stm ) {}
};

class Exp {
public:
    enum Kind { EX, NX, CX };

    Kind kind;

    Exp( Kind kind ) : kind( kind ) {}

    virtual T::Exp* UnEx() const = 0;
    virtual T::Stm* UnNx() const = 0;
    virtual Cx      UnCx() const = 0;
};

class ExpAndTy {
public:
    TR::Exp* exp;
    TY::Ty*  ty;

    ExpAndTy( TR::Exp* exp, TY::Ty* ty ) : exp( exp ), ty( ty ) {}
};

class ExExp : public Exp {
public:
    T::Exp* exp;

    ExExp( T::Exp* exp ) : Exp( EX ), exp( exp ) {}

    T::Exp* UnEx() const override {
        std::cout << "Ex's UnEx() called" << std::endl;
        return this->exp;
    }
    T::Stm* UnNx() const override {
        std::cout << "Ex's UnNx() called" << std::endl;
        // special condition: if the second part of the eseq is constant, then let's just put the first part as Statement.
        if ( this->exp->kind == T::Exp::Kind::ESEQ && reinterpret_cast< T::EseqExp* >( this->exp )->exp->kind == T::Exp::Kind::CONST ) {
            return reinterpret_cast< T::EseqExp* >( this->exp )->stm;
        }

        return new T::ExpStm( this->exp );
    }
    Cx UnCx() const override {
        std::cout << "Ex's UnCx() called" << std::endl;
        auto stm    = new T::CjumpStm( T::NE_OP, this->exp, new T::ConstExp( 0 ), nullptr, nullptr );
        auto trues  = new PatchList( &stm->true_label, nullptr );
        auto falses = new PatchList( &stm->true_label, nullptr );
        return TR::Cx( trues, falses, stm );
    }
};

class NxExp : public Exp {
public:
    T::Stm* stm;

    NxExp( T::Stm* stm ) : Exp( NX ), stm( stm ) {}

    T::Exp* UnEx() const override {
        std::cout << "Nx's UnEx() called" << std::endl;
        return new T::EseqExp( this->stm, new T::ConstExp( 0 ) );
    }
    T::Stm* UnNx() const override {
        return this->stm;
    }
    Cx UnCx() const override {
        std::cout << "!![unpermitted]!!\n Nx's UnCx() called" << std::endl;
        assert( 0 );
        TR::Cx( nullptr, nullptr, nullptr );
    }
};

class CxExp : public Exp {
public:
    Cx cx;

    CxExp( struct Cx cx ) : Exp( CX ), cx( cx ) {}
    CxExp( PatchList* trues, PatchList* falses, T::Stm* stm ) : Exp( CX ), cx( trues, falses, stm ) {}

    T::Exp* UnEx() const override {
        std::cout << "Cx's UnEx() called" << std::endl;
        auto r = TEMP::Temp::NewTemp();
        auto t = TEMP::NewLabel();
        auto f = TEMP::NewLabel();
        TR::do_patch( this->cx.trues, t );
        TR::do_patch( this->cx.falses, f );
        return new T::EseqExp( new T::MoveStm( new T::TempExp( r ), new T::ConstExp( 1 ) ),
                               new T::EseqExp( this->cx.stm, new T::EseqExp( new T::LabelStm( f ), new T::EseqExp( new T::MoveStm( new T::TempExp( r ), new T::ConstExp( 0 ) ),
                                                                                                                   new T::EseqExp( new T::LabelStm( t ), new T::TempExp( r ) ) ) ) ) );
    }
    T::Stm* UnNx() const override {
        std::cout << "Cx's UnNx() called" << std::endl;
        return this->cx.stm;
    }
    Cx UnCx() const override {
        std::cout << "Cx's UnCx() called" << std::endl;
        return this->cx;
    }
};

static Access* AllocLocal( Level* level, bool escape, std::string sym ) {
    std::cout << "Entered AllocLocal. escape? " << escape << std::endl;
    F::Access* newAcc;
    if ( escape ) {
        level->frame->varCount += 1;
        newAcc      = level->frame->InFrame( -( level->frame->varCount * F::wordSize ) );
        newAcc->sym = sym;
    }
    else {
        // never happens in lab5.
        // everything is escape
    }
    std::cout << "create newAcc fine" << std::endl;
    level->frame->vars = new F::AccessList( newAcc, level->frame->vars );
    auto finAccess     = new Access( level, newAcc );
}

PatchList* join_patch( PatchList* first, PatchList* second ) {
    if ( !first )
        return second;
    for ( ; first->tail; first = first->tail )
        ;
    first->tail = second;
    return first;
}

Level* Outermost() {
    static Level* lv = nullptr;
    if ( lv != nullptr )
        return lv;

    auto label = TEMP::NewLabel();
    lv         = new Level( new F::Frame( *label ), nullptr );
    return lv;
}

F::FragList* TranslateProgram( A::Exp* root ) {
    std::cout << "Called TranslateProgram(A::Exp* root)." << std::endl;
    root->Translate( E::BaseVEnv(), E::BaseTEnv(), Outermost(), TEMP::NewLabel() );
}

static TY::TyList* make_formal_tylist( TEnvType tenv, A::FieldList* params ) {
    if ( params == nullptr ) {
        return nullptr;
    }

    TY::Ty* ty = tenv->Look( params->head->typ );
    if ( ty == nullptr ) {
        std::cout << params->head->pos << "undefined type " << params->head->typ->Name() << std::endl;
    }

    return new TY::TyList( ty->ActualTy(), make_formal_tylist( tenv, params->tail ) );
}

static TY::FieldList* make_fieldlist( TEnvType tenv, A::FieldList* fields ) {
    if ( fields == nullptr ) {
        return nullptr;
    }

    TY::Ty* ty = tenv->Look( fields->head->typ );
    std::cout << " make_fieldlist kind called. " << fields->head->name->Name() << std::endl;
    if ( !ty ) {
        std::cout << " undefined type " + fields->head->typ->Name() << std::endl;
    }
    return new TY::FieldList( new TY::Field( fields->head->name, ty ), make_fieldlist( tenv, fields->tail ) );
}

static TY::FieldList* make_fieldlist_from_e( TEnvType tenv, A::EFieldList* fields, TR::Level* level, TEMP::Label* label ) {
    if ( fields == nullptr ) {
        return nullptr;
    }

    TY::Ty* ty = fields->head->exp->Translate( nullptr, tenv, level, label ).ty;
    std::cout << " make_fieldlist kind called. " << fields->head->name->Name() << std::endl;

    return new TY::FieldList( new TY::Field( fields->head->name, ty ), make_fieldlist_from_e( tenv, fields->tail, level, label ) );
}

}  // namespace TR

namespace A {

T::Exp* getExp( F::Access* acc, T::Exp* framePtr ) {
    std::cout << "called getExp " << std::endl;
    switch ( acc->kind ) {
    case F::Access::Kind::INFRAME: {
        std::cout << "with a INFRAME access" << std::endl;
        return new T::MemExp( new T::BinopExp( T::BinOp::PLUS_OP, framePtr, new T::ConstExp( reinterpret_cast< F::InFrameAccess* >( acc )->offset ) ) );
    }

    case F::Access::Kind::INREG: {
        std::cout << "with a INREG access" << std::endl;
        return new T::TempExp( reinterpret_cast< F::InRegAccess* >( acc )->reg );
    }
    }
    std::cout << "with nothing! dead" << std::endl;
}

TR::ExpAndTy SimpleVar::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered SimpleVar::Translate. this level: " << level << "; parent level: " << level->parent << std::endl;
    E::EnvEntry* var = venv->Look( this->sym );

    F::Access* accessObj    = nullptr;
    auto       currentLevel = level;

    while ( accessObj == nullptr ) {
        auto accessList = currentLevel->frame->vars;
        while ( accessList ) {
            auto access = accessList->head;
            if ( access->kind == F::Access::Kind::INFRAME ) {
                std::cout << "  compare access sym " << access->sym << ", " << this->sym->Name() << std::endl;
                if ( access->sym == this->sym->Name() ) {
                    accessObj = access;
                    break;
                }
            }
            else if ( access->kind == F::Access::Kind::INREG ) {
                // not going to do this in lab5
            }

            accessList = accessList->tail;
        }
        std::cout << "accessList traverse over" << std::endl;
        if ( accessObj ) {
            std::cout << "Already found~" << std::endl;
            break;
        }
        if ( currentLevel->parent == nullptr ) {
            std::cout << "No... I can't find anything anywhere. " << std::endl;
            break;
        }
        std::cout << "I'll go one layer above. " << std::endl;
        currentLevel = currentLevel->parent;
    }

    auto accessList = level->frame->args;
    while ( accessList ) {
        auto access = accessList->head;

        if ( access->kind == F::Access::Kind::INFRAME ) {
            std::cout << "  compare access sym " << access->sym << ", " << this->sym->Name() << std::endl;
            if ( access->sym == this->sym->Name() ) {
                accessObj = access;
                std::cout << "Found it!" << std::endl;
                break;
            }
        }
        else if ( access->kind == F::Access::Kind::INREG ) {
            // not going to do this in lab5
        }

        accessList = accessList->tail;
    }

    if ( true || var && var->kind == E::EnvEntry::Kind::VAR ) {
        auto tempExp = new T::TempExp( F::Frame::framePointer() );
        std::cout << "successfully build tempExp" << std::endl;
        std::cout << "going to call getExp with access: " << accessObj << std::endl;
        auto targetExp = A::getExp( accessObj, tempExp );
        std::cout << "successfully build targetExp " << targetExp << std::endl;
        auto exp = new TR::ExExp( targetExp );
        std::cout << "successfully generated exp " << exp << std::endl;
        return TR::ExpAndTy( exp, ( ( E::VarEntry* )var )->ty );
    }

    std::cout << "undefined variable " + this->sym->Name() << std::endl;

    return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );

    // return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
}

TR::ExpAndTy FieldVar::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered SimpleVar::Translate." << std::endl;
    auto recT = this->var->Translate( venv, tenv, level, label );
    // if ( recT->kind == E ) {
    auto recEnt = ( E::VarEntry* )recT.ty;

    if ( recT.ty->kind != TY::Ty::Kind::RECORD ) {
        std::cout << "not a record type" << std::endl;
    }

    auto fields = ( ( TY::RecordTy* )( ( A::SimpleVar* )this->var )->Translate( venv, tenv, level, label ).ty )->fields;
    while ( fields ) {
        std::cout << "checking field " << fields->head->name->Name() << " and " << this->sym->Name() << std::endl;
        if ( fields->head->name->Name() == this->sym->Name() ) {
            return TR::ExpAndTy( nullptr, fields->head->ty );
        }
        fields = fields->tail;
    }

    std::cout << "field " + this->sym->Name() + " doesn't exist" << std::endl;
    return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
}

TR::ExpAndTy SubscriptVar::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered SubscriptVar::Translate." << std::endl;

    auto recT = this->var->Translate( venv, tenv, level, label );
    // if ( recT->kind == E ) {
    auto recEnt = recT.ty;

    std::string type_names[] = { "SIMPLE", "FIELD", "SUBSCRIPT" };

    if ( this->var->kind != A::Var::Kind::SUBSCRIPT ) {
        std::cout << "this->var->kind = " << type_names[ this->var->kind ] << std::endl;
        std::cout << "array type required" << std::endl;
    }

    std::string ty_names[] = { "RECORD", "NIL", "INT", "STRING", "ARRAY", "NAME", "VOID" };
    std::cout << "before we call recEnt->ty->kind" << std::endl;
    std::cout << recEnt->kind << std::endl;
    std::cout << "recEnt->ty->kind = " << ty_names[ recEnt->kind ] << std::endl;

    if ( recEnt->kind == TY::Ty::Kind::ARRAY ) {
        auto indexT = this->subscript->Translate( venv, tenv, level, label );
        std::cout << "Translated indexT, which .exp = " << indexT.exp << std::endl;
        auto subscriptExp = new TR::ExExp( new T::MemExp( new T::BinopExp( T::PLUS_OP, recT.exp->UnEx(), new T::BinopExp( T::MUL_OP, indexT.exp->UnEx(), new T::ConstExp( F::wordSize ) ) ) ) );
        return TR::ExpAndTy( subscriptExp, ( ( TY::ArrayTy* )( recEnt ) )->ty );
    }
}

TR::ExpAndTy VarExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered VarExp::Translate." << std::endl;

    if ( this->var->kind == A::Var::Kind::SIMPLE ) {
        std::cout << "analyse a SIMPLE var" << std::endl;
        A::SimpleVar* spvar = ( A::SimpleVar* )this->var;
        return spvar->Translate( venv, tenv, level, label );
    }
    else if ( this->var->kind == A::Var::Kind::FIELD ) {
        std::cout << "analyse a FIELD var" << std::endl;
        A::FieldVar* fvar = ( A::FieldVar* )this->var;
        return fvar->Translate( venv, tenv, level, label );
    }
    else if ( this->var->kind == A::Var::Kind::SUBSCRIPT ) {
        std::cout << "analyse a SUBSCRIPT var" << std::endl;
        A::SubscriptVar* svar = ( A::SubscriptVar* )this->var;
        return svar->Translate( venv, tenv, level, label );
    }
    return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
}

TR::ExpAndTy NilExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered NilExp::Translate." << std::endl;
    return TR::ExpAndTy( nullptr, TY::NilTy::Instance() );
}

TR::ExpAndTy IntExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered IntExp::Translate." << std::endl;
    return TR::ExpAndTy( new TR::ExExp( new T::ConstExp( this->i ) ), TY::IntTy::Instance() );
}

TR::ExpAndTy StringExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered StringExp::Translate." << std::endl;
    return TR::ExpAndTy( nullptr, TY::StringTy::Instance() );
}

TR::ExpAndTy CallExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered CallExp::Translate." << std::endl;

    auto fun = venv->Look( this->func );
    if ( !fun || fun->kind != E::EnvEntry::Kind::FUN ) {
        std::cout << "undefined function " + this->func->Name() << std::endl;
        return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
    }

    std::cout << "found function " + this->func->Name() << std::endl;
    auto        formals = ( ( E::FunEntry* )fun )->formals;
    A::ExpList* arg     = this->args;
    TY::Ty*     retType;

    venv->BeginScope();
    while ( arg || formals ) {
        if ( arg && formals ) {
            std::cout << "comparing arg->head and formals->head" << std::endl;
            if ( !arg->head->Translate( venv, tenv, level, label ).ty->IsSameType( formals->head ) ) {
                std::cout << "para type mismatch" << std::endl;
                std::cout << "ty1: " << arg->head->Translate( venv, tenv, level, label ).ty << "; ty2:" << formals->head << std::endl;
                break;
            }

            arg     = arg->tail;
            formals = formals->tail;
        }
        else {
            if ( arg ) {
                std::cout << "too many params in function " + this->func->Name() << std::endl;
            }
            else {
                std::cout << "too less params in function " + this->func->Name() << std::endl;
            }
            break;
        }
    }

    auto result = ( ( E::FunEntry* )fun )->result;
    std::cout << "Call operation returns an " << result->kind << std::endl;

    auto        rawExps = arg;
    T::ExpList* finExps = nullptr;
    while ( rawExps ) {
        finExps = new T::ExpList( rawExps->head->Translate( venv, tenv, level, label ).exp->UnEx(), finExps );
        rawExps = rawExps->tail;
    }
    venv->EndScope();
    auto finExp = new TR::ExExp( new T::CallExp( new T::NameExp( TEMP::NamedLabel( this->func->Name() ) ), finExps ) );
    return TR::ExpAndTy( finExp, result );
}

TR::ExpAndTy OpExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::string op_names[] = { "PLUS_OP", "MINUS_OP", "TIMES_OP", "DIVIDE_OP", "EQ_OP", "NEQ_OP", "LT_OP", "LE_OP", "GT_OP", "GE_OP" };
    std::cout << "Entered OpExp::Translate. OpCode: " << op_names[ this->oper ] << std::endl;

    auto leftT = left->Translate( venv, tenv, level, label );
    std::cout << "Analyzed leftT, which.exp = " << leftT.exp << std::endl;
    auto rightT = right->Translate( venv, tenv, level, label );
    std::cout << "Analyzed rightT, which.exp = " << rightT.exp << std::endl;
    T::BinOp op;
    T::RelOp rop;
    bool     isBinOpButNotRelOp = false;
    switch ( this->oper ) {
        // this->oper:
        // PLUS_OP, MINUS_OP, TIMES_OP, DIVIDE_OP, EQ_OP, NEQ_OP, LT_OP, LE_OP, GT_OP, GE_OP
        //
        // T::BinOp:
        // PLUS_OP, MINUS_OP, MUL_OP, DIV_OP, AND_OP, OR_OP, LSHIFT_OP, RSHIFT_OP, ARSHIFT_OP, XOR_OP
        // T::RelOp
        // EQ_OP, NE_OP, LT_OP, GT_OP, LE_OP, GE_OP, ULT_OP, ULE_OP, UGT_OP, UGE_OP

    case A::PLUS_OP:
        op                 = T::PLUS_OP;
        isBinOpButNotRelOp = true;
        break;
    case A::MINUS_OP:
        op                 = T::MINUS_OP;
        isBinOpButNotRelOp = true;
        break;
    case A::TIMES_OP:
        op                 = T::MUL_OP;
        isBinOpButNotRelOp = true;
        break;
    case A::DIVIDE_OP:
        op                 = T::DIV_OP;
        isBinOpButNotRelOp = true;
        break;
    case A::EQ_OP:
        rop                = T::EQ_OP;
        isBinOpButNotRelOp = false;
        break;
    case A::NEQ_OP:
        rop                = T::NE_OP;
        isBinOpButNotRelOp = false;
        break;
    case A::LT_OP:
        rop                = T::LT_OP;
        isBinOpButNotRelOp = false;
        break;
    case A::LE_OP:
        rop                = T::LE_OP;
        isBinOpButNotRelOp = false;
        break;
    case A::GT_OP:
        rop                = T::GT_OP;
        isBinOpButNotRelOp = false;
        break;
    case A::GE_OP:
        rop                = T::GE_OP;
        isBinOpButNotRelOp = false;
        break;
    default:
        std::cout << "! invalid oper " << this->oper << std::endl;
        // 去死吧
        assert( 0 );
    }
    TR::Exp* finExp;
    if ( isBinOpButNotRelOp ) {
        finExp = new TR::ExExp( new T::BinopExp( op, leftT.exp->UnEx(), rightT.exp->UnEx() ) );
    }
    else {
        auto stm    = new T::CjumpStm( rop, leftT.exp->UnEx(), rightT.exp->UnEx(), nullptr, nullptr );
        auto trues  = new TR::PatchList( &stm->true_label, nullptr );
        auto falses = new TR::PatchList( &stm->false_label, nullptr );
        finExp      = new TR::CxExp( TR::Cx( trues, falses, stm ) );
    }
    std::cout << "  making a comparison between <type> " << leftT.ty->ActualTy()->kind << " and <type> " << rightT.ty->ActualTy()->kind << std::endl;
    if ( leftT.ty->kind == TY::Ty::Kind::INT && rightT.ty->kind == TY::Ty::Kind::VOID ) {
        std::cout << "integer required" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    else if ( rightT.ty->kind == TY::Ty::Kind::INT && leftT.ty->kind == TY::Ty::Kind::VOID ) {
        std::cout << "integer required" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    else if ( this->oper == A::Oper::PLUS_OP && leftT.ty->kind == TY::Ty::Kind::INT && rightT.ty->kind == TY::Ty::Kind::STRING ) {
        std::cout << "integer required" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    else if ( this->oper == A::Oper::NEQ_OP && !leftT.ty->IsSameType( rightT.ty ) ) {
        std::cout << "same type required" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    else if ( leftT.ty->kind == TY::Ty::Kind::RECORD && !leftT.ty->IsSameType( rightT.ty ) ) {
        std::cout << "type mismatch" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    else if ( !leftT.ty->IsSameType( rightT.ty ) ) {
        std::cout << "same type required" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    else if ( leftT.ty->kind == TY::Ty::Kind::INT && rightT.ty->kind != TY::Ty::Kind::INT ) {
        std::cout << "integer required" << std::endl;
        return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
    }
    return TR::ExpAndTy( finExp, TY::IntTy::Instance() );
}

TR::ExpAndTy RecordExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered RecordExp::Translate." << std::endl;
    auto recT = tenv->Look( this->typ );
    if ( !recT ) {
        std::cout << "undefined type rectype" << std::endl;
    }
    return TR::ExpAndTy( nullptr, recT /*new TY::RecordTy( make_fieldlist_from_e( tenv, this->fields, level, label ) ) */ );
}

TR::ExpAndTy SeqExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered SeqExp::Translate." << std::endl;
    ExpList* seq = this->seq;
    if ( seq == nullptr ) {
        return TR::ExpAndTy( new TR::NxExp( nullptr ), TY::VoidTy::Instance() );
    }
    T::EseqExp* node = new T::EseqExp( nullptr, nullptr );
    // T::EseqExp* node = nullptr;

    // std::vector< TR::Exp* > exps;

    TY::Ty* lastType = nullptr;
    while ( seq ) {
        std::cout << "Add one translation layer" << std::endl;
        auto la = seq->head->Translate( venv, tenv, level, label );
        std::cout << "la.exp = " << la.exp << std::endl;
        auto newExp = la.exp->UnEx();
        lastType    = la.ty;
        if ( node->exp == nullptr ) {
            node->exp = newExp;
        }
        else {
            node->exp = new T::EseqExp( new T::ExpStm( node->exp ), newExp );
        }
        seq = seq->tail;
    }

    return TR::ExpAndTy( new TR::ExExp( node ), lastType );
}

TR::ExpAndTy AssignExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered AssignExp::Translate." << std::endl;

    auto varKind = this->var->Translate( venv, tenv, level, label );
    auto expT    = this->exp->Translate( venv, tenv, level, label );

    if ( varKind.ty->kind == TY::Ty::Kind::VOID ) {
    }
    else if ( varKind.ty->kind == TY::Ty::Kind::RECORD ) {
        if ( !expT.ty->IsSameType( varKind.ty ) ) {
            std::cout << "type mismatch: " << expT.ty << " and " << varKind.ty << std::endl;
        }
    }
    else {
        if ( !this->exp->Translate( venv, tenv, level, label ).ty->IsSameType( varKind.ty ) ) {
            std::cout << "unmatched assign exp" << std::endl;
        }
    }

    return TR::ExpAndTy( new TR::NxExp( nullptr ), TY::VoidTy::Instance() );
}

TR::ExpAndTy IfExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered IfExp::Translate" << std::endl;

    auto testT = this->test->Translate( venv, tenv, level, label );
    if ( testT.ty->kind != TY::Ty::Kind::INT ) {
        return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
    }
    // randomly initialize anyway
    TR::ExpAndTy thenT( nullptr, nullptr ), elseT( nullptr, nullptr );
    TY::Ty*      ifReturnType = nullptr;

    if ( !this->elsee ) {
        thenT = this->then->Translate( venv, tenv, level, label );
        if ( thenT.ty->kind != TY::Ty::Kind::VOID ) {
            std::cout << "if-then exp's body must produce no value" << std::endl;
            assert( 0 );
        }
        ifReturnType = nullptr;
    }
    else {
        thenT = this->then->Translate( venv, tenv, level, label );
        elseT = this->elsee->Translate( venv, tenv, level, label );
        if ( !thenT.ty->IsSameType( elseT.ty ) ) {
            std::cout << "then exp and else exp type mismatch" << std::endl;
            assert( 0 );
        }
        ifReturnType = elseT.ty;
    }

    auto t = TEMP::NewLabel(), f = TEMP::NewLabel(), done = TEMP::NewLabel();
    if ( ifReturnType ) {
        auto access = TR::AllocLocal( level, true, "__RETURN__" );
        getExp( access->access, new T::TempExp( level->frame->framePointer() ) );
    }
    auto cx = testT.exp->UnCx();
    TR::do_patch( cx.trues, t );
    TR::do_patch( cx.falses, f );

    TR::Exp* finExp;
    if ( ifReturnType ) {
        /* if exp has a return value */
        auto seq = new T::SeqStm(
            cx.stm, new T::SeqStm( new T::LabelStm( t ),
                                   new T::SeqStm( new T::MoveStm( nullptr, thenT.exp->UnEx() ),
                                                  new T::SeqStm( new T::JumpStm( new T::NameExp( done ), new TEMP::LabelList( done, nullptr ) ),
                                                                 new T::SeqStm( new T::LabelStm( f ), new T::SeqStm( new T::MoveStm( nullptr, elseT.exp->UnEx() ), new T::LabelStm( done ) ) ) ) ) ) );

        finExp = new TR::ExExp( new T::EseqExp( seq, nullptr ) );

        return TR::ExpAndTy( finExp, ifReturnType );
    }
    else {
        /* if exp has no return value */
        finExp = new TR::NxExp( new T::SeqStm( cx.stm, new T::SeqStm( new T::LabelStm( t ), new T::SeqStm( thenT.exp->UnNx(), new T::LabelStm( f ) ) ) ) );
        return TR::ExpAndTy(finExp, TY::VoidTy::Instance());
    }
}

TR::ExpAndTy WhileExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered WhileExp::Translate." << std::endl;

    auto expAndTy = this->body->Translate( venv, tenv, level, label );
    if ( expAndTy.ty->kind != TY::Ty::Kind::VOID ) {
        std::cout << "while body must produce no value" << std::endl;
    }
    return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
}

TR::ExpAndTy ForExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered ForExp::Translate." << std::endl;

    auto loTrans = this->lo->Translate( venv, tenv, level, label );
    std::cout << "loTrans success. loTrans.exp = " << loTrans.exp << std::endl;
    auto hiTrans = this->hi->Translate( venv, tenv, level, label );
    std::cout << "hiTrans success. hiTrans.exp = " << hiTrans.exp << std::endl;
    if ( hiTrans.ty->kind != TY::Ty::Kind::INT ) {
        std::cout << "for exp's range type is not integer" << std::endl;
    }
    if ( this->body->kind == A::Exp::Kind::ASSIGN ) {
        std::cout << "loop variable can't be assigned" << std::endl;
    }

    auto        start    = TEMP::NewLabel();
    auto        t        = TEMP::NewLabel();
    auto        done     = TEMP::NewLabel();
    TR::Access* loopVar  = TR::AllocLocal( level, true, this->var->Name() );
    TR::Access* limitVar = TR::AllocLocal( level, true, "__l__" );
    venv->BeginScope();
    venv->Enter( S::Symbol::UniqueSymbol( this->var->Name() ), new E::VarEntry( new TR::Access( level, loopVar->access ), TY::IntTy::Instance(), true ) );
    venv->Enter( S::Symbol::UniqueSymbol( "__l__" ), new E::VarEntry( new TR::Access( level, loopVar->access ), TY::IntTy::Instance(), true ) );

    auto bodyTrans = this->body->Translate( venv, tenv, level, label );
    std::cout << "bodyTrans success. bodyTrans.exp = " << bodyTrans.exp << std::endl;
    auto limit  = getExp( limitVar->access, new T::TempExp( level->frame->framePointer() ) );
    auto loop   = getExp( loopVar->access, new T::TempExp( level->frame->framePointer() ) );
    auto finExp = new TR::NxExp( new T::SeqStm(
        new T::MoveStm( limit, hiTrans.exp->UnEx() ),
        new T::SeqStm(
            new T::MoveStm( loop, loTrans.exp->UnEx() ),
            new T::SeqStm( new T::CjumpStm( T::LE_OP, loop, limit, start, done ),
                           new T::SeqStm( new T::LabelStm( start ),
                                          new T::SeqStm( bodyTrans.exp->UnNx(),
                                                         new T::SeqStm( new T::CjumpStm( T::LT_OP, loop, limit, t, done ),
                                                                        new T::SeqStm( new T::LabelStm( t ),
                                                                                       new T::SeqStm( new T::MoveStm( loop, new T::BinopExp( T::PLUS_OP, loop, new T::ConstExp( 1 ) ) ),
                                                                                                      new T::SeqStm( new T::JumpStm( new T::NameExp( start ), new TEMP::LabelList( start, nullptr ) ),
                                                                                                                     new T::LabelStm( done ) ) ) ) ) ) ) ) ) ) );
    venv->EndScope();
    return TR::ExpAndTy( finExp, TY::VoidTy::Instance() );
}

TR::ExpAndTy BreakExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered BreakExp::Translate." << std::endl;
    return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
}

TR::ExpAndTy LetExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered LetExp::Translate." << std::endl;

    A::DecList* node = this->decs;
    while ( node ) {
        // std::cout << "while!" << std::endl;
        node->head->Translate( venv, tenv, level, label );
        node = node->tail;
    }

    return this->body->Translate( venv, tenv, level, label );

    // return TR::ExpAndTy( nullptr, TY::VoidTy::Instance() );
}

TR::ExpAndTy ArrayExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered ArrayExp::Translate." << std::endl;
    auto    initTsl  = this->init->Translate( venv, tenv, level, label );
    auto    sizeTsl  = this->size->Translate( venv, tenv, level, label );
    T::Exp* arrayExp = new T::CallExp( new T::NameExp( TEMP::NamedLabel( "initArray" ) ), new T::ExpList( sizeTsl.exp->UnEx(), new T::ExpList( sizeTsl.exp->UnEx(), nullptr ) ) );

    return TR::ExpAndTy( new TR::ExExp( arrayExp ), tenv->Look( this->typ ) );
}

TR::ExpAndTy VoidExp::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered VoidExp::Translate." << std::endl;
    return TR::ExpAndTy( new TR::NxExp( nullptr ), TY::VoidTy::Instance() );
}

TR::Exp* FunctionDec::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered FunctionDec::Translate." << std::endl;

    FunDecList* func = this->functions;
    while ( func ) {
        // std::cout << "Analyse FunDecList. this time, func = " << func << std::endl;
        auto head = func->head;
        std::cout << "To-declare function name: " << head->name->Name() << std::endl;
        // if ( venv->Look( head->name ) ) {
        //     std::cout << "two functions have the same name" << std::endl;
        // }

        if ( head->result ) {
            venv->Enter( head->name, new E::FunEntry( TR::make_formal_tylist( tenv, head->params ), tenv->Look( head->result ) ) );
        }
        else {
            venv->Enter( head->name, new E::FunEntry( TR::make_formal_tylist( tenv, head->params ), TY::VoidTy::Instance() ) );
        }
        if ( func->tail ) {
            venv->Enter( func->tail->head->name, new E::FunEntry( nullptr, TY::VoidTy::Instance() ) );
        }
        auto   field           = head->params;
        size_t inputParamCount = 0;

        venv->BeginScope();
        auto blist       = new U::BoolList( true, "__STATIC_LINK__", nullptr );
        auto originBlist = blist;
        while ( field ) {
            auto fh = field->head;
            venv->Enter( fh->name, new E::VarEntry( tenv->Look( fh->typ ) ) );
            std::cout << "constructing formal: " << fh->name->Name() << ", type: " << fh->typ->Name() << ", id: " << tenv->Look( fh->typ ) << std::endl;

            blist->tail = new U::BoolList( true, fh->name->Name(), nullptr );
            blist       = blist->tail;
            std::cout << " param +1" << std::endl;
            field = field->tail;
            ++inputParamCount;
        }

        std::cout << "===== PRINT BLIST =====" << std::endl;
        auto newblist = originBlist;

        while ( newblist ) {
            std::cout << "# " << newblist->symbol << std::endl;
            newblist = newblist->tail;
        }
        auto newlabel = TEMP::NamedLabel( head->name->Name() );
        std::cout << "I'm entering the evil part of translate, which didn't give me a good exp " << std::endl;
        auto bodyTranslate = head->body->Translate( venv, tenv, level->NewLevel( level, newlabel, originBlist ), newlabel );
        std::cout << "bodyTranslate success. which .exp = " << bodyTranslate.exp << std::endl;
        ;
        auto retType = bodyTranslate.ty;

        field = head->params;
        while ( inputParamCount > 0 ) {
            std::cout << "try pop one out" << std::endl;
            venv->Pop();

            --inputParamCount;
        }
        std::cout << "function return value type: " << ( head->result ? head->result->Name() : "NULL" ) << std::endl;
        if ( ( ( E::FunEntry* )( venv->Look( head->name ) ) )->result->kind == TY::Ty::Kind::VOID && retType->kind != TY::Ty::Kind::VOID ) {
            std::cout << "procedure returns value" << std::endl;
        }
        std::cout << "head->result: " << head->result << ", retType->kind: " << retType->kind << std::endl;
        std::cout << "going to unEx bodyTranslate. Now it is actually" << bodyTranslate.exp << std::endl;
        auto bodyEx = bodyTranslate.exp->UnEx();
        std::cout << "bodyEx now is " << bodyEx << std::endl;
        new TR::NxExp( new T::MoveStm( new T::TempExp( F::Frame::returnValue() ), bodyEx ) );

        venv->EndScope();
        func = func->tail;
    }
    return new TR::NxExp( nullptr );
}

TR::Exp* VarDec::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered VarDec::Translate. going to declare var: " << this->var->Name() << std::endl;
    auto initT = this->init->Translate( venv, tenv, level, label );
    if ( this->typ ) {
        std::cout << "explicit type" << tenv->Look( this->typ ) << std::endl;
        std::cout << "implicit type: " << initT.ty << std::endl;
        venv->Enter( this->var, new E::VarEntry( tenv->Look( this->typ ) ) );
        // if ( this->typ->name != this->init->SemAnalyze( venv, tenv, labelcount ) ) {
        if ( !tenv->Look( this->typ )->IsSameType( initT.ty ) ) {
            // if ( tenv->Look( this->typ )->kind == TY::Ty::Kind::RECORD || tenv->Look( this->typ )->kind == TY::Ty::Kind::ARRAY ) {
            std::cout << "type mismatch" << std::endl;
            // }
            // else {
            // errormsg.Error( labelcount, "same type required" );
            // }
        }
        // }
    }
    else {
        std::cout << "implicit type: " << initT.ty << std::endl;
        if ( initT.ty->kind == TY::Ty::Kind::NIL ) {
            std::cout << "init should not be nil without type specified" << std::endl;
        }
        venv->Enter( this->var, new E::VarEntry( initT.ty ) );
    }

    TR::Access* new_acc = TR::AllocLocal( level, true, this->var->Name() );
    std::cout << "AllocLocal fine" << std::endl;
    auto resultExp = new TR::NxExp( new T::MoveStm( new_acc->access->ToExp( new T::TempExp( level->frame->framePointer() ) ), initT.exp->UnEx() ) );
    std::cout << "successfully generated resultExp" << resultExp << std::endl;
    return resultExp;
}

TR::Exp* TypeDec::Translate( S::Table< E::EnvEntry >* venv, S::Table< TY::Ty >* tenv, TR::Level* level, TEMP::Label* label ) const {
    std::cout << "Entered TypeDec::Translate." << std::endl;
    // std::cout << "Entered TypeDec::SemAnalyse; labelcount: " << labelcount << std::endl;
    // NameAndTyList*           natl = this->types;
    // TY::Ty*                  last = nullptr;
    // std::vector< NameAndTy > nstack;
    // while ( natl ) {
    //     nstack.push_back( natl->head );
    //     natl = natl->tail;
    // }
    // size_t lastEnds = nstack.size
    // while ( natl != nullptr ) {
    // std::cout << "gonna declare type " << natl->head->name->Name() << std::endl;
    // if ( natl->head->ty->SemAnalyze( tenv ) ) {
    //     tenv->Enter( natl->head->name, natl->head->ty->SemAnalyze( tenv ) );
    //     }
    //     natl = natl->tail;
    // }

    NameAndTyList*           natl = this->types;
    std::deque< NameAndTy* > queue;
    while ( natl ) {
        queue.push_back( natl->head );
        natl = natl->tail;
    }
    if ( queue.size() > 1 ) {
        std::cout << "queue.size() > 1" << std::endl;
        size_t unmovedTimes = 0;
        while ( unmovedTimes < queue.size() && queue.size() ) {
            auto nt = queue[ 0 ];
            queue.pop_front();
            // tenv->Enter( nt->name, TY::NilTy::Instance() );
            auto ans = nt->ty->Translate( tenv );
            if ( ans ) {
                unmovedTimes = 0;
                if ( tenv->Look( nt->name ) ) {
                    std::cout << "two types have the same name" << std::endl;
                }
                tenv->Enter( nt->name, ans );
            }
            else {
                queue.push_back( nt );
                unmovedTimes += 1;
            }
            std::cout << "declared type " << nt->name->Name() << ", id: " << tenv->Look( nt->name ) << std::endl;
        }
        if ( queue.size() ) {
            std::cout << "illegal type cycle" << std::endl;
        }
    }
    else {
        auto nt = queue[ 0 ];
        tenv->Enter( nt->name, TY::NilTy::Instance() );
        std::cout << "queue.size() <= 1" << std::endl;

        auto ans = nt->ty->Translate( tenv );
        if ( ans ) {
            tenv->Set( nt->name, ans );
        }
        else {
            std::cout << " undefined type " << std::endl;
        }
        std::cout << "declared type " << nt->name->Name() << ", id: " << tenv->Look( nt->name ) << std::endl;
    }

    return new TR::NxExp( nullptr );
}

TY::Ty* NameTy::Translate( S::Table< TY::Ty >* tenv ) const {
    std::cout << "Entered NameTy::Translate." << std::endl;
    auto ty = tenv->Look( this->name );
    if ( ty ) {
        return new TY::NameTy( this->name, ty );
    }
    else {
        std::cout << " undefined type " + this->name->Name() << std::endl;
    }
}

TY::Ty* RecordTy::Translate( S::Table< TY::Ty >* tenv ) const {
    std::cout << "Entered RecordTy::Translate." << std::endl;
    return new TY::RecordTy( TR::make_fieldlist( tenv, this->record ) );
}

TY::Ty* ArrayTy::Translate( S::Table< TY::Ty >* tenv ) const {
    std::cout << "Entered ArrayTy::Translate." << std::endl;
    return new TY::ArrayTy( tenv->Look( this->array ) );
}

}  // namespace A