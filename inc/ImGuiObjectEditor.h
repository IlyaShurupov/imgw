
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

		stack<object_path> view_path;

		DictObject* root = NULL;
		Object* active = NULL;

		Object* clipboard = NULL;
		string save_path;

		ImGuiObjectEditor() {
			assert(NDO && "Objects library is not initialized");
			root = (DictObject*) NDO->create("dict");
			default_init();
		}

		ImGuiObjectEditor(string path) {
			assert(NDO && "Objects library is not initialized");
			load(path);
			if (!root) {
				root = (DictObject*) NDO->create("dict");
			}
			default_init();
			save_path = path;
		}

		void default_init() {
			active = root;
			view_path.push({active , "dict 'root'"});
		}

		void cd(Object* child, const char* name) {
			active = child;
			view_path.push({active , name});
		}

		void save(string path) {
			if (root) {
				NDO->save(root, path);
			}
		}
		void load(string path) {
			Object* obj = NDO->load(path);
			if (!obj || obj->type->name != "dict") {
				NDO->destroy(obj);
				return;
			}
			root = NDO_CAST(DictObject, obj);
		}

		void Draw();
		void oexplorer(halnf width);
		void oproperties(const ObjectType*);

		~ImGuiObjectEditor() {
			if (save_path != " ") {
				save(save_path);
			}
			NDO->destroy(root);
		}
	};
};
#endif