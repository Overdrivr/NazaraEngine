// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Lua scripting module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Lua/LuaInstance.hpp>
#include <Lua/lauxlib.h>
#include <Lua/lua.h>
#include <Lua/lualib.h>
#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/MemoryHelper.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <cstdlib>
#include <stdexcept>
#include <unordered_map>
#include <Nazara/Lua/Debug.hpp>

namespace
{
	nzLuaType FromLuaType(int type)
	{
		switch (type)
		{
			case LUA_TBOOLEAN:
				return nzLuaType_Boolean;

			case LUA_TFUNCTION:
				return nzLuaType_Function;

			case LUA_TLIGHTUSERDATA:
				return nzLuaType_LightUserdata;

			case LUA_TNIL:
				return nzLuaType_Nil;

			case LUA_TNONE:
				return nzLuaType_None;

			case LUA_TNUMBER:
				return nzLuaType_Number;

			case LUA_TSTRING:
				return nzLuaType_String;

			case LUA_TTABLE:
				return nzLuaType_Table;

			case LUA_TTHREAD:
				return nzLuaType_Thread;

			case LUA_TUSERDATA:
				return nzLuaType_Userdata;

			default:
				return nzLuaType_None;
		}
	}

	struct StreamData
	{
		NzInputStream* stream;
		char buffer[NAZARA_CORE_FILE_BUFFERSIZE];
	};

	int AtPanic(lua_State* state)
	{
		NzString lastError(lua_tostring(state, -1));

		throw std::runtime_error("Lua panic: " + lastError);
	}

	const char* StreamReader(lua_State* state, void* data, std::size_t* size)
	{
		NazaraUnused(state);

		StreamData* streamData = static_cast<StreamData*>(data);

		if (streamData->stream->EndOfStream())
			return nullptr;
		else
		{
			*size = streamData->stream->Read(streamData->buffer, NAZARA_CORE_FILE_BUFFERSIZE);
			return streamData->buffer;
		}
	}

	int s_comparisons[] = {
		LUA_OPEQ, // nzLuaComparison_Equality
		LUA_OPLT, // nzLuaComparison_Less
		LUA_OPLE  // nzLuaComparison_LessOrEqual
	};

	static_assert(sizeof(s_comparisons)/sizeof(int) == nzLuaComparison_Max+1, "Lua comparison array is incomplete");

	int s_operations[] = {
		LUA_OPADD,  // nzLuaOperation_Addition
		LUA_OPBAND, // nzLuaOperation_BitwiseAnd
		LUA_OPSHL,  // nzLuaOperation_BitwiseLeftShift
		LUA_OPBNOT, // nzLuaOperation_BitwiseNot
		LUA_OPBOR,  // nzLuaOperation_BitwiseOr
		LUA_OPSHR,  // nzLuaOperation_BitwiseRightShift
		LUA_OPBXOR, // nzLuaOperation_BitwiseXOr
		LUA_OPDIV,  // nzLuaOperation_Division
		LUA_OPPOW,  // nzLuaOperation_Exponentiation
		LUA_OPIDIV, // nzLuaOperation_FloorDivision
		LUA_OPMUL,  // nzLuaOperation_Multiplication
		LUA_OPMOD,  // nzLuaOperation_Modulo
		LUA_OPUNM,  // nzLuaOperation_Negation
		LUA_OPSUB   // nzLuaOperation_Substraction
	};

	static_assert(sizeof(s_operations)/sizeof(int) == nzLuaOperation_Max+1, "Lua operation array is incomplete");

	int s_types[] = {
		LUA_TBOOLEAN,       // nzLuaType_Boolean
		LUA_TFUNCTION,      // nzLuaType_Function
		LUA_TLIGHTUSERDATA, // nzLuaType_LightUserdata
		LUA_TNIL,           // nzLuaType_Nil
		LUA_TNUMBER,        // nzLuaType_Number
		LUA_TNONE,          // nzLuaType_None
		LUA_TSTRING,        // nzLuaType_String
		LUA_TTABLE,         // nzLuaType_Table
		LUA_TTHREAD,        // nzLuaType_Thread
		LUA_TUSERDATA       // nzLuaType_Userdata
	};

