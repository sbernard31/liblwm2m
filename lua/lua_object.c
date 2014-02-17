/*
MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "core/liblwm2m.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua5.1/lua.h"
#include "lua5.1/lauxlib.h"
#include "lua5.1/lualib.h"

typedef struct luaobject_userdata {
	lua_State * L;
	int tableref;
} luaobject_userdata;

static uint8_t prv_read(lwm2m_uri_t * uriP, char ** buffer, int * length,
		lwm2m_object_t * objectP) {
	// Default value for output parameters.
	*buffer = NULL;
	*length = 0;

	// Get user data.
	luaobject_userdata * userdata = (luaobject_userdata*) objectP->userData;
	lua_State * L = userdata->L;

	// Get table of this object on the stack.
	lua_rawgeti(L, LUA_REGISTRYINDEX, userdata->tableref); // stack: ..., objtable

	if (LWM2M_URI_IS_SET_RESOURCE(uriP)) {
		int instanceId;
		if (!LWM2M_URI_IS_SET_INSTANCE(uriP))
			instanceId = 0; // default value

		// Get resource.
		lua_pushinteger(L, uriP->resourceId); // stack: ..., objtable, resourceindex
		lua_gettable(L, -2); // stack: ..., objtable, resourcevalue

		// If this is a function call it.
		if (lua_isfunction(L, -1)) {
			lua_call(L, 0, 1); // stack: ..., objtable, functionresult
		}

		// Manager other cases.
		if (lua_isnil(L, -1)) {
			return COAP_404_NOT_FOUND ;
		} else if (lua_isstring(L, -1)) {
			*buffer = strdup(lua_tostring(L, -1));
			if (NULL != *buffer) {
				*length = strlen(*buffer);
				return COAP_205_CONTENT ;
			}
		}
		return COAP_501_NOT_IMPLEMENTED ;
	} else {
		// TODO: TLV or JSON not manage.
		return COAP_501_NOT_IMPLEMENTED ;
	}
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_execute(lwm2m_uri_t * uriP, char * buffer, int length,
		lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_write(lwm2m_uri_t * uriP, char * buffer, int length,
		lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_create(lwm2m_uri_t * uriP, char * buffer, int length,
		lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_delete(uint16_t id, lwm2m_object_t * objectP) {
	return COAP_501_NOT_IMPLEMENTED ;
}

static uint8_t prv_close(lwm2m_object_t * objectP) {

	luaobject_userdata * userdata = (luaobject_userdata *) objectP->userData;
	if (userdata != NULL) {
		// Release table reference in lua registry.
		if (userdata->tableref != LUA_NOREF) {
			luaL_unref(userdata->L, LUA_REGISTRYINDEX, userdata->tableref);
			userdata->tableref = LUA_NOREF;
		}

		// Release memory.
		free(userdata);
		objectP->userData = NULL;
	}
	return 1;
}

lwm2m_object_t * get_lua_object(lua_State *L, int tableindex, int objId) {

	// Allocate memory for lwm2m object.
	lwm2m_object_t * objectP = (lwm2m_object_t *) malloc(
			sizeof(lwm2m_object_t));

	if (NULL != objectP) {
		memset(objectP, 0, sizeof(lwm2m_object_t));

		// Allocate memory for userdata.
		luaobject_userdata * userdata = (luaobject_userdata *) malloc(
				sizeof(luaobject_userdata));
		if (userdata == NULL) {
			free(objectP);
			return NULL;
		}

		userdata->L = L;
		lua_pushvalue(L, tableindex); // stack: ..., table
		userdata->tableref = luaL_ref(L, LUA_REGISTRYINDEX); //stack: ...

		objectP->objID = objId;
		objectP->readFunc = prv_read;
		objectP->writeFunc = prv_write;
		objectP->executeFunc = prv_execute;
		objectP->createFunc = prv_create;
		objectP->deleteFunc = prv_delete;
		objectP->closeFunc = prv_close;
		objectP->userData = userdata;
	}

	return objectP;
}
