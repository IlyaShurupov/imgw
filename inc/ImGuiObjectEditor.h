
#pragma once

#define IMGW_OBJECTS_LIB

#ifdef IMGW_OBJECTS_LIB

#include "ImGuiClass.h"

#include "object/object.h"
#include "primitives/primitives.h"

#include "stack.h"
#include "strings.h"

namespace ImGui {
	struct object_path {
		object_path(Object* obj, string id) : obj(obj), id(id) {}
		object_path() {
			obj = NULL;
		}

		operator bool() { return obj; }
		Object* obj;
		string id;
	};


	struct ImGuiObjectEditor {

		objects_api* oh;
		DictObject* root;

		Object* active;
		stack<object_path> path;
		Object* clipboard = NULL;

		void oedior_init(DictObject* root, objects_api* oh, ogl::window* window);

		void Draw();

		void oexplorer();
		void oproperties(const ObjectType*);
	};
};
#endif