	static_assert(sizeof(s_types)/sizeof(int) == nzLuaType_Max+1, "Lua type array is incomplete");
}

NzLuaInstance::NzLuaInstance() :
m_memoryLimit(0),
m_memoryUsage(0),
m_timeLimit(1000),
m_level(0)
{
	m_state = lua_newstate(MemoryAllocator, this);
	lua_atpanic(m_state, AtPanic);
	lua_sethook(m_state, TimeLimiter, LUA_MASKCOUNT, 1000);
	luaL_openlibs(m_state);
}

NzLuaInstance::~NzLuaInstance()
{
	lua_close(m_state);
}

void NzLuaInstance::ArgCheck(bool condition, unsigned int argNum, const char* error)
{
	luaL_argcheck(m_state, condition, argNum, error);
}

void NzLuaInstance::ArgCheck(bool condition, unsigned int argNum, const NzString& error)
{
	luaL_argcheck(m_state, condition, argNum, error.GetConstBuffer());
}

int NzLuaInstance::ArgError(unsigned int argNum, const char* error)
{
	return luaL_argerror(m_state, argNum, error);
}

int NzLuaInstance::ArgError(unsigned int argNum, const NzString& error)
{
	return luaL_argerror(m_state, argNum, error.GetConstBuffer());
}

bool NzLuaInstance::Call(unsigned int argCount)
{
	return Run(argCount, LUA_MULTRET);
}

bool NzLuaInstance::Call(unsigned int argCount, unsigned int resultCount)
{
	return Run(argCount, resultCount);
}

void NzLuaInstance::CheckAny(int index) const
{
	luaL_checkany(m_state, index);
}

bool NzLuaInstance::CheckBoolean(int index) const
{
	if (lua_isnoneornil(m_state, index))
	{
		const char* msg = lua_pushfstring(m_state, "%s expected, got %s", lua_typename(m_state, LUA_TBOOLEAN), luaL_typename(m_state, index));
		luaL_argerror(m_state, index, msg); // Lance une exception
		return false;
	}

	return lua_toboolean(m_state, index) != 0;
}

bool NzLuaInstance::CheckBoolean(int index, bool defValue) const
{
	if (lua_isnoneornil(m_state, index))
		return defValue;

	return lua_toboolean(m_state, index) != 0;
}

long long NzLuaInstance::CheckInteger(int index) const
{
	return luaL_checkinteger(m_state, index);
}

long long NzLuaInstance::CheckInteger(int index, long long defValue) const
{
	return luaL_optinteger(m_state, index, defValue);
}

double NzLuaInstance::CheckNumber(int index) const
{
	return luaL_checknumber(m_state, index);
}

double NzLuaInstance::CheckNumber(int index, double defValue) const
{
	return luaL_optnumber(m_state, index, defValue);
}

void NzLuaInstance::CheckStack(int space, const char* error) const
{
	luaL_checkstack(m_state, space, error);
}

void NzLuaInstance::CheckStack(int space, const NzString& error) const
{
	CheckStack(space, error.GetConstBuffer());
}

const char* NzLuaInstance::CheckString(int index, std::size_t* length) const
{
	return luaL_checklstring(m_state, index, length);
}

const char* NzLuaInstance::CheckString(int index, const char* defValue, std::size_t* length) const
{
	return luaL_optlstring(m_state, index, defValue, length);
}

void NzLuaInstance::CheckType(int index, nzLuaType type) const
{
	#ifdef NAZARA_DEBUG
	if (type > nzLuaType_Max)
	{
		NazaraError("Lua type out of enum");
		return;
	}
	#endif

	luaL_checktype(m_state, index, s_types[type]);
}

void* NzLuaInstance::CheckUserdata(int index, const char* tname) const
{
	return luaL_checkudata(m_state, index, tname);
}

void* NzLuaInstance::CheckUserdata(int index, const NzString& tname) const
{
	return luaL_checkudata(m_state, index, tname.GetConstBuffer());
}

