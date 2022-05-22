
#include "ImGuiObjectEditor.h"

#ifdef IMGW_OBJECTS_LIB

#include "imgui_internal.h"

using namespace ImGui;

object_path nullo_view(obj::Object* in, obj::Object*& clipboard, obj::objects_api* oh) {
	ImGui::Text("Null Object.");
	return object_path();
}

object_path linko_view(obj::LinkObject* in, obj::Object*& clipboard, obj::objects_api* oh) {

	if (in->link) {

		ImGui::Text("%s at %x", in->link->type->name.cstr(), in->link);

		if (ImGui::Selectable("View")) {
			return {in->link, "adress"};
		}
	} else {
		ImGui::Text("Link Is Null");
	}

	if (ImGui::Selectable("Set from clipboard")) {
		in->link = clipboard;
	}

	if (ImGui::Selectable("Copy value")) {
		clipboard = in->link;
	}
	if (ImGui::Selectable("Set Null")) {
		in->link = NULL;
	}

	return object_path();
}

object_path into_view(obj::IntObject* in, obj::Object*& clipboard, obj::objects_api* oh) {
	ImGui::Text("Int Value: "); ImGui::SameLine();
	int val = (int) in->val;
	ImGui::InputInt(" ", &val);
	in->val = (tp::alni) val;
	return object_path();
}

