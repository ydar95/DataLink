#pragma once
#include<Lua/lua.hpp>
#include<Lua/LuaBridge.hpp>
#include"integer.hpp"
#include"script_component.hpp"
#include"typedef.hpp"
namespace cc::lua_handler {


  template<typename _Ty> void Inject(lua_State *pLuaState) {
    static_assert( false , "this class no define its inject func" );
  }

  template<> inline void Inject<Integer>(lua_State *pLuaState) {
    luabridge::getGlobalNamespace(pLuaState)
      .beginClass<Integer>("Integer")
      .addConstructor<void( * )( std::int32_t )>()
      .addData("src" , &Integer::src , false)
      .addData("val" , &Integer::val)
      .addData("inc" , &Integer::inc)
      .endClass();
  }

  template<> inline void Inject<cc::ScriptComponent>(lua_State *pLuaState) {
    luabridge::getGlobalNamespace(pLuaState)
      .beginClass<cc::ScriptComponent>("ScriptComponent")
      .addFunction("BindOdd" , &cc::ScriptComponent::BindOdd)
      .addFunction("BindEven" , &cc::ScriptComponent::BindEven)
      .addFunction("BindPrint" , &cc::ScriptComponent::BindPrint)
      .addFunction("BindSend" , &cc::ScriptComponent::BindSend)
      .addFunction("SendFunc" , &cc::ScriptComponent::SendFunc)
      .endClass();
  }

  template<> inline void Inject<devs::TimeType>(lua_State *pLuaState) {
    luabridge::getGlobalNamespace(pLuaState)
      .beginNamespace("Const")
      .addVariable("kTimeMax" , &devs::TIME_MAX , false)
      .endNamespace();
  }
}