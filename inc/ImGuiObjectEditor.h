
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
		object_path(obj::Object* obj, tp::string id) : obj(obj), id(id) {}
		object_path() {
			obj = NULL;
		}

		operator bool() { return obj; }
		obj::Object* obj;
		tp::string id;
	};


	struct ImGuiObjectEditor {

		tp::Stack<object_path> view_path;

		obj::DictObject* root = NULL;
		obj::Object* active = NULL;

		obj::Object* clipboard = NULL;
		tp::string save_path;

		ImGuiObjectEditor() {
			assert(obj::NDO && "Objects library is not initialized");
			root = (obj::DictObject*) obj::NDO->create("dict");
			default_init();
		}

		ImGuiObjectEditor(tp::string path) {
			assert(obj::NDO && "Objects library is not initialized");
			load(path);
			if (!root) {
				root = (obj::DictObject*) obj::NDO->create("dict");
			}
			default_init();
			save_path = path;
		}

		void default_init() {
			active = root;
			view_path.push({active , "dict 'root'"});
		}

		void cd(obj::Object* child, const char* name) {
			active = child;
			view_path.push({active , name});
		}

		void save(tp::string path) {
			if (root) {
				obj::NDO->save(root, path);
			}
		}
		void load(tp::string path) {
			obj::Object* obj = obj::NDO->load(path);
			if (!obj || obj->type->name != "dict") {
				obj::NDO->destroy(obj);
				return;
			}
			root = NDO_CAST(obj::DictObject, obj);
		}

		void Draw();
		void oexplorer(tp::halnf width);
		void oproperties(const obj::ObjectType*);

		~ImGuiObjectEditor() {
			if (save_path != " ") {
				save(save_path);
			}
			obj::NDO->destroy(root);
		}
	};
};
#endif