object_path dicto_view(obj::DictObject* active, obj::Object*& clipboard, obj::objects_api* oh) {

	bool popup = false;
	object_path out;

	if (active->items.size()) {
		ImGui::Text("Dictinary Items: ");
	} else {
		ImGui::Text("Dictinary Is Empty. ");
	}

	ImGui::BeginChild("child_2", {ImGui::GetWindowContentRegionWidth(), 150}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
	for (auto childo : active->items) {
		ImGui::PushID((int) childo.entry_idx);

		if (ImGui::Selectable(childo.iter->key.cstr())) {
			out = {childo.iter->val,  childo.iter->val->type->name + " '" + childo.iter->key + "'"};
			ImGui::PopID();
			break;
		}

		if (ImGui::BeginPopupContextItem(childo.iter->key.cstr(), ImGuiPopupFlags_MouseButtonRight)) {
			popup = true;

			ImGui::Text("%s at %x", childo->val->type->name.cstr(), childo->val);

			static obj::Object* name_parent = NULL;
			static char name[100] = {"asdas"};

			if (name_parent != childo) {
				tp::memcp(name, childo->key.cstr(), childo->key.size() + 1);
				name_parent = childo;
			}

			if (ImGui::InputTextEx(" ", "new name", name, 100, {140 , 30}, ImGuiInputTextFlags_EnterReturnsTrue)) {
				tp::alni idx = active->items.presents(name);
				if (idx != -1) {
					Notify("Object with such name Already Exists");
				} else {
					active->items.remove(childo->key);
					active->items.put(tp::string(name).capture(), name_parent);
					name_parent = NULL;
					ImGui::EndPopup();
					ImGui::PopID();
					break;
				}
			}

			if (ImGui::Selectable("Exclude")) {
				active->items.remove(childo->key);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Destroy")) {
				oh->destroy(childo->val);
				active->items.remove(childo->key);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Copy Link")) {
				clipboard = childo.iter->val;
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			name_parent = childo;
			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
	ImGui::EndChild();

	if (!popup && ImGui::BeginPopupContextItem("child_2", ImGuiPopupFlags_MouseButtonRight)) {

		if (ImGui::Selectable("Paste")) {

			tp::alni idx = 1;
			tp::string name_base = tp::string("clipboard ") + clipboard->type->name + tp::string(" ");
			tp::string name_out = name_base;

			while (active->items.presents(name_out)) {
				name_out = name_base + tp::string(idx);
				idx++;
			}

			active->items.put(name_out, clipboard);
		}

		if (ImGui::BeginMenu("Create")) {

			ImGui::Text("Types: ");

			ImGui::BeginChild("child_4", {100, 140}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			for (auto childo : oh->types) {
				ImGui::PushID((int) childo.entry_idx);

				if (ImGui::Selectable(childo.iter->key.cstr())) {
					obj::Object* newo = oh->create(childo->key);

					tp::alni idx = 1;
					tp::string name_base = tp::string("new ") + newo->type->name + tp::string(" ");
					tp::string name_out = name_base;

					while (active->items.presents(name_out)) {
						name_out = name_base + tp::string(idx);
						idx++;
					}

					active->items.put(name_out, newo);

					ImGui::PopID();
					break;
				}

				ImGui::PopID();
			}

			ImGui::EndChild();

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return out;
}

object_path listo_view(obj::ListObject* active, obj::Object*& clipboard, obj::objects_api* oh) {
	bool popup = false;
	object_path out;

	if (active->items.length()) {
		ImGui::Text("List Items: ");
	} else {
		ImGui::Text("List Is Empty. ");
	}

	ImGui::BeginChild("child_2", {0, 0}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
	for (auto childo : active->items) {
		ImGui::PushID((int) childo.Idx());

		if (ImGui::Selectable(childo->type->name.cstr())) {
			out = object_path(childo.Data(), childo->type->name + " at " + tp::string(childo.Idx()));
			ImGui::PopID();
			break;
		}

		if (ImGui::BeginPopupContextItem(childo->type->name.cstr(), ImGuiPopupFlags_MouseButtonRight)) {
			popup = true;

			ImGui::Text("%s at %x", childo->type->name.cstr(), childo.Data());

			if (ImGui::Selectable("Exclude")) {
				active->items.delNode(childo.node());
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Copy Link")) {
				clipboard = childo.Data();
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (childo.node()->prev && ImGui::Selectable("Move Up")) {
				SWAP(childo.node()->prev->data, childo.Data(), obj::Object*);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (childo.node()->next && ImGui::Selectable("Move Down")) {
				SWAP(childo.node()->next->data, childo.Data(), obj::Object*);
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			if (ImGui::Selectable("Destroy")) {
				oh->destroy(childo.Data());
				active->items.delNode(childo.node());
				ImGui::EndPopup();
				ImGui::PopID();
				break;
			}

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}
	ImGui::EndChild();

	if (!popup && ImGui::BeginPopupContextItem("child_2", ImGuiPopupFlags_MouseButtonRight)) {

		if (ImGui::Selectable("Paste")) {
			active->items.pushBack(clipboard);
		}

		if (ImGui::BeginMenu("Create")) {

			ImGui::Text("Types: ");

			ImGui::BeginChild("child_4", {100, 140}, true, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);

			for (auto childo : oh->types) {
				ImGui::PushID((int) childo.entry_idx);

				if (ImGui::Selectable(childo.iter->key.cstr())) {
					obj::Object* newo = oh->create(childo->key);
					active->items.pushBack(newo);
					ImGui::PopID();
					break;
				}

				ImGui::PopID();
			}

			ImGui::EndChild();

			ImGui::EndMenu();
		}

		ImGui::EndPopup();
	}

	return out;
}

object_path stringo_view(obj::StringObject* in, obj::Object*& clipboard, obj::objects_api* oh) {

	ImGui::Text("String Data: ");

	static char val[2048] = {" "};

	if (in->val != val) {
		tp::memcp(val, in->val.cstr(), in->val.size() + 1);
	}

	ImGui::InputTextMultiline(" ", val, 2048, {ImGui::GetWindowContentRegionWidth() - 25, ImGui::GetWindowContentRegionWidth() * 1.1f});

	if (in->val != val) {
		in->val = tp::string(val).capture();
	}

	return object_path();
}

object_path floato_view(obj::Object* in, obj::Object*& clipboard, obj::objects_api* oh) {
	return object_path();
}

void ImGuiObjectEditor::oexplorer(tp::halnf width) {

	//ImGui::Text("View Path: "); ImGui::SameLine();
	ImGui::BeginChild("child_path", {width, 45}, false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar);
	tp::Stack<object_path*> rev_path;
	for (auto childo = view_path.last; childo; childo = childo->prev) {
		rev_path.push(&childo->data);
	}
	tp::alni idx = 0;
	for (auto childo = rev_path.last; childo; childo = childo->prev) {
		ImGui::PushID((int) idx);
		bool go_back = false;
		if (childo == rev_path.last) {
			go_back = ImGui::Button(childo->data->id.cstr()); ImGui::SameLine();
		} else {
			go_back = ImGui::Button((childo->data->id).cstr()); ImGui::SameLine();
		}
		ImGui::PopID();

		if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonRight)) {

			obj::Object* curretn_object = childo->data->obj;
			ImGui::Text("%s at %x", curretn_object->type->name.cstr(), curretn_object);
			ImGui::Separator();

			if (ImGui::Selectable("Copy Link")) {
				clipboard = curretn_object;
			}

			if (ImGui::Selectable("Instantiate  ")) {
				clipboard = obj::NDO->create(curretn_object->type->name.cstr());
				obj::NDO->copy(clipboard, curretn_object);
				Notify("Object copied to clipboard");
			}

			ImGui::Separator();

			static char path_str[100] = {"data.o\0"};
			ImGui::InputTextEx(" ", "save path", path_str, 100, {100, 30}, 0);

			bool save_object = ImGui::Selectable("Save Object");
			bool load_object = ImGui::Selectable("Load Object");

			if (save_object) {
				obj::NDO->save(curretn_object, path_str);
				Notify("Object saved");
			}

			if (load_object) {
				obj::Object* loadedo = obj::NDO->load(path_str);
				if (loadedo) {
					clipboard = loadedo;
					Notify("Object copied to clipboard");
				} else {
					Notify("Can't load Object");
				}
			}

			ImGui::EndPopup();
		}

		if (go_back) {
			while (&view_path.last->data != childo->data) {
				view_path.pop();
			}
			active = childo->data->obj;
			ImGui::EndChild();
			return;
		}
		idx++;
	}
	ImGui::Text(" ");
	ImGui::EndChild();

	ImGui::Separator();
	object_path new_active;
	if (active->type->name == "null") {
		new_active = nullo_view(active, clipboard, obj::NDO);
	} else if (active->type->name == "link") {
		new_active = linko_view((obj::LinkObject*) active, clipboard, obj::NDO);
	} else if (active->type->name == "int") {
		new_active = into_view((obj::IntObject*) active, clipboard, obj::NDO);
	} else if (active->type->name == "float") {
		new_active = floato_view(active, clipboard, obj::NDO);
	} else if (active->type->name == "str") {
		new_active = stringo_view((obj::StringObject*) active, clipboard, obj::NDO);
	} else if (active->type->name == "list") {
		new_active = listo_view((obj::ListObject*) active, clipboard, obj::NDO);
	} else if (active->type->name == "dict") {
		new_active = dicto_view((obj::DictObject*) active, clipboard, obj::NDO);
	} else {
		ImGui::Text("Preview is Unavaliable");
	}

	if (new_active != NULL) {
		active = new_active.obj;
		view_path.push(new_active);
	}
}

void ImGuiObjectEditor::oproperties(const obj::ObjectType* type) {
	ImGui::Text("Type: %s", type->name.cstr());
	if (type->base) {
		ImGui::Text("Inherits From"); ImGui::SameLine();
		if (ImGui::Selectable(type->base->name.cstr())) {
			ImGui::PushID(type->base->name.cstr());
			oproperties(type->base);
			ImGui::PopID();
		}
	}
	ImGui::Text("Struct Size: %i bytes", type->size);
	if (type->save_size) ImGui::Text("Save Size: %i bytes", type->save_size(active));

	if (type->methods) {
		ImGui::BeginChild("methods", {ImGui::GetWindowContentRegionWidth(), 350}, false, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::Text("type Methods: ");

		for (tp::alni idx = 0; type->methods[idx].adress; idx++) {
			obj::type_method* method = &type->methods[idx];
			ImGui::PushID(method->name.cstr());
			ImGui::Text("%s", method->name.cstr());
			ImGui::PopID();
		}
		ImGui::EndChild();
	}
}

void ImGuiObjectEditor::Draw() {

	if (WindowEditor("Explorer")) {
		oexplorer(ImGui::GetWindowContentRegionWidth());
	}
	ImGui::End();

	if (WindowEditor("Object Info")) {
		oproperties(active->type);
	}
	ImGui::End();
}

#endif