bool NzLuaInstance::Compare(int index1, int index2, nzLuaComparison comparison) const
{
	#ifdef NAZARA_DEBUG
	if (comparison > nzLuaComparison_Max)
	{
		NazaraError("Lua comparison out of enum");
		return false;
	}
	#endif

	return (lua_compare(m_state, index1, index2, s_comparisons[comparison]) != 0);
}

void NzLuaInstance::Compute(nzLuaOperation operation)
{
	#ifdef NAZARA_DEBUG
	if (operation > nzLuaOperation_Max)
	{
		NazaraError("Lua operation out of enum");
		return;
	}
	#endif

	lua_arith(m_state, s_operations[operation]);
}

void NzLuaInstance::Concatenate(int count)
{
	lua_concat(m_state, count);
}

int NzLuaInstance::CreateReference()
{
	return luaL_ref(m_state, LUA_REGISTRYINDEX);
}

void NzLuaInstance::DestroyReference(int ref)
{
	luaL_unref(m_state, LUA_REGISTRYINDEX, ref);
}

NzString NzLuaInstance::DumpStack() const
{
	NzStringStream stream;
	unsigned int stackTop = GetStackTop();
	stream << stackTop << " entries\n";

	for (unsigned int i = 1; i <= stackTop; ++i)
	{
		stream << i << ": ";
		switch (GetType(i))
		{
			case nzLuaType_Boolean:
				stream << "Boolean(" << ToBoolean(i) << ')';
				break;

			case nzLuaType_Function:
				stream << "Function(" << ToPointer(i) << ')';
				break;

			case nzLuaType_LightUserdata:
			case nzLuaType_Userdata:
				stream << "Userdata(" << ToUserdata(i) << ')';
				break;

			case nzLuaType_Nil:
				stream << "Nil";
				break;

			case nzLuaType_None:
				stream << "None";
				break;

			case nzLuaType_Number:
				stream << "Number(" << ToNumber(i) << ')';
				break;

			case nzLuaType_String:
				stream << "String(" << ToString(i) << ')';
				break;

			case nzLuaType_Table:
				stream << "Table(" << ToPointer(i) << ')';
				break;

			case nzLuaType_Thread:
				stream << "Thread(" << ToPointer(i) << ')';
				break;

			default:
				stream << "Unknown(" << ToPointer(i) << ')';
				break;
		}

		stream << '\n';
	}

	return stream.ToString();
}

void NzLuaInstance::Error(const char* message)
{
	luaL_error(m_state, message);
}

void NzLuaInstance::Error(const NzString& message)
{
	luaL_error(m_state, message.GetConstBuffer());
}

bool NzLuaInstance::Execute(const NzString& code)
{
	if (code.IsEmpty())
		return true;

	if (luaL_loadstring(m_state, code.GetConstBuffer()) != 0)
	{
		m_lastError = lua_tostring(m_state, -1);
		lua_pop(m_state, 1);

		return false;
	}

	return Run(0, 0);
}

bool NzLuaInstance::ExecuteFromFile(const NzString& filePath)
{
	NzFile file(filePath);
	if (!file.Open(NzFile::ReadOnly | NzFile::Text))
	{
		NazaraError("Failed to open file");
		return false;
	}

	unsigned int length = static_cast<unsigned int>(file.GetSize());

	NzString source(length, '\0');

	if (file.Read(&source[0], length) != length)
	{
		NazaraError("Failed to read file");
		return false;
	}

	file.Close();

	return Execute(source);
}

bool NzLuaInstance::ExecuteFromMemory(const void* data, unsigned int size)
{
	NzMemoryStream stream(data, size);
	return ExecuteFromStream(stream);
}

bool NzLuaInstance::ExecuteFromStream(NzInputStream& stream)
{
	StreamData data;
	data.stream = &stream;

	if (lua_load(m_state, StreamReader, &data, "C++", nullptr) != 0)
	{
		m_lastError = lua_tostring(m_state, -1);
		lua_pop(m_state, 1);

		return false;
	}

	return Run(0, 0);
}

