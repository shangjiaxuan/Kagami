#pragma once
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <map>
#include <deque>
#include <regex>
#include <stack>
#include <locale>
#include <codecvt>
#include <cstdlib>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <ctime>
#include <type_traits>
#include <iterator>
#include <list>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>

//if you build this project by cmake,please turn off all switch macro below.
//Disbale SDL2 componets for non-GUI environment
//#define _DISABLE_SDL_

//Switching Debugging Feature
//#define _DEBUG_

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include "wstcp_wrapper.h"
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#else
#include <dlfcn.h>
#endif

#if not defined(_DISABLE_SDL_)
#if defined(_WIN32)
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")
#if defined(_DEBUG)
#pragma comment(lib,"SDL2test.lib")
#endif
#pragma comment(lib,"SDL2_image.lib")
#else
#endif
#include <SDL.h>
#include <SDL_image.h>
#endif

#define MAX_ERROR_COUNT 20

namespace kagami {
  using std::string;
  using std::pair;
  using std::vector;
  using std::map;
  using std::deque;
  using std::shared_ptr;
  using std::unique_ptr;
  using std::static_pointer_cast;
  using std::dynamic_pointer_cast;
  using std::regex;
  using std::regex_match;
  using std::make_shared;
  using std::make_unique;
  using std::size_t;
  using std::ifstream;
  using std::ofstream;
  using std::stack;
  using std::to_string;
  using std::stoi;
  using std::stof;
  using std::stod;
  using std::stol;
  using std::stoul;
  using std::wstring;
  using std::ostream;
  using std::list;
  using std::thread;
  using std::mutex;
  using std::lock_guard;
#if defined (_WIN32)
  using suzu::TCPClient;
  using suzu::TCPServer;
  using suzu::TCPConnector;
  using suzu::WSockInfo;
#endif

  using Byte = unsigned int;

  /* Application Info */
  const string kInterpreterVersion = "2.0";
  const string kPatchName          = "Colorless";
#if defined(_WIN32)
  const string kPlatformType   = "Windows";
#else
  const string kPlatformType   = "Linux";
#endif
  const string kEngineName     = "Kagami Project";
  const string kMaintainer     = "Suzu Nakamura";
  const string kCopyright      = "Copyright(c) 2019";

  /* Message state code for Message class */
  enum StateCode {
    kCodeInterface = 5,
    kCodeAutoSize = 4,
    kCodeAutoFill = 3,
    kCodeNormalParam = 2,
    kCodeObject = 1,
    kCodeSuccess = 0,
    kCodeIllegalParam = -1,
    kCodeIllegalCall = -2,
    kCodeIllegalSymbol = -3,
    kCodeBadStream = -4,
    kCodeBadExpression = -5
  };

  /* Message state level for Message class */
  enum StateLevel {
    kStateNormal,
    kStateError,
    kStateWarning
  };

  enum TokenType {
    kTokenTypeGeneric, 
    kTokenTypeString, 
    kTokenTypeInt, 
    kTokenTypeFloat,
    kTokenTypeBool, 
    kTokenTypeSymbol, 
    kTokenTypeBlank,
    kTokenTypeNull
  };

  using Token = pair<string, TokenType>;

  /* Reserved keywords mark/IR framework commands */
  enum GenericToken {
    kTokenSwap,
    kTokenRequire,
    kTokenUsing,
    kTokenSegment,
    kTokenExpList, 
    kTokenFn, 
    kTokenIf, 
    kTokenElif, 
    kTokenEnd, 
    kTokenElse, 
    kTokenBind, 
    kTokenWhile, 
    kTokenPlus, 
    kTokenMinus, 
    kTokenTimes, 
    kTokenDivide, 
    kTokenEquals, 
    kTokenLessOrEqual, 
    kTokenGreaterOrEqual, 
    kTokenNotEqual,
    kTokenGreater, 
    kTokenLess, 
    kTokenReturn,
    kTokenAnd, 
    kTokenOr, 
    kTokenNot, 
    kTokenBitAnd, 
    kTokenBitOr, 
    kTokenBitNot,
    kTokenInitialArray, 
    kTokenAssert, 
    kTokenAssertR,
    kTokenContinue, 
    kTokenBreak, 
    kTokenCase, 
    kTokenWhen, 
    kTokenTypeId, 
    kTokenExist,
    kTokenDir, 
    kTokenQuit,
    kTokenNull
  };

