#include <utility>

#include "TableInterface.h"

TableInterface::TableInterface(lua_State *state, int arg) : state(state), arg(arg), offset(lua_gettop(state)) {
    if (offset < arg) {
        throw LuaError("expected table as argument " + std::to_string(arg) + ", got nil");
    }

    int type = lua_type(state, -offset + arg - 1);
    if (type != LUA_TTABLE) {
        throw LuaError("expected table as argument " + std::to_string(arg) + ", got " + lua_typename(state, type),
                       LuaError::Type::NIL_ARG);
    }
}

void TableInterface::throwError(std::string desc) {
    if (desc.empty()) {
        throw LuaError("bad element '" + lastKey + "' of argument " + std::to_string(arg));
    } else {
        throw LuaError("bad element '" + lastKey + "' of argument " + std::to_string(arg) + " (" + desc + ")");
    }
}

void TableInterface::popToStack(std::string key) {
    lua_pushstring(state, key.c_str());
    lua_gettable(state, -(++offset));
    lastKey = key;
}

double TableInterface::getNumber(std::string key) {
    popToStack(key);

    double value;
    if (lua_isnil(state, -1)) {
        throw LuaError("expected number for element '" + key + "' of argument " + std::to_string(arg) + ", got nil",
                       LuaError::Type::NIL_ARG);
    } else {
        int type = lua_type(state, -1);

        if (type == LUA_TNUMBER) {
            value = lua_tonumber(state, -1);
        } else {
            throw LuaError("expected number for element '" + key + "' of argument " + std::to_string(arg) + ", got " +
                           lua_typename(state, type), LuaError::Type::BAD_TYPE);
        }
    }

    return value;
}

double TableInterface::getNumber(std::string key, double defaultValue) {
    try {
        return getNumber(std::move(key));
    } catch (const LuaError &e) {
        if (e.getErrorType() == LuaError::Type::NIL_ARG)
            return defaultValue;
        else
            throw e;
    }
}

int TableInterface::getInteger(std::string key) {
    popToStack(key);

    int value;
    if (lua_isnil(state, -1)) {
        throw LuaError("expected number for element '" + key + "' of argument " + std::to_string(arg) + ", got nil",
                       LuaError::Type::NIL_ARG);
    } else {
        int type = lua_type(state, -1);

        if (type == LUA_TNUMBER) {
            value = static_cast<int>(lua_tointeger(state, -1));
        } else {
            throw LuaError("expected number for element '" + key + "' of argument " + std::to_string(arg) + ", got " +
                           lua_typename(state, type), LuaError::Type::BAD_TYPE);
        }
    }

    return value;
}

int TableInterface::getInteger(std::string key, int defaultValue) {
    try {
        return getInteger(std::move(key));
    } catch (const LuaError &e) {
        if (e.getErrorType() == LuaError::Type::NIL_ARG)
            return defaultValue;
        else
            throw e;
    }
}