int NzLuaInstance::GetAbsIndex(int index) const
{
	return lua_absindex(m_state, index);
}

nzLuaType NzLuaInstance::GetField(const char* fieldName, int index) const
{
	return FromLuaType(lua_getfield(m_state, index, fieldName));
}

nzLuaType NzLuaInstance::GetField(const NzString& fieldName, int index) const
{
	return FromLuaType(lua_getfield(m_state, index, fieldName.GetConstBuffer()));
}

nzLuaType NzLuaInstance::GetGlobal(const char* name) const
{
	return FromLuaType(lua_getglobal(m_state, name));
}

nzLuaType NzLuaInstance::GetGlobal(const NzString& name) const
{
	return FromLuaType(lua_getglobal(m_state, name.GetConstBuffer()));
}

lua_State* NzLuaInstance::GetInternalState() const
{
	return m_state;
}

NzString NzLuaInstance::GetLastError() const
{
	return m_lastError;
}

nzUInt32 NzLuaInstance::GetMemoryLimit() const
{
	return m_memoryLimit;
}

nzUInt32 NzLuaInstance::GetMemoryUsage() const
{
	return m_memoryUsage;
}

nzLuaType NzLuaInstance::GetMetatable(const char* tname) const
{
	return FromLuaType(luaL_getmetatable(m_state, tname));
}

nzLuaType NzLuaInstance::GetMetatable(const NzString& tname) const
{
	return FromLuaType(luaL_getmetatable(m_state, tname.GetConstBuffer()));
}

bool NzLuaInstance::GetMetatable(int index) const
{
	return lua_getmetatable(m_state, index) != 0;
}

unsigned int NzLuaInstance::GetStackTop() const
{
	return lua_gettop(m_state);
}

nzLuaType NzLuaInstance::GetTable(int index) const
{
	return FromLuaType(lua_gettable(m_state, index));
}

nzUInt32 NzLuaInstance::GetTimeLimit() const
{
	return m_timeLimit;
}

nzLuaType NzLuaInstance::GetType(int index) const
{
	return FromLuaType(lua_type(m_state, index));
}

const char* NzLuaInstance::GetTypeName(nzLuaType type) const
{
	#ifdef NAZARA_DEBUG
	if (type > nzLuaType_Max)
	{
		NazaraError("Lua type out of enum");
		return nullptr;
	}
	#endif

	return lua_typename(m_state, s_types[type]);
}

void NzLuaInstance::Insert(int index)
{
	lua_insert(m_state, index);
}

bool NzLuaInstance::IsOfType(int index, nzLuaType type) const
{
	switch (type)
	{
		case nzLuaType_Boolean:
			return lua_isboolean(m_state, index) != 0;

		case nzLuaType_Function:
			return lua_isfunction(m_state, index) != 0;

		case nzLuaType_LightUserdata:
			return lua_islightuserdata(m_state, index) != 0;

		case nzLuaType_Nil:
			return lua_isnil(m_state, index) != 0;

		case nzLuaType_None:
			return lua_isnone(m_state, index) != 0;

		case nzLuaType_Number:
			return lua_isnumber(m_state, index) != 0;

		case nzLuaType_String:
			return lua_isstring(m_state, index) != 0;

		case nzLuaType_Table:
			return lua_istable(m_state, index) != 0;

		case nzLuaType_Thread:
			return lua_isthread(m_state, index) != 0;

		case nzLuaType_Userdata:
			return lua_isuserdata(m_state, index) != 0;
	}

	NazaraError("Lua type not handled (0x" + NzString::Number(type, 16) + ')');
	return false;
}

bool NzLuaInstance::IsOfType(int index, const char* tname) const
{
	void* ud = luaL_testudata(m_state, index, tname);
	return ud != nullptr;
}

bool NzLuaInstance::IsOfType(int index, const NzString& tname) const
{
	return IsOfType(index, tname.GetConstBuffer());
}

bool NzLuaInstance::IsValid(int index) const
{
	return lua_isnoneornil(m_state, index) == 0;
}