  const vector<GenericToken> nest_flag_collection = {
    kTokenIf,kTokenWhile,kTokenFn,kTokenCase
  };

  enum Terminator {
    kBasicTokenAssign, 
    kBasicTokenComma, 
    kBasicTokenLeftSqrBracket, 
    kBasicTokenDot,
    kBasicTokenLeftBracket, 
    kBasicTokenRightSqrBracket, 
    kBasicTokenRightBracket,
    kBasicTokenLeftCurBracket, 
    kBasicTokenRightCurBracket, 
    kBasicTokenMonoOperator,
    kBasicTokenOther
  };

  /* IR framework runtime mode code */
  enum MachineMode {
    kModeNormal,
    kModeNextCondition,
    kModeCycle,
    kModeCycleJump,
    kModeCondition,
    kModeDef,
    kModeCase,
    kModeCaseJump,
    kModeClosureCatching
  };

  /* Embedded type identifier strings */
  const string kTypeIdNull            = "null";
  const string kTypeIdInt             = "int";
  const string kTypeIdFloat           = "float";
  const string kTypeIdBool            = "bool";
  const string kTypeIdByte            = "byte";
  const string kTypeIdString          = "string";
  const string kTypeIdWideString      = "wstring";
  const string kTypeIdArray           = "array";
  const string kTypeIdInStream        = "instream";
  const string kTypeIdOutStream       = "outstream";
  const string kTypeIdRegex           = "regex";
  const string kTypeIdFunction        = "function";
  const string kTypeIdTCPClient       = "TCPClient";
  const string kTypeIdTCPServer       = "TCPServer";
  const string kTypeIdClientConnector = "ClientConnector";
  const string kTypeIdTCPConnector    = "TCPConnector";
  const string kTypeIdIterator        = "iterator";

  const int kPlainIdInt = 0;
  const int kPlainIdFloat = 1;
  const int kPlainIdString = 2;
  const int kPlainIdBool = 3;
  const int kPlainIdArray = 4;

  const string
    kStrIf             = "if",
    kStrFn             = "fn",
    kStrEnd            = "end",
    kStrPrint          = "__print",
    kStrCaseObj        = "__case",
    kStrCommentBegin   = "=begin",
    kStrCommentEnd     = "=end",
    kStrRequire        = "require",
    kStrUsing          = "using",
    kStrFor            = "for",
    kStrElse           = "else",
    kStrElif           = "elif",
    kStrWhile          = "while",
    kStrContinue       = "continue",
    kStrBreak          = "break",
    kStrCase           = "case",
    kStrWhen           = "when",
    kStrReturn         = "return",
    kStrOptional       = "optional",
    kStrVaribale       = "variable",
    kStrPlus           = "+",
    kStrMinus          = "-",
    kStrTimes          = "*",
    kStrDiv            = "/",
    kStrIs             = "==",
    kStrAnd            = "&&",
    kStrOr             = "||",
    kStrNot            = "!",
    kStrBitAnd         = "&",
    kStrBitOr          = "|",
    kStrBitNot         = "~",
    kStrLessOrEqual    = "<=",
    kStrGreaterOrEqual = ">=",
    kStrNotEqual       = "!=",
    kStrGreater        = ">",
    kStrLess           = "<",
    kStrUserFunc       = "__func",
    kStrArray          = "__array",
    kStrTypeId         = "typeid",
    kStrDir            = "dir",
    kStrExist          = "exist",
    kStrSwap           = "swap",
    kStrTrue           = "true",
    kStrFalse          = "false",
    kStrMember         = "__member",
    kStrCompare        = "__compare",
    kStrRightHandSide  = "__rhs",
    kStrLeftHandSide   = "__lhs",
    kStrObject         = "__object";

  /* Compare multiple objects */
  template <class Tx, class Ty>
  inline bool compare(Tx lhs, const std::initializer_list<Ty> rhs) {
    bool result = false;
    for (const auto &unit : rhs) {
      if (lhs == unit) result = true;
    }
    return result;
  }

  template <class T>
  inline bool find_in_vector(T t, vector<T> vec) {
    for (auto &unit : vec) {
      if (t == unit) {
        return true;
      }
    }

    return false;
  }
}