unsigned int NzLuaInstance::Length(int index) const
{
	return luaL_len(m_state, index);
}

void NzLuaInstance::MoveTo(NzLuaInstance* instance, int n)
{
	lua_xmove(m_state, instance->m_state, n);
}

bool NzLuaInstance::NewMetatable(const char* str)
{
	return luaL_newmetatable(m_state, str) != 0;
}

bool NzLuaInstance::NewMetatable(const NzString& str)
{
	return luaL_newmetatable(m_state, str.GetConstBuffer()) != 0;
}

bool NzLuaInstance::Next(int index)
{
	return lua_next(m_state, index) != 0;
}

void NzLuaInstance::Pop(unsigned int n)
{
	lua_pop(m_state, static_cast<int>(n));
}

void NzLuaInstance::PushBoolean(bool value)
{
	lua_pushboolean(m_state, (value) ? 1 : 0);
}

void NzLuaInstance::PushCFunction(NzLuaCFunction func, int upvalueCount)
{
	lua_pushcclosure(m_state, func, upvalueCount);
}

void NzLuaInstance::PushFunction(NzLuaFunction func)
{
	NzLuaFunction* luaFunc = reinterpret_cast<NzLuaFunction*>(lua_newuserdata(m_state, sizeof(NzLuaFunction)));
	NzPlacementNew<NzLuaFunction>(luaFunc, std::move(func));

	lua_pushcclosure(m_state, ProxyFunc, 1);
}

void NzLuaInstance::PushInteger(long long value)
{
	lua_pushinteger(m_state, value);
}

void NzLuaInstance::PushLightUserdata(void* value)
{
	lua_pushlightuserdata(m_state, value);
}

void NzLuaInstance::PushMetatable(const char* str)
{
	luaL_getmetatable(m_state, str);
}

void NzLuaInstance::PushMetatable(const NzString& str)
{
	luaL_getmetatable(m_state, str.GetConstBuffer());
}

void NzLuaInstance::PushNil()
{
	lua_pushnil(m_state);
}

void NzLuaInstance::PushNumber(double value)
{
	lua_pushnumber(m_state, value);
}

void NzLuaInstance::PushReference(int ref)
{
	lua_rawgeti(m_state, LUA_REGISTRYINDEX, ref);
}

void NzLuaInstance::PushString(const char* str)
{
	lua_pushstring(m_state, str);
}

void NzLuaInstance::PushString(const NzString& str)
{
	lua_pushlstring(m_state, str.GetConstBuffer(), str.GetSize());
}

void NzLuaInstance::PushTable(unsigned int sequenceElementCount, unsigned int arrayElementCount)
{
	lua_createtable(m_state, sequenceElementCount, arrayElementCount);
}

void* NzLuaInstance::PushUserdata(unsigned int size)
{
	return lua_newuserdata(m_state, size);
}

void NzLuaInstance::PushValue(int index)
{
	lua_pushvalue(m_state, index);
}

void NzLuaInstance::Remove(int index)
{
	lua_remove(m_state, index);
}

void NzLuaInstance::Replace(int index)
{
	lua_replace(m_state, index);
}

void NzLuaInstance::SetField(const char* name, int index)
{
	lua_setfield(m_state, index, name);
}

void NzLuaInstance::SetField(const NzString& name, int index)
{
	lua_setfield(m_state, index, name.GetConstBuffer());
}

void NzLuaInstance::SetGlobal(const char* name)
{
	lua_setglobal(m_state, name);
}

void NzLuaInstance::SetGlobal(const NzString& name)
{
	lua_setglobal(m_state, name.GetConstBuffer());
}

void NzLuaInstance::SetMetatable(const char* tname)
{
	luaL_setmetatable(m_state, tname);
}

void NzLuaInstance::SetMetatable(const NzString& tname)
{
	luaL_setmetatable(m_state, tname.GetConstBuffer());
}

void NzLuaInstance::SetMetatable(int index)
{
	lua_setmetatable(m_state, index);
}

void NzLuaInstance::SetMemoryLimit(nzUInt32 memoryLimit)
{
	m_memoryLimit = memoryLimit;
}

void NzLuaInstance::SetTable(int index)
{
	lua_settable(m_state, index);
}

void NzLuaInstance::SetTimeLimit(nzUInt32 timeLimit)
{
	if (m_timeLimit != timeLimit)
	{
		if (m_timeLimit == 0)
			lua_sethook(m_state, TimeLimiter, LUA_MASKCOUNT, 1000);
		else if (timeLimit == 0)
			lua_sethook(m_state, TimeLimiter, 0, 1000);

		m_timeLimit = timeLimit;
	}
}

bool NzLuaInstance::ToBoolean(int index) const
{
	return lua_toboolean(m_state, index) != 0;
}

long long NzLuaInstance::ToInteger(int index, bool* succeeded) const
{
	int success;
	long long result = lua_tointegerx(m_state, index, &success);

	if (succeeded)
		*succeeded = (success != 0);

	return result;
}

double NzLuaInstance::ToNumber(int index, bool* succeeded) const
{
	int success;
	double result = lua_tonumberx(m_state, index, &success);

	if (succeeded)
		*succeeded = (success != 0);

	return result;
}

const void* NzLuaInstance::ToPointer(int index) const
{
	return lua_topointer(m_state, index);
}

const char* NzLuaInstance::ToString(int index, std::size_t* length) const
{
	return lua_tolstring(m_state, index, length);
}

void* NzLuaInstance::ToUserdata(int index) const
{
	return lua_touserdata(m_state, index);
}

void* NzLuaInstance::ToUserdata(int index, const char* tname) const
{
	return luaL_testudata(m_state, index, tname);
}

void* NzLuaInstance::ToUserdata(int index, const NzString& tname) const
{
	return luaL_testudata(m_state, index, tname.GetConstBuffer());
}

int NzLuaInstance::GetIndexOfUpValue(int upValue)
{
	return lua_upvalueindex(upValue);
}

NzLuaInstance* NzLuaInstance::GetInstance(lua_State* state)
{
	NzLuaInstance* instance;
	lua_getallocf(state, reinterpret_cast<void**>(&instance));

	return instance;
}

bool NzLuaInstance::Run(int argCount, int resultCount)
{
	if (m_level++ == 0)
		m_clock.Restart();

	int status = lua_pcall(m_state, argCount, resultCount, 0);

	m_level--;

	if (status != 0)
	{
		m_lastError = lua_tostring(m_state, -1);
		lua_pop(m_state, 1);

		return false;
	}

	return true;
}

void* NzLuaInstance::MemoryAllocator(void* ud, void* ptr, std::size_t osize, std::size_t nsize)
{
	NzLuaInstance* instance = static_cast<NzLuaInstance*>(ud);
	nzUInt32& memoryLimit = instance->m_memoryLimit;
	nzUInt32& memoryUsage = instance->m_memoryUsage;

	if (nsize == 0)
	{
		memoryUsage -= osize;
		std::free(ptr);

		return nullptr;
	}
	else
	{
		nzUInt32 usage = memoryUsage + nsize;
		if (ptr)
			usage -= osize;

		if (memoryLimit != 0 && usage > memoryLimit)
		{
			NazaraError("Lua memory usage is over memory limit (" + NzString::Number(usage) + " > " + NzString::Number(memoryLimit) + ')');
			return nullptr;
		}

		memoryUsage = usage;

		return std::realloc(ptr, nsize);
	}
}

int NzLuaInstance::ProxyFunc(lua_State* state)
{
	NzLuaFunction& func = *static_cast<NzLuaFunction*>(lua_touserdata(state, lua_upvalueindex(1)));
	return func(*GetInstance(state));
}

void NzLuaInstance::TimeLimiter(lua_State* state, lua_Debug* debug)
{
	NazaraUnused(debug);

	NzLuaInstance* instance = GetInstance(state);
	if (instance->m_clock.GetMilliseconds() > instance->m_timeLimit)
		luaL_error(state, "maximum execution time exceeded");
}
